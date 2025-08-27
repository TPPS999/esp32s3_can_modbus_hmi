// =====================================================================
// === [MODULE_NAME].h - ESP32S3 CAN to Modbus TCP Bridge ===
// =====================================================================
// 
// 📋 PROJECT INFO:
//    Repository: https://github.com/user/esp32s3-can-modbus-tcp
//    Project: ESP32S3 CAN to Modbus TCP Bridge
//    Branch: main
//    Created: [DATE] (Warsaw Time)
//
// 📋 MODULE INFO:
//    Module: [MODULE_DESCRIPTION]
//    Version: v1.0.0
//    Created: [DATE] (Warsaw Time)
//    Last Modified: [DATE] (Warsaw Time)
//    Author: ESP32 Development Team
//
// 📊 VERSION HISTORY:
//    v1.0.0 - [DATE] - Initial implementation using Universal Workflow template
//
// 🎯 DEPENDENCIES:
//    Internal: config.h, [OTHER_INTERNAL_MODULES]
//    External: Arduino.h, [EXTERNAL_LIBRARIES]
//
// 📝 DESCRIPTION:
//    [DETAILED_MODULE_DESCRIPTION]
//    This module provides [FUNCTIONALITY] for the ESP32S3 CAN-Modbus TCP Bridge.
//    Handles [SPECIFIC_RESPONSIBILITIES] with [INTEGRATION_POINTS].
//
// 🔧 CONFIGURATION:
//    - [CONFIG_PARAMETER_1]: [DESCRIPTION]
//    - [CONFIG_PARAMETER_2]: [DESCRIPTION]
//    - [CONFIG_PARAMETER_3]: [DESCRIPTION]
//
// ⚠️  KNOWN ISSUES:
//    - None currently identified
//
// 🧪 TESTING STATUS:
//    Unit Tests: NOT_TESTED
//    Integration Tests: NOT_TESTED
//    Manual Testing: NOT_TESTED
//
// 📈 PERFORMANCE NOTES:
//    - Memory footprint: [MEMORY_USAGE]
//    - Execution time: [TIMING_INFO]
//    - Hardware requirements: [HW_REQUIREMENTS]
//
// =====================================================================

#ifndef [MODULE_NAME_UPPER]_H
#define [MODULE_NAME_UPPER]_H

// === INCLUDES ===
#include <Arduino.h>
#include "config.h"
// Add other required includes here

// === TARGET VALIDATION ===
#if !defined(CONFIG_IDF_TARGET_ESP32S3)
    #warning "This module is optimized for ESP32S3 but will attempt to compile anyway"
#endif

// === MODULE CONSTANTS ===
#define [MODULE_NAME_UPPER]_VERSION "1.0.0"
#define [MODULE_NAME_UPPER]_DEFAULT_TIMEOUT_MS 5000
#define [MODULE_NAME_UPPER]_MAX_RETRIES 3

// === MODULE CONFIGURATION ===
// Define configuration parameters specific to this module
#define [MODULE_NAME_UPPER]_BUFFER_SIZE 256
#define [MODULE_NAME_UPPER]_UPDATE_INTERVAL_MS 1000

// === ERROR CODES ===
typedef enum {
    [MODULE_NAME_UPPER]_SUCCESS = 0,
    [MODULE_NAME_UPPER]_ERROR_INIT_FAILED = -1,
    [MODULE_NAME_UPPER]_ERROR_INVALID_PARAM = -2,
    [MODULE_NAME_UPPER]_ERROR_TIMEOUT = -3,
    [MODULE_NAME_UPPER]_ERROR_COMMUNICATION = -4,
    [MODULE_NAME_UPPER]_ERROR_MEMORY = -5,
    [MODULE_NAME_UPPER]_ERROR_HARDWARE = -6
} [ModuleName]Error_t;

// === MODULE STATE ===
typedef enum {
    [MODULE_NAME_UPPER]_STATE_UNINITIALIZED = 0,
    [MODULE_NAME_UPPER]_STATE_INITIALIZING,
    [MODULE_NAME_UPPER]_STATE_IDLE,
    [MODULE_NAME_UPPER]_STATE_ACTIVE,
    [MODULE_NAME_UPPER]_STATE_ERROR,
    [MODULE_NAME_UPPER]_STATE_SUSPENDED
} [ModuleName]State_t;

