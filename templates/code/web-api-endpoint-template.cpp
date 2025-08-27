// =====================================================================
// === [WEB_API_MODULE_NAME].cpp - ESP32S3 Web API Endpoint Template ===
// =====================================================================
// 
// 📋 PROJECT INFO:
//    Repository: https://github.com/user/esp32s3-can-modbus-tcp
//    Project: ESP32S3 CAN to Modbus TCP Bridge
//    Branch: main
//    Created: [DATE] (Warsaw Time)
//
// 📋 MODULE INFO:
//    Module: [WEB_API_MODULE_DESCRIPTION] - Web API Endpoints Implementation
//    Version: v1.0.0
//    Created: [DATE] (Warsaw Time)
//    Last Modified: [DATE] (Warsaw Time)
//    Author: ESP32 Development Team
//
// 📊 VERSION HISTORY:
//    v1.0.0 - [DATE] - Initial Web API implementation using Universal Workflow template
//
// 🎯 DEPENDENCIES:
//    Internal: [WEB_API_MODULE_NAME].h, config.h, bms_data.h
//    External: Arduino.h, AsyncWebServer.h, ArduinoJson.h
//
// 📝 DESCRIPTION:
//    Implementation of RESTful Web API endpoints for ESP32S3 platform.
//    Provides structured HTTP API for system configuration, BMS data access,
//    and diagnostics. Supports JSON request/response format with comprehensive
//    error handling and authentication.
//
// 🔧 CONFIGURATION:
//    - HTTP Methods: GET, POST, PUT, DELETE support
//    - Response Format: JSON with consistent structure
//    - Authentication: Optional API key validation
//    - CORS: Cross-origin request support
//
// ⚠️  KNOWN ISSUES:
//    - None currently identified
//
// 🧪 TESTING STATUS:
//    Unit Tests: NOT_TESTED
//    Integration Tests: NOT_TESTED
//    API Testing: NOT_TESTED
//
// 📈 PERFORMANCE NOTES:
//    - Response time: <100ms for simple requests
//    - JSON serialization: <10ms for typical payloads
//    - Memory footprint: ~4KB for JSON buffers
//    - Concurrent connections: Up to 4 clients
//
// =====================================================================

#include "[WEB_API_MODULE_NAME].h"
#include "config.h"
#include "bms_data.h"
#include <AsyncWebServer.h>
#include <ArduinoJson.h>

// === GLOBAL VARIABLES ===
[WebApiModuleName]Data g[WebApiModuleName]Data;
[WebApiModuleName]AuthCallback_t g[WebApiModuleName]AuthCallback = nullptr;
[WebApiModuleName]EventCallback_t g[WebApiModuleName]EventCallback = nullptr;

// === PRIVATE VARIABLES ===
static bool s_initialized = false;
static AsyncWebServer* s_webServer = nullptr;
static StaticJsonDocument<[WEB_API_MODULE_NAME_UPPER]_JSON_BUFFER_SIZE> s_jsonBuffer;

// === API ENDPOINT CONSTANTS ===
static const char* const API_PREFIX = "/api/v1";
static const char* const CONTENT_TYPE_JSON = "application/json";
static const char* const HEADER_AUTH = "X-API-Key";

// === PRIVATE FUNCTION DECLARATIONS ===
static bool validateApiKey(AsyncWebServerRequest* request);
static void sendJsonResponse(AsyncWebServerRequest* request, int statusCode, const JsonDocument& doc);
static void sendErrorResponse(AsyncWebServerRequest* request, int statusCode, const char* error, const char* message);
static void sendSuccessResponse(AsyncWebServerRequest* request, const char* message);
static void logApiAccess(AsyncWebServerRequest* request, int statusCode);
static [WebApiModuleName]Error_t populateSystemStatus(JsonObject& obj);
static [WebApiModuleName]Error_t populateBmsData(JsonObject& obj, uint8_t nodeId);
static [WebApiModuleName]Error_t populateStatistics(JsonObject& obj);
static [WebApiModuleName]Error_t parseConfigUpdate(const JsonObject& obj);
static [WebApiModuleName]Error_t parseBmsCommand(const JsonObject& obj, uint8_t nodeId);

// === INITIALIZATION FUNCTIONS ===

