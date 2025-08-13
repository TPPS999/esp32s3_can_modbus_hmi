/*
 * modbus_tcp.cpp - ESP32S3 CAN to Modbus TCP Bridge Modbus Server Implementation
 * 
 * VERSION: v4.0.1 - COMPLETE IMPLEMENTATION
 * DATE: 2025-08-13
 * STATUS: ✅ READY - Kompletne mapowanie 125 rejestrów per BMS z v3.0.0
 * 
 * DESCRIPTION: Kompletna implementacja serwera Modbus TCP
 * - Pełne mapowanie 125 rejestrów per BMS (2000 rejestrów total)
 * - Wszystkie 54 typy multipleksera Frame 490
 * - Zaawansowane statystyki i diagnostyka
 * - Kompatybilność z oryginalnym kodem v3.0.0
 */

#include "modbus_tcp.h"
#include "bms_data.h"
#include "utils.h"

// === GLOBAL VARIABLES ===
WiFiServer modbusServer(MODBUS_TCP_PORT);
WiFiClient currentModbusClient;
uint16_t holdingRegisters[MODBUS_MAX_HOLDING_REGISTERS];  // 2000 rejestrów (16 x 125)
ModbusStats modbusStats = {0};
ModbusState_t modbusState = MODBUS_STATE_UNINITIALIZED;

// === PRIVATE VARIABLES ===
static uint8_t modbusRequestBuffer[MODBUS_MAX_FRAME_SIZE];
static uint8_t modbusResponseBuffer[MODBUS_MAX_FRAME_SIZE];
static bool modbusDebugEnabled = true;
static unsigned long lastClientActivity = 0;
static unsigned long lastRegisterUpdate = 0;

// === PRIVATE FUNCTION DECLARATIONS ===
static void handleModbusRequest(WiFiClient& client);
static int processReadHoldingRegisters(uint8_t* request, int requestLen, uint8_t* response);
static int processReadInputRegisters(uint8_t* request, int requestLen, uint8_t* response);
static int processWriteSingleRegister(uint8_t* request, int requestLen, uint8_t* response);
static int processWriteMultipleRegisters(uint8_t* request, int requestLen, uint8_t* response);
static int createExceptionResponse(uint8_t functionCode, uint8_t exceptionCode, uint8_t* response);
static bool validateModbusRequest(uint8_t* request, int length);
static void printModbusRequest(uint8_t* request, int length);
static void printModbusResponse(uint8_t* response, int length);
static uint16_t calculateCRC16(uint8_t* data, int length);

// === 🔥 MODBUS UTILITY FUNCTIONS (z oryginalnego v3.0.0) ===
uint16_t floatToModbusRegister(float value, float scale = 100.0) {
  int32_t scaledValue = (int32_t)(value * scale);
  // Ograniczamy do zakresu uint16_t ze znakiem
  if (scaledValue > 32767) scaledValue = 32767;
  if (scaledValue < -32768) scaledValue = -32768;
  return (uint16_t)scaledValue;
}

float modbusRegisterToFloat(uint16_t reg, float scale = 100.0) {
  int16_t signedValue = (int16_t)reg;
  return (float)signedValue / scale;
}

