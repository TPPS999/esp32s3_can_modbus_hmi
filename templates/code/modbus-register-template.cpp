// =====================================================================
// === [MODBUS_MODULE_NAME].cpp - ESP32S3 Modbus Register Handler ===
// =====================================================================
// 
// 📋 PROJECT INFO:
//    Repository: https://github.com/user/esp32s3-can-modbus-tcp
//    Project: ESP32S3 CAN to Modbus TCP Bridge
//    Branch: main
//    Created: [DATE] (Warsaw Time)
//
// 📋 MODULE INFO:
//    Module: [MODBUS_MODULE_DESCRIPTION] - Modbus Register Implementation
//    Version: v1.0.0
//    Created: [DATE] (Warsaw Time)
//    Last Modified: [DATE] (Warsaw Time)
//    Author: ESP32 Development Team
//
// 📊 VERSION HISTORY:
//    v1.0.0 - [DATE] - Initial Modbus register implementation using Universal Workflow template
//
// 🎯 DEPENDENCIES:
//    Internal: [MODBUS_MODULE_NAME].h, config.h, bms_data.h
//    External: Arduino.h
//
// 📝 DESCRIPTION:
//    Implementation of Modbus register mapping and handling for ESP32S3 platform.
//    Provides structured register access, data conversion, and integration with
//    BMS data structures. Supports both holding registers and input registers
//    with comprehensive error handling and validation.
//
// 🔧 CONFIGURATION:
//    - Register Base: Starting address for register mapping
//    - BMS Nodes: Support for up to 16 BMS nodes
//    - Data Types: 16-bit and 32-bit register support
//    - Scaling: Configurable scaling factors for data conversion
//
// ⚠️  KNOWN ISSUES:
//    - None currently identified
//
// 🧪 TESTING STATUS:
//    Unit Tests: NOT_TESTED
//    Integration Tests: NOT_TESTED
//    Modbus Client Testing: NOT_TESTED
//
// 📈 PERFORMANCE NOTES:
//    - Register read: <1ms per operation
//    - Register write: <1ms per operation
//    - Memory footprint: ~3KB for full register map
//    - Update cycle: ~10ms for all registers
//
// =====================================================================

#include "[MODBUS_MODULE_NAME].h"
#include "config.h"
#include "bms_data.h"
#include <string.h>

// === GLOBAL VARIABLES ===
[ModbusModuleName]Data g[ModbusModuleName]Data;
[ModbusModuleName]ReadCallback_t g[ModbusModuleName]ReadCallback = nullptr;
[ModbusModuleName]WriteCallback_t g[ModbusModuleName]WriteCallback = nullptr;

// === REGISTER MAPPING TABLES ===

/**
 * @brief Holding register mapping table
 * Each entry defines register address, data source, scaling, and access rights
 */
