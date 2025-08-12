/*
 * modbus_tcp.cpp - ESP32S3 CAN to Modbus TCP Bridge Modbus TCP Implementation
 * 
 * VERSION: v3.1.0
 * DATE: 2025-08-12
 */

#include "modbus_tcp.h"
#include "utils.h"
#include "statistics.h"

// === GLOBAL VARIABLES ===
WiFiServer modbusServer(MODBUS_TCP_PORT);
WiFiClient currentModbusClient;
uint16_t modbusRegisters[MODBUS_MAX_HOLDING_REGISTERS];
ModbusStats modbusStats = {0};

// === PRIVATE VARIABLES ===
static bool modbusLoggingEnabled = false;
static unsigned long modbusTimeout = 5000;
static bool serverStarted = false;

// === INITIALIZATION ===

bool setupModbusTCP() {
  DEBUG_PRINTLN("🔌 Starting Modbus TCP server...");
  
  // Initialize register array
  clearModbusRegisters();
  
  // Start server
  modbusServer.begin();
  serverStarted = true;
  
  // Reset statistics
  resetModbusStatistics();
  
  DEBUG_PRINTF("✅ Modbus TCP server started on port %d\n", MODBUS_TCP_PORT);
  DEBUG_PRINTF("   Slave ID: %d\n", MODBUS_SLAVE_ID);
  DEBUG_PRINTF("   Max registers: %d\n", MODBUS_MAX_HOLDING_REGISTERS);
  DEBUG_PRINTF("   Registers per BMS: %d\n", MODBUS_REGISTERS_PER_BMS);
  
  return true;
}

void stopModbusTCP() {
  if (serverStarted) {
    modbusServer.stop();
    if (currentModbusClient) {
      currentModbusClient.stop();
    }
    serverStarted = false;
    DEBUG_PRINTLN("🔌 Modbus TCP server stopped");
  }
}

bool restartModbusTCP() {
  DEBUG_PRINTLN("🔄 Restarting Modbus TCP server...");
  stopModbusTCP();
  delay(1000);
  return setupModbusTCP();
}

// === MAIN PROCESSING ===

void processModbusTCP() {
  if (!serverStarted) return;
  
  handleNewClients();
  handleClientRequests();
  checkClientConnections();
  
  // Update registers periodically
  static unsigned long lastRegisterUpdate = 0;
  if (millis() - lastRegisterUpdate > 1000) { // Every second
    updateAllModbusRegisters();
    lastRegisterUpdate = millis();
  }
}

void handleNewClients() {
  if (!currentModbusClient || !currentModbusClient.connected()) {
    WiFiClient newClient = modbusServer.available();
    if (newClient) {
      currentModbusClient = newClient;
      modbusStats.clientConnections++;
      DEBUG_PRINTF("🔗 New Modbus client connected: %s\n", 
                   currentModbusClient.remoteIP().toString().c_str());
    }
  }
}

void handleClientRequests() {
  if (currentModbusClient && currentModbusClient.connected()) {
    if (currentModbusClient.available()) {
      if (!processModbusRequest(currentModbusClient)) {
        // Request processing failed
        modbusStats.invalidRequests++;
      }
    }
  }
}

void checkClientConnections() {
  if (currentModbusClient && !currentModbusClient.connected()) {
    DEBUG_PRINTLN("🔗 Modbus client disconnected");
    modbusStats.clientDisconnections++;
    currentModbusClient.stop();
  }
}

// === REQUEST PROCESSING ===

bool processModbusRequest(WiFiClient& client) {
  if (!client.available()) return false;
  
  uint8_t buffer[MODBUS_MAX_FRAME_SIZE];
  size_t bytesRead = client.readBytes(buffer, sizeof(buffer));
  
  if (bytesRead < MODBUS_MBAP_HEADER_SIZE + MODBUS_PDU_HEADER_SIZE) {
    DEBUG_PRINTF("⚠️ Modbus request too short: %d bytes\n", bytesRead);
    return false;
  }
  
  modbusStats.totalRequests++;
  modbusStats.lastRequestTime = millis();
  
  if (modbusLoggingEnabled) {
    printModbusFrame(buffer, bytesRead, true);
  }
  
  handleModbusRequest(client, buffer, bytesRead);
  return true;
}