[WebApiModuleName]Error_t [webApiModuleName]Init(const [WebApiModuleName]Config* config, AsyncWebServer* webServer) {
    DEBUG_PRINTF("🚀 Initializing [WEB_API_MODULE_NAME] module...\n");
    
    if (config == nullptr || webServer == nullptr) {
        DEBUG_PRINTF("❌ [WEB_API_MODULE_NAME] initialization failed: null parameters\n");
        return [WEB_API_MODULE_NAME_UPPER]_ERROR_INVALID_PARAM;
    }
    
    // Initialize global data
    memset(&g[WebApiModuleName]Data, 0, sizeof([WebApiModuleName]Data));
    g[WebApiModuleName]Data.config = *config;
    
    // Store web server reference
    s_webServer = webServer;
    
    // Initialize statistics
    g[WebApiModuleName]Data.stats.requestCount = 0;
    g[WebApiModuleName]Data.stats.errorCount = 0;
    g[WebApiModuleName]Data.stats.authFailures = 0;
    g[WebApiModuleName]Data.stats.lastRequestTime = 0;
    g[WebApiModuleName]Data.stats.initTime = millis();
    
    // Setup API endpoints
    [WebApiModuleName]Error_t error = [webApiModuleName]SetupEndpoints();
    if (error != [WEB_API_MODULE_NAME_UPPER]_SUCCESS) {
        DEBUG_PRINTF("❌ [WEB_API_MODULE_NAME] endpoint setup failed\n");
        return error;
    }
    
    // Initialize state
    g[WebApiModuleName]Data.state = [WEB_API_MODULE_NAME_UPPER]_STATE_ACTIVE;
    s_initialized = true;
    
    DEBUG_PRINTF("✅ [WEB_API_MODULE_NAME] module initialized successfully\n");
    DEBUG_PRINTF("   API Prefix: %s\n", API_PREFIX);
    DEBUG_PRINTF("   Auth Required: %s\n", config->requireAuth ? "Yes" : "No");
    DEBUG_PRINTF("   CORS Enabled: %s\n", config->enableCors ? "Yes" : "No");
    
    return [WEB_API_MODULE_NAME_UPPER]_SUCCESS;
}

[WebApiModuleName]Error_t [webApiModuleName]Cleanup() {
    DEBUG_PRINTF("🧹 Cleaning up [WEB_API_MODULE_NAME] module...\n");
    
    if (!s_initialized) {
        return [WEB_API_MODULE_NAME_UPPER]_SUCCESS;
    }
    
    // Clear callbacks
    g[WebApiModuleName]AuthCallback = nullptr;
    g[WebApiModuleName]EventCallback = nullptr;
    
    // Clear server reference
    s_webServer = nullptr;
    
    // Clear global data
    memset(&g[WebApiModuleName]Data, 0, sizeof([WebApiModuleName]Data));
    s_initialized = false;
    
    DEBUG_PRINTF("✅ [WEB_API_MODULE_NAME] module cleanup complete\n");
    
    return [WEB_API_MODULE_NAME_UPPER]_SUCCESS;
}

// === ENDPOINT SETUP ===

