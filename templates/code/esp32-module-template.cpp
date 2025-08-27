// =====================================================================
// === [MODULE_NAME].cpp - ESP32S3 CAN to Modbus TCP Bridge ===
// =====================================================================
// 
// 📋 PROJECT INFO:
//    Repository: https://github.com/user/esp32s3-can-modbus-tcp
//    Project: ESP32S3 CAN to Modbus TCP Bridge
//    Branch: main
//    Created: [DATE] (Warsaw Time)
//
// 📋 MODULE INFO:
//    Module: [MODULE_DESCRIPTION] Implementation
//    Version: v1.0.0
//    Created: [DATE] (Warsaw Time)
//    Last Modified: [DATE] (Warsaw Time)
//    Author: ESP32 Development Team
//
// 📊 VERSION HISTORY:
//    v1.0.0 - [DATE] - Initial implementation using Universal Workflow template
//
// 🎯 DEPENDENCIES:
//    Internal: [MODULE_NAME].h, config.h
//    External: Arduino.h, [EXTERNAL_LIBRARIES]
//
// 📝 DESCRIPTION:
//    Implementation of [MODULE_DESCRIPTION] functionality for ESP32S3 platform.
//    Provides [FUNCTIONALITY] with error handling, state management, and 
//    integration with the Universal Workflow system. Optimized for embedded
//    systems with memory and performance considerations.
//
// 🔧 CONFIGURATION:
//    - Memory allocation: Static allocation preferred for embedded systems
//    - Error handling: Comprehensive error codes and logging
//    - State management: Robust state machine implementation
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
//    - Initialization time: [INIT_TIME]
//    - Update cycle time: [UPDATE_TIME]
//    - Stack usage: [STACK_USAGE]
//
// =====================================================================

#include "[MODULE_NAME].h"
#include "config.h"
#include <string.h>

// === GLOBAL VARIABLES ===
[ModuleName]Data g[ModuleName]Data;
[ModuleName]EventCallback_t g[ModuleName]EventCallback = nullptr;
[ModuleName]ErrorCallback_t g[ModuleName]ErrorCallback = nullptr;

// === PRIVATE VARIABLES ===
static bool s_initialized = false;
static const char* s_lastErrorMessage = "";
static uint32_t s_lastUpdateTime = 0;

// === ERROR MESSAGES ===
static const char* const ERROR_MESSAGES[] = {
    "Success",
    "Initialization failed",
    "Invalid parameter",
    "Operation timeout",
    "Communication error",
    "Memory allocation error",
    "Hardware error"
};

// === PRIVATE FUNCTION DECLARATIONS ===
static [ModuleName]Error_t validateInitialization();
static void setError([ModuleName]Error_t error, const char* message);
static void triggerErrorCallback([ModuleName]Error_t error, const char* message);
static void triggerEventCallback(uint16_t eventType, void* data, size_t dataSize);
static [ModuleName]Error_t internalStateUpdate();
static void resetStatistics();

// === INITIALIZATION AND CLEANUP ===

[ModuleName]Error_t [moduleName]Init(const [ModuleName]Config* config) {
    DEBUG_PRINTF("🚀 Initializing [MODULE_NAME] module...\n");
    
    // Validate parameters
    if (config == nullptr) {
        setError([MODULE_NAME_UPPER]_ERROR_INVALID_PARAM, "Configuration pointer is null");
        return [MODULE_NAME_UPPER]_ERROR_INVALID_PARAM;
    }
    
    // Validate configuration
    [ModuleName]Error_t error = [moduleName]ValidateConfig(config);
    if (error != [MODULE_NAME_UPPER]_SUCCESS) {
        setError(error, "Configuration validation failed");
        return error;
    }
    
    // Initialize global data structure
    memset(&g[ModuleName]Data, 0, sizeof([ModuleName]Data));
    
    // Copy configuration
    g[ModuleName]Data.config = *config;
    
    // Initialize status
    g[ModuleName]Data.status.state = [MODULE_NAME_UPPER]_STATE_INITIALIZING;
    g[ModuleName]Data.status.lastUpdateTime = millis();
    g[ModuleName]Data.status.errorCount = 0;
    g[ModuleName]Data.status.lastError = [MODULE_NAME_UPPER]_SUCCESS;
    g[ModuleName]Data.status.isInitialized = false;
    
    // Perform module-specific initialization here
    // Example:
    // if (!initializeHardware()) {
    //     setError([MODULE_NAME_UPPER]_ERROR_HARDWARE, "Hardware initialization failed");
    //     return [MODULE_NAME_UPPER]_ERROR_HARDWARE;
    // }
    
    // Mark as initialized
    s_initialized = true;
    g[ModuleName]Data.status.isInitialized = true;
    g[ModuleName]Data.status.state = [MODULE_NAME_UPPER]_STATE_IDLE;
    
    DEBUG_PRINTF("✅ [MODULE_NAME] module initialized successfully\n");
    triggerEventCallback(0x0001, nullptr, 0); // INIT_SUCCESS event
    
    return [MODULE_NAME_UPPER]_SUCCESS;
}

