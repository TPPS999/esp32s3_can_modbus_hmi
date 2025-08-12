/*
 * modbus_tcp.cpp - ESP32S3 CAN to Modbus TCP Bridge Modbus Server Implementation
 * 
 * VERSION: v4.0.0
 * DATE: 2025-08-12
 * DESCRIPTION: Implementacja serwera Modbus TCP dla ESP32S3
 */

#include "modbus_tcp.h"
#include "bms_data.h"
#include "utils.h"

// === GLOBAL VARIABLES ===
WiFiServer modbusServer(MODBUS_TCP_PORT);
WiFiClient currentModbusClient;
uint16_t modbusRegisters[MODBUS_MAX_HOLDING_REGISTERS];
ModbusStats modbusStats = {0};
ModbusState_t modbusState = MODBUS_STATE_UNINITIALIZED;

// === PRIVATE VARIABLES ===
static uint8_t modbusRequestBuffer[MODBUS_MAX_FRAME_SIZE];
static uint8_t modbusResponseBuffer[MODBUS_MAX_FRAME_SIZE];
static bool modbusDebugEnabled = true;
static unsigned long lastClientActivity = 0;
static unsigned long lastRegisterUpdate = 0;

// === PRIVATE FUNCTION DECLARATIONS ===
static void updateModbusRegistersFromBMS();
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

// === PUBLIC FUNCTIONS ===

bool setupModbusTCP() {
  DEBUG_PRINTLN("🔧 Setting up Modbus TCP server...");
  
  setModbusState(MODBUS_STATE_INITIALIZING);
  
  // Initialize register array
  memset(modbusRegisters, 0, sizeof(modbusRegisters));
  
  // Initialize statistics
  memset(&modbusStats, 0, sizeof(ModbusStats));
  
  // Start TCP server
  modbusServer.begin();
  modbusServer.setNoDelay(true);
  
  DEBUG_PRINTF("✅ Modbus TCP server started on port %d\n", MODBUS_TCP_PORT);
  DEBUG_PRINTF("📊 %d holding registers available (0-%d)\n", 
               MODBUS_MAX_HOLDING_REGISTERS, MODBUS_MAX_HOLDING_REGISTERS-1);
  DEBUG_PRINTF("🔋 %d BMS modules x %d registers each\n", 
               MAX_BMS_NODES, MODBUS_REGISTERS_PER_BMS);
  
  setModbusState(MODBUS_STATE_RUNNING);
  
  return true;
}

