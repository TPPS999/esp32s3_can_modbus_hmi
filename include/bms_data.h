/*
 * bms_data.h - ESP32S3 CAN to Modbus TCP Bridge BMS Data Management
 * 
 * VERSION: v4.0.1 - COMPLETE IMPLEMENTATION
 * DATE: 2025-08-13
 * STATUS: ✅ READY - Kompletna struktura z oryginalnego kodu v3.0.0
 * 
 * DESCRIPTION: Kompletna struktura BMS z wszystkimi 54 typami multipleksera
 * oraz pełną funkcjonalnością z oryginalnego kodu.
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

// === MULTIPLEXER TYPE DEFINITIONS (Frame 490 - 54 typy) ===
#define MUX490_SERIAL_NUMBER_0      0x00
#define MUX490_SERIAL_NUMBER_1      0x01
#define MUX490_HW_VERSION_0         0x02
#define MUX490_HW_VERSION_1         0x03
#define MUX490_SW_VERSION_0         0x04
#define MUX490_SW_VERSION_1         0x05
#define MUX490_FACTORY_ENERGY       0x06
#define MUX490_DESIGN_CAPACITY      0x07
#define MUX490_SYSTEM_ENERGY        0x0C
#define MUX490_BALLANCER_TEMP       0x0D
#define MUX490_LTC_TEMP             0x0E
#define MUX490_INLET_OUTLET_TEMP    0x0F
#define MUX490_HUMIDITY             0x10
#define MUX490_ERROR_MAP_0          0x13
#define MUX490_ERROR_MAP_1          0x14
#define MUX490_ERROR_MAP_2          0x15
#define MUX490_ERROR_MAP_3          0x16
#define MUX490_TIME_TO_FULL_CHARGE  0x17
#define MUX490_TIME_TO_FULL_DISCHARGE 0x18
#define MUX490_POWER_ON_COUNTER     0x19
#define MUX490_BATTERY_CYCLES       0x1A
#define MUX490_DDCL_CRC             0x1B
#define MUX490_DCCL_CRC             0x1C
#define MUX490_DRCCL_CRC            0x1D
#define MUX490_OCV_CRC              0x1E
#define MUX490_BL_VERSION_0         0x1F
#define MUX490_BL_VERSION_1         0x20
#define MUX490_OD_VERSION_0         0x21
#define MUX490_OD_VERSION_1         0x22
#define MUX490_IOT_STATUS           0x23
#define MUX490_FULLY_CHARGED_ON     0x24
#define MUX490_FULLY_CHARGED_OFF    0x25
#define MUX490_FULLY_DISCHARGED_ON  0x26
#define MUX490_FULLY_DISCHARGED_OFF 0x27
#define MUX490_BATTERY_FULL_ON      0x28
#define MUX490_BATTERY_FULL_OFF     0x29
#define MUX490_BATTERY_EMPTY_ON     0x2A
#define MUX490_BATTERY_EMPTY_OFF    0x2B
#define MUX490_DETECTED_IMBS        0x2C
#define MUX490_DBC_VERSION_0        0x2D
#define MUX490_DBC_VERSION_1        0x2E
#define MUX490_CONFIG_CRC           0x2F
#define MUX490_CHARGE_ENERGY_0      0x30
#define MUX490_CHARGE_ENERGY_1      0x31
#define MUX490_DISCHARGE_ENERGY_0   0x32
#define MUX490_DISCHARGE_ENERGY_1   0x33
#define MUX490_RECUPERATIVE_ENERGY_0 0x34
#define MUX490_RECUPERATIVE_ENERGY_1 0x35

// === FRAME INFORMATION STRUCTURE ===
struct BMSFrameInfo {
  uint16_t baseId;
  const char* name;
  const char* description;
  bool isCritical;
  unsigned long timeout;
};

extern const BMSFrameInfo frameInfo[BMS_FRAME_TYPE_COUNT];

// === 🔥 KOMPLETNA STRUKTURA BMSData (z oryginalnego v3.0.0) ===
struct BMSData {
  // === Frame 190 - Basic Data ===
  float batteryVoltage;           // Battery voltage [V]
  float batteryCurrent;           // Battery current [A] (+ = charge, - = discharge)
  float remainingEnergy;          // Remaining energy [kWh]
  float soc;                      // State of Charge [%]
  
  // === Frame 190 - Error Flags (z oryginalnego kodu) ===
  bool masterError;               // Master error flag
  bool cellVoltageError;          // Cell voltage error
  bool cellTempMinError;          // Cell temperature min error
  bool cellTempMaxError;          // Cell temperature max error
  bool cellVoltageMinError;       // Cell voltage min error
  bool cellVoltageMaxError;       // Cell voltage max error
  bool systemShutdown;            // System shutdown flag
  bool ibbVoltageSupplyError;     // IBB voltage supply error
  
  // === Frame 290 - Cell Voltages ===
  float cellMinVoltage;           // Minimum cell voltage [V]
  float cellMeanVoltage;          // Mean cell voltage [V]
  uint8_t minVoltageBlock;        // Block with minimum voltage
  uint8_t minVoltageCell;         // Cell with minimum voltage
  uint8_t minVoltageString;       // String with minimum voltage
  uint8_t balancingTempMax;       // Balancing temperature max [°C]
  
  // === Frame 310 - SOH, Temperature, Impedance ===
  float soh;                      // State of Health [%]
  float cellVoltage;              // Cell voltage [mV]
  float cellTemperature;          // Cell temperature [°C]
  float dcir;                     // DC Internal Resistance [mΩ]
  bool nonEqualStringsRamp;       // Non-equal strings ramp flag
  bool dynamicLimitationTimer;    // Dynamic limitation timer flag
  bool overcurrentTimer;          // Overcurrent timer flag
  uint16_t channelMultiplexor;    // Channel multiplexor
  
  // === Frame 390 - Max Voltages ===
  float cellMaxVoltage;           // Maximum cell voltage [V]
  float cellVoltageDelta;         // Cell voltage delta [V]
  uint8_t maxVoltageBlock;        // Block with maximum voltage
  uint8_t maxVoltageCell;         // Cell with maximum voltage
  uint8_t maxVoltageString;       // String with maximum voltage
  uint8_t afeTemperatureMax;      // AFE temperature max [°C]
  
  // === Frame 410 - Temperatures & Ready States ===
  float cellMaxTemperature;       // Maximum cell temperature [°C]
  float cellTempDelta;            // Cell temperature delta [°C]
  uint8_t maxTempString;          // String with max temperature
  uint8_t maxTempBlock;           // Block with max temperature
  uint8_t maxTempSensor;          // Sensor with max temperature
  bool readyToCharge;             // Ready to charge flag
  bool readyToDischarge;          // Ready to discharge flag
  
  // === Frame 510 - Power Limits & I/O ===
  float dccl;                     // Discharge current limit [A]
  float ddcl;                     // Charge current limit [A]
  bool input_IN02;                // Digital input IN02
  bool input_IN01;                // Digital input IN01
  bool relay_AUX4;                // Relay AUX4 status
  bool relay_AUX3;                // Relay AUX3 status
  bool relay_AUX2;                // Relay AUX2 status
  bool relay_AUX1;                // Relay AUX1 status
  bool relay_R2;                  // Relay R2 status
  bool relay_R1;                  // Relay R1 status
  
  // === Frame 490 - Multiplexed Data (54 typy!) ===
  uint8_t mux490Type;             // Multiplexer type (0x00-0x35)
  uint16_t mux490Value;           // Raw multiplexer value
  
  // 🔥 WSZYSTKIE 54 TYPY MULTIPLEKSERA (z oryginalnego v3.0.0):
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
  uint8_t frame1B0Data[8];        // Raw data from frame 1B0
  
  // === Frame 710 - CANopen State ===
  uint8_t canopenState;           // CANopen state (zgodnie z v3.0.0)
  
  // === Communication & Diagnostics (z oryginalnego v3.0.0) ===
  unsigned long lastUpdate;       // Last update timestamp [ms]
  bool communicationOk;           // Communication status
  int packetsReceived;            // Total packets received
  int parseErrors;                // Parse errors counter
  
  // 🔥 LICZNIKI WSZYSTKICH RAMEK (z oryginalnego v3.0.0):
  int frame190Count;              // Frame 190 counter
  int frame290Count;              // Frame 290 counter
  int frame310Count;              // Frame 310 counter
  int frame390Count;              // Frame 390 counter
  int frame410Count;              // Frame 410 counter
  int frame510Count;              // Frame 510 counter
  int frame490Count;              // Frame 490 counter (multiplexed)
  int frame1B0Count;              // Frame 1B0 counter (additional)
  int frame710Count;              // Frame 710 counter (CANopen)
};

// === GLOBAL VARIABLES (kompatybilność z v3.0.0) ===
extern BMSData bmsModules[MAX_BMS_NODES];

// === BMS DATA MANAGEMENT FUNCTIONS ===
bool initializeBMSData();
BMSData* getBMSData(uint8_t nodeId);
BMSData* getBMSDataByIndex(int index);
int getBMSIndexByNodeId(uint8_t nodeId);  // Zgodność z v3.0.0: getBatteryIndexFromNodeId
void resetBMSData(uint8_t nodeId);
void resetAllBMSData();

// === COMMUNICATION STATUS FUNCTIONS (z v3.0.0) ===
void updateCommunicationStatus(uint8_t nodeId);
bool isBMSCommunicationHealthy(uint8_t nodeId);
unsigned long getLastUpdateTime(uint8_t nodeId);
int getTotalPacketsReceived(uint8_t nodeId);

// === FRAME TIMESTAMP FUNCTIONS ===
void updateFrameTimestamp(uint8_t nodeId, BMSFrameType_t frameType);
unsigned long getFrameTimestamp(uint8_t nodeId, BMSFrameType_t frameType);

// === VALIDATION FUNCTIONS ===
bool validateBMSData(uint8_t nodeId);
bool isVoltageInRange(float voltage);
bool isCurrentInRange(float current);
bool isTemperatureInRange(float temperature);
bool isSOCInRange(float soc);
bool isSOHInRange(float soh);

// === STATISTICS FUNCTIONS ===
int getActiveBMSCount();
int getTotalFramesReceived();
int getTotalParseErrors();
float getAverageSOC();
float getAverageVoltage();
float getTotalCurrent();
float getTotalEnergy();

// === UTILITY FUNCTIONS ===
void printBMSData(uint8_t nodeId);
void printBMSSummary();
String formatBMSStatus(uint8_t nodeId);
String getBMSErrorString(uint8_t nodeId);

// === MULTIPLEXER UTILITY FUNCTIONS (NOWE) ===
const char* getMux490TypeName(uint8_t type);
const char* getMux490TypeUnit(uint8_t type);
float convertMux490Value(uint8_t type, uint16_t rawValue);
bool isMux490TypeKnown(uint8_t type);

// === COMPATIBILITY ALIASES (z v3.0.0) ===
// Dla zachowania kompatybilności z oryginalnym kodem
#define getBatteryIndexFromNodeId(nodeId) getBMSIndexByNodeId(nodeId)

// === MODBUS UTILITY FUNCTIONS (z v3.0.0) ===
uint16_t floatToModbusRegister(float value, float scale = 100.0);
float modbusRegisterToFloat(uint16_t reg, float scale = 100.0);

#endif // BMS_DATA_H