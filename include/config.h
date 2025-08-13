/*
 * config.h - ESP32S3 CAN to Modbus TCP Bridge Configuration
 * 
 * VERSION: v4.0.0 - MODULAR ARCHITECTURE
 * DATE: 2025-08-12
 * DESCRIPTION: Centralna konfiguracja systemu ESP32S3 CAN to Modbus TCP Bridge
 */

#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>
#include <mcp_can.h>

// === FIRMWARE INFORMATION ===
#define FIRMWARE_VERSION "v4.0.0"
#define BUILD_DATE __DATE__ " " __TIME__
#define DEVICE_NAME "ESP32S3-CAN-Modbus-Bridge"

// === DEBUG CONFIGURATION ===
#define ENABLE_DEBUG_SERIAL 1
#define ENABLE_CAN_FRAME_LOGGING 1
#define ENABLE_MODBUS_FRAME_LOGGING 1
#define ENABLE_WIFI_LOGGING 1

#if ENABLE_DEBUG_SERIAL
  #define DEBUG_PRINT(x) Serial.print(x)
  #define DEBUG_PRINTLN(x) Serial.println(x)
  #define DEBUG_PRINTF(fmt, ...) Serial.printf(fmt, ##__VA_ARGS__)
#else
  #define DEBUG_PRINT(x)
  #define DEBUG_PRINTLN(x)
  #define DEBUG_PRINTF(fmt, ...)
#endif

// === HARDWARE CONFIGURATION ===

// SPI Pins for MCP2515 CAN Controller
#define SPI_CS_PIN    44
#define SPI_MOSI_PIN  9
#define SPI_MISO_PIN  8
#define SPI_SCK_PIN   7
#define CAN_INT_PIN   2

// Other Hardware Pins
#define LED_PIN       21

// === SYSTEM LIMITS ===
#define MAX_BMS_NODES 16
#define MAX_WIFI_SSID_LENGTH 32
#define MAX_WIFI_PASSWORD_LENGTH 64
#define MAX_IP_ADDRESS_LENGTH 16
#define MAX_DEVICE_NAME_LENGTH 32

// === CAN CONFIGURATION ===
#define CAN_SPEED_125K CAN_125KBPS
#define CAN_SPEED_250K CAN_250KBPS
#define CAN_SPEED_500K CAN_500KBPS

// CAN Frame Types and Base IDs
#define BMS_FRAME_190_BASE 0x180  // Basic data (voltage, current, SOC)
#define BMS_FRAME_290_BASE 0x280  // Cell voltages (min, mean)
#define BMS_FRAME_310_BASE 0x300  // SOH, temperature, impedance
#define BMS_FRAME_390_BASE 0x380  // Max voltages
#define BMS_FRAME_410_BASE 0x400  // Temperatures, ready states
#define BMS_FRAME_510_BASE 0x500  // Power limits, I/O states
#define BMS_FRAME_490_BASE 0x480  // Multiplexed data
#define BMS_FRAME_1B0_BASE 0x1A0  // Additional data
#define BMS_FRAME_710_BASE 0x700  // CANopen status

// CAN Timing
#define CAN_RESET_DELAY_MS 100
#define CAN_INIT_TIMEOUT_MS 5000
#define CAN_FRAME_TIMEOUT_MS 1000

// === MODBUS TCP CONFIGURATION ===
#define MODBUS_TCP_PORT 502
#define MODBUS_SLAVE_ID 1
#define MODBUS_MAX_HOLDING_REGISTERS 2000
#define MODBUS_REGISTERS_PER_BMS 125
#define MODBUS_CLIENT_TIMEOUT_MS 30000

// === WIFI CONFIGURATION ===
#define DEFAULT_WIFI_SSID "WNK3"
#define DEFAULT_WIFI_PASSWORD "PiotrStrzyklaskiNieIstnieje"
#define DEFAULT_DEVICE_IP "DHCP"

// WiFi Timeouts
#define WIFI_CONNECTION_TIMEOUT_MS 30000
#define WIFI_RECONNECT_DELAY_MS 5000
#define WIFI_MAX_RECONNECT_ATTEMPTS 5

// === BMS DATA CONFIGURATION ===
#define BMS_DATA_TIMEOUT_MS 30000
#define BMS_CRITICAL_TIMEOUT_MS 10000
#define BMS_HEARTBEAT_INTERVAL_MS 5000

// BMS Value Limits
#define BMS_MIN_VOLTAGE 30.0
#define BMS_MAX_VOLTAGE 60.0
#define BMS_MIN_TEMPERATURE -40
#define BMS_MAX_TEMPERATURE 85
#define BMS_MIN_SOC 0.0
#define BMS_MAX_SOC 100.0

// === EEPROM CONFIGURATION ===
#define EEPROM_SIZE 512
#define EEPROM_MAGIC 0xA5
#define EEPROM_MAGIC_VALUE 0xA5

