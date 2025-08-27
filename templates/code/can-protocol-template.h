// =====================================================================
// === [CAN_MODULE_NAME].h - ESP32S3 CAN Protocol Handler Template ===
// =====================================================================
// 
// 📋 PROJECT INFO:
//    Repository: https://github.com/user/esp32s3-can-modbus-tcp
//    Project: ESP32S3 CAN to Modbus TCP Bridge
//    Branch: main
//    Created: [DATE] (Warsaw Time)
//
// 📋 MODULE INFO:
//    Module: [CAN_MODULE_DESCRIPTION] - CAN Protocol Handler
//    Version: v1.0.0
//    Created: [DATE] (Warsaw Time)
//    Last Modified: [DATE] (Warsaw Time)
//    Author: ESP32 Development Team
//
// 📊 VERSION HISTORY:
//    v1.0.0 - [DATE] - Initial CAN protocol implementation using Universal Workflow template
//
// 🎯 DEPENDENCIES:
//    Internal: config.h, bms_data.h
//    External: Arduino.h, mcp_can.h
//
// 📝 DESCRIPTION:
//    CAN protocol handler template for ESP32S3 with MCP2515 controller.
//    Provides structured approach for implementing CAN frame processing,
//    filtering, and integration with BMS data structures. Optimized for
//    embedded systems with robust error handling and diagnostics.
//
// 🔧 CONFIGURATION:
//    - CAN Speed: 125/500 kbps support
//    - Frame Filtering: Hardware and software filtering
//    - Buffer Management: Circular buffer for frame queuing
//    - Error Recovery: Automatic bus-off recovery
//
// ⚠️  KNOWN ISSUES:
//    - None currently identified
//
// 🧪 TESTING STATUS:
//    Unit Tests: NOT_TESTED
//    Integration Tests: NOT_TESTED
//    Hardware Testing: NOT_TESTED
//
// 📈 PERFORMANCE NOTES:
//    - Frame processing: <1ms per frame
//    - Buffer capacity: 64 frames
//    - Memory footprint: ~2KB RAM
//    - CAN interrupt response: <100µs
//
// =====================================================================

#ifndef [CAN_MODULE_NAME_UPPER]_H
#define [CAN_MODULE_NAME_UPPER]_H

// === INCLUDES ===
#include <Arduino.h>
#include "config.h"
#include "bms_data.h"
#include <mcp_can.h>

// === TARGET VALIDATION ===
#if !defined(CONFIG_IDF_TARGET_ESP32S3)
    #warning "This CAN module is optimized for ESP32S3 but will attempt to compile anyway"
#endif

// === CAN MODULE CONSTANTS ===
#define [CAN_MODULE_NAME_UPPER]_VERSION "1.0.0"
#define [CAN_MODULE_NAME_UPPER]_FRAME_BUFFER_SIZE 64
#define [CAN_MODULE_NAME_UPPER]_MAX_FRAME_DATA_SIZE 8
#define [CAN_MODULE_NAME_UPPER]_RECEIVE_TIMEOUT_MS 100

// === CAN PROTOCOL CONFIGURATION ===
#define [CAN_MODULE_NAME_UPPER]_DEFAULT_BITRATE CAN_500KBPS
#define [CAN_MODULE_NAME_UPPER]_RETRY_COUNT 3
#define [CAN_MODULE_NAME_UPPER]_ERROR_THRESHOLD 10

// === CAN FRAME IDENTIFIERS ===
// Define your specific CAN IDs here
#define [CAN_MODULE_NAME_UPPER]_FRAME_ID_STATUS    0x[ID1]
#define [CAN_MODULE_NAME_UPPER]_FRAME_ID_DATA      0x[ID2]
#define [CAN_MODULE_NAME_UPPER]_FRAME_ID_ERROR     0x[ID3]
#define [CAN_MODULE_NAME_UPPER]_FRAME_ID_CONFIG    0x[ID4]

// === CAN ERROR CODES ===
typedef enum {
    [CAN_MODULE_NAME_UPPER]_SUCCESS = 0,
    [CAN_MODULE_NAME_UPPER]_ERROR_INIT_FAILED = -1,
    [CAN_MODULE_NAME_UPPER]_ERROR_INVALID_PARAM = -2,
    [CAN_MODULE_NAME_UPPER]_ERROR_TIMEOUT = -3,
    [CAN_MODULE_NAME_UPPER]_ERROR_BUS_OFF = -4,
    [CAN_MODULE_NAME_UPPER]_ERROR_BUFFER_FULL = -5,
    [CAN_MODULE_NAME_UPPER]_ERROR_HARDWARE = -6,
    [CAN_MODULE_NAME_UPPER]_ERROR_INVALID_FRAME = -7,
    [CAN_MODULE_NAME_UPPER]_ERROR_CRC_MISMATCH = -8
} [CanModuleName]Error_t;

