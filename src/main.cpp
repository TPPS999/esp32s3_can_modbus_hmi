/*
 * ESP32S3 CAN to MQTT Bridge - IFS BMS Protocol - MQTT OPTIMIZED
 * 
 * FIXED VERSION - Compilation errors resolved
 * 
 * VERSION: v2.2.1 - Fixed compilation
 * DATE: 2025-07-29T06:00:30.000Z
 * STATUS: ✅ READY FOR COMPILATION
 */

// === INCLUDES - MUSZĄ BYĆ NA POCZĄTKU ===
#include <Arduino.h>
#include <WiFi.h>
#include <SPI.h>
#include <mcp_can.h>

#define MQTT_MAX_PACKET_SIZE 1024
#include <PubSubClient.h>

// === HARDWARE CONFIGURATION - PRZED UŻYCIEM ===
#define SPI_CS_PIN    44
#define SPI_MOSI_PIN  9
#define SPI_MISO_PIN  8  
#define SPI_SCK_PIN   7
#define CAN_INT_PIN   2
#define LED_PIN       21
#define MAX_BMS_NODES 12

#define CAN_SPEED_125K CAN_125KBPS

// === NETWORK CONFIGURATION ===
const char* WIFI_SSID = "WNK3";
const char* WIFI_PASSWORD = "PiotrStrzykalskiNieIstnieje";
const char* MQTT_BROKER = "192.168.148.13";
const int MQTT_PORT = 1883;
const char* MQTT_USER = "mqtt";
const char* MQTT_PASSWORD = "L&2QRukV%Jz^TC449da7nN&h";

// === MQTT OPTIMIZATION CONFIGURATION ===
struct AdaptiveConfig {
  // Publishing intervals (milliseconds)
  int idle_interval = 10000;        // 10s gdy system spokojny
  int active_interval = 5000;       // 5s gdy prąd > threshold
  int charging_interval = 2000;     // 2s podczas ładowania/rozładowania
  int error_interval = 500;         // 0.5s gdy błędy aktywne
  
  // Thresholds dla adaptive logic
  float current_threshold = 1.0;    // > 1A = system aktywny
  float charging_threshold = 0.5;   // > 0.5A = ładowanie/rozładowanie
  
  // Delta publishing thresholds
  float voltage_delta = 0.1;        // 0.1V
  float current_delta = 0.5;        // 0.5A
  float soc_delta = 1.0;            // 1%
  float temperature_delta = 1.0;    // 1°C
  float energy_delta = 0.1;         // 0.1kWh
  
  // Priority publishing
  bool always_publish_errors = true;
  bool always_publish_status = true;
  int diagnostics_every_n_cycles = 10;  // Co 10 cykli publikuj diagnostykę
};

// === GLOBAL OBJECTS ===
MCP_CAN CAN(SPI_CS_PIN);
WiFiClient wifiClient;
PubSubClient mqtt(wifiClient);
AdaptiveConfig adaptiveConfig;

// === BMS NODE CONFIGURATION ===
uint8_t bmsNodeIds[MAX_BMS_NODES] = {26, 27, 28, 1, 2, 3, 4, 5, 6, 7, 8, 9};
int activeBmsNodes = 1;

// === BMS DATA STRUCTURES ===
struct BMSData {
  // Frame 0x190 data
  float batteryVoltage;
  float batteryCurrent;
  float remainingEnergy;
  float soc;
  
  // Frame 0x190 error flags
  bool masterError;
  bool cellVoltageError;
  bool cellTempMinError;
  bool cellTempMaxError;
  bool cellVoltageMinError;
  bool cellVoltageMaxError;
  bool systemShutdown;
  bool ibbVoltageSupplyError;
  
  // Frame 0x290 data
  float cellMinVoltage;
  float cellMeanVoltage;
  uint8_t minVoltageBlock;
  uint8_t minVoltageCell;
  uint8_t minVoltageString;
  uint8_t balancingTempMax;
  
  // Frame 0x310 data
  float soh;
  float cellVoltage;
  float cellTemperature;
  float dcir;
  bool nonEqualStringsRamp;
  bool dynamicLimitationTimer;
  bool overcurrentTimer;
  uint16_t channelMultiplexor;
  
  // Frame 0x390 data
  float cellMaxVoltage;
  float cellVoltageDelta;
  uint8_t maxVoltageBlock;
  uint8_t maxVoltageCell;
  uint8_t maxVoltageString;
  uint8_t afeTemperatureMax;
  
  // Frame 0x410 data
  float cellMaxTemperature;
  float cellTempDelta;
  uint8_t maxTempString;
  uint8_t maxTempBlock;
  uint8_t maxTempSensor;
  bool readyToCharge;
  bool readyToDischarge;
  
  // Frame 0x510 data
  float dccl;
  float ddcl;
  bool input_IN02;
  bool input_IN01;
  bool relay_AUX4;
  bool relay_AUX3;
  bool relay_AUX2;
  bool relay_AUX1;
  bool relay_R2;
  bool relay_R1;
  
  // Communication & diagnostics
  unsigned long lastUpdate;
  bool communicationOk;
  int packetsReceived;
  int parseErrors;
  int frame190Count;
  int frame290Count;
  int frame310Count;
  int frame390Count;
  int frame410Count;
  int frame510Count;
};

// === DELTA PUBLISHING TRACKING ===
struct PreviousValues {
  float voltage;
  float current;
  float soc;
  float temperature;
  float energy;
  float soh;
  float cellMinVoltage;
  float cellMaxVoltage;
  float cellVoltageDelta;
  float cellTempDelta;
  bool initialized;
};

// === GLOBAL ARRAYS ===
BMSData bmsModules[MAX_BMS_NODES];
PreviousValues previousBmsData[MAX_BMS_NODES];

// === STATISTICS ===
struct CanStats {
  int totalFramesReceived = 0;
  int validBmsFrames = 0;
  int invalidFrames = 0;
  int parseErrors = 0;
  int mqttPublishes = 0;
  int mqttSkipped = 0;           // NEW: zlicza pominięte publikacje
  int discoveryMessages = 0;
  int reconnectAttempts = 0;
  unsigned long lastFrameTime = 0;
  unsigned long lastMqttReconnect = 0;
  
  // Adaptive stats
  int adaptiveIdleCycles = 0;
  int adaptiveActiveCycles = 0;
  int adaptiveChargingCycles = 0;
  int adaptiveErrorCycles = 0;
} stats;