static const [ModbusModuleName]RegisterDef holdingRegisters[] = {
    // BMS Node 1 Registers (Base: 1000)
    { 1000, [MODBUS_MODULE_NAME_UPPER]_DATA_VOLTAGE,      [MODBUS_MODULE_NAME_UPPER]_ACCESS_READ_WRITE, 100.0f, 1 },
    { 1001, [MODBUS_MODULE_NAME_UPPER]_DATA_CURRENT,      [MODBUS_MODULE_NAME_UPPER]_ACCESS_READ_WRITE, 100.0f, 1 },
    { 1002, [MODBUS_MODULE_NAME_UPPER]_DATA_SOC,          [MODBUS_MODULE_NAME_UPPER]_ACCESS_READ_ONLY,  10.0f,  1 },
    { 1003, [MODBUS_MODULE_NAME_UPPER]_DATA_SOH,          [MODBUS_MODULE_NAME_UPPER]_ACCESS_READ_ONLY,  10.0f,  1 },
    { 1004, [MODBUS_MODULE_NAME_UPPER]_DATA_TEMPERATURE,  [MODBUS_MODULE_NAME_UPPER]_ACCESS_READ_ONLY,  10.0f,  1 },
    { 1005, [MODBUS_MODULE_NAME_UPPER]_DATA_STATUS,       [MODBUS_MODULE_NAME_UPPER]_ACCESS_READ_ONLY,  1.0f,   1 },
    
    // Add more BMS nodes (increment nodeId)
    // BMS Node 2 Registers (Base: 1100)
    { 1100, [MODBUS_MODULE_NAME_UPPER]_DATA_VOLTAGE,      [MODBUS_MODULE_NAME_UPPER]_ACCESS_READ_WRITE, 100.0f, 2 },
    { 1101, [MODBUS_MODULE_NAME_UPPER]_DATA_CURRENT,      [MODBUS_MODULE_NAME_UPPER]_ACCESS_READ_WRITE, 100.0f, 2 },
    // ... continue for all BMS nodes
    
    // System Configuration Registers (Base: 9000)
    { 9000, [MODBUS_MODULE_NAME_UPPER]_DATA_CAN_SPEED,    [MODBUS_MODULE_NAME_UPPER]_ACCESS_READ_WRITE, 1.0f,   0 },
    { 9001, [MODBUS_MODULE_NAME_UPPER]_DATA_BMS_COUNT,    [MODBUS_MODULE_NAME_UPPER]_ACCESS_READ_WRITE, 1.0f,   0 },
    { 9002, [MODBUS_MODULE_NAME_UPPER]_DATA_SYSTEM_STATE, [MODBUS_MODULE_NAME_UPPER]_ACCESS_READ_ONLY,  1.0f,   0 },
    { 9003, [MODBUS_MODULE_NAME_UPPER]_DATA_ERROR_COUNT,  [MODBUS_MODULE_NAME_UPPER]_ACCESS_READ_ONLY,  1.0f,   0 },
    
    // Diagnostics Registers (Base: 9100)
    { 9100, [MODBUS_MODULE_NAME_UPPER]_DATA_UPTIME_LOW,   [MODBUS_MODULE_NAME_UPPER]_ACCESS_READ_ONLY,  1.0f,   0 },
    { 9101, [MODBUS_MODULE_NAME_UPPER]_DATA_UPTIME_HIGH,  [MODBUS_MODULE_NAME_UPPER]_ACCESS_READ_ONLY,  1.0f,   0 },
    { 9102, [MODBUS_MODULE_NAME_UPPER]_DATA_FREE_MEMORY,  [MODBUS_MODULE_NAME_UPPER]_ACCESS_READ_ONLY,  1.0f,   0 },
    { 9103, [MODBUS_MODULE_NAME_UPPER]_DATA_CAN_ERRORS,   [MODBUS_MODULE_NAME_UPPER]_ACCESS_READ_ONLY,  1.0f,   0 },
};

/**
 * @brief Input register mapping table
 * Read-only registers for sensor data and status information
 */
static const [ModbusModuleName]RegisterDef inputRegisters[] = {
    // Real-time sensor readings
    { 30001, [MODBUS_MODULE_NAME_UPPER]_DATA_VOLTAGE,     [MODBUS_MODULE_NAME_UPPER]_ACCESS_READ_ONLY, 100.0f, 1 },
    { 30002, [MODBUS_MODULE_NAME_UPPER]_DATA_CURRENT,     [MODBUS_MODULE_NAME_UPPER]_ACCESS_READ_ONLY, 100.0f, 1 },
    { 30003, [MODBUS_MODULE_NAME_UPPER]_DATA_POWER,       [MODBUS_MODULE_NAME_UPPER]_ACCESS_READ_ONLY, 10.0f,  1 },
    { 30004, [MODBUS_MODULE_NAME_UPPER]_DATA_TEMPERATURE, [MODBUS_MODULE_NAME_UPPER]_ACCESS_READ_ONLY, 10.0f,  1 },
    
    // Add more input registers as needed
};

// === PRIVATE VARIABLES ===
static bool s_initialized = false;
static uint32_t s_lastUpdateTime = 0;
static uint16_t s_registerCache[3200]; // Cache for register values
static uint32_t s_registerTimestamps[3200]; // Update timestamps

// === PRIVATE FUNCTION DECLARATIONS ===
static [ModbusModuleName]Error_t findRegisterDef(uint16_t address, [ModbusModuleName]RegisterType_t type, const [ModbusModuleName]RegisterDef** regDef);
static [ModbusModuleName]Error_t validateRegisterAccess(const [ModbusModuleName]RegisterDef* regDef, [ModbusModuleName]Access_t requestedAccess);
static [ModbusModuleName]Error_t readBMSData(uint8_t nodeId, [ModbusModuleName]DataType_t dataType, float* value);
static [ModbusModuleName]Error_t writeBMSData(uint8_t nodeId, [ModbusModuleName]DataType_t dataType, float value);
static [ModbusModuleName]Error_t readSystemData([ModbusModuleName]DataType_t dataType, float* value);
static [ModbusModuleName]Error_t writeSystemData([ModbusModuleName]DataType_t dataType, float value);
static uint16_t floatToRegister(float value, float scale);
static float registerToFloat(uint16_t regValue, float scale);
static void updateRegisterCache();
static void logRegisterAccess(uint16_t address, bool isWrite, uint16_t value);

