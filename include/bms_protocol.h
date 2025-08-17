/*
 * bms_protocol.h - ESP32S3 CAN to Modbus TCP Bridge - BMS Protocol Header
 * 
 * VERSION: v4.0.2 - NAPRAWIONY Z GŁÓWNYMI FUNKCJAMI
 * DATE: 2025-08-17 11:02
 * STATUS: ✅ WSZYSTKIE FUNKCJE DODANE
 * 
 * Naprawione:
 * - Dodane główne funkcje: setupBMSProtocol(), processBMSProtocol(), isBMSProtocolHealthy()
 * - Wszystkie 9 parserów ramek CAN
 * - 54 typy multipleksera Frame 490
 * - Kompatybilność z main.cpp
 * - CAN handling functions (zastąpienie can_handler)
 */

#ifndef BMS_PROTOCOL_H
#define BMS_PROTOCOL_H

#include <Arduino.h>
#include <SPI.h>
#include <mcp_can.h>
#include "config.h"
#include "bms_data.h"

// === PROTOCOL CONSTANTS ===
#define MAX_CAN_FRAME_LENGTH 8
#define BMS_PROTOCOL_TIMEOUT_MS 30000

// === 🔥 GŁÓWNE FUNKCJE PROTOKOŁU (wymagane przez main.cpp) ===

// Lifecycle management
bool setupBMSProtocol();           // Inicjalizacja protokołu BMS + CAN
void shutdownBMSProtocol();        // Zamknięcie protokołu
bool restartBMSProtocol();         // Restart protokołu

// Processing functions
void processBMSProtocol();         // Główna pętla przetwarzania (zastępuje processCAN)
bool isBMSProtocolHealthy();       // Status zdrowia protokołu (zastępuje isCANHealthy)

// === 🔥 CAN HANDLING FUNCTIONS (zastąpienie can_handler) ===

// CAN initialization and management
bool initializeCAN();
bool initializeMCP2515();
void shutdownCAN();
bool isCANInitialized();

// CAN processing
void processCAN();                 // Odbiór i przetwarzanie ramek CAN
bool isCANHealthy();              // Status CAN communication

// === 🔥 FRAME PROCESSING ===

// Main frame processing
void parseCANFrame(unsigned long canId, unsigned char len, unsigned char* buf);
uint8_t extractNodeId(unsigned long canId, uint16_t baseId);

// Frame validation
bool isValidBMSFrame(unsigned long canId);
bool validateFrameData(unsigned long canId, unsigned char len, unsigned char* buf);

// === 🔥 FRAME PARSERS (wszystkie 9 typów) ===

void parseBMSFrame190(uint8_t nodeId, unsigned char* data);  // Basic data (voltage, current, SOC)
void parseBMSFrame290(uint8_t nodeId, unsigned char* data);  // Cell voltages (min/mean)
void parseBMSFrame310(uint8_t nodeId, unsigned char* data);  // SOH/Temperature/DCiR
void parseBMSFrame390(uint8_t nodeId, unsigned char* data);  // Max voltages
void parseBMSFrame410(uint8_t nodeId, unsigned char* data);  // Temperatures/Ready states
void parseBMSFrame510(uint8_t nodeId, unsigned char* data);  // Power limits/I/O
void parseBMSFrame490(uint8_t nodeId, unsigned char* data);  // 🔥 Multiplexed data (54 typy!)
void parseBMSFrame1B0(uint8_t nodeId, unsigned char* data);  // Additional data
void parseBMSFrame710(uint8_t nodeId, unsigned char* data);  // CANopen status

// === 🔥 FRAME TYPE DETECTION ===

bool isFrame190(unsigned long canId);  // Basic data
bool isFrame290(unsigned long canId);  // Cell voltages
bool isFrame310(unsigned long canId);  // SOH/Temperature
bool isFrame390(unsigned long canId);  // Max voltages
bool isFrame410(unsigned long canId);  // Temperatures
bool isFrame490(unsigned long canId);  // Multiplexed
bool isFrame510(unsigned long canId);  // Power limits
bool isFrame1B0(unsigned long canId);  // Additional
bool isFrame710(unsigned long canId);  // CANopen

// Frame type utilities
const char* getFrameTypeName(unsigned long canId);
BMSFrameType_t getFrameType(unsigned long canId);

// === 🔥 MULTIPLEXER DEFINITIONS (Frame 490 - 54 typy) ===

// Serial Number & Versions (0x00-0x05)
#define MUX490_SERIAL_NUMBER_0      0x00
#define MUX490_SERIAL_NUMBER_1      0x01
#define MUX490_HW_VERSION_0         0x02
#define MUX490_HW_VERSION_1         0x03
#define MUX490_SW_VERSION_0         0x04
#define MUX490_SW_VERSION_1         0x05