void handleModbusRequest(WiFiClient& client, uint8_t* buffer, size_t length) {
  ModbusRequest request = parseModbusRequest(buffer, length);
  
  if (!request.valid) {
    DEBUG_PRINTLN("❌ Invalid Modbus request");
    modbusStats.invalidRequests++;
    return;
  }
  
  if (!validateModbusHeader(request)) {
    DEBUG_PRINTF("❌ Invalid Modbus header: SlaveID=%d\n", request.slaveId);
    sendModbusException(client, request.transactionId, request.functionCode, 
                       MODBUS_EXCEPTION_SLAVE_DEVICE_FAILURE);
    return;
  }
  
  // Handle different function codes
  switch (request.functionCode) {
    case MODBUS_FC_READ_HOLDING_REGISTERS:
      modbusStats.readHoldingRegisterRequests++;
      handleReadHoldingRegisters(client, request.transactionId, 
                                request.startAddress, request.registerCount);
      break;
      
    case MODBUS_FC_READ_INPUT_REGISTERS:
      modbusStats.readInputRegisterRequests++;
      handleReadInputRegisters(client, request.transactionId, 
                              request.startAddress, request.registerCount);
      break;
      
    case MODBUS_FC_WRITE_SINGLE_REGISTER:
      modbusStats.writeSingleRegisterRequests++;
      handleWriteSingleRegister(client, request.transactionId, 
                               request.startAddress, request.registerValue);
      break;
      
    case MODBUS_FC_WRITE_MULTIPLE_REGISTERS:
      modbusStats.writeMultipleRegisterRequests++;
      handleWriteMultipleRegisters(client, request.transactionId, 
                                  request.startAddress, request.registerCount, 
                                  request.registerData);
      break;
      
    default:
      DEBUG_PRINTF("❌ Unsupported function code: 0x%02X\n", request.functionCode);
      sendModbusException(client, request.transactionId, request.functionCode, 
                         MODBUS_EXCEPTION_ILLEGAL_FUNCTION);
      break;
  }
}

// === FUNCTION CODE HANDLERS ===

void handleReadHoldingRegisters(WiFiClient& client, uint16_t transactionId, 
                               uint16_t startAddress, uint16_t registerCount) {
  DEBUG_PRINTF("📊 Read Holding Registers: addr=%d, count=%d\n", startAddress, registerCount);
  
  if (!isValidRegisterAddress(startAddress, registerCount)) {
    DEBUG_PRINTF("❌ Invalid register address range: %d-%d\n", 
                 startAddress, startAddress + registerCount - 1);
    sendModbusException(client, transactionId, MODBUS_FC_READ_HOLDING_REGISTERS, 
                       MODBUS_EXCEPTION_ILLEGAL_DATA_ADDRESS);
    return;
  }
  
  // Prepare response data
  uint8_t responseData[256];
  responseData[0] = registerCount * 2; // Byte count
  
  // Copy register data (big-endian format)
  for (uint16_t i = 0; i < registerCount; i++) {
    uint16_t regValue = modbusRegisters[startAddress + i];
    responseData[1 + i * 2] = (regValue >> 8) & 0xFF;     // High byte
    responseData[2 + i * 2] = regValue & 0xFF;            // Low byte
  }
  
  sendModbusResponse(client, transactionId, MODBUS_FC_READ_HOLDING_REGISTERS, 
                    responseData, 1 + registerCount * 2);
  
  modbusStats.successfulResponses++;
  DEBUG_PRINTF("📤 Sent %d registers\n", registerCount);
}

