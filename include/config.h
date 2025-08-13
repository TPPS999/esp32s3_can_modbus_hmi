/*
 * config.h - ESP32S3 CAN to Modbus TCP Bridge Configuration
 * 
 * VERSION: v4.0.1 - POPRAWIONY
 * DATE: 2025-08-13 09:10
 * STATUS: ✅ WSZYSTKIE BŁĘDY NAPRAWIONE
 * 
 * Naprawione:
 * - ESP32S3 target validation
 * - Wszystkie brakujące definicje CAN_FRAME_*_BASE
 * - Wszystkie brakujące definicje CAN_FREQ_*
 * - MODBUS_MAX_HOLDING_REGISTERS
 * - MODBUS_SLAVE_ID
 * - Wszystkie inne brakujące makra
 */

#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>

// === TARGET VALIDATION ===
#if !defined(CONFIG_IDF_TARGET_ESP32S3)
    #warning "This code is designed for ESP32S3 but will attempt to compile anyway"
#endif

// === BUILD INFORMATION ===
#define FIRMWARE_VERSION "4.0.1"
#define BUILD_DATE __DATE__ " " __TIME__
#define DEVICE_NAME "ESP32S3-CAN-MODBUS-TCP"

// === HARDWARE CONFIGURATION ===
#define SPI_CS_PIN    44
#define SPI_MOSI_PIN  9
#define SPI_MISO_PIN  8  
#define SPI_SCK_PIN   7
#define CAN_INT_PIN   2
#define LED_PIN       21

// === CAN CONFIGURATION ===
#define CAN_SPEED CAN_125KBPS
#define CAN_FRAME_LENGTH 8

// === CAN FRAME BASE ADDRESSES ===
#define CAN_FRAME_190_BASE  0x181
#define CAN_FRAME_290_BASE  0x281
#define CAN_FRAME_310_BASE  0x301
#define CAN_FRAME_390_BASE  0x381
#define CAN_FRAME_410_BASE  0x401
#define CAN_FRAME_510_BASE  0x501
#define CAN_FRAME_490_BASE  0x481
#define CAN_FRAME_1B0_BASE  0x1A1
#define CAN_FRAME_710_BASE  0x701

// === CAN FREQUENCY DEFINITIONS ===
#define CAN_FREQ_HIGH    100   // ms - ramki 190 (podstawowe dane)
#define CAN_FREQ_MEDIUM  500   // ms - ramki 290,310,390,410,510
#define CAN_FREQ_LOW     2000  // ms - ramki 490,1B0,710

// === BMS CONFIGURATION ===
#define MAX_BMS_NODES 16
#define BMS_MAX_SOH 100.0f
#define BMS_COMMUNICATION_TIMEOUT_MS 30000

// === VALIDATION MACROS ===
#define IS_VALID_BMS_NODE_ID(id) ((id) >= 1 && (id) <= 16)
#define IS_VALID_BMS_CAN_ID(canId) (((canId) >= 0x181 && (canId) <= 0x190) || \
                                   ((canId) >= 0x281 && (canId) <= 0x290) || \
                                   ((canId) >= 0x301 && (canId) <= 0x310) || \
                                   ((canId) >= 0x381 && (canId) <= 0x390) || \
                                   ((canId) >= 0x401 && (canId) <= 0x410) || \
                                   ((canId) >= 0x501 && (canId) <= 0x510) || \
                                   ((canId) >= 0x481 && (canId) <= 0x490) || \
                                   ((canId) >= 0x1A1 && (canId) <= 0x1B0) || \
                                   ((canId) >= 0x701 && (canId) <= 0x710))

// === NETWORK CONFIGURATION ===
const char* const WIFI_SSID = "WNK3";
const char* const WIFI_PASSWORD = "PiotrStrzyklaskiNieIstnieje";
#define WIFI_CONNECT_TIMEOUT_MS 30000
#define WIFI_RECONNECT_INTERVAL_MS 60000

