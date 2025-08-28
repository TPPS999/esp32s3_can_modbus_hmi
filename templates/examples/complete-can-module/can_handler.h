/**
 * @file can_handler.h
 * @brief Complete CAN Bus Handler for ESP32S3 - Production Ready Example
 * 
 * This is a comprehensive example of a CAN bus handler implementation for ESP32S3,
 * demonstrating best practices for:
 * - CAN controller initialization and configuration
 * - Message transmission and reception with error handling
 * - Frame filtering and message queue management
 * - Error detection and bus recovery procedures
 * - Integration with FreeRTOS tasks and queues
 * 
 * @version 1.0
 * @date 2024-08-28
 * @author Universal Workflow Phase 5 Template System
 * 
 * Usage:
 * 1. Include this header in your main application
 * 2. Call can_handler_init() during system startup
 * 3. Use can_send_message() to transmit CAN frames
 * 4. Register callback with can_register_rx_callback() for message reception
 * 5. Monitor status with can_get_statistics()
 * 
 * Hardware Requirements:
 * - ESP32S3 with integrated CAN controller (TWAI)
 * - External CAN transceiver (e.g., TJA1050, MCP2551)
 * - Proper 120Ω bus termination
 * - 5V power supply for transceiver
 * 
 * @note This template provides a complete, production-ready implementation
 *       Customize pin assignments, bitrate, and filtering as needed
 */

#pragma once

#include <esp_err.h>
#include <driver/twai.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/queue.h>
#include <freertos/semphr.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ========================================================================== */
/*                              CONFIGURATION                                */
/* ========================================================================== */

/**
 * @brief CAN Handler Configuration Parameters
 * 
 * Customize these values based on your hardware setup and requirements
 */
#define CAN_TX_PIN                  GPIO_NUM_21    ///< CAN TX pin (customize for your board)
#define CAN_RX_PIN                  GPIO_NUM_22    ///< CAN RX pin (customize for your board)

#define CAN_BITRATE                 500000         ///< CAN bitrate (bps) - common: 125k, 250k, 500k, 1M
#define CAN_MAX_RETRY_COUNT         3              ///< Maximum transmission retry attempts
#define CAN_TX_TIMEOUT_MS           1000           ///< Transmission timeout (milliseconds)
#define CAN_RX_TIMEOUT_MS           100            ///< Reception timeout (milliseconds)

#define CAN_TX_QUEUE_SIZE           50             ///< Transmit queue depth
#define CAN_RX_QUEUE_SIZE           100            ///< Receive queue depth
#define CAN_ERROR_QUEUE_SIZE        20             ///< Error event queue depth

#define CAN_TASK_STACK_SIZE         4096           ///< CAN handler task stack size
#define CAN_TASK_PRIORITY           10             ///< CAN handler task priority (high)
#define CAN_TASK_CORE               1              ///< CPU core for CAN task (0 or 1)

#define CAN_STATISTICS_INTERVAL_MS  10000          ///< Statistics reporting interval

/* ========================================================================== */
/*                              DATA TYPES                                   */
/* ========================================================================== */

/**
 * @brief CAN Message Structure
 * 
 * Enhanced message structure with metadata for comprehensive message handling
 */
typedef struct {
    uint32_t id;                    ///< CAN message ID (11-bit or 29-bit)
    bool     extended;              ///< Extended frame format (29-bit ID)
    bool     rtr;                   ///< Remote transmission request
    uint8_t  data_length;          ///< Data length (0-8 bytes)
    uint8_t  data[8];              ///< Message data payload
    uint32_t timestamp;            ///< Reception timestamp (milliseconds)
    uint8_t  retry_count;          ///< Transmission retry count
} can_message_t;

/**
 * @brief CAN Message Filter Configuration
 * 
 * Flexible filtering system for selective message reception
 */
