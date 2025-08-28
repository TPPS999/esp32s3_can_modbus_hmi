// =====================================================================
// === trio_hp_protocol.cpp - TRIO HP CAN Protocol Implementation ===
// =====================================================================
// 
// 📋 PROJECT INFO:
//    Repository: https://github.com/user/esp32s3-can-modbus-tcp
//    Project: ESP32S3 CAN to Modbus TCP Bridge
//    Branch: main
//    Created: 28.08.2025 (Warsaw Time)
//
// 📋 MODULE INFO:
//    Module: TRIO HP CAN Protocol Implementation
//    Version: v1.0.0
//    Created: 28.08.2025 (Warsaw Time)
//    Last Modified: 28.08.2025 (Warsaw Time)
//    Author: ESP32 Development Team
//
// 📊 VERSION HISTORY:
//    v1.0.0 - 28.08.2025 - Initial TRIO HP protocol implementation
//
// 🎯 DEPENDENCIES:
//    Internal: trio_hp_protocol.h, config.h
//    External: Arduino.h
//
// 📝 DESCRIPTION:
//    Complete implementation of TRIO HP GCP V1.00 protocol for VDE 4105 & UL1741.
//    Provides IEEE-754 float conversion, CAN ID encoding/decoding, frame construction
//    and parsing, unit conversions, and comprehensive validation functions.
//    Optimized for ESP32S3 with minimal memory footprint and high performance.
//
// 🔧 KEY FEATURES:
//    - IEEE-754 32-bit float conversion with MSB-LSB byte order
//    - 29-bit CAN ID encoding/decoding per TRIO HP specification
//    - Command frame construction for all TRIO HP command types
//    - Response frame parsing with error handling
//    - Unit conversions for voltage, current, power, frequency
//    - Heartbeat detection and module ID extraction
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
//    - All functions use stack allocation only
//    - Bitwise operations for maximum speed
//    - Input validation prevents runtime errors
//    - Memory footprint: <100 bytes stack per function call
//
// =====================================================================

#include "trio_hp_protocol.h"
#include "config.h"

// === IEEE-754 CONVERSION FUNCTIONS ===

void trioHPFloatToIEEE754Bytes(float value, uint8_t* bytes) {
    if (bytes == nullptr) return;
    
    union {
        float f;
        uint32_t i;
    } converter;
    
    converter.f = value;
    
    // MSB-LSB byte order as per TRIO HP specification
    bytes[0] = (converter.i >> 24) & 0xFF;  // MSB
    bytes[1] = (converter.i >> 16) & 0xFF;
    bytes[2] = (converter.i >> 8) & 0xFF;
    bytes[3] = converter.i & 0xFF;          // LSB
}

float trioHPIEEE754BytesToFloat(const uint8_t* bytes) {
    if (bytes == nullptr) return 0.0f;
    
    union {
        float f;
        uint32_t i;
    } converter;
    
    // MSB-LSB byte order reconstruction
    converter.i = ((uint32_t)bytes[0] << 24) |
                  ((uint32_t)bytes[1] << 16) |
                  ((uint32_t)bytes[2] << 8) |
                  (uint32_t)bytes[3];
    
    return converter.f;
}

bool trioHPValidateIEEE754(const uint8_t* bytes) {
    if (bytes == nullptr) return false;
    
    uint32_t value = ((uint32_t)bytes[0] << 24) |
                     ((uint32_t)bytes[1] << 16) |
                     ((uint32_t)bytes[2] << 8) |
                     (uint32_t)bytes[3];
    
    // Check for NaN and infinity
    uint32_t exponent = (value >> 23) & 0xFF;
    uint32_t mantissa = value & 0x7FFFFF;
    
    // Invalid if exponent is 255 (NaN or infinity)
    return (exponent != 0xFF);
}

// === CAN ID ENCODING/DECODING FUNCTIONS ===

uint32_t trioHPEncodeCanId(uint8_t errorCode, uint8_t deviceNo, uint8_t commandNo, 
                           uint8_t targetAddr, uint8_t sourceAddr) {
    return ((uint32_t)(errorCode & TRIO_HP_ERROR_CODE_MASK) << TRIO_HP_ERROR_CODE_SHIFT) |
           ((uint32_t)(deviceNo & TRIO_HP_DEVICE_NO_MASK) << TRIO_HP_DEVICE_NO_SHIFT) |
           ((uint32_t)(commandNo & TRIO_HP_COMMAND_NO_MASK) << TRIO_HP_COMMAND_NO_SHIFT) |
           ((uint32_t)(targetAddr & TRIO_HP_TARGET_ADDR_MASK) << TRIO_HP_TARGET_ADDR_SHIFT) |
           ((uint32_t)(sourceAddr & TRIO_HP_SOURCE_ADDR_MASK) << TRIO_HP_SOURCE_ADDR_SHIFT);
}