// === CAN FRAME TYPES ===
typedef enum {
    [CAN_MODULE_NAME_UPPER]_FRAME_TYPE_STATUS = 0,
    [CAN_MODULE_NAME_UPPER]_FRAME_TYPE_DATA,
    [CAN_MODULE_NAME_UPPER]_FRAME_TYPE_ERROR,
    [CAN_MODULE_NAME_UPPER]_FRAME_TYPE_CONFIG,
    [CAN_MODULE_NAME_UPPER]_FRAME_TYPE_UNKNOWN
} [CanModuleName]FrameType_t;

// === CAN MODULE STATES ===
typedef enum {
    [CAN_MODULE_NAME_UPPER]_STATE_UNINITIALIZED = 0,
    [CAN_MODULE_NAME_UPPER]_STATE_INITIALIZING,
    [CAN_MODULE_NAME_UPPER]_STATE_IDLE,
    [CAN_MODULE_NAME_UPPER]_STATE_RECEIVING,
    [CAN_MODULE_NAME_UPPER]_STATE_TRANSMITTING,
    [CAN_MODULE_NAME_UPPER]_STATE_ERROR,
    [CAN_MODULE_NAME_UPPER]_STATE_BUS_OFF
} [CanModuleName]State_t;

// === CAN FRAME STRUCTURE ===
struct [CanModuleName]Frame {
    unsigned long canId;                       // CAN identifier
    unsigned char dataLength;                  // Data length code (0-8)
    unsigned char data[8];                     // Frame data
    unsigned long timestamp;                   // Receive timestamp
    [CanModuleName]FrameType_t frameType;      // Frame type
    bool isExtended;                           // Extended frame format
    bool isRemote;                             // Remote transmission request
    uint8_t nodeId;                           // Node ID (extracted from CAN ID)
};

// === CAN FILTER CONFIGURATION ===
struct [CanModuleName]Filter {
    unsigned long mask;                        // Filter mask
    unsigned long filter;                      // Filter value
    bool enabled;                              // Filter enable flag
};

// === CAN STATISTICS ===
struct [CanModuleName]Statistics {
    uint32_t framesReceived;                   // Total frames received
    uint32_t framesSent;                       // Total frames sent
    uint32_t framesDropped;                    // Frames dropped due to buffer full
    uint32_t errorFrames;                      // Error frames received
    uint32_t busOffEvents;                     // Bus-off recovery events
    uint32_t timeouts;                         // Reception timeouts
    uint32_t lastErrorCode;                    // Last MCP2515 error code
    unsigned long lastActivityTime;            // Last CAN activity timestamp
};

// === CAN CONFIGURATION ===
struct [CanModuleName]Config {
    uint8_t canSpeed;                          // CAN bit rate (CAN_125KBPS, CAN_500KBPS)
    uint8_t csPin;                             // SPI CS pin
    uint8_t intPin;                            // Interrupt pin
    bool enableFiltering;                      // Enable hardware filtering
    [CanModuleName]Filter filters[6];          // Hardware filters (MCP2515 has 6)
    uint16_t receiveTimeout;                   // Receive timeout in ms
    uint8_t maxRetries;                        // Maximum retry attempts
    bool enableAutoRecovery;                   // Enable automatic bus-off recovery
    bool enableTimestamping;                   // Enable frame timestamping
};

// === CAN MODULE DATA ===
struct [CanModuleName]Data {
    [CanModuleName]Config config;              // Module configuration
    [CanModuleName]State_t state;              // Current state
    [CanModuleName]Statistics stats;           // Statistics counters
    [CanModuleName]Frame frameBuffer[64];      // Circular frame buffer
    uint8_t bufferHead;                        // Buffer head index
    uint8_t bufferTail;                        // Buffer tail index
    uint8_t bufferCount;                       // Current buffer count
    MCP_CAN* canController;                    // MCP2515 controller instance
    bool initialized;                          // Initialization status
    unsigned long lastHealthCheck;             // Last health check time
};

// === CALLBACK FUNCTION TYPES ===

/**
 * @brief Callback function for received CAN frames
 * @param frame Pointer to received frame
 */
typedef void (*[CanModuleName]FrameCallback_t)(const [CanModuleName]Frame* frame);

/**
 * @brief Callback function for CAN errors
 * @param error Error code
 * @param errorMessage Error description
 */
typedef void (*[CanModuleName]ErrorCallback_t)([CanModuleName]Error_t error, const char* errorMessage);

/**
 * @brief Callback function for state changes
 * @param oldState Previous state
 * @param newState New state
 */