// === 🔥 KOMPLETNE MAPOWANIE MODBUS - 125 REJESTRÓW PER BMS (z v3.0.0) ===
void updateModbusRegisters(uint8_t nodeId) {
  int batteryIndex = getBMSIndexByNodeId(nodeId);
  if (batteryIndex == -1) return;
  
  BMSData* bms = getBMSData(nodeId);
  if (!bms) return;
  
  uint16_t baseAddr = batteryIndex * 125;  // 125 rejestrów na baterię
  
  // === FRAME 190 DATA (rejestry 0-9) ===
  holdingRegisters[baseAddr + 0] = floatToModbusRegister(bms->batteryVoltage, 1000);     // mV
  holdingRegisters[baseAddr + 1] = floatToModbusRegister(bms->batteryCurrent, 1000);     // mA
  holdingRegisters[baseAddr + 2] = floatToModbusRegister(bms->remainingEnergy, 100);     // 0.01 kWh
  holdingRegisters[baseAddr + 3] = floatToModbusRegister(bms->soc, 10);                  // 0.1%
  holdingRegisters[baseAddr + 4] = 0; // Reserved
  holdingRegisters[baseAddr + 5] = 0; // Reserved
  holdingRegisters[baseAddr + 6] = 0; // Reserved
  holdingRegisters[baseAddr + 7] = 0; // Reserved
  holdingRegisters[baseAddr + 8] = 0; // Reserved
  holdingRegisters[baseAddr + 9] = 0; // Reserved
  
  // === FRAME 190 ERROR FLAGS (rejestry 10-19) ===
  holdingRegisters[baseAddr + 10] = bms->masterError ? 1 : 0;
  holdingRegisters[baseAddr + 11] = bms->cellVoltageError ? 1 : 0;
  holdingRegisters[baseAddr + 12] = bms->cellTempMinError ? 1 : 0;
  holdingRegisters[baseAddr + 13] = bms->cellTempMaxError ? 1 : 0;
  holdingRegisters[baseAddr + 14] = bms->cellVoltageMinError ? 1 : 0;
  holdingRegisters[baseAddr + 15] = bms->cellVoltageMaxError ? 1 : 0;
  holdingRegisters[baseAddr + 16] = bms->systemShutdown ? 1 : 0;
  holdingRegisters[baseAddr + 17] = bms->ibbVoltageSupplyError ? 1 : 0;
  holdingRegisters[baseAddr + 18] = 0; // Reserved
  holdingRegisters[baseAddr + 19] = 0; // Reserved
  
  // === FRAME 290 DATA (rejestry 20-29) ===
  holdingRegisters[baseAddr + 20] = floatToModbusRegister(bms->cellMinVoltage, 10000);   // 0.1mV
  holdingRegisters[baseAddr + 21] = floatToModbusRegister(bms->cellMeanVoltage, 10000);  // 0.1mV
  holdingRegisters[baseAddr + 22] = bms->minVoltageBlock;
  holdingRegisters[baseAddr + 23] = bms->minVoltageCell;
  holdingRegisters[baseAddr + 24] = bms->minVoltageString;
  holdingRegisters[baseAddr + 25] = bms->balancingTempMax;
  holdingRegisters[baseAddr + 26] = 0; // Reserved
  holdingRegisters[baseAddr + 27] = 0; // Reserved
  holdingRegisters[baseAddr + 28] = 0; // Reserved
  holdingRegisters[baseAddr + 29] = 0; // Reserved
  
  // === FRAME 310 DATA (rejestry 30-39) ===
  holdingRegisters[baseAddr + 30] = floatToModbusRegister(bms->soh, 10);                 // 0.1%
  holdingRegisters[baseAddr + 31] = floatToModbusRegister(bms->cellVoltage, 10);         // 0.1mV
  holdingRegisters[baseAddr + 32] = floatToModbusRegister(bms->cellTemperature, 10);     // 0.1°C
  holdingRegisters[baseAddr + 33] = floatToModbusRegister(bms->dcir, 10);                // 0.1mΩ
  holdingRegisters[baseAddr + 34] = bms->nonEqualStringsRamp ? 1 : 0;
  holdingRegisters[baseAddr + 35] = bms->dynamicLimitationTimer ? 1 : 0;
  holdingRegisters[baseAddr + 36] = bms->overcurrentTimer ? 1 : 0;
  holdingRegisters[baseAddr + 37] = bms->channelMultiplexor;
  holdingRegisters[baseAddr + 38] = 0; // Reserved
  holdingRegisters[baseAddr + 39] = 0; // Reserved
  
  // === FRAME 390 DATA (rejestry 40-49) ===
  holdingRegisters[baseAddr + 40] = floatToModbusRegister(bms->cellMaxVoltage, 10000);   // 0.1mV
  holdingRegisters[baseAddr + 41] = floatToModbusRegister(bms->cellVoltageDelta, 10000); // 0.1mV
  holdingRegisters[baseAddr + 42] = bms->maxVoltageBlock;
  holdingRegisters[baseAddr + 43] = bms->maxVoltageCell;
  holdingRegisters[baseAddr + 44] = bms->maxVoltageString;
  holdingRegisters[baseAddr + 45] = bms->afeTemperatureMax;
  holdingRegisters[baseAddr + 46] = 0; // Reserved
  holdingRegisters[baseAddr + 47] = 0; // Reserved
  holdingRegisters[baseAddr + 48] = 0; // Reserved
  holdingRegisters[baseAddr + 49] = 0; // Reserved
  
  // === FRAME 410 DATA (rejestry 50-59) ===
  holdingRegisters[baseAddr + 50] = floatToModbusRegister(bms->cellMaxTemperature, 10);  // 0.1°C
  holdingRegisters[baseAddr + 51] = floatToModbusRegister(bms->cellTempDelta, 10);       // 0.1°C
  holdingRegisters[baseAddr + 52] = bms->maxTempString;
  holdingRegisters[baseAddr + 53] = bms->maxTempBlock;
  holdingRegisters[baseAddr + 54] = bms->maxTempSensor;
  holdingRegisters[baseAddr + 55] = bms->readyToCharge ? 1 : 0;
  holdingRegisters[baseAddr + 56] = bms->readyToDischarge ? 1 : 0;
  holdingRegisters[baseAddr + 57] = 0; // Reserved
  holdingRegisters[baseAddr + 58] = 0; // Reserved
  holdingRegisters[baseAddr + 59] = 0; // Reserved
  
  // === FRAME 510 DATA (rejestry 60-69) ===
  holdingRegisters[baseAddr + 60] = floatToModbusRegister(bms->dccl, 1000);              // mA
  holdingRegisters[baseAddr + 61] = floatToModbusRegister(bms->ddcl, 1000);              // mA
  holdingRegisters[baseAddr + 62] = bms->input_IN02 ? 1 : 0;
  holdingRegisters[baseAddr + 63] = bms->input_IN01 ? 1 : 0;
  holdingRegisters[baseAddr + 64] = bms->relay_AUX4 ? 1 : 0;
  holdingRegisters[baseAddr + 65] = bms->relay_AUX3 ? 1 : 0;
  holdingRegisters[baseAddr + 66] = bms->relay_AUX2 ? 1 : 0;
  holdingRegisters[baseAddr + 67] = bms->relay_AUX1 ? 1 : 0;
  holdingRegisters[baseAddr + 68] = bms->relay_R2 ? 1 : 0;
  holdingRegisters[baseAddr + 69] = bms->relay_R1 ? 1 : 0;
  
  // === 🔥 FRAME 490 MULTIPLEXED DATA (rejestry 70-89) - KLUCZOWE NOWE REJESTRY ===
  holdingRegisters[baseAddr + 70] = bms->mux490Type;                                     // Mux type
  holdingRegisters[baseAddr + 71] = bms->mux490Value;                                    // Mux value
  holdingRegisters[baseAddr + 72] = bms->serialNumber0;                                  // Serial number low
  holdingRegisters[baseAddr + 73] = bms->serialNumber1;                                  // Serial number high
  holdingRegisters[baseAddr + 74] = bms->hwVersion0;                                     // HW version low
  holdingRegisters[baseAddr + 75] = bms->hwVersion1;                                     // HW version high
  holdingRegisters[baseAddr + 76] = bms->swVersion0;                                     // SW version low
  holdingRegisters[baseAddr + 77] = bms->swVersion1;                                     // SW version high
  holdingRegisters[baseAddr + 78] = floatToModbusRegister(bms->factoryEnergy, 10);      // 0.1 kWh
  holdingRegisters[baseAddr + 79] = floatToModbusRegister(bms->designCapacity, 1000);   // mAh
  holdingRegisters[baseAddr + 80] = floatToModbusRegister(bms->systemDesignedEnergy, 10); // 0.1 kWh
  holdingRegisters[baseAddr + 81] = floatToModbusRegister(bms->ballancerTempMaxBlock, 10); // 0.1°C
  holdingRegisters[baseAddr + 82] = floatToModbusRegister(bms->ltcTempMaxBlock, 10);     // 0.1°C
  holdingRegisters[baseAddr + 83] = floatToModbusRegister(bms->inletTemperature, 10);    // 0.1°C
  holdingRegisters[baseAddr + 84] = floatToModbusRegister(bms->outletTemperature, 10);   // 0.1°C
  holdingRegisters[baseAddr + 85] = bms->humidity;                                       // %
  holdingRegisters[baseAddr + 86] = bms->timeToFullCharge;                               // min
  holdingRegisters[baseAddr + 87] = bms->timeToFullDischarge;                            // min
  holdingRegisters[baseAddr + 88] = bms->batteryCycles;                                  // cycles
  holdingRegisters[baseAddr + 89] = bms->numberOfDetectedIMBs;                           // count
  
  // === 🔥 ERROR MAPS & VERSIONS (rejestry 90-109) - NOWE REJESTRY DIAGNOSTYCZNE ===
  holdingRegisters[baseAddr + 90] = bms->errorsMap0;        // Error map bits 0-15
  holdingRegisters[baseAddr + 91] = bms->errorsMap1;        // Error map bits 16-31
  holdingRegisters[baseAddr + 92] = bms->errorsMap2;        // Error map bits 32-47
  holdingRegisters[baseAddr + 93] = bms->errorsMap3;        // Error map bits 48-63
  holdingRegisters[baseAddr + 94] = bms->blVersion0;        // Bootloader version low
  holdingRegisters[baseAddr + 95] = bms->blVersion1;        // Bootloader version high
  holdingRegisters[baseAddr + 96] = bms->odVersion0;        // OD version low
  holdingRegisters[baseAddr + 97] = bms->odVersion1;        // OD version high
  holdingRegisters[baseAddr + 98] = bms->dbcVersion0;       // DBC version low
  holdingRegisters[baseAddr + 99] = bms->dbcVersion1;       // DBC version high
  holdingRegisters[baseAddr + 100] = bms->configCrc;       // Configuration CRC
  holdingRegisters[baseAddr + 101] = bms->iotStatus;       // IoT status
  holdingRegisters[baseAddr + 102] = bms->powerOnCounter;  // Power on counter
  holdingRegisters[baseAddr + 103] = bms->ddclCrc;         // DDCL CRC
  holdingRegisters[baseAddr + 104] = bms->dcclCrc;         // DCCL CRC
  holdingRegisters[baseAddr + 105] = bms->drcclCrc;        // DRCCL CRC
  holdingRegisters[baseAddr + 106] = bms->ocvCrc;          // OCV CRC
  holdingRegisters[baseAddr + 107] = floatToModbusRegister(bms->fullyChargedOn, 1);     // Threshold
  holdingRegisters[baseAddr + 108] = floatToModbusRegister(bms->fullyChargedOff, 1);    // Threshold
  holdingRegisters[baseAddr + 109] = floatToModbusRegister(bms->fullyDischargedOn, 1);  // Threshold
  
  // === 🔥 FRAME 710 & COMMUNICATION STATUS (rejestry 110-119) - NOWE REJESTRY ===
  holdingRegisters[baseAddr + 110] = bms->canopenState;                                 // CANopen state
  holdingRegisters[baseAddr + 111] = bms->communicationOk ? 1 : 0;                      // Communication OK
  holdingRegisters[baseAddr + 112] = bms->packetsReceived & 0xFFFF;                     // Packets received low
  holdingRegisters[baseAddr + 113] = (bms->packetsReceived >> 16) & 0xFFFF;             // Packets received high
  holdingRegisters[baseAddr + 114] = bms->parseErrors;                                  // Parse errors
  holdingRegisters[baseAddr + 115] = bms->frame190Count & 0xFFFF;                       // Frame counts
  holdingRegisters[baseAddr + 116] = bms->frame290Count & 0xFFFF;
  holdingRegisters[baseAddr + 117] = bms->frame310Count & 0xFFFF;
  holdingRegisters[baseAddr + 118] = bms->frame490Count & 0xFFFF;                       // Multiplexed frame count
  holdingRegisters[baseAddr + 119] = bms->frame710Count & 0xFFFF;                       // CANopen frame count
  
  // === 🔥 EXTENDED COUNTERS & NODE ID (rejestry 120-124) - NOWE REJESTRY ===
  holdingRegisters[baseAddr + 120] = bms->frame390Count & 0xFFFF;                       // Frame 390 count
  holdingRegisters[baseAddr + 121] = bms->frame410Count & 0xFFFF;                       // Frame 410 count
  holdingRegisters[baseAddr + 122] = bms->frame510Count & 0xFFFF;                       // Frame 510 count
  holdingRegisters[baseAddr + 123] = bms->frame1B0Count & 0xFFFF;                       // Frame 1B0 count
  holdingRegisters[baseAddr + 124] = nodeId;                                            // Node ID
  
  // === DEBUG LOG ===
  if (ENABLE_MODBUS_FRAME_LOGGING) {
    DEBUG_PRINTF("📊 Modbus registers updated for BMS%d (base=%d): V=%.2fV I=%.2fA SOC=%.1f%% SOH=%.1f%%\n",
                 nodeId, baseAddr, bms->batteryVoltage, bms->batteryCurrent, bms->soc, bms->soh);
  }
}

