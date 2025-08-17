/*
 * main.cpp - ESP32S3 CAN to Modbus TCP Bridge - Complete Implementation
 * 
 * VERSION: v4.0.2 - USUNIĘTO CAN_HANDLER
 * DATE: 2025-08-13 09:17
 * STATUS: ✅ READY - can_handler.h/.cpp USUNIĘTE - funkcje w bms_protocol.h
 * 
 * DESCRIPTION: Główny plik aplikacji z pełną funkcjonalnością
 * 
 * MODULES USED (POPRAWIONE):
 * - config.h/cpp          ✅ System configuration and EEPROM
 * - wifi_manager.h/cpp    ✅ WiFi connection management
 * - modbus_tcp.h/cpp      ✅ Modbus TCP server implementation
 * - bms_data.h            ✅ BMS data structures and management (ROZSZERZONA)
 * - bms_protocol.h/cpp    ✅ BMS protocol parsing + CAN handling (KOMPLETNA)
 * - utils.h/cpp           ✅ Utility functions and diagnostics
 * 
 * REMOVED:
 * - can_handler.h/cpp     ❌ USUNIĘTE - duplikat funkcji z bms_protocol.h
 */

// === CORE INCLUDES ===
#include <Arduino.h>

// === PROJECT MODULES (POPRAWIONE - bez can_handler) ===
#include "config.h"
#include "wifi_manager.h"
#include "modbus_tcp.h"
#include "bms_data.h"
#include "bms_protocol.h"  // 🔥 ZAWIERA: setupCAN, processCAN, isCANHealthy + parsery
#include "utils.h"

// === SYSTEM STATE VARIABLES ===
SystemState_t currentSystemState = SYSTEM_STATE_INIT;
unsigned long systemStartTime = 0;
unsigned long lastHeartbeat = 0;
unsigned long lastDiagnostics = 0;
unsigned long lastStatusCheck = 0;

// === 🔥 HEARTBEAT AND MONITORING ===
#define HEARTBEAT_INTERVAL_MS 60000        // 1 minute
#define DIAGNOSTICS_INTERVAL_MS 300000     // 5 minutes  
#define STATUS_CHECK_INTERVAL_MS 10000     // 10 seconds
#define COMMUNICATION_TIMEOUT_MS 30000     // 30 seconds
#define SYSTEM_RESTART_DELAY_MS 2000       // 2 seconds

// === FORWARD DECLARATIONS ===
void initializeSystem();
bool initializeModules();
void processSystemLoop();
void checkSystemHealth();
void performSystemDiagnostics();
void handleSystemHeartbeat();
void handleSystemState();
void emergencyActions();
void printStartupBanner();
void printSystemStatus();

// === CALLBACK FUNCTIONS ===
void onWiFiStateChange(WiFiState_t oldState, WiFiState_t newState);
void onWiFiConnected(String ip);
void onWiFiDisconnected();

// === 🔥 MAIN SETUP FUNCTION ===
void setup() {
  systemStartTime = millis();
  
  // Initialize serial communication
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
  
  printStartupBanner();
  
  // Initialize system
  initializeSystem();
  
  // Print final status
  if (currentSystemState == SYSTEM_STATE_RUNNING) {
    Serial.println("✅ System initialization completed successfully!");
    Serial.println("🚀 ESP32S3 CAN to Modbus TCP Bridge is READY!");
    blinkLED(5, 200); // Success signal
  } else {
    Serial.println("❌ System initialization failed!");
    Serial.println("🚨 System entering error recovery mode...");
    blinkLED(10, 100); // Error signal
  }
  
  Serial.println();
  printSystemStatus();
  Serial.println("\n" + String('=', 60));
  Serial.println("📊 Starting main processing loop...");
  Serial.println(String('=', 60) + "\n");
}

// === 🔥 MAIN LOOP FUNCTION ===
void loop() {
  unsigned long now = millis();
  
  // Process main system loop
  processSystemLoop();
  
  // Periodic system checks
  if (now - lastStatusCheck >= STATUS_CHECK_INTERVAL_MS) {
    lastStatusCheck = now;
    checkSystemHealth();
    checkCommunicationTimeouts();
    handleSystemState();
  }
  
  // 🔥 AP Mode trigger management (sprawdzaj co pętlę dla responsywności)
  updateAPModeStatus();
  
  // 🔥 ROZSZERZONY HEARTBEAT z danymi multipleksera
  if (now - lastHeartbeat >= HEARTBEAT_INTERVAL_MS) {
    lastHeartbeat = now;
    handleSystemHeartbeat();
  }
  
  // System diagnostics
  if (now - lastDiagnostics >= DIAGNOSTICS_INTERVAL_MS) {
    lastDiagnostics = now;
    performSystemDiagnostics();
  }
  
  // Minimal delay for optimal CAN responsiveness
  delay(1);
}

