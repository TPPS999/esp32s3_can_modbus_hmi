/*
 * ESP32S3 CAN to Modbus TCP Bridge - Implementacja Funkcji BMS
 * 
 * VERSION: v4.0.0 - MODULAR ARCHITECTURE
 * DATE: 2025-08-12
 * STATUS: ✅ READY FOR COMPILATION
 * 
 * OPIS: Implementacja funkcji pomocniczych dla struktur danych BMS
 */

#include "bms_data.h"
#include "config.h"

// Global BMS data arrays
BMSData bmsModules[MAX_BMS_NODES];
uint16_t holdingRegisters[MODBUS_MAX_HOLDING_REGISTERS];

// ================================
// === MULTIPLEXER UTILITIES ===
// ================================

// Define the multiplexer types array
static const MultiplexerInfo MULTIPLEXER_TYPES[] = {
    {MUX_TYPE_SERIAL_NUMBER_0, "Serial Number Low", "", 1.0f, false},
    {MUX_TYPE_SERIAL_NUMBER_1, "Serial Number High", "", 1.0f, false},
    {MUX_TYPE_HW_VERSION_0, "HW Version Low", "", 1.0f, false},
    {MUX_TYPE_HW_VERSION_1, "HW Version High", "", 1.0f, false},
    {MUX_TYPE_SW_VERSION_0, "SW Version Low", "", 1.0f, false},
    {MUX_TYPE_SW_VERSION_1, "SW Version High", "", 1.0f, false},
    {MUX_TYPE_FACTORY_ENERGY, "Factory Energy", "kWh", 0.1f, false},
    {MUX_TYPE_DESIGN_CAPACITY, "Design Capacity", "Ah", 0.1f, false},
    {MUX_TYPE_INLET_TEMPERATURE, "Inlet Temperature", "°C", 0.1f, true},
    {MUX_TYPE_OUTLET_TEMPERATURE, "Outlet Temperature", "°C", 0.1f, true},
    {MUX_TYPE_HUMIDITY, "Humidity", "%", 0.1f, false},
    {MUX_TYPE_ERROR_MAP_0, "Error Map 0", "", 1.0f, false},
    {MUX_TYPE_ERROR_MAP_1, "Error Map 1", "", 1.0f, false},
    {MUX_TYPE_ERROR_MAP_2, "Error Map 2", "", 1.0f, false},
    {MUX_TYPE_ERROR_MAP_3, "Error Map 3", "", 1.0f, false},
    {MUX_TYPE_TIME_TO_FULL_CHARGE, "Time to Full Charge", "min", 1.0f, false},
    {MUX_TYPE_TIME_TO_FULL_DISCHARGE, "Time to Full Discharge", "min", 1.0f, false},
    {MUX_TYPE_BATTERY_CYCLES, "Battery Cycles", "", 1.0f, false}
};

const MultiplexerInfo* getMultiplexerTypeInfo(uint8_t type) {
    // Przeszukaj tablicę typów multipleksera
    for (size_t i = 0; i < ARRAY_SIZE(MULTIPLEXER_TYPES); i++) {
        if (MULTIPLEXER_TYPES[i].type == (MultiplexerType)type) {
            return &MULTIPLEXER_TYPES[i];
        }
    }
    return nullptr; // Nie znaleziono
}

float convertMultiplexerValue(uint8_t type, uint16_t rawValue) {
    const MultiplexerInfo* typeInfo = getMultiplexerTypeInfo(type);
    if (typeInfo == nullptr) {
        return 0.0f; // Nieznany typ
    }
    
    float convertedValue;
    
    if (typeInfo->isSigned) {
        // Konwersja na signed int16_t
        int16_t signedValue = (int16_t)rawValue;
        convertedValue = signedValue * typeInfo->scale;
    } else {
        // Konwersja unsigned
        convertedValue = rawValue * typeInfo->scale;
    }
    
    return convertedValue;
}

// ================================
// === BMS DATA MANAGEMENT ===
// ================================

bool initializeBMSData() {
    // Initialize all BMS modules
    for (int i = 0; i < MAX_BMS_NODES; i++) {
        memset(&bmsModules[i], 0, sizeof(BMSData));
        bmsModules[i].communicationOk = false;
    }
    
    // Initialize holding registers
    for (int i = 0; i < MODBUS_MAX_HOLDING_REGISTERS; i++) {
        holdingRegisters[i] = 0;
    }
    
    return true;
}

void resetBMSData(uint8_t nodeId) {
    int index = getBMSIndexByNodeId(nodeId);
    if (index >= 0 && index < MAX_BMS_NODES) {
        memset(&bmsModules[index], 0, sizeof(BMSData));
        bmsModules[index].communicationOk = false;
    }
}

void resetAllBMSData() {
    for (int i = 0; i < MAX_BMS_NODES; i++) {
        memset(&bmsModules[i], 0, sizeof(BMSData));
        bmsModules[i].communicationOk = false;
    }
}

BMSData* getBMSData(uint8_t nodeId) {
    int index = getBMSIndexByNodeId(nodeId);
    if (index >= 0 && index < MAX_BMS_NODES) {
        return &bmsModules[index];
    }
    return nullptr;
}

int getBMSIndexByNodeId(uint8_t nodeId) {
    // Find the index in the active BMS nodes list
    extern SystemConfig systemConfig;
    for (int i = 0; i < systemConfig.activeBmsNodes && i < MAX_BMS_NODES; i++) {
        if (systemConfig.bmsNodeIds[i] == nodeId) {
            return i;
        }
    }
    return -1; // Not found
}

int getBatteryIndexFromNodeId(uint8_t nodeId) {
    return getBMSIndexByNodeId(nodeId); // Alias for compatibility
}

bool isBMSNodeActive(uint8_t nodeId) {
    return getBMSIndexByNodeId(nodeId) >= 0;
}

int getActiveBMSCount() {
    extern SystemConfig systemConfig;
    int count = 0;
    for (int i = 0; i < systemConfig.activeBmsNodes && i < MAX_BMS_NODES; i++) {
        BMSData* bms = &bmsModules[i];
        if (bms->communicationOk && (millis() - bms->lastUpdate < 30000)) {
            count++;
        }
    }
    return count;
}

// updateCommunicationStatus is defined in bms_protocol.cpp

bool isBMSCommunicationOK(uint8_t nodeId) {
    BMSData* bms = getBMSData(nodeId);
    if (!bms) return false;
    
    return bms->communicationOk && (millis() - bms->lastUpdate < 30000);
}

unsigned long getLastUpdateTime(uint8_t nodeId) {
    BMSData* bms = getBMSData(nodeId);
    return bms ? bms->lastUpdate : 0;
}

// ================================
// === MULTIPLEXER TYPE FUNCTIONS ===
// ================================

const char* getMultiplexerTypeName(uint8_t type) {
    const MultiplexerInfo* info = getMultiplexerTypeInfo(type);
    return info ? info->name : "Unknown";
}

const char* getMultiplexerTypeUnit(uint8_t type) {
    const MultiplexerInfo* info = getMultiplexerTypeInfo(type);
    return info ? info->unit : "";
}

float getMultiplexerTypeScale(uint8_t type) {
    const MultiplexerInfo* info = getMultiplexerTypeInfo(type);
    return info ? info->scale : 1.0f;
}