void processModbusTCP() {
  // Update registers from BMS data
  updateModbusRegistersFromBMS();
  
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

void updateModbusRegister(uint16_t address, uint16_t value) {
  if (address < MODBUS_MAX_HOLDING_REGISTERS) {
    modbusRegisters[address] = value;
    lastRegisterUpdate = millis();
  } else {
    DEBUG_PRINTF("❌ Invalid Modbus register address: %d\n", address);
  }
}

uint16_t readModbusRegister(uint16_t address) {
  if (address < MODBUS_MAX_HOLDING_REGISTERS) {
    return modbusRegisters[address];
  }
  DEBUG_PRINTF("❌ Invalid Modbus register address: %d\n", address);
  return 0;
}

void setModbusState(ModbusState_t newState) {
  if (modbusState != newState) {
    DEBUG_PRINTF("🔄 Modbus state: %d -> %d\n", modbusState, newState);
    modbusState = newState;
  }
}

ModbusState_t getModbusState() {
  return modbusState;
}

void resetModbusStatistics() {
  memset(&modbusStats, 0, sizeof(ModbusStats));
  DEBUG_PRINTLN("📊 Modbus statistics reset");
}

void printModbusStatistics() {
  DEBUG_PRINTLN("\n📊 === MODBUS TCP STATISTICS ===");
  DEBUG_PRINTF("   Total Requests: %lu\n", modbusStats.totalRequests);
  DEBUG_PRINTF("   Successful Responses: %lu\n", modbusStats.successfulResponses);
  DEBUG_PRINTF("   Error Responses: %lu\n", modbusStats.errorResponses);
  DEBUG_PRINTF("   Invalid Requests: %lu\n", modbusStats.invalidRequests);
  DEBUG_PRINTLN("\n📈 Function Code Statistics:");
  DEBUG_PRINTF("   Read Holding Registers (0x03): %lu\n", modbusStats.readHoldingRegisterRequests);
  DEBUG_PRINTF("   Read Input Registers (0x04): %lu\n", modbusStats.readInputRegisterRequests);
  DEBUG_PRINTF("   Write Single Register (0x06): %lu\n", modbusStats.writeSingleRegisterRequests);
  DEBUG_PRINTF("   Write Multiple Registers (0x10): %lu\n", modbusStats.writeMultipleRegisterRequests);
  DEBUG_PRINTLN("\n🔗 Connection Statistics:");
  DEBUG_PRINTF("   Client Connections: %lu\n", modbusStats.clientConnections);
  DEBUG_PRINTF("   Client Timeouts: %lu\n", modbusStats.clientTimeouts);
  DEBUG_PRINTF("   Client Disconnections: %lu\n", modbusStats.clientDisconnections);
  
  if (modbusStats.lastRequestTime > 0) {
    DEBUG_PRINTF("   Last Request: %lu ms ago\n", millis() - modbusStats.lastRequestTime);
  }
  if (modbusStats.lastResponseTime > 0) {
    DEBUG_PRINTF("   Last Response: %lu ms ago\n", millis() - modbusStats.lastResponseTime);
  }
  DEBUG_PRINTLN();
}

bool isModbusHealthy() {
  return (modbusState == MODBUS_STATE_RUNNING) && 
         (modbusStats.errorResponses < modbusStats.totalRequests / 2);
}

// === PRIVATE FUNCTIONS ===

static void updateModbusRegistersFromBMS() {
  static unsigned long lastUpdate = 0;
  
  // Update every 100ms to avoid excessive processing
  if (millis() - lastUpdate < 100) return;
  lastUpdate = millis();
  
  for (int i = 0; i < systemConfig.activeBmsNodes; i++) {
    BMSData* bms = getBMSData(systemConfig.bmsNodeIds[i]);
    if (!bms) continue;
    
    uint16_t baseAddr = i * MODBUS_REGISTERS_PER_BMS;
    
    // Frame 190 - Basic Data (Base+0-9)
    modbusRegisters[baseAddr + 0] = floatToModbusRegister(bms->batteryVoltage, 1000.0f);  // mV
    modbusRegisters[baseAddr + 1] = floatToModbusRegister(bms->batteryCurrent, 1000.0f);  // mA
    modbusRegisters[baseAddr + 2] = floatToModbusRegister(bms->remainingEnergy, 100.0f);  // 0.01kWh
    modbusRegisters[baseAddr + 3] = floatToModbusRegister(bms->soc, 10.0f);               // 0.1%
    modbusRegisters[baseAddr + 4] = floatToModbusRegister(bms->fullChargeCapacity, 100.0f); // 0.01Ah
    modbusRegisters[baseAddr + 5] = floatToModbusRegister(bms->remainingCapacity, 100.0f);  // 0.01Ah
    modbusRegisters[baseAddr + 6] = floatToModbusRegister(bms->averageCellVoltage, 1000.0f); // mV
    modbusRegisters[baseAddr + 7] = floatToModbusRegister(bms->totalVoltage, 1000.0f);      // mV
    modbusRegisters[baseAddr + 8] = floatToModbusRegister(bms->packCurrent, 1000.0f);       // mA
    modbusRegisters[baseAddr + 9] = floatToModbusRegister(bms->packPower, 10.0f);           // 0.1W
    
    // Frame 190 - Error Flags (Base+10-19)
    modbusRegisters[baseAddr + 10] = bms->masterError ? 1 : 0;
    modbusRegisters[baseAddr + 11] = bms->cellVoltageError ? 1 : 0;
    modbusRegisters[baseAddr + 12] = bms->cellTempError ? 1 : 0;
    modbusRegisters[baseAddr + 13] = bms->chargeCurrentError ? 1 : 0;
    modbusRegisters[baseAddr + 14] = bms->totalVoltageError ? 1 : 0;
    modbusRegisters[baseAddr + 15] = bms->chargeHighTempError ? 1 : 0;
    modbusRegisters[baseAddr + 16] = bms->chargeLowTempError ? 1 : 0;
    modbusRegisters[baseAddr + 17] = bms->packOverVoltError ? 1 : 0;
    modbusRegisters[baseAddr + 18] = 0; // Reserved
    modbusRegisters[baseAddr + 19] = 0; // Reserved
    
    // Frame 290 - Cell Voltages (Base+20-29)
    modbusRegisters[baseAddr + 20] = floatToModbusRegister(bms->cellMinVoltage, 1000.0f);  // mV
    modbusRegisters[baseAddr + 21] = floatToModbusRegister(bms->cellMeanVoltage, 1000.0f); // mV
    modbusRegisters[baseAddr + 22] = bms->cellMinString;
    modbusRegisters[baseAddr + 23] = bms->cellMinBlock;
    modbusRegisters[baseAddr + 24] = bms->cellMinCell;
    modbusRegisters[baseAddr + 25] = floatToModbusRegister(bms->cellMaxVoltage, 1000.0f);  // mV
    modbusRegisters[baseAddr + 26] = bms->cellMaxString;
    modbusRegisters[baseAddr + 27] = bms->cellMaxBlock;
    modbusRegisters[baseAddr + 28] = bms->cellMaxCell;
    modbusRegisters[baseAddr + 29] = floatToModbusRegister(bms->cellVoltageDelta, 1000.0f); // mV
    
    // Frame 310 - SOH & Temperature (Base+30-39)
    modbusRegisters[baseAddr + 30] = floatToModbusRegister(bms->soh, 10.0f);              // 0.1%
    modbusRegisters[baseAddr + 31] = floatToModbusRegister(bms->cellVoltage, 1.0f);       // mV
    modbusRegisters[baseAddr + 32] = floatToModbusRegister(bms->cellTemperature, 10.0f);  // 0.1°C
    modbusRegisters[baseAddr + 33] = floatToModbusRegister(bms->dcir, 1.0f);              // mΩ
    modbusRegisters[baseAddr + 34] = bms->stringNumber;
    modbusRegisters[baseAddr + 35] = bms->blockNumber;
    modbusRegisters[baseAddr + 36] = bms->cellNumber;
    modbusRegisters[baseAddr + 37] = 0; // Reserved
    modbusRegisters[baseAddr + 38] = 0; // Reserved
    modbusRegisters[baseAddr + 39] = 0; // Reserved
    
    // Frame 410 - Temperatures & Ready States (Base+50-59)
    modbusRegisters[baseAddr + 50] = floatToModbusRegister(bms->cellMaxTemperature, 10.0f); // 0.1°C
    modbusRegisters[baseAddr + 51] = floatToModbusRegister(bms->cellTempDelta, 10.0f);      // 0.1°C
    modbusRegisters[baseAddr + 52] = bms->cellMaxTempString;
    modbusRegisters[baseAddr + 53] = bms->cellMaxTempBlock;
    modbusRegisters[baseAddr + 54] = bms->cellMaxTempCell;
    modbusRegisters[baseAddr + 55] = bms->readyToCharge ? 1 : 0;
    modbusRegisters[baseAddr + 56] = bms->readyToDischarge ? 1 : 0;
    modbusRegisters[baseAddr + 57] = bms->generalReadyFlag ? 1 : 0;
    modbusRegisters[baseAddr + 58] = bms->chargeReadyFlag ? 1 : 0;
    modbusRegisters[baseAddr + 59] = 0; // Reserved
    
    // Frame 510 - Power Limits & I/O (Base+60-69)
    modbusRegisters[baseAddr + 60] = floatToModbusRegister(bms->dccl, 100.0f);           // 0.01A
    modbusRegisters[baseAddr + 61] = floatToModbusRegister(bms->ddcl, 100.0f);           // 0.01A
    modbusRegisters[baseAddr + 62] = floatToModbusRegister(bms->maxChargePower, 10.0f);  // 0.1W
    modbusRegisters[baseAddr + 63] = floatToModbusRegister(bms->maxDischargePower, 10.0f); // 0.1W
    modbusRegisters[baseAddr + 64] = bms->digitalInputs;
    modbusRegisters[baseAddr + 65] = bms->digitalOutputs;
    modbusRegisters[baseAddr + 66] = 0; // Reserved
    modbusRegisters[baseAddr + 67] = 0; // Reserved
    modbusRegisters[baseAddr + 68] = 0; // Reserved
    modbusRegisters[baseAddr + 69] = 0; // Reserved
    
    // Frame 490 - Multiplexed Data (Base+70-89)
    modbusRegisters[baseAddr + 70] = bms->mux490Type;
    modbusRegisters[baseAddr + 71] = bms->mux490Value;
    modbusRegisters[baseAddr + 72] = bms->serialNumberLow;
    modbusRegisters[baseAddr + 73] = bms->serialNumberHigh;
    modbusRegisters[baseAddr + 74] = bms->hwVersionLow;
    modbusRegisters[baseAddr + 75] = bms->hwVersionHigh;
    modbusRegisters[baseAddr + 76] = bms->swVersionLow;
    modbusRegisters[baseAddr + 77] = bms->swVersionHigh;
    modbusRegisters[baseAddr + 78] = floatToModbusRegister(bms->factoryEnergy, 10.0f);   // 0.1kWh
    modbusRegisters[baseAddr + 79] = floatToModbusRegister(bms->designCapacity, 100.0f); // 0.01Ah
    modbusRegisters[baseAddr + 80] = floatToModbusRegister(bms->inletTemperature, 10.0f); // 0.1°C
    modbusRegisters[baseAddr + 81] = floatToModbusRegister(bms->outletTemperature, 10.0f); // 0.1°C
    modbusRegisters[baseAddr + 82] = bms->humidity;
    modbusRegisters[baseAddr + 83] = bms->errorMap1Low;
    modbusRegisters[baseAddr + 84] = bms->errorMap1High;
    modbusRegisters[baseAddr + 85] = bms->errorMap2Low;
    modbusRegisters[baseAddr + 86] = bms->timeToFullCharge;
    modbusRegisters[baseAddr + 87] = bms->timeToFullDischarge;
    modbusRegisters[baseAddr + 88] = bms->batteryCycles;
    modbusRegisters[baseAddr + 89] = 0; // Reserved
    
    // Communication Status (Base+110-119)
    modbusRegisters[baseAddr + 110] = bms->canOpenState;
    modbusRegisters[baseAddr + 111] = bms->communicationOk ? 1 : 0;
    modbusRegisters[baseAddr + 112] = (uint16_t)(bms->packetsReceived & 0xFFFF);
    modbusRegisters[baseAddr + 113] = (uint16_t)((bms->packetsReceived >> 16) & 0xFFFF);
    modbusRegisters[baseAddr + 114] = (uint16_t)(bms->lastUpdate & 0xFFFF);
    modbusRegisters[baseAddr + 115] = (uint16_t)((bms->lastUpdate >> 16) & 0xFFFF);
    modbusRegisters[baseAddr + 116] = 0; // Reserved
    modbusRegisters[baseAddr + 117] = 0; // Reserved
    modbusRegisters[baseAddr + 118] = 0; // Reserved
    modbusRegisters[baseAddr + 119] = 0; // Reserved
    
    // Reserved for future (Base+120-124)
    for (int j = 120; j < 125; j++) {
      modbusRegisters[baseAddr + j] = 0;
    }
  }
}

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
  
  // Copy MBAP header to response
  modbusResponseBuffer[0] = modbusRequestBuffer[0]; // Transaction ID High
  modbusResponseBuffer[1] = modbusRequestBuffer[1]; // Transaction ID Low
  modbusResponseBuffer[2] = 0; // Protocol ID High
  modbusResponseBuffer[3] = 0; // Protocol ID Low
  modbusResponseBuffer[6] = slaveId; // Slave ID
  
  int responseLength = 0;
  
  // Process function code
  switch (functionCode) {
    case MODBUS_FC_READ_HOLDING_REGISTERS:
      modbusStats.readHoldingRegisterRequests++;
      responseLength = processReadHoldingRegisters(modbusRequestBuffer, bytesRead, modbusResponseBuffer);
      break;
      
    case MODBUS_FC_READ_INPUT_REGISTERS:
      modbusStats.readInputRegisterRequests++;
      responseLength = processReadInputRegisters(modbusRequestBuffer, bytesRead, modbusResponseBuffer);
      break;
      
    case MODBUS_FC_WRITE_SINGLE_REGISTER:
      modbusStats.writeSingleRegisterRequests++;
      responseLength = processWriteSingleRegister(modbusRequestBuffer, bytesRead, modbusResponseBuffer);
      break;
      
    case MODBUS_FC_WRITE_MULTIPLE_REGISTERS:
      modbusStats.writeMultipleRegisterRequests++;
      responseLength = processWriteMultipleRegisters(modbusRequestBuffer, bytesRead, modbusResponseBuffer);
      break;
      
    default:
      responseLength = createExceptionResponse(functionCode, MODBUS_EXCEPTION_ILLEGAL_FUNCTION, modbusResponseBuffer);
      modbusStats.errorResponses++;
      break;
  }
  
  if (responseLength > 0) {
    // Update length field in MBAP header
    uint16_t pduLength = responseLength - 6; // Exclude MBAP header
    modbusResponseBuffer[4] = (pduLength >> 8) & 0xFF;
    modbusResponseBuffer[5] = pduLength & 0xFF;
    
    client.write(modbusResponseBuffer, responseLength);
    modbusStats.successfulResponses++;
    modbusStats.lastResponseTime = millis();
    
    if (modbusDebugEnabled) {
      printModbusResponse(modbusResponseBuffer, responseLength);
    }
  }
}