// === DATA STRUCTURES ===

/**
 * @brief Configuration structure for [MODULE_NAME] module
 */
struct [ModuleName]Config {
    bool enabled;                           // Module enable flag
    uint16_t updateInterval;               // Update interval in milliseconds
    uint8_t maxRetries;                    // Maximum retry attempts
    uint32_t timeout;                      // Operation timeout in milliseconds
    // Add module-specific configuration fields here
};

/**
 * @brief Status structure for [MODULE_NAME] module
 */
struct [ModuleName]Status {
    [ModuleName]State_t state;             // Current module state
    uint32_t lastUpdateTime;               // Last update timestamp
    uint32_t errorCount;                   // Total error count
    [ModuleName]Error_t lastError;         // Last error code
    bool isInitialized;                    // Initialization status
    // Add module-specific status fields here
};

/**
 * @brief Main data structure for [MODULE_NAME] module
 */
struct [ModuleName]Data {
    [ModuleName]Config config;             // Module configuration
    [ModuleName]Status status;             // Module status
    uint8_t buffer[1024];                  // Data buffer
    size_t bufferSize;                     // Current buffer size
    // Add module-specific data fields here
};

// === CALLBACK FUNCTION TYPES ===

/**
 * @brief Callback function type for [MODULE_NAME] events
 * @param eventType Event type identifier
 * @param data Pointer to event data
 * @param dataSize Size of event data
 */
typedef void (*[ModuleName]EventCallback_t)(uint16_t eventType, void* data, size_t dataSize);

/**
 * @brief Callback function type for [MODULE_NAME] errors
 * @param error Error code
 * @param errorMessage Error description string
 */
typedef void (*[ModuleName]ErrorCallback_t)([ModuleName]Error_t error, const char* errorMessage);

// === GLOBAL VARIABLES ===
extern [ModuleName]Data g[ModuleName]Data;
extern [ModuleName]EventCallback_t g[ModuleName]EventCallback;
extern [ModuleName]ErrorCallback_t g[ModuleName]ErrorCallback;

// === FUNCTION DECLARATIONS ===

// === INITIALIZATION AND CLEANUP ===

/**
 * @brief Initialize [MODULE_NAME] module
 * @param config Pointer to module configuration
 * @return [ModuleName]Error_t Success or error code
 */
[ModuleName]Error_t [moduleName]Init(const [ModuleName]Config* config);

/**
 * @brief Cleanup [MODULE_NAME] module resources
 * @return [ModuleName]Error_t Success or error code
 */
[ModuleName]Error_t [moduleName]Cleanup();

/**
 * @brief Reset [MODULE_NAME] module to default state
 * @return [ModuleName]Error_t Success or error code
 */
[ModuleName]Error_t [moduleName]Reset();

// === CONFIGURATION MANAGEMENT ===

/**
 * @brief Set [MODULE_NAME] configuration
 * @param config Pointer to new configuration
 * @return [ModuleName]Error_t Success or error code
 */
[ModuleName]Error_t [moduleName]SetConfig(const [ModuleName]Config* config);

/**
 * @brief Get current [MODULE_NAME] configuration
 * @return Pointer to current configuration
 */
const [ModuleName]Config* [moduleName]GetConfig();

/**
 * @brief Validate [MODULE_NAME] configuration
 * @param config Pointer to configuration to validate
 * @return [ModuleName]Error_t Success if valid, error code if invalid
 */
[ModuleName]Error_t [moduleName]ValidateConfig(const [ModuleName]Config* config);

// === STATUS AND MONITORING ===

/**
 * @brief Get current [MODULE_NAME] status
 * @return Pointer to current status
 */
const [ModuleName]Status* [moduleName]GetStatus();

/**
 * @brief Get current [MODULE_NAME] state
 * @return Current module state
 */
[ModuleName]State_t [moduleName]GetState();

/**
 * @brief Check if [MODULE_NAME] is initialized
 * @return true if initialized, false otherwise
 */
bool [moduleName]IsInitialized();

/**
 * @brief Get error count since last reset
 * @return Number of errors
 */
uint32_t [moduleName]GetErrorCount();

/**
 * @brief Clear error count
 */