typedef struct {
    uint32_t id;                    ///< Filter ID
    uint32_t mask;                  ///< Filter mask
    bool     extended;              ///< Extended frame filtering
    bool     enabled;               ///< Filter enable/disable
} can_filter_t;

/**
 * @brief CAN Handler Statistics
 * 
 * Comprehensive statistics for monitoring and diagnostics
 */
typedef struct {
    uint32_t messages_transmitted;   ///< Total messages sent
    uint32_t messages_received;      ///< Total messages received
    uint32_t transmission_errors;    ///< Transmission error count
    uint32_t reception_errors;       ///< Reception error count
    uint32_t bus_errors;            ///< Bus error occurrences
    uint32_t queue_overruns;        ///< Queue overflow events
    uint32_t retransmissions;       ///< Message retry count
    uint32_t last_error_timestamp;  ///< Timestamp of last error
    twai_state_t controller_state;  ///< Current controller state
} can_statistics_t;

/**
 * @brief CAN Error Types
 * 
 * Classification of different error conditions
 */
typedef enum {
    CAN_ERROR_NONE = 0,
    CAN_ERROR_TX_TIMEOUT,
    CAN_ERROR_TX_QUEUE_FULL,
    CAN_ERROR_RX_QUEUE_FULL,
    CAN_ERROR_BUS_OFF,
    CAN_ERROR_PASSIVE,
    CAN_ERROR_ARBITRATION_LOST,
    CAN_ERROR_BIT_ERROR,
    CAN_ERROR_STUFF_ERROR,
    CAN_ERROR_CRC_ERROR,
    CAN_ERROR_FORM_ERROR,
    CAN_ERROR_ACK_ERROR
} can_error_type_t;

/**
 * @brief CAN Event Callback Function Type
 * 
 * User-defined callback for handling received CAN messages
 * 
 * @param message Received CAN message
 * @param user_data User-provided context data
 */
typedef void (*can_rx_callback_t)(const can_message_t* message, void* user_data);

/**
 * @brief CAN Error Callback Function Type
 * 
 * User-defined callback for handling CAN errors
 * 
 * @param error_type Type of error that occurred
 * @param user_data User-provided context data
 */
typedef void (*can_error_callback_t)(can_error_type_t error_type, void* user_data);

/* ========================================================================== */
/*                              PUBLIC INTERFACE                             */
/* ========================================================================== */

/**
 * @brief Initialize CAN Handler
 * 
 * Configures CAN controller, creates necessary tasks and queues,
 * and prepares the system for CAN communication.
 * 
 * @return ESP_OK on success, ESP_ERR_* on failure
 * 
 * @note This function must be called before any other CAN operations
 * @note Call only once during system initialization
 */
esp_err_t can_handler_init(void);

/**
 * @brief Deinitialize CAN Handler
 * 
 * Stops CAN operations, deletes tasks and queues, releases resources
 * 
 * @return ESP_OK on success, ESP_ERR_* on failure
 */
esp_err_t can_handler_deinit(void);

/**
 * @brief Send CAN Message
 * 
 * Queues a CAN message for transmission with automatic retry handling
 * 
 * @param message Pointer to message to transmit
 * @param timeout_ms Timeout for queuing operation (0 = no wait)
 * @return ESP_OK on successful queuing, ESP_ERR_* on failure
 * 
 * @note Message is copied into internal queue, original can be freed
 * @note Function returns after queuing, not after transmission completion
 */
esp_err_t can_send_message(const can_message_t* message, uint32_t timeout_ms);

/**
 * @brief Register Message Reception Callback
 * 
 * Registers a callback function to be called when CAN messages are received
 * 
 * @param callback Function to call on message reception
 * @param user_data User data passed to callback function
 * @return ESP_OK on success, ESP_ERR_INVALID_ARG if callback is NULL
 * 
 * @note Only one callback can be registered at a time
 * @note Callback is executed in CAN handler task context
 * @note Keep callback execution time minimal to avoid blocking
 */