// EEPROM Memory Map
#define EEPROM_MAGIC_ADDR 0
#define EEPROM_WIFI_SSID 1
#define EEPROM_WIFI_PASS (EEPROM_WIFI_SSID + MAX_WIFI_SSID_LENGTH)
#define EEPROM_DEVICE_IP (EEPROM_WIFI_PASS + MAX_WIFI_PASSWORD_LENGTH)
#define EEPROM_ACTIVE_BMS (EEPROM_DEVICE_IP + MAX_IP_ADDRESS_LENGTH)
#define EEPROM_BMS_IDS (EEPROM_ACTIVE_BMS + 1)
#define EEPROM_CAN_SPEED (EEPROM_BMS_IDS + MAX_BMS_NODES)

// === SYSTEM STATES ===
typedef enum {
  SYSTEM_STATE_INIT = 0,
  SYSTEM_STATE_INITIALIZING,
  SYSTEM_STATE_RUNNING,
  SYSTEM_STATE_ERROR,
  SYSTEM_STATE_RECOVERY,
  SYSTEM_STATE_SHUTDOWN
} SystemState_t;

// === CAN STATES ===
typedef enum {
  CAN_STATE_UNINITIALIZED = 0,
  CAN_STATE_INITIALIZING,
  CAN_STATE_RUNNING,
  CAN_STATE_ERROR,
  CAN_STATE_RECOVERY
} CanState_t;

// === CAN ERRORS ===
typedef enum {
  CAN_ERROR_NONE = 0,
  CAN_ERROR_INIT_FAILED,
  CAN_ERROR_MODE_FAILED,
  CAN_ERROR_FILTER_FAILED,
  CAN_ERROR_COMMUNICATION_FAILED,
  CAN_ERROR_TIMEOUT,
  CAN_ERROR_HARDWARE_FAULT
} CanError_t;

// === MODBUS STATES ===
typedef enum {
  MODBUS_STATE_UNINITIALIZED = 0,
  MODBUS_STATE_INITIALIZING,
  MODBUS_STATE_RUNNING,
  MODBUS_STATE_ERROR,
  MODBUS_STATE_CLIENT_CONNECTED
} ModbusState_t;

// === BMS FRAME TYPES ===
typedef enum {
  BMS_FRAME_TYPE_190 = 0,  // Basic data
  BMS_FRAME_TYPE_290,      // Cell voltages
  BMS_FRAME_TYPE_310,      // SOH/Temperature
  BMS_FRAME_TYPE_390,      // Max limits
  BMS_FRAME_TYPE_410,      // Temperatures
  BMS_FRAME_TYPE_510,      // Power limits
  BMS_FRAME_TYPE_490,      // Multiplexed
  BMS_FRAME_TYPE_1B0,      // Additional
  BMS_FRAME_TYPE_710,      // CANopen
  BMS_FRAME_TYPE_COUNT
} BMSFrameType_t;

// === CONFIGURATION STRUCTURES ===

struct SystemConfig {
  // WiFi Configuration
  char wifiSSID[MAX_WIFI_SSID_LENGTH];
  char wifiPassword[MAX_WIFI_PASSWORD_LENGTH];
  char deviceIP[MAX_IP_ADDRESS_LENGTH];
  
  // BMS Configuration
  uint8_t activeBmsNodes;
  uint8_t bmsNodeIds[MAX_BMS_NODES];
  
  // CAN Configuration
  uint8_t canSpeed;
  
  // System Configuration
  bool configValid;
  char deviceName[MAX_DEVICE_NAME_LENGTH];
  
  // Feature Flags
  bool enableAutoReconnect;
  bool enableAPFallback;
  bool enableOTA;
  bool enableWebServer;
  bool enableSNMP;
  
  // Timeouts and Intervals
  unsigned long heartbeatInterval;
  unsigned long diagnosticsInterval;
  unsigned long communicationTimeout;
  unsigned long reconnectInterval;
};

// === GLOBAL VARIABLES ===
extern SystemConfig systemConfig;
extern SystemState_t systemState;

// === FUNCTION DECLARATIONS ===

// Configuration Management
bool loadConfiguration();
bool saveConfiguration();
void initializeDefaultConfiguration();
bool validateConfiguration();
void printConfiguration();
void resetConfigurationToDefaults();

// System State Management
void setSystemState(SystemState_t newState);
SystemState_t getSystemState();
String systemStateToString(SystemState_t state);

// Utility Functions
bool isValidIPAddress(const String& ip);
bool isValidSSID(const String& ssid);
bool isValidBMSNodeId(uint8_t nodeId);
void updateSystemUptime();
unsigned long getSystemUptime();

// Debug and Diagnostics
void printSystemInfo();
void printMemoryStatus();
void printBootProgress(const String& step, bool success);
String formatBytes(size_t bytes);
String formatUptime(unsigned long milliseconds);

// Emergency Functions
void systemRestart(unsigned long delayMs = 0);
void systemShutdown();
void enterSafeMode();

