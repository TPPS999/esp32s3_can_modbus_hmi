/*
 * main.cpp - ESP32S3 CAN to Modbus TCP Bridge - Complete Implementation
 * 
 * VERSION: v4.0.1 - COMPLETE WITH ALL v3.0.0 FUNCTIONALITY
 * DATE: 2025-08-13
 * STATUS: ✅ READY - Wykorzystuje wszystkie nowe funkcje z uzupełnionych modułów
 * 
 * DESCRIPTION: Główny plik aplikacji z pełną funkcjonalnością z v3.0.0
 * - Wszystkie 9 parserów ramek CAN + 54 typy multipleksera
 * - Kompletne mapowanie 125 rejestrów Modbus per BMS
 * - Rozszerzony heartbeat z danymi multipleksera
 * - Zaawansowane statystyki i diagnostyka
 * - Modularny design v4.0.0 + funkcjonalność v3.0.0
 * 
 * MODULES USED:
 * - config.h/cpp          - System configuration and EEPROM
 * - wifi_manager.h/cpp    - WiFi connection management
 * - can_handler.h/cpp     - CAN communication and parsing
 * - modbus_tcp.h/cpp      - Modbus TCP server implementation
 * - bms_data.h            - BMS data structures and management (ROZSZERZONA)
 * - bms_protocol.h/cpp    - BMS protocol parsing (KOMPLETNA)
 * - utils.h/cpp           - Utility functions and diagnostics
 */

// === CORE INCLUDES ===
#include <Arduino.h>

// === PROJECT MODULES ===
#include "config.h"
#include "wifi_manager.h"
#include "can_handler.h"
#include "modbus_tcp.h"
#include "bms_data.h"
#include "bms_protocol.h"  // 🔥 NOWY: Kompletny protokół BMS
#include "utils.h"

// === SYSTEM STATE VARIABLES ===
SystemState_t currentSystemState = SYSTEM_STATE_INIT;
unsigned long systemStartTime = 0;
unsigned long lastHeartbeat = 0;
unsigned long lastDiagnostics = 0;
unsigned long lastStatusCheck = 0;

// === 🔥 HEARTBEAT AND MONITORING (rozszerzone z v3.0.0) ===
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
void checkCommunicationTimeouts();
void handleSystemState();
void emergencyActions();
void printStartupBanner();
void printSystemStatus();

// === CALLBACK FUNCTIONS ===
void onWiFiStateChange(WiFiState_t oldState, WiFiState_t newState);
void onWiFiConnected(String ip);
void onWiFiDisconnected();

// === 🔥 MAIN SETUP FUNCTION (rozszerzony) ===
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

// === 🔥 MAIN LOOP FUNCTION (zoptymalizowany) ===
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
  
  // 🔥 ROZSZERZONY HEARTBEAT z danymi multipleksera (z v3.0.0)
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
  Serial.println("📋 System Architecture: Modular v4.0.1 with v3.0.0 Functionality");
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
  
  // 🔥 Initialize BMS data structures (rozszerzone z v3.0.0)
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
  
  // 3. Initialize CAN Handler
  Serial.print("🚌 CAN Handler... ");
  if (setupCAN()) {
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
    Serial.printf("   🔋 %d BMS modules x 125 registers each\n", MAX_BMS_NODES);
  } else {
    Serial.println("❌ FAILED");
    success = false;
  }
  
  Serial.println();
  return success;
}

// === 🔥 MAIN PROCESSING LOOP (zoptymalizowany z v3.0.0) ===

void processSystemLoop() {
  // PRIORITY 1: Process CAN messages (highest priority - real-time data)
  processCAN();
  
  // PRIORITY 2: Process WiFi management  
  wifiManager.process();
  
  // PRIORITY 3: Process Modbus TCP requests
  processModbusTCP();
  
  // PRIORITY 4: Update BMS data timeouts
  checkCommunicationTimeouts();
}

// === 🔥 SYSTEM HEALTH AND MONITORING (rozszerzone) ===

void checkSystemHealth() {
  // Check individual module health
  bool wifiHealthy = wifiManager.isConnected() || wifiManager.isAPModeActive();
  bool canHealthy = isCANHealthy();
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
      if (!canHealthy) Serial.println("   ❌ CAN communication issues");
      if (!modbusHealthy) Serial.println("   ❌ Modbus TCP server issues");
      if (!bmsHealthy) Serial.println("   ❌ No active BMS communication");
    }
  }
}