// === INITIALIZATION FUNCTIONS ===

void initializeSystem() {
  Serial.println("🔧 Initializing ESP32S3 CAN to Modbus TCP Bridge...");
  Serial.println("📋 System Architecture: Modular v4.0.2 (can_handler removed)");
  Serial.println();
  
  currentSystemState = SYSTEM_STATE_INITIALIZING;
  
  // Initialize LED for status indication
  printBootProgress("LED System", true);
  setupLED();
  
  // Initialize configuration system
  printBootProgress("Configuration System", loadConfiguration());
  if (!systemConfig.configValid) {
    Serial.println("⚠️ Using default configuration");
  }
  
  // 🔥 Initialize AP Trigger system
  printBootProgress("AP Trigger System", true);
  initializeAPTrigger();
  
  // 🔥 Initialize BMS data structures
  for (int i = 0; i < MAX_BMS_NODES; i++) {
    memset(&bmsModules[i], 0, sizeof(BMSData));
    bmsModules[i].communicationOk = false;
  }
  
  // Initialize all modules
  bool modulesOK = initializeModules();
  
  if (modulesOK) {
    currentSystemState = SYSTEM_STATE_RUNNING;
    Serial.println("✅ All modules initialized successfully");
  } else {
    currentSystemState = SYSTEM_STATE_ERROR;
    Serial.println("❌ Module initialization failed");
  }
}

bool initializeModules() {
  Serial.println("🔧 Initializing system modules...");
  bool success = true;
  
  // 1. Initialize BMS Data Manager
  Serial.print("📊 BMS Data Manager... ");
  if (initializeBMSData()) {
    Serial.println("✅ OK");
  } else {
    Serial.println("❌ FAILED");
    success = false;
  }
  
  // 2. Initialize WiFi Manager  
  Serial.print("📡 WiFi Manager... ");
  wifiManager.setCallbacks(onWiFiStateChange, onWiFiConnected, onWiFiDisconnected);
  if (wifiManager.begin()) {
    Serial.println("✅ OK");
  } else {
    Serial.println("❌ FAILED");
    success = false;
  }
  
  // 3. Initialize BMS Protocol (zawiera CAN handling)
  Serial.print("🚌 BMS Protocol + CAN... ");
  if (setupBMSProtocol()) {  // 🔥 ZMIANA: setupCAN() → setupBMSProtocol()
    Serial.println("✅ OK");
    Serial.printf("   🎯 Monitoring %d BMS nodes at 125 kbps\n", systemConfig.activeBmsNodes);
    Serial.printf("   🔋 Node IDs: ");
    for (int i = 0; i < systemConfig.activeBmsNodes; i++) {
      Serial.printf("%d ", systemConfig.bmsNodeIds[i]);
    }
    Serial.println();
  } else {
    Serial.println("❌ FAILED");
    success = false;
  }
  
  // 4. Initialize Modbus TCP Server
  Serial.print("🔗 Modbus TCP Server... ");
  if (setupModbusTCP()) {
    Serial.println("✅ OK");
    Serial.printf("   🎯 Server running on port %d\n", MODBUS_TCP_PORT);
    Serial.printf("   📊 %d holding registers available\n", MODBUS_MAX_HOLDING_REGISTERS);
    Serial.printf("   🔋 %d BMS modules x 200 registers each\n", MAX_BMS_NODES);
  } else {
    Serial.println("❌ FAILED");
    success = false;
  }
  
  Serial.println();
  return success;
}

// === 🔥 MAIN PROCESSING LOOP ===

void processSystemLoop() {
  // PRIORITY 1: Process CAN messages via BMS Protocol (highest priority - real-time data)
  processBMSProtocol();  // 🔥 ZMIANA: processCAN() → processBMSProtocol()
  
  // PRIORITY 2: Process WiFi management  
  wifiManager.process();
  
  // PRIORITY 3: Process Modbus TCP requests
  processModbusTCP();
  
  // PRIORITY 4: Update BMS data timeouts
  checkCommunicationTimeouts();
}

// === 🔥 SYSTEM HEALTH AND MONITORING ===

