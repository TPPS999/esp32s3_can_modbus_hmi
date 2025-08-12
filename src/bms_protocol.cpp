float convertImpedance(uint16_t rawValue) {
  return rawValue * IFS_BMS_IMPEDANCE_RESOLUTION;
}

// === ERROR FLAG PARSING ===

void parseErrorFlags(uint8_t errorByte, BMSData& bms) {
  bms.masterError = (errorByte & 0x01) != 0;
  bms.cellVoltageError = (errorByte & 0x02) != 0;
  bms.cellUnderVoltageError = (errorByte & 0x04) != 0;
  bms.cellOverVoltageError = (errorByte & 0x08) != 0;
  bms.cellImbalanceError = (errorByte & 0x10) != 0;
  bms.underTemperatureError = (errorByte & 0x20) != 0;
  bms.overTemperatureError = (errorByte & 0x40) != 0;
  bms.overCurrentError = (errorByte & 0x80) != 0;
}

uint8_t createErrorByte(const BMSData& bms) {
  uint8_t errorByte = 0;
  if (bms.masterError) errorByte |= 0x01;
  if (bms.cellVoltageError) errorByte |= 0x02;
  if (bms.cellUnderVoltageError) errorByte |= 0x04;
  if (bms.cellOverVoltageError) errorByte |= 0x08;
  if (bms.cellImbalanceError) errorByte |= 0x10;
  if (bms.underTemperatureError) errorByte |= 0x20;
  if (bms.overTemperatureError) errorByte |= 0x40;
  if (bms.overCurrentError) errorByte |= 0x80;
  return errorByte;
}

// === MULTIPLEXED DATA HANDLING ===

void processMultiplexedData(uint8_t nodeId, uint8_t muxType, uint16_t muxValue) {
  BMSData* bms = getBMSData(nodeId);
  if (!bms) return;
  
  switch (muxType) {
    case 0x00: bms->serialNumber0 = muxValue; break;
    case 0x01: bms->serialNumber1 = muxValue; break;
    case 0x02: bms->serialNumber2 = muxValue; break;
    case 0x03: bms->serialNumber3 = muxValue; break;
    case 0x04: bms->hwVersionA = muxValue; break;
    case 0x05: bms->hwVersionB = muxValue; break;
    case 0x06: bms->hwVersionC = muxValue; break;
    case 0x07: bms->hwVersionD = muxValue; break;
    case 0x08: bms->factoryEnergy = muxValue * 0.1; break;
    case 0x20: bms->batteryCycles = muxValue; break;
    case 0x30: bms->swVersionA = muxValue; break;
    case 0x31: bms->swVersionB = muxValue; break;
    case 0x32: bms->swVersionC = muxValue; break;
    case 0x33: bms->swVersionD = muxValue; break;
    default:
      DEBUG_PRINTF("⚠️ Unknown multiplexed data type: 0x%02X\n", muxType);
      break;
  }
}

// === VALIDATION FUNCTIONS ===

bool isValidNodeIdForFrame(uint8_t nodeId, unsigned long canId) {
  return getBatteryIndexFromNodeId(nodeId) != -1;
}

bool validateVoltageRange(float voltage) {
  return voltage >= BMS_MIN_VOLTAGE && voltage <= BMS_MAX_VOLTAGE;
}

bool validateCurrentRange(float current) {
  return abs(current) <= BMS_MAX_CURRENT;
}

bool validateTemperatureRange(int16_t temperature) {
  return temperature >= BMS_MIN_TEMPERATURE && temperature <= BMS_MAX_TEMPERATURE;
}

bool validateSOCRange(float soc) {
  return soc >= 0.0 && soc <= 100.0;
}

bool validateSOHRange(float soh) {
  return soh >= 0.0 && soh <= 100.0;
}

// === UTILITY FUNCTIONS ===

uint16_t combineBytes(uint8_t lowByte, uint8_t highByte) {
  return (uint16_t(highByte) << 8) | uint16_t(lowByte);
}

uint32_t combineWords(uint16_t lowWord, uint16_t highWord) {
  return (uint32_t(highWord) << 16) | uint32_t(lowWord);
}

void splitWord(uint16_t word, uint8_t& lowByte, uint8_t& highByte) {
  lowByte = word & 0xFF;
  highByte = (word >> 8) & 0xFF;
}