[ModuleName]Error_t [moduleName]Cleanup() {
    DEBUG_PRINTF("🧹 Cleaning up [MODULE_NAME] module...\n");
    
    if (!s_initialized) {
        return [MODULE_NAME_UPPER]_SUCCESS; // Already cleaned up
    }
    
    // Stop any ongoing operations
    [moduleName]Stop();
    
    // Perform module-specific cleanup here
    // Example:
    // cleanupHardware();
    // freeAllocatedMemory();
    
    // Reset global state
    memset(&g[ModuleName]Data, 0, sizeof([ModuleName]Data));
    s_initialized = false;
    s_lastErrorMessage = "";
    
    // Clear callbacks
    g[ModuleName]EventCallback = nullptr;
    g[ModuleName]ErrorCallback = nullptr;
    
    DEBUG_PRINTF("✅ [MODULE_NAME] module cleanup complete\n");
    
    return [MODULE_NAME_UPPER]_SUCCESS;
}

[ModuleName]Error_t [moduleName]Reset() {
    DEBUG_PRINTF("🔄 Resetting [MODULE_NAME] module...\n");
    
    [MODULE_NAME_UPPER]_CHECK_INIT();
    
    // Store current configuration
    [ModuleName]Config currentConfig = g[ModuleName]Data.config;
    
    // Cleanup and reinitialize
    [moduleName]Cleanup();
    [ModuleName]Error_t error = [moduleName]Init(&currentConfig);
    
    if (error == [MODULE_NAME_UPPER]_SUCCESS) {
        DEBUG_PRINTF("✅ [MODULE_NAME] module reset successfully\n");
        triggerEventCallback(0x0002, nullptr, 0); // RESET_SUCCESS event
    } else {
        DEBUG_PRINTF("❌ [MODULE_NAME] module reset failed: %s\n", [moduleName]ErrorToString(error));
    }
    
    return error;
}

// === CONFIGURATION MANAGEMENT ===

[ModuleName]Error_t [moduleName]SetConfig(const [ModuleName]Config* config) {
    [MODULE_NAME_UPPER]_CHECK_INIT();
    [MODULE_NAME_UPPER]_VALIDATE_POINTER(config);
    
    DEBUG_PRINTF("⚙️ Updating [MODULE_NAME] configuration...\n");
    
    // Validate new configuration
    [ModuleName]Error_t error = [moduleName]ValidateConfig(config);
    if (error != [MODULE_NAME_UPPER]_SUCCESS) {
        setError(error, "Configuration validation failed");
        return error;
    }
    
    // Apply configuration
    g[ModuleName]Data.config = *config;
    
    DEBUG_PRINTF("✅ [MODULE_NAME] configuration updated\n");
    triggerEventCallback(0x0003, (void*)config, sizeof([ModuleName]Config)); // CONFIG_UPDATED event
    
    return [MODULE_NAME_UPPER]_SUCCESS;
}

const [ModuleName]Config* [moduleName]GetConfig() {
    if (!s_initialized) {
        return nullptr;
    }
    return &g[ModuleName]Data.config;
}