void checkCommunicationTimeouts() {
  static unsigned long lastCommCheck = 0;
  unsigned long now = millis();
  
  if (now - lastCommCheck >= 10000) {  // Check every 10 seconds
    for (int i = 0; i < systemConfig.activeBmsNodes; i++) {
      uint8_t nodeId = systemConfig.bmsNodeIds[i];
      BMSData* bms = getBMSData(nodeId);
      
      if (bms && bms->communicationOk && (now - bms->lastUpdate > COMMUNICATION_TIMEOUT_MS)) {
        Serial.printf("⚠️ BMS%d communication timeout!\n", nodeId);
        bms->communicationOk = false;
        
        // Update Modbus register for communication status
        int batteryIndex = getBMSIndexByNodeId(nodeId);
        if (batteryIndex >= 0) {
          uint16_t baseAddr = batteryIndex * 125;
          holdingRegisters[baseAddr + 111] = 0;  // Communication OK = false
        }
      }
    }
    lastCommCheck = now;
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
  
  // 🔥 PROTOKÓŁ BMS STATISTICS (NOWE z v3.0.0)
  printBMSProtocolStatistics();
  
  // 🔥 MODBUS STATISTICS (NOWE z v3.0.0)  
  printModbusStatistics();
  
  Serial.println(F("=================================="));
  Serial.println();
}

// === 🔥 ROZSZERZONY HEARTBEAT Z MULTIPLEXEREM (z v3.0.0) ===

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
    
    // 🔥 EXTENDED HEARTBEAT dla każdej baterii (z v3.0.0)
    for (int i = 0; i < systemConfig.activeBmsNodes; i++) {
      uint8_t nodeId = systemConfig.bmsNodeIds[i];
      BMSData* bms = getBMSData(nodeId);
      
      if (bms && bms->communicationOk) {
        printBMSHeartbeatExtended(nodeId);  // 🔥 NOWA FUNKCJA z kompletnego Modbus
      } else {
        int batteryIndex = getBMSIndexByNodeId(nodeId);
        uint16_t baseAddr = batteryIndex * 125;
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
  Serial.println("📋 Version: v4.0.1 - Complete Implementation"); 
  Serial.println("📅 Build Date: " BUILD_DATE);
  Serial.println("🏭 Device: " DEVICE_NAME);
  Serial.println("🏗️ Architecture: Modular + Complete v3.0.0 Functionality");
  Serial.println(String('=', 60));
  Serial.println();
  
  Serial.println("📦 Module Overview:");
  Serial.println("   🔧 config.h/cpp         - System configuration");
  Serial.println("   📡 wifi_manager.h/cpp   - WiFi management");
  Serial.println("   🚌 can_handler.h/cpp    - CAN communication");
  Serial.println("   🔗 modbus_tcp.h/cpp     - Modbus TCP server");
  Serial.println("   📊 bms_data.h           - 🔥 BMS data (ROZSZERZONA 80+ pól)");
  Serial.println("   🛠️ bms_protocol.h/cpp   - 🔥 BMS protocol (KOMPLETNA 9 parserów)");
  Serial.println("   🛠️ utils.h/cpp          - Utility functions");
  Serial.println();
  
  Serial.println("🎯 System Capabilities:");
  Serial.printf("   🔋 %d BMS modules support\n", MAX_BMS_NODES);
  Serial.printf("   📊 %d Modbus registers (%d per BMS)\n", 
               MODBUS_MAX_HOLDING_REGISTERS, 125);
  Serial.printf("   🚌 9 CAN frame types (190,290,310,390,410,510,490,1B0,710)\n");
  Serial.printf("   🔥 54 multiplexer types (Frame 490)\n");
  Serial.printf("   📡 WiFi + AP fallback mode\n");
  Serial.printf("   🔗 Modbus TCP Server (port %d)\n", MODBUS_TCP_PORT);
  Serial.println();
  
  Serial.println("🔥 NEW FEATURES from v3.0.0:");
  Serial.println("   📊 Complete 125 registers per BMS mapping");
  Serial.println("   🔄 All 54 multiplexer types (Frame 490)");
  Serial.println("   📈 Extended heartbeat with multiplexer data");
  Serial.println("   🔍 Advanced diagnostics & frame statistics");
  Serial.println("   📋 Error maps, versions, CRC validation");
  Serial.println("   🚀 Enhanced communication monitoring");
  Serial.println();
}

void printSystemStatus() {
  Serial.println("📊 === SYSTEM STATUS ===");
  Serial.printf("🔄 System State: %s\n", systemStateToString(currentSystemState).c_str());
  Serial.printf("⏰ Boot Time: %lu ms\n", millis() - systemStartTime);
  Serial.printf("💾 Free Heap: %s\n", formatBytes(ESP.getFreeHeap()).c_str());
  Serial.printf("📶 WiFi Status: %s\n", wifiManager.isConnected() ? "Connected" : "Disconnected");
  Serial.printf("🚌 CAN Status: %s\n", isCANHealthy() ? "Healthy" : "Error");
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

String systemStateToString(SystemState_t state) {
  switch (state) {
    case SYSTEM_STATE_INIT: return "Initializing";
    case SYSTEM_STATE_INITIALIZING: return "Starting Modules";
    case SYSTEM_STATE_RUNNING: return "Running";
    case SYSTEM_STATE_ERROR: return "Error";
    case SYSTEM_STATE_RECOVERY: return "Recovery";
    case SYSTEM_STATE_SHUTDOWN: return "Shutdown";
    default: return "Unknown";
  }
}

String formatUptime(unsigned long milliseconds) {
  unsigned long seconds = milliseconds / 1000;
  unsigned long minutes = seconds / 60;
  unsigned long hours = minutes / 60;
  unsigned long days = hours / 24;
  
  if (days > 0) {
    return String(days) + "d " + String(hours % 24) + "h " + String(minutes % 60) + "m";
  } else if (hours > 0) {
    return String(hours) + "h " + String(minutes % 60) + "m " + String(seconds % 60) + "s";
  } else if (minutes > 0) {
    return String(minutes) + "m " + String(seconds % 60) + "s";
  } else {
    return String(seconds) + "s";
  }
}

String formatBytes(uint32_t bytes) {
  if (bytes < 1024) {
    return String(bytes) + " B";
  } else if (bytes < 1024 * 1024) {
    return String(bytes / 1024.0, 1) + " KB";
  } else {
    return String(bytes / (1024.0 * 1024.0), 1) + " MB";
  }
}