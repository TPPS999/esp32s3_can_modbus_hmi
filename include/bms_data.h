/*
 * bms_data.h - ESP32S3 CAN to Modbus TCP Bridge BMS Data Management
 * 
 * VERSION: v4.0.0 - MODULAR ARCHITECTURE
 * DATE: 2025-08-12
 * DESCRIPTION: BMS data structures and management functions
 */

#ifndef BMS_DATA_H
#define BMS_DATA_H

#include "config.h"

// === BMS LIMITS AND CONSTRAINTS ===
#define BMS_MAX_CURRENT 200.0      // Maximum current [A]
#define BMS_MIN_CELL_VOLTAGE 2.5   // Minimum cell voltage [V]
#define BMS_MAX_CELL_VOLTAGE 4.3   // Maximum cell voltage [V]
#define BMS_MIN_PACK_VOLTAGE 30.0  // Minimum pack voltage [V]
#define BMS_MAX_PACK_VOLTAGE 60.0  // Maximum pack voltage [V]

// === FRAME INFORMATION STRUCTURE ===
struct BMSFrameInfo {
  uint16_t baseId;
  const char* name;
  const char* description;
  bool isCritical;
  unsigned long timeout;
};

extern const BMSFrameInfo frameInfo[BMS_FRAME_TYPE_COUNT];

// === BMS DATA STRUCTURE ===
struct BMSData {
  // === Frame 190 - Basic Data ===
  float batteryVoltage;           // Battery voltage [V]
  float batteryCurrent;           // Battery current [A] (+ = charge, - = discharge)
  float remainingEnergy;          // Remaining energy [kWh]
  float soc;                      // State of Charge [%]
  float fullChargeCapacity;       // Full charge capacity [Ah]
  float remainingCapacity;        // Remaining capacity [Ah]
  float averageCellVoltage;       // Average cell voltage [V]
  float totalVoltage;             // Total voltage [V]
  float packCurrent;              // Pack current [A]
  float packPower;                // Pack power [W]
  
  // === Frame 190 - Error Flags ===
  bool masterError;               // Master error flag
  bool cellVoltageError;          // Cell voltage error
  bool cellTempError;             // Cell temperature error
  bool chargeCurrentError;        // Charge current error
  bool totalVoltageError;         // Total voltage error
  bool chargeHighTempError;       // Charge high temperature error
  bool chargeLowTempError;        // Charge low temperature error
  bool packOverVoltError;         // Pack over voltage error
  bool cellUnderVoltageError;     // Cell under voltage error
  bool cellOverVoltageError;      // Cell over voltage error
  bool cellImbalanceError;        // Cell imbalance error
  bool underTemperatureError;     // Under temperature error
  bool overTemperatureError;      // Over temperature error
  bool overCurrentError;          // Over current error
  
  // === Frame 290 - Cell Voltages ===
  float cellMinVoltage;           // Minimum cell voltage [V]
  float cellMeanVoltage;          // Mean cell voltage [V]
  uint8_t cellMinString;          // String with minimum voltage
  uint8_t cellMinBlock;           // Block with minimum voltage
  uint8_t cellMinCell;            // Cell with minimum voltage
  float cellMaxVoltage;           // Maximum cell voltage [V]
  uint8_t cellMaxString;          // String with maximum voltage
  uint8_t cellMaxBlock;           // Block with maximum voltage
  uint8_t cellMaxCell;            // Cell with maximum voltage
  float cellVoltageDelta;         // Cell voltage delta [V]
  
  // === Frame 310 - SOH & Temperature ===
  float soh;                      // State of Health [%]
  float cellVoltage;              // Current cell voltage [mV]
  float cellTemperature;          // Current cell temperature [°C]
  float dcir;                     // DC internal resistance [mΩ]
  uint8_t stringNumber;           // Current string number
  uint8_t blockNumber;            // Current block number
  uint8_t cellNumber;             // Current cell number
  
  // === Frame 410 - Temperatures ===
  float cellMaxTemperature;       // Maximum cell temperature [°C]
  float cellTempDelta;            // Temperature delta [°C]
  uint8_t cellMaxTempString;      // String with max temperature
  uint8_t cellMaxTempBlock;       // Block with max temperature
  uint8_t cellMaxTempCell;        // Cell with max temperature
  bool readyToCharge;             // Ready to charge flag
  bool readyToDischarge;          // Ready to discharge flag
  bool generalReadyFlag;          // General ready flag
  bool chargeReadyFlag;           // Charge ready flag
  int averageTemperature;         // Average temperature [°C]
  int temperature1;               // Temperature sensor 1 [°C]
  int temperature2;               // Temperature sensor 2 [°C]
  int temperature3;               // Temperature sensor 3 [°C]
  