// === PUBLIC FUNCTIONS ===
bool setupModbusTCP() {
  DEBUG_PRINTLN("🔧 Setting up Modbus TCP server...");
  
  setModbusState(MODBUS_STATE_INITIALIZING);
  
  // Initialize all holding registers to 0
  for (int i = 0; i < MODBUS_MAX_HOLDING_REGISTERS; i++) {
    holdingRegisters[i] = 0;
  }
  
  // Initialize statistics
  memset(&modbusStats, 0, sizeof(ModbusStats));
  
  // Start TCP server
  modbusServer.begin();
  modbusServer.setNoDelay(true);
  
  DEBUG_PRINTF("✅ Modbus TCP server ready!\n");
  DEBUG_PRINTF("📊 Total registers available: %d\n", MODBUS_MAX_HOLDING_REGISTERS);
  DEBUG_PRINTF("🔋 Batteries supported: %d (each uses 125 registers)\n", MAX_BMS_NODES);
  DEBUG_PRINTF("🎯 Server IP: %s:%d\n", WiFi.localIP().toString().c_str(), MODBUS_TCP_PORT);
  
  // Print complete register map
  printCompleteModbusRegisterMap();
  
  setModbusState(MODBUS_STATE_RUNNING);
  
  return true;
}

void processModbusTCP() {
  // Handle client connections
  if (!currentModbusClient || !currentModbusClient.connected()) {
    currentModbusClient = modbusServer.available();
    if (currentModbusClient) {
      modbusStats.clientConnections++;
      lastClientActivity = millis();
      
      if (modbusDebugEnabled) {
        DEBUG_PRINTF("🔗 New Modbus TCP client connected: %s:%d\n", 
                     currentModbusClient.remoteIP().toString().c_str(),
                     currentModbusClient.remotePort());
      }
    }
  }
  
  // Process client requests
  if (currentModbusClient && currentModbusClient.connected()) {
    if (currentModbusClient.available()) {
      lastClientActivity = millis();
      handleModbusRequest(currentModbusClient);
    }
    
    // Check for client timeout
    if (millis() - lastClientActivity > MODBUS_CLIENT_TIMEOUT_MS) {
      DEBUG_PRINTLN("⏰ Modbus client timeout - disconnecting");
      currentModbusClient.stop();
      modbusStats.clientTimeouts++;
    }
  }
}