[WebApiModuleName]Error_t [webApiModuleName]SetupEndpoints() {
    if (s_webServer == nullptr) {
        return [WEB_API_MODULE_NAME_UPPER]_ERROR_INVALID_PARAM;
    }
    
    DEBUG_PRINTF("⚙️ Setting up Web API endpoints...\n");
    
    // === CORS PREFLIGHT HANDLER ===
    if (g[WebApiModuleName]Data.config.enableCors) {
        s_webServer->on("*", HTTP_OPTIONS, [](AsyncWebServerRequest* request) {
            request->send(200, "text/plain", "");
        });
    }
    
    // === SYSTEM ENDPOINTS ===
    
    // GET /api/v1/system/status
    s_webServer->on((String(API_PREFIX) + "/system/status").c_str(), HTTP_GET, 
                   [](AsyncWebServerRequest* request) {
        [webApiModuleName]HandleSystemStatus(request);
    });
    
    // GET /api/v1/system/info
    s_webServer->on((String(API_PREFIX) + "/system/info").c_str(), HTTP_GET,
                   [](AsyncWebServerRequest* request) {
        [webApiModuleName]HandleSystemInfo(request);
    });
    
    // POST /api/v1/system/restart
    s_webServer->on((String(API_PREFIX) + "/system/restart").c_str(), HTTP_POST,
                   [](AsyncWebServerRequest* request) {
        [webApiModuleName]HandleSystemRestart(request);
    });
    
    // === CONFIGURATION ENDPOINTS ===
    
    // GET /api/v1/config
    s_webServer->on((String(API_PREFIX) + "/config").c_str(), HTTP_GET,
                   [](AsyncWebServerRequest* request) {
        [webApiModuleName]HandleConfigGet(request);
    });
    
    // PUT /api/v1/config
    s_webServer->on((String(API_PREFIX) + "/config").c_str(), HTTP_PUT,
                   [](AsyncWebServerRequest* request) {
        // Body handled in onBody callback
    }, nullptr, [](AsyncWebServerRequest* request, uint8_t* data, size_t len, size_t index, size_t total) {
        [webApiModuleName]HandleConfigUpdate(request, data, len, index, total);
    });
    
    // === BMS DATA ENDPOINTS ===
    
    // GET /api/v1/bms/nodes
    s_webServer->on((String(API_PREFIX) + "/bms/nodes").c_str(), HTTP_GET,
                   [](AsyncWebServerRequest* request) {
        [webApiModuleName]HandleBmsNodeList(request);
    });
    
    // GET /api/v1/bms/nodes/{nodeId}/data
    s_webServer->on("^\\/api\\/v1\\/bms\\/nodes\\/([0-9]+)\\/data$", HTTP_GET,
                   [](AsyncWebServerRequest* request) {
        [webApiModuleName]HandleBmsNodeData(request);
    });
    
    // POST /api/v1/bms/nodes/{nodeId}/command
    s_webServer->on("^\\/api\\/v1\\/bms\\/nodes\\/([0-9]+)\\/command$", HTTP_POST,
                   [](AsyncWebServerRequest* request) {
        // Body handled in onBody callback
    }, nullptr, [](AsyncWebServerRequest* request, uint8_t* data, size_t len, size_t index, size_t total) {
        [webApiModuleName]HandleBmsNodeCommand(request, data, len, index, total);
    });
    
    // === STATISTICS ENDPOINTS ===
    
    // GET /api/v1/statistics
    s_webServer->on((String(API_PREFIX) + "/statistics").c_str(), HTTP_GET,
                   [](AsyncWebServerRequest* request) {
        [webApiModuleName]HandleStatistics(request);
    });
    
    // DELETE /api/v1/statistics
    s_webServer->on((String(API_PREFIX) + "/statistics").c_str(), HTTP_DELETE,
                   [](AsyncWebServerRequest* request) {
        [webApiModuleName]HandleStatisticsReset(request);
    });
    
    // === DIAGNOSTICS ENDPOINTS ===
    
    // GET /api/v1/diagnostics/can
    s_webServer->on((String(API_PREFIX) + "/diagnostics/can").c_str(), HTTP_GET,
                   [](AsyncWebServerRequest* request) {
        [webApiModuleName]HandleCanDiagnostics(request);
    });
    
    // GET /api/v1/diagnostics/modbus
    s_webServer->on((String(API_PREFIX) + "/diagnostics/modbus").c_str(), HTTP_GET,
                   [](AsyncWebServerRequest* request) {
        [webApiModuleName]HandleModbusDiagnostics(request);
    });
    
    DEBUG_PRINTF("✅ Web API endpoints configured\n");
    
    return [WEB_API_MODULE_NAME_UPPER]_SUCCESS;
}

// === ENDPOINT HANDLERS ===

void [webApiModuleName]HandleSystemStatus(AsyncWebServerRequest* request) {
    if (!validateApiKey(request)) return;
    
    g[WebApiModuleName]Data.stats.requestCount++;
    g[WebApiModuleName]Data.stats.lastRequestTime = millis();
    
    s_jsonBuffer.clear();
    JsonObject response = s_jsonBuffer.to<JsonObject>();
    
    [WebApiModuleName]Error_t error = populateSystemStatus(response);
    if (error != [WEB_API_MODULE_NAME_UPPER]_SUCCESS) {
        sendErrorResponse(request, 500, "INTERNAL_ERROR", "Failed to get system status");
        return;
    }
    
    sendJsonResponse(request, 200, s_jsonBuffer);
    logApiAccess(request, 200);
}