void trioHPDecodeCanId(uint32_t canId, TrioHPCanIdComponents_t* components) {
    if (components == nullptr) return;
    
    components->errorCode = (canId >> TRIO_HP_ERROR_CODE_SHIFT) & TRIO_HP_ERROR_CODE_MASK;
    components->deviceNo = (canId >> TRIO_HP_DEVICE_NO_SHIFT) & TRIO_HP_DEVICE_NO_MASK;
    components->commandNo = (canId >> TRIO_HP_COMMAND_NO_SHIFT) & TRIO_HP_COMMAND_NO_MASK;
    components->targetAddr = (canId >> TRIO_HP_TARGET_ADDR_SHIFT) & TRIO_HP_TARGET_ADDR_MASK;
    components->sourceAddr = (canId >> TRIO_HP_SOURCE_ADDR_SHIFT) & TRIO_HP_SOURCE_ADDR_MASK;
}

bool trioHPValidateCanId(uint32_t canId) {
    // Check if CAN ID fits in 29 bits
    return (canId <= 0x1FFFFFFF);
}

// === FRAME CONSTRUCTION FUNCTIONS ===

bool trioHPBuildCommandFrame(uint8_t targetAddr, uint16_t command, uint32_t data, 
                             TrioHPCanFrame_t* frame) {
    if (frame == nullptr) return false;
    
    // Extract command and sub-command
    uint8_t mainCmd = (command >> 8) & 0xFF;
    uint8_t subCmd = command & 0xFF;
    
    // Build CAN ID
    frame->canId = trioHPEncodeCanId(TRIO_HP_ERROR_NORMAL, TRIO_HP_DEVICE_SINGLE_MODULE,
                                     mainCmd, targetAddr, TRIO_HP_ADDR_CONTROLLER);
    
    // Build data frame
    frame->length = TRIO_HP_CAN_FRAME_LENGTH;
    frame->data[0] = mainCmd;
    frame->data[1] = subCmd;
    frame->data[2] = 0x00;  // Reserved
    frame->data[3] = 0x00;  // Reserved
    
    // Add data in MSB-LSB order
    frame->data[4] = (data >> 24) & 0xFF;
    frame->data[5] = (data >> 16) & 0xFF;
    frame->data[6] = (data >> 8) & 0xFF;
    frame->data[7] = data & 0xFF;
    
    return true;
}

bool trioHPBuildControlFrame(uint8_t targetAddr, uint16_t command, uint8_t controlValue, 
                             TrioHPCanFrame_t* frame) {
    if (frame == nullptr) return false;
    
    // Extract command and sub-command
    uint8_t mainCmd = (command >> 8) & 0xFF;
    uint8_t subCmd = command & 0xFF;
    
    // Build CAN ID
    frame->canId = trioHPEncodeCanId(TRIO_HP_ERROR_NORMAL, TRIO_HP_DEVICE_SINGLE_MODULE,
                                     mainCmd, targetAddr, TRIO_HP_ADDR_CONTROLLER);
    
    // Build control frame
    frame->length = TRIO_HP_CAN_FRAME_LENGTH;
    frame->data[0] = mainCmd;
    frame->data[1] = subCmd;
    frame->data[2] = 0x00;
    frame->data[3] = 0x00;
    frame->data[4] = 0x00;
    frame->data[5] = 0x00;
    frame->data[6] = 0x00;
    frame->data[7] = controlValue;
    
    return true;
}

bool trioHPBuildFloatFrame(uint8_t targetAddr, uint16_t command, float floatValue, 
                           TrioHPCanFrame_t* frame) {
    if (frame == nullptr) return false;
    
    // Extract command and sub-command
    uint8_t mainCmd = (command >> 8) & 0xFF;
    uint8_t subCmd = command & 0xFF;
    
    // Build CAN ID
    frame->canId = trioHPEncodeCanId(TRIO_HP_ERROR_NORMAL, TRIO_HP_DEVICE_SINGLE_MODULE,
                                     mainCmd, targetAddr, TRIO_HP_ADDR_CONTROLLER);
    
    // Build float frame
    frame->length = TRIO_HP_CAN_FRAME_LENGTH;
    frame->data[0] = mainCmd;
    frame->data[1] = subCmd;
    frame->data[2] = 0x00;
    frame->data[3] = 0x00;
    
    // Convert float to IEEE-754 bytes
    trioHPFloatToIEEE754Bytes(floatValue, &frame->data[4]);
    
    return true;
}