[ModuleName]Error_t [moduleName]ValidateConfig(const [ModuleName]Config* config) {
    [MODULE_NAME_UPPER]_VALIDATE_POINTER(config);
    
    // Validate configuration parameters
    if (config->updateInterval < 100 || config->updateInterval > 60000) {
        return [MODULE_NAME_UPPER]_ERROR_INVALID_PARAM;
    }
    
    if (config->maxRetries > 10) {
        return [MODULE_NAME_UPPER]_ERROR_INVALID_PARAM;
    }
    
    if (config->timeout < 1000 || config->timeout > 300000) {
        return [MODULE_NAME_UPPER]_ERROR_INVALID_PARAM;
    }
    
    // Add module-specific validation here
    
    return [MODULE_NAME_UPPER]_SUCCESS;
}

// === STATUS AND MONITORING ===

const [ModuleName]Status* [moduleName]GetStatus() {
    if (!s_initialized) {
        return nullptr;
    }
    return &g[ModuleName]Data.status;
}

[ModuleName]State_t [moduleName]GetState() {
    if (!s_initialized) {
        return [MODULE_NAME_UPPER]_STATE_UNINITIALIZED;
    }
    return g[ModuleName]Data.status.state;
}

bool [moduleName]IsInitialized() {
    return s_initialized && g[ModuleName]Data.status.isInitialized;
}

uint32_t [moduleName]GetErrorCount() {
    if (!s_initialized) {
        return 0;
    }
    return g[ModuleName]Data.status.errorCount;
}

void [moduleName]ClearErrorCount() {
    if (s_initialized) {
        g[ModuleName]Data.status.errorCount = 0;
        DEBUG_PRINTF("📊 [MODULE_NAME] error count cleared\n");
    }
}

// === MAIN FUNCTIONALITY ===

[ModuleName]Error_t [moduleName]Update() {
    [MODULE_NAME_UPPER]_CHECK_INIT();
    
    uint32_t currentTime = millis();
    
    // Check if update interval has elapsed
    if (currentTime - s_lastUpdateTime < g[ModuleName]Data.config.updateInterval) {
        return [MODULE_NAME_UPPER]_SUCCESS; // Not time to update yet
    }
    
    s_lastUpdateTime = currentTime;
    g[ModuleName]Data.status.lastUpdateTime = currentTime;
    
    // Perform internal state update
    [ModuleName]Error_t error = internalStateUpdate();
    if (error != [MODULE_NAME_UPPER]_SUCCESS) {
        setError(error, "Internal state update failed");
        return error;
    }
    
    // Perform module-specific update logic here
    // Example:
    // error = updateHardwareState();
    // if (error != [MODULE_NAME_UPPER]_SUCCESS) {
    //     return error;
    // }
    
    return [MODULE_NAME_UPPER]_SUCCESS;
}

[ModuleName]Error_t [moduleName]Start() {
    [MODULE_NAME_UPPER]_CHECK_INIT();
    
    if (g[ModuleName]Data.status.state != [MODULE_NAME_UPPER]_STATE_IDLE) {
        setError([MODULE_NAME_UPPER]_ERROR_INVALID_PARAM, "Module not in idle state");
        return [MODULE_NAME_UPPER]_ERROR_INVALID_PARAM;
    }
    
    DEBUG_PRINTF("▶️ Starting [MODULE_NAME] operation...\n");
    
    // Perform start operations here
    
    g[ModuleName]Data.status.state = [MODULE_NAME_UPPER]_STATE_ACTIVE;
    
    DEBUG_PRINTF("✅ [MODULE_NAME] started successfully\n");
    triggerEventCallback(0x0004, nullptr, 0); // START_SUCCESS event
    
    return [MODULE_NAME_UPPER]_SUCCESS;
}

[ModuleName]Error_t [moduleName]Stop() {
    [MODULE_NAME_UPPER]_CHECK_INIT();
    
    if (g[ModuleName]Data.status.state != [MODULE_NAME_UPPER]_STATE_ACTIVE) {
        return [MODULE_NAME_UPPER]_SUCCESS; // Already stopped
    }
    
    DEBUG_PRINTF("⏹️ Stopping [MODULE_NAME] operation...\n");
    
    // Perform stop operations here
    
    g[ModuleName]Data.status.state = [MODULE_NAME_UPPER]_STATE_IDLE;
    
    DEBUG_PRINTF("✅ [MODULE_NAME] stopped successfully\n");
    triggerEventCallback(0x0005, nullptr, 0); // STOP_SUCCESS event
    
    return [MODULE_NAME_UPPER]_SUCCESS;
}