typedef void (*[CanModuleName]StateCallback_t)([CanModuleName]State_t oldState, [CanModuleName]State_t newState);

// === GLOBAL VARIABLES ===
extern [CanModuleName]Data g[CanModuleName]Data;
extern [CanModuleName]FrameCallback_t g[CanModuleName]FrameCallback;
extern [CanModuleName]ErrorCallback_t g[CanModuleName]ErrorCallback;
extern [CanModuleName]StateCallback_t g[CanModuleName]StateCallback;

// === FUNCTION DECLARATIONS ===

// === INITIALIZATION AND SETUP ===

/**
 * @brief Initialize CAN module with configuration
 * @param config Pointer to CAN configuration
 * @return [CanModuleName]Error_t Success or error code
 */
[CanModuleName]Error_t [canModuleName]Init(const [CanModuleName]Config* config);

/**
 * @brief Cleanup CAN module resources
 * @return [CanModuleName]Error_t Success or error code
 */
[CanModuleName]Error_t [canModuleName]Cleanup();

/**
 * @brief Reset CAN module to default state
 * @return [CanModuleName]Error_t Success or error code
 */
[CanModuleName]Error_t [canModuleName]Reset();

/**
 * @brief Set default CAN configuration
 * @param config Pointer to configuration to populate with defaults
 */
void [canModuleName]SetDefaultConfig([CanModuleName]Config* config);

// === FRAME OPERATIONS ===

/**
 * @brief Send CAN frame
 * @param frame Pointer to frame to send
 * @return [CanModuleName]Error_t Success or error code
 */
[CanModuleName]Error_t [canModuleName]SendFrame(const [CanModuleName]Frame* frame);

/**
 * @brief Receive CAN frame (blocking)
 * @param frame Pointer to frame structure to fill
 * @param timeoutMs Timeout in milliseconds
 * @return [CanModuleName]Error_t Success or error code
 */
[CanModuleName]Error_t [canModuleName]ReceiveFrame([CanModuleName]Frame* frame, uint16_t timeoutMs);

/**
 * @brief Check if frame is available in buffer
 * @return true if frame available, false otherwise
 */
bool [canModuleName]IsFrameAvailable();

/**
 * @brief Get frame from buffer (non-blocking)
 * @param frame Pointer to frame structure to fill
 * @return [CanModuleName]Error_t Success or error code
 */
[CanModuleName]Error_t [canModuleName]GetFrame([CanModuleName]Frame* frame);

/**
 * @brief Process incoming CAN frames (call in main loop)
 * @return [CanModuleName]Error_t Success or error code
 */
[CanModuleName]Error_t [canModuleName]ProcessFrames();

// === FRAME ANALYSIS ===

/**
 * @brief Determine frame type from CAN ID
 * @param canId CAN identifier
 * @return [CanModuleName]FrameType_t Frame type
 */
[CanModuleName]FrameType_t [canModuleName]GetFrameType(unsigned long canId);

/**
 * @brief Extract node ID from CAN identifier
 * @param canId CAN identifier
 * @return uint8_t Node ID (0 if invalid)
 */
uint8_t [canModuleName]ExtractNodeId(unsigned long canId);

/**
 * @brief Validate CAN frame data
 * @param frame Pointer to frame to validate
 * @return [CanModuleName]Error_t Success if valid, error code if invalid
 */
[CanModuleName]Error_t [canModuleName]ValidateFrame(const [CanModuleName]Frame* frame);

/**
 * @brief Calculate frame checksum (if applicable)
 * @param frame Pointer to frame
 * @return uint16_t Calculated checksum
 */
uint16_t [canModuleName]CalculateChecksum(const [CanModuleName]Frame* frame);

// === FILTERING AND CONFIGURATION ===

/**
 * @brief Set CAN hardware filter
 * @param filterIndex Filter index (0-5 for MCP2515)
 * @param filter Pointer to filter configuration
 * @return [CanModuleName]Error_t Success or error code
 */
[CanModuleName]Error_t [canModuleName]SetFilter(uint8_t filterIndex, const [CanModuleName]Filter* filter);

/**
 * @brief Enable/disable hardware filtering
 * @param enable true to enable, false to disable
 * @return [CanModuleName]Error_t Success or error code
 */
[CanModuleName]Error_t [canModuleName]SetFilteringEnabled(bool enable);

/**
 * @brief Change CAN bit rate
 * @param canSpeed New bit rate (CAN_125KBPS, CAN_500KBPS, etc.)
 * @return [CanModuleName]Error_t Success or error code
 */
[CanModuleName]Error_t [canModuleName]SetBitRate(uint8_t canSpeed);

// === STATUS AND DIAGNOSTICS ===