esp_err_t can_register_rx_callback(can_rx_callback_t callback, void* user_data);

/**
 * @brief Register Error Callback
 * 
 * Registers a callback function for CAN error notifications
 * 
 * @param callback Function to call on error occurrence
 * @param user_data User data passed to callback function
 * @return ESP_OK on success, ESP_ERR_INVALID_ARG if callback is NULL
 */
esp_err_t can_register_error_callback(can_error_callback_t callback, void* user_data);

/**
 * @brief Configure Message Filter
 * 
 * Sets up acceptance filtering for selective message reception
 * 
 * @param filter Filter configuration
 * @return ESP_OK on success, ESP_ERR_* on failure
 * 
 * @note Filtering reduces CPU load by rejecting unwanted messages
 * @note Multiple filters can be configured (hardware dependent)
 */
esp_err_t can_configure_filter(const can_filter_t* filter);

/**
 * @brief Get Handler Statistics
 * 
 * Retrieves comprehensive statistics about CAN operations
 * 
 * @param stats Pointer to statistics structure to populate
 * @return ESP_OK on success, ESP_ERR_INVALID_ARG if stats is NULL
 * 
 * @note Statistics are updated continuously during operation
 * @note Useful for monitoring system health and performance
 */
esp_err_t can_get_statistics(can_statistics_t* stats);

/**
 * @brief Reset Statistics
 * 
 * Clears all accumulated statistics counters
 * 
 * @return ESP_OK on success
 * 
 * @note Useful for periodic monitoring and diagnostics
 */
esp_err_t can_reset_statistics(void);

/**
 * @brief Get Controller State
 * 
 * Returns current state of the CAN controller
 * 
 * @return Current TWAI controller state
 * 
 * @note States: RUNNING, BUS_OFF, RECOVERING, STOPPED
 * @note Use for diagnostics and error handling
 */
twai_state_t can_get_controller_state(void);

/**
 * @brief Trigger Bus Recovery
 * 
 * Initiates bus recovery procedure after bus-off condition
 * 
 * @return ESP_OK on success, ESP_ERR_* on failure
 * 
 * @note Use when controller enters bus-off state
 * @note Automatic recovery can be enabled in configuration
 */
esp_err_t can_trigger_bus_recovery(void);

/**
 * @brief Enable/Disable Statistics Logging
 * 
 * Controls periodic logging of CAN statistics to console
 * 
 * @param enable true to enable logging, false to disable
 * @return ESP_OK on success
 * 
 * @note Logs every CAN_STATISTICS_INTERVAL_MS milliseconds
 * @note Useful for debugging and monitoring
 */
esp_err_t can_enable_statistics_logging(bool enable);

/* ========================================================================== */
/*                              UTILITY FUNCTIONS                            */
/* ========================================================================== */

/**
 * @brief Create Standard CAN Message
 * 
 * Helper function to create a standard 11-bit ID CAN message
 * 
 * @param id 11-bit CAN ID (0x000 - 0x7FF)
 * @param data Pointer to data bytes
 * @param length Data length (0-8 bytes)
 * @return Initialized can_message_t structure
 */
can_message_t can_create_standard_message(uint32_t id, const uint8_t* data, uint8_t length);

/**
 * @brief Create Extended CAN Message
 * 
 * Helper function to create an extended 29-bit ID CAN message
 * 
 * @param id 29-bit CAN ID (0x00000000 - 0x1FFFFFFF)
 * @param data Pointer to data bytes
 * @param length Data length (0-8 bytes)
 * @return Initialized can_message_t structure
 */
can_message_t can_create_extended_message(uint32_t id, const uint8_t* data, uint8_t length);

/**
 * @brief Convert Error Code to String
 * 
 * Returns human-readable description of CAN error types
 * 
 * @param error_type Error type to convert
 * @return Const string describing the error
 */
const char* can_error_to_string(can_error_type_t error_type);

#ifdef __cplusplus
}
#endif