[ModuleName]Error_t [moduleName]Suspend() {
    [MODULE_NAME_UPPER]_CHECK_INIT();
    
    if (g[ModuleName]Data.status.state != [MODULE_NAME_UPPER]_STATE_ACTIVE) {
        setError([MODULE_NAME_UPPER]_ERROR_INVALID_PARAM, "Module not in active state");
        return [MODULE_NAME_UPPER]_ERROR_INVALID_PARAM;
    }
    
    DEBUG_PRINTF("⏸️ Suspending [MODULE_NAME] operation...\n");
    
    // Perform suspend operations here
    
    g[ModuleName]Data.status.state = [MODULE_NAME_UPPER]_STATE_SUSPENDED;
    
    DEBUG_PRINTF("✅ [MODULE_NAME] suspended successfully\n");
    triggerEventCallback(0x0006, nullptr, 0); // SUSPEND_SUCCESS event
    
    return [MODULE_NAME_UPPER]_SUCCESS;
}

[ModuleName]Error_t [moduleName]Resume() {
    [MODULE_NAME_UPPER]_CHECK_INIT();
    
    if (g[ModuleName]Data.status.state != [MODULE_NAME_UPPER]_STATE_SUSPENDED) {
        setError([MODULE_NAME_UPPER]_ERROR_INVALID_PARAM, "Module not in suspended state");
        return [MODULE_NAME_UPPER]_ERROR_INVALID_PARAM;
    }
    
    DEBUG_PRINTF("⏯️ Resuming [MODULE_NAME] operation...\n");
    
    // Perform resume operations here
    
    g[ModuleName]Data.status.state = [MODULE_NAME_UPPER]_STATE_ACTIVE;
    
    DEBUG_PRINTF("✅ [MODULE_NAME] resumed successfully\n");
    triggerEventCallback(0x0007, nullptr, 0); // RESUME_SUCCESS event
    
    return [MODULE_NAME_UPPER]_SUCCESS;
}

// === CALLBACK MANAGEMENT ===

void [moduleName]SetEventCallback([ModuleName]EventCallback_t callback) {
    g[ModuleName]EventCallback = callback;
    DEBUG_PRINTF("📡 [MODULE_NAME] event callback %s\n", callback ? "registered" : "cleared");
}

void [moduleName]SetErrorCallback([ModuleName]ErrorCallback_t callback) {
    g[ModuleName]ErrorCallback = callback;
    DEBUG_PRINTF("🚨 [MODULE_NAME] error callback %s\n", callback ? "registered" : "cleared");
}

// === UTILITY FUNCTIONS ===

const char* [moduleName]GetVersion() {
    return [MODULE_NAME_UPPER]_VERSION;
}

const char* [moduleName]GetLastErrorMessage() {
    return s_lastErrorMessage;
}

const char* [moduleName]ErrorToString([ModuleName]Error_t error) {
    int errorIndex = -error; // Convert negative error to positive index
    if (errorIndex < 0 || errorIndex >= (int)(sizeof(ERROR_MESSAGES) / sizeof(ERROR_MESSAGES[0]))) {
        return "Unknown error";
    }
    return ERROR_MESSAGES[errorIndex];
}

void [moduleName]PrintDiagnostics() {
    if (!s_initialized) {
        DEBUG_PRINTF("📊 [MODULE_NAME] Diagnostics: MODULE NOT INITIALIZED\n");
        return;
    }
    
    DEBUG_PRINTF("📊 [MODULE_NAME] Diagnostics:\n");
    DEBUG_PRINTF("   Version: %s\n", [moduleName]GetVersion());
    DEBUG_PRINTF("   State: %d\n", g[ModuleName]Data.status.state);
    DEBUG_PRINTF("   Error Count: %lu\n", g[ModuleName]Data.status.errorCount);
    DEBUG_PRINTF("   Last Error: %s\n", [moduleName]ErrorToString(g[ModuleName]Data.status.lastError));
    DEBUG_PRINTF("   Last Update: %lu ms ago\n", millis() - g[ModuleName]Data.status.lastUpdateTime);
    DEBUG_PRINTF("   Config Enabled: %s\n", g[ModuleName]Data.config.enabled ? "Yes" : "No");
    DEBUG_PRINTF("   Update Interval: %d ms\n", g[ModuleName]Data.config.updateInterval);
}

