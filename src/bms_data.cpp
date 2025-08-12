/*
 * bms_data.cpp - ESP32S3 CAN to Modbus TCP Bridge BMS Data Implementation
 * 
 * VERSION: v3.1.0
 * DATE: 2025-08-12
 */

#include "bms_data.h"
#include "utils.h"

// === GLOBAL VARIABLES ===
BMSData bmsModules[MAX_BMS_NODES];

// === FRAME INFORMATION ===
const BMSFrameInfo frameInfo[BMS_FRAME_TYPE_COUNT] = {
  {BMS_FRAME_190_BASE, "Basic Data", "Voltage, Current, SOC, Energy", true, BMS_CRITICAL_TIMEOUT_MS},
  {BMS_FRAME_290_BASE, "Cell Voltages", "Min/Max/Avg cell voltages", true, BMS_CRITICAL_TIMEOUT_MS},
  {BMS_FRAME_310_BASE, "SOH/Temperature", "State of Health, Average Temperature", false, BMS_DATA_TIMEOUT_MS},
  {BMS_FRAME_390_BASE, "Max Limits", "Maximum allowed charge/discharge values", true, BMS_CRITICAL_TIMEOUT_MS},
  {BMS_FRAME_410_BASE, "Temperatures", "Temperature sensor readings", false, BMS_DATA_TIMEOUT_MS},
  {BMS_FRAME_510_BASE, "Power Limits", "Power limits and digital I/O", false, BMS_DATA_TIMEOUT_MS},
  {BMS_FRAME_490_BASE, "Multiplexed", "Serial numbers, versions, cycles", false, BMS_DATA_TIMEOUT_MS},
  {BMS_FRAME_1B0_BASE, "Additional", "Additional data fields", false, BMS_DATA_TIMEOUT_MS},
  {BMS_FRAME_710_BASE, "CANopen", "CANopen status and timestamp", false, BMS_DATA_TIMEOUT_MS}
};

// === INITIALIZATION ===

void initializeBMSData() {
  DEBUG_PRINTLN("🔋 Initializing BMS data structures...");
  
  resetAllBMSData();
  
  DEBUG_PRINTF("✅ BMS data initialized for %d modules\n", MAX_BMS_NODES);
}

void resetBMSData(int index) {
  if (index < 0 || index >= MAX_BMS_NODES) return;
  
  BMSData& bms = bmsModules[index];
  
  // Reset all data to default values
  memset(&bms, 0, sizeof(BMSData));
  
  // Set default values for special cases
  bms.soh = 100.0;  // Default SOH
  bms.averageTemperature = 25;  // Default temperature
  bms.temperature1 = 25;
  bms.temperature2 = 25;
  bms.temperature3 = 25;
  
  // Initialize frame timestamps
  for (int i = 0; i < BMS_FRAME_TYPE_COUNT; i++) {
    bms.lastFrameTime[i] = 0;
  }
  
  bms.communicationOk = false;
  bms.lastUpdate = 0;
  bms.packetsReceived = 0;
  
  DEBUG_PRINTF("   Reset BMS data for index %d\n", index);
}

void resetAllBMSData() {
  for (int i = 0; i < MAX_BMS_NODES; i++) {
    resetBMSData(i);
  }
  DEBUG_PRINTLN("✅ All BMS data structures reset");
}

// === DATA ACCESS ===

int getBatteryIndexFromNodeId(uint8_t nodeId) {
  for (int i = 0; i < systemConfig.activeBmsNodes; i++) {
    if (systemConfig.bmsNodeIds[i] == nodeId) {
      return i;
    }
  }
  return -1;
}

uint8_t getNodeIdFromBatteryIndex(int index) {
  if (index < 0 || index >= systemConfig.activeBmsNodes) {
    return 0;
  }
  return systemConfig.bmsNodeIds[index];
}