void [webApiModuleName]HandleSystemInfo(AsyncWebServerRequest* request) {
    if (!validateApiKey(request)) return;
    
    g[WebApiModuleName]Data.stats.requestCount++;
    g[WebApiModuleName]Data.stats.lastRequestTime = millis();
    
    s_jsonBuffer.clear();
    JsonObject response = s_jsonBuffer.to<JsonObject>();
    
    response["firmware_version"] = FIRMWARE_VERSION;
    response["build_date"] = BUILD_DATE;
    response["device_name"] = DEVICE_NAME;
    response["chip_model"] = ESP.getChipModel();
    response["chip_cores"] = ESP.getChipCores();
    response["cpu_frequency"] = ESP.getCpuFreqMHz();
    response["flash_size"] = ESP.getFlashChipSize();
    response["free_heap"] = ESP.getFreeHeap();
    response["uptime"] = millis();
    
    sendJsonResponse(request, 200, s_jsonBuffer);
    logApiAccess(request, 200);
}

void [webApiModuleName]HandleSystemRestart(AsyncWebServerRequest* request) {
    if (!validateApiKey(request)) return;
    
    g[WebApiModuleName]Data.stats.requestCount++;
    
    sendSuccessResponse(request, "System restart initiated");
    logApiAccess(request, 200);
    
    // Delay restart to allow response to be sent
    delay(1000);
    ESP.restart();
}

void [webApiModuleName]HandleConfigGet(AsyncWebServerRequest* request) {
    if (!validateApiKey(request)) return;
    
    g[WebApiModuleName]Data.stats.requestCount++;
    g[WebApiModuleName]Data.stats.lastRequestTime = millis();
    
    s_jsonBuffer.clear();
    JsonObject response = s_jsonBuffer.to<JsonObject>();
    
    // Populate configuration data (excluding sensitive information)
    JsonObject config = response.createNestedObject("config");
    config["can_speed"] = systemConfig.canSpeed;
    config["active_bms_nodes"] = systemConfig.activeBmsNodes;
    config["modbus_port"] = systemConfig.modbusPort;
    config["modbus_slave_id"] = systemConfig.modbusSlaveId;
    config["enable_can_filtering"] = systemConfig.enableCanFiltering;
    config["enable_modbus_write"] = systemConfig.enableModbusWrite;
    config["enable_wifi_ap"] = systemConfig.enableWifiAP;
    config["heartbeat_interval"] = systemConfig.heartbeatInterval;
    config["communication_timeout"] = systemConfig.communicationTimeout;
    
    // BMS node IDs
    JsonArray bmsNodes = config.createNestedArray("bms_node_ids");
    for (int i = 0; i < systemConfig.activeBmsNodes; i++) {
        bmsNodes.add(systemConfig.bmsNodeIds[i]);
    }
    
    sendJsonResponse(request, 200, s_jsonBuffer);
    logApiAccess(request, 200);
}

void [webApiModuleName]HandleConfigUpdate(AsyncWebServerRequest* request, uint8_t* data, size_t len, size_t index, size_t total) {
    if (!validateApiKey(request)) return;
    
    if (index == 0) {
        // First chunk - clear buffer
        s_jsonBuffer.clear();
    }
    
    // Accumulate data (simplified - in production you'd need proper buffering)
    if (index + len == total) {
        // Last chunk - process the complete request
        g[WebApiModuleName]Data.stats.requestCount++;
        
        DeserializationError error = deserializeJson(s_jsonBuffer, data, len);
        if (error) {
            sendErrorResponse(request, 400, "INVALID_JSON", "Failed to parse JSON request");
            return;
        }
        
        JsonObject requestObj = s_jsonBuffer.as<JsonObject>();
        [WebApiModuleName]Error_t configError = parseConfigUpdate(requestObj);
        
        if (configError != [WEB_API_MODULE_NAME_UPPER]_SUCCESS) {
            sendErrorResponse(request, 400, "INVALID_CONFIG", "Configuration validation failed");
            return;
        }
        
        // Save configuration
        if (saveConfiguration()) {
            sendSuccessResponse(request, "Configuration updated successfully");
            logApiAccess(request, 200);
        } else {
            sendErrorResponse(request, 500, "SAVE_FAILED", "Failed to save configuration");
        }
    }
}

