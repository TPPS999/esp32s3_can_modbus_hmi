/*
 * bms_protocol.h - ESP32S3 CAN to Modbus TCP Bridge BMS Protocol Header
 * 
 * VERSION: v4.0.1 - COMPLETE DECLARATIONS
 * DATE: 2025-08-13
 * STATUS: ✅ READY - Wszystkie deklaracje funkcji z kompletnego bms_protocol.cpp
 * 
 * DESCRIPTION: Kompletny interfejs dla protokołu IFS BMS parsing
 * - Wszystkie 9 parserów ramek CAN 
 * - 54 typy multipleksera Frame 490
 * - Zaawansowane utility functions i diagnostyka
 * - Kompatybilność z oryginalnym kodem v3.0.0
 */

#ifndef BMS_PROTOCOL_H
#define BMS_PROTOCOL_H

#include "config.h"
#include "bms_data.h"

// === PROTOCOL CONSTANTS ===
#define MAX_CAN_FRAME_LENGTH 8
#define BMS_PROTOCOL_TIMEOUT_MS 30000

// === 🔥 MAIN FRAME PROCESSING ===
void parseCANFrame(unsigned long canId, unsigned char len, unsigned char* buf);
uint8_t extractNodeId(unsigned long canId, uint16_t baseId);

// === 🔥 FRAME PARSERS (wszystkie 9 typów z v3.0.0) ===
void parseBMSFrame190(uint8_t nodeId, unsigned char* data);  // Basic data (voltage, current, SOC)
void parseBMSFrame290(uint8_t nodeId, unsigned char* data);  // Cell voltages (min/mean)
void parseBMSFrame310(uint8_t nodeId, unsigned char* data);  // SOH/Temperature/DCiR
void parseBMSFrame390(uint8_t nodeId, unsigned char* data);  // Max voltages
void parseBMSFrame410(uint8_t nodeId, unsigned char* data);  // Temperatures/Ready states
void parseBMSFrame510(uint8_t nodeId, unsigned char* data);  // Power limits/I/O
void parseBMSFrame490(uint8_t nodeId, unsigned char* data);  // 🔥 Multiplexed data (54 typy!)
void parseBMSFrame1B0(uint8_t nodeId, unsigned char* data);  // Additional data
void parseBMSFrame710(uint8_t nodeId, unsigned char* data);  // CANopen status

// === 🔥 FRAME VALIDATION ===
bool isValidBMSFrame(unsigned long canId);
bool validateFrameData(unsigned long canId, unsigned char len, unsigned char* buf);

// === 🔥 FRAME TYPE DETECTION FUNCTIONS ===
bool isFrame190(unsigned long canId);  // Basic data
bool isFrame290(unsigned long canId);  // Cell voltages
bool isFrame310(unsigned long canId);  // SOH/Temperature
bool isFrame390(unsigned long canId);  // Max voltages
bool isFrame410(unsigned long canId);  // Temperatures
bool isFrame510(unsigned long canId);  // Power limits
bool isFrame490(unsigned long canId);  // Multiplexed
bool isFrame1B0(unsigned long canId);  // Additional
bool isFrame710(unsigned long canId);  // CANopen

// === 🔥 UTILITY FUNCTIONS ===
const char* getFrameTypeName(unsigned long canId);
BMSFrameType_t getFrameType(unsigned long canId);

// === 🔥 MULTIPLEXER UTILITY FUNCTIONS (NOWE z v3.0.0) ===
const char* getMux490TypeName(uint8_t type);
const char* getMux490TypeUnit(uint8_t type);
float convertMux490Value(uint8_t type, uint16_t rawValue);
bool isMux490TypeKnown(uint8_t type);

// === 🔥 CANOPEN UTILITY FUNCTIONS (NOWE) ===
const char* getCANopenStateName(uint8_t state);

// === 🔥 CANOPEN STATE DEFINITIONS ===
#define CANOPEN_STATE_BOOTUP            0x00
#define CANOPEN_STATE_STOPPED           0x04
#define CANOPEN_STATE_OPERATIONAL       0x05
#define CANOPEN_STATE_PRE_OPERATIONAL   0x7F

// === 🔥 DIAGNOSTICS & STATISTICS (rozszerzone z v3.0.0) ===
void enableProtocolLogging(bool enable);
void printBMSProtocolStatistics();
void printBMSFrameDetails(uint8_t nodeId);