// === ERROR HANDLING ===

void handleParseError(uint8_t nodeId, BMSFrameType_t frameType, const char* error) {
  protocolStats.parseErrors++;
  protocolStats.lastErrorTime = millis();
  snprintf(protocolStats.lastError, sizeof(protocolStats.lastError), 
           "BMS%d Frame%s: %s", nodeId, frameInfo[frameType].name, error);
  
  DEBUG_PRINTF("❌ Parse Error: %s\n", protocolStats.lastError);
  
  // Optionally reset problematic data
  if (frameType < BMS_FRAME_TYPE_COUNT && frameInfo[frameType].isCritical) {
    resetBMSDataOnError(nodeId);
  }
}

void resetBMSDataOnError(uint8_t nodeId) {
  BMSData* bms = getBMSData(nodeId);
  if (!bms) return;
  
  // Reset only non-critical data to safe defaults
  bms->communicationOk = false;
  DEBUG_PRINTF("🔄 Reset BMS%d data due to parse error\n", nodeId);
}

bool attemptDataRecovery(uint8_t nodeId, BMSFrameType_t frameType) {
  // Attempt to recover from parse errors
  BMSData* bms = getBMSData(nodeId);
  if (!bms) return false;
  
  // Simple recovery: mark as communication error and wait for next valid frame
  bms->communicationOk = false;
  DEBUG_PRINTF("🔄 Attempting data recovery for BMS%d frame %s\n", 
               nodeId, frameInfo[frameType].name);
  
  return true;
}

// === STATISTICS AND DIAGNOSTICS ===

void updateProtocolStatistics(uint8_t nodeId, BMSFrameType_t frameType, bool success) {
  if (frameType >= BMS_FRAME_TYPE_COUNT) return;
  
  if (success) {
    protocolStats.successfulParses++;
  } else {
    protocolStats.parseErrors++;
  }
}

void printProtocolStatistics() {
  DEBUG_PRINTLN("\n📊 === PROTOCOL STATISTICS ===");
  DEBUG_PRINTF("   Total Frames Parsed: %lu\n", protocolStats.totalFramesParsed);
  DEBUG_PRINTF("   Successful Parses: %lu\n", protocolStats.successfulParses);
  DEBUG_PRINTF("   Parse Errors: %lu\n", protocolStats.parseErrors);
  DEBUG_PRINTF("   Invalid Frames: %lu\n", protocolStats.invalidFrames);
  
  if (protocolStats.totalFramesParsed > 0) {
    float successRate = (float)protocolStats.successfulParses / protocolStats.totalFramesParsed * 100.0;
    DEBUG_PRINTF("   Success Rate: %.1f%%\n", successRate);
  }
  
  DEBUG_PRINTLN("\n   Frame Type Counts:");
  for (int i = 0; i < BMS_FRAME_TYPE_COUNT; i++) {
    if (protocolStats.frameTypeCounts[i] > 0) {
      DEBUG_PRINTF("     %s: %lu\n", frameInfo[i].name, protocolStats.frameTypeCounts[i]);
    }
  }
  
  if (protocolStats.lastErrorTime > 0) {
    DEBUG_PRINTF("\n   Last Error: %s (%lu ms ago)\n", 
                 protocolStats.lastError, millis() - protocolStats.lastErrorTime);
  }
  
  DEBUG_PRINTLN("==============================\n");
}

void resetProtocolStatistics() {
  memset(&protocolStats, 0, sizeof(ProtocolStats));
  DEBUG_PRINTLN("📊 Protocol statistics reset");
}

void logFrameParsing(uint8_t nodeId, BMSFrameType_t frameType, bool success, const char* details) {
  if (!protocolLoggingEnabled) return;
  
  const char* status = success ? "✅" : "❌";
  const char* frameName = (frameType < BMS_FRAME_TYPE_COUNT) ? frameInfo[frameType].name : "Unknown";
  
  if (details && strlen(details) > 0) {
    DEBUG_PRINTF("%s BMS%d %s: %s\n", status, nodeId, frameName, details);
  } else {
    DEBUG_PRINTF("%s BMS%d %s parsed\n", status, nodeId, frameName);
  }
}

