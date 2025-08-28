/**
 * @file config_page.h  
 * @brief Web-Based Configuration Interface for ESP32S3 CAN Bridge
 * 
 * This header defines a comprehensive web-based configuration system for the
 * ESP32S3 CAN to Modbus TCP bridge, providing:
 * - Modern responsive web interface for device configuration
 * - Real-time system status monitoring and diagnostics  
 * - Secure authentication and session management
 * - RESTful API for programmatic configuration access
 * - Live data visualization and graphing capabilities
 * - Firmware update interface with progress tracking
 * 
 * @version 1.0
 * @date 2024-08-28
 * @author Universal Workflow Phase 5 Template System
 * 
 * Web Interface Features:
 * - Dashboard: System overview, status indicators, live metrics
 * - Network Config: IP settings, WiFi credentials, TCP parameters  
 * - CAN Config: Bitrate, filtering, message routing settings
 * - Modbus Config: Register mappings, client limits, timeout settings
 * - Diagnostics: Log viewer, statistics, network tools
 * - Firmware: OTA update interface with rollback capability
 * 
 * Technical Implementation:
 * - Built on ESP32 AsyncWebServer for high performance
 * - JSON-based REST API for all configuration operations
 * - WebSocket support for real-time data updates
 * - Responsive Bootstrap-based UI for mobile compatibility
 * - HTTPS support with configurable certificates
 * - Role-based access control (Admin, Operator, Viewer)
 * 
 * Security Features:
 * - Configurable authentication (local accounts, LDAP)
 * - Session timeout and CSRF protection
 * - Rate limiting for API endpoints
 * - Audit logging of configuration changes
 * - Secure password storage with bcrypt hashing
 * 
 * Usage Example:
 * ```c
 * // Initialize web interface
 * config_page_init();
 * 
 * // Start HTTP server on port 80
 * config_page_start_server(80, false);
 * 
 * // Enable HTTPS on port 443
 * config_page_start_server(443, true);
 * 
 * // Register custom API endpoint
 * config_page_add_api_handler("/api/custom", custom_handler);
 * ```
 */

#pragma once

#include <esp_err.h>
#include <esp_http_server.h>
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ========================================================================== */
/*                              CONFIGURATION                                */
/* ========================================================================== */

/// Web server configuration parameters
#define CONFIG_WEB_PORT_HTTP        80       ///< Default HTTP port
#define CONFIG_WEB_PORT_HTTPS       443      ///< Default HTTPS port  
#define CONFIG_WEB_MAX_SESSIONS     10       ///< Maximum concurrent sessions
#define CONFIG_WEB_SESSION_TIMEOUT  1800     ///< Session timeout (30 minutes)
#define CONFIG_WEB_REQUEST_TIMEOUT  30       ///< Request timeout (seconds)

/// Buffer and memory limits  
#define CONFIG_WEB_MAX_REQUEST_SIZE 8192     ///< Maximum request body size
#define CONFIG_WEB_MAX_RESPONSE_SIZE 32768   ///< Maximum response size
#define CONFIG_WEB_UPLOAD_BUFFER_SIZE 4096   ///< File upload buffer size
#define CONFIG_WEB_JSON_BUFFER_SIZE  2048    ///< JSON parsing buffer size

/// WebSocket configuration
#define CONFIG_WS_MAX_CONNECTIONS   5        ///< Maximum WebSocket connections
#define CONFIG_WS_PING_INTERVAL     30       ///< WebSocket ping interval (seconds)
#define CONFIG_WS_MAX_MESSAGE_SIZE  1024     ///< Maximum WebSocket message size

/// API rate limiting
#define CONFIG_API_RATE_LIMIT       100      ///< Requests per minute per IP
#define CONFIG_API_BURST_LIMIT      20       ///< Burst request limit

/// File paths and storage
#define CONFIG_WEB_ROOT_PATH        "/spiffs" ///< Web root filesystem path
#define CONFIG_CERT_PATH           "/spiffs/cert.pem"  ///< TLS certificate path
#define CONFIG_KEY_PATH            "/spiffs/key.pem"   ///< TLS private key path
#define CONFIG_USER_DB_PATH        "/spiffs/users.db"  ///< User database path