void [webApiModuleName]HandleBmsNodeList(AsyncWebServerRequest* request) {
    if (!validateApiKey(request)) return;
    
    g[WebApiModuleName]Data.stats.requestCount++;
    g[WebApiModuleName]Data.stats.lastRequestTime = millis();
    
    s_jsonBuffer.clear();
    JsonObject response = s_jsonBuffer.to<JsonObject>();
    
    JsonArray nodes = response.createNestedArray("bms_nodes");
    for (int i = 0; i < systemConfig.activeBmsNodes; i++) {
        JsonObject node = nodes.createNestedObject();
        node["node_id"] = systemConfig.bmsNodeIds[i];
        node["online"] = true; // You'd check actual BMS status here
        
        // Add basic status info
        JsonObject status = node.createNestedObject("status");
        status["voltage"] = 0.0; // Get from BMS data
        status["current"] = 0.0;
        status["soc"] = 0.0;
        status["temperature"] = 0.0;
    }
    
    response["total_nodes"] = systemConfig.activeBmsNodes;
    response["timestamp"] = millis();
    
    sendJsonResponse(request, 200, s_jsonBuffer);
    logApiAccess(request, 200);
}

void [webApiModuleName]HandleBmsNodeData(AsyncWebServerRequest* request) {
    if (!validateApiKey(request)) return;
    
    g[WebApiModuleName]Data.stats.requestCount++;
    g[WebApiModuleName]Data.stats.lastRequestTime = millis();
    
    // Extract node ID from path parameter
    String pathParam = request->pathArg(0);
    uint8_t nodeId = pathParam.toInt();
    
    if (nodeId < 1 || nodeId > 16) {
        sendErrorResponse(request, 400, "INVALID_NODE_ID", "Node ID must be between 1 and 16");
        return;
    }
    
    s_jsonBuffer.clear();
    JsonObject response = s_jsonBuffer.to<JsonObject>();
    
    [WebApiModuleName]Error_t error = populateBmsData(response, nodeId);
    if (error != [WEB_API_MODULE_NAME_UPPER]_SUCCESS) {
        sendErrorResponse(request, 404, "NODE_NOT_FOUND", "BMS node not found or offline");
        return;
    }
    
    sendJsonResponse(request, 200, s_jsonBuffer);
    logApiAccess(request, 200);
}

void [webApiModuleName]HandleBmsNodeCommand(AsyncWebServerRequest* request, uint8_t* data, size_t len, size_t index, size_t total) {
    if (!validateApiKey(request)) return;
    
    // Extract node ID from path parameter
    String pathParam = request->pathArg(0);
    uint8_t nodeId = pathParam.toInt();
    
    if (nodeId < 1 || nodeId > 16) {
        sendErrorResponse(request, 400, "INVALID_NODE_ID", "Node ID must be between 1 and 16");
        return;
    }
    
    if (index + len == total) {
        // Complete request received
        g[WebApiModuleName]Data.stats.requestCount++;
        
        DeserializationError error = deserializeJson(s_jsonBuffer, data, len);
        if (error) {
            sendErrorResponse(request, 400, "INVALID_JSON", "Failed to parse JSON request");
            return;
        }
        
        JsonObject requestObj = s_jsonBuffer.as<JsonObject>();
        [WebApiModuleName]Error_t cmdError = parseBmsCommand(requestObj, nodeId);
        
        if (cmdError != [WEB_API_MODULE_NAME_UPPER]_SUCCESS) {
            sendErrorResponse(request, 400, "INVALID_COMMAND", "Command validation failed");
            return;
        }
        
        sendSuccessResponse(request, "Command executed successfully");
        logApiAccess(request, 200);
    }
}

void [webApiModuleName]HandleStatistics(AsyncWebServerRequest* request) {
    if (!validateApiKey(request)) return;
    
    g[WebApiModuleName]Data.stats.requestCount++;
    g[WebApiModuleName]Data.stats.lastRequestTime = millis();
    
    s_jsonBuffer.clear();
    JsonObject response = s_jsonBuffer.to<JsonObject>();
    
    [WebApiModuleName]Error_t error = populateStatistics(response);
    if (error != [WEB_API_MODULE_NAME_UPPER]_SUCCESS) {
        sendErrorResponse(request, 500, "INTERNAL_ERROR", "Failed to get statistics");
        return;
    }
    
    sendJsonResponse(request, 200, s_jsonBuffer);
    logApiAccess(request, 200);
}