String getFrameDescription(unsigned long canId) {
  if (isFrame190(canId)) return "Basic battery data (voltage, current, SOC, energy)";
  if (isFrame290(canId)) return "Cell voltages (min, max, average)";
  if (isFrame310(canId)) return "SOH and average temperature";
  if (isFrame390(canId)) return "Maximum allowed charge/discharge limits";
  if (isFrame410(canId)) return "Temperature sensors and ready flags";
  if (isFrame510(canId)) return "Power limits and digital I/O";
  if (isFrame490(canId)) return "Multiplexed data (serial, versions, cycles)";
  if (isFrame1B0(canId)) return "Additional data fields";
  if (isFrame710(canId)) return "CANopen status and timestamp";
  return "Unknown frame type";
}

void dumpFrameData(unsigned long canId, unsigned char len, unsigned char* buf) {
  DEBUG_PRINTF("🔍 Frame Dump 0x%03lX [%d]: ", canId, len);
  for (int i = 0; i < len; i++) {
    DEBUG_PRINTF("%02X ", buf[i]);
  }
  DEBUG_PRINTF("(%s)\n", getFrameDescription(canId).c_str());
}

// === CONFIGURATION FUNCTIONS ===

void configureProtocolLimits(float maxVoltage, float maxCurrent, int16_t maxTemp) {
  // These would typically be stored in a configuration structure
  DEBUG_PRINTF("🔧 Protocol limits configured: %.1fV, %.1fA, %d°C\n", 
               maxVoltage, maxCurrent, maxTemp);
}

void enableProtocolLogging(bool enable) {
  protocolLoggingEnabled = enable;
  DEBUG_PRINTF("🐛 Protocol logging %s\n", enable ? "enabled" : "disabled");
}

void setProtocolTimeout(unsigned long timeoutMs) {
  protocolTimeout = timeoutMs;
  DEBUG_PRINTF("⏰ Protocol timeout set to %lu ms\n", timeoutMs);
}

// === ADVANCED PARSING FUNCTIONS ===

bool parseAndValidateBasicData(uint8_t nodeId, unsigned char* data) {
  BMSData* bms = getBMSData(nodeId);
  if (!bms) return false;
  
  float voltage = convertVoltage(combineBytes(data[0], data[1]));
  float current = convertCurrent(combineBytes(data[2], data[3]));
  float soc = data[6];
  
  if (!validateVoltageRange(voltage) || !validateCurrentRange(current) || !validateSOCRange(soc)) {
    return false;
  }
  
  // Data is valid, update BMS structure
  bms->batteryVoltage = voltage;
  bms->batteryCurrent = current;
  bms->soc = soc;
  bms->remainingEnergy = convertEnergy(combineBytes(data[4], data[5]));
  
  return true;
}

bool parseAndValidateCellData(uint8_t nodeId, unsigned char* data) {
  BMSData* bms = getBMSData(nodeId);
  if (!bms) return false;
  
  float minVoltage = convertCellVoltage(combineBytes(data[0], data[1]));
  float maxVoltage = convertCellVoltage(combineBytes(data[4], data[5]));
  float avgVoltage = convertCellVoltage(combineBytes(data[2], data[3]));
  
  // Validate cell voltage relationships
  if (minVoltage > maxVoltage || avgVoltage < minVoltage || avgVoltage > maxVoltage) {
    return false;
  }
  
  if (minVoltage < 2.0 || maxVoltage > 5.0) {
    return false;
  }
  
  // Data is valid, update BMS structure
  bms->minCellVoltage = minVoltage;
  bms->maxCellVoltage = maxVoltage;
  bms->averageCellVoltage = avgVoltage;
  bms->maxCellVoltageId = data[6];
  bms->minCellVoltageId = data[7];
  bms->deltaCellVoltage = maxVoltage - minVoltage;
  
  return true;
}

bool parseAndValidateTemperatureData(uint8_t nodeId, unsigned char* data) {
  BMSData* bms = getBMSData(nodeId);
  if (!bms) return false;
  
  int16_t temp1 = convertTemperature(combineBytes(data[0], data[1]));
  int16_t temp2 = convertTemperature(combineBytes(data[2], data[3]));
  int16_t temp3 = convertTemperature(combineBytes(data[4], data[5]));
  
  if (!validateTemperatureRange(temp1) || 
      !validateTemperatureRange(temp2) || 
      !validateTemperatureRange(temp3)) {
    return false;
  }
  
  // Data is valid, update BMS structure
  bms->temperature1 = temp1;
  bms->temperature2 = temp2;
  bms->temperature3 = temp3;
  bms->generalReadyFlag = (data[6] & 0x01) != 0;
  bms->chargeReadyFlag = (data[6] & 0x02) != 0;
  
  return true;
}

