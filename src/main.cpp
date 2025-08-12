/*
 * main.cpp - ESP32S3 CAN to Modbus TCP Bridge - Modular Architecture
 * 
 * VERSION: v4.0.0 - MODULAR ARCHITECTURE
 * DATE: 2025-08-12
 * STATUS: ✅ READY FOR COMPILATION - Fully Modular Design
 * 
 * DESCRIPTION: Main application file using modular architecture with
 * separate modules for configuration, WiFi, CAN, Modbus TCP, and BMS data.
 * 
 * MODULES USED:
 * - config.h/cpp          - System configuration and EEPROM
 * - wifi_manager.h/cpp    - WiFi connection management
 * - can_handler.h/cpp     - CAN communication and parsing
 * - modbus_tcp.h/cpp      - Modbus TCP server implementation
 * - bms_data.h            - BMS data structures and management
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
#include "utils.h"

// === SYSTEM STATE VARIABLES ===
SystemState_t currentSystemState = SYSTEM_STATE_INIT;
unsigned long systemStartTime = 0;
unsigned long lastHeartbeat = 0;
unsigned long lastDiagnostics = 0;
unsigned long lastStatusCheck = 0;

// === HEARTBEAT AND MONITORING ===
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

// === MAIN SETUP FUNCTION ===
void setup() {
  systemStartTime = millis();
  
  // Initialize serial communication
  Serial.begin(115200);
  
  // Wait for Serial Monitor with timeout
  unsigned long serialTimeout = millis() + 5000;
  while (!Serial && millis() < serialTimeout) {
    delay(10);
  }
  
  delay(1000); // Additional stability delay
  
  // Clear serial buffer and print startup
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

// === MAIN LOOP FUNCTION ===
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
  
  // System heartbeat
  if (now - lastHeartbeat >= HEARTBEAT_INTERVAL_MS) {
    lastHeartbeat = now;
    handleSystemHeartbeat();
  }
  
  // System diagnostics
  if (now - lastDiagnostics >= DIAGNOSTICS_INTERVAL_MS) {
    lastDiagnostics = now;
    performSystemDiagnostics();
  }
  
  // Small delay to prevent watchdog issues
  delay(1);
}

// === INITIALIZATION FUNCTIONS ===

void initializeSystem() {
  Serial.println("🔧 Initializing ESP32S3 CAN to Modbus TCP Bridge...");
  Serial.println("📋 System Architecture: Modular v4.0.0");
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
  if (wifiManager.initialize()) {
    Serial.println("✅ OK");
    
    // Set WiFi callbacks
    setWiFiStateChangeCallback(onWiFiStateChange);
    setWiFiConnectedCallback(onWiFiConnected);
    setWiFiDisconnectedCallback(onWiFiDisconnected);
    
    // Start WiFi connection if credentials available
    if (wifiManager.hasCredentials()) {
      Serial.println("🔗 Starting WiFi connection...");
      wifiManager.connect();
    } else {
      Serial.println("⚠️ No WiFi credentials - starting AP mode");
      wifiManager.startAPMode();
    }
  } else {
    Serial.println("❌ FAILED");
    success = false;
  }
  
  // 3. Initialize CAN Handler
  Serial.print("🚌 CAN Handler... ");
  if (setupCAN()) {
    Serial.println("✅ OK");
    Serial.printf("   Monitoring %d BMS nodes at 125 kbps\n", systemConfig.activeBmsNodes);
  } else {
    Serial.println("❌ FAILED");
    success = false;
  }
  
  // 4. Initialize Modbus TCP Server
  Serial.print("🔗 Modbus TCP Server... ");
  if (setupModbusTCP()) {
    Serial.println("✅ OK");
    Serial.printf("   Server running on port %d\n", MODBUS_TCP_PORT);
    Serial.printf("   %d holding registers available\n", MODBUS_MAX_HOLDING_REGISTERS);
  } else {
    Serial.println("❌ FAILED");
    success = false;
  }
  
  Serial.println();
  return success;
}

// === MAIN PROCESSING LOOP ===

void processSystemLoop() {
  // Process modules in order of priority
  
  // PRIORITY 1: Process CAN messages (highest priority - real-time data)
  processCAN();
  
  // PRIORITY 2: Process WiFi management
  wifiManager.process();
  
  // PRIORITY 3: Process Modbus TCP requests
  processModbusTCP();
  
  // PRIORITY 4: Update BMS data and timeouts
  updateBMSDataTimeouts();
}

// === SYSTEM HEALTH AND MONITORING ===

void checkSystemHealth() {
  // Check individual module health
  bool wifiHealthy = wifiManager.isConnected() || wifiManager.isAPModeActive();
  bool canHealthy = isCANHealthy();
  bool modbusHealthy = isModbusHealthy();
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

void performSystemDiagnostics() {
  Serial.println("\n" + String('=', 60));
  Serial.println("🔍 === SYSTEM DIAGNOSTICS ===");
  Serial.printf("📅 System Uptime: %s\n", formatUptime(millis() - systemStartTime).c_str());
  Serial.printf("🔄 Current State: %s\n", systemStateToString(currentSystemState).c_str());
  Serial.println();
  
  // Print module statistics
  printSystemInfo();
  wifiManager.printStatus();
  printCANStatistics();
  printModbusStatistics();
  printBMSStatistics();
  
  Serial.println(String('=', 60) + "\n");
}

void handleSystemHeartbeat() {
  int activeBMSCount = getActiveBMSCount();
  
  Serial.println();
  Serial.println("💓 ===== SYSTEM HEARTBEAT =====");
  Serial.printf("⏰ Uptime: %s\n", formatUptime(millis() - systemStartTime).c_str());
  Serial.printf("🔄 State: %s\n", systemStateToString(currentSystemState).c_str());
  Serial.printf("📡 WiFi: %s", wifiManager.isConnected() ? "Connected" : "Disconnected");
  
  if (wifiManager.isConnected()) {
    Serial.printf(" (%s, %d dBm)", wifiManager.getLocalIP().c_str(), wifiManager.getRSSI());
  } else if (wifiManager.isAPModeActive()) {
    Serial.printf(" (AP Mode: %d clients)", wifiManager.getAPClientCount());
  }
  Serial.println();
  
  // CAN Statistics
  const CanStats& canStats = getCANStatistics();
  Serial.printf("🚌 CAN: %lu frames (%lu valid, %lu errors)\n", 
               canStats.totalFramesReceived, canStats.validBmsFrames, canStats.parseErrors);
  
  if (canStats.lastFrameTime > 0) {
    Serial.printf("   Last frame: %lu sec ago\n", (millis() - canStats.lastFrameTime) / 1000);
  } else {
    Serial.println("   No frames received yet");
  }
  
  // Modbus Statistics
  const ModbusStats& modbusStats = getModbusStatistics();
  Serial.printf("🔗 Modbus: %lu requests (%lu responses, %lu errors)\n",
               modbusStats.totalRequests, modbusStats.successfulResponses, modbusStats.errorResponses);
  
  if (modbusStats.lastRequestTime > 0) {
    Serial.printf("   Last request: %lu sec ago\n", (millis() - modbusStats.lastRequestTime) / 1000);
  } else {
    Serial.println("   No requests received yet");
  }
  
  // BMS Statistics
  Serial.printf("🔋 BMS: %d/%d active modules\n", activeBMSCount, systemConfig.activeBmsNodes);
  
  // Print active BMS modules
  if (activeBMSCount > 0) {
    Serial.print("   Active: ");
    for (int i = 0; i < systemConfig.activeBmsNodes; i++) {
      BMSData* bms = getBMSData(systemConfig.bmsNodeIds[i]);
      if (bms && bms->communicationOk) {
        Serial.printf("BMS%d ", systemConfig.bmsNodeIds[i]);
      }
    }
    Serial.println();
  }
  
  Serial.println("==============================\n");
  
  // Visual heartbeat with LED
  ledHeartbeat(activeBMSCount);
}

void checkCommunicationTimeouts() {
  unsigned long now = millis();
  bool timeoutDetected = false;
  
  for (int i = 0; i < systemConfig.activeBmsNodes; i++) {
    uint8_t nodeId = systemConfig.bmsNodeIds[i];
    BMSData* bms = getBMSData(nodeId);
    
    if (bms && bms->communicationOk) {
      if (now - bms->lastUpdate > COMMUNICATION_TIMEOUT_MS) {
        Serial.printf("⚠️ BMS%d communication timeout!\n", nodeId);
        bms->communicationOk = false;
        timeoutDetected = true;
        
        // Update Modbus register to reflect communication loss
        uint16_t baseAddr = i * MODBUS_REGISTERS_PER_BMS;
        updateModbusRegister(baseAddr + 111, 0); // Communication OK = false
      }
    }
  }
  
  if (timeoutDetected) {
    Serial.printf("🔍 Active BMS modules: %d/%d\n", getActiveBMSCount(), systemConfig.activeBmsNodes);
  }
}

void handleSystemState() {
  switch (currentSystemState) {
    case SYSTEM_STATE_ERROR:
      // Attempt recovery actions
      if (millis() % 30000 == 0) { // Every 30 seconds
        Serial.println("🔄 Attempting system recovery...");
        
        // Try to recover WiFi
        if (!wifiManager.isConnected() && !wifiManager.isAPModeActive()) {
          if (wifiManager.hasCredentials()) {
            wifiManager.reconnect();
          } else {
            wifiManager.startAPMode();
          }
        }
        
        // Try to recover CAN
        if (!isCANHealthy()) {
          Serial.println("🔄 Attempting CAN recovery...");
          resetCAN();
        }
      }
      break;
      
    case SYSTEM_STATE_RUNNING:
      // Normal operation - no special actions needed
      break;
      
    default:
      break;
  }
}

// === CALLBACK IMPLEMENTATIONS ===

void onWiFiStateChange(WiFiState_t oldState, WiFiState_t newState) {
  Serial.printf("📡 WiFi state change: %s → %s\n", 
               wifiStateToString(oldState).c_str(),
               wifiStateToString(newState).c_str());
}

void onWiFiConnected(String ip) {
  Serial.printf("✅ WiFi connected! IP: %s\n", ip.c_str());
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
  Serial.println("📋 Version: v4.0.0 - Modular Architecture");
  Serial.println("📅 Build Date: " BUILD_DATE);
  Serial.println("🏭 Device: " DEVICE_NAME);
  Serial.println("🏗️ Architecture: Fully Modular Design");
  Serial.println(String('=', 60));
  Serial.println();
  
  Serial.println("📦 Module Overview:");
  Serial.println("   🔧 config.h/cpp         - System configuration");
  Serial.println("   📡 wifi_manager.h/cpp   - WiFi management");
  Serial.println("   🚌 can_handler.h/cpp    - CAN communication");
  Serial.println("   🔗 modbus_tcp.h/cpp     - Modbus TCP server");
  Serial.println("   📊 bms_data.h           - BMS data management");
  Serial.println("   🛠️ utils.h/cpp          - Utility functions");
  Serial.println();
  
  Serial.println("🎯 System Capabilities:");
  Serial.printf("   🔋 %d BMS modules support\n", MAX_BMS_NODES);
  Serial.printf("   📊 %d Modbus registers (%d per BMS)\n", 
               MODBUS_MAX_HOLDING_REGISTERS, MODBUS_REGISTERS_PER_BMS);
  Serial.printf("   🚌 %d CAN frame types\n", 9);
  Serial.printf("   📡 WiFi + AP fallback mode\n");
  Serial.printf("   🔗 Modbus TCP Server (port %d)\n", MODBUS_TCP_PORT);
  Serial.println();
}

void printSystemStatus() {
  Serial.println("📊 === SYSTEM STATUS ===");
  Serial.printf("🔄 System State: %s\n", systemStateToString(currentSystemState).c_str());
  Serial.printf("⏰ Boot Time: %lu ms\n", millis() - systemStartTime);
  Serial.printf("💾 Free Heap: %s\n", formatBytes(ESP.getFreeHeap()).c_str());
  Serial.printf("📶 WiFi Status: %s\n", wifiStateToString(wifiManager.getState()).c_str());
  Serial.printf("🚌 CAN Status: %s\n", isCANHealthy() ? "Healthy" : "Error");
  Serial.printf("🔗 Modbus Status: %s\n", isModbusHealthy() ? "Healthy" : "Error");
  Serial.printf("🔋 Active BMS: %d/%d\n", getActiveBMSCount(), systemConfig.activeBmsNodes);
  Serial.println("========================");
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