/* ========================================================================== */
/*                              DATA TYPES                                   */
/* ========================================================================== */

/**
 * @brief Web Server Status
 */
typedef enum {
    WEB_STATUS_STOPPED = 0,    ///< Server not running
    WEB_STATUS_STARTING,       ///< Server initialization in progress
    WEB_STATUS_RUNNING,        ///< Server running normally
    WEB_STATUS_ERROR,          ///< Server error state
    WEB_STATUS_STOPPING        ///< Server shutdown in progress
} web_server_status_t;

/**
 * @brief User Authentication Levels
 */
typedef enum {
    USER_LEVEL_VIEWER = 1,     ///< Read-only access to status and diagnostics
    USER_LEVEL_OPERATOR = 2,   ///< Operator access: status + basic configuration
    USER_LEVEL_ADMIN = 3       ///< Full administrative access
} user_access_level_t;

/**
 * @brief HTTP Request Methods
 */
typedef enum {
    HTTP_METHOD_GET = 0,
    HTTP_METHOD_POST,
    HTTP_METHOD_PUT,  
    HTTP_METHOD_DELETE,
    HTTP_METHOD_PATCH,
    HTTP_METHOD_OPTIONS
} http_method_t;

/**
 * @brief WebSocket Message Types
 */
typedef enum {
    WS_MSG_STATUS = 0,         ///< System status update
    WS_MSG_METRICS,            ///< Performance metrics
    WS_MSG_LOG,                ///< Log entry
    WS_MSG_ALERT,              ///< System alert/alarm
    WS_MSG_CONFIG_CHANGE,      ///< Configuration change notification
    WS_MSG_PING,               ///< Keep-alive ping
    WS_MSG_PONG                ///< Keep-alive pong
} websocket_msg_type_t;

/**
 * @brief User Session Information
 */
typedef struct {
    char session_id[33];       ///< Session identifier (32 chars + null)
    char username[32];         ///< Authenticated username
    user_access_level_t level; ///< Access level
    uint32_t login_time;       ///< Login timestamp
    uint32_t last_activity;    ///< Last activity timestamp
    char client_ip[16];        ///< Client IP address
    bool active;               ///< Session active flag
} web_session_t;

/**
 * @brief Web Server Statistics
 */
typedef struct {
    uint32_t total_requests;   ///< Total HTTP requests processed
    uint32_t successful_requests; ///< Successful HTTP responses
    uint32_t failed_requests;  ///< Failed HTTP requests
    uint32_t active_sessions;  ///< Currently active sessions
    uint32_t total_logins;     ///< Total login attempts
    uint32_t failed_logins;    ///< Failed login attempts
    uint32_t websocket_connections; ///< Active WebSocket connections
    uint32_t api_calls;        ///< API endpoint invocations
    uint32_t file_uploads;     ///< File upload operations
    uint32_t last_request_time; ///< Timestamp of last request
    uint64_t bytes_sent;       ///< Total bytes transmitted
    uint64_t bytes_received;   ///< Total bytes received
} web_server_stats_t;

/**
 * @brief Configuration Data Structure
 * 
 * Centralized configuration structure for all system parameters
 */