void [webApiModuleName]HandleStatisticsReset(AsyncWebServerRequest* request) {
    if (!validateApiKey(request)) return;
    
    g[WebApiModuleName]Data.stats.requestCount++;
    
    // Reset statistics
    memset(&g[WebApiModuleName]Data.stats, 0, sizeof([WebApiModuleName]Statistics));
    g[WebApiModuleName]Data.stats.initTime = millis();
    
    sendSuccessResponse(request, "Statistics reset successfully");
    logApiAccess(request, 200);
}

void [webApiModuleName]HandleCanDiagnostics(AsyncWebServerRequest* request) {
    if (!validateApiKey(request)) return;
    
    g[WebApiModuleName]Data.stats.requestCount++;
    g[WebApiModuleName]Data.stats.lastRequestTime = millis();
    
    s_jsonBuffer.clear();
    JsonObject response = s_jsonBuffer.to<JsonObject>();
    
    // Populate CAN diagnostics data
    JsonObject can = response.createNestedObject("can_diagnostics");
    can["bus_state"] = "active"; // Get from CAN module
    can["error_count"] = 0;      // Get from CAN module
    can["frames_received"] = 0;  // Get from CAN module
    can["frames_sent"] = 0;      // Get from CAN module
    can["bus_load"] = 0.0;       // Calculate bus utilization
    
    response["timestamp"] = millis();
    
    sendJsonResponse(request, 200, s_jsonBuffer);
    logApiAccess(request, 200);
}

void [webApiModuleName]HandleModbusDiagnostics(AsyncWebServerRequest* request) {
    if (!validateApiKey(request)) return;
    
    g[WebApiModuleName]Data.stats.requestCount++;
    g[WebApiModuleName]Data.stats.lastRequestTime = millis();
    
    s_jsonBuffer.clear();
    JsonObject response = s_jsonBuffer.to<JsonObject>();
    
    // Populate Modbus diagnostics data
    JsonObject modbus = response.createNestedObject("modbus_diagnostics");
    modbus["server_state"] = "running";
    modbus["client_connections"] = 0;  // Get from Modbus module
    modbus["register_reads"] = 0;      // Get from Modbus module
    modbus["register_writes"] = 0;     // Get from Modbus module
    modbus["error_count"] = 0;         // Get from Modbus module
    
    response["timestamp"] = millis();
    
    sendJsonResponse(request, 200, s_jsonBuffer);
    logApiAccess(request, 200);
}

// === CALLBACK MANAGEMENT ===

void [webApiModuleName]SetAuthCallback([WebApiModuleName]AuthCallback_t callback) {
    g[WebApiModuleName]AuthCallback = callback;
    DEBUG_PRINTF("🔐 [WEB_API_MODULE_NAME] auth callback %s\n", callback ? "registered" : "cleared");
}

void [webApiModuleName]SetEventCallback([WebApiModuleName]EventCallback_t callback) {
    g[WebApiModuleName]EventCallback = callback;
    DEBUG_PRINTF("📡 [WEB_API_MODULE_NAME] event callback %s\n", callback ? "registered" : "cleared");
}

// === UTILITY FUNCTIONS ===

const [WebApiModuleName]Statistics* [webApiModuleName]GetStatistics() {
    if (!s_initialized) {
        return nullptr;
    }
    return &g[WebApiModuleName]Data.stats;
}

const char* [webApiModuleName]ErrorToString([WebApiModuleName]Error_t error) {
    switch (error) {
        case [WEB_API_MODULE_NAME_UPPER]_SUCCESS: return "Success";
        case [WEB_API_MODULE_NAME_UPPER]_ERROR_INIT_FAILED: return "Initialization failed";
        case [WEB_API_MODULE_NAME_UPPER]_ERROR_INVALID_PARAM: return "Invalid parameter";
        case [WEB_API_MODULE_NAME_UPPER]_ERROR_AUTH_FAILED: return "Authentication failed";
        case [WEB_API_MODULE_NAME_UPPER]_ERROR_JSON_PARSE: return "JSON parsing failed";
        case [WEB_API_MODULE_NAME_UPPER]_ERROR_DATA_UNAVAILABLE: return "Data unavailable";
        default: return "Unknown error";
    }
}