void handleReadInputRegisters(WiFiClient& client, uint16_t transactionId, 
                             uint16_t startAddress, uint16_t registerCount) {
  // Input registers are read-only, same as holding registers for our application
  handleReadHoldingRegisters(client, transactionId, startAddress, registerCount);
}

void handleWriteSingleRegister(WiFiClient& client, uint16_t transactionId, 
                              uint16_t registerAddress, uint16_t registerValue) {
  DEBUG_PRINTF("📝 Write Single Register: addr=%d, value=%d\n", registerAddress, registerValue);
  
  if (!isValidRegisterAddress(registerAddress, 1)) {
    sendModbusException(client, transactionId, MODBUS_FC_WRITE_SINGLE_REGISTER, 
                       MODBUS_EXCEPTION_ILLEGAL_DATA_ADDRESS);
    return;
  }
  
  // Check if register is writable (only configuration registers)
  if (registerAddress < REG_SYSTEM_STATUS) {
    sendModbusException(client, transactionId, MODBUS_FC_WRITE_SINGLE_REGISTER, 
                       MODBUS_EXCEPTION_ILLEGAL_DATA_ADDRESS);
    return;
  }
  
  modbusRegisters[registerAddress] = registerValue;
  
  // Echo back the written value
  uint8_t responseData[4];
  responseData[0] = (registerAddress >> 8) & 0xFF;
  responseData[1] = registerAddress & 0xFF;
  responseData[2] = (registerValue >> 8) & 0xFF;
  responseData[3] = registerValue & 0xFF;
  
  sendModbusResponse(client, transactionId, MODBUS_FC_WRITE_SINGLE_REGISTER, 
                    responseData, 4);
  
  modbusStats.successfulResponses++;
}

void handleWriteMultipleRegisters(WiFiClient& client, uint16_t transactionId, 
                                 uint16_t startAddress, uint16_t registerCount, 
                                 const uint8_t* registerData) {
  DEBUG_PRINTF("📝 Write Multiple Registers: addr=%d, count=%d\n", startAddress, registerCount);
  
  if (!isValidRegisterAddress(startAddress, registerCount)) {
    sendModbusException(client, transactionId, MODBUS_FC_WRITE_MULTIPLE_REGISTERS, 
                       MODBUS_EXCEPTION_ILLEGAL_DATA_ADDRESS);
    return;
  }
  
  // Check if registers are writable
  if (startAddress < REG_SYSTEM_STATUS) {
    sendModbusException(client, transactionId, MODBUS_FC_WRITE_MULTIPLE_REGISTERS, 
                       MODBUS_EXCEPTION_ILLEGAL_DATA_ADDRESS);
    return;
  }
  
  // Write register values
  for (uint16_t i = 0; i < registerCount; i++) {
    uint16_t regValue = (registerData[i * 2] << 8) | registerData[i * 2 + 1];
    modbusRegisters[startAddress + i] = regValue;
  }
  
  // Response: start address and register count
  uint8_t responseData[4];
  responseData[0] = (startAddress >> 8) & 0xFF;
  responseData[1] = startAddress & 0xFF;
  responseData[2] = (registerCount >> 8) & 0xFF;
  responseData[3] = registerCount & 0xFF;
  
  sendModbusResponse(client, transactionId, MODBUS_FC_WRITE_MULTIPLE_REGISTERS, 
                    responseData, 4);
  
  modbusStats.successfulResponses++;
}

// === RESPONSE GENERATION ===

