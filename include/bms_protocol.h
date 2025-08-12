/*
 * bms_protocol.h - ESP32S3 CAN to Modbus TCP Bridge BMS Protocol Parser
 * 
 * VERSION: v3.1.0
 * DATE: 2025-08-12
 * DESCRIPTION: Parsery protokołu IFS BMS - ramki CAN
 */

#ifndef BMS_PROTOCOL_H
#define BMS_PROTOCOL_H

#include "config.h"
#include "bms_data.h"

// === PROTOCOL CONSTANTS ===
#define IFS_BMS_FRAME_LENGTH 8
#define IFS_BMS_VOLTAGE_RESOLUTION 0.0625    // V
#define IFS_BMS_CURRENT_RESOLUTION 0.0625    // A
#define IFS_BMS_ENERGY_RESOLUTION 0.1        // Wh
#define IFS_BMS_CELL_VOLTAGE_RESOLUTION 0.0001 // V
#define IFS_BMS_SOH_RESOLUTION 0.1           // %
#define IFS_BMS_IMPEDANCE_RESOLUTION 0.1     // mΩ
#define IFS_BMS_TEMPERATURE_OFFSET 273       // Kelvin offset

// === FUNCTION DECLARATIONS ===

// === Main Protocol Processing ===
void parseCANFrame(unsigned long canId, unsigned char len, unsigned char* buf);
bool validateFrameData(unsigned long canId, unsigned char len, unsigned char* buf);

// === Frame Type Parsers ===
void parseBMSFrame190(uint8_t nodeId, unsigned char* data);  // Basic battery data
void parseBMSFrame290(uint8_t nodeId, unsigned char* data);  // Cell voltages
void parseBMSFrame310(uint8_t nodeId, unsigned char* data);  // SOH and temperature
void parseBMSFrame390(uint8_t nodeId, unsigned char* data);  // Maximum allowed values
void parseBMSFrame410(uint8_t nodeId, unsigned char* data);  // Temperature sensors
void parseBMSFrame510(uint8_t nodeId, unsigned char* data);  // Power limits
void parseBMSFrame490(uint8_t nodeId, unsigned char* data);  // Multiplexed data
void parseBMSFrame1B0(uint8_t nodeId, unsigned char* data);  // Additional data
void parseBMSFrame710(uint8_t nodeId, unsigned char* data);  // CANopen status

// === Data Conversion Utilities ===
float convertVoltage(uint16_t rawValue, float resolution = IFS_BMS_VOLTAGE_RESOLUTION);
float convertCurrent(uint16_t rawValue, float resolution = IFS_BMS_CURRENT_RESOLUTION);
float convertEnergy(uint16_t rawValue, float resolution = IFS_BMS_ENERGY_RESOLUTION);
float convertCellVoltage(uint16_t rawValue);
float convertSOH(uint16_t rawValue);
int16_t convertTemperature(uint16_t rawValue);
float convertImpedance(uint16_t rawValue);

// === Error Flag Parsing ===
void parseErrorFlags(uint8_t errorByte, BMSData& bms);
uint8_t createErrorByte(const BMSData& bms);

// === Multiplexed Data Handling ===
void processMultiplexedData(uint8_t nodeId, uint8_t muxType, uint16_t muxValue);
void updateSerialNumber(BMSData& bms, uint8_t part, uint16_t value);
void updateHardwareVersion(BMSData& bms, uint8_t part, uint16_t value);
void updateSoftwareVersion(BMSData& bms, uint8_t part, uint16_t value);

// === Frame Validation ===
bool isValidNodeIdForFrame(uint8_t nodeId, unsigned long canId);
bool isValidDataLength(unsigned char len);
bool isValidDataRange(const unsigned char* data, unsigned char len);

// === Protocol Statistics ===
void updateProtocolStatistics(uint8_t nodeId, BMSFrameType_t frameType, bool success);
void printProtocolStatistics();
void resetProtocolStatistics();

// === Diagnostic Functions ===
void dumpFrameData(unsigned long canId, unsigned char len, unsigned char* buf);
void logFrameParsing(uint8_t nodeId, BMSFrameType_t frameType, bool success, const char* details = "");
String getFrameDescription(unsigned long canId);

// === Protocol State ===
struct ProtocolStats {
  unsigned long totalFramesParsed;
  unsigned long successfulParses;
  unsigned long parseErrors;
  unsigned long invalidFrames;
  unsigned long frameTypeCounts[BMS_FRAME_TYPE_COUNT];
  unsigned long lastParseTime;
  unsigned long lastErrorTime;
  char lastError[64];
};

extern ProtocolStats protocolStats;

// === Advanced Parsing Functions ===
bool parseAndValidateBasicData(uint8_t nodeId, unsigned char* data);
bool parseAndValidateCellData(uint8_t nodeId, unsigned char* data);
bool parseAndValidateTemperatureData(uint8_t nodeId, unsigned char* data);
bool parseAndValidatePowerData(uint8_t nodeId, unsigned char* data);

// === Data Integrity ===
bool validateVoltageRange(float voltage);
bool validateCurrentRange(float current);
bool validateTemperatureRange(int16_t temperature);
bool validateSOCRange(float soc);
bool validateSOHRange(float soh);

// === Conversion Helpers ===
uint16_t combineBytes(uint8_t lowByte, uint8_t highByte);
uint32_t combineWords(uint16_t lowWord, uint16_t highWord);
void splitWord(uint16_t word, uint8_t& lowByte, uint8_t& highByte);

// === Error Recovery ===
void handleParseError(uint8_t nodeId, BMSFrameType_t frameType, const char* error);
void resetBMSDataOnError(uint8_t nodeId);
bool attemptDataRecovery(uint8_t nodeId, BMSFrameType_t frameType);

// === Protocol Configuration ===
void configureProtocolLimits(float maxVoltage, float maxCurrent, int16_t maxTemp);
void enableProtocolLogging(bool enable);
void setProtocolTimeout(unsigned long timeoutMs);

#endif // BMS_PROTOCOL_H