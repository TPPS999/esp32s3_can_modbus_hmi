/*
 * utils.h - ESP32S3 CAN to Modbus TCP Bridge Utilities
 * 
 * VERSION: v3.1.0
 * DATE: 2025-08-12
 * DESCRIPTION: Funkcje pomocnicze, LED, diagnostyki
 */

#ifndef UTILS_H
#define UTILS_H

#include "config.h"

// === LED FUNCTIONS ===
void setupLED();
void blinkLED(int count, int duration = 200);
void setLED(bool state);
void ledHeartbeat(int activeBatteries);

// === DIAGNOSTIC FUNCTIONS ===
void printSystemInfo();
void printMemoryStatus();
void printUptimeInfo();
void printNetworkInfo();

// === STRING UTILITIES ===
String formatUptime(unsigned long milliseconds);
String formatBytes(size_t bytes);
String formatRSSI(int rssi);

// === SYSTEM UTILITIES ===
void systemRestart(int delayMs = 3000);
unsigned long getUptime();
size_t getFreeHeap();
size_t getMaxAllocHeap();

// === VALIDATION UTILITIES ===
bool isValidIPAddress(const String& ip);
bool isValidMACAddress(const String& mac);
bool isValidSSID(const String& ssid);

// === CONVERSION UTILITIES ===
float convertKelvinToCelsius(float kelvin);
float convertCelsiusToKelvin(float celsius);
uint16_t floatToModbusRegister(float value, float multiplier = 1.0);
float modbusRegisterToFloat(uint16_t reg, float divider = 1.0);

// === DEBUG UTILITIES ===
void printHexDump(const uint8_t* data, size_t length, const char* prefix = "");
void printCANFrame(unsigned long canId, unsigned char len, unsigned char* buf);
void printModbusFrame(const uint8_t* frame, size_t length, bool isRequest = true);

// === TIME UTILITIES ===
String getFormattedTime();
String getFormattedDate();
unsigned long getMillisecondsSinceBoot();

#endif // UTILS_H