bool parseAndValidatePowerData(uint8_t nodeId, unsigned char* data) {
  BMSData* bms = getBMSData(nodeId);
  if (!bms) return false;
  
  float chargePower = combineBytes(data[0], data[1]);
  float dischargePower = combineBytes(data[2], data[3]);
  
  // Validate power ranges (assuming max 10kW)
  if (chargePower > 10000 || dischargePower > 10000) {
    return false;
  }
  
  // Data is valid, update BMS structure
  bms->maxChargePower = chargePower;
  bms->maxDischargePower = dischargePower;
  bms->digitalInputs = combineBytes(data[4], data[5]);
  bms->digitalOutputs = combineBytes(data[6], data[7]);
  
  return true;
}/*
 * bms_protocol.cpp - ESP32S3 CAN to Modbus TCP Bridge BMS Protocol Implementation
 * 
 * VERSION: v3.1.0
 * DATE: 2025-08-12
 */

#include "bms_protocol.h"
#include "can_handler.h"
#include "utils.h"

// === GLOBAL VARIABLES ===
ProtocolStats protocolStats = {0};
static bool protocolLoggingEnabled = true;
static unsigned long protocolTimeout = 5000;

// === MAIN PROTOCOL PROCESSING ===

void parseCANFrame(unsigned long canId, unsigned char len, unsigned char* buf) {
  protocolStats.totalFramesParsed++;
  protocolStats.lastParseTime = millis();
  
  if (!validateFrameData(canId, len, buf)) {
    protocolStats.parseErrors++;
    protocolStats.invalidFrames++;
    return;
  }
  
  // Extract node ID and determine frame type
  uint8_t nodeId = 0;
  BMSFrameType_t frameType;
  
  if (isFrame190(canId)) {
    nodeId = extractNodeId(canId, BMS_FRAME_190_BASE);
    frameType = BMS_FRAME_TYPE_190;
    parseBMSFrame190(nodeId, buf);
  } else if (isFrame290(canId)) {
    nodeId = extractNodeId(canId, BMS_FRAME_290_BASE);
    frameType = BMS_FRAME_TYPE_290;
    parseBMSFrame290(nodeId, buf);
  } else if (isFrame310(canId)) {
    nodeId = extractNodeId(canId, BMS_FRAME_310_BASE);
    frameType = BMS_FRAME_TYPE_310;
    parseBMSFrame310(nodeId, buf);
  } else if (isFrame390(canId)) {
    nodeId = extractNodeId(canId, BMS_FRAME_390_BASE);
    frameType = BMS_FRAME_TYPE_390;
    parseBMSFrame390(nodeId, buf);
  } else if (isFrame410(canId)) {
    nodeId = extractNodeId(canId, BMS_FRAME_410_BASE);
    frameType = BMS_FRAME_TYPE_410;
    parseBMSFrame410(nodeId, buf);
  } else if (isFrame510(canId)) {
    nodeId = extractNodeId(canId, BMS_FRAME_510_BASE);
    frameType = BMS_FRAME_TYPE_510;
    parseBMSFrame510(nodeId, buf);
  } else if (isFrame490(canId)) {
    nodeId = extractNodeId(canId, BMS_FRAME_490_BASE);
    frameType = BMS_FRAME_TYPE_490;
    parseBMSFrame490(nodeId, buf);
  } else if (isFrame1B0(canId)) {
    nodeId = extractNodeId(canId, BMS_FRAME_1B0_BASE);
    frameType = BMS_FRAME_TYPE_1B0;
    parseBMSFrame1B0(nodeId, buf);
  } else if (isFrame710(canId)) {
    nodeId = extractNodeId(canId, BMS_FRAME_710_BASE);
    frameType = BMS_FRAME_TYPE_710;
    parseBMSFrame710(nodeId, buf);
  } else {
    protocolStats.invalidFrames++;
    DEBUG_PRINTF("❌ Unknown frame type: 0x%03lX\n", canId);
    return;
  }
  
  // Update statistics
  if (frameType < BMS_FRAME_TYPE_COUNT) {
    protocolStats.frameTypeCounts[frameType]++;
    updateProtocolStatistics(nodeId, frameType, true);
  }
  
  protocolStats.successfulParses++;
  
  if (protocolLoggingEnabled) {
    logFrameParsing(nodeId, frameType, true);
  }
}

