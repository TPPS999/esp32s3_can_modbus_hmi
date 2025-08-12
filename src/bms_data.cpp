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

// ================================
// === MULTIPLEXER UTILITIES ===
// ================================

const MultiplexerType* getMultiplexerTypeInfo(uint8_t type) {
    // Przeszukaj tablicę typów multipleksera
    for (size_t i = 0; i < ARRAY_SIZE(MULTIPLEXER_TYPES); i++) {
        if (MULTIPLEXER_TYPES[i].type == type) {
            return &MULTIPLEXER_TYPES[i];
        }
    }
    return nullptr; // Nie znaleziono
}

float convertMultiplexerValue(uint8_t type, uint16_t rawValue) {
    const MultiplexerType* typeInfo = getMultiplexerTypeInfo(type);
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