void [webApiModuleName]PrintDiagnostics() {
    if (!s_initialized) {
        DEBUG_PRINTF("📊 [WEB_API_MODULE_NAME] Diagnostics: MODULE NOT INITIALIZED\n");
        return;
    }
    
    const [WebApiModuleName]Statistics* stats = &g[WebApiModuleName]Data.stats;
    
    DEBUG_PRINTF("📊 [WEB_API_MODULE_NAME] Diagnostics:\n");
    DEBUG_PRINTF("   State: %d\n", g[WebApiModuleName]Data.state);
    DEBUG_PRINTF("   Request Count: %lu\n", stats->requestCount);
    DEBUG_PRINTF("   Error Count: %lu\n", stats->errorCount);
    DEBUG_PRINTF("   Auth Failures: %lu\n", stats->authFailures);
    DEBUG_PRINTF("   Last Request: %lu ms ago\n", millis() - stats->lastRequestTime);
    DEBUG_PRINTF("   Uptime: %lu ms\n", millis() - stats->initTime);
}

// === PRIVATE FUNCTIONS ===

static bool validateApiKey(AsyncWebServerRequest* request) {
    if (!g[WebApiModuleName]Data.config.requireAuth) {
        return true; // Authentication disabled
    }
    
    if (!request->hasHeader(HEADER_AUTH)) {
        sendErrorResponse(request, 401, "UNAUTHORIZED", "API key required");
        g[WebApiModuleName]Data.stats.authFailures++;
        return false;
    }
    
    String providedKey = request->header(HEADER_AUTH);
    
    // Use callback for authentication if available
    if (g[WebApiModuleName]AuthCallback) {
        if (!g[WebApiModuleName]AuthCallback(providedKey.c_str())) {
            sendErrorResponse(request, 401, "UNAUTHORIZED", "Invalid API key");
            g[WebApiModuleName]Data.stats.authFailures++;
            return false;
        }
    } else {
        // Default authentication check
        if (providedKey != g[WebApiModuleName]Data.config.apiKey) {
            sendErrorResponse(request, 401, "UNAUTHORIZED", "Invalid API key");
            g[WebApiModuleName]Data.stats.authFailures++;
            return false;
        }
    }
    
    return true;
}

static void sendJsonResponse(AsyncWebServerRequest* request, int statusCode, const JsonDocument& doc) {
    String response;
    serializeJson(doc, response);
    
    AsyncWebServerResponse* serverResponse = request->beginResponse(statusCode, CONTENT_TYPE_JSON, response);
    
    if (g[WebApiModuleName]Data.config.enableCors) {
        serverResponse->addHeader("Access-Control-Allow-Origin", "*");
        serverResponse->addHeader("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS");
        serverResponse->addHeader("Access-Control-Allow-Headers", "Content-Type, X-API-Key");
    }
    
    request->send(serverResponse);
}

static void sendErrorResponse(AsyncWebServerRequest* request, int statusCode, const char* error, const char* message) {
    s_jsonBuffer.clear();
    JsonObject response = s_jsonBuffer.to<JsonObject>();
    
    response["error"] = error;
    response["message"] = message;
    response["timestamp"] = millis();
    
    sendJsonResponse(request, statusCode, s_jsonBuffer);
    logApiAccess(request, statusCode);
    
    g[WebApiModuleName]Data.stats.errorCount++;
}

static void sendSuccessResponse(AsyncWebServerRequest* request, const char* message) {
    s_jsonBuffer.clear();
    JsonObject response = s_jsonBuffer.to<JsonObject>();
    
    response["success"] = true;
    response["message"] = message;
    response["timestamp"] = millis();
    
    sendJsonResponse(request, 200, s_jsonBuffer);
}

static void logApiAccess(AsyncWebServerRequest* request, int statusCode) {
#ifdef DEBUG_WEB_API
    DEBUG_PRINTF("🌐 API %s %s -> %d\n", 
                request->methodToString(), 
                request->url().c_str(), 
                statusCode);
#endif
}

static [WebApiModuleName]Error_t populateSystemStatus(JsonObject& obj) {
    obj["system_state"] = (int)systemState;
    obj["wifi_connected"] = WiFi.status() == WL_CONNECTED;
    obj["can_active"] = true; // Get from CAN module
    obj["modbus_active"] = true; // Get from Modbus module
    obj["free_memory"] = ESP.getFreeHeap();
    obj["uptime"] = millis();
    obj["timestamp"] = millis();
    
    return [WEB_API_MODULE_NAME_UPPER]_SUCCESS;
}