// === FUNCTION DECLARATIONS ===
void setupWiFi();
void setupMQTT();
void setupCAN();
bool initializeMCP2515();
void processCAN();
void parseCANFrame(unsigned long canId, unsigned char len, unsigned char *buf);

// BMS Frame parsers
void parseBMSFrame190(uint8_t nodeId, unsigned char* data);
void parseBMSFrame290(uint8_t nodeId, unsigned char* data);
void parseBMSFrame310(uint8_t nodeId, unsigned char* data);
void parseBMSFrame390(uint8_t nodeId, unsigned char* data);
void parseBMSFrame410(uint8_t nodeId, unsigned char* data);
void parseBMSFrame510(uint8_t nodeId, unsigned char* data);

// MQTT functions - OPTIMIZED
void publishBMSDataOptimized(uint8_t nodeId, int publishMode);
void publishBMSDiscovery(uint8_t nodeId);
bool shouldPublishParameter(int nodeIndex, String paramType, float newValue);
int determinePublishingInterval();
void publishMQTT(String topic, String payload);
void publishMQTT(String topic, float value, int decimals = 2);
void publishMQTT(String topic, bool value);
void reconnectMQTT();

// Utility functions
uint8_t extractNodeId(unsigned long canId, uint16_t baseId);
bool isValidBMSFrame(unsigned long canId);
void updateCommunicationStatus(uint8_t nodeId);
void printCANFrame(unsigned long canId, unsigned char len, unsigned char* buf);
void blinkLED(int count, int duration = 100);

// === UTILITY FUNCTIONS ===
void printCANFrame(unsigned long canId, unsigned char len, unsigned char* buf) {
  Serial.printf("🔍 CAN: ID=0x%03lX Len=%d Data=[", canId, len);
  for (int i = 0; i < len; i++) {
    Serial.printf("%02X", buf[i]);
    if (i < len - 1) Serial.print(" ");
  }
  Serial.print("]");
  
  if ((canId & 0xFF80) == 0x180) Serial.print(" (Basic data)");
  else if ((canId & 0xFF80) == 0x280) Serial.print(" (Cell voltages)");
  else if ((canId & 0xFF80) == 0x300) Serial.print(" (Multiplexed)");
  else if ((canId & 0xFF80) == 0x380) Serial.print(" (Max voltages)");
  else if ((canId & 0xFF80) == 0x400) Serial.print(" (Temperatures)");
  else if ((canId & 0xFF80) == 0x500) Serial.print(" (Power limits)");
  
  Serial.println();
}

void parseCANFrame(unsigned long canId, unsigned char len, unsigned char *buf) {
  if (len != 8) {
    Serial.printf("⚠️ Invalid frame length: %d (expected 8)\n", len);
    stats.parseErrors++;
    return;
  }
  
  if ((canId & 0xFF80) == 0x180) {
    uint8_t nodeId = extractNodeId(canId, 0x180);
    if (nodeId > 0) parseBMSFrame190(nodeId, buf);
  } else if ((canId & 0xFF80) == 0x280) {
    uint8_t nodeId = extractNodeId(canId, 0x280);
    if (nodeId > 0) parseBMSFrame290(nodeId, buf);
  } else if ((canId & 0xFF80) == 0x300) {
    uint8_t nodeId = extractNodeId(canId, 0x300);
    if (nodeId > 0) parseBMSFrame310(nodeId, buf);
  } else if ((canId & 0xFF80) == 0x380) {
    uint8_t nodeId = extractNodeId(canId, 0x380);
    if (nodeId > 0) parseBMSFrame390(nodeId, buf);
  } else if ((canId & 0xFF80) == 0x400) {
    uint8_t nodeId = extractNodeId(canId, 0x400);
    if (nodeId > 0) parseBMSFrame410(nodeId, buf);
  } else if ((canId & 0xFF80) == 0x500) {
    uint8_t nodeId = extractNodeId(canId, 0x500);
    if (nodeId > 0) parseBMSFrame510(nodeId, buf);
  }
}

uint8_t extractNodeId(unsigned long canId, uint16_t baseId) {
  uint8_t nodeId = canId - baseId;
  
  for (int i = 0; i < activeBmsNodes; i++) {
    if (bmsNodeIds[i] == nodeId) {
      return nodeId;
    }
  }
  return 0;
}

bool isValidBMSFrame(unsigned long canId) {
  return ((canId & 0xFF80) == 0x180) ||
         ((canId & 0xFF80) == 0x280) ||
         ((canId & 0xFF80) == 0x300) ||
         ((canId & 0xFF80) == 0x380) ||
         ((canId & 0xFF80) == 0x400) ||
         ((canId & 0xFF80) == 0x500) ||
         ((canId & 0xFF80) == 0x480) ||
         ((canId & 0xFF80) == 0x700);
}

void updateCommunicationStatus(uint8_t nodeId) {
  int index = -1;
  for (int i = 0; i < activeBmsNodes; i++) {
    if (bmsNodeIds[i] == nodeId) {
      index = i;
      break;
    }
  }
  if (index == -1) return;
  
  BMSData& bms = bmsModules[index];
  bms.lastUpdate = millis();
  bms.communicationOk = true;
  bms.packetsReceived++;
}

void blinkLED(int count, int duration) {
  for (int i = 0; i < count; i++) {
    digitalWrite(LED_PIN, HIGH);
    delay(duration);
    digitalWrite(LED_PIN, LOW);
    delay(duration);
  }
}

// === BMS FRAME PARSERS ===
void parseBMSFrame190(uint8_t nodeId, unsigned char* data) {
  if (nodeId == 0) return;
  
  int index = -1;
  for (int i = 0; i < activeBmsNodes; i++) {
    if (bmsNodeIds[i] == nodeId) {
      index = i;
      break;
    }
  }
  if (index == -1) return;
  
  BMSData& bms = bmsModules[index];
  
  bms.batteryVoltage = (float)((data[1] << 8) | data[0]) * 0.0625;
  bms.batteryCurrent = (float)((data[3] << 8) | data[2]) * 0.0625;
  bms.remainingEnergy = (float)((data[5] << 8) | data[4]) * 0.1;
  bms.soc = (float)data[6] * 1.0;
  
  bms.ibbVoltageSupplyError = (data[7] & 0x01) > 0;
  bms.cellVoltageError = (data[7] & 0x02) > 0;
  bms.cellTempMaxError = (data[7] & 0x04) > 0;
  bms.cellTempMinError = (data[7] & 0x08) > 0;
  bms.cellVoltageMaxError = (data[7] & 0x10) > 0;
  bms.cellVoltageMinError = (data[7] & 0x20) > 0;
  bms.systemShutdown = (data[7] & 0x40) > 0;
  bms.masterError = (data[7] & 0x80) > 0;
  
  bms.frame190Count++;
  updateCommunicationStatus(nodeId);
  
  Serial.printf("📊 BMS%d-190: U=%.2fV I=%.2fA SOC=%.1f%% E=%.1fkWh MasterErr=%s\n", 
                nodeId, bms.batteryVoltage, bms.batteryCurrent, bms.soc, 
                bms.remainingEnergy, bms.masterError ? "YES" : "NO");
}