// === INITIALIZATION FUNCTIONS ===

[ModbusModuleName]Error_t [modbusModuleName]Init(const [ModbusModuleName]Config* config) {
    DEBUG_PRINTF("🚀 Initializing [MODBUS_MODULE_NAME] module...\n");
    
    if (config == nullptr) {
        DEBUG_PRINTF("❌ [MODBUS_MODULE_NAME] initialization failed: null config\n");
        return [MODBUS_MODULE_NAME_UPPER]_ERROR_INVALID_PARAM;
    }
    
    // Initialize global data
    memset(&g[ModbusModuleName]Data, 0, sizeof([ModbusModuleName]Data));
    g[ModbusModuleName]Data.config = *config;
    
    // Initialize register cache
    memset(s_registerCache, 0, sizeof(s_registerCache));
    memset(s_registerTimestamps, 0, sizeof(s_registerTimestamps));
    
    // Initialize statistics
    g[ModbusModuleName]Data.stats.registerReads = 0;
    g[ModbusModuleName]Data.stats.registerWrites = 0;
    g[ModbusModuleName]Data.stats.errorCount = 0;
    g[ModbusModuleName]Data.stats.lastError = [MODBUS_MODULE_NAME_UPPER]_SUCCESS;
    g[ModbusModuleName]Data.stats.initTime = millis();
    
    // Validate configuration
    if (config->baseAddress > 65000) {
        DEBUG_PRINTF("❌ [MODBUS_MODULE_NAME] invalid base address: %d\n", config->baseAddress);
        return [MODBUS_MODULE_NAME_UPPER]_ERROR_INVALID_PARAM;
    }
    
    if (config->maxBmsNodes > 16 || config->maxBmsNodes == 0) {
        DEBUG_PRINTF("❌ [MODBUS_MODULE_NAME] invalid BMS node count: %d\n", config->maxBmsNodes);
        return [MODBUS_MODULE_NAME_UPPER]_ERROR_INVALID_PARAM;
    }
    
    // Initialize state
    g[ModbusModuleName]Data.state = [MODBUS_MODULE_NAME_UPPER]_STATE_IDLE;
    s_initialized = true;
    s_lastUpdateTime = millis();
    
    DEBUG_PRINTF("✅ [MODBUS_MODULE_NAME] module initialized successfully\n");
    DEBUG_PRINTF("   Base Address: %d\n", config->baseAddress);
    DEBUG_PRINTF("   Max BMS Nodes: %d\n", config->maxBmsNodes);
    DEBUG_PRINTF("   Holding Registers: %d\n", sizeof(holdingRegisters) / sizeof(holdingRegisters[0]));
    DEBUG_PRINTF("   Input Registers: %d\n", sizeof(inputRegisters) / sizeof(inputRegisters[0]));
    
    return [MODBUS_MODULE_NAME_UPPER]_SUCCESS;
}

[ModbusModuleName]Error_t [modbusModuleName]Cleanup() {
    DEBUG_PRINTF("🧹 Cleaning up [MODBUS_MODULE_NAME] module...\n");
    
    if (!s_initialized) {
        return [MODBUS_MODULE_NAME_UPPER]_SUCCESS;
    }
    
    // Clear callbacks
    g[ModbusModuleName]ReadCallback = nullptr;
    g[ModbusModuleName]WriteCallback = nullptr;
    
    // Clear global data
    memset(&g[ModbusModuleName]Data, 0, sizeof([ModbusModuleName]Data));
    memset(s_registerCache, 0, sizeof(s_registerCache));
    memset(s_registerTimestamps, 0, sizeof(s_registerTimestamps));
    
    s_initialized = false;
    
    DEBUG_PRINTF("✅ [MODBUS_MODULE_NAME] module cleanup complete\n");
    
    return [MODBUS_MODULE_NAME_UPPER]_SUCCESS;
}

// === REGISTER ACCESS FUNCTIONS ===

