/*
 * utils.cpp - ESP32S3 CAN to Modbus TCP Bridge Utilities Implementation
 * 
 * VERSION: v4.0.3 - DODANE BRAKUJĄCE FUNKCJE
 * DATE: 2025-08-17 11:17
 * STATUS: ✅ KOMPLETNE - Dodane wszystkie brakujące funkcje używane w main.cpp
 * 
 * DODANE:
 * - systemStateToString() - konwersja SystemState_t na String
 * - getActiveBMSCount() - liczba aktywnych modułów BMS
 * - formatBytes() with uint32_t support - formatowanie bajtów
 * - Wszystkie funkcje BMS helper functions
 */

#include "utils.h"
#include "bms_data.h"  // 🔥 Potrzebne dla getActiveBMSCount()
#include <WiFi.h>

// === 🔥 BRAKUJĄCE FUNKCJE Z MAIN.CPP ===

/**
 * @brief Konwertuj SystemState_t na String (przeniesione z main.cpp)
 */
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

/**
 * @brief Formatowanie bajtów (przeniesione z main.cpp + rozszerzone)
 */
String formatBytes(uint32_t bytes) {
  if (bytes < 1024) {
    return String(bytes) + " B";
  } else if (bytes < 1024 * 1024) {
    return String(bytes / 1024.0, 1) + " KB";
  } else {
    return String(bytes / (1024.0 * 1024.0), 1) + " MB";
  }
}

/**
 * @brief Overload dla size_t (zgodny z utils.h)
 */
String formatBytes(size_t bytes) {
  return formatBytes((uint32_t)bytes);
}

// getActiveBMSCount is defined in bms_data.cpp

/**
 * @brief Formatowanie uptime z main.cpp (przeniesione tutaj)
 */
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

// === LED FUNCTIONS ===

void setupLED() {
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);
  DEBUG_PRINTF("✅ LED initialized on GPIO%d\n", LED_PIN);
}

void blinkLED(int count, int duration) {
  for (int i = 0; i < count; i++) {
    digitalWrite(LED_PIN, HIGH);
    delay(duration);
    digitalWrite(LED_PIN, LOW);
    if (i < count - 1) delay(duration);
  }
}

void setLED(bool state) {
  digitalWrite(LED_PIN, state ? HIGH : LOW);
}

void ledHeartbeat(int activeBatteries) {
  if (activeBatteries > 0) {
    // Mrugnij liczbą równą liczbie aktywnych baterii
    blinkLED(activeBatteries, 100);
  } else {
    // Długie mrugnięcie jeśli brak komunikacji
    blinkLED(1, 500);
  }
}

// === DIAGNOSTIC FUNCTIONS ===

void printSystemInfo() {
  DEBUG_PRINTLN("\n🖥️ === SYSTEM INFORMATION ===");
  DEBUG_PRINTF("   Chip Model: %s\n", ESP.getChipModel());
  DEBUG_PRINTF("   Chip Revision: %d\n", ESP.getChipRevision());
  DEBUG_PRINTF("   CPU Frequency: %d MHz\n", ESP.getCpuFreqMHz());
  DEBUG_PRINTF("   Flash Size: %s\n", formatBytes(ESP.getFlashChipSize()).c_str());
  DEBUG_PRINTF("   Flash Speed: %d MHz\n", ESP.getFlashChipSpeed() / 1000000);
  DEBUG_PRINTF("   SDK Version: %s\n", ESP.getSdkVersion());
  DEBUG_PRINTF("   Firmware: %s (%s)\n", FIRMWARE_VERSION, BUILD_DATE);
  printMemoryStatus();
  printUptimeInfo();
  DEBUG_PRINTLN("==============================\n");
}

void printMemoryStatus() {
  DEBUG_PRINTLN("\n💾 Memory Status:");
  DEBUG_PRINTF("   Free Heap: %s\n", formatBytes(ESP.getFreeHeap()).c_str());
  DEBUG_PRINTF("   Max Alloc: %s\n", formatBytes(ESP.getMaxAllocHeap()).c_str());
  DEBUG_PRINTF("   Min Free Heap: %s\n", formatBytes(ESP.getMinFreeHeap()).c_str());
  DEBUG_PRINTF("   Heap Size: %s\n", formatBytes(ESP.getHeapSize()).c_str());
  
  // Check memory health
  size_t freeHeap = ESP.getFreeHeap();
  if (freeHeap < 50000) {  // Less than 50KB
    DEBUG_PRINTF("   ⚠️ WARNING: Low memory (%s)\n", formatBytes(freeHeap).c_str());
  }
}

