/*
 * bms_data.h - ESP32S3 CAN to Modbus TCP Bridge - BMS Data Structures
 * 
 * VERSION: v4.0.1 - POPRAWIONA
 * DATE: 2025-08-13 09:15
 * STATUS: ✅ WSZYSTKIE BŁĘDY NAPRAWIONE
 * 
 * Naprawione:
 * - Poprawione nazwy pól: errorMap0-3 → errorsMap0-3
 * - Dodane brakujące pola: lastFrameTime, firstFrameTime, totalFrames
 * - Dodane wszystkie multipleksowane pola z Frame 490
 * - Dodane funkcje zarządzania danymi BMS
 * - Kompatybilność z can_handler.cpp
 */

#ifndef BMS_DATA_H
#define BMS_DATA_H

#include <Arduino.h>
#include "config.h"

// === BMS DATA STRUCTURE ===
struct BMSData {
  // Frame 0x190 - podstawowe dane
  float batteryVoltage = 0.0;          // [V] Napięcie baterii
  float batteryCurrent = 0.0;          // [A] Prąd baterii (+ = ładowanie, - = rozładowanie)
  float remainingEnergy = 0.0;         // [kWh] Pozostała energia
  float soc = 0.0;                     // [%] Stan naładowania
  
  // Frame 0x190 - flagi błędów (NAPRAWIONE NAZWY)
  bool masterError = false;            // Master error flag
  bool cellVoltageError = false;       // Cell voltage error
  bool cellTempMinError = false;       // Cell temperature minimum error
  bool cellTempMaxError = false;       // Cell temperature maximum error
  bool cellVoltageMinError = false;    // Cell voltage minimum error
  bool cellVoltageMaxError = false;    // Cell voltage maximum error
  bool systemShutdown = false;         // System shutdown flag
  bool ibbVoltageSupplyError = false;  // IBB voltage supply error
  
  // Frame 0x290 - napięcia ogniw
  float cellMinVoltage = 0.0;          // [V] Minimalne napięcie ogniwa
  float cellMeanVoltage = 0.0;         // [V] Średnie napięcie ogniwa
  uint8_t minVoltageBlock = 0;         // Blok z min napięciem
  uint8_t minVoltageCell = 0;          // Ogniwo z min napięciem
  uint8_t minVoltageString = 0;        // String z min napięciem
  uint8_t balancingTempMax = 0;        // Max temperatura balansowania
  
  // Frame 0x310 - SOH, temperatura, impedancja
  float soh = 0.0;                     // [%] Stan zdrowia baterii
  float cellVoltage = 0.0;             // [mV] Napięcie ogniwa
  float cellTemperature = 0.0;         // [°C] Temperatura ogniwa
  int8_t cellMinTemperature = 0;       // [°C] Minimalna temperatura ogniwa
  int8_t cellMeanTemperature = 0;      // [°C] Średnia temperatura ogniwa
  float dcir = 0.0;                    // [mΩ] Impedancja wewnętrzna DC
  bool nonEqualStringsRamp = false;    // Non-equal strings ramp
  bool dynamicLimitationTimer = false; // Dynamic limitation timer
  bool overcurrentTimer = false;       // Overcurrent timer
  uint16_t channelMultiplexor = 0;     // Channel multiplexor
  
  // Frame 0x390 - maksymalne napięcia ogniw
  float cellMaxVoltage = 0.0;          // [V] Maksymalne napięcie ogniwa
  float cellVoltageDelta = 0.0;        // [V] Delta napięć ogniw
  uint8_t maxVoltageBlock = 0;         // Blok z max napięciem
  uint8_t maxVoltageCell = 0;          // Ogniwo z max napięciem
  uint8_t maxVoltageString = 0;        // String z max napięciem
  uint8_t afeTemperatureMax = 0;       // Max temperatura AFE
  