// === 🔥 ADVANCED DIAGNOSTICS (NOWE) ===
void printFrame1B0Diagnostics(uint8_t nodeId);
void printFrame710Diagnostics(uint8_t nodeId);
void printAllFramesAnalysis(uint8_t nodeId);

// === 🔥 MULTIPLEXER TYPE DEFINITIONS (Frame 490 - kompletne 54 typy) ===
// Serial Number & Versions (0x00-0x05)
#define MUX490_SERIAL_NUMBER_0      0x00
#define MUX490_SERIAL_NUMBER_1      0x01
#define MUX490_HW_VERSION_0         0x02
#define MUX490_HW_VERSION_1         0x03
#define MUX490_SW_VERSION_0         0x04
#define MUX490_SW_VERSION_1         0x05

// Energy & Capacity (0x06-0x07, 0x0C)
#define MUX490_FACTORY_ENERGY       0x06
#define MUX490_DESIGN_CAPACITY      0x07
#define MUX490_SYSTEM_ENERGY        0x0C

// Temperatures (0x0D-0x0F)
#define MUX490_BALLANCER_TEMP       0x0D
#define MUX490_LTC_TEMP             0x0E
#define MUX490_INLET_OUTLET_TEMP    0x0F

// Humidity & Error Maps (0x10, 0x13-0x16)
#define MUX490_HUMIDITY             0x10
#define MUX490_ERROR_MAP_0          0x13
#define MUX490_ERROR_MAP_1          0x14
#define MUX490_ERROR_MAP_2          0x15
#define MUX490_ERROR_MAP_3          0x16

// Timing & Cycles (0x17-0x1A)
#define MUX490_TIME_TO_FULL_CHARGE  0x17
#define MUX490_TIME_TO_FULL_DISCHARGE 0x18
#define MUX490_POWER_ON_COUNTER     0x19
#define MUX490_BATTERY_CYCLES       0x1A

// CRC Values (0x1B-0x1E)
#define MUX490_DDCL_CRC             0x1B
#define MUX490_DCCL_CRC             0x1C
#define MUX490_DRCCL_CRC            0x1D
#define MUX490_OCV_CRC              0x1E

// Bootloader & Versions (0x1F-0x22)
#define MUX490_BL_VERSION_0         0x1F
#define MUX490_BL_VERSION_1         0x20
#define MUX490_OD_VERSION_0         0x21
#define MUX490_OD_VERSION_1         0x22

// IoT Status & Thresholds (0x23-0x2B)
#define MUX490_IOT_STATUS           0x23
#define MUX490_FULLY_CHARGED_ON     0x24
#define MUX490_FULLY_CHARGED_OFF    0x25
#define MUX490_FULLY_DISCHARGED_ON  0x26
#define MUX490_FULLY_DISCHARGED_OFF 0x27
#define MUX490_BATTERY_FULL_ON      0x28
#define MUX490_BATTERY_FULL_OFF     0x29
#define MUX490_BATTERY_EMPTY_ON     0x2A
#define MUX490_BATTERY_EMPTY_OFF    0x2B

// IMB Count & DBC Versions (0x2C-0x2F)
#define MUX490_DETECTED_IMBS        0x2C
#define MUX490_DBC_VERSION_0        0x2D
#define MUX490_DBC_VERSION_1        0x2E
#define MUX490_CONFIG_CRC           0x2F

// Energy Counters (0x30-0x35)
#define MUX490_CHARGE_ENERGY_0      0x30
#define MUX490_CHARGE_ENERGY_1      0x31
#define MUX490_DISCHARGE_ENERGY_0   0x32
#define MUX490_DISCHARGE_ENERGY_1   0x33
#define MUX490_RECUPERATIVE_ENERGY_0 0x34
#define MUX490_RECUPERATIVE_ENERGY_1 0x35

// === 🔥 PROTOCOL ERROR CODES (NOWE) ===
typedef enum {
  BMS_PROTOCOL_OK = 0,
  BMS_PROTOCOL_ERROR_INVALID_LENGTH,
  BMS_PROTOCOL_ERROR_INVALID_NODE_ID,
  BMS_PROTOCOL_ERROR_INVALID_FRAME_ID,
  BMS_PROTOCOL_ERROR_NULL_DATA,
  BMS_PROTOCOL_ERROR_PARSER_FAILED,
  BMS_PROTOCOL_ERROR_TIMEOUT,
  BMS_PROTOCOL_ERROR_UNKNOWN
} BMSProtocolError_t;