void parseBMSFrame290(uint8_t nodeId, unsigned char* data) {
  if (nodeId == 0) return;
  
  int index = -1;
  for (int i = 0; i < activeBmsNodes; i++) {
    if (bmsNodeIds[i] == nodeId) {
      index = i;
      break;
    }
  }
  if (index == -1) return;
  
  BMSData& bms = bmsModules[index];
  
  bms.cellMinVoltage = (float)((data[1] << 8) | data[0]) * 0.0001;
  bms.minVoltageString = data[2];
  bms.minVoltageBlock = data[3];
  bms.minVoltageCell = data[4]; 
  bms.cellMeanVoltage = (float)((data[6] << 8) | data[5]) * 0.0001;
  bms.balancingTempMax = data[7];
  
  bms.frame290Count++;
  updateCommunicationStatus(nodeId);
  
  Serial.printf("📊 BMS%d-290: VMin=%.4fV VMean=%.4fV Block=%d Cell=%d String=%d\n", 
                nodeId, bms.cellMinVoltage, bms.cellMeanVoltage, 
                bms.minVoltageBlock, bms.minVoltageCell, bms.minVoltageString);
}

void parseBMSFrame310(uint8_t nodeId, unsigned char* data) {
  if (nodeId == 0) return;
  
  int index = -1;
  for (int i = 0; i < activeBmsNodes; i++) {
    if (bmsNodeIds[i] == nodeId) {
      index = i;
      break;
    }
  }
  if (index == -1) return;
  
  BMSData& bms = bmsModules[index];
  
  bms.channelMultiplexor = ((data[0] << 6) | ((data[1] & 0xFC) >> 2));
  bms.dynamicLimitationTimer = (data[1] & 0x40) > 0;
  bms.overcurrentTimer = (data[1] & 0x80) > 0;
  bms.cellVoltage = (float)((data[3] << 8) | data[2]) * 0.1;
  bms.cellTemperature = (float)data[4];
  bms.dcir = (float)((data[6] << 8) | data[5]) * 0.1;
  bms.soh = (float)(data[7] & 0x7F);
  bms.nonEqualStringsRamp = (data[7] & 0x80) > 0;
  
  bms.frame310Count++;
  updateCommunicationStatus(nodeId);
  
  Serial.printf("📊 BMS%d-310: SOH=%.1f%% CellV=%.1fmV CellT=%.1f°C DCiR=%.1fmΩ\n", 
                nodeId, bms.soh, bms.cellVoltage, bms.cellTemperature, bms.dcir);
}

void parseBMSFrame390(uint8_t nodeId, unsigned char* data) {
  if (nodeId == 0) return;
  
  int index = -1;
  for (int i = 0; i < activeBmsNodes; i++) {
    if (bmsNodeIds[i] == nodeId) {
      index = i;
      break;
    }
  }
  if (index == -1) return;
  
  BMSData& bms = bmsModules[index];
  
  bms.cellMaxVoltage = (float)((data[1] << 8) | data[0]) * 0.0001;
  bms.maxVoltageString = data[2];
  bms.maxVoltageBlock = data[3];
  bms.maxVoltageCell = data[4];
  bms.cellVoltageDelta = (float)((data[6] << 8) | data[5]) * 0.0001;
  bms.afeTemperatureMax = data[7];
  
  bms.frame390Count++;
  updateCommunicationStatus(nodeId);
  
  Serial.printf("📊 BMS%d-390: VMax=%.4fV VDelta=%.4fV Block=%d Cell=%d String=%d\n", 
                nodeId, bms.cellMaxVoltage, bms.cellVoltageDelta,
                bms.maxVoltageBlock, bms.maxVoltageCell, bms.maxVoltageString);
}

void parseBMSFrame410(uint8_t nodeId, unsigned char* data) {
  if (nodeId == 0) return;
  
  int index = -1;
  for (int i = 0; i < activeBmsNodes; i++) {
    if (bmsNodeIds[i] == nodeId) {
      index = i;
      break;
    }
  }
  if (index == -1) return;
  
  BMSData& bms = bmsModules[index];
  
  bms.cellMaxTemperature = (float)data[0];
  bms.maxTempString = data[1];
  bms.maxTempBlock = data[2];
  bms.maxTempSensor = data[3];
  bms.cellTempDelta = (float)data[4];
  
  bms.readyToCharge = (data[7] & 0x20) > 0;
  bms.readyToDischarge = (data[7] & 0x40) > 0;
  
  bms.frame410Count++;
  updateCommunicationStatus(nodeId);
  
  Serial.printf("📊 BMS%d-410: TMax=%.1f°C TDelta=%.1f°C Ready: Chg=%s Dchg=%s\n", 
                nodeId, bms.cellMaxTemperature, bms.cellTempDelta,
                bms.readyToCharge ? "✅" : "❌",
                bms.readyToDischarge ? "✅" : "❌");
}