  // Frame 0x410 - temperatury i gotowość
  float cellMaxTemperature = 0.0;      // [°C] Maksymalna temperatura ogniwa
  float cellTempDelta = 0.0;           // [°C] Delta temperatur ogniw
  uint8_t maxTempString = 0;           // String z max temperaturą
  uint8_t maxTempBlock = 0;            // Blok z max temperaturą
  uint8_t maxTempSensor = 0;           // Sensor z max temperaturą
  bool readyToCharge = false;          // Gotowość do ładowania
  bool readyToDischarge = false;       // Gotowość do rozładowania
  
  // Frame 0x510 - limity mocy i I/O
  float dccl = 0.0;                    // [A] Discharge Current Continuous Limit
  float ddcl = 0.0;                    // [A] Discharge Power Continuous Limit
  uint8_t inputs = 0;                  // Raw input byte
  uint8_t outputs = 0;                 // Raw output byte
  bool input_IN02 = false;             // Wejście IN02
  bool input_IN01 = false;             // Wejście IN01
  bool relay_AUX4 = false;             // Przekaźnik AUX4
  bool relay_AUX3 = false;             // Przekaźnik AUX3
  bool relay_AUX2 = false;             // Przekaźnik AUX2
  bool relay_AUX1 = false;             // Przekaźnik AUX1
  bool relay_R2 = false;               // Przekaźnik R2
  bool relay_R1 = false;               // Przekaźnik R1
  
  // Frame 0x490 - multipleksowane dane - podstawowe
  uint8_t mux490Type = 0;              // Typ multipleksera (0x00-0x35)
  uint16_t mux490Value = 0;            // Wartość multipleksera (16-bit)
  uint8_t frame490Data[8] = {0};       // Raw data z ramki 0x490
  
  // Frame 0x490 - konkretne zmienne multipleksowane (ROZSZERZONE)
  uint16_t serialNumber0 = 0;          // 0x00 - Serial number low
  uint16_t serialNumber1 = 0;          // 0x01 - Serial number high
  uint16_t hwVersion0 = 0;             // 0x02 - HW version low
  uint16_t hwVersion1 = 0;             // 0x03 - HW version high
  uint16_t swVersion0 = 0;             // 0x04 - SW version low
  uint16_t swVersion1 = 0;             // 0x05 - SW version high
  float factoryEnergy = 0.0;           // 0x06 - Factory energy [kWh]
  float designCapacity = 0.0;          // 0x07 - Design capacity [Ah]
  float systemDesignedEnergy = 0.0;    // 0x0C - System designed energy [kWh]
  float ballancerTempMaxBlock = 0.0;   // 0x0D - Ballancer temp max block [°C]
  float ltcTempMaxBlock = 0.0;         // 0x0E - LTC temp max block [°C]
  float inletTemperature = 0.0;        // 0x0F - Inlet temperature [°C]
  float outletTemperature = 0.0;       // 0x0F - Outlet temperature [°C]
  uint16_t humidity = 0;               // 0x10 - Humidity [%]
  
  // Frame 0x490 - error maps (POPRAWIONE NAZWY)
  uint16_t errorsMap0 = 0;             // 0x13 - Error map bits 0-15
  uint16_t errorsMap1 = 0;             // 0x14 - Error map bits 16-31
  uint16_t errorsMap2 = 0;             // 0x15 - Error map bits 32-47
  uint16_t errorsMap3 = 0;             // 0x16 - Error map bits 48-63
  
  // Frame 0x490 - time and cycles
  uint16_t timeToFullCharge = 0;       // 0x17 - Time to full charge [min]
  uint16_t timeToFullDischarge = 0;    // 0x18 - Time to full discharge [min]
  uint16_t batteryCycles = 0;          // 0x1A - Number of battery cycles
  