void sendModbusResponse(WiFiClient& client, uint16_t transactionId, uint8_t functionCode, 
                       const uint8_t* data, size_t dataLength) {
  uint8_t response[MODBUS_MAX_FRAME_SIZE];
  size_t responseLength = 0;
  
  // MBAP Header
  response[0] = (transactionId >> 8) & 0xFF;    // Transaction ID high
  response[1] = transactionId & 0xFF;           // Transaction ID low
  response[2] = 0x00;                           // Protocol ID high
  response[3] = 0x00;                           // Protocol ID low
  
  uint16_t length = 2 + dataLength;             // Unit ID + Function Code + Data
  response[4] = (length >> 8) & 0xFF;           // Length high
  response[5] = length & 0xFF;                  // Length low
  
  response[6] = MODBUS_SLAVE_ID;                // Unit ID
  response[7] = functionCode;                   // Function Code
  
  responseLength = 8;
  
  // Copy data
  if (data && dataLength > 0) {
    memcpy(&response[responseLength], data, dataLength);
    responseLength += dataLength;
  }
  
  // Send response
  client.write(response, responseLength);
  
  modbusStats.lastResponseTime = millis();
  
  if (modbusLoggingEnabled) {
    printModbusFrame(response, responseLength, false);
  }
  
  DEBUG_PRINTF("📤 Modbus response sent: %d bytes\n", responseLength);
}

void sendModbusException(WiFiClient& client, uint16_t transactionId, uint8_t functionCode, 
                        uint8_t exceptionCode) {
  uint8_t exceptionData = exceptionCode;
  uint8_t errorFunctionCode = functionCode | 0x80;  // Set error bit
  
  sendModbusResponse(client, transactionId, errorFunctionCode, &exceptionData, 1);
  
  modbusStats.errorResponses++;
  logModbusError(transactionId, functionCode, exceptionCode);
  
  DEBUG_PRINTF("❌ Modbus exception sent: FC=0x%02X, Exception=0x%02X\n", 
               functionCode, exceptionCode);
}

// === REGISTER MANAGEMENT ===

void updateModbusRegisters(uint8_t nodeId) {
  BMSData* bms = getBMSData(nodeId);
  if (!bms) return;
  
  uint16_t baseAddress = getBMSBaseAddress(nodeId);
  if (baseAddress == 0) return;
  
  mapBMSDataToRegisters(nodeId, *bms);
  
  DEBUG_PRINTF("📊 Updated Modbus registers for BMS%d (base: %d)\n", nodeId, baseAddress);
}

void updateAllModbusRegisters() {
  // Update all BMS data
  for (int i = 0; i < systemConfig.activeBmsNodes; i++) {
    uint8_t nodeId = systemConfig.bmsNodeIds[i];
    updateModbusRegisters(nodeId);
  }
  
  // Update system status
  mapSystemStatusToRegisters();
}

void clearModbusRegisters() {
  memset(modbusRegisters, 0, sizeof(modbusRegisters));
  DEBUG_PRINTLN("📊 Modbus registers cleared");
}

bool isValidRegisterAddress(uint16_t address, uint16_t count) {
  return (address + count) <= MODBUS_MAX_HOLDING_REGISTERS && count > 0;
}

uint16_t getBMSBaseAddress(uint8_t nodeId) {
  int index = getBatteryIndexFromNodeId(nodeId);
  if (index == -1) return 0;
  
  return index * MODBUS_REGISTERS_PER_BMS;
}

