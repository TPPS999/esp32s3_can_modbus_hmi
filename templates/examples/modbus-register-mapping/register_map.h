/**
 * @file register_map.h
 * @brief Comprehensive Modbus Register Mapping for ESP32S3 CAN Bridge
 * 
 * This header defines a complete Modbus register mapping system for the ESP32S3
 * CAN to Modbus TCP bridge, demonstrating professional practices for:
 * - Structured register organization and addressing
 * - Type-safe register access with validation
 * - Dynamic register updates from CAN messages
 * - Comprehensive error handling and diagnostics
 * - Thread-safe multi-client access
 * 
 * @version 1.0
 * @date 2024-08-28
 * @author Universal Workflow Phase 5 Template System
 * 
 * Register Map Layout:
 * - Input Registers (30001-39999): CAN data, system status, diagnostics
 * - Holding Registers (40001-49999): Configuration, control, setpoints
 * - Coils (00001-09999): Digital outputs and control flags
 * - Discrete Inputs (10001-19999): Digital inputs and status flags
 * 
 * Usage Example:
 * ```c
 * // Initialize register system
 * register_map_init();
 * 
 * // Update from CAN message
 * can_message_t can_msg = {...};
 * register_update_from_can(&can_msg);
 * 
 * // Handle Modbus read request
 * uint16_t values[10];
 * esp_err_t result = register_read_holding(40001, 10, values);
 * ```
 */

#pragma once

#include <esp_err.h>
#include <stdint.h>
#include <stdbool.h>
#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ========================================================================== */
/*                              CONFIGURATION                                */
/* ========================================================================== */

/// Maximum number of registers per type
#define MAX_INPUT_REGISTERS      1000    ///< Input registers (30001-30999)
#define MAX_HOLDING_REGISTERS    1000    ///< Holding registers (40001-40999)
#define MAX_COILS               1000     ///< Coils (00001-00999)
#define MAX_DISCRETE_INPUTS     1000     ///< Discrete inputs (10001-10999)

/// Register update frequency and timing
#define REGISTER_UPDATE_RATE_MS  100     ///< Update rate for dynamic registers
#define REGISTER_CACHE_TIMEOUT_MS 5000   ///< Cache validity timeout
#define MAX_REGISTER_AGE_MS     10000    ///< Max age before stale flag

/// CAN to Modbus mapping configuration
#define MAX_CAN_MAPPINGS        100      ///< Maximum CAN ID to register mappings
#define CAN_DATA_TIMEOUT_MS     3000     ///< CAN data freshness timeout

/// Data validation ranges
#define MIN_ANALOG_VALUE        -32768   ///< Minimum signed 16-bit value
#define MAX_ANALOG_VALUE         32767   ///< Maximum signed 16-bit value
#define MIN_UNSIGNED_VALUE           0   ///< Minimum unsigned 16-bit value
#define MAX_UNSIGNED_VALUE       65535   ///< Maximum unsigned 16-bit value

/* ========================================================================== */
/*                              DATA TYPES                                   */
/* ========================================================================== */

/**
 * @brief Register Data Types
 * 
 * Defines the interpretation and validation rules for register values
 */
typedef enum {
    REG_TYPE_UINT16 = 0,       ///< Unsigned 16-bit integer
    REG_TYPE_INT16,            ///< Signed 16-bit integer  
    REG_TYPE_UINT32,           ///< Unsigned 32-bit integer (2 registers)
    REG_TYPE_INT32,            ///< Signed 32-bit integer (2 registers)
    REG_TYPE_FLOAT32,          ///< 32-bit IEEE float (2 registers)
    REG_TYPE_BCD,              ///< Binary coded decimal
    REG_TYPE_BITFIELD,         ///< Individual bit fields
    REG_TYPE_STRING,           ///< ASCII string data
    REG_TYPE_TIMESTAMP,        ///< Unix timestamp (2 registers)
    REG_TYPE_ENUM              ///< Enumerated value with validation
} register_data_type_t;

/**
 * @brief Register Access Permissions
 */
typedef enum {
    REG_ACCESS_READ_ONLY = 1,      ///< Read-only register
    REG_ACCESS_WRITE_ONLY = 2,     ///< Write-only register
    REG_ACCESS_READ_WRITE = 3      ///< Read/write register
} register_access_t;

/**
 * @brief Register Update Sources
 */
typedef enum {
    REG_SOURCE_STATIC = 0,     ///< Static configuration value
    REG_SOURCE_CAN,            ///< Updated from CAN messages
    REG_SOURCE_SYSTEM,         ///< System-generated (uptime, errors, etc.)
    REG_SOURCE_COMPUTED,       ///< Computed from other registers
    REG_SOURCE_EXTERNAL        ///< Updated by external systems
} register_source_t;

/**
 * @brief Register Quality Indicators
 */
