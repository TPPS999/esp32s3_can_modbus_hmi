/*
 * config.cpp - ESP32S3 CAN to Modbus TCP Bridge Configuration Implementation
 * 
 * VERSION: v3.1.0
 * DATE: 2025-08-12
 */

#include "config.h"
#include <EEPROM.h>
#include <string.h>

// === GLOBAL VARIABLES ===
SystemConfig systemConfig;
SystemState_t systemState = SYSTEM_STATE_INIT;

// === PRIVATE FUNCTIONS ===
void parseBMSIds(const String& idsStr);
String getBMSIdsString();

// === PUBLIC FUNCTIONS ===

bool loadConfiguration() {
  DEBUG_PRINTLN("📚 Loading configuration from EEPROM...");
  
  if (!EEPROM.begin(EEPROM_SIZE)) {
    DEBUG_PRINTLN("❌ EEPROM initialization failed!");
    return false;
  }
  
  // Sprawdź magic number
  if (EEPROM.read(EEPROM_MAGIC) != EEPROM_MAGIC_VALUE) {
    DEBUG_PRINTLN("⚙️ First boot - initializing default configuration");
    initializeDefaultConfiguration();
    return saveConfiguration();
  }
  
  // Wczytaj konfigurację WiFi
  for (int i = 0; i < MAX_WIFI_SSID_LENGTH; i++) {
    systemConfig.wifiSSID[i] = EEPROM.read(EEPROM_WIFI_SSID + i);
    systemConfig.wifiPassword[i] = EEPROM.read(EEPROM_WIFI_PASS + i);
    if (i < MAX_IP_ADDRESS_LENGTH) {
      systemConfig.deviceIP[i] = EEPROM.read(EEPROM_DEVICE_IP + i);
    }
  }
  
  // Wczytaj konfigurację BMS
  systemConfig.activeBmsNodes = EEPROM.read(EEPROM_ACTIVE_BMS);
  if (systemConfig.activeBmsNodes > MAX_BMS_NODES) {
    systemConfig.activeBmsNodes = MAX_BMS_NODES;
  }
  
  for (int i = 0; i < MAX_BMS_NODES; i++) {
    systemConfig.bmsNodeIds[i] = EEPROM.read(EEPROM_BMS_IDS + i);
  }
  
  // Wczytaj konfigurację CAN
  systemConfig.canSpeed = EEPROM.read(EEPROM_CAN_SPEED);
  
  // Walidacja konfiguracji
  systemConfig.configValid = validateConfiguration();
  
  DEBUG_PRINTF("✅ Configuration loaded: WiFi=%s, IP=%s, BMS=%d, CAN=%d\n", 
               systemConfig.wifiSSID, systemConfig.deviceIP, 
               systemConfig.activeBmsNodes, systemConfig.canSpeed);
  
  return systemConfig.configValid;
}

bool saveConfiguration() {
  DEBUG_PRINTLN("💾 Saving configuration to EEPROM...");
  
  if (!EEPROM.begin(EEPROM_SIZE)) {
    DEBUG_PRINTLN("❌ EEPROM initialization failed!");
    return false;
  }
  
  // Zapisz konfigurację WiFi
  for (int i = 0; i < MAX_WIFI_SSID_LENGTH; i++) {
    EEPROM.write(EEPROM_WIFI_SSID + i, 
                 i < strlen(systemConfig.wifiSSID) ? systemConfig.wifiSSID[i] : 0);
    EEPROM.write(EEPROM_WIFI_PASS + i, 
                 i < strlen(systemConfig.wifiPassword) ? systemConfig.wifiPassword[i] : 0);
    if (i < MAX_IP_ADDRESS_LENGTH) {
      EEPROM.write(EEPROM_DEVICE_IP + i, 
                   i < strlen(systemConfig.deviceIP) ? systemConfig.deviceIP[i] : 0);
    }
  }
  
  // Zapisz konfigurację BMS
  EEPROM.write(EEPROM_ACTIVE_BMS, systemConfig.activeBmsNodes);
  for (int i = 0; i < MAX_BMS_NODES; i++) {
    EEPROM.write(EEPROM_BMS_IDS + i, 
                 i < systemConfig.activeBmsNodes ? systemConfig.bmsNodeIds[i] : 0);
  }
  
  // Zapisz konfigurację CAN
  EEPROM.write(EEPROM_CAN_SPEED, systemConfig.canSpeed);
  
  // Zapisz magic number
  EEPROM.write(EEPROM_MAGIC, EEPROM_MAGIC_VALUE);
  
  bool result = EEPROM.commit();
  
  if (result) {
    DEBUG_PRINTLN("✅ Configuration saved successfully");
    systemConfig.configValid = true;
  } else {
    DEBUG_PRINTLN("❌ Configuration save failed");
  }
  
  return result;
}