void mapBMSDataToRegisters(uint8_t nodeId, const BMSData& bms) {
  uint16_t baseAddr = getBMSBaseAddress(nodeId);
  if (baseAddr == 0) return;
  
  // Basic data
  modbusRegisters[baseAddr + REG_BATTERY_VOLTAGE] = floatToRegister(bms.batteryVoltage, 1000);  // mV
  modbusRegisters[baseAddr + REG_BATTERY_CURRENT] = floatToRegister(bms.batteryCurrent, 1000);  // mA
  modbusRegisters[baseAddr + REG_REMAINING_ENERGY] = floatToRegister(bms.remainingEnergy, 10);  // Wh*10
  modbusRegisters[baseAddr + REG_SOC] = (uint16_t)(bms.soc * 10);                              // 0.1%
  modbusRegisters[baseAddr + REG_SOH] = (uint16_t)(bms.soh * 10);                              // 0.1%
  
  // Error flags
  modbusRegisters[baseAddr + REG_ERROR_MASTER] = bms.masterError ? 1 : 0;
  modbusRegisters[baseAddr + REG_ERROR_CELL_VOLTAGE] = bms.cellVoltageError ? 1 : 0;
  modbusRegisters[baseAddr + REG_ERROR_UNDER_VOLTAGE] = bms.cellUnderVoltageError ? 1 : 0;
  modbusRegisters[baseAddr + REG_ERROR_OVER_VOLTAGE] = bms.cellOverVoltageError ? 1 : 0;
  modbusRegisters[baseAddr + REG_ERROR_CELL_IMBALANCE] = bms.cellImbalanceError ? 1 : 0;
  modbusRegisters[baseAddr + REG_ERROR_UNDER_TEMP] = bms.underTemperatureError ? 1 : 0;
  modbusRegisters[baseAddr + REG_ERROR_OVER_TEMP] = bms.overTemperatureError ? 1 : 0;
  modbusRegisters[baseAddr + REG_ERROR_OVER_CURRENT] = bms.overCurrentError ? 1 : 0;
  
  // Cell voltages
  modbusRegisters[baseAddr + REG_MIN_CELL_VOLTAGE] = floatToRegister(bms.minCellVoltage, 10000);  // 0.1mV
  modbusRegisters[baseAddr + REG_AVG_CELL_VOLTAGE] = floatToRegister(bms.averageCellVoltage, 10000);
  modbusRegisters[baseAddr + REG_MAX_CELL_VOLTAGE] = floatToRegister(bms.maxCellVoltage, 10000);
  modbusRegisters[baseAddr + REG_MAX_CELL_ID] = bms.maxCellVoltageId;
  modbusRegisters[baseAddr + REG_MIN_CELL_ID] = bms.minCellVoltageId;
  modbusRegisters[baseAddr + REG_DELTA_CELL_VOLTAGE] = floatToRegister(bms.deltaCellVoltage, 10000);
  
  // Temperatures
  modbusRegisters[baseAddr + REG_AVG_TEMPERATURE] = bms.averageTemperature + 273;
  modbusRegisters[baseAddr + REG_TEMPERATURE_1] = bms.temperature1 + 273;
  modbusRegisters[baseAddr + REG_TEMPERATURE_2] = bms.temperature2 + 273;
  modbusRegisters[baseAddr + REG_TEMPERATURE_3] = bms.temperature3 + 273;
  
  // Limits
  modbusRegisters[baseAddr + REG_MAX_CHARGE_CURRENT] = floatToRegister(bms.maxAllowedChargeCurrent, 1000);
  modbusRegisters[baseAddr + REG_MAX_DISCHARGE_CURRENT] = floatToRegister(bms.maxAllowedDischargeCurrent, 1000);
  modbusRegisters[baseAddr + REG_MAX_CHARGE_VOLTAGE] = floatToRegister(bms.maxAllowedChargeVoltage, 1000);
  modbusRegisters[baseAddr + REG_MAX_DISCHARGE_VOLTAGE] = floatToRegister(bms.maxAllowedDischargeVoltage, 1000);
  
  // Power limits
  modbusRegisters[baseAddr + REG_MAX_CHARGE_POWER] = (uint16_t)bms.maxChargePower;
  modbusRegisters[baseAddr + REG_MAX_DISCHARGE_POWER] = (uint16_t)bms.maxDischargePower;
  modbusRegisters[baseAddr + REG_DIGITAL_INPUTS] = bms.digitalInputs;
  modbusRegisters[baseAddr + REG_DIGITAL_OUTPUTS] = bms.digitalOutputs;
  
  // Status flags
  modbusRegisters[baseAddr + REG_GENERAL_READY] = bms.generalReadyFlag ? 1 : 0;
  modbusRegisters[baseAddr + REG_CHARGE_READY] = bms.chargeReadyFlag ? 1 : 0;
  modbusRegisters[baseAddr + REG_COMMUNICATION_OK] = bms.communicationOk ? 1 : 0;
  
  // Communication statistics
  writeRegisterPair(baseAddr + REG_PACKETS_RECEIVED_LOW, bms.packetsReceived);
}