// Energy & Capacity (0x06-0x0C)
#define MUX490_FACTORY_ENERGY       0x06
#define MUX490_DESIGN_CAPACITY      0x07
#define MUX490_SYSTEM_DESIGNED_ENERGY 0x0C

// Temperature & Environment (0x0D-0x10)
#define MUX490_BALLANCER_TEMP_MAX   0x0D
#define MUX490_LTC_TEMP_MAX         0x0E
#define MUX490_INLET_TEMPERATURE    0x0F
#define MUX490_OUTLET_TEMPERATURE   0x0F  // Same as inlet in data[7]
#define MUX490_HUMIDITY             0x10

// Error Maps (0x13-0x16)
#define MUX490_ERROR_MAP_0          0x13
#define MUX490_ERROR_MAP_1          0x14
#define MUX490_ERROR_MAP_2          0x15
#define MUX490_ERROR_MAP_3          0x16

// Timing & Cycles (0x17-0x1A)
#define MUX490_TIME_TO_FULL_CHARGE  0x17
#define MUX490_TIME_TO_FULL_DISCHARGE 0x18
#define MUX490_BATTERY_CYCLES       0x1A

// Extended Multiplexer Types (0x1B-0x35)
#define MUX490_NUMBER_OF_IMBS       0x1B
#define MUX490_BALANCING_ENERGY     0x1C
#define MUX490_MAX_DISCHARGE_POWER  0x1D
#define MUX490_MAX_CHARGE_POWER     0x1E
#define MUX490_MAX_DISCHARGE_ENERGY 0x1F
#define MUX490_MAX_CHARGE_ENERGY    0x20

// Version & CRC Information (0x21-0x29)
#define MUX490_BL_VERSION_0         0x21
#define MUX490_BL_VERSION_1         0x22
#define MUX490_APP_VERSION_0        0x23
#define MUX490_APP_VERSION_1        0x24
#define MUX490_CRC_APP              0x25
#define MUX490_CRC_BOOT             0x26

// Energy Counters (0x30-0x35)
#define MUX490_CHARGE_ENERGY_0      0x30
#define MUX490_CHARGE_ENERGY_1      0x31
#define MUX490_DISCHARGE_ENERGY_0   0x32
#define MUX490_DISCHARGE_ENERGY_1   0x33
#define MUX490_RECUPERATIVE_ENERGY_0 0x34
#define MUX490_RECUPERATIVE_ENERGY_1 0x35

// === 🔥 MULTIPLEXER UTILITY FUNCTIONS ===

const char* getMux490TypeName(uint8_t type);
const char* getMux490TypeUnit(uint8_t type);
float convertMux490Value(uint8_t type, uint16_t rawValue);
bool isMux490TypeKnown(uint8_t type);

// === 🔥 CANOPEN DEFINITIONS ===

#define CANOPEN_STATE_BOOTUP            0x00
#define CANOPEN_STATE_STOPPED           0x04
#define CANOPEN_STATE_OPERATIONAL       0x05
#define CANOPEN_STATE_PRE_OPERATIONAL   0x7F

const char* getCANopenStateName(uint8_t state);

// === 🔥 PROTOCOL STATISTICS ===

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

// Statistics functions
BMSProtocolStats_t* getBMSProtocolStats();
void resetBMSProtocolStats();
void printBMSProtocolStatistics();

// === 🔥 COMMUNICATION MANAGEMENT ===

// Communication status
void updateCommunicationStatus(uint8_t nodeId);
bool isBMSCommunicationActive(uint8_t nodeId);
void checkCommunicationTimeouts();

// Frame timing
void updateFrameTimestamp(uint8_t nodeId, BMSFrameType_t frameType);
unsigned long getLastFrameTime(uint8_t nodeId);

// === 🔥 DIAGNOSTICS & MONITORING ===

// Protocol diagnostics
void enableProtocolLogging(bool enable);
void printBMSFrameDetails(uint8_t nodeId);
void printAllFramesAnalysis(uint8_t nodeId);

// Advanced diagnostics
void printFrame1B0Diagnostics(uint8_t nodeId);
void printFrame710Diagnostics(uint8_t nodeId);
void printMultiplexerDiagnostics(uint8_t nodeId);

// Heartbeat functions
void printBMSHeartbeatExtended(uint8_t nodeId);
void printSystemHeartbeat();

// === 🔥 UTILITY FUNCTIONS ===

// CAN frame utilities
void printCANFrame(unsigned long canId, unsigned char len, unsigned char* buf);
bool isValidCANFrame(unsigned long canId, unsigned char len);