void checkSystemHealth() {
  // Check individual module health
  bool wifiHealthy = wifiManager.isConnected() || wifiManager.isAPModeActive();
  bool canHealthy = isBMSProtocolHealthy();  // 🔥 ZMIANA: isCANHealthy() → isBMSProtocolHealthy()
  bool modbusHealthy = isModbusServerActive();
  bool bmsHealthy = getActiveBMSCount() > 0;
  
  // Update system state based on module health
  if (wifiHealthy && canHealthy && modbusHealthy) {
    if (currentSystemState == SYSTEM_STATE_ERROR) {
      currentSystemState = SYSTEM_STATE_RUNNING;
      Serial.println("✅ System recovered from error state");
    }
  } else {
    if (currentSystemState == SYSTEM_STATE_RUNNING) {
      currentSystemState = SYSTEM_STATE_ERROR;
      Serial.println("⚠️ System health degraded - entering error state");
      
      // Log specific issues
      if (!wifiHealthy) Serial.println("   ❌ WiFi connection issues");
      if (!canHealthy) Serial.println("   ❌ BMS Protocol/CAN communication issues");
      if (!modbusHealthy) Serial.println("   ❌ Modbus TCP server issues");
      if (!bmsHealthy) Serial.println("   ❌ No active BMS communication");
    }
  }
}


void performSystemDiagnostics() {
  Serial.println();
  Serial.println(F("🔍 ===== SYSTEM DIAGNOSTICS ====="));
  Serial.printf("📅 System Uptime: %s\n", formatUptime(millis() - systemStartTime).c_str());
  Serial.printf("🔄 Current State: %s\n", systemStateToString(currentSystemState).c_str());
  Serial.printf("💾 Free Heap: %s\n", formatBytes(ESP.getFreeHeap()).c_str());
  Serial.println();
  
  // WiFi status
  Serial.printf("📶 WiFi Status: %s\n", wifiManager.isConnected() ? "Connected" : "Disconnected");
  if (wifiManager.isConnected()) {
    Serial.printf("   IP: %s\n", WiFi.localIP().toString().c_str());
    Serial.printf("   RSSI: %d dBm\n", WiFi.RSSI());
  }
  
  // Modbus TCP status
  Serial.printf("🔗 Modbus TCP Server: %s:%d\n", 
                WiFi.localIP().toString().c_str(), MODBUS_TCP_PORT);
  Serial.printf("   State: %s\n", getModbusState() == MODBUS_STATE_RUNNING ? "Running" : "Error");
  
  // 🔥 BMS PROTOCOL STATISTICS (zamiast CAN statistics)
  printBMSProtocolStatistics();
  
  // 🔥 MODBUS STATISTICS
  printModbusStatistics();
  
  Serial.println(F("=================================="));
  Serial.println();
}

// === 🔥 ROZSZERZONY HEARTBEAT Z MULTIPLEXEREM ===

void handleSystemHeartbeat() {
  unsigned long now = millis();
  
  Serial.println();
  Serial.println(F("💓 ===== MODBUS TCP BRIDGE HEARTBEAT ====="));
  
  // System info
  Serial.printf("⏰ Uptime: %s\n", formatUptime(now - systemStartTime).c_str());
  Serial.printf("🔄 State: %s\n", systemStateToString(currentSystemState).c_str());
  Serial.printf("📶 WiFi: %s (RSSI: %d dBm)\n", 
                wifiManager.isConnected() ? "Connected" : "Disconnected",
                WiFi.RSSI());
  Serial.printf("🎯 Modbus TCP Server: %s:%d\n", 
                WiFi.localIP().toString().c_str(), MODBUS_TCP_PORT);
  
  // BMS statistics summary
  int activeBMSCount = getActiveBMSCount();
  Serial.printf("🔋 Active BMS: %d/%d\n", activeBMSCount, systemConfig.activeBmsNodes);
  
  if (activeBMSCount > 0) {
    Serial.println("🔋 ACTIVE BATTERIES STATUS:");
    
    // 🔥 EXTENDED HEARTBEAT dla każdej baterii
    for (int i = 0; i < systemConfig.activeBmsNodes; i++) {
      uint8_t nodeId = systemConfig.bmsNodeIds[i];
      BMSData* bms = getBMSData(nodeId);
      
      if (bms && bms->communicationOk) {
        printBMSHeartbeatExtended(nodeId);  // 🔥 Funkcja z bms_data.h
      } else {
        int batteryIndex = getBMSIndexByNodeId(nodeId);
        uint16_t baseAddr = batteryIndex * 200;
        Serial.printf("🔋 BMS%d [Modbus:%d]: OFFLINE\n", nodeId, baseAddr);
      }
    }
  } else {
    Serial.println("   ❌ No active BMS communication detected");
  }
  
  Serial.println(F("=========================================="));
  Serial.println();
  
  // Visual heartbeat blink (proportional to active BMS count)
  blinkLED(max(1, activeBMSCount), 200);
}

// === CALLBACK FUNCTIONS ===

void onWiFiStateChange(WiFiState_t oldState, WiFiState_t newState) {
  Serial.printf("📡 WiFi state changed: %d -> %d\n", oldState, newState);
}