void mapSystemStatusToRegisters() {
  // System status
  modbusRegisters[REG_SYSTEM_STATUS] = systemState;
  modbusRegisters[REG_TOTAL_BATTERIES] = systemConfig.activeBmsNodes;
  modbusRegisters[REG_ONLINE_BATTERIES] = getActiveBatteryCount();
  
  // Count batteries with errors
  int errorBatteries = 0;
  for (int i = 0; i < systemConfig.activeBmsNodes; i++) {
    uint8_t nodeId = systemConfig.bmsNodeIds[i];
    if (hasCriticalError(nodeId)) errorBatteries++;
  }
  modbusRegisters[REG_ERROR_BATTERIES] = errorBatteries;
  
  // System uptime
  writeRegisterPair(REG_UPTIME_LOW, millis() / 1000);
  
  // Statistics
  writeRegisterPair(REG_CAN_FRAMES_LOW, canStats.totalFramesReceived);
  writeRegisterPair(REG_MODBUS_REQUESTS_LOW, modbusStats.totalRequests);
  
  // Configuration
  modbusRegisters[REG_CAN_SPEED] = systemConfig.canSpeed;
  modbusRegisters[REG_ACTIVE_BMS_COUNT] = systemConfig.activeBmsNodes;
}

// === DATA CONVERSION ===

uint16_t floatToRegister(float value, float multiplier) {
  int32_t intValue = (int32_t)(value * multiplier);
  if (intValue > 65535) intValue = 65535;
  if (intValue < 0) intValue = 0;
  return (uint16_t)intValue;
}

float registerToFloat(uint16_t reg, float divider) {
  return (float)reg / divider;
}

void writeRegisterPair(uint16_t baseAddress, uint32_t value) {
  modbusRegisters[baseAddress] = value & 0xFFFF;         // Low word
  modbusRegisters[baseAddress + 1] = (value >> 16) & 0xFFFF;  // High word
}

uint32_t readRegisterPair(uint16_t baseAddress) {
  return ((uint32_t)modbusRegisters[baseAddress + 1] << 16) | modbusRegisters[baseAddress];
}

// === PROTOCOL PARSING ===

ModbusRequest parseModbusRequest(const uint8_t* buffer, size_t length) {
  ModbusRequest request = {0};
  
  if (length < MODBUS_MBAP_HEADER_SIZE + MODBUS_PDU_HEADER_SIZE) {
    request.valid = false;
    return request;
  }
  
  // Parse MBAP header
  request.transactionId = (buffer[0] << 8) | buffer[1];
  request.protocolId = (buffer[2] << 8) | buffer[3];
  request.length = (buffer[4] << 8) | buffer[5];
  request.slaveId = buffer[6];
  request.functionCode = buffer[7];
  
  // Parse PDU based on function code
  switch (request.functionCode) {
    case MODBUS_FC_READ_HOLDING_REGISTERS:
    case MODBUS_FC_READ_INPUT_REGISTERS:
      if (length >= 12) {
        request.startAddress = (buffer[8] << 8) | buffer[9];
        request.registerCount = (buffer[10] << 8) | buffer[11];
        request.valid = true;
      }
      break;
      
    case MODBUS_FC_WRITE_SINGLE_REGISTER:
      if (length >= 12) {
        request.startAddress = (buffer[8] << 8) | buffer[9];
        request.registerValue = (buffer[10] << 8) | buffer[11];
        request.valid = true;
      }
      break;
      
    case MODBUS_FC_WRITE_MULTIPLE_REGISTERS:
      if (length >= 13) {
        request.startAddress = (buffer[8] << 8) | buffer[9];
        request.registerCount = (buffer[10] << 8) | buffer[11];
        uint8_t byteCount = buffer[12];
        if (length >= 13 + byteCount && byteCount == request.registerCount * 2) {
          request.registerData = &buffer[13];
          request.valid = true;
        }
      }
      break;
      
    default:
      request.valid = false;
      break;
  }
  
  return request;
}