BMSData* getBMSData(uint8_t nodeId) {
  int index = getBatteryIndexFromNodeId(nodeId);
  if (index == -1) return nullptr;
  return &bmsModules[index];
}

BMSData* getBMSDataByIndex(int index) {
  if (index < 0 || index >= MAX_BMS_NODES) return nullptr;
  return &bmsModules[index];
}

// === COMMUNICATION STATUS ===

void updateCommunicationStatus(uint8_t nodeId) {
  int index = getBatteryIndexFromNodeId(nodeId);
  if (index == -1) return;
  
  BMSData& bms = bmsModules[index];
  bms.lastUpdate = millis();
  bms.communicationOk = true;
  bms.packetsReceived++;
  
  DEBUG_PRINTF("📡 Updated communication status for BMS%d (packets: %lu)\n", 
               nodeId, bms.packetsReceived);
}

void checkCommunicationTimeouts() {
  unsigned long now = millis();
  
  for (int i = 0; i < systemConfig.activeBmsNodes; i++) {
    BMSData& bms = bmsModules[i];
    uint8_t nodeId = systemConfig.bmsNodeIds[i];
    
    if (bms.communicationOk && (now - bms.lastUpdate > BMS_DATA_TIMEOUT_MS)) {
      bms.communicationOk = false;
      DEBUG_PRINTF("⚠️ BMS%d communication timeout! (last: %lu ms ago)\n", 
                   nodeId, now - bms.lastUpdate);
    }
    
    // Check individual frame timeouts
    for (int frameType = 0; frameType < BMS_FRAME_TYPE_COUNT; frameType++) {
      if (bms.lastFrameTime[frameType] > 0 && 
          frameInfo[frameType].isCritical &&
          (now - bms.lastFrameTime[frameType] > frameInfo[frameType].timeout)) {
        DEBUG_PRINTF("⚠️ BMS%d critical frame %s timeout!\n", 
                     nodeId, frameInfo[frameType].name);
      }
    }
  }
}

int getActiveBatteryCount() {
  int count = 0;
  for (int i = 0; i < systemConfig.activeBmsNodes; i++) {
    if (bmsModules[i].communicationOk) {
      count++;
    }
  }
  return count;
}

bool isBatteryOnline(uint8_t nodeId) {
  BMSData* bms = getBMSData(nodeId);
  return bms != nullptr && bms->communicationOk;
}

// === DATA VALIDATION ===

bool validateBMSData(const BMSData& data) {
  // Sprawdź podstawowe limity
  if (data.batteryVoltage < BMS_MIN_VOLTAGE || data.batteryVoltage > BMS_MAX_VOLTAGE) {
    DEBUG_PRINTF("❌ Invalid battery voltage: %.2fV\n", data.batteryVoltage);
    return false;
  }
  
  if (abs(data.batteryCurrent) > BMS_MAX_CURRENT) {
    DEBUG_PRINTF("❌ Invalid battery current: %.2fA\n", data.batteryCurrent);
    return false;
  }
  
  if (data.soc > 100.0) {
    DEBUG_PRINTF("❌ Invalid SOC: %.1f%%\n", data.soc);
    return false;
  }
  
  if (data.averageTemperature < BMS_MIN_TEMPERATURE || 
      data.averageTemperature > BMS_MAX_TEMPERATURE) {
    DEBUG_PRINTF("❌ Invalid temperature: %d°C\n", data.averageTemperature);
    return false;
  }
  
  return true;
}

bool isDataWithinLimits(const BMSData& data) {
  // Sprawdź czy dane są w bezpiecznych granicach
  if (data.soc < 5.0 || data.soc > 95.0) return false;
  if (data.averageTemperature > 60 || data.averageTemperature < 0) return false;
  if (data.batteryVoltage < 36.0 || data.batteryVoltage > 58.0) return false;
  
  return true;
}