bool validateFrameData(unsigned long canId, unsigned char len, unsigned char* buf) {
  if (len != IFS_BMS_FRAME_LENGTH) {
    DEBUG_PRINTF("❌ Invalid frame length: %d (expected %d)\n", len, IFS_BMS_FRAME_LENGTH);
    return false;
  }
  
  if (buf == nullptr) {
    DEBUG_PRINTLN("❌ Null buffer pointer");
    return false;
  }
  
  if (!isValidBMSFrame(canId)) {
    DEBUG_PRINTF("❌ Invalid BMS frame ID: 0x%03lX\n", canId);
    return false;
  }
  
  return true;
}

// === FRAME PARSERS ===

void parseBMSFrame190(uint8_t nodeId, unsigned char* data) {
  if (!isValidNodeIdForFrame(nodeId, BMS_FRAME_190_BASE)) return;
  
  BMSData* bms = getBMSData(nodeId);
  if (!bms) return;
  
  // Parse basic battery data (Frame 0x190)
  bms->batteryVoltage = convertVoltage(combineBytes(data[0], data[1]));
  bms->batteryCurrent = convertCurrent(combineBytes(data[2], data[3]));
  bms->remainingEnergy = convertEnergy(combineBytes(data[4], data[5]));
  bms->soc = data[6]; // 1% resolution
  
  // Parse error flags from byte 7
  parseErrorFlags(data[7], *bms);
  
  // Validate data ranges
  if (!validateVoltageRange(bms->batteryVoltage) || 
      !validateCurrentRange(bms->batteryCurrent) ||
      !validateSOCRange(bms->soc)) {
    handleParseError(nodeId, BMS_FRAME_TYPE_190, "Data out of range");
    return;
  }
  
  updateCommunicationStatus(nodeId);
  updateBMSStatistics(nodeId, BMS_FRAME_TYPE_190);
  
  DEBUG_PRINTF("📊 BMS%d Frame190: %.2fV %.2fA %.1f%% %.1fWh\n", 
               nodeId, bms->batteryVoltage, bms->batteryCurrent, bms->soc, bms->remainingEnergy);
}

void parseBMSFrame290(uint8_t nodeId, unsigned char* data) {
  if (!isValidNodeIdForFrame(nodeId, BMS_FRAME_290_BASE)) return;
  
  BMSData* bms = getBMSData(nodeId);
  if (!bms) return;
  
  // Parse cell voltage data (Frame 0x290)
  bms->minCellVoltage = convertCellVoltage(combineBytes(data[0], data[1]));
  bms->averageCellVoltage = convertCellVoltage(combineBytes(data[2], data[3]));
  bms->maxCellVoltage = convertCellVoltage(combineBytes(data[4], data[5]));
  bms->maxCellVoltageId = data[6];
  bms->minCellVoltageId = data[7];
  
  // Calculate delta
  bms->deltaCellVoltage = bms->maxCellVoltage - bms->minCellVoltage;
  
  // Validate cell voltage ranges
  if (bms->minCellVoltage < 2.5 || bms->maxCellVoltage > 4.5 || 
      bms->deltaCellVoltage > 0.5) {
    handleParseError(nodeId, BMS_FRAME_TYPE_290, "Cell voltages out of range");
    return;
  }
  
  updateCommunicationStatus(nodeId);
  updateBMSStatistics(nodeId, BMS_FRAME_TYPE_290);
  
  DEBUG_PRINTF("🔋 BMS%d Frame290: %.4fV-%.4fV (Δ%.4fV) avg:%.4fV\n", 
               nodeId, bms->minCellVoltage, bms->maxCellVoltage, 
               bms->deltaCellVoltage, bms->averageCellVoltage);
}