void initializeDefaultConfiguration() {
  DEBUG_PRINTLN("🔧 Initializing default configuration...");
  
  // Domyślna konfiguracja WiFi
  strcpy(systemConfig.wifiSSID, DEFAULT_WIFI_SSID);
  strcpy(systemConfig.wifiPassword, DEFAULT_WIFI_PASSWORD);
  strcpy(systemConfig.deviceIP, DEFAULT_DEVICE_IP);
  
  // Domyślna konfiguracja BMS
  systemConfig.activeBmsNodes = MAX_BMS_NODES;
  for (int i = 0; i < MAX_BMS_NODES; i++) {
    systemConfig.bmsNodeIds[i] = i + 1;  // Node IDs 1-16
  }
  
  // Domyślna konfiguracja CAN
  systemConfig.canSpeed = CAN_SPEED_125K;
  
  systemConfig.configValid = true;
  
  DEBUG_PRINTLN("✅ Default configuration initialized");
}

bool validateConfiguration() {
  DEBUG_PRINTLN("🔍 Validating configuration...");
  
  // Sprawdź długość SSID
  if (strlen(systemConfig.wifiSSID) == 0 || strlen(systemConfig.wifiSSID) >= MAX_WIFI_SSID_LENGTH) {
    DEBUG_PRINTLN("❌ Invalid WiFi SSID length");
    return false;
  }
  
  // Sprawdź długość hasła
  if (strlen(systemConfig.wifiPassword) >= MAX_WIFI_PASSWORD_LENGTH) {
    DEBUG_PRINTLN("❌ Invalid WiFi password length");
    return false;
  }
  
  // Sprawdź liczbę węzłów BMS
  if (systemConfig.activeBmsNodes == 0 || systemConfig.activeBmsNodes > MAX_BMS_NODES) {
    DEBUG_PRINTLN("❌ Invalid number of BMS nodes");
    return false;
  }
  
  // Sprawdź ID węzłów BMS
  for (int i = 0; i < systemConfig.activeBmsNodes; i++) {
    if (systemConfig.bmsNodeIds[i] == 0 || systemConfig.bmsNodeIds[i] > 255) {
      DEBUG_PRINTF("❌ Invalid BMS node ID at index %d: %d\n", i, systemConfig.bmsNodeIds[i]);
      return false;
    }
  }
  
  // Sprawdź prędkość CAN
  if (systemConfig.canSpeed != CAN_125KBPS && 
      systemConfig.canSpeed != CAN_250KBPS && 
      systemConfig.canSpeed != CAN_500KBPS) {
    DEBUG_PRINTLN("❌ Invalid CAN speed");
    return false;
  }
  
  DEBUG_PRINTLN("✅ Configuration validation passed");
  return true;
}