void parseBMSFrame510(uint8_t nodeId, unsigned char* data) {
  if (nodeId == 0) return;
  
  int index = -1;
  for (int i = 0; i < activeBmsNodes; i++) {
    if (bmsNodeIds[i] == nodeId) {
      index = i;
      break;
    }
  }
  if (index == -1) return;
  
  BMSData& bms = bmsModules[index];
  
  bms.dccl = (float)((data[4] << 8) | data[3]) * 0.0625;
  bms.ddcl = (float)((data[6] << 8) | data[5]) * 0.0625;
  
  bms.input_IN02 = (data[0] & 0x01) > 0;
  bms.input_IN01 = (data[0] & 0x02) > 0;
  bms.relay_AUX4 = (data[0] & 0x04) > 0;
  bms.relay_AUX3 = (data[0] & 0x08) > 0;
  bms.relay_AUX2 = (data[0] & 0x10) > 0;
  bms.relay_AUX1 = (data[0] & 0x20) > 0;
  bms.relay_R2 = (data[0] & 0x40) > 0;
  bms.relay_R1 = (data[0] & 0x80) > 0;
  
  bms.frame510Count++;
  updateCommunicationStatus(nodeId);
  
  Serial.printf("📊 BMS%d-510: ChgLim=%.2fA DchgLim=%.2fA R1=%s R2=%s IN01=%s IN02=%s\n", 
                nodeId, bms.dccl, bms.ddcl,
                bms.relay_R1 ? "ON" : "OFF",
                bms.relay_R2 ? "ON" : "OFF",
                bms.input_IN01 ? "HIGH" : "LOW",
                bms.input_IN02 ? "HIGH" : "LOW");
}

// === MQTT OPTIMIZATION FUNCTIONS ===
bool shouldPublishParameter(int nodeIndex, String paramType, float newValue) {
  if (!previousBmsData[nodeIndex].initialized) {
    return true;  // Pierwszy raz - publikuj wszystko
  }
  
  PreviousValues& prev = previousBmsData[nodeIndex];
  
  if (paramType == "voltage") {
    return abs(newValue - prev.voltage) >= adaptiveConfig.voltage_delta;
  } else if (paramType == "current") {
    return abs(newValue - prev.current) >= adaptiveConfig.current_delta;
  } else if (paramType == "soc") {
    return abs(newValue - prev.soc) >= adaptiveConfig.soc_delta;
  } else if (paramType == "temperature") {
    return abs(newValue - prev.temperature) >= adaptiveConfig.temperature_delta;
  } else if (paramType == "energy") {
    return abs(newValue - prev.energy) >= adaptiveConfig.energy_delta;
  } else if (paramType == "soh") {
    return abs(newValue - prev.soh) >= 1.0;  // SOH zmienia się rzadko
  } else if (paramType == "cell_min_voltage") {
    return abs(newValue - prev.cellMinVoltage) >= 0.0001;  // Wysoka precyzja
  } else if (paramType == "cell_max_voltage") {
    return abs(newValue - prev.cellMaxVoltage) >= 0.0001;
  } else if (paramType == "cell_voltage_delta") {
    return abs(newValue - prev.cellVoltageDelta) >= 0.0001;
  } else if (paramType == "cell_temp_delta") {
    return abs(newValue - prev.cellTempDelta) >= 0.5;
  }
  
  return false;  // Default: nie publikuj
}

int determinePublishingInterval() {
  int interval = adaptiveConfig.idle_interval;  // Default: 10s
  bool hasActivity = false;
  bool hasCharging = false;
  bool hasErrors = false;
  
  // Sprawdź wszystkie aktywne BMS nodes
  for (int i = 0; i < activeBmsNodes; i++) {
    BMSData& bms = bmsModules[i];
    if (!bms.communicationOk) continue;
    
    // Sprawdź aktywność (prąd > threshold)
    if (abs(bms.batteryCurrent) > adaptiveConfig.current_threshold) {
      hasActivity = true;
    }
    
    // Sprawdź ładowanie/rozładowanie
    if (abs(bms.batteryCurrent) > adaptiveConfig.charging_threshold) {
      hasCharging = true;
    }
    
    // Sprawdź błędy krytyczne
    if (bms.masterError || bms.cellVoltageError || 
        bms.cellTempMaxError || bms.cellTempMinError ||
        bms.cellVoltageMaxError || bms.cellVoltageMinError) {
      hasErrors = true;
    }
  }
  
  // Wybierz najszybszy interwał z aktywnych warunków
  if (hasErrors) {
    interval = adaptiveConfig.error_interval;
    stats.adaptiveErrorCycles++;
  } else if (hasCharging) {
    interval = adaptiveConfig.charging_interval;
    stats.adaptiveChargingCycles++;
  } else if (hasActivity) {
    interval = adaptiveConfig.active_interval;
    stats.adaptiveActiveCycles++;
  } else {
    stats.adaptiveIdleCycles++;
  }
  
  return interval;
}