  // === Frame 510 - Power Limits & I/O ===
  float dccl;                     // Discharge current limit [A]
  float ddcl;                     // Charge current limit [A]
  float maxChargePower;           // Maximum charge power [W]
  float maxDischargePower;        // Maximum discharge power [W]
  uint16_t digitalInputs;         // Digital inputs status
  uint16_t digitalOutputs;        // Digital outputs status
  
  // === Frame 490 - Multiplexed Data ===
  uint8_t mux490Type;             // Multiplexer type
  uint16_t mux490Value;           // Multiplexer value
  
  // Konkretne zmienne z multipleksowanych danych 0x490 (54 typy)
  uint16_t serialNumber0;         // 0x00 - Serial number low
  uint16_t serialNumber1;         // 0x01 - Serial number high
  uint16_t hwVersion0;            // 0x02 - HW version low
  uint16_t hwVersion1;            // 0x03 - HW version high
  uint16_t swVersion0;            // 0x04 - SW version low
  uint16_t swVersion1;            // 0x05 - SW version high
  float factoryEnergy;            // 0x06 - Factory energy [kWh]
  float designCapacity;           // 0x07 - Design capacity [Ah]
  float systemDesignedEnergy;     // 0x0C - System designed energy [kWh]
  float ballancerTempMaxBlock;    // 0x0D - Ballancer temp max block [°C]
  float ltcTempMaxBlock;          // 0x0E - LTC temp max block [°C]
  float inletTemperature;         // 0x0F - Inlet temperature [°C]
  float outletTemperature;        // 0x0F - Outlet temperature [°C]
  uint16_t humidity;              // 0x10 - Humidity [%]
  uint16_t errorsMap0;            // 0x13 - Error map bits 0-15
  uint16_t errorsMap1;            // 0x14 - Error map bits 16-31
  uint16_t errorsMap2;            // 0x15 - Error map bits 32-47
  uint16_t errorsMap3;            // 0x16 - Error map bits 48-63
  uint16_t timeToFullCharge;      // 0x17 - Time to full charge [min]
  uint16_t timeToFullDischarge;   // 0x18 - Time to full discharge [min]
  uint16_t powerOnCounter;        // 0x19 - Power on counter
  uint16_t batteryCycles;         // 0x1A - Battery cycles
  uint16_t ddclCrc;               // 0x1B - DDCL CRC
  uint16_t dcclCrc;               // 0x1C - DCCL CRC
  uint16_t drcclCrc;              // 0x1D - DRCCL CRC
  uint16_t ocvCrc;                // 0x1E - OCV CRC
  uint16_t blVersion0;            // 0x1F - Bootloader version low
  uint16_t blVersion1;            // 0x20 - Bootloader version high
  uint16_t odVersion0;            // 0x21 - Object dictionary version low
  uint16_t odVersion1;            // 0x22 - Object dictionary version high
  uint16_t iotStatus;             // 0x23 - IoT status
  float fullyChargedOn;           // 0x24 - Fully charged ON threshold
  float fullyChargedOff;          // 0x25 - Fully charged OFF threshold
  float fullyDischargedOn;        // 0x26 - Fully discharged ON threshold
  float fullyDischargedOff;       // 0x27 - Fully discharged OFF threshold
  float batteryFullOn;            // 0x28 - Battery full ON threshold
  float batteryFullOff;           // 0x29 - Battery full OFF threshold
  float batteryEmptyOn;           // 0x2A - Battery empty ON threshold
  float batteryEmptyOff;          // 0x2B - Battery empty OFF threshold
  uint16_t numberOfDetectedIMBs;  // 0x2C - Number of detected IMBs
  uint16_t dbcVersion0;           // 0x2D - DBC version low
  uint16_t dbcVersion1;           // 0x2E - DBC version high
  uint16_t configCrc;             // 0x2F - Configuration CRC
  float chargeEnergy0;            // 0x30 - Charge energy low
  float chargeEnergy1;            // 0x31 - Charge energy high
  float dischargeEnergy0;         // 0x32 - Discharge energy low
  float dischargeEnergy1;         // 0x33 - Discharge energy high
  float recuperativeEnergy0;      // 0x34 - Recuperative energy low
  float recuperativeEnergy1;      // 0x35 - Recuperative energy high
  
  // === Frame 1B0 - Additional Data ===
  uint8_t frame1B0Data[8];        // Raw data z ramki 0x1B0
  
  // === Frame 710 - CANopen ===
  uint8_t canOpenState;           // CANopen state
  
  // === Communication & Diagnostics ===
  unsigned long lastUpdate;          // Last update timestamp
  unsigned long lastFrameTime[BMS_FRAME_TYPE_COUNT]; // Last frame timestamps
  bool communicationOk;              // Communication status
  unsigned long packetsReceived;     // Total packets received
  unsigned long parseErrors;         // Parse errors count
  