void printUptimeInfo() {
  unsigned long uptime = millis();
  DEBUG_PRINTF("\n⏰ Uptime: %s\n", formatUptime(uptime).c_str());
  DEBUG_PRINTF("   Boot time: %lu ms\n", uptime);
  DEBUG_PRINTF("   Free running time: %.2f hours\n", uptime / 3600000.0);
}

void printNetworkInfo() {
  if (WiFi.status() == WL_CONNECTED) {
    DEBUG_PRINTLN("\n📡 Network Information:");
    DEBUG_PRINTF("   SSID: %s\n", WiFi.SSID().c_str());
    DEBUG_PRINTF("   IP Address: %s\n", WiFi.localIP().toString().c_str());
    DEBUG_PRINTF("   Subnet Mask: %s\n", WiFi.subnetMask().toString().c_str());
    DEBUG_PRINTF("   Gateway: %s\n", WiFi.gatewayIP().toString().c_str());
    DEBUG_PRINTF("   DNS: %s\n", WiFi.dnsIP().toString().c_str());
    DEBUG_PRINTF("   MAC Address: %s\n", WiFi.macAddress().c_str());
    DEBUG_PRINTF("   RSSI: %d dBm (%s)\n", WiFi.RSSI(), formatRSSI(WiFi.RSSI()).c_str());
    DEBUG_PRINTF("   Channel: %d\n", WiFi.channel());
  } else {
    DEBUG_PRINTLN("\n📡 Network: Not connected");
  }
}

// === STRING UTILITIES ===

String formatRSSI(int rssi) {
  if (rssi >= -50) return "Excellent";
  else if (rssi >= -60) return "Good";
  else if (rssi >= -70) return "Fair";
  else if (rssi >= -80) return "Weak";
  else return "Very Weak";
}

// === SYSTEM UTILITIES ===

void systemRestart(int delayMs) {
  DEBUG_PRINTF("🔄 System restart in %d ms...\n", delayMs);
  
  // Visual countdown with LED
  int blinkCount = delayMs / 1000;
  for (int i = blinkCount; i > 0; i--) {
    DEBUG_PRINTF("   Restarting in %d...\n", i);
    blinkLED(1, 200);
    delay(800);
  }
  
  DEBUG_PRINTLN("🚀 Restarting ESP32S3...");
  ESP.restart();
}

unsigned long getUptime() {
  return millis();
}

size_t getFreeHeap() {
  return ESP.getFreeHeap();
}

size_t getMaxAllocHeap() {
  return ESP.getMaxAllocHeap();
}

// === VALIDATION UTILITIES ===

bool isValidIPAddress(const String& ip) {
  int parts = 0;
  int lastDot = -1;
  
  for (int i = 0; i <= ip.length(); i++) {
    if (i == ip.length() || ip[i] == '.') {
      if (i - lastDot - 1 == 0) return false; // Empty part
      String part = ip.substring(lastDot + 1, i);
      int num = part.toInt();
      if (num < 0 || num > 255) return false;
      parts++;
      lastDot = i;
    } else if (!isDigit(ip[i])) {
      return false;
    }
  }
  
  return parts == 4;
}

bool isValidMACAddress(const String& mac) {
  if (mac.length() != 17) return false;
  
  for (int i = 0; i < mac.length(); i++) {
    if (i % 3 == 2) {
      if (mac[i] != ':') return false;
    } else {
      if (!isHexadecimalDigit(mac[i])) return false;
    }
  }
  return true;
}

bool isValidSSID(const String& ssid) {
  return ssid.length() > 0 && ssid.length() <= 32;
}

// === CONVERSION UTILITIES ===

float convertKelvinToCelsius(float kelvin) {
  return kelvin - 273.15;
}

float convertCelsiusToKelvin(float celsius) {
  return celsius + 273.15;
}

// floatToModbusRegister and modbusRegisterToFloat moved to modbus_tcp.cpp to avoid duplicates

// === DEBUG UTILITIES ===

void printHexDump(const uint8_t* data, size_t length, const char* prefix) {
  DEBUG_PRINTF("%s[", prefix);
  for (size_t i = 0; i < length; i++) {
    DEBUG_PRINTF("%02X", data[i]);
    if (i < length - 1) DEBUG_PRINT(" ");
  }
  DEBUG_PRINT("]");
}

// printCANFrame moved to bms_protocol.cpp to avoid duplicates