[ModbusModuleName]Error_t [modbusModuleName]ReadHoldingRegister(uint16_t address, uint16_t* value) {
    if (!s_initialized || value == nullptr) {
        return [MODBUS_MODULE_NAME_UPPER]_ERROR_INVALID_PARAM;
    }
    
    const [ModbusModuleName]RegisterDef* regDef;
    [ModbusModuleName]Error_t error = findRegisterDef(address, [MODBUS_MODULE_NAME_UPPER]_REGISTER_HOLDING, &regDef);
    if (error != [MODBUS_MODULE_NAME_UPPER]_SUCCESS) {
        g[ModbusModuleName]Data.stats.errorCount++;
        g[ModbusModuleName]Data.stats.lastError = error;
        return error;
    }
    
    // Check access rights
    error = validateRegisterAccess(regDef, [MODBUS_MODULE_NAME_UPPER]_ACCESS_READ_ONLY);
    if (error != [MODBUS_MODULE_NAME_UPPER]_SUCCESS) {
        g[ModbusModuleName]Data.stats.errorCount++;
        g[ModbusModuleName]Data.stats.lastError = error;
        return error;
    }
    
    // Read data based on register definition
    float rawValue;
    if (regDef->nodeId == 0) {
        // System data
        error = readSystemData(regDef->dataType, &rawValue);
    } else {
        // BMS data
        error = readBMSData(regDef->nodeId, regDef->dataType, &rawValue);
    }
    
    if (error != [MODBUS_MODULE_NAME_UPPER]_SUCCESS) {
        g[ModbusModuleName]Data.stats.errorCount++;
        g[ModbusModuleName]Data.stats.lastError = error;
        return error;
    }
    
    // Convert to register value
    *value = floatToRegister(rawValue, regDef->scale);
    
    // Update statistics and cache
    g[ModbusModuleName]Data.stats.registerReads++;
    s_registerCache[address - g[ModbusModuleName]Data.config.baseAddress] = *value;
    s_registerTimestamps[address - g[ModbusModuleName]Data.config.baseAddress] = millis();
    
    logRegisterAccess(address, false, *value);
    
    // Trigger callback if set
    if (g[ModbusModuleName]ReadCallback) {
        g[ModbusModuleName]ReadCallback(address, *value);
    }
    
    return [MODBUS_MODULE_NAME_UPPER]_SUCCESS;
}

[ModbusModuleName]Error_t [modbusModuleName]WriteHoldingRegister(uint16_t address, uint16_t value) {
    if (!s_initialized) {
        return [MODBUS_MODULE_NAME_UPPER]_ERROR_INVALID_PARAM;
    }
    
    const [ModbusModuleName]RegisterDef* regDef;
    [ModbusModuleName]Error_t error = findRegisterDef(address, [MODBUS_MODULE_NAME_UPPER]_REGISTER_HOLDING, &regDef);
    if (error != [MODBUS_MODULE_NAME_UPPER]_SUCCESS) {
        g[ModbusModuleName]Data.stats.errorCount++;
        g[ModbusModuleName]Data.stats.lastError = error;
        return error;
    }
    
    // Check access rights
    error = validateRegisterAccess(regDef, [MODBUS_MODULE_NAME_UPPER]_ACCESS_READ_WRITE);
    if (error != [MODBUS_MODULE_NAME_UPPER]_SUCCESS) {
        g[ModbusModuleName]Data.stats.errorCount++;
        g[ModbusModuleName]Data.stats.lastError = error;
        return error;
    }
    
    // Convert register value to float
    float rawValue = registerToFloat(value, regDef->scale);
    
    // Write data based on register definition
    if (regDef->nodeId == 0) {
        // System data
        error = writeSystemData(regDef->dataType, rawValue);
    } else {
        // BMS data
        error = writeBMSData(regDef->nodeId, regDef->dataType, rawValue);
    }
    
    if (error != [MODBUS_MODULE_NAME_UPPER]_SUCCESS) {
        g[ModbusModuleName]Data.stats.errorCount++;
        g[ModbusModuleName]Data.stats.lastError = error;
        return error;
    }
    
    // Update statistics and cache
    g[ModbusModuleName]Data.stats.registerWrites++;
    s_registerCache[address - g[ModbusModuleName]Data.config.baseAddress] = value;
    s_registerTimestamps[address - g[ModbusModuleName]Data.config.baseAddress] = millis();
    
    logRegisterAccess(address, true, value);
    
    // Trigger callback if set
    if (g[ModbusModuleName]WriteCallback) {
        g[ModbusModuleName]WriteCallback(address, value);
    }
    
    return [MODBUS_MODULE_NAME_UPPER]_SUCCESS;
}