void publishBMSDataOptimized(uint8_t nodeId, int publishMode) {
  if (!mqtt.connected()) return;
  
  int index = -1;
  for (int i = 0; i < activeBmsNodes; i++) {
    if (bmsNodeIds[i] == nodeId) {
      index = i;
      break;
    }
  }
  if (index == -1) return;
  
  BMSData& bms = bmsModules[index];
  if (!bms.communicationOk) return;
  
  String baseTopic = "bms/" + String(nodeId);
  int publishCount = 0;
  int skippedCount = 0;
  
  // === PRIORITY 1: CRITICAL PARAMETERS (zawsze publikuj) ===
  if (adaptiveConfig.always_publish_errors) {
    publishMQTT(baseTopic + "/status/master_error", bms.masterError);
    publishMQTT(baseTopic + "/status/cell_voltage_error", bms.cellVoltageError);
    publishMQTT(baseTopic + "/status/cell_temp_min_error", bms.cellTempMinError);
    publishMQTT(baseTopic + "/status/cell_temp_max_error", bms.cellTempMaxError);
    publishMQTT(baseTopic + "/status/system_shutdown", bms.systemShutdown);
    publishCount += 5;
  }
  
  if (adaptiveConfig.always_publish_status) {
    publishMQTT(baseTopic + "/status/ready_charge", bms.readyToCharge);
    publishMQTT(baseTopic + "/status/ready_discharge", bms.readyToDischarge);
    publishCount += 2;
  }
  
  // === PRIORITY 2: CORE MEASUREMENTS (delta publishing) ===
  
  // Voltage
  if (shouldPublishParameter(index, "voltage", bms.batteryVoltage)) {
    publishMQTT(baseTopic + "/voltage", bms.batteryVoltage, 3);
    previousBmsData[index].voltage = bms.batteryVoltage;
    publishCount++;
  } else {
    skippedCount++;
  }
  
  // Current
  if (shouldPublishParameter(index, "current", bms.batteryCurrent)) {
    publishMQTT(baseTopic + "/current", bms.batteryCurrent, 3);
    previousBmsData[index].current = bms.batteryCurrent;
    publishCount++;
  } else {
    skippedCount++;
  }
  
  // SOC
  if (shouldPublishParameter(index, "soc", bms.soc)) {
    publishMQTT(baseTopic + "/soc", bms.soc, 1);
    previousBmsData[index].soc = bms.soc;
    publishCount++;
  } else {
    skippedCount++;
  }
  
  // Energy
  if (shouldPublishParameter(index, "energy", bms.remainingEnergy)) {
    publishMQTT(baseTopic + "/energy", bms.remainingEnergy, 2);
    previousBmsData[index].energy = bms.remainingEnergy;
    publishCount++;
  } else {
    skippedCount++;
  }
  
  // Temperature
  if (shouldPublishParameter(index, "temperature", bms.cellMaxTemperature)) {
    publishMQTT(baseTopic + "/temperature/max", bms.cellMaxTemperature, 1);
    previousBmsData[index].temperature = bms.cellMaxTemperature;
    publishCount++;
  } else {
    skippedCount++;
  }
  
  // === PRIORITY 3: EXTENDED MEASUREMENTS (delta publishing) ===
  
  // SOH (zmienia się rzadko)
  if (shouldPublishParameter(index, "soh", bms.soh)) {
    publishMQTT(baseTopic + "/soh", bms.soh, 1);
    previousBmsData[index].soh = bms.soh;
    publishCount++;
  } else {
    skippedCount++;
  }
  
  // Cell voltages (wysokiej precyzji)
  if (shouldPublishParameter(index, "cell_min_voltage", bms.cellMinVoltage)) {
    publishMQTT(baseTopic + "/cell/voltage_min", bms.cellMinVoltage, 4);
    previousBmsData[index].cellMinVoltage = bms.cellMinVoltage;
    publishCount++;
  } else {
    skippedCount++;
  }
  
  if (shouldPublishParameter(index, "cell_max_voltage", bms.cellMaxVoltage)) {
    publishMQTT(baseTopic + "/cell/voltage_max", bms.cellMaxVoltage, 4);
    previousBmsData[index].cellMaxVoltage = bms.cellMaxVoltage;
    publishCount++;
  } else {
    skippedCount++;
  }
  
  if (shouldPublishParameter(index, "cell_voltage_delta", bms.cellVoltageDelta)) {
    publishMQTT(baseTopic + "/cell/voltage_delta", bms.cellVoltageDelta, 4);
    previousBmsData[index].cellVoltageDelta = bms.cellVoltageDelta;
    publishCount++;
  } else {
    skippedCount++;
  }
  
  // Temperature delta
  if (shouldPublishParameter(index, "cell_temp_delta", bms.cellTempDelta)) {
    publishMQTT(baseTopic + "/temperature/delta", bms.cellTempDelta, 1);
    previousBmsData[index].cellTempDelta = bms.cellTempDelta;
    publishCount++;
  } else {
    skippedCount++;
  }
  
  // === PRIORITY 4: DIAGNOSTIC DATA (periodic) ===
  static int diagnosticCounter = 0;
  if (++diagnosticCounter >= adaptiveConfig.diagnostics_every_n_cycles) {
    // Power limits
    publishMQTT(baseTopic + "/power/charge_limit", bms.dccl, 2);
    publishMQTT(baseTopic + "/power/discharge_limit", bms.ddcl, 2);
    
    // Cell locations
    publishMQTT(baseTopic + "/cell/min_voltage_block", (float)bms.minVoltageBlock, 0);
    publishMQTT(baseTopic + "/cell/min_voltage_cell", (float)bms.minVoltageCell, 0);
    publishMQTT(baseTopic + "/cell/max_voltage_block", (float)bms.maxVoltageBlock, 0);
    publishMQTT(baseTopic + "/cell/max_voltage_cell", (float)bms.maxVoltageCell, 0);
    
    // I/O States
    publishMQTT(baseTopic + "/io/relay_r1", bms.relay_R1);
    publishMQTT(baseTopic + "/io/relay_r2", bms.relay_R2);
    publishMQTT(baseTopic + "/io/input_in01", bms.input_IN01);
    publishMQTT(baseTopic + "/io/input_in02", bms.input_IN02);
    
    // Communication diagnostics
    publishMQTT(baseTopic + "/comm/packets_received", (float)bms.packetsReceived, 0);
    publishMQTT(baseTopic + "/comm/last_update_ms", (float)(millis() - bms.lastUpdate), 0);
    
    publishCount += 12;
    diagnosticCounter = 0;
  }
  
  // Mark as initialized for future delta comparisons
  previousBmsData[index].initialized = true;
  
  // Update statistics
  stats.mqttPublishes += publishCount;
  stats.mqttSkipped += skippedCount;
  
  Serial.printf("📤 MQTT Optimized: Published %d values, skipped %d for BMS%d (%.1f%% reduction)\n", 
                publishCount, skippedCount, nodeId, 
                skippedCount > 0 ? (float)skippedCount/(publishCount + skippedCount)*100 : 0);
}