bool validateModbusHeader(const ModbusRequest& request) {
  // Check protocol ID (should be 0 for Modbus TCP)
  if (request.protocolId != 0) {
    DEBUG_PRINTF("❌ Invalid protocol ID: %d\n", request.protocolId);
    return false;
  }
  
  // Check slave ID
  if (request.slaveId != MODBUS_SLAVE_ID) {
    DEBUG_PRINTF("❌ Invalid slave ID: %d (expected %d)\n", request.slaveId, MODBUS_SLAVE_ID);
    return false;
  }
  
  return true;
}

// === STATISTICS AND DIAGNOSTICS ===

void updateModbusStatistics(bool success, uint8_t functionCode) {
  if (success) {
    modbusStats.successfulResponses++;
  } else {
    modbusStats.errorResponses++;
  }
}

void printModbusStatistics() {
  DEBUG_PRINTLN("\n📊 === MODBUS TCP STATISTICS ===");
  DEBUG_PRINTF("   Total Requests: %lu\n", modbusStats.totalRequests);
  DEBUG_PRINTF("   Successful Responses: %lu\n", modbusStats.successfulResponses);
  DEBUG_PRINTF("   Error Responses: %lu\n", modbusStats.errorResponses);
  DEBUG_PRINTF("   Invalid Requests: %lu\n", modbusStats.invalidRequests);
  
  DEBUG_PRINTLN("\n   Function Code Statistics:");
  DEBUG_PRINTF("     Read Holding Registers: %lu\n", modbusStats.readHoldingRegisterRequests);
  DEBUG_PRINTF("     Read Input Registers: %lu\n", modbusStats.readInputRegisterRequests);
  DEBUG_PRINTF("     Write Single Register: %lu\n", modbusStats.writeSingleRegisterRequests);
  DEBUG_PRINTF("     Write Multiple Registers: %lu\n", modbusStats.writeMultipleRegisterRequests);
  
  DEBUG_PRINTF("\n   Client Connections: %lu\n", modbusStats.clientConnections);
  DEBUG_PRINTF("   Client Disconnections: %lu\n", modbusStats.clientDisconnections);
  
  if (modbusStats.totalRequests > 0) {
    float successRate = (float)modbusStats.successfulResponses / modbusStats.totalRequests * 100.0;
    DEBUG_PRINTF("   Success Rate: %.1f%%\n", successRate);
  }
  
  if (modbusStats.lastRequestTime > 0) {
    DEBUG_PRINTF("   Last Request: %lu ms ago\n", millis() - modbusStats.lastRequestTime);
  }
  
  if (strlen(modbusStats.lastError) > 0) {
    DEBUG_PRINTF("   Last Error: %s\n", modbusStats.lastError);
  }
  
  DEBUG_PRINTLN("================================\n");
}

void resetModbusStatistics() {
  memset(&modbusStats, 0, sizeof(ModbusStats));
  DEBUG_PRINTLN("📊 Modbus statistics reset");
}

void handleModbusError(const char* error) {
  modbusStats.errorResponses++;
  strncpy(modbusStats.lastError, error, sizeof(modbusStats.lastError) - 1);
  modbusStats.lastError[sizeof(modbusStats.lastError) - 1] = '\0';
  DEBUG_PRINTF("❌ Modbus error: %s\n", error);
}

void logModbusError(uint16_t transactionId, uint8_t functionCode, uint8_t exceptionCode) {
  char errorMsg[64];
  snprintf(errorMsg, sizeof(errorMsg), "TxID:%d FC:0x%02X Exception:0x%02X", 
           transactionId, functionCode, exceptionCode);
  handleModbusError(errorMsg);
}