// === CONFIGURATION DEFAULTS ===
#define DEFAULT_HEARTBEAT_INTERVAL_MS 60000
#define DEFAULT_DIAGNOSTICS_INTERVAL_MS 300000
#define DEFAULT_COMMUNICATION_TIMEOUT_MS 30000
#define DEFAULT_RECONNECT_INTERVAL_MS 5000

// === VALIDATION MACROS ===
#define IS_VALID_BMS_COUNT(x) ((x) > 0 && (x) <= MAX_BMS_NODES)
#define IS_VALID_NODE_ID(x) ((x) > 0 && (x) <= 255)
#define IS_VALID_CAN_SPEED(x) ((x) == CAN_125KBPS || (x) == CAN_250KBPS || (x) == CAN_500KBPS)

// === FEATURE FLAGS ===
#define FEATURE_WIFI_MANAGER 1
#define FEATURE_CAN_HANDLER 1
#define FEATURE_MODBUS_TCP 1
#define FEATURE_BMS_DATA 1
#define FEATURE_WEB_SERVER 0
#define FEATURE_OTA_UPDATES 0
#define FEATURE_SNMP_AGENT 0
#define FEATURE_DATA_LOGGING 0

// === PERFORMANCE SETTINGS ===
#define MAIN_LOOP_DELAY_MS 1
#define STATUS_CHECK_INTERVAL_MS 10000
#define MEMORY_CHECK_INTERVAL_MS 60000
#define WATCHDOG_TIMEOUT_MS 30000

// === SECURITY SETTINGS ===
#define ENABLE_WPA3_SUPPORT 1
#define ENABLE_HTTPS_SUPPORT 0
#define ENABLE_AUTH_REQUIRED 0
#define DEFAULT_API_KEY ""

// === ADVANCED CAN SETTINGS ===
#define CAN_RX_BUFFER_SIZE 32
#define CAN_TX_BUFFER_SIZE 16
#define CAN_FILTER_COUNT 8
#define CAN_ERROR_THRESHOLD 100
#define CAN_RECOVERY_ATTEMPTS 3

// === ADVANCED MODBUS SETTINGS ===
#define MODBUS_MAX_CONCURRENT_CLIENTS 4
#define MODBUS_REQUEST_TIMEOUT_MS 5000
#define MODBUS_RESPONSE_BUFFER_SIZE 256
#define MODBUS_EXCEPTION_RETRY_COUNT 3

// === MEMORY MANAGEMENT ===
#define MIN_FREE_HEAP_BYTES 50000
#define HEAP_WARNING_THRESHOLD 75000
#define STACK_GUARD_SIZE 4096

// === LOGGING CONFIGURATION ===
typedef enum {
  LOG_LEVEL_NONE = 0,
  LOG_LEVEL_ERROR,
  LOG_LEVEL_WARNING,
  LOG_LEVEL_INFO,
  LOG_LEVEL_DEBUG,
  LOG_LEVEL_VERBOSE
} LogLevel_t;

#define DEFAULT_LOG_LEVEL LOG_LEVEL_INFO
#define MAX_LOG_MESSAGE_LENGTH 256

// === CONDITIONAL COMPILATION ===
#ifdef ESP32S3
  #define PLATFORM_ESP32S3 1
  #define CPU_FREQUENCY_MHZ 240
  #define FLASH_SIZE_MB 8
  #define PSRAM_SIZE_MB 8
#else
  #error "This code is designed for ESP32S3 only"
#endif

// === VERSION COMPATIBILITY ===
#define MIN_ARDUINO_ESP32_VERSION "2.0.0"
#define REQUIRED_RADIOLIB_VERSION "6.0.0"
#define REQUIRED_WIFI_LIB_VERSION "2.0.0"

// === COMPILE-TIME CHECKS ===
#if MAX_BMS_NODES > 16
  #error "MAX_BMS_NODES cannot exceed 16"
#endif

#if MODBUS_MAX_HOLDING_REGISTERS != (MAX_BMS_NODES * MODBUS_REGISTERS_PER_BMS)
  #error "Modbus register count mismatch"
#endif

#if EEPROM_SIZE < 512
  #error "EEPROM size too small for configuration"
#endif

// === INLINE UTILITY FUNCTIONS ===
inline bool isSystemRunning() {
  return systemState == SYSTEM_STATE_RUNNING;
}

inline bool isSystemError() {
  return systemState == SYSTEM_STATE_ERROR;
}

inline unsigned long millisSinceBoot() {
  return millis();
}

inline void systemDelay(unsigned long ms) {
  if (ms > 0) delay(ms);
}

// === CALLBACK TYPES ===
typedef void (*SystemStateCallback)(SystemState_t oldState, SystemState_t newState);
typedef void (*ErrorCallback)(const char* errorMessage);
typedef void (*HeartbeatCallback)();

// === CALLBACK REGISTRATION ===
void setSystemStateCallback(SystemStateCallback callback);
void setErrorCallback(ErrorCallback callback);
void setHeartbeatCallback(HeartbeatCallback callback);

#endif // CONFIG_H