void onWiFiConnected(String ip) {
  Serial.printf("📡 WiFi connected! IP: %s\n", ip.c_str());
  Serial.printf("   Gateway: %s\n", wifiManager.getGatewayIP().c_str());
  Serial.printf("   RSSI: %d dBm (%s)\n", wifiManager.getRSSI(), wifiManager.getSignalStrength().c_str());
  
  // Blink LED to indicate successful connection
  blinkLED(3, 200);
}

void onWiFiDisconnected() {
  Serial.println("📡 WiFi disconnected");
}

// === UTILITY FUNCTIONS ===

void printStartupBanner() {
  Serial.println();
  Serial.println(String('=', 60));
  Serial.println("🚀 ESP32S3 CAN to Modbus TCP Bridge");
  Serial.println("📋 Version: v4.0.2 - CAN Handler Removed"); 
  Serial.println("📅 Build Date: " BUILD_DATE);
  Serial.println("🏭 Device: " DEVICE_NAME);
  Serial.println("🏗️ Architecture: Modular (5 modules)");
  Serial.println(String('=', 60));
  Serial.println();
  
  Serial.println("📦 Module Overview:");
  Serial.println("   🔧 config.h/cpp         - System configuration");
  Serial.println("   📡 wifi_manager.h/cpp   - WiFi management");
  Serial.println("   🔗 modbus_tcp.h/cpp     - Modbus TCP server");
  Serial.println("   📊 bms_data.h           - 🔥 BMS data (80+ pól)");
  Serial.println("   🛠️ bms_protocol.h/cpp   - 🔥 CAN + BMS protocol (9 parserów + 54 mux)");
  Serial.println("   🛠️ utils.h/cpp          - Utility functions");
  Serial.println();
  Serial.println("❌ REMOVED MODULES:");
  Serial.println("   🚫 can_handler.h/cpp    - Duplikat (funkcje w bms_protocol)");
  Serial.println();
  
  Serial.println("🎯 System Capabilities:");
  Serial.printf("   🔋 %d BMS modules support\n", MAX_BMS_NODES);
  Serial.printf("   📊 %d Modbus registers (%d per BMS)\n", 
               MODBUS_MAX_HOLDING_REGISTERS, 200);
  Serial.printf("   🚌 9 CAN frame types (190,290,310,390,410,510,490,1B0,710)\n");
  Serial.printf("   🔥 54 multiplexer types (Frame 490)\n");
  Serial.printf("   📡 WiFi + AP fallback mode\n");
  Serial.printf("   🎯 CAN-triggered AP mode (CAN ID: 0x%03X)\n", AP_TRIGGER_CAN_ID);
  Serial.printf("   🔗 Modbus TCP Server (port %d)\n", MODBUS_TCP_PORT);
  Serial.println();
}

void printSystemStatus() {
  Serial.println("📊 === SYSTEM STATUS ===");
  Serial.printf("🔄 System State: %s\n", systemStateToString(currentSystemState).c_str());
  Serial.printf("⏰ Boot Time: %lu ms\n", millis() - systemStartTime);
  Serial.printf("💾 Free Heap: %s\n", formatBytes(ESP.getFreeHeap()).c_str());
  Serial.printf("📶 WiFi Status: %s\n", wifiManager.isConnected() ? "Connected" : "Disconnected");
  Serial.printf("🚌 BMS Protocol: %s\n", isBMSProtocolHealthy() ? "Healthy" : "Error");
  Serial.printf("🔗 Modbus Status: %s\n", isModbusServerActive() ? "Running" : "Error");
  Serial.printf("🔋 Active BMS: %d/%d\n", getActiveBMSCount(), systemConfig.activeBmsNodes);
}

void handleSystemState() {
  // Handle different system states
  switch (currentSystemState) {
    case SYSTEM_STATE_ERROR:
      // Attempt recovery if possible
      if (millis() % 30000 == 0) {  // Every 30 seconds
        Serial.println("🔄 Attempting system recovery...");
        // Could reinitialize failed modules here
      }
      break;
      
    case SYSTEM_STATE_RUNNING:
      // Normal operation - no special handling needed
      break;
      
    default:
      break;
  }
}

void emergencyActions() {
  Serial.println("🚨 EMERGENCY: Critical system failure detected!");
  Serial.println("🔄 Attempting system restart in 5 seconds...");
  
  blinkLED(20, 100);  // Rapid emergency blinks
  delay(5000);
  ESP.restart();
}

// === HELPER FUNCTIONS ===
// All utility functions are defined in utils.cpp

// === 🔥 AP TRIGGER HELPER FUNCTIONS ===

/**
 * @brief Wrapper do uruchomienia wyzwalanego trybu AP
 */
void callWiFiManagerStartTriggeredAP() {
  wifiManager.startTriggeredAPMode();
}

/**
 * @brief Wrapper do zatrzymania wyzwalanego trybu AP
 */
void callWiFiManagerStopTriggeredAP() {
  wifiManager.stopTriggeredAPMode();
}