void printModbusFrame(const uint8_t* frame, size_t length, bool isRequest) {
  if (length < 8) return;
  
  uint16_t transactionId = (frame[0] << 8) | frame[1];
  uint16_t protocolId = (frame[2] << 8) | frame[3];
  uint16_t frameLength = (frame[4] << 8) | frame[5];
  uint8_t slaveId = frame[6];
  uint8_t functionCode = frame[7];
  
  DEBUG_PRINTF("📦 Modbus %s: TxID=%d SlaveID=%d Func=0x%02X Len=%d Data=", 
               isRequest ? "Request" : "Response", 
               transactionId, slaveId, functionCode, frameLength);
  
  if (length > 8) {
    printHexDump(&frame[8], length - 8, "");
  }
  DEBUG_PRINTLN();
}

// === TIME UTILITIES ===

String getFormattedTime() {
  unsigned long totalSeconds = millis() / 1000;
  unsigned long hours = (totalSeconds / 3600) % 24;
  unsigned long minutes = (totalSeconds / 60) % 60;
  unsigned long seconds = totalSeconds % 60;
  
  char timeStr[16];
  sprintf(timeStr, "%02lu:%02lu:%02lu", hours, minutes, seconds);
  return String(timeStr);
}

String getFormattedDate() {
  // Zwraca czas od uruchomienia jako datę względną
  unsigned long days = millis() / (24UL * 60UL * 60UL * 1000UL);
  return "Day " + String(days + 1);
}

unsigned long getMillisecondsSinceBoot() {
  return millis();
}

// === 🔥 ADDITIONAL BMS HELPER FUNCTIONS ===

/**
 * @brief Sprawdź czy system jest zdrowy
 */
bool isSystemHealthy() {
  // Check basic system health indicators
  if (ESP.getFreeHeap() < 20000) return false;  // Less than 20KB free
  if (getActiveBMSCount() == 0) return false;   // No BMS communication
  if (WiFi.status() != WL_CONNECTED) return false; // No WiFi
  
  return true;
}

/**
 * @brief Pobierz procent wykorzystania pamięci
 */
float getMemoryUsagePercent() {
  size_t totalHeap = ESP.getHeapSize();
  size_t freeHeap = ESP.getFreeHeap();
  return ((float)(totalHeap - freeHeap) / totalHeap) * 100.0;
}

/**
 * @brief Sprawdź czy jest krytyczny stan pamięci
 */
bool isMemoryCritical() {
  return ESP.getFreeHeap() < 10000; // Less than 10KB
}

/**
 * @brief Pobierz string stanu WiFi
 */
String getWiFiStatusString() {
  switch (WiFi.status()) {
    case WL_CONNECTED: return "Connected";
    case WL_NO_SSID_AVAIL: return "No SSID Available";
    case WL_CONNECT_FAILED: return "Connection Failed";
    case WL_CONNECTION_LOST: return "Connection Lost";
    case WL_DISCONNECTED: return "Disconnected";
    case WL_IDLE_STATUS: return "Idle";
    default: return "Unknown";
  }
}

// performSystemDiagnostics moved to main.cpp to avoid duplicates

// === ADDITIONAL UTILITY FUNCTIONS ===

void printBootHeader() {
  DEBUG_PRINTLN("\n" + String('=', 50));
  DEBUG_PRINTLN("🚀 ESP32S3 CAN to Modbus TCP Bridge");
  DEBUG_PRINTF("   Version: %s\n", FIRMWARE_VERSION);
  DEBUG_PRINTF("   Build Date: %s\n", BUILD_DATE);
  DEBUG_PRINTF("   Device: %s\n", DEVICE_NAME);
  DEBUG_PRINTLN(String('=', 50));
  printSystemInfo();
}

void printBootProgress(const char* step, bool success) {
  DEBUG_PRINTF("[%s] %s: %s\n", 
               getFormattedTime().c_str(),
               step, 
               success ? "✅ OK" : "❌ FAILED");
}

bool waitForCondition(bool (*condition)(), unsigned long timeoutMs, const String& description) {
  DEBUG_PRINTF("⏳ Waiting for %s (timeout: %lu ms)...", description.c_str(), timeoutMs);
  
  unsigned long startTime = millis();
  while (!condition() && (millis() - startTime) < timeoutMs) {
    delay(100);
    if ((millis() - startTime) % 1000 == 0) {
      DEBUG_PRINT(".");
    }
  }
  
  bool success = condition();
  DEBUG_PRINTF(" %s\n", success ? "✅ OK" : "❌ TIMEOUT");
  return success;
}

void emergencyRestart(const String& reason) {
  DEBUG_PRINTF("🚨 EMERGENCY RESTART: %s\n", reason.c_str());
  
  // Szybkie mrugnięcie LED jako sygnał błędu
  for (int i = 0; i < 10; i++) {
    setLED(true);
    delay(100);
    setLED(false);
    delay(100);
  }
  
  systemRestart(1000);
}