[ModbusModuleName]Error_t [modbusModuleName]ReadInputRegister(uint16_t address, uint16_t* value) {
    if (!s_initialized || value == nullptr) {
        return [MODBUS_MODULE_NAME_UPPER]_ERROR_INVALID_PARAM;
    }
    
    const [ModbusModuleName]RegisterDef* regDef;
    [ModbusModuleName]Error_t error = findRegisterDef(address, [MODBUS_MODULE_NAME_UPPER]_REGISTER_INPUT, &regDef);
    if (error != [MODBUS_MODULE_NAME_UPPER]_SUCCESS) {
        g[ModbusModuleName]Data.stats.errorCount++;
        g[ModbusModuleName]Data.stats.lastError = error;
        return error;
    }
    
    // Read data (input registers are always read-only)
    float rawValue;
    if (regDef->nodeId == 0) {
        error = readSystemData(regDef->dataType, &rawValue);
    } else {
        error = readBMSData(regDef->nodeId, regDef->dataType, &rawValue);
    }
    
    if (error != [MODBUS_MODULE_NAME_UPPER]_SUCCESS) {
        g[ModbusModuleName]Data.stats.errorCount++;
        g[ModbusModuleName]Data.stats.lastError = error;
        return error;
    }
    
    // Convert to register value
    *value = floatToRegister(rawValue, regDef->scale);
    
    // Update statistics
    g[ModbusModuleName]Data.stats.registerReads++;
    
    logRegisterAccess(address, false, *value);
    
    return [MODBUS_MODULE_NAME_UPPER]_SUCCESS;
}

// === BULK OPERATIONS ===

[ModbusModuleName]Error_t [modbusModuleName]ReadHoldingRegisters(uint16_t startAddress, uint16_t count, uint16_t* values) {
    if (!s_initialized || values == nullptr || count == 0) {
        return [MODBUS_MODULE_NAME_UPPER]_ERROR_INVALID_PARAM;
    }
    
    if (count > [MODBUS_MODULE_NAME_UPPER]_MAX_REGISTER_COUNT) {
        return [MODBUS_MODULE_NAME_UPPER]_ERROR_INVALID_PARAM;
    }
    
    DEBUG_PRINTF("📖 Reading %d holding registers starting at %d\n", count, startAddress);
    
    for (uint16_t i = 0; i < count; i++) {
        [ModbusModuleName]Error_t error = [modbusModuleName]ReadHoldingRegister(startAddress + i, &values[i]);
        if (error != [MODBUS_MODULE_NAME_UPPER]_SUCCESS) {
            DEBUG_PRINTF("❌ Failed to read register %d: %s\n", startAddress + i, [modbusModuleName]ErrorToString(error));
            return error;
        }
    }
    
    DEBUG_PRINTF("✅ Successfully read %d holding registers\n", count);
    return [MODBUS_MODULE_NAME_UPPER]_SUCCESS;
}

[ModbusModuleName]Error_t [modbusModuleName]WriteHoldingRegisters(uint16_t startAddress, uint16_t count, const uint16_t* values) {
    if (!s_initialized || values == nullptr || count == 0) {
        return [MODBUS_MODULE_NAME_UPPER]_ERROR_INVALID_PARAM;
    }
    
    if (count > [MODBUS_MODULE_NAME_UPPER]_MAX_REGISTER_COUNT) {
        return [MODBUS_MODULE_NAME_UPPER]_ERROR_INVALID_PARAM;
    }
    
    DEBUG_PRINTF("📝 Writing %d holding registers starting at %d\n", count, startAddress);
    
    for (uint16_t i = 0; i < count; i++) {
        [ModbusModuleName]Error_t error = [modbusModuleName]WriteHoldingRegister(startAddress + i, values[i]);
        if (error != [MODBUS_MODULE_NAME_UPPER]_SUCCESS) {
            DEBUG_PRINTF("❌ Failed to write register %d: %s\n", startAddress + i, [modbusModuleName]ErrorToString(error));
            return error;
        }
    }
    
    DEBUG_PRINTF("✅ Successfully wrote %d holding registers\n", count);
    return [MODBUS_MODULE_NAME_UPPER]_SUCCESS;
}

// === STATUS AND DIAGNOSTICS ===

const [ModbusModuleName]Statistics* [modbusModuleName]GetStatistics() {
    if (!s_initialized) {
        return nullptr;
    }
    return &g[ModbusModuleName]Data.stats;
}

[ModbusModuleName]State_t [modbusModuleName]GetState() {
    if (!s_initialized) {
        return [MODBUS_MODULE_NAME_UPPER]_STATE_UNINITIALIZED;
    }
    return g[ModbusModuleName]Data.state;
}

void [modbusModuleName]ResetStatistics() {
    if (s_initialized) {
        g[ModbusModuleName]Data.stats.registerReads = 0;
        g[ModbusModuleName]Data.stats.registerWrites = 0;
        g[ModbusModuleName]Data.stats.errorCount = 0;
        g[ModbusModuleName]Data.stats.lastError = [MODBUS_MODULE_NAME_UPPER]_SUCCESS;
        DEBUG_PRINTF("📊 [MODBUS_MODULE_NAME] statistics reset\n");
    }
}