void printModbusFrame(const uint8_t* frame, size_t length, bool isRequest) {
  if (!modbusLoggingEnabled) return;
  
  DEBUG_PRINTF("📦 Modbus %s [%d]: ", isRequest ? "Request" : "Response", length);
  printHexDump(frame, length, "");
  
  if (length >= 8) {
    uint16_t transactionId = (frame[0] << 8) | frame[1];
    uint8_t slaveId = frame[6];
    uint8_t functionCode = frame[7];
    DEBUG_PRINTF(" (TxID:%d SlaveID:%d FC:0x%02X)", transactionId, slaveId, functionCode);
  }
  
  DEBUG_PRINTLN();
}

// === ADVANCED FEATURES ===

void enableModbusLogging(bool enable) {
  modbusLoggingEnabled = enable;
  DEBUG_PRINTF("🐛 Modbus logging %s\n", enable ? "enabled" : "disabled");
}

void setModbusTimeout(unsigned long timeoutMs) {
  modbusTimeout = timeoutMs;
  DEBUG_PRINTF("⏰ Modbus timeout set to %lu ms\n", timeoutMs);
}

bool testModbusConnection() {
  if (!serverStarted) {
    DEBUG_PRINTLN("❌ Modbus server not started");
    return false;
  }
  
  if (!currentModbusClient || !currentModbusClient.connected()) {
    DEBUG_PRINTLN("❌ No Modbus client connected");
    return false;
  }
  
  DEBUG_PRINTLN("✅ Modbus connection test passed");
  return true;
}

// === CLIENT MANAGEMENT ===

bool acceptNewClient() {
  WiFiClient newClient = modbusServer.available();
  if (newClient) {
    if (currentModbusClient && currentModbusClient.connected()) {
      // Disconnect old client
      currentModbusClient.stop();
      modbusStats.clientDisconnections++;
    }
    
    currentModbusClient = newClient;
    modbusStats.clientConnections++;
    DEBUG_PRINTF("🔗 New Modbus client accepted: %s\n", 
                 newClient.remoteIP().toString().c_str());
    return true;
  }
  return false;
}

void disconnectClient(WiFiClient& client) {
  if (client.connected()) {
    client.stop();
    modbusStats.clientDisconnections++;
    DEBUG_PRINTLN("🔗 Modbus client disconnected");
  }
}

bool isClientConnected(WiFiClient& client) {
  return client && client.connected();
}

void cleanupDisconnectedClients() {
  if (currentModbusClient && !currentModbusClient.connected()) {
    currentModbusClient.stop();
    modbusStats.clientDisconnections++;
  }
}

// === UTILITY FUNCTIONS ===

uint16_t calculateCRC16(const uint8_t* data, size_t length) {
  // CRC-16 calculation (not used in Modbus TCP, but useful for RTU)
  uint16_t crc = 0xFFFF;
  
  for (size_t i = 0; i < length; i++) {
    crc ^= data[i];
    for (int j = 0; j < 8; j++) {
      if (crc & 0x0001) {
        crc = (crc >> 1) ^ 0xA001;
      } else {
        crc >>= 1;
      }
    }
  }
  
  return crc;
}

bool validateCRC(const uint8_t* data, size_t length) {
  // Not used in Modbus TCP
  return true;
}

// === REGISTER ACCESS HELPERS ===

bool setRegisterValue(uint16_t address, uint16_t value) {
  if (!isValidRegisterAddress(address, 1)) return false;
  
  modbusRegisters[address] = value;
  return true;
}

uint16_t getRegisterValue(uint16_t address) {
  if (!isValidRegisterAddress(address, 1)) return 0;
  
  return modbusRegisters[address];
}

void dumpRegisters(uint16_t startAddr, uint16_t count) {
  DEBUG_PRINTF("📊 Register dump [%d-%d]:\n", startAddr, startAddr + count - 1);
  
  for (uint16_t i = 0; i < count; i++) {
    if (i % 8 == 0) {
      DEBUG_PRINTF("   %04d: ", startAddr + i);
    }
    DEBUG_PRINTF("%04X ", modbusRegisters[startAddr + i]);
    if ((i + 1) % 8 == 0 || i == count - 1) {
      DEBUG_PRINTLN();
    }
  }
}