typedef enum {
    REG_QUALITY_GOOD = 0,      ///< Data is valid and current
    REG_QUALITY_STALE,         ///< Data is valid but old
    REG_QUALITY_INVALID,       ///< Data validation failed
    REG_QUALITY_TIMEOUT,       ///< Source timeout occurred
    REG_QUALITY_ERROR,         ///< Error during data acquisition
    REG_QUALITY_UNKNOWN        ///< Quality status unknown
} register_quality_t;

/**
 * @brief Individual Register Descriptor
 * 
 * Complete metadata and value storage for a single register
 */
typedef struct {
    uint16_t address;              ///< Modbus register address
    uint16_t value;                ///< Current register value
    register_data_type_t type;     ///< Data type and interpretation
    register_access_t access;      ///< Access permissions
    register_source_t source;      ///< Data source type
    register_quality_t quality;    ///< Data quality indicator
    
    uint32_t last_update;          ///< Timestamp of last update (ms)
    uint32_t update_count;         ///< Number of updates since init
    
    uint16_t min_value;            ///< Minimum valid value
    uint16_t max_value;            ///< Maximum valid value
    uint16_t default_value;        ///< Default/reset value
    
    uint32_t can_id;              ///< Associated CAN message ID (if any)
    uint8_t can_byte_offset;      ///< Byte offset in CAN message
    uint8_t can_bit_offset;       ///< Bit offset for boolean values
    
    char description[64];          ///< Human-readable description
    char units[16];               ///< Engineering units (V, A, °C, etc.)
} register_descriptor_t;

/**
 * @brief CAN to Register Mapping
 * 
 * Defines how CAN message data maps to Modbus registers
 */
typedef struct {
    uint32_t can_id;              ///< CAN message identifier
    bool extended_id;             ///< Extended (29-bit) CAN ID
    uint16_t start_register;      ///< First Modbus register address
    uint8_t register_count;       ///< Number of registers to update
    uint8_t can_data_offset;      ///< Starting byte in CAN data
    register_data_type_t data_type; ///< How to interpret CAN data
    float scale_factor;           ///< Scaling factor for numeric data
    int16_t offset;               ///< Offset applied after scaling
    bool enabled;                 ///< Mapping enable/disable flag
} can_register_mapping_t;

/**
 * @brief Register Block Definition
 * 
 * Groups related registers for efficient batch operations
 */
typedef struct {
    uint16_t start_address;        ///< First register in block
    uint16_t count;               ///< Number of registers in block
    register_source_t source;     ///< Primary data source for block
    uint32_t update_rate_ms;      ///< Update frequency for dynamic blocks
    bool auto_update;             ///< Automatic update enable
    char name[32];                ///< Block identifier name
} register_block_t;

/**
 * @brief Register System Statistics
 * 
 * Comprehensive statistics for monitoring and diagnostics
 */
typedef struct {
    uint32_t total_reads;          ///< Total read operations
    uint32_t total_writes;         ///< Total write operations
    uint32_t read_errors;          ///< Read operation errors
    uint32_t write_errors;         ///< Write operation errors
    uint32_t validation_errors;    ///< Data validation failures
    uint32_t can_updates;          ///< Updates from CAN messages
    uint32_t system_updates;       ///< System-generated updates
    uint32_t cache_hits;           ///< Cache hit count
    uint32_t cache_misses;         ///< Cache miss count
    uint32_t stale_registers;      ///< Registers marked as stale
    uint32_t last_error_timestamp; ///< Timestamp of last error
    uint16_t last_error_address;   ///< Register address of last error
} register_statistics_t;

/**
 * @brief Register Change Callback Function Type
 * 
 * Called when register values change, useful for notifications
 * 
 * @param address Register address that changed
 * @param old_value Previous register value
 * @param new_value New register value
 * @param user_data User-provided context data
 */
typedef void (*register_change_callback_t)(uint16_t address, uint16_t old_value, 
                                           uint16_t new_value, void* user_data);

/* ========================================================================== */
/*                              REGISTER MAP LAYOUT                          */
/* ========================================================================== */

/**
 * @brief Standard Register Address Ranges
 * 
 * Organized by functional area for easy maintenance and documentation
 */

/// System Information Registers (Input Registers 30001-30099)
#define REG_SYSTEM_UPTIME_LOW       30001    ///< System uptime low word (seconds)
#define REG_SYSTEM_UPTIME_HIGH      30002    ///< System uptime high word (seconds)
#define REG_FIRMWARE_VERSION_MAJOR  30003    ///< Firmware major version
#define REG_FIRMWARE_VERSION_MINOR  30004    ///< Firmware minor version
#define REG_FIRMWARE_VERSION_PATCH  30005    ///< Firmware patch version
#define REG_HARDWARE_VERSION        30006    ///< Hardware revision number
#define REG_SERIAL_NUMBER_LOW       30007    ///< Device serial number low word
#define REG_SERIAL_NUMBER_HIGH      30008    ///< Device serial number high word
#define REG_CPU_USAGE               30009    ///< CPU utilization percentage
#define REG_MEMORY_FREE             30010    ///< Free memory (KB)
#define REG_TEMPERATURE             30011    ///< Internal temperature (°C × 10)