  // Frame 0x490 - extended data (dodatkowe typy multipleksera)
  uint16_t numberOfDetectedIMBs = 0;   // 0x1B - Number of detected IMBs
  float balancingEnergy = 0.0;         // 0x1C - Balancing energy [Wh]
  float maxDischargePower = 0.0;       // 0x1D - Max discharge power [W]
  float maxChargePower = 0.0;          // 0x1E - Max charge power [W]
  float maxDischargeEnergy = 0.0;      // 0x1F - Max discharge energy [kWh]
  float maxChargeEnergy = 0.0;         // 0x20 - Max charge energy [kWh]
  float chargeEnergy0 = 0.0;           // 0x30 - Charge energy low [kWh]
  float chargeEnergy1 = 0.0;           // 0x31 - Charge energy high [kWh]
  float dischargeEnergy0 = 0.0;        // 0x32 - Discharge energy low [kWh]
  float dischargeEnergy1 = 0.0;        // 0x33 - Discharge energy high [kWh]
  float recuperativeEnergy0 = 0.0;     // 0x34 - Recuperative energy low [kWh]
  float recuperativeEnergy1 = 0.0;     // 0x35 - Recuperative energy high [kWh]
  
  // Frame 0x490 - versions (rozszerzone)
  uint16_t blVersion0 = 0;             // Bootloader version low
  uint16_t blVersion1 = 0;             // Bootloader version high
  uint16_t appVersion0 = 0;            // Application version low
  uint16_t appVersion1 = 0;            // Application version high
  uint16_t crcApp = 0;                 // Application CRC
  uint16_t crcBoot = 0;                // Bootloader CRC
  
  // Frame 0x1B0 - dodatkowe dane
  uint8_t frame1B0Data[8] = {0};       // Raw data z ramki 0x1B0
  
  // Frame 0x710 - CANopen state
  uint8_t canopenState = 0;            // Stan CANopen
  
  // === DODANE BRAKUJĄCE POLA KOMUNIKACJI ===
  unsigned long lastUpdate = 0;        // Ostatnia aktualizacja [ms]
  unsigned long firstFrameTime = 0;    // Pierwsza ramka [ms] - DODANE
  unsigned long lastFrameTime = 0;     // Ostatnia ramka [ms] - DODANE
  unsigned long lastCommunication = 0; // Ostatnia komunikacja [ms]
  bool communicationOk = false;        // Status komunikacji
  bool communicationActive = false;    // Aktywna komunikacja
  int packetsReceived = 0;             // Liczba odebranych pakietów
  int parseErrors = 0;                 // Liczba błędów parsowania
  int totalFrames = 0;                 // Całkowita liczba ramek - DODANE
  unsigned long frameTimestamps[10];   // Timestamps for frame types
  
  // === STATYSTYKI RAMEK ===
  int frame190Count = 0;               // Licznik ramek 190
  int frame290Count = 0;               // Licznik ramek 290
  int frame310Count = 0;               // Licznik ramek 310
  int frame390Count = 0;               // Licznik ramek 390
  int frame410Count = 0;               // Licznik ramek 410
  int frame510Count = 0;               // Licznik ramek 510
  int frame490Count = 0;               // Licznik ramek 490
  int frame1B0Count = 0;               // Licznik ramek 1B0
  int frame710Count = 0;               // Licznik ramek 710
};

// === BACKWARD COMPATIBILITY ALIASES ===
// Dla kompatybilności z kodem używającym starych nazw
#define errorMap0 errorsMap0
#define errorMap1 errorsMap1
#define errorMap2 errorsMap2
#define errorMap3 errorsMap3

// === GLOBAL BMS DATA ARRAYS ===
extern BMSData bmsModules[MAX_BMS_NODES];
extern uint16_t holdingRegisters[MODBUS_MAX_HOLDING_REGISTERS];

// === BMS DATA MANAGEMENT FUNCTIONS ===

// Initialization functions
bool initializeBMSData();
void resetBMSData(uint8_t nodeId);
void resetAllBMSData();