void sanitizeBMSData(BMSData& data) {
  // Ogranicz wartości do bezpiecznych zakresów
  if (data.batteryVoltage < BMS_MIN_VOLTAGE) data.batteryVoltage = BMS_MIN_VOLTAGE;
  if (data.batteryVoltage > BMS_MAX_VOLTAGE) data.batteryVoltage = BMS_MAX_VOLTAGE;
  
  if (data.soc > 100.0) data.soc = 100.0;
  if (data.soc < 0.0) data.soc = 0.0;
  
  if (data.soh > 100.0) data.soh = 100.0;
  if (data.soh < 0.0) data.soh = 0.0;
  
  // Ogranicz temperatury
  if (data.averageTemperature > BMS_MAX_TEMPERATURE) data.averageTemperature = BMS_MAX_TEMPERATURE;
  if (data.averageTemperature < BMS_MIN_TEMPERATURE) data.averageTemperature = BMS_MIN_TEMPERATURE;
}

// === STATISTICS ===

void updateBMSStatistics(uint8_t nodeId, uint8_t frameType) {
  int index = getBatteryIndexFromNodeId(nodeId);
  if (index == -1 || frameType >= BMS_FRAME_TYPE_COUNT) return;
  
  BMSData& bms = bmsModules[index];
  bms.lastFrameTime[frameType] = millis();
}

void printBMSStatistics() {
  DEBUG_PRINTLN("\n📊 === BMS STATISTICS ===");
  
  int totalOnline = getActiveBatteryCount();
  DEBUG_PRINTF("Active Batteries: %d/%d\n", totalOnline, systemConfig.activeBmsNodes);
  
  for (int i = 0; i < systemConfig.activeBmsNodes; i++) {
    uint8_t nodeId = systemConfig.bmsNodeIds[i];
    BMSData& bms = bmsModules[i];
    
    DEBUG_PRINTF("\nBMS%d:", nodeId);
    if (bms.communicationOk) {
      DEBUG_PRINTF(" ✅ ONLINE (packets: %lu)\n", bms.packetsReceived);
      DEBUG_PRINTF("   %.2fV %.2fA %.1f%% SOH:%.1f%% %d°C\n",
                   bms.batteryVoltage, bms.batteryCurrent, bms.soc, 
                   bms.soh, bms.averageTemperature);
      
      if (hasCriticalError(nodeId)) {
        DEBUG_PRINTF("   ⚠️ ERRORS: %s\n", getErrorDescription(nodeId).c_str());
      }
    } else {
      DEBUG_PRINTF(" ❌ OFFLINE\n");
    }
  }
  
  DEBUG_PRINTLN("========================\n");
}

void resetBMSStatistics() {
  for (int i = 0; i < MAX_BMS_NODES; i++) {
    bmsModules[i].packetsReceived = 0;
    for (int j = 0; j < BMS_FRAME_TYPE_COUNT; j++) {
      bmsModules[i].lastFrameTime[j] = 0;
    }
  }
  DEBUG_PRINTLN("📊 BMS statistics reset");
}

// === UTILITY FUNCTIONS ===

String getBMSStatusString(uint8_t nodeId) {
  BMSData* bms = getBMSData(nodeId);
  if (!bms) return "INVALID";
  
  if (!bms->communicationOk) return "OFFLINE";
  if (hasCriticalError(nodeId)) return "ERROR";
  if (!isDataWithinLimits(*bms)) return "WARNING";
  
  return "OK";
}