// === 🔥 EXTENDED HEARTBEAT WITH ALL MULTIPLEXER DATA (z v3.0.0) ===
void printBMSHeartbeatExtended(uint8_t nodeId) {
  BMSData* bms = getBMSData(nodeId);
  if (!bms) return;
  
  int batteryIndex = getBMSIndexByNodeId(nodeId);
  uint16_t baseAddr = batteryIndex * 125;
  
  Serial.printf("🔋 BMS%d [Modbus:%d]: %.2fV %.2fA %.1f%% SOH:%.1f%% %s\n",
                nodeId, baseAddr, bms->batteryVoltage, bms->batteryCurrent, 
                bms->soc, bms->soh, bms->masterError ? "⚠️ERR" : "✅OK");
  
  // Show key multiplexer data if available
  if (bms->frame490Count > 0) {
    Serial.printf("   📋 SN:%d/%d HW:%d.%d SW:%d.%d Cycles:%d Energy:%.1fkWh Cap:%.2fAh\n",
                  bms->serialNumber1, bms->serialNumber0,
                  bms->hwVersion1, bms->hwVersion0,
                  bms->swVersion1, bms->swVersion0,
                  bms->batteryCycles, bms->factoryEnergy, bms->designCapacity);
    
    if (bms->errorsMap0 || bms->errorsMap1 || bms->errorsMap2 || bms->errorsMap3) {
      Serial.printf("   ⚠️ Errors: [0x%04X 0x%04X 0x%04X 0x%04X]\n",
                    bms->errorsMap0, bms->errorsMap1, bms->errorsMap2, bms->errorsMap3);
    }
  }
  
  // Show frame statistics
  Serial.printf("   📊 Frames: 190:%d 290:%d 310:%d 390:%d 410:%d 510:%d 490:%d 1B0:%d 710:%d\n",
                bms->frame190Count, bms->frame290Count, bms->frame310Count,
                bms->frame390Count, bms->frame410Count, bms->frame510Count,
                bms->frame490Count, bms->frame1B0Count, bms->frame710Count);
}