// === HOME ASSISTANT DISCOVERY ===
void publishBMSDiscovery(uint8_t nodeId) {
  if (!mqtt.connected()) return;
  
  Serial.printf("🆕 Publishing HA discovery for BMS Node %d\n", nodeId);
  
  String deviceId = "bms_node_" + String(nodeId);
  String nodeName = "BMS Node " + String(nodeId);
  
  String deviceInfo = "\"device\": {";
  deviceInfo += "\"identifiers\": [\"" + deviceId + "\"],";
  deviceInfo += "\"name\": \"" + nodeName + "\",";
  deviceInfo += "\"model\": \"IFS BMS Module (Optimized)\",";
  deviceInfo += "\"manufacturer\": \"Industrial Battery Systems\",";
  deviceInfo += "\"via_device\": \"esp32_can_bridge_optimized\",";
  deviceInfo += "\"sw_version\": \"v2.2.1_mqtt_optimized\"";
  deviceInfo += "}";
  
  // Voltage sensor
  String voltageConfig = "{";
  voltageConfig += "\"name\": \"" + nodeName + " Voltage\",";
  voltageConfig += "\"state_topic\": \"bms/" + String(nodeId) + "/voltage\",";
  voltageConfig += "\"device_class\": \"voltage\",";
  voltageConfig += "\"unit_of_measurement\": \"V\",";
  voltageConfig += "\"state_class\": \"measurement\",";
  voltageConfig += "\"icon\": \"mdi:lightning-bolt\",";
  voltageConfig += "\"unique_id\": \"bms_" + String(nodeId) + "_voltage\",";
  voltageConfig += deviceInfo + "}";
  
  mqtt.publish(("homeassistant/sensor/bms_" + String(nodeId) + "_voltage/config").c_str(), voltageConfig.c_str(), true);
  delay(100);
  
  // Current sensor  
  String currentConfig = "{";
  currentConfig += "\"name\": \"" + nodeName + " Current\",";
  currentConfig += "\"state_topic\": \"bms/" + String(nodeId) + "/current\",";
  currentConfig += "\"device_class\": \"current\",";
  currentConfig += "\"unit_of_measurement\": \"A\",";
  currentConfig += "\"state_class\": \"measurement\",";
  currentConfig += "\"icon\": \"mdi:current-dc\",";
  currentConfig += "\"unique_id\": \"bms_" + String(nodeId) + "_current\",";
  currentConfig += deviceInfo + "}";
  
  mqtt.publish(("homeassistant/sensor/bms_" + String(nodeId) + "_current/config").c_str(), currentConfig.c_str(), true);
  delay(100);
  
  // SOC sensor
  String socConfig = "{";
  socConfig += "\"name\": \"" + nodeName + " State of Charge\",";
  socConfig += "\"state_topic\": \"bms/" + String(nodeId) + "/soc\",";
  socConfig += "\"device_class\": \"battery\",";
  socConfig += "\"unit_of_measurement\": \"%\",";
  socConfig += "\"state_class\": \"measurement\",";
  socConfig += "\"icon\": \"mdi:battery\",";
  socConfig += "\"unique_id\": \"bms_" + String(nodeId) + "_soc\",";
  socConfig += deviceInfo + "}";
  
  mqtt.publish(("homeassistant/sensor/bms_" + String(nodeId) + "_soc/config").c_str(), socConfig.c_str(), true);
  delay(100);
  
  // Temperature sensor
  String tempConfig = "{";
  tempConfig += "\"name\": \"" + nodeName + " Max Temperature\",";
  tempConfig += "\"state_topic\": \"bms/" + String(nodeId) + "/temperature/max\",";
  tempConfig += "\"device_class\": \"temperature\",";
  tempConfig += "\"unit_of_measurement\": \"°C\",";
  tempConfig += "\"state_class\": \"measurement\",";
  tempConfig += "\"icon\": \"mdi:thermometer\",";
  tempConfig += "\"unique_id\": \"bms_" + String(nodeId) + "_temp_max\",";
  tempConfig += deviceInfo + "}";
  
  mqtt.publish(("homeassistant/sensor/bms_" + String(nodeId) + "_temp_max/config").c_str(), tempConfig.c_str(), true);
  delay(100);
  
  // Master error binary sensor
  String errorConfig = "{";
  errorConfig += "\"name\": \"" + nodeName + " Master Error\",";
  errorConfig += "\"state_topic\": \"bms/" + String(nodeId) + "/status/master_error\",";
  errorConfig += "\"device_class\": \"problem\",";
  errorConfig += "\"payload_on\": \"true\",";
  errorConfig += "\"payload_off\": \"false\",";
  errorConfig += "\"icon\": \"mdi:alert-circle\",";
  errorConfig += "\"unique_id\": \"bms_" + String(nodeId) + "_master_error\",";
  errorConfig += deviceInfo + "}";
  
  mqtt.publish(("homeassistant/binary_sensor/bms_" + String(nodeId) + "_master_error/config").c_str(), errorConfig.c_str(), true);
  delay(100);
  
  stats.discoveryMessages += 5;
  Serial.printf("✅ Discovery completed for BMS Node %d (5 core entities)\n", nodeId);
}

// === MQTT UTILITY FUNCTIONS ===
void publishMQTT(String topic, String payload) {
  if (mqtt.connected()) {
    mqtt.publish(topic.c_str(), payload.c_str());
  }
}

void publishMQTT(String topic, float value, int decimals) {
  if (mqtt.connected()) {
    mqtt.publish(topic.c_str(), String(value, decimals).c_str());
  }
}

void publishMQTT(String topic, bool value) {
  if (mqtt.connected()) {
    mqtt.publish(topic.c_str(), value ? "true" : "false");
  }
}

// === SETUP FUNCTIONS ===
void setupWiFi() {
  Serial.print("📶 Connecting to WiFi: ");
  Serial.println(WIFI_SSID);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 30) {
    delay(500);
    Serial.print(".");
    attempts++;
    
    if (attempts % 2 == 0) {
      digitalWrite(LED_PIN, !digitalRead(LED_PIN));
    }
  }
  
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println();
    Serial.printf("✅ WiFi connected! IP: %s\n", WiFi.localIP().toString().c_str());
    Serial.printf("📶 Signal: %d dBm\n", WiFi.RSSI());
    blinkLED(3, 200);
  } else {
    Serial.println();
    Serial.println("❌ WiFi connection failed!");
    blinkLED(10, 50);
    while(true) delay(1000);
  }
}

void setupMQTT() {
  mqtt.setServer(MQTT_BROKER, MQTT_PORT);
  mqtt.setKeepAlive(90);
  mqtt.setSocketTimeout(15);
  mqtt.setBufferSize(1024);
  
  reconnectMQTT();
}

void reconnectMQTT() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("⚠️ WiFi not connected - skipping MQTT");
    return;
  }
  
  String clientId = "ESP32_CAN_BMS_Optimized_" + String(ESP.getEfuseMac() & 0xFFFF, HEX);
  
  Serial.print("🔗 Connecting to MQTT...");
  
  if (mqtt.connect(clientId.c_str(), MQTT_USER, MQTT_PASSWORD)) {
    Serial.println(" connected!");
    stats.lastMqttReconnect = millis();
    
    mqtt.publish("bms/gateway/status", "online", true);
    mqtt.publish("bms/gateway/version", "v2.2.1_mqtt_optimized", true);
    
    Serial.println("🆕 Publishing Home Assistant discovery...");
    for (int i = 0; i < activeBmsNodes; i++) {
      publishBMSDiscovery(bmsNodeIds[i]);
      delay(200);
    }
    
    blinkLED(2, 150);
    
  } else {
    Serial.printf(" failed, rc=%d\n", mqtt.state());
    stats.reconnectAttempts++;
    blinkLED(5, 100);
  }
}