[ModbusModuleName]Error_t [modbusModuleName]Update() {
    if (!s_initialized) {
        return [MODBUS_MODULE_NAME_UPPER]_ERROR_INIT_FAILED;
    }
    
    uint32_t currentTime = millis();
    if (currentTime - s_lastUpdateTime < g[ModbusModuleName]Data.config.updateInterval) {
        return [MODBUS_MODULE_NAME_UPPER]_SUCCESS; // Not time to update yet
    }
    
    s_lastUpdateTime = currentTime;
    
    // Update register cache with fresh data
    updateRegisterCache();
    
    // Update uptime
    g[ModbusModuleName]Data.stats.uptime = currentTime - g[ModbusModuleName]Data.stats.initTime;
    
    return [MODBUS_MODULE_NAME_UPPER]_SUCCESS;
}

// === CALLBACK MANAGEMENT ===

void [modbusModuleName]SetReadCallback([ModbusModuleName]ReadCallback_t callback) {
    g[ModbusModuleName]ReadCallback = callback;
    DEBUG_PRINTF("📡 [MODBUS_MODULE_NAME] read callback %s\n", callback ? "registered" : "cleared");
}

void [modbusModuleName]SetWriteCallback([ModbusModuleName]WriteCallback_t callback) {
    g[ModbusModuleName]WriteCallback = callback;
    DEBUG_PRINTF("📡 [MODBUS_MODULE_NAME] write callback %s\n", callback ? "registered" : "cleared");
}

// === UTILITY FUNCTIONS ===

const char* [modbusModuleName]ErrorToString([ModbusModuleName]Error_t error) {
    switch (error) {
        case [MODBUS_MODULE_NAME_UPPER]_SUCCESS: return "Success";
        case [MODBUS_MODULE_NAME_UPPER]_ERROR_INIT_FAILED: return "Initialization failed";
        case [MODBUS_MODULE_NAME_UPPER]_ERROR_INVALID_PARAM: return "Invalid parameter";
        case [MODBUS_MODULE_NAME_UPPER]_ERROR_REGISTER_NOT_FOUND: return "Register not found";
        case [MODBUS_MODULE_NAME_UPPER]_ERROR_ACCESS_DENIED: return "Access denied";
        case [MODBUS_MODULE_NAME_UPPER]_ERROR_DATA_UNAVAILABLE: return "Data unavailable";
        case [MODBUS_MODULE_NAME_UPPER]_ERROR_RANGE_ERROR: return "Value out of range";
        default: return "Unknown error";
    }
}

void [modbusModuleName]PrintDiagnostics() {
    if (!s_initialized) {
        DEBUG_PRINTF("📊 [MODBUS_MODULE_NAME] Diagnostics: MODULE NOT INITIALIZED\n");
        return;
    }
    
    const [ModbusModuleName]Statistics* stats = &g[ModbusModuleName]Data.stats;
    
    DEBUG_PRINTF("📊 [MODBUS_MODULE_NAME] Diagnostics:\n");
    DEBUG_PRINTF("   State: %d\n", g[ModbusModuleName]Data.state);
    DEBUG_PRINTF("   Base Address: %d\n", g[ModbusModuleName]Data.config.baseAddress);
    DEBUG_PRINTF("   Register Reads: %lu\n", stats->registerReads);
    DEBUG_PRINTF("   Register Writes: %lu\n", stats->registerWrites);
    DEBUG_PRINTF("   Error Count: %lu\n", stats->errorCount);
    DEBUG_PRINTF("   Last Error: %s\n", [modbusModuleName]ErrorToString(stats->lastError));
    DEBUG_PRINTF("   Uptime: %lu ms\n", stats->uptime);
    DEBUG_PRINTF("   Last Update: %lu ms ago\n", millis() - s_lastUpdateTime);
}

// === PRIVATE FUNCTIONS ===

static [ModbusModuleName]Error_t findRegisterDef(uint16_t address, [ModbusModuleName]RegisterType_t type, const [ModbusModuleName]RegisterDef** regDef) {
    const [ModbusModuleName]RegisterDef* table;
    size_t tableSize;
    
    if (type == [MODBUS_MODULE_NAME_UPPER]_REGISTER_HOLDING) {
        table = holdingRegisters;
        tableSize = sizeof(holdingRegisters) / sizeof(holdingRegisters[0]);
    } else {
        table = inputRegisters;
        tableSize = sizeof(inputRegisters) / sizeof(inputRegisters[0]);
    }
    
    for (size_t i = 0; i < tableSize; i++) {
        if (table[i].address == address) {
            *regDef = &table[i];
            return [MODBUS_MODULE_NAME_UPPER]_SUCCESS;
        }
    }
    
    return [MODBUS_MODULE_NAME_UPPER]_ERROR_REGISTER_NOT_FOUND;
}