bool trioHPBuildBroadcastFrame(uint16_t command, uint32_t data, TrioHPCanFrame_t* frame) {
    return trioHPBuildCommandFrame(TRIO_HP_ADDR_BROADCAST, command, data, frame);
}

// === FRAME PARSING FUNCTIONS ===

bool trioHPParseResponseFrame(const TrioHPCanFrame_t* frame, TrioHPCommand_t* command) {
    if (frame == nullptr || command == nullptr) return false;
    if (frame->length != TRIO_HP_CAN_FRAME_LENGTH) return false;
    
    // Extract command
    command->command = (frame->data[0] << 8) | frame->data[1];
    command->subCommand = frame->data[1];
    
    // Extract data (bytes 4-7)
    command->data = ((uint32_t)frame->data[4] << 24) |
                    ((uint32_t)frame->data[5] << 16) |
                    ((uint32_t)frame->data[6] << 8) |
                    (uint32_t)frame->data[7];
    
    command->dataType = TRIO_HP_DATA_TYPE_DWORD;
    
    return true;
}

bool trioHPParseDataFrame(const uint8_t* data, uint8_t length, uint32_t* result) {
    if (data == nullptr || result == nullptr || length < 4) return false;
    
    *result = ((uint32_t)data[0] << 24) |
              ((uint32_t)data[1] << 16) |
              ((uint32_t)data[2] << 8) |
              (uint32_t)data[3];
    
    return true;
}

bool trioHPParseFloatData(const uint8_t* data, uint8_t length, float* result) {
    if (data == nullptr || result == nullptr || length < 4) return false;
    
    if (!trioHPValidateIEEE754(data)) return false;
    
    *result = trioHPIEEE754BytesToFloat(data);
    return true;
}

bool trioHPParseStatusData(const uint8_t* data, uint8_t length, uint32_t* status) {
    if (data == nullptr || status == nullptr) return false;
    
    // Status data typically in last 3 bytes for TRIO HP
    if (length >= 3) {
        *status = ((uint32_t)data[length-3] << 16) |
                  ((uint32_t)data[length-2] << 8) |
                  (uint32_t)data[length-1];
        return true;
    }
    
    return false;
}

// === UNIT CONVERSION FUNCTIONS ===

uint32_t trioHPVoltageToMillivolts(float voltage) {
    if (voltage < 0.0f) voltage = 0.0f;
    if (voltage > 1000.0f) voltage = 1000.0f;  // Max 1000V
    return (uint32_t)(voltage * 1000.0f);
}

float trioHPMillivoltsToVoltage(uint32_t millivolts) {
    return (float)millivolts / 1000.0f;
}

uint32_t trioHPCurrentToMilliamps(float current) {
    if (current < 0.0f) current = 0.0f;
    if (current > 73.3f) current = 73.3f;  // Max 73.3A per spec
    return (uint32_t)(current * 1000.0f);
}

float trioHPMilliampsToCurrent(uint32_t milliamps) {
    return (float)milliamps / 1000.0f;
}

uint32_t trioHPPowerToMilliwatts(float power) {
    // Power can be negative (reactive)
    return (uint32_t)(power * 1000.0f);
}

float trioHPMilliwattsToPower(uint32_t milliwatts) {
    return (float)milliwatts / 1000.0f;
}

uint32_t trioHPFrequencyToMillihertz(float frequency) {
    if (frequency < 45.0f) frequency = 45.0f;    // Min 45Hz
    if (frequency > 65.0f) frequency = 65.0f;    // Max 65Hz
    return (uint32_t)(frequency * 1000.0f);
}

float trioHPMillihertzToFrequency(uint32_t millihertz) {
    return (float)millihertz / 1000.0f;
}

// === DATA VALIDATION FUNCTIONS ===

bool trioHPValidateVoltage(float voltage) {
    return (voltage >= 0.0f && voltage <= 1000.0f);
}

bool trioHPValidateCurrent(float current) {
    return (current >= 0.0f && current <= 73.3f);
}

bool trioHPValidatePower(float power) {
    return (power >= -50000.0f && power <= 50000.0f);  // ±50kW
}

bool trioHPValidateFrequency(float frequency) {
    return (frequency >= 45.0f && frequency <= 65.0f);
}

bool trioHPValidateTemperature(int8_t temperature) {
    return (temperature >= -40 && temperature <= 85);  // Industrial range
}

bool trioHPValidatePowerFactor(float powerFactor) {
    return (powerFactor >= 0.8f && powerFactor <= 1.0f);
}

// === HEARTBEAT DETECTION FUNCTIONS ===

bool trioHPIsHeartbeatFrame(uint32_t canId) {
    return ((canId & 0x0757F700) == TRIO_HP_HEARTBEAT_ID_BASE);
}