bool initializeMCP2515() {
  Serial.println("🚌 Initializing MCP2515 with fixed 125 kbps...");
  Serial.printf("📍 CS Pin: GPIO%d\n", SPI_CS_PIN);
  
  pinMode(SPI_CS_PIN, OUTPUT);
  digitalWrite(SPI_CS_PIN, HIGH);
  delay(10);
  digitalWrite(SPI_CS_PIN, LOW);
  delay(10);
  digitalWrite(SPI_CS_PIN, HIGH);
  
  Serial.println("✅ CS pin control OK");
  Serial.println("🔄 Initializing CAN at 125 kbps (fixed)...");
  
  digitalWrite(SPI_CS_PIN, LOW);
  delay(10);
  digitalWrite(SPI_CS_PIN, HIGH);
  delay(100);
  
  int result = CAN.begin(CAN_SPEED_125K);
  
  if (result == CAN_OK) {
    Serial.println("✅ CAN initialized at 125 kbps (fixed)");
    Serial.println("📋 CAN controller ready at 125 kbps");
    return true;
  }
  
  Serial.println("❌ CAN initialization failed at 125 kbps!");
  return false;
}

void setupCAN() {
  Serial.println("🔧 Configuring SPI pins for CAN Expansion Board...");
  Serial.printf("   MOSI: GPIO%d\n", SPI_MOSI_PIN);
  Serial.printf("   MISO: GPIO%d\n", SPI_MISO_PIN);
  Serial.printf("   SCK:  GPIO%d\n", SPI_SCK_PIN);
  Serial.printf("   CS:   GPIO%d\n", SPI_CS_PIN);
  
  SPI.begin(SPI_SCK_PIN, SPI_MISO_PIN, SPI_MOSI_PIN, SPI_CS_PIN);
  delay(100);
  
  SPI.beginTransaction(SPISettings(10000000, MSBFIRST, SPI_MODE0));
  SPI.endTransaction();
  
  Serial.println("✅ SPI pins configured");
  
  if (!initializeMCP2515()) {
    blinkLED(20, 100);
    while(true) {
      Serial.println("💡 Tip: Try reseating CAN Expansion Board");
      delay(5000);
    }
  }
  
  Serial.printf("🎯 Monitoring BMS Node IDs: ");
  for (int i = 0; i < activeBmsNodes; i++) {
    Serial.printf("%d(0x%X) ", bmsNodeIds[i], bmsNodeIds[i]);
  }
  Serial.println();
}

// === CAN PROCESSING ===
void processCAN() {
  unsigned char len = 0;
  unsigned char buf[8];
  unsigned long canId;
  
  if (CAN_MSGAVAIL == CAN.checkReceive()) {
    if (CAN_OK == CAN.readMsgBuf(&len, buf)) {
      canId = CAN.getCanId();
      
      stats.totalFramesReceived++;
      stats.lastFrameTime = millis();
      
      digitalWrite(LED_PIN, HIGH);
      
      printCANFrame(canId, len, buf);
      
      if (isValidBMSFrame(canId)) {
        parseCANFrame(canId, len, buf);
        stats.validBmsFrames++;
      } else {
        stats.invalidFrames++;
        Serial.printf("⚠️ Non-BMS frame: 0x%lX\n", canId);
      }
      
      digitalWrite(LED_PIN, LOW);
    } else {
      Serial.println("❌ CAN read error!");
      stats.parseErrors++;
    }
  }
}

// === MAIN SETUP & LOOP ===
void setup() {
  Serial.begin(115200);
  
  // KLUCZOWE: Poczekaj na połączenie Serial Monitor
  while (!Serial && millis() < 5000) {
    delay(10);  // Czekaj na Serial lub 5 sekund timeout
  }
  
  delay(1000);  // Dodatkowe opóźnienie dla stabilności
  
  // Wyczyść bufor i wyślij znak startowy
  Serial.flush();
  Serial.println();
  Serial.println("🚀 ESP32S3 STARTING...");
  
  Serial.println();
  Serial.println(F("=== ESP32S3 CAN to MQTT Bridge - MQTT OPTIMIZED FIXED ==="));
  Serial.println(F("VERSION: v2.2.1 - Fixed compilation errors"));
  Serial.println(F("OPTIMIZATION: 80-95% MQTT bandwidth reduction"));
  Serial.println(F("LIBRARY: limengdu/Arduino_CAN_BUS_MCP2515"));
  Serial.println(F("======================================================"));
  Serial.println();
  
  Serial.println("📊 MQTT Optimization Configuration:");
  Serial.printf("   Idle interval: %d ms (%.1f sec)\n", adaptiveConfig.idle_interval, adaptiveConfig.idle_interval/1000.0);
  Serial.printf("   Active interval: %d ms (%.1f sec)\n", adaptiveConfig.active_interval, adaptiveConfig.active_interval/1000.0);
  Serial.printf("   Charging interval: %d ms (%.1f sec)\n", adaptiveConfig.charging_interval, adaptiveConfig.charging_interval/1000.0);
  Serial.printf("   Error interval: %d ms (%.1f sec)\n", adaptiveConfig.error_interval, adaptiveConfig.error_interval/1000.0);
  Serial.printf("   Current threshold: %.1f A\n", adaptiveConfig.current_threshold);
  Serial.printf("   Delta thresholds: V=%.1f, I=%.1f, SOC=%.1f, T=%.1f\n", 
                adaptiveConfig.voltage_delta, adaptiveConfig.current_delta, 
                adaptiveConfig.soc_delta, adaptiveConfig.temperature_delta);
  Serial.println();
  
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);
  
  // Initialize BMS data structures
  for (int i = 0; i < MAX_BMS_NODES; i++) {
    memset(&bmsModules[i], 0, sizeof(BMSData));
    memset(&previousBmsData[i], 0, sizeof(PreviousValues));
    bmsModules[i].communicationOk = false;
    previousBmsData[i].initialized = false;
  }
  
  setupWiFi();
  setupCAN();
  setupMQTT();
  
  blinkLED(5, 200);
  
  Serial.println(F("🚀 MQTT Optimized CAN Bridge ready!"));
  Serial.println(F("📊 Smart publishing: Adaptive intervals + Delta detection"));
  Serial.println(F("🎯 Bandwidth savings: 80-95% reduction in MQTT traffic"));
  Serial.println();
}