typedef struct {
    // Network Configuration
    struct {
        char ssid[32];         ///< WiFi SSID
        char password[64];     ///< WiFi password  
        bool dhcp_enabled;     ///< DHCP client enable
        char ip_address[16];   ///< Static IP address
        char subnet_mask[16];  ///< Subnet mask
        char gateway[16];      ///< Default gateway
        char dns_primary[16];  ///< Primary DNS server
        char dns_secondary[16]; ///< Secondary DNS server
    } network;
    
    // CAN Configuration
    struct {
        uint32_t bitrate;      ///< CAN bitrate (bps)
        bool auto_recovery;    ///< Automatic bus recovery
        uint16_t rx_queue_size; ///< RX queue depth
        uint16_t tx_queue_size; ///< TX queue depth
        bool extended_id;      ///< Extended ID support
        uint32_t acceptance_filter; ///< Acceptance filter
        uint32_t acceptance_mask;   ///< Acceptance mask
    } can;
    
    // Modbus Configuration  
    struct {
        uint16_t tcp_port;     ///< TCP listening port
        uint8_t device_id;     ///< Modbus device ID
        uint16_t response_timeout; ///< Response timeout (ms)
        uint8_t max_clients;   ///< Maximum concurrent clients
        bool coil_read_only;   ///< Coils read-only mode
        bool input_reg_read_only; ///< Input registers read-only
    } modbus;
    
    // System Configuration
    struct {
        char device_name[32];  ///< Device friendly name
        uint16_t update_rate;  ///< Data update rate (ms)
        uint8_t log_level;     ///< Logging verbosity
        bool watchdog_enable;  ///< Hardware watchdog enable
        uint16_t watchdog_timeout; ///< Watchdog timeout (seconds)
        bool ntp_enable;       ///< NTP time sync enable
        char ntp_server[64];   ///< NTP server address
    } system;
    
    // Web Interface Configuration
    struct {
        bool http_enable;      ///< HTTP server enable
        bool https_enable;     ///< HTTPS server enable
        uint16_t http_port;    ///< HTTP port number
        uint16_t https_port;   ///< HTTPS port number
        uint16_t session_timeout; ///< Session timeout (minutes)
        bool auth_required;    ///< Authentication required
        char default_page[64]; ///< Default landing page
    } web;
    
} system_config_t;

/**
 * @brief API Handler Function Type
 * 
 * Custom API endpoint handler function signature
 * 
 * @param req HTTP request handle
 * @return ESP_OK on success, ESP_ERR_* on failure
 */
typedef esp_err_t (*api_handler_t)(httpd_req_t *req);

/**
 * @brief WebSocket Event Handler Function Type
 * 
 * WebSocket event notification handler
 * 
 * @param client_fd Client socket descriptor
 * @param msg_type Message type
 * @param data Message data payload
 * @param length Data length
 * @param user_data User-provided context data
 */
typedef void (*websocket_event_handler_t)(int client_fd, websocket_msg_type_t msg_type,
                                         const uint8_t* data, size_t length, void* user_data);

/**
 * @brief Configuration Change Handler Function Type
 * 
 * Called when configuration parameters are modified via web interface
 * 
 * @param section Configuration section that changed
 * @param old_config Previous configuration
 * @param new_config New configuration  
 * @param user_data User-provided context data
 * @return ESP_OK to accept change, ESP_ERR_* to reject
 */
typedef esp_err_t (*config_change_handler_t)(const char* section, 
                                            const system_config_t* old_config,
                                            const system_config_t* new_config,
                                            void* user_data);

/* ========================================================================== */
/*                              PUBLIC INTERFACE                             */
/* ========================================================================== */

/**
 * @brief Initialize Web Configuration System
 * 
 * Sets up web server infrastructure, loads configuration, initializes security
 * 
 * @return ESP_OK on success, ESP_ERR_* on failure
 * 
 * @note Must be called before starting web server
 * @note Requires SPIFFS filesystem to be mounted
 */
esp_err_t config_page_init(void);

/**
 * @brief Deinitialize Web Configuration System
 * 
 * Stops server, saves configuration, releases resources
 * 
 * @return ESP_OK on success, ESP_ERR_* on failure
 */
esp_err_t config_page_deinit(void);

/**
 * @brief Start Web Server
 * 
 * Starts HTTP/HTTPS server on specified port with optional TLS
 * 
 * @param port TCP port to listen on
 * @param use_https Enable HTTPS with TLS encryption
 * @return ESP_OK on success, ESP_ERR_* on failure
 * 
 * @note For HTTPS, certificate and key files must exist in SPIFFS
 * @note Can be called multiple times to run HTTP and HTTPS simultaneously
 */
esp_err_t config_page_start_server(uint16_t port, bool use_https);

/**
 * @brief Stop Web Server
 * 
 * Gracefully stops web server and closes all connections
 * 
 * @return ESP_OK on success, ESP_ERR_* on failure
 */
esp_err_t config_page_stop_server(void);

/**
 * @brief Get Web Server Status
 * 
 * Returns current status of web server
 * 
 * @return Current web server status
 */
web_server_status_t config_page_get_status(void);