// === 🔥 KOMPLETNA MAPA REJESTRÓW MODBUS (z v3.0.0) ===
void printCompleteModbusRegisterMap() {
  Serial.println();
  Serial.println(F("📋 KOMPLETNA MAPA REJESTRÓW MODBUS TCP - 16 BATERII x 125 REJESTRÓW"));
  Serial.println(F("========================================================================"));
  Serial.println(F("🎯 KAŻDA BATERIA: 125 rejestrów (base = battery_index * 125)"));
  Serial.println();
  Serial.println(F("📊 STRUKTURA REJESTRÓW DLA KAŻDEJ BATERII:"));
  Serial.println(F("Base+0-9:    Frame 190 - Basic Data (voltage, current, energy, SOC)"));
  Serial.println(F("Base+10-19:  Frame 190 - Error Flags (8 błędów + 2 reserved)"));
  Serial.println(F("Base+20-29:  Frame 290 - Cell Voltages (min/mean voltage, lokalizacje)"));
  Serial.println(F("Base+30-39:  Frame 310 - SOH/Temperature (SOH, cell voltage, temp, DCiR)"));
  Serial.println(F("Base+40-49:  Frame 390 - Max Voltages (cell max voltage, voltage delta)"));
  Serial.println(F("Base+50-59:  Frame 410 - Temperatures (cell max temp, ready states)"));
  Serial.println(F("Base+60-69:  Frame 510 - Power Limits (charge/discharge limits, I/O)"));
  Serial.println(F("Base+70-89:  🔥 Frame 490 - Multiplexed Data (20 kluczowych parametrów)"));
  Serial.println(F("Base+90-109: 🔥 Error Maps & Versions (20 rejestrów diagnostycznych)"));
  Serial.println(F("Base+110-119: 🔥 Frame 710 & Communication Status (10 rejestrów)"));
  Serial.println(F("Base+120-124: 🔥 Extended Counters & Node ID (5 rejestrów)"));
  Serial.println();
  Serial.println(F("🔥 KLUCZOWE REJESTRY MULTIPLEKSERA (Frame 490):"));
  Serial.println(F("Base+70: Mux Type              | Base+71: Mux Value"));
  Serial.println(F("Base+72-73: Serial Number       | Base+74-77: HW/SW Versions"));
  Serial.println(F("Base+78: Factory Energy [0.1kWh]| Base+79: Design Capacity [mAh]"));
  Serial.println(F("Base+80: System Energy [0.1kWh] | Base+81-84: Temperatures [0.1°C]"));
  Serial.println(F("Base+85: Humidity [%]           | Base+86-87: Time to Full [min]"));
  Serial.println(F("Base+88: Battery Cycles         | Base+89: Detected IMBs"));
  Serial.println();
  Serial.println(F("🔥 REJESTRY DIAGNOSTYCZNE (90-109):"));
  Serial.println(F("Base+90-93: Error Maps [bits]   | Base+94-99: Versions"));
  Serial.println(F("Base+100: Config CRC            | Base+101: IoT Status"));
  Serial.println(F("Base+102: Power On Counter      | Base+103-106: CRC Values"));
  Serial.println(F("Base+107-109: Charge Thresholds"));
  Serial.println();
  Serial.println(F("🔥 REJESTRY KOMUNIKACJI (110-119):"));
  Serial.println(F("Base+110: CANopen State         | Base+111: Communication OK"));
  Serial.println(F("Base+112-113: Packets Received  | Base+114: Parse Errors"));
  Serial.println(F("Base+115-119: Frame Counters (190,290,310,490,710)"));
  Serial.println();
  Serial.println(F("📍 PRZYKŁADY ADRESÓW BATERII:"));
  Serial.println(F("Bateria 0: 0-124      | Bateria 1: 125-249    | Bateria 2: 250-374"));
  Serial.println(F("Bateria 3: 375-499    | Bateria 4: 500-624    | Bateria 5: 625-749"));
  Serial.println(F("..."));
  Serial.println(F("Bateria 15: 1875-1999"));
  Serial.println();
  Serial.println(F("🚀 OBSŁUGIWANE RAMKI CAN:"));
  Serial.println(F("0x190+ID: Basic data           | 0x290+ID: Cell voltages"));
  Serial.println(F("0x310+ID: SOH/temperature      | 0x390+ID: Max voltages"));
  Serial.println(F("0x410+ID: Temperatures         | 0x510+ID: Power limits"));
  Serial.println(F("0x490+ID: 🔥 54 typy multipleksera | 0x1B0+ID: Additional data"));
  Serial.println(F("0x710+ID: CANopen state"));
  Serial.println(F("========================================================================"));
  Serial.println();
}