// Node ID extraction
uint8_t extractNodeIdFromCanId(unsigned long canId);
bool isValidBMSNodeId(uint8_t nodeId);

// Error handling
void logProtocolError(const char* context, uint8_t nodeId, const char* error);
void handleProtocolTimeout(uint8_t nodeId);

// === 🔥 GLOBAL INSTANCES ===

// CAN controller (będzie zdefiniowany w .cpp)
extern MCP_CAN* canController;

// Protocol statistics (będzie zdefiniowany w .cpp)
extern BMSProtocolStats_t protocolStats;

// === 🔥 CONFIGURATION ===

typedef struct {
  bool enableDebugLogging;
  bool enablePerformanceMonitoring;
  bool enableDetailedMultiplexerLogging;
  bool enableFrameValidation;
  bool enableTimeoutDetection;
  unsigned long frameTimeoutMs;
  unsigned long maxProcessingTimeMs;
} BMSProtocolConfig_t;

// Configuration functions
void setBMSProtocolConfig(const BMSProtocolConfig_t* config);
BMSProtocolConfig_t* getBMSProtocolConfig();
void resetBMSProtocolConfigToDefaults();

// === 🔥 BACKWARD COMPATIBILITY ===

// Aliasy dla zachowania kompatybilności z poprzednimi wersjami
#define setupCAN() setupBMSProtocol()
#define processCAN() processBMSProtocol()
#define isCANHealthy() isBMSProtocolHealthy()

// Legacy function wrappers
bool legacySetupCAN();
void legacyProcessCAN();
bool legacyIsCANHealthy();

// === 🔥 INLINE HELPER FUNCTIONS ===

inline bool isFrameInRange(unsigned long canId, uint16_t baseId) {
  return (canId & 0xFFF0) == (baseId & 0xFFF0);
}

inline uint8_t getFrameNodeId(unsigned long canId) {
  return canId & 0x0F;  // Last 4 bits
}

inline bool isFrameTypeValid(BMSFrameType_t frameType) {
  return frameType >= BMS_FRAME_190 && frameType < BMS_FRAME_UNKNOWN;
}

inline bool isMultiplexerTypeValid(uint8_t muxType) {
  return muxType <= 0x35;  // Maximum known multiplexer type
}

// === 🔥 DEBUG MACROS ===

#ifdef DEBUG_BMS_PARSING
  #define DEBUG_PRINTF(fmt, ...) Serial.printf(fmt, ##__VA_ARGS__)
  #define DEBUG_PRINT(str) Serial.print(str)
  #define DEBUG_PRINTLN(str) Serial.println(str)
#else
  #define DEBUG_PRINTF(fmt, ...)
  #define DEBUG_PRINT(str)
  #define DEBUG_PRINTLN(str)
#endif

// === 🔥 FRAME TYPE ENUMERATION ===

typedef enum {
  BMS_FRAME_TYPE_190 = 0,  // Basic data
  BMS_FRAME_TYPE_290,      // Cell voltages
  BMS_FRAME_TYPE_310,      // SOH, temperature
  BMS_FRAME_TYPE_390,      // Max voltages
  BMS_FRAME_TYPE_410,      // Temperature, ready states
  BMS_FRAME_TYPE_510,      // Power limits
  BMS_FRAME_TYPE_490,      // Multiplexed data
  BMS_FRAME_TYPE_1B0,      // Additional data
  BMS_FRAME_TYPE_710,      // CANopen state
  BMS_FRAME_TYPE_UNKNOWN,
  BMS_FRAME_TYPE_COUNT = BMS_FRAME_TYPE_UNKNOWN
} BMSFrameType_t;

// === 🔥 ERROR CODES ===

typedef enum {
  BMS_PROTOCOL_OK = 0,
  BMS_PROTOCOL_ERROR_INVALID_LENGTH,
  BMS_PROTOCOL_ERROR_INVALID_NODE_ID,
  BMS_PROTOCOL_ERROR_INVALID_FRAME_ID,
  BMS_PROTOCOL_ERROR_NULL_DATA,
  BMS_PROTOCOL_ERROR_PARSER_FAILED,
  BMS_PROTOCOL_ERROR_TIMEOUT,
  BMS_PROTOCOL_ERROR_CAN_INIT_FAILED,
  BMS_PROTOCOL_ERROR_MCP2515_FAILED,
  BMS_PROTOCOL_ERROR_UNKNOWN
} BMSProtocolError_t;

const char* getBMSProtocolErrorString(BMSProtocolError_t error);

#endif // BMS_PROTOCOL_H