/**
 * @brief Load System Configuration
 * 
 * Loads configuration from persistent storage
 * 
 * @param config Pointer to configuration structure to populate
 * @return ESP_OK on success, ESP_ERR_* on failure
 */
esp_err_t config_page_load_config(system_config_t* config);

/**
 * @brief Save System Configuration
 * 
 * Saves configuration to persistent storage
 * 
 * @param config Pointer to configuration structure to save
 * @return ESP_OK on success, ESP_ERR_* on failure
 */
esp_err_t config_page_save_config(const system_config_t* config);

/**
 * @brief Register Custom API Handler
 * 
 * Adds custom REST API endpoint handler
 * 
 * @param uri API endpoint URI (e.g., "/api/custom")
 * @param method HTTP method for this endpoint
 * @param handler Function to handle requests
 * @param min_access_level Minimum user access level required
 * @return ESP_OK on success, ESP_ERR_* on failure
 * 
 * @note Handler function receives standard httpd_req_t parameter
 * @note Authentication and authorization handled automatically
 */
esp_err_t config_page_add_api_handler(const char* uri, http_method_t method,
                                     api_handler_t handler, user_access_level_t min_access_level);

/**
 * @brief Remove Custom API Handler
 * 
 * Removes previously registered API endpoint handler
 * 
 * @param uri API endpoint URI to remove
 * @param method HTTP method
 * @return ESP_OK on success, ESP_ERR_* on failure
 */
esp_err_t config_page_remove_api_handler(const char* uri, http_method_t method);

/**
 * @brief Register WebSocket Event Handler
 * 
 * Registers callback for WebSocket events and messages
 * 
 * @param handler Function to call on WebSocket events
 * @param user_data User data passed to handler
 * @return ESP_OK on success, ESP_ERR_INVALID_ARG if handler is NULL
 */
esp_err_t config_page_set_websocket_handler(websocket_event_handler_t handler, void* user_data);

/**
 * @brief Send WebSocket Message
 * 
 * Broadcasts message to all connected WebSocket clients
 * 
 * @param msg_type Message type identifier  
 * @param data Message payload data
 * @param length Payload data length
 * @return ESP_OK on success, ESP_ERR_* on failure
 */
esp_err_t config_page_send_websocket_message(websocket_msg_type_t msg_type,
                                            const uint8_t* data, size_t length);

/**
 * @brief Send WebSocket Message to Specific Client
 * 
 * Sends message to single WebSocket client
 * 
 * @param client_fd Client socket descriptor
 * @param msg_type Message type identifier
 * @param data Message payload data
 * @param length Payload data length
 * @return ESP_OK on success, ESP_ERR_* on failure
 */
esp_err_t config_page_send_websocket_message_to_client(int client_fd, websocket_msg_type_t msg_type,
                                                      const uint8_t* data, size_t length);

/**
 * @brief Register Configuration Change Handler
 * 
 * Registers callback for configuration change notifications
 * 
 * @param handler Function to call when configuration changes
 * @param user_data User data passed to handler
 * @return ESP_OK on success, ESP_ERR_INVALID_ARG if handler is NULL
 * 
 * @note Handler can reject configuration changes by returning error
 * @note Changes are applied only if handler returns ESP_OK
 */
esp_err_t config_page_set_config_change_handler(config_change_handler_t handler, void* user_data);

/**
 * @brief Get Web Server Statistics
 * 
 * Retrieves comprehensive web server statistics
 * 
 * @param stats Pointer to statistics structure to populate
 * @return ESP_OK on success, ESP_ERR_INVALID_ARG if stats is NULL
 */
esp_err_t config_page_get_statistics(web_server_stats_t* stats);

/**
 * @brief Reset Web Server Statistics
 * 
 * Clears all accumulated web server statistics
 * 
 * @return ESP_OK on success
 */
esp_err_t config_page_reset_statistics(void);

/**
 * @brief Create User Account
 * 
 * Creates new user account with specified credentials
 * 
 * @param username User account name
 * @param password Plain text password (will be hashed)
 * @param access_level User access level
 * @return ESP_OK on success, ESP_ERR_* on failure
 * 
 * @note Password is hashed using bcrypt before storage
 * @note Username must be unique
 */
