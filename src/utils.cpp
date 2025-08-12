/*
 * utils.cpp - ESP32S3 CAN to Modbus TCP Bridge Utilities Implementation
 * 
 * VERSION: v3.1.0
 * DATE: 2025-08-12
 */

#include "utils.h"
#include <WiFi.h>
#include <regex>

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
}

void printUptimeInfo() {
  DEBUG_PRINTLN("\n⏰ Uptime Information:");
  DEBUG_PRINTF("   Uptime: %s\n", formatUptime(millis()).c_str());
  DEBUG_PRINTF("   Boot Time: %lu ms\n", millis());
}

void printNetworkInfo() {
  DEBUG_PRINTLN("\n📡 Network Information:");
  if (WiFi.status() == WL_CONNECTED) {
    DEBUG_PRINTF("   Status: Connected\n");
    DEBUG_PRINTF("   SSID: %s\n", WiFi.SSID().c_str());
    DEBUG_PRINTF("   IP Address: %s\n", WiFi.localIP().toString().c_str());
    DEBUG_PRINTF("   Gateway: %s\n", WiFi.gatewayIP().toString().c_str());
    DEBUG_PRINTF("   Subnet: %s\n", WiFi.subnetMask().toString().c_str());
    DEBUG_PRINTF("   DNS: %s\n", WiFi.dnsIP().toString().c_str());
    DEBUG_PRINTF("   MAC Address: %s\n", WiFi.macAddress().c_str());
    DEBUG_PRINTF("   RSSI: %s\n", formatRSSI(WiFi.RSSI()).c_str());
  } else {
    DEBUG_PRINTF("   Status: Disconnected (%d)\n", WiFi.status());
  }
}

// === STRING UTILITIES ===

String formatUptime(unsigned long milliseconds) {
  unsigned long seconds = milliseconds / 1000;
  unsigned long minutes = seconds / 60;
  unsigned long hours = minutes / 60;
  unsigned long days = hours / 24;
  
  hours %= 24;
  minutes %= 60;
  seconds %= 60;
  
  String uptime = "";
  if (days > 0) uptime += String(days) + "d ";
  if (hours > 0) uptime += String(hours) + "h ";
  if (minutes > 0) uptime += String(minutes) + "m ";
  uptime += String(seconds) + "s";
  
  return uptime;
}

String formatBytes(size_t bytes) {
  if (bytes < 1024) return String(bytes) + " B";
  else if (bytes < 1024 * 1024) return String(bytes / 1024.0, 1) + " KB";
  else if (bytes < 1024 * 1024 * 1024) return String(bytes / (1024.0 * 1024.0), 1) + " MB";
  else return String(bytes / (1024.0 * 1024.0 * 1024.0), 1) + " GB";
}

String formatRSSI(int rssi) {
  String quality;
  if (rssi >= -30) quality = "Excellent";
  else if (rssi >= -50) quality = "Good";
  else if (rssi >= -70) quality = "Fair";
  else if (rssi >= -90) quality = "Poor";
  else quality = "Very Poor";
  
  return String(rssi) + " dBm (" + quality + ")";
}

// === SYSTEM UTILITIES ===

void systemRestart(int delayMs) {
  DEBUG_PRINTF("🔄 System restart in %d ms...\n", delayMs);
  setLED(true);
  delay(delayMs);
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
  if (ip == "DHCP") return true;
  
  // Prosta walidacja IP address
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

uint16_t floatToModbusRegister(float value, float multiplier) {
  int32_t intValue = (int32_t)(value * multiplier);
  // Clamp to 16-bit range
  if (intValue > 65535) intValue = 65535;
  if (intValue < 0) intValue = 0;
  return (uint16_t)intValue;
}

float modbusRegisterToFloat(uint16_t reg, float divider) {
  return (float)reg / divider;
}

// === DEBUG UTILITIES ===

void printHexDump(const uint8_t* data, size_t length, const char* prefix) {
  DEBUG_PRINTF("%s[", prefix);
  for (size_t i = 0; i < length; i++) {
    DEBUG_PRINTF("%02X", data[i]);
    if (i < length - 1) DEBUG_PRINT(" ");
  }
  DEBUG_PRINT("]");
}

void printCANFrame(unsigned long canId, unsigned char len, unsigned char* buf) {
  DEBUG_PRINTF("🔍 CAN: ID=0x%03lX Len=%d Data=", canId, len);
  printHexDump(buf, len, "");
  
  // Identyfikacja typu ramki
  if ((canId & 0xFF80) == BMS_FRAME_190_BASE) DEBUG_PRINT(" (Basic data)");
  else if ((canId & 0xFF80) == BMS_FRAME_290_BASE) DEBUG_PRINT(" (Cell voltages)");
  else if ((canId & 0xFF80) == BMS_FRAME_310_BASE) DEBUG_PRINT(" (SOH/Temperature)");
  else if ((canId & 0xFF80) == BMS_FRAME_390_BASE) DEBUG_PRINT(" (Max limits)");
  else if ((canId & 0xFF80) == BMS_FRAME_410_BASE) DEBUG_PRINT(" (Temperatures)");
  else if ((canId & 0xFF80) == BMS_FRAME_510_BASE) DEBUG_PRINT(" (Power limits)");
  else if ((canId & 0xFF80) == BMS_FRAME_490_BASE) DEBUG_PRINT(" (Multiplexed)");
  else if ((canId & 0xFF80) == BMS_FRAME_1B0_BASE) DEBUG_PRINT(" (Additional)");
  else if ((canId & 0xFF80) == BMS_FRAME_710_BASE) DEBUG_PRINT(" (CANopen)");
  else if (canId == AP_TRIGGER_CAN_ID) DEBUG_PRINT(" (AP TRIGGER!)");
  
  DEBUG_PRINTLN();
}

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

void printBootProgress(const String& step, bool success) {
  DEBUG_PRINTF("[%s] %s: %s\n", 
               getFormattedTime().c_str(),
               step.c_str(), 
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