void printConfiguration() {
  DEBUG_PRINTLN("\n📋 === CURRENT CONFIGURATION ===");
  DEBUG_PRINTF("   Firmware Version: %s\n", FIRMWARE_VERSION);
  DEBUG_PRINTF("   Build Date: %s\n", BUILD_DATE);
  DEBUG_PRINTF("   Device Name: %s\n", DEVICE_NAME);
  DEBUG_PRINTLN("\n📶 WiFi Configuration:");
  DEBUG_PRINTF("   SSID: %s\n", systemConfig.wifiSSID);
  DEBUG_PRINTF("   Password: %s\n", strlen(systemConfig.wifiPassword) > 0 ? "***" : "Not set");
  DEBUG_PRINTF("   IP Mode: %s\n", systemConfig.deviceIP);
  DEBUG_PRINTLN("\n🔋 BMS Configuration:");
  DEBUG_PRINTF("   Active Nodes: %d\n", systemConfig.activeBmsNodes);
  DEBUG_PRINT("   Node IDs: ");
  for (int i = 0; i < systemConfig.activeBmsNodes; i++) {
    DEBUG_PRINTF("%d", systemConfig.bmsNodeIds[i]);
    if (i < systemConfig.activeBmsNodes - 1) DEBUG_PRINT(", ");
  }
  DEBUG_PRINTLN();
  DEBUG_PRINTLN("\n🚌 CAN Configuration:");
  DEBUG_PRINTF("   Speed: %d kbps\n", 
               systemConfig.canSpeed == CAN_125KBPS ? 125 :
               systemConfig.canSpeed == CAN_250KBPS ? 250 : 500);
  DEBUG_PRINTLN("\n🔌 Modbus TCP Configuration:");
  DEBUG_PRINTF("   Port: %d\n", MODBUS_TCP_PORT);
  DEBUG_PRINTF("   Slave ID: %d\n", MODBUS_SLAVE_ID);
  DEBUG_PRINTF("   Max Registers: %d\n", MODBUS_MAX_HOLDING_REGISTERS);
  DEBUG_PRINTF("   Registers per BMS: %d\n", MODBUS_REGISTERS_PER_BMS);
  DEBUG_PRINTF("   Config Valid: %s\n", systemConfig.configValid ? "✅ Yes" : "❌ No");
  DEBUG_PRINTLN("==============================\n");
}

// === HELPER FUNCTIONS ===

void parseBMSIds(const String& idsStr) {
  DEBUG_PRINTF("🔧 Parsing BMS IDs: %s\n", idsStr.c_str());
  
  int count = 0;
  int start = 0;
  int pos = 0;
  
  while (pos <= idsStr.length() && count < MAX_BMS_NODES) {
    if (pos == idsStr.length() || idsStr[pos] == ',') {
      if (pos > start) {
        int id = idsStr.substring(start, pos).toInt();
        if (id > 0 && id <= 255) {
          systemConfig.bmsNodeIds[count++] = id;
          DEBUG_PRINTF("   Added BMS ID: %d\n", id);
        } else {
          DEBUG_PRINTF("   Skipped invalid ID: %d\n", id);
        }
      }
      start = pos + 1;
    }
    pos++;
  }
  
  systemConfig.activeBmsNodes = count;
  DEBUG_PRINTF("✅ Parsed %d BMS node IDs\n", count);
}

String getBMSIdsString() {
  String ids = "";
  for (int i = 0; i < systemConfig.activeBmsNodes; i++) {
    if (i > 0) ids += ",";
    ids += String(systemConfig.bmsNodeIds[i]);
  }
  return ids;
}

// === CONFIGURATION UPDATE FUNCTIONS ===

bool updateWiFiConfig(const String& ssid, const String& password, const String& ip) {
  if (ssid.length() >= MAX_WIFI_SSID_LENGTH || 
      password.length() >= MAX_WIFI_PASSWORD_LENGTH ||
      ip.length() >= MAX_IP_ADDRESS_LENGTH) {
    DEBUG_PRINTLN("❌ WiFi config parameters too long");
    return false;
  }
  
  strcpy(systemConfig.wifiSSID, ssid.c_str());
  strcpy(systemConfig.wifiPassword, password.c_str());
  strcpy(systemConfig.deviceIP, ip.c_str());
  
  DEBUG_PRINTF("✅ WiFi config updated: %s / %s\n", ssid.c_str(), ip.c_str());
  return true;
}

bool updateBMSConfig(int nodeCount, const String& nodeIds) {
  if (nodeCount <= 0 || nodeCount > MAX_BMS_NODES) {
    DEBUG_PRINTF("❌ Invalid BMS node count: %d\n", nodeCount);
    return false;
  }
  
  systemConfig.activeBmsNodes = nodeCount;
  parseBMSIds(nodeIds);
  
  return validateConfiguration();
}

bool updateCANConfig(int speed) {
  uint8_t canSpeed;
  switch (speed) {
    case 125: canSpeed = CAN_125KBPS; break;
    case 250: canSpeed = CAN_250KBPS; break;
    case 500: canSpeed = CAN_500KBPS; break;
    default:
      DEBUG_PRINTF("❌ Invalid CAN speed: %d\n", speed);
      return false;
  }
  
  systemConfig.canSpeed = canSpeed;
  DEBUG_PRINTF("✅ CAN speed updated: %d kbps\n", speed);
  return true;
}