// === DEBUG FUNCTIONS ===
#ifdef DEBUG_[MODULE_NAME_UPPER]
static bool s_debugEnabled = true;

void [moduleName]SetDebugEnabled(bool enable) {
    s_debugEnabled = enable;
    DEBUG_PRINTF("🐛 [MODULE_NAME] debug output %s\n", enable ? "enabled" : "disabled");
}

void [moduleName]DebugPrintState() {
    if (!s_debugEnabled || !s_initialized) {
        return;
    }
    
    DEBUG_PRINTF("🐛 [MODULE_NAME] Internal State:\n");
    DEBUG_PRINTF("   Initialized: %s\n", s_initialized ? "Yes" : "No");
    DEBUG_PRINTF("   Last Error Message: %s\n", s_lastErrorMessage);
    DEBUG_PRINTF("   Last Update Time: %lu\n", s_lastUpdateTime);
    DEBUG_PRINTF("   Buffer Size: %zu bytes\n", g[ModuleName]Data.bufferSize);
}
#endif

// === PRIVATE FUNCTIONS ===

static [ModuleName]Error_t validateInitialization() {
    if (!s_initialized) {
        return [MODULE_NAME_UPPER]_ERROR_INIT_FAILED;
    }
    return [MODULE_NAME_UPPER]_SUCCESS;
}

static void setError([ModuleName]Error_t error, const char* message) {
    if (s_initialized) {
        g[ModuleName]Data.status.lastError = error;
        g[ModuleName]Data.status.errorCount++;
    }
    
    s_lastErrorMessage = message;
    DEBUG_PRINTF("❌ [MODULE_NAME] Error: %s (%s)\n", [moduleName]ErrorToString(error), message);
    
    triggerErrorCallback(error, message);
}

static void triggerErrorCallback([ModuleName]Error_t error, const char* message) {
    if (g[ModuleName]ErrorCallback) {
        g[ModuleName]ErrorCallback(error, message);
    }
}

static void triggerEventCallback(uint16_t eventType, void* data, size_t dataSize) {
    if (g[ModuleName]EventCallback) {
        g[ModuleName]EventCallback(eventType, data, dataSize);
    }
}

static [ModuleName]Error_t internalStateUpdate() {
    // Perform internal state validation and maintenance
    
    // Check for timeouts
    uint32_t currentTime = millis();
    if (currentTime - g[ModuleName]Data.status.lastUpdateTime > g[ModuleName]Data.config.timeout) {
        // Handle timeout condition
        DEBUG_PRINTF("⚠️ [MODULE_NAME] Update timeout detected\n");
    }
    
    // Add module-specific state update logic here
    
    return [MODULE_NAME_UPPER]_SUCCESS;
}

static void resetStatistics() {
    if (s_initialized) {
        g[ModuleName]Data.status.errorCount = 0;
        g[ModuleName]Data.status.lastError = [MODULE_NAME_UPPER]_SUCCESS;
        s_lastErrorMessage = "";
    }
}

// === TEMPLATE USAGE INSTRUCTIONS ===
// 
// To use this template:
// 1. Replace all placeholder tokens as described in the header file
// 2. Implement module-specific functionality in marked sections
// 3. Add hardware initialization/cleanup code as needed
// 4. Implement the actual business logic in update functions
// 5. Add error handling for module-specific operations
// 6. Test thoroughly on target hardware
// 7. Remove this instruction section
//
// Key areas to customize:
// - Initialization code in [moduleName]Init()
// - Cleanup code in [moduleName]Cleanup()  
// - Main logic in [moduleName]Update()
// - Hardware-specific operations in Start/Stop/Suspend/Resume
// - Configuration validation in [moduleName]ValidateConfig()
// - Internal state management in internalStateUpdate()
//
// =====================================================================