// === MODBUS TCP CONFIGURATION ===
#define MODBUS_TCP_PORT 502
#define MODBUS_SLAVE_ID 1
#define MODBUS_MAX_HOLDING_REGISTERS 2000  // 16 baterii * 125 rejestrów

// Modbus function codes
#define MODBUS_FUNC_READ_HOLDING_REGISTERS 0x03
#define MODBUS_FUNC_WRITE_SINGLE_REGISTER 0x06
#define MODBUS_FUNC_WRITE_MULTIPLE_REGISTERS 0x10

// Modbus error codes
#define MODBUS_EXCEPTION_ILLEGAL_FUNCTION 0x01
#define MODBUS_EXCEPTION_ILLEGAL_DATA_ADDRESS 0x02
#define MODBUS_EXCEPTION_ILLEGAL_DATA_VALUE 0x03
#define MODBUS_EXCEPTION_SLAVE_DEVICE_FAILURE 0x04

// === MULTIPLEXER DEFINITIONS (Frame 490) ===
#define MUX490_SERIAL_NUMBER_0      0x00
#define MUX490_SERIAL_NUMBER_1      0x01
#define MUX490_HW_VERSION_0         0x02
#define MUX490_HW_VERSION_1         0x03
#define MUX490_SW_VERSION_0         0x04
#define MUX490_SW_VERSION_1         0x05
#define MUX490_FACTORY_ENERGY       0x06
#define MUX490_DESIGN_CAPACITY      0x07
#define MUX490_INLET_TEMPERATURE    0x0F
#define MUX490_OUTLET_TEMPERATURE   0x10
#define MUX490_HUMIDITY             0x10
#define MUX490_ERROR_MAP_0          0x13
#define MUX490_ERROR_MAP_1          0x14
#define MUX490_ERROR_MAP_2          0x15
#define MUX490_ERROR_MAP_3          0x16
#define MUX490_TIME_TO_FULL_CHARGE  0x17
#define MUX490_TIME_TO_FULL_DISCHARGE 0x18
#define MUX490_BATTERY_CYCLES       0x1A

// === FEATURES CONFIGURATION ===
#define FEATURE_CAN_FILTERING 1
#define FEATURE_MODBUS_WRITE 1
#define FEATURE_WIFI_AP_FALLBACK 1
#define FEATURE_EEPROM_CONFIG 1
#define FEATURE_WATCHDOG 1

// === DEBUG CONFIGURATION ===
#define DEBUG_CAN_FRAMES 1
#define DEBUG_MODBUS_REQUESTS 1
#define DEBUG_BMS_PARSING 1
#define DEBUG_WIFI_EVENTS 1

// === TIMING CONFIGURATION ===
#define HEARTBEAT_INTERVAL_MS 60000
#define DIAGNOSTICS_INTERVAL_MS 300000
#define STATUS_CHECK_INTERVAL_MS 10000
#define COMMUNICATION_TIMEOUT_MS 30000

// === SYSTEM STATE ENUMERATION ===
typedef enum {
  SYSTEM_STATE_INIT = 0,
  SYSTEM_STATE_INITIALIZING,
  SYSTEM_STATE_RUNNING,
  SYSTEM_STATE_ERROR,
  SYSTEM_STATE_RECOVERY,
  SYSTEM_STATE_SHUTDOWN
} SystemState_t;

// === WIFI STATE ENUMERATION ===
typedef enum {
  WIFI_STATE_DISCONNECTED = 0,
  WIFI_STATE_CONNECTING,
  WIFI_STATE_CONNECTED,
  WIFI_STATE_AP_MODE,
  WIFI_STATE_ERROR
} WiFiState_t;

// === MODBUS STATE ENUMERATION (bez konfliktu) ===
typedef enum {
  MODBUS_STATE_UNINITIALIZED = 0,
  MODBUS_STATE_INITIALIZING,
  MODBUS_STATE_RUNNING,
  MODBUS_STATE_ERROR,
  MODBUS_STATE_CLIENT_CONNECTED
} ModbusState_t;