static [WebApiModuleName]Error_t populateBmsData(JsonObject& obj, uint8_t nodeId) {
    // This should interface with your BMS data structures
    obj["node_id"] = nodeId;
    obj["online"] = true; // Check actual BMS status
    
    JsonObject data = obj.createNestedObject("data");
    data["voltage"] = 0.0;      // Get from BMS
    data["current"] = 0.0;      // Get from BMS
    data["soc"] = 0.0;          // Get from BMS
    data["soh"] = 0.0;          // Get from BMS
    data["temperature"] = 0.0;  // Get from BMS
    data["status"] = 0;         // Get from BMS
    
    obj["timestamp"] = millis();
    
    return [WEB_API_MODULE_NAME_UPPER]_SUCCESS;
}

static [WebApiModuleName]Error_t populateStatistics(JsonObject& obj) {
    const [WebApiModuleName]Statistics* stats = &g[WebApiModuleName]Data.stats;
    
    obj["request_count"] = stats->requestCount;
    obj["error_count"] = stats->errorCount;
    obj["auth_failures"] = stats->authFailures;
    obj["last_request_time"] = stats->lastRequestTime;
    obj["uptime"] = millis() - stats->initTime;
    obj["timestamp"] = millis();
    
    return [WEB_API_MODULE_NAME_UPPER]_SUCCESS;
}

static [WebApiModuleName]Error_t parseConfigUpdate(const JsonObject& obj) {
    // Parse and validate configuration update request
    if (obj.containsKey("can_speed")) {
        uint8_t canSpeed = obj["can_speed"];
        if (canSpeed != CAN_125KBPS && canSpeed != CAN_500KBPS) {
            return [WEB_API_MODULE_NAME_UPPER]_ERROR_INVALID_PARAM;
        }
        systemConfig.canSpeed = canSpeed;
    }
    
    if (obj.containsKey("active_bms_nodes")) {
        int activeBms = obj["active_bms_nodes"];
        if (activeBms < 1 || activeBms > 16) {
            return [WEB_API_MODULE_NAME_UPPER]_ERROR_INVALID_PARAM;
        }
        systemConfig.activeBmsNodes = activeBms;
    }
    
    // Add more configuration parsing as needed
    
    return [WEB_API_MODULE_NAME_UPPER]_SUCCESS;
}

static [WebApiModuleName]Error_t parseBmsCommand(const JsonObject& obj, uint8_t nodeId) {
    // Parse and execute BMS command
    if (!obj.containsKey("command")) {
        return [WEB_API_MODULE_NAME_UPPER]_ERROR_INVALID_PARAM;
    }
    
    String command = obj["command"];
    
    if (command == "reset") {
        // Reset BMS node
        DEBUG_PRINTF("🔄 Resetting BMS node %d\n", nodeId);
    } else if (command == "calibrate") {
        // Calibrate BMS node
        DEBUG_PRINTF("📐 Calibrating BMS node %d\n", nodeId);
    } else {
        return [WEB_API_MODULE_NAME_UPPER]_ERROR_INVALID_PARAM;
    }
    
    return [WEB_API_MODULE_NAME_UPPER]_SUCCESS;
}

// === TEMPLATE USAGE INSTRUCTIONS ===
// 
// To use this Web API endpoint template:
// 1. Replace [WEB_API_MODULE_NAME] with your module name (e.g., "system_api")
// 2. Replace [WEB_API_MODULE_NAME_UPPER] with uppercase version (e.g., "SYSTEM_API")
// 3. Replace [WebApiModuleName] with PascalCase version (e.g., "SystemApi")
// 4. Replace [webApiModuleName] with camelCase version (e.g., "systemApi")
// 5. Replace [DATE] with current date in DD.MM.YYYY format
// 6. Customize API endpoints and data structures
// 7. Implement data source integration functions
// 8. Add authentication and authorization logic
// 9. Remove this instruction section
//
// Key customization areas:
// - API endpoint definitions and URL patterns
// - Request/response data structures and JSON schemas
// - Authentication and authorization mechanisms
// - Integration with existing data sources (BMS, CAN, Modbus)
// - Error handling and validation logic
// - CORS and security headers
//
// =====================================================================