static [ModbusModuleName]Error_t validateRegisterAccess(const [ModbusModuleName]RegisterDef* regDef, [ModbusModuleName]Access_t requestedAccess) {
    if (requestedAccess == [MODBUS_MODULE_NAME_UPPER]_ACCESS_READ_ONLY) {
        // Read access is always allowed if register exists
        return [MODBUS_MODULE_NAME_UPPER]_SUCCESS;
    }
    
    if (requestedAccess == [MODBUS_MODULE_NAME_UPPER]_ACCESS_READ_WRITE && regDef->access == [MODBUS_MODULE_NAME_UPPER]_ACCESS_READ_ONLY) {
        return [MODBUS_MODULE_NAME_UPPER]_ERROR_ACCESS_DENIED;
    }
    
    return [MODBUS_MODULE_NAME_UPPER]_SUCCESS;
}

static [ModbusModuleName]Error_t readBMSData(uint8_t nodeId, [ModbusModuleName]DataType_t dataType, float* value) {
    // This function should interface with your BMS data structures
    // Example implementation:
    
    if (nodeId < 1 || nodeId > 16) {
        return [MODBUS_MODULE_NAME_UPPER]_ERROR_INVALID_PARAM;
    }
    
    // Get BMS data for the specified node
    // This is where you would integrate with your existing BMS data structures
    // Example:
    /*
    const BMSData* bmsData = getBMSData(nodeId);
    if (!bmsData || !bmsData->dataValid) {
        return [MODBUS_MODULE_NAME_UPPER]_ERROR_DATA_UNAVAILABLE;
    }
    
    switch (dataType) {
        case [MODBUS_MODULE_NAME_UPPER]_DATA_VOLTAGE:
            *value = bmsData->voltage;
            break;
        case [MODBUS_MODULE_NAME_UPPER]_DATA_CURRENT:
            *value = bmsData->current;
            break;
        case [MODBUS_MODULE_NAME_UPPER]_DATA_SOC:
            *value = bmsData->soc;
            break;
        case [MODBUS_MODULE_NAME_UPPER]_DATA_SOH:
            *value = bmsData->soh;
            break;
        case [MODBUS_MODULE_NAME_UPPER]_DATA_TEMPERATURE:
            *value = bmsData->temperature;
            break;
        case [MODBUS_MODULE_NAME_UPPER]_DATA_STATUS:
            *value = bmsData->status;
            break;
        default:
            return [MODBUS_MODULE_NAME_UPPER]_ERROR_INVALID_PARAM;
    }
    */
    
    // Placeholder implementation
    *value = 0.0f;
    
    return [MODBUS_MODULE_NAME_UPPER]_SUCCESS;
}

static [ModbusModuleName]Error_t writeBMSData(uint8_t nodeId, [ModbusModuleName]DataType_t dataType, float value) {
    // This function should interface with your BMS control structures
    // Example implementation for writable parameters
    
    if (nodeId < 1 || nodeId > 16) {
        return [MODBUS_MODULE_NAME_UPPER]_ERROR_INVALID_PARAM;
    }
    
    // Write BMS data for the specified node (only for writable parameters)
    // Example:
    /*
    switch (dataType) {
        case [MODBUS_MODULE_NAME_UPPER]_DATA_VOLTAGE:
            // Voltage setpoint for charging/discharging
            setBMSVoltageSetpoint(nodeId, value);
            break;
        case [MODBUS_MODULE_NAME_UPPER]_DATA_CURRENT:
            // Current limit setpoint
            setBMSCurrentLimit(nodeId, value);
            break;
        default:
            return [MODBUS_MODULE_NAME_UPPER]_ERROR_ACCESS_DENIED; // Read-only parameter
    }
    */
    
    // Placeholder implementation
    return [MODBUS_MODULE_NAME_UPPER]_SUCCESS;
}

