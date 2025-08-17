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
APTriggerState_t apTriggerState;

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

// === AP MODE TRIGGER FUNCTIONS ===

/**
 * @brief Inicjalizacja systemu wyzwalania trybu AP
 */
void initializeAPTrigger() {
  resetAPTriggerState();
  DEBUG_PRINTLN("📡 AP Trigger system initialized");
  DEBUG_PRINTF("   Trigger CAN ID: 0x%03lX\n", AP_TRIGGER_CAN_ID);
  DEBUG_PRINTF("   Required pattern: 0x%02X 0x%02X\n", AP_TRIGGER_DATA_0, AP_TRIGGER_DATA_1);
  DEBUG_PRINTF("   Required count: %d within %d ms\n", AP_TRIGGER_COUNT_REQUIRED, AP_TRIGGER_TIME_WINDOW_MS);
  DEBUG_PRINTF("   AP duration: %d ms\n", AP_MODE_DURATION_MS);
}

/**
 * @brief Resetowanie stanu wyzwalaczy AP
 */
void resetAPTriggerState() {
  apTriggerState.triggerCount = 0;
  apTriggerState.lastTriggerTime = 0;
  apTriggerState.apModeStartTime = 0;
  apTriggerState.apModeActive = false;
  apTriggerState.manualAPMode = false;
  
  for (int i = 0; i < AP_TRIGGER_COUNT_REQUIRED; i++) {
    apTriggerState.triggerTimestamps[i] = 0;
  }
}

/**
 * @brief Sprawdź czy ramka CAN to wyzwalacz AP
 */
bool isAPTriggerFrame(unsigned long canId, unsigned char len, unsigned char* buf) {
  if (canId != AP_TRIGGER_CAN_ID) return false;
  if (len < 2) return false;
  if (buf[0] != AP_TRIGGER_DATA_0) return false;
  if (buf[1] != AP_TRIGGER_DATA_1) return false;
  
  return true;
}

/**
 * @brief Przetwarzanie ramki wyzwalacza AP
 */
bool processAPTriggerFrame(unsigned long canId, unsigned char len, unsigned char* buf) {
  if (!isAPTriggerFrame(canId, len, buf)) {
    return false;  // To nie jest ramka wyzwalacza
  }
  
  unsigned long now = millis();
  
  DEBUG_PRINTF("🎯 AP Trigger frame received: 0x%03lX [%02X %02X]\n", 
               canId, buf[0], buf[1]);
  
  // Usuń stare wyzwalacze poza oknem czasowym
  unsigned long windowStart = now - AP_TRIGGER_TIME_WINDOW_MS;
  int validTriggers = 0;
  
  for (int i = 0; i < apTriggerState.triggerCount; i++) {
    if (apTriggerState.triggerTimestamps[i] >= windowStart) {
      if (validTriggers != i) {
        apTriggerState.triggerTimestamps[validTriggers] = apTriggerState.triggerTimestamps[i];
      }
      validTriggers++;
    }
  }
  
  apTriggerState.triggerCount = validTriggers;
  
  // Dodaj nowy wyzwalacz
  if (apTriggerState.triggerCount < AP_TRIGGER_COUNT_REQUIRED) {
    apTriggerState.triggerTimestamps[apTriggerState.triggerCount] = now;
    apTriggerState.triggerCount++;
  }
  
  apTriggerState.lastTriggerTime = now;
  
  DEBUG_PRINTF("   Trigger count: %d/%d\n", apTriggerState.triggerCount, AP_TRIGGER_COUNT_REQUIRED);
  
  // Sprawdź czy należy uruchomić tryb AP
  if (shouldStartAPMode()) {
    startTriggeredAPMode();
    return true;
  }
  
  return true;  // Ramka została przetworzona
}

/**
 * @brief Sprawdź czy należy uruchomić tryb AP
 */
bool shouldStartAPMode() {
  return (apTriggerState.triggerCount >= AP_TRIGGER_COUNT_REQUIRED && 
          !apTriggerState.apModeActive);
}

/**
 * @brief Uruchom tryb AP na podstawie wyzwalaczy
 */
void startTriggeredAPMode() {
  unsigned long now = millis();
  
  apTriggerState.apModeActive = true;
  apTriggerState.apModeStartTime = now;
  apTriggerState.manualAPMode = false;
  
  DEBUG_PRINTLN("🚀 Starting triggered AP mode");
  DEBUG_PRINTF("   Start time: %lu ms\n", now);
  DEBUG_PRINTF("   Duration: %d ms\n", AP_MODE_DURATION_MS);
  
  // Wywołaj funkcję WiFiManager do uruchomienia AP
  // Forward declaration - właściwa implementacja w main.cpp
  void callWiFiManagerStartTriggeredAP();
  callWiFiManagerStartTriggeredAP();
}

/**
 * @brief Zatrzymaj wyzwalany tryb AP
 */
void stopTriggeredAPMode() {
  if (apTriggerState.apModeActive && !apTriggerState.manualAPMode) {
    apTriggerState.apModeActive = false;
    apTriggerState.apModeStartTime = 0;
    
    DEBUG_PRINTLN("🛑 Stopping triggered AP mode");
    
    // Reset liczników wyzwalaczy
    resetAPTriggerState();
    
    // Wywołaj funkcję WiFiManager do zatrzymania AP
    // Forward declaration - właściwa implementacja w main.cpp
    void callWiFiManagerStopTriggeredAP();
    callWiFiManagerStopTriggeredAP();
  }
}

/**
 * @brief Aktualizacja stanu trybu AP (wywoływane w pętli głównej)
 */
void updateAPModeStatus() {
  if (!apTriggerState.apModeActive) return;
  if (apTriggerState.manualAPMode) return;  // Tryb ręczny - nie zatrzymuj automatycznie
  
  unsigned long now = millis();
  unsigned long elapsed = now - apTriggerState.apModeStartTime;
  
  // Sprawdź czy ostatni wyzwalacz był w ciągu ostatnich 30 sekund
  unsigned long timeSinceLastTrigger = now - apTriggerState.lastTriggerTime;
  
  if (timeSinceLastTrigger > AP_MODE_DURATION_MS) {
    DEBUG_PRINTF("⏰ AP mode timeout: %lu ms since last trigger\n", timeSinceLastTrigger);
    stopTriggeredAPMode();
  }
}