/// CAN Bus Status Registers (Input Registers 30100-30199)
#define REG_CAN_STATUS              30100    ///< CAN controller status
#define REG_CAN_TX_COUNT_LOW        30101    ///< CAN TX message count low word
#define REG_CAN_TX_COUNT_HIGH       30102    ///< CAN TX message count high word
#define REG_CAN_RX_COUNT_LOW        30103    ///< CAN RX message count low word
#define REG_CAN_RX_COUNT_HIGH       30104    ///< CAN RX message count high word
#define REG_CAN_ERROR_COUNT         30105    ///< CAN error counter
#define REG_CAN_BUS_LOAD            30106    ///< CAN bus utilization percentage
#define REG_CAN_LAST_MESSAGE_ID     30107    ///< Last received CAN ID
#define REG_CAN_MESSAGE_RATE        30108    ///< Messages per second

/// Network Status Registers (Input Registers 30200-30299)
#define REG_IP_ADDRESS_1            30200    ///< IP address bytes 1-2
#define REG_IP_ADDRESS_2            30201    ///< IP address bytes 3-4
#define REG_SUBNET_MASK_1           30202    ///< Subnet mask bytes 1-2
#define REG_SUBNET_MASK_2           30203    ///< Subnet mask bytes 3-4
#define REG_GATEWAY_1               30204    ///< Gateway bytes 1-2
#define REG_GATEWAY_2               30205    ///< Gateway bytes 3-4
#define REG_TCP_CONNECTIONS         30206    ///< Active TCP connections
#define REG_MODBUS_REQUESTS         30207    ///< Total Modbus requests
#define REG_MODBUS_ERRORS           30208    ///< Modbus error responses

/// Process Data Registers (Input Registers 30300-30499)
#define REG_PROCESS_DATA_START      30300    ///< Start of process data area
#define REG_PROCESS_DATA_END        30499    ///< End of process data area

/// Configuration Registers (Holding Registers 40001-40099)
#define REG_CONFIG_CAN_BITRATE      40001    ///< CAN bitrate (bps / 1000)
#define REG_CONFIG_MODBUS_ID        40002    ///< Modbus device ID
#define REG_CONFIG_TCP_PORT         40003    ///< TCP listening port
#define REG_CONFIG_UPDATE_RATE      40004    ///< Data update rate (ms)
#define REG_CONFIG_WATCHDOG_TIMEOUT 40005    ///< Watchdog timeout (seconds)
#define REG_CONFIG_LOG_LEVEL        40006    ///< Logging verbosity level
#define REG_CONFIG_SAVE_SETTINGS    40007    ///< Save configuration to flash
#define REG_CONFIG_RESTORE_DEFAULTS 40008    ///< Restore factory defaults

/// Control Registers (Holding Registers 40100-40199)
#define REG_CONTROL_SYSTEM_RESET    40100    ///< System reset control
#define REG_CONTROL_CAN_RESTART     40101    ///< CAN controller restart
#define REG_CONTROL_NETWORK_RESTART 40102    ///< Network interface restart
#define REG_CONTROL_CLEAR_ERRORS    40103    ///< Clear error counters
#define REG_CONTROL_CALIBRATION_MODE 40104   ///< Enter calibration mode

/// Status Coils (00001-00099)
#define COIL_SYSTEM_READY           1        ///< System ready indicator
#define COIL_CAN_ACTIVE             2        ///< CAN bus active
#define COIL_NETWORK_CONNECTED      3        ///< Network connection active
#define COIL_ERROR_STATE            4        ///< System error state
#define COIL_CALIBRATION_ACTIVE     5        ///< Calibration mode active

/// Control Coils (00100-00199)
#define COIL_ENABLE_LOGGING         100      ///< Enable diagnostic logging
#define COIL_ENABLE_CAN_FORWARD     101      ///< Enable CAN message forwarding
#define COIL_ENABLE_AUTO_UPDATE     102      ///< Enable automatic updates

/* ========================================================================== */
/*                              PUBLIC INTERFACE                             */
/* ========================================================================== */

/**
 * @brief Initialize Register Mapping System
 * 
 * Sets up register storage, creates synchronization objects,
 * and initializes all registers to default values.
 * 
 * @return ESP_OK on success, ESP_ERR_* on failure
 */
esp_err_t register_map_init(void);

/**
 * @brief Deinitialize Register System
 * 
 * Cleans up resources and saves configuration if needed
 * 
 * @return ESP_OK on success, ESP_ERR_* on failure
 */