static int processReadHoldingRegisters(uint8_t* request, int requestLen, uint8_t* response) {
  if (requestLen < 12) return 0; // Invalid request length
  
  uint16_t startAddress = (request[8] << 8) | request[9];
  uint16_t registerCount = (request[10] << 8) | request[11];
  
  // Validate address range
  if (startAddress >= MODBUS_MAX_HOLDING_REGISTERS || 
      (startAddress + registerCount) > MODBUS_MAX_HOLDING_REGISTERS ||
      registerCount == 0 || registerCount > 125) {
    return createExceptionResponse(MODBUS_FC_READ_HOLDING_REGISTERS, 
                                 MODBUS_EXCEPTION_ILLEGAL_DATA_ADDRESS, response);
  }
  
  // Build response
  response[7] = MODBUS_FC_READ_HOLDING_REGISTERS; // Function code
  response[8] = registerCount * 2; // Byte count
  
  // Copy register values
  for (int i = 0; i < registerCount; i++) {
    uint16_t regValue = modbusRegisters[startAddress + i];
    response[9 + i * 2] = (regValue >> 8) & 0xFF; // High byte
    response[10 + i * 2] = regValue & 0xFF;       // Low byte
  }
  
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
    return createExceptionResponse(MODBUS_FC_WRITE_SINGLE_REGISTER, 
                                 MODBUS_EXCEPTION_ILLEGAL_DATA_ADDRESS, response);
  }
  
  // Write register (for now, just acknowledge - no actual writing to BMS)
  // modbusRegisters[address] = value; // Uncomment to allow writing
  
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
    return createExceptionResponse(MODBUS_FC_WRITE_MULTIPLE_REGISTERS, 
                                 MODBUS_EXCEPTION_ILLEGAL_DATA_ADDRESS, response);
  }
  
  // Write registers (for now, just acknowledge - no actual writing to BMS)
  // for (int i = 0; i < registerCount; i++) {
  //   uint16_t value = (request[13 + i * 2] << 8) | request[14 + i * 2];
  //   modbusRegisters[startAddress + i] = value;
  // }
  
  // Build response
  response[7] = MODBUS_FC_WRITE_MULTIPLE_REGISTERS; // Function code
  response[8] = request[8]; // Start address high
  response[9] = request[9]; // Start address low
  response[10] = request[10]; // Register count high
  response[11] = request[11]; // Register count low
  
  return 12; // MBAP header + PDU
}