esp_err_t config_page_create_user(const char* username, const char* password,
                                 user_access_level_t access_level);

/**
 * @brief Delete User Account
 * 
 * Removes user account from system
 * 
 * @param username User account name to delete
 * @return ESP_OK on success, ESP_ERR_* on failure
 * 
 * @note Active sessions for deleted user are invalidated
 */
esp_err_t config_page_delete_user(const char* username);

/**
 * @brief Authenticate User
 * 
 * Validates user credentials and creates session
 * 
 * @param username User account name  
 * @param password Plain text password
 * @param client_ip Client IP address for session tracking
 * @param session_id Buffer to receive session ID (33 bytes minimum)
 * @return ESP_OK on success, ESP_ERR_* on failure
 * 
 * @note Session ID must be included in subsequent requests
 * @note Sessions automatically expire after configured timeout
 */
esp_err_t config_page_authenticate_user(const char* username, const char* password,
                                       const char* client_ip, char* session_id);

/**
 * @brief Validate User Session
 * 
 * Checks if session ID is valid and updates activity timestamp
 * 
 * @param session_id Session identifier to validate
 * @param session Pointer to session structure to populate (can be NULL)
 * @return ESP_OK if valid, ESP_ERR_* if invalid/expired
 */
esp_err_t config_page_validate_session(const char* session_id, web_session_t* session);

/**
 * @brief Invalidate User Session
 * 
 * Forcibly terminates user session (logout)
 * 
 * @param session_id Session identifier to invalidate
 * @return ESP_OK on success, ESP_ERR_* on failure
 */
esp_err_t config_page_invalidate_session(const char* session_id);

/**
 * @brief Get Active Sessions
 * 
 * Retrieves list of currently active user sessions
 * 
 * @param sessions Array to store session information
 * @param max_sessions Maximum number of sessions to return
 * @param count Pointer to receive actual number of sessions returned
 * @return ESP_OK on success, ESP_ERR_* on failure
 */
esp_err_t config_page_get_active_sessions(web_session_t* sessions, uint16_t max_sessions,
                                         uint16_t* count);

/* ========================================================================== */
/*                              UTILITY FUNCTIONS                            */
/* ========================================================================== */

/**
 * @brief Generate JSON Response
 * 
 * Helper function to create standardized JSON responses
 * 
 * @param success Success flag for response
 * @param message Response message text
 * @param data Additional data object (can be NULL)
 * @param buffer Output buffer for JSON string
 * @param buffer_size Size of output buffer
 * @return ESP_OK on success, ESP_ERR_* on failure
 */
esp_err_t config_page_create_json_response(bool success, const char* message,
                                          const char* data, char* buffer, size_t buffer_size);

/**
 * @brief Parse JSON Request Body
 * 
 * Helper function to parse JSON from HTTP request body
 * 
 * @param req HTTP request handle
 * @param buffer Buffer to store parsed JSON data
 * @param buffer_size Size of buffer  
 * @return ESP_OK on success, ESP_ERR_* on failure
 */
esp_err_t config_page_parse_json_body(httpd_req_t* req, char* buffer, size_t buffer_size);

/**
 * @brief Get Client IP Address
 * 
 * Extracts client IP address from HTTP request
 * 
 * @param req HTTP request handle
 * @param ip_buffer Buffer to store IP address string
 * @param buffer_size Size of IP buffer (minimum 16 bytes)
 * @return ESP_OK on success, ESP_ERR_* on failure
 */
esp_err_t config_page_get_client_ip(httpd_req_t* req, char* ip_buffer, size_t buffer_size);

/**
 * @brief Convert Access Level to String
 * 
 * Returns human-readable access level name
 * 
 * @param level User access level
 * @return Const string describing access level
 */
const char* config_page_access_level_to_string(user_access_level_t level);

/**
 * @brief Convert WebSocket Message Type to String
 * 
 * Returns human-readable message type name
 * 
 * @param msg_type WebSocket message type
 * @return Const string describing message type  
 */
const char* config_page_websocket_msg_type_to_string(websocket_msg_type_t msg_type);

#ifdef __cplusplus
}
#endif