/**
 * @brief Get current CAN module state
 * @return [CanModuleName]State_t Current state
 */
[CanModuleName]State_t [canModuleName]GetState();

/**
 * @brief Get CAN statistics
 * @return Pointer to statistics structure
 */
const [CanModuleName]Statistics* [canModuleName]GetStatistics();

/**
 * @brief Reset CAN statistics counters
 */
void [canModuleName]ResetStatistics();

/**
 * @brief Check CAN bus health
 * @return [CanModuleName]Error_t Success if healthy, error code if issues detected
 */
[CanModuleName]Error_t [canModuleName]CheckBusHealth();

/**
 * @brief Perform bus-off recovery
 * @return [CanModuleName]Error_t Success or error code
 */
[CanModuleName]Error_t [canModuleName]RecoverFromBusOff();

// === CALLBACK MANAGEMENT ===

/**
 * @brief Set frame received callback
 * @param callback Pointer to callback function
 */
void [canModuleName]SetFrameCallback([CanModuleName]FrameCallback_t callback);

/**
 * @brief Set error callback
 * @param callback Pointer to callback function
 */
void [canModuleName]SetErrorCallback([CanModuleName]ErrorCallback_t callback);

/**
 * @brief Set state change callback
 * @param callback Pointer to callback function
 */
void [canModuleName]SetStateCallback([CanModuleName]StateCallback_t callback);

// === UTILITY FUNCTIONS ===

/**
 * @brief Convert error code to string
 * @param error Error code
 * @return Error description string
 */
const char* [canModuleName]ErrorToString([CanModuleName]Error_t error);

/**
 * @brief Convert state to string
 * @param state State value
 * @return State description string
 */
const char* [canModuleName]StateToString([CanModuleName]State_t state);

/**
 * @brief Convert frame type to string
 * @param frameType Frame type
 * @return Frame type description string
 */
const char* [canModuleName]FrameTypeToString([CanModuleName]FrameType_t frameType);

/**
 * @brief Print frame contents for debugging
 * @param frame Pointer to frame
 */
void [canModuleName]PrintFrame(const [CanModuleName]Frame* frame);

/**
 * @brief Print module diagnostics
 */
void [canModuleName]PrintDiagnostics();

// === INTERRUPT HANDLING ===

/**
 * @brief CAN interrupt service routine (call from ISR)
 * This function should be called from the interrupt handler
 */
void IRAM_ATTR [canModuleName]InterruptHandler();

/**
 * @brief Setup CAN interrupt handling
 * @return [CanModuleName]Error_t Success or error code
 */
[CanModuleName]Error_t [canModuleName]SetupInterrupt();

// === MACROS ===
#define [CAN_MODULE_NAME_UPPER]_CHECK_INIT() \
    do { \
        if (!g[CanModuleName]Data.initialized) { \
            DEBUG_PRINTF("[%s] ERROR: CAN module not initialized\n", __FUNCTION__); \
            return [CAN_MODULE_NAME_UPPER]_ERROR_INIT_FAILED; \
        } \
    } while(0)

#define [CAN_MODULE_NAME_UPPER]_VALIDATE_FRAME(frame) \
    do { \
        if ((frame) == nullptr || (frame)->dataLength > 8) { \
            DEBUG_PRINTF("[%s] ERROR: Invalid frame parameter\n", __FUNCTION__); \
            return [CAN_MODULE_NAME_UPPER]_ERROR_INVALID_PARAM; \
        } \
    } while(0)

#define [CAN_MODULE_NAME_UPPER]_IS_VALID_NODE_ID(nodeId) \
    ((nodeId) >= 1 && (nodeId) <= 16)

#endif // [CAN_MODULE_NAME_UPPER]_H

// === TEMPLATE USAGE INSTRUCTIONS ===
// 
// To use this CAN protocol template:
// 1. Replace [CAN_MODULE_NAME] with your module name (e.g., "battery_can")
// 2. Replace [CAN_MODULE_NAME_UPPER] with uppercase version (e.g., "BATTERY_CAN")
// 3. Replace [CanModuleName] with PascalCase version (e.g., "BatteryCan")
// 4. Replace [canModuleName] with camelCase version (e.g., "batteryCan")
// 5. Replace [DATE] with current date in DD.MM.YYYY format
// 6. Define specific CAN IDs replacing [ID1], [ID2], etc.
// 7. Implement module-specific frame processing logic
// 8. Add protocol-specific validation and parsing
// 9. Remove this instruction section
//
// Key customization areas:
// - CAN frame identifiers and meanings
// - Frame processing and parsing logic
// - Protocol-specific validation rules
// - Integration with existing data structures
// - Error handling for protocol violations
//
// =====================================================================