  // === Frame Counters ===
  unsigned long frame190Count;       // Frame 190 counter
  unsigned long frame290Count;       // Frame 290 counter
  unsigned long frame310Count;       // Frame 310 counter
  unsigned long frame390Count;       // Frame 390 counter
  unsigned long frame410Count;       // Frame 410 counter
  unsigned long frame510Count;       // Frame 510 counter
  unsigned long frame490Count;       // Frame 490 counter
  unsigned long frame1B0Count;       // Frame 1B0 counter
  unsigned long frame710Count;       // Frame 710 counter
};

// === GLOBAL VARIABLES ===
extern BMSData bmsModules[MAX_BMS_NODES];

// === INITIALIZATION FUNCTIONS ===
bool initializeBMSData();
void resetBMSData(int index);
void resetAllBMSData();

// === DATA ACCESS FUNCTIONS ===
int getBatteryIndexFromNodeId(uint8_t nodeId);
uint8_t getNodeIdFromBatteryIndex(int index);
BMSData* getBMSData(uint8_t nodeId);
BMSData* getBMSDataByIndex(int index);

// === COMMUNICATION STATUS FUNCTIONS ===
void updateCommunicationStatus(uint8_t nodeId);
void updateBMSDataTimeouts();
void checkCommunicationTimeouts();
int getActiveBMSCount();
int getActiveBatteryCount();  // Alias for compatibility
bool isBatteryOnline(uint8_t nodeId);
bool hasCommunicationTimeout(uint8_t nodeId);

// === DATA VALIDATION FUNCTIONS ===
bool validateBMSData(const BMSData& data);
bool isDataWithinLimits(const BMSData& data);
void sanitizeBMSData(BMSData& data);
bool hasCriticalErrors(const BMSData& data);

// === STATISTICS FUNCTIONS ===
void updateBMSStatistics(uint8_t nodeId, uint8_t frameType);
void printBMSStatistics();
void resetBMSStatistics();

// === FRAME MANAGEMENT FUNCTIONS ===
void updateFrameTimestamp(uint8_t nodeId, BMSFrameType_t frameType);
bool isFrameTimedOut(uint8_t nodeId, BMSFrameType_t frameType);
void printFrameStatistics(uint8_t nodeId);
unsigned long getLastFrameTime(uint8_t nodeId, BMSFrameType_t frameType);

// === PACK CALCULATION FUNCTIONS ===
float calculatePackVoltage();
float calculatePackCurrent();
float calculatePackSOC();
float calculatePackSOH();
float calculatePackPower();
float calculatePackEnergy();
float calculatePackCapacity();
int calculatePackTemperature();

// === ERROR HANDLING FUNCTIONS ===
bool hasAnyErrors(uint8_t nodeId);
String getErrorSummary(uint8_t nodeId);
uint8_t getErrorFlags(uint8_t nodeId);
uint16_t getCriticalErrorCount();
uint16_t getWarningCount();

// === ADVANCED FUNCTIONS ===
float getMinCellVoltage();
float getMaxCellVoltage();
float getCellVoltageDelta();
int getMinTemperature();
int getMaxTemperature();
int getTemperatureDelta();
bool isAnyBMSCharging();
bool isAnyBMSDischarging();
bool areAllBMSReady();

// === DIAGNOSTIC FUNCTIONS ===
void dumpBMSData(uint8_t nodeId);
void printDetailedBMSStatus(uint8_t nodeId);
void printPackSummary();
void exportBMSDataCSV();

// === MODBUS INTEGRATION FUNCTIONS ===
uint16_t getBMSModbusRegister(uint8_t nodeId, uint16_t registerOffset);
void setBMSModbusRegister(uint8_t nodeId, uint16_t registerOffset, uint16_t value);
void updateAllModbusRegisters();

// === UTILITY FUNCTIONS ===
String formatBMSNodeId(uint8_t nodeId);
String formatBMSStatus(const BMSData& data);
String formatBMSErrors(const BMSData& data);
String formatBMSVoltages(const BMSData& data);
String formatBMSTemperatures(const BMSData& data);

// === CONVERSION UTILITIES ===
uint16_t floatToModbusRegister(float value, float multiplier = 1.0);
float modbusRegisterToFloat(uint16_t reg, float divider = 1.0);
int16_t temperatureToModbusRegister(float temperature);
float modbusRegisterToTemperature(int16_t reg);

// === CALLBACK TYPES ===
typedef void (*BMSDataUpdateCallback)(uint8_t nodeId, BMSFrameType_t frameType);
typedef void (*BMSErrorCallback)(uint8_t nodeId, const char* errorMessage);
typedef void (*BMSTimeoutCallback)(uint8_t nodeId);

// === CALLBACK REGISTRATION ===
void setBMSDataUpdateCallback(BMSDataUpdateCallback callback);
void setBMSErrorCallback(BMSErrorCallback callback);
void setBMSTimeoutCallback(BMSTimeoutCallback callback);

// === CONFIGURATION VALIDATION ===
bool isValidBMSConfiguration();
bool checkBMSConnectivity();
void validateBMSNodeIds();

#endif // BMS_DATA_H