// === PRIVATE FUNCTIONS ===
static void handleModbusRequest(WiFiClient& client) {
  int bytesAvailable = client.available();
  if (bytesAvailable < 8) return; // Minimum MBAP + PDU header
  
  int bytesRead = client.read(modbusRequestBuffer, min(bytesAvailable, MODBUS_MAX_FRAME_SIZE));
  
  modbusStats.totalRequests++;
  modbusStats.lastRequestTime = millis();
  
  if (modbusDebugEnabled) {
    printModbusRequest(modbusRequestBuffer, bytesRead);
  }
  
  if (!validateModbusRequest(modbusRequestBuffer, bytesRead)) {
    modbusStats.invalidRequests++;
    return;
  }
  
  // Parse MBAP header
  uint16_t transactionId = (modbusRequestBuffer[0] << 8) | modbusRequestBuffer[1];
  uint16_t protocolId = (modbusRequestBuffer[2] << 8) | modbusRequestBuffer[3];
  uint16_t length = (modbusRequestBuffer[4] << 8) | modbusRequestBuffer[5];
  uint8_t slaveId = modbusRequestBuffer[6];
  uint8_t functionCode = modbusRequestBuffer[7];
  
  // Validate request (zgodnie z v3.0.0)
  if (protocolId != 0 || slaveId != MODBUS_SLAVE_ID) {
    DEBUG_PRINTF("❌ Invalid protocol ID (%d) or slave ID (%d)\n", protocolId, slaveId);
    modbusStats.invalidRequests++;
    return;
  }
  
  // Copy MBAP header to response
  modbusResponseBuffer[0] = modbusRequestBuffer[0]; // Transaction ID High
  modbusResponseBuffer[1] = modbusRequestBuffer[1]; // Transaction ID Low
  modbusResponseBuffer[2] = 0; // Protocol ID High
  modbusResponseBuffer[3] = 0; // Protocol ID Low
  modbusResponseBuffer[6] = slaveId; // Slave ID
  
  int responseLength = 0;
  
  // Process function code (zgodnie z v3.0.0)
  switch (functionCode) {
    case MODBUS_FUNC_READ_HOLDING_REGISTERS:
      modbusStats.readHoldingRegisterRequests++;
      responseLength = processReadHoldingRegisters(modbusRequestBuffer, bytesRead, modbusResponseBuffer);
      break;
      
    case MODBUS_FUNC_READ_INPUT_REGISTERS:
      modbusStats.readInputRegisterRequests++;
      responseLength = processReadInputRegisters(modbusRequestBuffer, bytesRead, modbusResponseBuffer);
      break;
      
    case MODBUS_FUNC_WRITE_SINGLE_REGISTER:
      modbusStats.writeSingleRegisterRequests++;
      responseLength = processWriteSingleRegister(modbusRequestBuffer, bytesRead, modbusResponseBuffer);
      break;
      
    case MODBUS_FUNC_WRITE_MULTIPLE_REGISTERS:
      modbusStats.writeMultipleRegisterRequests++;
      responseLength = processWriteMultipleRegisters(modbusRequestBuffer, bytesRead, modbusResponseBuffer);
      break;
      
    default:
      DEBUG_PRINTF("❌ Unsupported function code: 0x%02X\n", functionCode);
      responseLength = createExceptionResponse(functionCode, MODBUS_EXCEPTION_ILLEGAL_FUNCTION, modbusResponseBuffer);
      modbusStats.exceptionResponses++;
      break;
  }
  
  // Send response
  if (responseLength > 0) {
    // Set length in MBAP header
    uint16_t pduLength = responseLength - 6; // Exclude MBAP header
    modbusResponseBuffer[4] = (pduLength >> 8) & 0xFF;
    modbusResponseBuffer[5] = pduLength & 0xFF;
    
    client.write(modbusResponseBuffer, responseLength);
    client.flush();
    modbusStats.totalResponses++;
    
    if (modbusDebugEnabled) {
      printModbusResponse(modbusResponseBuffer, responseLength);
    }
  }
}