void parseBMSFrame310(uint8_t nodeId, unsigned char* data) {
  if (!isValidNodeIdForFrame(nodeId, BMS_FRAME_310_BASE)) return;
  
  BMSData* bms = getBMSData(nodeId);
  if (!bms) return;
  
  // Parse SOH and temperature data (Frame 0x310)
  bms->soh = convertSOH(combineBytes(data[0], data[1]));
  bms->averageTemperature = convertTemperature(combineBytes(data[2], data[3]));
  bms->impedanceValue = convertImpedance(combineBytes(data[4], data[5]));
  bms->impedanceFrequency = combineBytes(data[6], data[7]);
  
  // Validate ranges
  if (!validateSOHRange(bms->soh) || 
      !validateTemperatureRange(bms->averageTemperature)) {
    handleParseError(nodeId, BMS_FRAME_TYPE_310, "SOH/Temperature out of range");
    return;
  }
  
  updateCommunicationStatus(nodeId);
  updateBMSStatistics(nodeId, BMS_FRAME_TYPE_310);
  
  DEBUG_PRINTF("🌡️ BMS%d Frame310: SOH:%.1f%% Temp:%d°C Impedance:%.1fmΩ\n", 
               nodeId, bms->soh, bms->averageTemperature, bms->impedanceValue);
}

void parseBMSFrame390(uint8_t nodeId, unsigned char* data) {
  if (!isValidNodeIdForFrame(nodeId, BMS_FRAME_390_BASE)) return;
  
  BMSData* bms = getBMSData(nodeId);
  if (!bms) return;
  
  // Parse maximum allowed values (Frame 0x390)
  bms->maxAllowedChargeCurrent = convertCurrent(combineBytes(data[0], data[1]));
  bms->maxAllowedDischargeCurrent = convertCurrent(combineBytes(data[2], data[3]));
  bms->maxAllowedChargeVoltage = convertVoltage(combineBytes(data[4], data[5]));
  bms->maxAllowedDischargeVoltage = convertVoltage(combineBytes(data[6], data[7]));
  
  updateCommunicationStatus(nodeId);
  updateBMSStatistics(nodeId, BMS_FRAME_TYPE_390);
  
  DEBUG_PRINTF("⚡ BMS%d Frame390: MaxChg:%.2fA/%.2fV MaxDischg:%.2fA/%.2fV\n", 
               nodeId, bms->maxAllowedChargeCurrent, bms->maxAllowedChargeVoltage,
               bms->maxAllowedDischargeCurrent, bms->maxAllowedDischargeVoltage);
}

void parseBMSFrame410(uint8_t nodeId, unsigned char* data) {
  if (!isValidNodeIdForFrame(nodeId, BMS_FRAME_410_BASE)) return;
  
  BMSData* bms = getBMSData(nodeId);
  if (!bms) return;
  
  // Parse temperature sensors (Frame 0x410)
  bms->temperature1 = convertTemperature(combineBytes(data[0], data[1]));
  bms->temperature2 = convertTemperature(combineBytes(data[2], data[3]));
  bms->temperature3 = convertTemperature(combineBytes(data[4], data[5]));
  
  // Parse ready flags from byte 6
  bms->generalReadyFlag = (data[6] & 0x01) != 0;
  bms->chargeReadyFlag = (data[6] & 0x02) != 0;
  
  updateCommunicationStatus(nodeId);
  updateBMSStatistics(nodeId, BMS_FRAME_TYPE_410);
  
  DEBUG_PRINTF("🌡️ BMS%d Frame410: T1:%d°C T2:%d°C T3:%d°C Ready:%s/%s\n", 
               nodeId, bms->temperature1, bms->temperature2, bms->temperature3,
               bms->generalReadyFlag ? "✅" : "❌", bms->chargeReadyFlag ? "✅" : "❌");
}

void parseBMSFrame510(uint8_t nodeId, unsigned char* data) {
  if (!isValidNodeIdForFrame(nodeId, BMS_FRAME_510_BASE)) return;
  
  BMSData* bms = getBMSData(nodeId);
  if (!bms) return;
  
  // Parse power limits (Frame 0x510)
  bms->maxChargePower = combineBytes(data[0], data[1]); // 1W resolution
  bms->maxDischargePower = combineBytes(data[2], data[3]); // 1W resolution
  bms->digitalInputs = combineBytes(data[4], data[5]);
  bms->digitalOutputs = combineBytes(data[6], data[7]);
  
  updateCommunicationStatus(nodeId);
  updateBMSStatistics(nodeId, BMS_FRAME_TYPE_510);
  
  DEBUG_PRINTF("⚡ BMS%d Frame510: MaxPwr:%.0f/%.0fW IO:0x%04X/0x%04X\n", 
               nodeId, bms->maxChargePower, bms->maxDischargePower,
               bms->digitalInputs, bms->digitalOutputs);
}