String formatBMSData(uint8_t nodeId) {
  BMSData* bms = getBMSData(nodeId);
  if (!bms) return "Invalid node ID";
  
  if (!bms->communicationOk) {
    return "BMS" + String(nodeId) + ": " + 
         String(bms->batteryVoltage, 2) + "V " +
         String(bms->batteryCurrent, 2) + "A " +
         String(bms->soc, 1) + "% " +
         String(bms->averageTemperature) + "°C " +
         getBMSStatusString(nodeId);
}

void printBMSData(uint8_t nodeId) {
  DEBUG_PRINTLN(formatBMSData(nodeId));
}

void printAllBMSData() {
  DEBUG_PRINTLN("\n🔋 === ALL BMS DATA ===");
  for (int i = 0; i < systemConfig.activeBmsNodes; i++) {
    uint8_t nodeId = systemConfig.bmsNodeIds[i];
    printBMSData(nodeId);
  }
  DEBUG_PRINTLN("======================\n");
}

// === DATA EXPORT ===

String exportBMSDataToJSON(uint8_t nodeId) {
  BMSData* bms = getBMSData(nodeId);
  if (!bms) return "{}";
  
  String json = "{";
  json += "\"nodeId\":" + String(nodeId) + ",";
  json += "\"online\":" + String(bms->communicationOk ? "true" : "false") + ",";
  json += "\"lastUpdate\":" + String(bms->lastUpdate) + ",";
  json += "\"packetsReceived\":" + String(bms->packetsReceived) + ",";
  
  if (bms->communicationOk) {
    // Basic data
    json += "\"batteryVoltage\":" + String(bms->batteryVoltage, 3) + ",";
    json += "\"batteryCurrent\":" + String(bms->batteryCurrent, 3) + ",";
    json += "\"soc\":" + String(bms->soc, 1) + ",";
    json += "\"soh\":" + String(bms->soh, 1) + ",";
    json += "\"remainingEnergy\":" + String(bms->remainingEnergy, 1) + ",";
    
    // Temperatures
    json += "\"averageTemperature\":" + String(bms->averageTemperature) + ",";
    json += "\"temperature1\":" + String(bms->temperature1) + ",";
    json += "\"temperature2\":" + String(bms->temperature2) + ",";
    json += "\"temperature3\":" + String(bms->temperature3) + ",";
    
    // Cell voltages
    json += "\"minCellVoltage\":" + String(bms->minCellVoltage, 4) + ",";
    json += "\"maxCellVoltage\":" + String(bms->maxCellVoltage, 4) + ",";
    json += "\"averageCellVoltage\":" + String(bms->averageCellVoltage, 4) + ",";
    json += "\"deltaCellVoltage\":" + String(bms->deltaCellVoltage, 4) + ",";
    
    // Limits
    json += "\"maxAllowedChargeCurrent\":" + String(bms->maxAllowedChargeCurrent, 3) + ",";
    json += "\"maxAllowedDischargeCurrent\":" + String(bms->maxAllowedDischargeCurrent, 3) + ",";
    json += "\"maxChargePower\":" + String(bms->maxChargePower, 0) + ",";
    json += "\"maxDischargePower\":" + String(bms->maxDischargePower, 0) + ",";
    
    // Error flags
    json += "\"errors\":{";
    json += "\"master\":" + String(bms->masterError ? "true" : "false") + ",";
    json += "\"cellVoltage\":" + String(bms->cellVoltageError ? "true" : "false") + ",";
    json += "\"cellUnderVoltage\":" + String(bms->cellUnderVoltageError ? "true" : "false") + ",";
    json += "\"cellOverVoltage\":" + String(bms->cellOverVoltageError ? "true" : "false") + ",";
    json += "\"cellImbalance\":" + String(bms->cellImbalanceError ? "true" : "false") + ",";
    json += "\"underTemperature\":" + String(bms->underTemperatureError ? "true" : "false") + ",";
    json += "\"overTemperature\":" + String(bms->overTemperatureError ? "true" : "false") + ",";
    json += "\"overCurrent\":" + String(bms->overCurrentError ? "true" : "false");
    json += "},";
    
    // Status flags
    json += "\"generalReady\":" + String(bms->generalReadyFlag ? "true" : "false") + ",";
    json += "\"chargeReady\":" + String(bms->chargeReadyFlag ? "true" : "false") + ",";
    
    // Multiplexed data
    json += "\"batteryCycles\":" + String(bms->batteryCycles) + ",";
    json += "\"factoryEnergy\":" + String(bms->factoryEnergy, 1);
  }
  
  json += "}";
  return json;
}

String exportAllBMSDataToJSON() {
  String json = "{";
  json += "\"timestamp\":" + String(millis()) + ",";
  json += "\"uptime\":" + String(getUptime()) + ",";
  json += "\"activeBatteries\":" + String(getActiveBatteryCount()) + ",";
  json += "\"totalBatteries\":" + String(systemConfig.activeBmsNodes) + ",";
  json += "\"batteries\":[";
  
  for (int i = 0; i < systemConfig.activeBmsNodes; i++) {
    uint8_t nodeId = systemConfig.bmsNodeIds[i];
    if (i > 0) json += ",";
    json += exportBMSDataToJSON(nodeId);
  }
  
  json += "]}";
  return json;
}

void exportBMSDataToSerial() {
  DEBUG_PRINTLN(exportAllBMSDataToJSON());
}

// === ERROR HANDLING ===

bool hasCriticalError(uint8_t nodeId) {
  BMSData* bms = getBMSData(nodeId);
  if (!bms || !bms->communicationOk) return true;
  
  return bms->masterError || 
         bms->cellVoltageError || 
         bms->cellUnderVoltageError || 
         bms->cellOverVoltageError ||
         bms->overCurrentError ||
         bms->overTemperatureError ||
         bms->underTemperatureError;
}

String getErrorDescription(uint8_t nodeId) {
  BMSData* bms = getBMSData(nodeId);
  if (!bms) return "Invalid node";
  if (!bms->communicationOk) return "Communication lost";
  
  String errors = "";
  if (bms->masterError) errors += "Master ";
  if (bms->cellVoltageError) errors += "CellVoltage ";
  if (bms->cellUnderVoltageError) errors += "UnderVoltage ";
  if (bms->cellOverVoltageError) errors += "OverVoltage ";
  if (bms->cellImbalanceError) errors += "Imbalance ";
  if (bms->underTemperatureError) errors += "ColdTemp ";
  if (bms->overTemperatureError) errors += "HotTemp ";
  if (bms->overCurrentError) errors += "OverCurrent ";
  
  return errors.length() > 0 ? errors : "None";
}

uint8_t getErrorFlags(uint8_t nodeId) {
  BMSData* bms = getBMSData(nodeId);
  if (!bms) return 0xFF;
  
  uint8_t flags = 0;
  if (bms->masterError) flags |= 0x01;
  if (bms->cellVoltageError) flags |= 0x02;
  if (bms->cellUnderVoltageError) flags |= 0x04;
  if (bms->cellOverVoltageError) flags |= 0x08;
  if (bms->cellImbalanceError) flags |= 0x10;
  if (bms->underTemperatureError) flags |= 0x20;
  if (bms->overTemperatureError) flags |= 0x40;
  if (bms->overCurrentError) flags |= 0x80;
  
  return flags;
}

// === ADVANCED FUNCTIONS ===

void updateFrameTimestamp(uint8_t nodeId, BMSFrameType_t frameType) {
  int index = getBatteryIndexFromNodeId(nodeId);
  if (index == -1 || frameType >= BMS_FRAME_TYPE_COUNT) return;
  
  bmsModules[index].lastFrameTime[frameType] = millis();
}

bool isFrameTimedOut(uint8_t nodeId, BMSFrameType_t frameType) {
  BMSData* bms = getBMSData(nodeId);
  if (!bms || frameType >= BMS_FRAME_TYPE_COUNT) return true;
  
  if (bms->lastFrameTime[frameType] == 0) return false; // Never received
  
  unsigned long timeout = frameInfo[frameType].timeout;
  return (millis() - bms->lastFrameTime[frameType]) > timeout;
}

void printFrameStatistics(uint8_t nodeId) {
  BMSData* bms = getBMSData(nodeId);
  if (!bms) return;
  
  DEBUG_PRINTF("\n📊 Frame Statistics for BMS%d:\n", nodeId);
  
  for (int i = 0; i < BMS_FRAME_TYPE_COUNT; i++) {
    unsigned long lastTime = bms->lastFrameTime[i];
    const char* status;
    
    if (lastTime == 0) {
      status = "Never received";
    } else {
      unsigned long age = millis() - lastTime;
      if (age > frameInfo[i].timeout) {
        status = "TIMEOUT";
      } else {
        status = "OK";
      }
    }
    
    DEBUG_PRINTF("   %s: %s", frameInfo[i].name, status);
    if (lastTime > 0) {
      DEBUG_PRINTF(" (age: %lu ms)", millis() - lastTime);
    }
    DEBUG_PRINTLN();
  }
}

float calculatePackVoltage() {
  float totalVoltage = 0.0;
  int count = 0;
  
  for (int i = 0; i < systemConfig.activeBmsNodes; i++) {
    if (bmsModules[i].communicationOk) {
      totalVoltage += bmsModules[i].batteryVoltage;
      count++;
    }
  }
  
  return count > 0 ? totalVoltage : 0.0;
}

float calculatePackCurrent() {
  float totalCurrent = 0.0;
  int count = 0;
  
  for (int i = 0; i < systemConfig.activeBmsNodes; i++) {
    if (bmsModules[i].communicationOk) {
      totalCurrent += bmsModules[i].batteryCurrent;
      count++;
    }
  }
  
  return count > 0 ? totalCurrent / count : 0.0; // Average current
}

float calculatePackSOC() {
  float totalSOC = 0.0;
  int count = 0;
  
  for (int i = 0; i < systemConfig.activeBmsNodes; i++) {
    if (bmsModules[i].communicationOk) {
      totalSOC += bmsModules[i].soc;
      count++;
    }
  }
  
  return count > 0 ? totalSOC / count : 0.0;
}

void performBMSHealthCheck() {
  DEBUG_PRINTLN("\n🏥 === BMS HEALTH CHECK ===");
  
  int totalBatteries = systemConfig.activeBmsNodes;
  int onlineBatteries = getActiveBatteryCount();
  int errorBatteries = 0;
  
  for (int i = 0; i < systemConfig.activeBmsNodes; i++) {
    uint8_t nodeId = systemConfig.bmsNodeIds[i];
    if (bmsModules[i].communicationOk && hasCriticalError(nodeId)) {
      errorBatteries++;
    }
  }
  
  DEBUG_PRINTF("📊 Overall Status:\n");
  DEBUG_PRINTF("   Total Batteries: %d\n", totalBatteries);
  DEBUG_PRINTF("   Online: %d (%.1f%%)\n", onlineBatteries, 
               (float)onlineBatteries / totalBatteries * 100.0);
  DEBUG_PRINTF("   With Errors: %d\n", errorBatteries);
  DEBUG_PRINTF("   Pack Voltage: %.1fV\n", calculatePackVoltage());
  DEBUG_PRINTF("   Pack Current: %.1fA\n", calculatePackCurrent());
  DEBUG_PRINTF("   Pack SOC: %.1f%%\n", calculatePackSOC());
  
  if (errorBatteries > 0) {
    DEBUG_PRINTLN("\n⚠️ Batteries with errors:");
    for (int i = 0; i < systemConfig.activeBmsNodes; i++) {
      uint8_t nodeId = systemConfig.bmsNodeIds[i];
      if (bmsModules[i].communicationOk && hasCriticalError(nodeId)) {
        DEBUG_PRINTF("   BMS%d: %s\n", nodeId, getErrorDescription(nodeId).c_str());
      }
    }
  }
  
  DEBUG_PRINTLN("==========================\n");
} OFFLINE";
  }
  
  return "BMS" + String(nodeId) + ":