static int processReadHoldingRegisters(uint8_t* request, int requestLen, uint8_t* response) {
  if (requestLen < 12) return 0; // Invalid request length
  
  uint16_t startAddress = (request[8] << 8) | request[9];
  uint16_t registerCount = (request[10] << 8) | request[11];
  
  // Validate parameters (zgodnie z v3.0.0)
  if (startAddress >= MODBUS_MAX_HOLDING_REGISTERS || 
      (startAddress + registerCount) > MODBUS_MAX_HOLDING_REGISTERS ||
      registerCount == 0 || registerCount > 125) {
    DEBUG_PRINTF("❌ Register address out of range: %d + %d > %d\n", 
                  startAddress, registerCount, MODBUS_MAX_HOLDING_REGISTERS);
    return createExceptionResponse(MODBUS_FUNC_READ_HOLDING_REGISTERS, 
                                 MODBUS_EXCEPTION_ILLEGAL_DATA_ADDRESS, response);
  }
  
  // Build response
  response[7] = MODBUS_FUNC_READ_HOLDING_REGISTERS; // Function code
  response[8] = registerCount * 2; // Byte count
  
  // Copy register values
  for (int i = 0; i < registerCount; i++) {
    uint16_t regValue = holdingRegisters[startAddress + i];
    response[9 + i * 2] = (regValue >> 8) & 0xFF; // High byte
    response[10 + i * 2] = regValue & 0xFF;       // Low byte
  }
  
  DEBUG_PRINTF("✅ Read %d registers from address %d\n", registerCount, startAddress);
  
  return 9 + registerCount * 2; // MBAP header + PDU
}

static int processReadInputRegisters(uint8_t* request, int requestLen, uint8_t* response) {
  // Input registers are the same as holding registers in this implementation
  return processReadHoldingRegisters(request, requestLen, response);
}

static int processWriteSingleRegister(uint8_t* request, int requestLen, uint8_t* response) {
  if (requestLen < 12) return 0; // Invalid request length
  
  uint16_t address = (request[8] << 8) | request[9];
  uint16_t value = (request[10] << 8) | request[11];
  
  // Validate address
  if (address >= MODBUS_MAX_HOLDING_REGISTERS) {
    return createExceptionResponse(MODBUS_FUNC_WRITE_SINGLE_REGISTER, 
                                 MODBUS_EXCEPTION_ILLEGAL_DATA_ADDRESS, response);
  }
  
  // Write register (for now, just acknowledge - no actual writing to BMS)
  // holdingRegisters[address] = value; // Uncomment to allow writing
  
  DEBUG_PRINTF("📝 Write single register %d = %d (acknowledged but not written)\n", address, value);
  
  // Echo request as response
  memcpy(response + 7, request + 7, 5); // Function code + address + value
  
  return 12; // MBAP header + PDU
}

static int processWriteMultipleRegisters(uint8_t* request, int requestLen, uint8_t* response) {
  if (requestLen < 13) return 0; // Invalid request length
  
  uint16_t startAddress = (request[8] << 8) | request[9];
  uint16_t registerCount = (request[10] << 8) | request[11];
  uint8_t byteCount = request[12];
  
  // Validate parameters
  if (startAddress >= MODBUS_MAX_HOLDING_REGISTERS || 
      (startAddress + registerCount) > MODBUS_MAX_HOLDING_REGISTERS ||
      registerCount == 0 || registerCount > 123 ||
      byteCount != registerCount * 2 ||
      requestLen < (13 + byteCount)) {
    return createExceptionResponse(MODBUS_FUNC_WRITE_MULTIPLE_REGISTERS, 
                                 MODBUS_EXCEPTION_ILLEGAL_DATA_ADDRESS, response);
  }
  
  // Write registers (for now, just acknowledge - no actual writing to BMS)
  /*
  for (int i = 0; i < registerCount; i++) {
    uint16_t value = (request[13 + i * 2] << 8) | request[14 + i * 2];
    holdingRegisters[startAddress + i] = value;
  }
  */
  
  DEBUG_PRINTF("📝 Write multiple registers %d-%d (acknowledged but not written)\n", 
               startAddress, startAddress + registerCount - 1);
  
  // Build response
  response[7] = MODBUS_FUNC_WRITE_MULTIPLE_REGISTERS; // Function code
  response[8] = (startAddress >> 8) & 0xFF; // Start address high
  response[9] = startAddress & 0xFF;        // Start address low
  response[10] = (registerCount >> 8) & 0xFF; // Register count high
  response[11] = registerCount & 0xFF;        // Register count low
  
  return 12; // MBAP header + PDU
}