uint8_t trioHPExtractModuleIdFromHeartbeat(uint32_t canId) {
    if (!trioHPIsHeartbeatFrame(canId)) return 0xFF;
    
    // Extract module ID from last byte of heartbeat CAN ID
    return (uint8_t)(canId & 0xFF);
}

bool trioHPValidateHeartbeatFrame(uint32_t canId, const uint8_t* data, uint8_t length) {
    if (!trioHPIsHeartbeatFrame(canId)) return false;
    if (data == nullptr || length == 0) return false;
    
    // Basic heartbeat frame validation
    return (length <= 8);
}

// === COMMAND VALIDATION FUNCTIONS ===

bool trioHPValidateCommand(uint16_t command) {
    uint8_t mainCmd = (command >> 8) & 0xFF;
    return (mainCmd == 0x10 || mainCmd == 0x11 || mainCmd == 0x21 || mainCmd == 0x31);
}

bool trioHPValidateControlValue(uint8_t controlValue) {
    return (controlValue == TRIO_HP_CTRL_ENABLE_ON_MODE0 ||
            controlValue == TRIO_HP_CTRL_DISABLE_OFF_MODE1 ||
            controlValue == TRIO_HP_CTRL_SPECIAL_MODE2);
}

bool trioHPValidateTargetAddress(uint8_t address) {
    return (address <= 0x3F || address == TRIO_HP_ADDR_CONTROLLER);
}

bool trioHPIsSystemCommand(uint16_t command) {
    return ((command & 0xFF00) == 0x1000);
}

bool trioHPIsModuleCommand(uint16_t command) {
    uint8_t mainCmd = (command >> 8) & 0xFF;
    return (mainCmd == 0x11 || mainCmd == 0x21);
}

bool trioHPIsVDECommand(uint16_t command) {
    return ((command & 0xFF00) == 0x3100);
}

// === UTILITY FUNCTIONS ===

void trioHPPrintFrame(const TrioHPCanFrame_t* frame) {
    if (frame == nullptr) return;
    
    Serial.print("TRIO HP Frame - ID: 0x");
    Serial.print(frame->canId, HEX);
    Serial.print(" Data: ");
    
    for (int i = 0; i < frame->length; i++) {
        if (frame->data[i] < 0x10) Serial.print("0");
        Serial.print(frame->data[i], HEX);
        Serial.print(" ");
    }
    Serial.println();
}

void trioHPPrintCanIdComponents(const TrioHPCanIdComponents_t* components) {
    if (components == nullptr) return;
    
    Serial.println("TRIO HP CAN ID Components:");
    Serial.printf("  Error Code: 0x%02X\n", components->errorCode);
    Serial.printf("  Device No: 0x%02X\n", components->deviceNo);
    Serial.printf("  Command No: 0x%02X\n", components->commandNo);
    Serial.printf("  Target Addr: 0x%02X\n", components->targetAddr);
    Serial.printf("  Source Addr: 0x%02X\n", components->sourceAddr);
}

const char* trioHPGetCommandName(uint16_t command) {
    switch (command) {
        case TRIO_HP_CMD_MODULE_ON_OFF: return "Module ON/OFF";
        case TRIO_HP_CMD_MODULE_LED_BLINK: return "LED Blink";
        case TRIO_HP_CMD_MODULE_SLEEP: return "Sleep Mode";
        case TRIO_HP_CMD_AC_WORK_MODE: return "AC Work Mode";
        case TRIO_HP_CMD_AC_ANTI_ISLANDING: return "Anti-Islanding";
        case TRIO_HP_CMD_NATIONAL_SETTINGS: return "National Settings";
        default: return "Unknown Command";
    }
}

const char* trioHPGetControlValueName(uint8_t controlValue) {
    switch (controlValue) {
        case TRIO_HP_CTRL_ENABLE_ON_MODE0: return "ENABLE/ON/MODE0";
        case TRIO_HP_CTRL_DISABLE_OFF_MODE1: return "DISABLE/OFF/MODE1";
        case TRIO_HP_CTRL_SPECIAL_MODE2: return "SPECIAL/MODE2";
        default: return "Unknown Control";
    }
}

const char* trioHPGetErrorCodeName(uint8_t errorCode) {
    switch (errorCode) {
        case TRIO_HP_ERROR_NORMAL: return "Normal";
        case TRIO_HP_ERROR_COMMAND_INVALID: return "Invalid Command";
        case TRIO_HP_ERROR_DATA_INVALID: return "Invalid Data";
        case TRIO_HP_ERROR_START_PROCESSING: return "Start Processing";
        default: return "Unknown Error";
    }
}