// Data access functions
BMSData* getBMSData(uint8_t nodeId);
int getBMSIndexByNodeId(uint8_t nodeId);
int getBatteryIndexFromNodeId(uint8_t nodeId);  // Alias dla kompatybilności
bool isBMSNodeActive(uint8_t nodeId);
int getActiveBMSCount();

// Communication status functions
void updateCommunicationStatus(uint8_t nodeId);
void checkCommunicationTimeouts();
bool isBMSCommunicationOK(uint8_t nodeId);
unsigned long getLastUpdateTime(uint8_t nodeId);

// Modbus register mapping functions
void updateModbusRegisters(uint8_t nodeId);
void updateAllModbusRegisters();
uint16_t floatToModbusRegister(float value, float scale = 1000.0);
float modbusRegisterToFloat(uint16_t value, float scale = 1000.0);

// Statistics and diagnostics functions
void printBMSStatistics(uint8_t nodeId);
void printAllBMSStatistics();
void printBMSHeartbeatExtended(uint8_t nodeId);
void printBMSProtocolStatistics();

// Utility functions
String formatBMSStatus(uint8_t nodeId);
String formatBMSErrors(uint8_t nodeId);
String formatMultiplexerData(uint8_t nodeId);

// Data validation functions
bool validateBMSData(const BMSData& data);
bool isDataValid(float value, float min, float max);
void sanitizeBMSData(BMSData& data);

// === INLINE UTILITY FUNCTIONS ===

inline bool isBMSDataRecent(uint8_t nodeId, unsigned long maxAge = 30000) {
  BMSData* bms = getBMSData(nodeId);
  return bms && bms->communicationOk && (millis() - bms->lastUpdate < maxAge);
}

inline float getBMSVoltage(uint8_t nodeId) {
  BMSData* bms = getBMSData(nodeId);
  return bms ? bms->batteryVoltage : 0.0;
}

inline float getBMSCurrent(uint8_t nodeId) {
  BMSData* bms = getBMSData(nodeId);
  return bms ? bms->batteryCurrent : 0.0;
}

inline float getBMSSOC(uint8_t nodeId) {
  BMSData* bms = getBMSData(nodeId);
  return bms ? bms->soc : 0.0;
}

inline bool getBMSMasterError(uint8_t nodeId) {
  BMSData* bms = getBMSData(nodeId);
  return bms ? bms->masterError : true;  // Default to error if no data
}

// === MODBUS REGISTER MAP CONSTANTS ===
#define BMS_REGISTERS_PER_MODULE    125

// Register offsets within each BMS module (base = nodeIndex * 125)
#define BMS_REG_VOLTAGE             0    // Battery voltage [mV]
#define BMS_REG_CURRENT             1    // Battery current [mA]
#define BMS_REG_ENERGY              2    // Remaining energy [0.01kWh]
#define BMS_REG_SOC                 3    // State of charge [0.1%]
#define BMS_REG_MASTER_ERROR        10   // Master error flag (0/1)
#define BMS_REG_SOH                 30   // State of health [0.1%]
#define BMS_REG_READY_CHARGE        55   // Ready to charge (0/1)
#define BMS_REG_READY_DISCHARGE     56   // Ready to discharge (0/1)
#define BMS_REG_MUX_TYPE            70   // Multiplexer type
#define BMS_REG_MUX_VALUE           71   // Multiplexer value
#define BMS_REG_SERIAL_LOW          72   // Serial number low
#define BMS_REG_SERIAL_HIGH         73   // Serial number high
#define BMS_REG_CANOPEN_STATE       110  // CANopen state
#define BMS_REG_COMM_OK             111  // Communication OK (0/1)
#define BMS_REG_PACKETS_RECEIVED    112  // Packets received count

// === MULTIPLEXER TYPE FUNCTIONS ===
const char* getMultiplexerTypeName(uint8_t type);
const char* getMultiplexerTypeUnit(uint8_t type);
float getMultiplexerTypeScale(uint8_t type);

#endif // BMS_DATA_H