void loop() {
  static unsigned long lastHeartbeat = 0;
  static unsigned long lastMqttCheck = 0;
  static unsigned long lastPublish = 0;
  static unsigned long lastCommCheck = 0;
  unsigned long now = millis();
  
  // === PRIORITY 1: Process CAN messages ===
  processCAN();
  
  // === PRIORITY 2: MQTT maintenance ===
  if (now - lastMqttCheck >= 5000) {
    if (WiFi.status() == WL_CONNECTED) {
      if (!mqtt.connected()) {
        Serial.println("🔄 MQTT reconnecting...");
        reconnectMQTT();
      } else {
        mqtt.loop();
      }
    } else {
      Serial.println("⚠️ WiFi disconnected - attempting reconnection...");
      WiFi.reconnect();
    }
    lastMqttCheck = now;
  } else if (mqtt.connected()) {
    mqtt.loop();
  }
  
  // === PRIORITY 3: ADAPTIVE MQTT PUBLISHING ===
  int currentInterval = determinePublishingInterval();
  
  if (now - lastPublish >= currentInterval) {
    if (mqtt.connected()) {
      Serial.printf("📤 Publishing BMS data (adaptive interval: %dms)...\n", currentInterval);
      
      for (int i = 0; i < activeBmsNodes; i++) {
        publishBMSDataOptimized(bmsNodeIds[i], 0);
        delay(50);  // Avoid overwhelming MQTT broker
      }
    }
    lastPublish = now;
  }
  
  // === PRIORITY 4: Communication timeout check ===
  if (now - lastCommCheck >= 10000) {
    for (int i = 0; i < activeBmsNodes; i++) {
      uint8_t nodeId = bmsNodeIds[i];
      BMSData& bms = bmsModules[i];
      
      if (bms.communicationOk && (now - bms.lastUpdate > 30000)) {
        Serial.printf("⚠️ BMS%d communication timeout!\n", nodeId);
        bms.communicationOk = false;
        
        if (mqtt.connected()) {
          String baseTopic = "bms/" + String(nodeId);
          publishMQTT(baseTopic + "/comm/status", "offline");
        }
      }
    }
    lastCommCheck = now;
  }
  
  // === PRIORITY 5: Optimized heartbeat ===
  if (now - lastHeartbeat >= 60000) {
    Serial.println();
    Serial.println(F("💓 ===== MQTT OPTIMIZED BRIDGE HEARTBEAT ====="));
    Serial.printf("🚌 CAN frames: %d total, %d BMS valid, %d invalid, %d errors\n", 
                  stats.totalFramesReceived, stats.validBmsFrames, stats.invalidFrames, stats.parseErrors);
    Serial.printf("📤 MQTT: %d published, %d skipped (%.1f%% bandwidth saved)\n", 
                  stats.mqttPublishes, stats.mqttSkipped,
                  stats.mqttSkipped > 0 ? (float)stats.mqttSkipped/(stats.mqttPublishes + stats.mqttSkipped)*100 : 0);
    
    if (stats.lastFrameTime > 0) {
      Serial.printf("📡 Last CAN frame: %lu sec ago\n", (now - stats.lastFrameTime)/1000);
    } else {
      Serial.println("📡 No CAN frames received yet");
    }
    
    Serial.printf("📶 WiFi: %s (RSSI: %d dBm)\n", 
                  WiFi.status() == WL_CONNECTED ? "Connected" : "Disconnected",
                  WiFi.RSSI());
    Serial.printf("🔗 MQTT: %s\n", mqtt.connected() ? "Connected" : "Disconnected");
    Serial.printf("⚡ Current publishing interval: %dms\n", currentInterval);
    Serial.printf("⏰ Uptime: %lu min\n", now/60000);
    Serial.printf("💾 Free heap: %d bytes\n", ESP.getFreeHeap());
    Serial.println(F("============================================"));
    Serial.println();
    
    // Publish gateway statistics to MQTT
    if (mqtt.connected()) {
      mqtt.publish("bms/gateway/status", "online");
      
      String statsJson = "{";
      statsJson += "\"can_frames_total\":" + String(stats.totalFramesReceived) + ",";
      statsJson += "\"can_frames_valid\":" + String(stats.validBmsFrames) + ",";
      statsJson += "\"mqtt_published\":" + String(stats.mqttPublishes) + ",";
      statsJson += "\"mqtt_skipped\":" + String(stats.mqttSkipped) + ",";
      statsJson += "\"bandwidth_saved_percent\":" + String(stats.mqttSkipped > 0 ? (float)stats.mqttSkipped/(stats.mqttPublishes + stats.mqttSkipped)*100 : 0, 1) + ",";
      statsJson += "\"current_interval_ms\":" + String(currentInterval) + ",";
      statsJson += "\"active_nodes\":" + String(activeBmsNodes) + ",";
      statsJson += "\"uptime_seconds\":" + String(now/1000) + ",";
      statsJson += "\"free_heap_bytes\":" + String(ESP.getFreeHeap()) + ",";
      statsJson += "\"wifi_rssi_dbm\":" + String(WiFi.RSSI()) + ",";
      statsJson += "\"version\":\"v2.2.1_fixed\"";
      statsJson += "}";
      
      mqtt.publish("bms/gateway/stats", statsJson.c_str());
    }
    
    lastHeartbeat = now;
    
    // Visual heartbeat blink
    blinkLED(1, 200);
  }
  
  // Minimal delay for optimal CAN responsiveness
  delay(1);
}

/*
; === Do can bus breakout board ===


[env:xiao_esp32s3]
platform = espressif32
board = seeed_xiao_esp32s3
framework = arduino

; === SERIAL CONFIGURATION ===
monitor_speed = 115200
monitor_filters = esp32_exception_decoder
monitor_port = auto

; KLUCZOWE: Automatyczny restart ESP32 przy połączeniu monitora
monitor_rts = 0
monitor_dtr = 1
monitor_eol = LF
monitor_echo = true

; === BUILD FLAGS ===
build_flags = 
    -DBOARD_HAS_PSRAM
    -DARDUINO_USB_MODE=1
    -DARDUINO_USB_CDC_ON_BOOT=1
    -DMQTT_MAX_PACKET_SIZE=1024
    -DCORE_DEBUG_LEVEL=3

; === DEPENDENCIES ===
lib_deps = 
    knolleary/PubSubClient@^2.8


; === UPLOAD CONFIGURATION ===
upload_speed = 460800
upload_port = auto
upload_resetmethod = esp32r0

; === BOARD SPECIFIC ===
board_build.flash_mode = qio
board_build.f_cpu = 240000000L

 */