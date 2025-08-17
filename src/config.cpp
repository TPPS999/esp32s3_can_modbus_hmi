/*
 * config.cpp - ESP32S3 CAN to Modbus TCP Bridge Configuration Implementation
 * 
 * VERSION: v3.1.0
 * DATE: 2025-08-12
 */

#include "config.h"
#include <EEPROM.h>
#include <string.h>
#include <mcp_can.h>

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
    setDefaultConfiguration();
    return saveConfiguration();
  }
  
  // Wczytaj konfigurację WiFi
  for (int i = 0; i < MAX_WIFI_SSID_LENGTH && i < 64; i++) {
    systemConfig.wifiSSID[i] = EEPROM.read(EEPROM_WIFI_SSID + i);
    systemConfig.wifiPassword[i] = EEPROM.read(EEPROM_WIFI_PASS + i);
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
  
  DEBUG_PRINTF("✅ Configuration loaded: WiFi=%s, BMS=%d, CAN=%d\n", 
               systemConfig.wifiSSID, systemConfig.activeBmsNodes, systemConfig.canSpeed);
  
  return systemConfig.configValid;
}

bool saveConfiguration() {
  DEBUG_PRINTLN("💾 Saving configuration to EEPROM...");
  
  if (!EEPROM.begin(EEPROM_SIZE)) {
    DEBUG_PRINTLN("❌ EEPROM initialization failed!");
    return false;
  }
  
  // Zapisz konfigurację WiFi
  for (int i = 0; i < MAX_WIFI_SSID_LENGTH && i < 64; i++) {
    EEPROM.write(EEPROM_WIFI_SSID + i, 
                 i < strlen(systemConfig.wifiSSID) ? systemConfig.wifiSSID[i] : 0);
    EEPROM.write(EEPROM_WIFI_PASS + i, 
                 i < strlen(systemConfig.wifiPassword) ? systemConfig.wifiPassword[i] : 0);
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

void setDefaultConfiguration() {
  DEBUG_PRINTLN("🔧 Initializing default configuration...");
  
  // Domyślna konfiguracja WiFi
  strcpy(systemConfig.wifiSSID, WIFI_SSID);
  strcpy(systemConfig.wifiPassword, WIFI_PASSWORD);
  
  // Domyślna konfiguracja BMS
  systemConfig.activeBmsNodes = 4; // Default to 4 BMS modules
  for (int i = 0; i < systemConfig.activeBmsNodes; i++) {
    systemConfig.bmsNodeIds[i] = i + 1;  // Node IDs 1-4
  }
  
  // Domyślna konfiguracja CAN i Modbus
  systemConfig.canSpeed = CAN_125KBPS;
  systemConfig.modbusPort = MODBUS_TCP_PORT;
  systemConfig.modbusSlaveId = MODBUS_SLAVE_ID;
  systemConfig.enableCanFiltering = true;
  systemConfig.enableModbusWrite = true;
  systemConfig.enableWifiAP = true;
  systemConfig.heartbeatInterval = HEARTBEAT_INTERVAL_MS;
  systemConfig.communicationTimeout = COMMUNICATION_TIMEOUT_MS;
  
  systemConfig.configValid = true;
  
  DEBUG_PRINTLN("✅ Default configuration initialized");
}

bool validateConfiguration() {
  DEBUG_PRINTLN("🔍 Validating configuration...");
  
  // Sprawdź długość SSID
  if (strlen(systemConfig.wifiSSID) == 0 || strlen(systemConfig.wifiSSID) >= 64) {
    DEBUG_PRINTLN("❌ Invalid WiFi SSID length");
    return false;
  }
  
  // Sprawdź długość hasła
  if (strlen(systemConfig.wifiPassword) >= 64) {
    DEBUG_PRINTLN("❌ Invalid WiFi password length");
    return false;
  }
  
  // Sprawdź liczbę aktywnych węzłów BMS
  if (systemConfig.activeBmsNodes <= 0 || systemConfig.activeBmsNodes > MAX_BMS_NODES) {
    DEBUG_PRINTLN("❌ Invalid BMS nodes count");
    return false;
  }
  
  DEBUG_PRINTLN("✅ Configuration validated successfully");
  return true;
}

const SystemConfig& getSystemConfig() {
  return systemConfig;
}