esp_err_t register_map_deinit(void);

/**
 * @brief Read Input Registers
 * 
 * Reads multiple input registers starting from specified address
 * 
 * @param start_address Starting register address (30001-39999)
 * @param count Number of registers to read
 * @param values Buffer to store register values
 * @return ESP_OK on success, ESP_ERR_* on failure
 */
esp_err_t register_read_input(uint16_t start_address, uint16_t count, uint16_t* values);

/**
 * @brief Read Holding Registers
 * 
 * Reads multiple holding registers starting from specified address
 * 
 * @param start_address Starting register address (40001-49999)
 * @param count Number of registers to read
 * @param values Buffer to store register values
 * @return ESP_OK on success, ESP_ERR_* on failure
 */
esp_err_t register_read_holding(uint16_t start_address, uint16_t count, uint16_t* values);

/**
 * @brief Write Holding Registers
 * 
 * Writes multiple holding registers starting from specified address
 * 
 * @param start_address Starting register address (40001-49999)
 * @param count Number of registers to write
 * @param values Array of values to write
 * @return ESP_OK on success, ESP_ERR_* on failure
 */
esp_err_t register_write_holding(uint16_t start_address, uint16_t count, const uint16_t* values);

/**
 * @brief Read Coils
 * 
 * Reads multiple coil values starting from specified address
 * 
 * @param start_address Starting coil address (00001-09999)
 * @param count Number of coils to read
 * @param values Buffer to store coil states (packed bits)
 * @return ESP_OK on success, ESP_ERR_* on failure
 */
esp_err_t register_read_coils(uint16_t start_address, uint16_t count, uint8_t* values);

/**
 * @brief Write Coils
 * 
 * Writes multiple coil values starting from specified address
 * 
 * @param start_address Starting coil address (00001-09999)
 * @param count Number of coils to write
 * @param values Array of coil states (packed bits)
 * @return ESP_OK on success, ESP_ERR_* on failure
 */
esp_err_t register_write_coils(uint16_t start_address, uint16_t count, const uint8_t* values);

/**
 * @brief Read Discrete Inputs
 * 
 * Reads multiple discrete input values starting from specified address
 * 
 * @param start_address Starting input address (10001-19999)
 * @param count Number of inputs to read
 * @param values Buffer to store input states (packed bits)
 * @return ESP_OK on success, ESP_ERR_* on failure
 */
esp_err_t register_read_discrete(uint16_t start_address, uint16_t count, uint8_t* values);

/**
 * @brief Update Registers from CAN Message
 * 
 * Processes incoming CAN message and updates mapped registers
 * 
 * @param can_id CAN message identifier
 * @param data CAN message data bytes
 * @param length CAN message data length
 * @return ESP_OK on success, ESP_ERR_* on failure
 */
esp_err_t register_update_from_can(uint32_t can_id, const uint8_t* data, uint8_t length);

/**
 * @brief Add CAN to Register Mapping
 * 
 * Configures how CAN messages map to Modbus registers
 * 
 * @param mapping Mapping configuration
 * @return ESP_OK on success, ESP_ERR_* on failure
 */
esp_err_t register_add_can_mapping(const can_register_mapping_t* mapping);

/**
 * @brief Register Change Notification Callback
 * 
 * Registers callback for register value changes
 * 
 * @param callback Function to call on register changes
 * @param user_data User data passed to callback
 * @return ESP_OK on success, ESP_ERR_INVALID_ARG if callback is NULL
 */
esp_err_t register_set_change_callback(register_change_callback_t callback, void* user_data);

/**
 * @brief Get Register Statistics
 * 
 * Retrieves comprehensive statistics about register operations
 * 
 * @param stats Pointer to statistics structure to populate
 * @return ESP_OK on success, ESP_ERR_INVALID_ARG if stats is NULL
 */
esp_err_t register_get_statistics(register_statistics_t* stats);

/**
 * @brief Reset Register Statistics
 * 
 * Clears all accumulated statistics counters
 * 
 * @return ESP_OK on success
 */
esp_err_t register_reset_statistics(void);

/**
 * @brief Validate Register Address and Access
 * 
 * Checks if register address is valid and accessible for specified operation
 * 
 * @param address Register address to validate
 * @param is_write true for write operation, false for read
 * @return ESP_OK if valid, ESP_ERR_* on validation failure
 */
esp_err_t register_validate_address(uint16_t address, bool is_write);

/**
 * @brief Get Register Descriptor
 * 
 * Retrieves complete metadata for specified register
 * 
 * @param address Register address
 * @param descriptor Pointer to descriptor structure to populate
 * @return ESP_OK on success, ESP_ERR_* on failure
 */
esp_err_t register_get_descriptor(uint16_t address, register_descriptor_t* descriptor);

#ifdef __cplusplus
}
#endif