static int createExceptionResponse(uint8_t functionCode, uint8_t exceptionCode, uint8_t* response) {
  response[7] = functionCode | 0x80; // Function code with exception bit
  response[8] = exceptionCode;
  return 9; // MBAP header + exception PDU
}

static bool validateModbusRequest(uint8_t* request, int length) {
  if (length < 8) return false; // Too short
  
  uint16_t protocolId = (request[2] << 8) | request[3];
  if (protocolId != 0) return false; // Invalid protocol ID
  
  uint8_t slaveId = request[6];
  if (slaveId != MODBUS_SLAVE_ID) return false; // Invalid slave ID
  
  return true;
}

static void printModbusRequest(uint8_t* request, int length) {
  if (length < 8) return;
  
  uint16_t transactionId = (request[0] << 8) | request[1];
  uint8_t slaveId = request[6];
  uint8_t functionCode = request[7];
  
  DEBUG_PRINTF("📥 Modbus Request: TxID=%d SlaveID=%d Func=0x%02X Len=%d\n", 
               transactionId, slaveId, functionCode, length);
  
  if (functionCode == MODBUS_FC_READ_HOLDING_REGISTERS && length >= 12) {
    uint16_t startAddr = (request[8] << 8) | request[9];
    uint16_t count = (request[10] << 8) | request[11];
    DEBUG_PRINTF("   Read %d registers from address %d\n", count, startAddr);
  }
}

static void printModbusResponse(uint8_t* response, int length) {
  DEBUG_PRINTF("📤 Modbus Response sent: %d bytes\n", length);
}