// === BMS FRAME TYPE ENUMERATION ===
typedef enum {
  BMS_FRAME_190 = 0,  // Basic data
  BMS_FRAME_290,      // Cell voltages
  BMS_FRAME_310,      // SOH, temperature
  BMS_FRAME_390,      // Max voltages
  BMS_FRAME_410,      // Temperature, ready states
  BMS_FRAME_510,      // Power limits
  BMS_FRAME_490,      // Multiplexed data
  BMS_FRAME_1B0,      // Additional data
  BMS_FRAME_710,      // CANopen state
  BMS_FRAME_UNKNOWN
} BMSFrameType_t;

// === MULTIPLEXER TYPE ENUMERATION ===
typedef enum {
  MUX_TYPE_SERIAL_NUMBER_0 = 0x00,
  MUX_TYPE_SERIAL_NUMBER_1 = 0x01,
  MUX_TYPE_HW_VERSION_0 = 0x02,
  MUX_TYPE_HW_VERSION_1 = 0x03,
  MUX_TYPE_SW_VERSION_0 = 0x04,
  MUX_TYPE_SW_VERSION_1 = 0x05,
  MUX_TYPE_FACTORY_ENERGY = 0x06,
  MUX_TYPE_DESIGN_CAPACITY = 0x07,
  MUX_TYPE_INLET_TEMPERATURE = 0x0F,
  MUX_TYPE_OUTLET_TEMPERATURE = 0x10,
  MUX_TYPE_HUMIDITY = 0x10,
  MUX_TYPE_ERROR_MAP_0 = 0x13,
  MUX_TYPE_ERROR_MAP_1 = 0x14,
  MUX_TYPE_ERROR_MAP_2 = 0x15,
  MUX_TYPE_ERROR_MAP_3 = 0x16,
  MUX_TYPE_TIME_TO_FULL_CHARGE = 0x17,
  MUX_TYPE_TIME_TO_FULL_DISCHARGE = 0x18,
  MUX_TYPE_BATTERY_CYCLES = 0x1A,
  MUX_TYPE_UNKNOWN = 0xFF
} MultiplexerType;

// === MULTIPLEXER INFO STRUCTURE ===
struct MultiplexerInfo {
  MultiplexerType type;
  const char* name;
  const char* unit;
  float scale;
  bool isSigned;
};

// === SYSTEM CONFIGURATION STRUCTURE ===
struct SystemConfig {
  bool configValid;
  uint8_t bmsNodeIds[MAX_BMS_NODES];
  int activeBmsNodes;
  char wifiSSID[64];
  char wifiPassword[64];
  uint16_t modbusPort;
  uint8_t modbusSlaveId;
  bool enableCanFiltering;
  bool enableModbusWrite;
  bool enableWifiAP;
  uint32_t heartbeatInterval;
  uint32_t communicationTimeout;
};

// === FUNCTION DECLARATIONS ===

// Configuration functions
bool loadConfiguration();
bool saveConfiguration();
void setDefaultConfiguration();
const SystemConfig& getSystemConfig();

// BMS frame type detection
BMSFrameType_t getFrameType(unsigned long canId);
const char* frameTypeToString(BMSFrameType_t frameType);

// Multiplexer functions
MultiplexerType getMultiplexerType(uint8_t muxValue);
const MultiplexerInfo* getMultiplexerTypeInfo(MultiplexerType type);
float convertMultiplexerValue(uint16_t rawValue, MultiplexerType type);

// Utility functions
uint8_t extractNodeIdFromCanId(unsigned long canId);
uint16_t calculateModbusCRC(uint8_t* data, int length);
void printBootProgress(const char* module, bool success);

// LED functions
void setupLED();
void blinkLED(int times, int delayMs);
void setLED(bool state);

// Array size macro
#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))

// === TRIGGER CAN ID FOR AP MODE ===
#define AP_TRIGGER_CAN_ID 0x1FF

// === GLOBAL CONFIGURATION INSTANCE ===
extern SystemConfig systemConfig;

#endif // CONFIG_H