// === 🔥 PROTOCOL STATISTICS STRUCTURE (NOWE) ===
typedef struct {
  unsigned long totalFramesProcessed;
  unsigned long validFramesProcessed;
  unsigned long invalidFramesDropped;
  unsigned long parseErrors;
  unsigned long timeouts;
  
  // Per frame type counters
  unsigned long frame190Processed;
  unsigned long frame290Processed;
  unsigned long frame310Processed;
  unsigned long frame390Processed;
  unsigned long frame410Processed;
  unsigned long frame510Processed;
  unsigned long frame490Processed;  // Multiplexed
  unsigned long frame1B0Processed;  // Additional
  unsigned long frame710Processed;  // CANopen
  
  // Multiplexer statistics
  unsigned long multiplexerTypesReceived[54];  // Counter for each mux type
  unsigned long unknownMultiplexerTypes;
  
  // Timing
  unsigned long lastFrameProcessedTime;
  unsigned long averageProcessingTime;
  unsigned long maxProcessingTime;
  
} BMSProtocolStats_t;

// === 🔥 PROTOCOL STATISTICS FUNCTIONS (NOWE) ===
BMSProtocolStats_t* getBMSProtocolStats();
void resetBMSProtocolStats();
void updateProtocolStats(BMSFrameType_t frameType, unsigned long processingTime);
void printProtocolStatsSummary();
void printMultiplexerStats();

// === 🔥 PERFORMANCE MONITORING (NOWE) ===
void startFrameProcessingTimer();
unsigned long stopFrameProcessingTimer();
void logFrameProcessingTime(BMSFrameType_t frameType, unsigned long time);

// === 🔥 PROTOCOL CONFIGURATION (NOWE) ===
typedef struct {
  bool enableDebugLogging;
  bool enablePerformanceMonitoring;
  bool enableDetailedMultiplexerLogging;
  bool enableFrameValidation;
  bool enableTimeoutDetection;
  unsigned long frameTimeoutMs;
  unsigned long maxProcessingTimeMs;
} BMSProtocolConfig_t;

// Protocol configuration functions
void setBMSProtocolConfig(const BMSProtocolConfig_t* config);
BMSProtocolConfig_t* getBMSProtocolConfig();
void resetBMSProtocolConfigToDefaults();

// === 🔥 CALLBACK SYSTEM (NOWE) ===
// Callback function types
typedef void (*BMSFrameReceivedCallback)(uint8_t nodeId, BMSFrameType_t frameType, unsigned char* data);
typedef void (*BMSMultiplexerDataCallback)(uint8_t nodeId, uint8_t muxType, uint16_t muxValue);
typedef void (*BMSErrorCallback)(uint8_t nodeId, BMSProtocolError_t error, const char* message);

// Callback registration functions
void setBMSFrameReceivedCallback(BMSFrameReceivedCallback callback);
void setBMSMultiplexerDataCallback(BMSMultiplexerDataCallback callback);
void setBMSErrorCallback(BMSErrorCallback callback);

// === 🔥 FRAME FILTERING (NOWE) ===
typedef struct {
  bool enableNodeFiltering;
  uint8_t allowedNodeIds[MAX_BMS_NODES];
  int allowedNodeCount;
  
  bool enableFrameTypeFiltering;
  BMSFrameType_t allowedFrameTypes[BMS_FRAME_TYPE_COUNT];
  int allowedFrameTypeCount;
  
  bool enableMultiplexerFiltering;
  uint8_t allowedMultiplexerTypes[54];
  int allowedMultiplexerTypeCount;
} BMSFrameFilter_t;

// Frame filtering functions
void setBMSFrameFilter(const BMSFrameFilter_t* filter);
BMSFrameFilter_t* getBMSFrameFilter();
bool isFrameAllowed(unsigned long canId, uint8_t nodeId);
bool isMultiplexerTypeAllowed(uint8_t muxType);
void resetBMSFrameFilter();

// === 🔥 BACKWARD COMPATIBILITY (z v3.0.0) ===
// Aliasy dla zachowania kompatybilności z oryginalnym kodem
#define updateCommunicationStatus(nodeId) updateCommunicationStatus(nodeId)
#define updateFrameTimestamp(nodeId, frameType) updateFrameTimestamp(nodeId, frameType)

#endif // BMS_PROTOCOL_H