void parseBMSFrame490(uint8_t nodeId, unsigned char* data) {
  if (!isValidNodeIdForFrame(nodeId, BMS_FRAME_490_BASE)) return;
  
  BMSData* bms = getBMSData(nodeId);
  if (!bms) return;
  
  // Parse multiplexed data (Frame 0x490)
  bms->mux490Type = data[5];
  bms->mux490Value = combineBytes(data[6], data[7]);
  
  // Process based on multiplexer type
  processMultiplexedData(nodeId, bms->mux490Type, bms->mux490Value);
  
  updateCommunicationStatus(nodeId);
  updateBMSStatistics(nodeId, BMS_FRAME_TYPE_490);
  
  DEBUG_PRINTF("🔄 BMS%d Frame490: MuxType:0x%02X Value:0x%04X\n", 
               nodeId, bms->mux490Type, bms->mux490Value);
}

void parseBMSFrame1B0(uint8_t nodeId, unsigned char* data) {
  if (!isValidNodeIdForFrame(nodeId, BMS_FRAME_1B0_BASE)) return;
  
  BMSData* bms = getBMSData(nodeId);
  if (!bms) return;
  
  // Parse additional data (Frame 0x1B0)
  bms->additionalData[0] = combineBytes(data[0], data[1]);
  bms->additionalData[1] = combineBytes(data[2], data[3]);
  bms->additionalData[2] = combineBytes(data[4], data[5]);
  bms->additionalData[3] = combineBytes(data[6], data[7]);
  
  updateCommunicationStatus(nodeId);
  updateBMSStatistics(nodeId, BMS_FRAME_TYPE_1B0);
  
  DEBUG_PRINTF("📋 BMS%d Frame1B0: Data:[0x%04X 0x%04X 0x%04X 0x%04X]\n", 
               nodeId, bms->additionalData[0], bms->additionalData[1],
               bms->additionalData[2], bms->additionalData[3]);
}

void parseBMSFrame710(uint8_t nodeId, unsigned char* data) {
  if (!isValidNodeIdForFrame(nodeId, BMS_FRAME_710_BASE)) return;
  
  BMSData* bms = getBMSData(nodeId);
  if (!bms) return;
  
  // Parse CANopen status (Frame 0x710)
  bms->canOpenState = data[0];
  bms->canOpenTimestamp = (uint32_t(data[4]) << 24) | (uint32_t(data[5]) << 16) | 
                          (uint32_t(data[6]) << 8) | uint32_t(data[7]);
  
  updateCommunicationStatus(nodeId);
  updateBMSStatistics(nodeId, BMS_FRAME_TYPE_710);
  
  DEBUG_PRINTF("🔗 BMS%d Frame710: CANopen:0x%02X Timestamp:%lu\n", 
               nodeId, bms->canOpenState, bms->canOpenTimestamp);
}

// === DATA CONVERSION UTILITIES ===

float convertVoltage(uint16_t rawValue, float resolution) {
  return rawValue * resolution;
}

float convertCurrent(uint16_t rawValue, float resolution) {
  // Handle signed current (two's complement)
  int16_t signedValue = (int16_t)rawValue;
  return signedValue * resolution;
}

float convertEnergy(uint16_t rawValue, float resolution) {
  return rawValue * resolution;
}

float convertCellVoltage(uint16_t rawValue) {
  return rawValue * IFS_BMS_CELL_VOLTAGE_RESOLUTION;
}

float convertSOH(uint16_t rawValue) {
  return rawValue * IFS_BMS_SOH_RESOLUTION;
}

int16_t convertTemperature(uint16_t rawValue) {
  return (int16_t)rawValue - IFS_BMS_TEMPERATURE_OFFSET;
}

float convertImpedance(uint16_t rawValue) {
  return rawValue * IFS_BMS_IMPEDANCE_RESOLUTION;
}