static [ModbusModuleName]Error_t readSystemData([ModbusModuleName]DataType_t dataType, float* value) {
    switch (dataType) {
        case [MODBUS_MODULE_NAME_UPPER]_DATA_CAN_SPEED:
            *value = (float)systemConfig.canSpeed;
            break;
        case [MODBUS_MODULE_NAME_UPPER]_DATA_BMS_COUNT:
            *value = (float)systemConfig.activeBmsNodes;
            break;
        case [MODBUS_MODULE_NAME_UPPER]_DATA_SYSTEM_STATE:
            *value = (float)systemState;
            break;
        case [MODBUS_MODULE_NAME_UPPER]_DATA_ERROR_COUNT:
            *value = (float)g[ModbusModuleName]Data.stats.errorCount;
            break;
        case [MODBUS_MODULE_NAME_UPPER]_DATA_UPTIME_LOW:
            *value = (float)(g[ModbusModuleName]Data.stats.uptime & 0xFFFF);
            break;
        case [MODBUS_MODULE_NAME_UPPER]_DATA_UPTIME_HIGH:
            *value = (float)((g[ModbusModuleName]Data.stats.uptime >> 16) & 0xFFFF);
            break;
        case [MODBUS_MODULE_NAME_UPPER]_DATA_FREE_MEMORY:
            *value = (float)ESP.getFreeHeap();
            break;
        case [MODBUS_MODULE_NAME_UPPER]_DATA_CAN_ERRORS:
            // Get CAN error count from CAN module
            *value = 0.0f; // Placeholder
            break;
        default:
            return [MODBUS_MODULE_NAME_UPPER]_ERROR_INVALID_PARAM;
    }
    
    return [MODBUS_MODULE_NAME_UPPER]_SUCCESS;
}

static [ModbusModuleName]Error_t writeSystemData([ModbusModuleName]DataType_t dataType, float value) {
    switch (dataType) {
        case [MODBUS_MODULE_NAME_UPPER]_DATA_CAN_SPEED:
            if (value != CAN_125KBPS && value != CAN_500KBPS) {
                return [MODBUS_MODULE_NAME_UPPER]_ERROR_RANGE_ERROR;
            }
            systemConfig.canSpeed = (uint8_t)value;
            saveConfiguration();
            break;
        case [MODBUS_MODULE_NAME_UPPER]_DATA_BMS_COUNT:
            if (value < 1 || value > 16) {
                return [MODBUS_MODULE_NAME_UPPER]_ERROR_RANGE_ERROR;
            }
            systemConfig.activeBmsNodes = (int)value;
            saveConfiguration();
            break;
        default:
            return [MODBUS_MODULE_NAME_UPPER]_ERROR_ACCESS_DENIED; // Read-only parameter
    }
    
    return [MODBUS_MODULE_NAME_UPPER]_SUCCESS;
}

static uint16_t floatToRegister(float value, float scale) {
    return (uint16_t)(value * scale);
}

static float registerToFloat(uint16_t regValue, float scale) {
    return (float)regValue / scale;
}

static void updateRegisterCache() {
    // Update cache with fresh data for frequently accessed registers
    // This is an optimization to reduce data source access
    
    uint32_t currentTime = millis();
    
    // Update system registers
    s_registerCache[9000 - g[ModbusModuleName]Data.config.baseAddress] = (uint16_t)systemConfig.canSpeed;
    s_registerCache[9001 - g[ModbusModuleName]Data.config.baseAddress] = (uint16_t)systemConfig.activeBmsNodes;
    
    // Mark timestamp
    s_registerTimestamps[9000 - g[ModbusModuleName]Data.config.baseAddress] = currentTime;
    s_registerTimestamps[9001 - g[ModbusModuleName]Data.config.baseAddress] = currentTime;
}

static void logRegisterAccess(uint16_t address, bool isWrite, uint16_t value) {
#ifdef DEBUG_MODBUS_REQUESTS
    DEBUG_PRINTF("📋 Modbus %s: addr=%d, value=%d\n", 
                isWrite ? "WRITE" : "READ", address, value);
#endif
}

// === TEMPLATE USAGE INSTRUCTIONS ===
// 
// To use this Modbus register template:
// 1. Replace [MODBUS_MODULE_NAME] with your module name (e.g., "bms_modbus")
// 2. Replace [MODBUS_MODULE_NAME_UPPER] with uppercase version (e.g., "BMS_MODBUS")
// 3. Replace [ModbusModuleName] with PascalCase version (e.g., "BmsModbus")
// 4. Replace [modbusModuleName] with camelCase version (e.g., "bmsModbus")
// 5. Replace [DATE] with current date in DD.MM.YYYY format
// 6. Define your register mapping in the holdingRegisters and inputRegisters tables
// 7. Implement readBMSData() and writeBMSData() with your data source integration
// 8. Customize data types and validation logic
// 9. Remove this instruction section
//
// Key customization areas:
// - Register mapping tables (holdingRegisters, inputRegisters)
// - Data source integration (readBMSData, writeBMSData, readSystemData, writeSystemData)
// - Validation and range checking logic
// - Scaling factors and data conversion
// - Error handling and logging
//
// =====================================================================