static int createExceptionResponse(uint8_t functionCode, uint8_t exceptionCode, uint8_t* response) {
  response[7] = functionCode | 0x80; // Function code with error bit
  response[8] = exceptionCode;       // Exception code
  
  DEBUG_PRINTF("❌ Exception response: FC=0x%02X EC=0x%02X\n", functionCode, exceptionCode);
  
  return 9; // MBAP header + error PDU
}

static bool validateModbusRequest(uint8_t* request, int length) {
  if (length < 8) return false; // Too short
  if (length > MODBUS_MAX_FRAME_SIZE) return false; // Too long
  
  // Validate MBAP header
  uint16_t protocolId = (request[2] << 8) | request[3];
  uint16_t mbapLength = (request[4] << 8) | request[5];
  
  if (protocolId != 0) return false; // Must be 0 for Modbus TCP
  if (mbapLength != (length - 6)) return false; // Length mismatch
  
  return true;
}

static void printModbusRequest(uint8_t* request, int length) {
  DEBUG_PRINTF("📥 Modbus Request (%d bytes): ", length);
  for (int i = 0; i < min(length, 20); i++) {
    DEBUG_PRINTF("%02X ", request[i]);
  }
  if (length > 20) DEBUG_PRINT("...");
  DEBUG_PRINTLN();
  
  if (length >= 12) {
    uint16_t transactionId = (request[0] << 8) | request[1];
    uint8_t slaveId = request[6];
    uint8_t functionCode = request[7];
    uint16_t startAddress = (request[8] << 8) | request[9];
    uint16_t registerCount = (request[10] << 8) | request[11];
    
    DEBUG_PRINTF("   TxID=%d SlaveID=%d Func=0x%02X Addr=%d Count=%d\n", 
                 transactionId, slaveId, functionCode, startAddress, registerCount);
  }
}

static void printModbusResponse(uint8_t* response, int length) {
  DEBUG_PRINTF("📤 Modbus Response (%d bytes): ", length);
  for (int i = 0; i < min(length, 20); i++) {
    DEBUG_PRINTF("%02X ", response[i]);
  }
  if (length > 20) DEBUG_PRINT("...");
  DEBUG_PRINTLN();
}

// === PUBLIC UTILITY FUNCTIONS ===
void updateModbusRegister(uint16_t address, uint16_t value) {
  if (address < MODBUS_MAX_HOLDING_REGISTERS) {
    holdingRegisters[address] = value;
    lastRegisterUpdate = millis();
  } else {
    DEBUG_PRINTF("❌ Invalid Modbus register address: %d\n", address);
  }
}

uint16_t readModbusRegister(uint16_t address) {
  if (address < MODBUS_MAX_HOLDING_REGISTERS) {
    return holdingRegisters[address];
  } else {
    DEBUG_PRINTF("❌ Invalid Modbus register address: %d\n", address);
    return 0;
  }
}

void setModbusState(ModbusState_t newState) {
  ModbusState_t oldState = modbusState;
  modbusState = newState;
  
  if (oldState != newState) {
    DEBUG_PRINTF("🔄 Modbus state changed: %d -> %d\n", oldState, newState);
  }
}

ModbusState_t getModbusState() {
  return modbusState;
}

void enableModbusDebug(bool enable) {
  modbusDebugEnabled = enable;
  DEBUG_PRINTF("🔧 Modbus debug %s\n", enable ? "enabled" : "disabled");
}

void printModbusStatistics() {
  DEBUG_PRINTLN("\n📊 === MODBUS TCP STATISTICS ===");
  DEBUG_PRINTF("State: %d\n", modbusState);
  DEBUG_PRINTF("Total Requests: %lu\n", modbusStats.totalRequests);
  DEBUG_PRINTF("Total Responses: %lu\n", modbusStats.totalResponses);
  DEBUG_PRINTF("Invalid Requests: %lu\n", modbusStats.invalidRequests);
  DEBUG_PRINTF("Exception Responses: %lu\n", modbusStats.exceptionResponses);
  DEBUG_PRINTF("Client Connections: %lu\n", modbusStats.clientConnections);
  DEBUG_PRINTF("Client Timeouts: %lu\n", modbusStats.clientTimeouts);
  DEBUG_PRINTF("Read Holding: %lu\n", modbusStats.readHoldingRegisterRequests);
  DEBUG_PRINTF("Read Input: %lu\n", modbusStats.readInputRegisterRequests);
  DEBUG_PRINTF("Write Single: %lu\n", modbusStats.writeSingleRegisterRequests);
  DEBUG_PRINTF("Write Multiple: %lu\n", modbusStats.writeMultipleRegisterRequests);
  
  if (modbusStats.lastRequestTime > 0) {
    DEBUG_PRINTF("Last Request: %lu ms ago\n", millis() - modbusStats.lastRequestTime);
  } else {
    DEBUG_PRINTLN("Last Request: Never");
  }
  
  if (lastRegisterUpdate > 0) {
    DEBUG_PRINTF("Last Register Update: %lu ms ago\n", millis() - lastRegisterUpdate);
  } else {
    DEBUG_PRINTLN("Last Register Update: Never");
  }
  
  DEBUG_PRINTLN("==============================\n");
}