void [moduleName]ClearErrorCount();

// === MAIN FUNCTIONALITY ===

/**
 * @brief Main update function for [MODULE_NAME] module
 * Call this function regularly in main loop
 * @return [ModuleName]Error_t Success or error code
 */
[ModuleName]Error_t [moduleName]Update();

/**
 * @brief Start [MODULE_NAME] operation
 * @return [ModuleName]Error_t Success or error code
 */
[ModuleName]Error_t [moduleName]Start();

/**
 * @brief Stop [MODULE_NAME] operation
 * @return [ModuleName]Error_t Success or error code
 */
[ModuleName]Error_t [moduleName]Stop();

/**
 * @brief Suspend [MODULE_NAME] operation
 * @return [ModuleName]Error_t Success or error code
 */
[ModuleName]Error_t [moduleName]Suspend();

/**
 * @brief Resume [MODULE_NAME] operation
 * @return [ModuleName]Error_t Success or error code
 */
[ModuleName]Error_t [moduleName]Resume();

// === CALLBACK MANAGEMENT ===

/**
 * @brief Set event callback function
 * @param callback Pointer to callback function
 */
void [moduleName]SetEventCallback([ModuleName]EventCallback_t callback);

/**
 * @brief Set error callback function
 * @param callback Pointer to callback function
 */
void [moduleName]SetErrorCallback([ModuleName]ErrorCallback_t callback);

// === UTILITY FUNCTIONS ===

/**
 * @brief Get [MODULE_NAME] version string
 * @return Version string
 */
const char* [moduleName]GetVersion();

/**
 * @brief Get [MODULE_NAME] last error message
 * @return Error message string
 */
const char* [moduleName]GetLastErrorMessage();

/**
 * @brief Convert error code to string
 * @param error Error code
 * @return Error description string
 */
const char* [moduleName]ErrorToString([ModuleName]Error_t error);

/**
 * @brief Print [MODULE_NAME] diagnostic information
 */
void [moduleName]PrintDiagnostics();

// === DEBUG FUNCTIONS (only available in debug builds) ===
#ifdef DEBUG_[MODULE_NAME_UPPER]
/**
 * @brief Enable debug output for [MODULE_NAME] module
 * @param enable true to enable, false to disable
 */
void [moduleName]SetDebugEnabled(bool enable);

/**
 * @brief Print [MODULE_NAME] internal state for debugging
 */
void [moduleName]DebugPrintState();
#endif

// === MACROS ===
#define [MODULE_NAME_UPPER]_CHECK_INIT() \
    do { \
        if (![moduleName]IsInitialized()) { \
            DEBUG_PRINTF("[%s] ERROR: Module not initialized\n", __FUNCTION__); \
            return [MODULE_NAME_UPPER]_ERROR_INIT_FAILED; \
        } \
    } while(0)

#define [MODULE_NAME_UPPER]_VALIDATE_POINTER(ptr) \
    do { \
        if ((ptr) == nullptr) { \
            DEBUG_PRINTF("[%s] ERROR: Invalid pointer parameter\n", __FUNCTION__); \
            return [MODULE_NAME_UPPER]_ERROR_INVALID_PARAM; \
        } \
    } while(0)

#endif // [MODULE_NAME_UPPER]_H

// === TEMPLATE USAGE INSTRUCTIONS ===
// 
// To use this template:
// 1. Replace [MODULE_NAME] with your module name (e.g., "sensor_manager")
// 2. Replace [MODULE_NAME_UPPER] with uppercase version (e.g., "SENSOR_MANAGER")
// 3. Replace [ModuleName] with PascalCase version (e.g., "SensorManager")
// 4. Replace [moduleName] with camelCase version (e.g., "sensorManager")
// 5. Replace [DATE] with current date in DD.MM.YYYY format
// 6. Fill in [MODULE_DESCRIPTION] and other placeholder text
// 7. Add module-specific constants, structures, and functions
// 8. Remove this instruction section
//
// Example replacements:
// [MODULE_NAME] -> "battery_monitor"
// [MODULE_NAME_UPPER] -> "BATTERY_MONITOR" 
// [ModuleName] -> "BatteryMonitor"
// [moduleName] -> "batteryMonitor"
//
// =====================================================================