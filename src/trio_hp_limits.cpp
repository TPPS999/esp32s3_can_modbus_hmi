// =====================================================================
// === trio_hp_limits.cpp - ESP32S3 CAN to Modbus TCP Bridge ===
// =====================================================================
// 
// 📋 PROJECT INFO:
//    Repository: https://github.com/user/esp32s3-can-modbus-tcp
//    Project: ESP32S3 CAN to Modbus TCP Bridge
//    Branch: main
//    Created: 29.08.2025 (Warsaw Time)
//
// 📋 MODULE INFO:
//    Module: TRIO HP Safety Limits and Digital Inputs Integration
//    Version: v1.0.0
//    Created: 29.08.2025 (Warsaw Time)
//    Last Modified: 29.08.2025 (Warsaw Time)
//    Author: ESP32 Development Team
//
// 📊 VERSION HISTORY:
//    v1.0.0 - 29.08.2025 - Initial TRIO HP Phase 3 safety limits implementation
//
// 🎯 DEPENDENCIES:
//    Internal: trio_hp_limits.h for structure definitions
//    Internal: bms_data.h for BMSData structure and bmsModules array
//    Internal: config.h for system constants
//    External: Arduino.h for millis() and Serial functions
//
// 📝 DESCRIPTION:
//    Implementation of TRIO HP Phase 3 safety limits and digital inputs system.
//    Provides real-time integration with BMS DCCL/DDCL limits, configurable safety
//    thresholds, E-STOP and AC contactor monitoring, and comprehensive validation
//    for all power and current commands. Uses existing BMSData structure for
//    seamless integration with the BMS communication system.
//
// 🔧 IMPLEMENTATION DETAILS:
//    - BMS Limits: Read from bmsModules[].dccl/ddcl with safety thresholds
//    - Digital Inputs: Parse bmsModules[].inputs bit fields (bits 1,2)
//    - Validation: Current and power validation with dynamic limit calculation
//    - Safety: E-STOP override and AC contactor verification
//
// ⚠️  SAFETY CRITICAL:
//    All functions in this module are safety-critical for TRIO HP operation.
//    Validation functions MUST be called before any power command execution.
//    E-STOP active state immediately invalidates all operations.
//
// =====================================================================

#include "trio_hp_limits.h"

// === GLOBAL VARIABLES ===
static TrioHPLimits_t trioHPLimits;
static TrioHPDigitalInputs_t trioHPInputs;

// === INITIALIZATION FUNCTIONS ===

bool initTrioHPLimits() {
    // Initialize limits structure with safe defaults
    trioHPLimits.dccl_bms = 0.0f;
    trioHPLimits.ddcl_bms = 0.0f;
    trioHPLimits.dccl_threshold = TRIO_HP_DEFAULT_THRESHOLD;
    trioHPLimits.ddcl_threshold = TRIO_HP_DEFAULT_THRESHOLD;
    trioHPLimits.limits_valid = false;
    trioHPLimits.last_update = 0;
    
    // Initialize inputs structure with safe defaults
    trioHPInputs.estop_active = true;      // Default to E-STOP active (safe state)
    trioHPInputs.ac_contactor = false;     // Default to contactor open (safe state)
    trioHPInputs.inputs_valid = false;
    trioHPInputs.last_update = 0;
    
    Serial.println("[TRIO HP LIMITS] Initialized with safe defaults");
    Serial.printf("[TRIO HP LIMITS] DCCL/DDCL thresholds: %.1f%%/%.1f%%\n", 
                  trioHPLimits.dccl_threshold * 100.0f, 
                  trioHPLimits.ddcl_threshold * 100.0f);
    
    return true;
}

// === BMS LIMITS FUNCTIONS ===

bool updateBMSLimits(uint8_t bmsNodeId) {
    // Get BMS data using existing function
    BMSData* bmsData = getBMSData(bmsNodeId);
    if (!bmsData) {
        Serial.printf("[TRIO HP LIMITS] ERROR: Invalid BMS node ID: %d\n", bmsNodeId);
        return false;
    }
    
    // Check if BMS data is recent and valid
    if (!isBMSDataRecent(bmsNodeId, TRIO_HP_LIMITS_TIMEOUT)) {
        Serial.printf("[TRIO HP LIMITS] WARNING: BMS node %d data is stale\n", bmsNodeId);
        trioHPLimits.limits_valid = false;
        return false;
    }
    
    // Update limits from BMS data
    trioHPLimits.dccl_bms = bmsData->dccl;  // Discharge Current Continuous Limit
    trioHPLimits.ddcl_bms = bmsData->ddcl;  // Dynamic Charge Current Limit
    trioHPLimits.last_update = millis();
    trioHPLimits.limits_valid = true;
    
    Serial.printf("[TRIO HP LIMITS] Updated from BMS %d: DCCL=%.1fA, DDCL=%.1fA\n", 
                  bmsNodeId, trioHPLimits.dccl_bms, trioHPLimits.ddcl_bms);
    
    return true;
}

bool updateAllBMSLimits() {
    float max_dccl = 0.0f;
    float max_ddcl = 0.0f;
    bool any_valid = false;
    int valid_nodes = 0;
    
    // Search through all BMS nodes for highest limits
    for (int i = 0; i < MAX_BMS_NODES; i++) {
        if (!isBMSNodeActive(i)) continue;
        
        BMSData* bmsData = getBMSData(i);
        if (!bmsData || !isBMSDataRecent(i, TRIO_HP_LIMITS_TIMEOUT)) continue;
        
        // Use highest limits from all active BMS nodes
        if (bmsData->dccl > max_dccl) max_dccl = bmsData->dccl;
        if (bmsData->ddcl > max_ddcl) max_ddcl = bmsData->ddcl;
        
        valid_nodes++;
        any_valid = true;
    }
    
    if (any_valid) {
        trioHPLimits.dccl_bms = max_dccl;
        trioHPLimits.ddcl_bms = max_ddcl;
        trioHPLimits.last_update = millis();
        trioHPLimits.limits_valid = true;
        
        Serial.printf("[TRIO HP LIMITS] Updated from %d BMS nodes: DCCL=%.1fA, DDCL=%.1fA\n", 
                      valid_nodes, max_dccl, max_ddcl);
    } else {
        trioHPLimits.limits_valid = false;
        Serial.println("[TRIO HP LIMITS] WARNING: No valid BMS limits available");
    }
    
    return any_valid;
}

float getEffectiveCurrentLimit(bool charging) {
    if (!trioHPLimits.limits_valid) {
        Serial.println("[TRIO HP LIMITS] ERROR: BMS limits not valid");
        return 0.0f;
    }
    
    if (charging) {
        // Charging: apply threshold to DDCL
        return trioHPLimits.ddcl_bms * trioHPLimits.ddcl_threshold;
    } else {
        // Discharging: apply threshold to DCCL  
        return trioHPLimits.dccl_bms * trioHPLimits.dccl_threshold;
    }
}

bool validateRequestedCurrent(float current) {
    // Check if limits are valid
    if (!areBMSLimitsValid()) {
        Serial.println("[TRIO HP LIMITS] ERROR: Cannot validate - BMS limits invalid");
        return false;
    }
    
    // Get effective limits based on current direction
    float effective_limit;
    if (current >= 0) {
        // Positive current = charging
        effective_limit = getEffectiveCurrentLimit(true);
        if (current > effective_limit) {
            Serial.printf("[TRIO HP LIMITS] ERROR: Charge current %.1fA exceeds limit %.1fA\n", 
                          current, effective_limit);
            return false;
        }
    } else {
        // Negative current = discharging
        effective_limit = getEffectiveCurrentLimit(false);
        if (-current > effective_limit) {
            Serial.printf("[TRIO HP LIMITS] ERROR: Discharge current %.1fA exceeds limit %.1fA\n", 
                          -current, effective_limit);
            return false;
        }
    }
    
    return true;
}

bool validateRequestedPower(float power) {
    // Calculate equivalent current using first active BMS voltage
    float battery_voltage = 0.0f;
    
    // Find first active BMS for voltage reference
    for (int i = 0; i < MAX_BMS_NODES; i++) {
        if (isBMSNodeActive(i) && isBMSDataRecent(i, TRIO_HP_LIMITS_TIMEOUT)) {
            BMSData* bmsData = getBMSData(i);
            if (bmsData && bmsData->batteryVoltage > 0) {
                battery_voltage = bmsData->batteryVoltage;
                break;
            }
        }
    }
    
    if (battery_voltage <= 0) {
        Serial.println("[TRIO HP LIMITS] ERROR: No valid battery voltage for power validation");
        return false;
    }
    
    // Calculate equivalent current: I = P / V
    float equivalent_current = power / battery_voltage;
    
    Serial.printf("[TRIO HP LIMITS] Power validation: %.1fW @ %.1fV = %.1fA\n", 
                  power, battery_voltage, equivalent_current);
    
    // Use current validation logic
    return validateRequestedCurrent(equivalent_current);
}

// === DIGITAL INPUTS FUNCTIONS ===

bool updateDigitalInputs() {
    bool any_valid = false;
    bool estop_detected = false;
    bool ac_contactor_closed = false;
    
    // Check all active BMS nodes for digital inputs
    for (int i = 0; i < MAX_BMS_NODES; i++) {
        if (!isBMSNodeActive(i)) continue;
        
        BMSData* bmsData = getBMSData(i);
        if (!bmsData || !isBMSDataRecent(i, TRIO_HP_INPUTS_TIMEOUT)) continue;
        
        // Parse digital inputs from BMS inputs byte
        uint8_t inputs = bmsData->inputs;
        
        // Input 9 (AC Contactor) - bit 1 (0x02)
        bool ac_bit = (inputs & TRIO_HP_INPUT_9_BIT) != 0;
        ac_contactor_closed |= ac_bit;
        
        // Input 10 (E-STOP) - bit 2 (0x04) 
        bool estop_bit = (inputs & TRIO_HP_INPUT_10_BIT) != 0;
        estop_detected |= estop_bit;
        
        any_valid = true;
        
        Serial.printf("[TRIO HP LIMITS] BMS %d inputs: 0x%02X (AC:%d, ESTOP:%d)\n", 
                      i, inputs, ac_bit ? 1 : 0, estop_bit ? 1 : 0);
    }
    
    if (any_valid) {
        trioHPInputs.ac_contactor = ac_contactor_closed;
        trioHPInputs.estop_active = estop_detected;
        trioHPInputs.inputs_valid = true;
        trioHPInputs.last_update = millis();
        
        Serial.printf("[TRIO HP LIMITS] Digital inputs updated: AC=%d, E-STOP=%d\n", 
                      ac_contactor_closed ? 1 : 0, estop_detected ? 1 : 0);
    } else {
        trioHPInputs.inputs_valid = false;
        Serial.println("[TRIO HP LIMITS] WARNING: No valid digital inputs available");
    }
    
    return any_valid;
}

bool isEstopActive() {
    if (!areDigitalInputsValid()) {
        Serial.println("[TRIO HP LIMITS] WARNING: E-STOP status unknown - assuming ACTIVE");
        return true;  // Default to safe state
    }
    
    return trioHPInputs.estop_active;
}

bool isACContactorClosed() {
    if (!areDigitalInputsValid()) {
        Serial.println("[TRIO HP LIMITS] WARNING: AC contactor status unknown - assuming OPEN");
        return false;  // Default to safe state
    }
    
    return trioHPInputs.ac_contactor;
}

bool areInputsSafeForOperation() {
    // Update inputs first
    if (!updateDigitalInputs()) {
        Serial.println("[TRIO HP LIMITS] ERROR: Cannot update digital inputs");
        return false;
    }
    
    // Check E-STOP (ACTIVE = unsafe)
    if (isEstopActive()) {
        Serial.println("[TRIO HP LIMITS] SAFETY: E-STOP ACTIVE - operation not allowed");
        return false;
    }
    
    // Check AC contactor (CLOSED = safe)
    if (!isACContactorClosed()) {
        Serial.println("[TRIO HP LIMITS] SAFETY: AC contactor OPEN - operation not allowed");
        return false;
    }
    
    // Check BMS readiness
    bool any_ready = false;
    for (int i = 0; i < MAX_BMS_NODES; i++) {
        if (!isBMSNodeActive(i)) continue;
        
        BMSData* bmsData = getBMSData(i);
        if (!bmsData || !isBMSDataRecent(i, TRIO_HP_INPUTS_TIMEOUT)) continue;
        
        if (bmsData->readyToCharge || bmsData->readyToDischarge) {
            any_ready = true;
            break;
        }
    }
    
    if (!any_ready) {
        Serial.println("[TRIO HP LIMITS] SAFETY: No BMS ready for operation");
        return false;
    }
    
    Serial.println("[TRIO HP LIMITS] SAFETY: All inputs safe for operation");
    return true;
}

// === CONFIGURATION FUNCTIONS ===

bool setLimitsThresholds(float dccl_thresh, float ddcl_thresh) {
    // Validate thresholds
    if (dccl_thresh < TRIO_HP_MIN_THRESHOLD || dccl_thresh > TRIO_HP_MAX_THRESHOLD ||
        ddcl_thresh < TRIO_HP_MIN_THRESHOLD || ddcl_thresh > TRIO_HP_MAX_THRESHOLD) {
        Serial.printf("[TRIO HP LIMITS] ERROR: Invalid thresholds: DCCL=%.2f, DDCL=%.2f\n", 
                      dccl_thresh, ddcl_thresh);
        return false;
    }
    
    trioHPLimits.dccl_threshold = dccl_thresh;
    trioHPLimits.ddcl_threshold = ddcl_thresh;
    
    Serial.printf("[TRIO HP LIMITS] Thresholds updated: DCCL=%.1f%%, DDCL=%.1f%%\n", 
                  dccl_thresh * 100.0f, ddcl_thresh * 100.0f);
    
    return true;
}

void getLimitsThresholds(float* dccl_thresh, float* ddcl_thresh) {
    if (dccl_thresh) *dccl_thresh = trioHPLimits.dccl_threshold;
    if (ddcl_thresh) *ddcl_thresh = trioHPLimits.ddcl_threshold;
}

// === STATUS FUNCTIONS ===

bool areBMSLimitsValid() {
    if (!trioHPLimits.limits_valid) return false;
    
    // Check timeout
    unsigned long now = millis();
    if (now - trioHPLimits.last_update > TRIO_HP_LIMITS_TIMEOUT) {
        Serial.printf("[TRIO HP LIMITS] WARNING: BMS limits timeout (%lu ms)\n", 
                      now - trioHPLimits.last_update);
        trioHPLimits.limits_valid = false;
        return false;
    }
    
    return true;
}

bool areDigitalInputsValid() {
    if (!trioHPInputs.inputs_valid) return false;
    
    // Check timeout
    unsigned long now = millis();
    if (now - trioHPInputs.last_update > TRIO_HP_INPUTS_TIMEOUT) {
        Serial.printf("[TRIO HP LIMITS] WARNING: Digital inputs timeout (%lu ms)\n", 
                      now - trioHPInputs.last_update);
        trioHPInputs.inputs_valid = false;
        return false;
    }
    
    return true;
}

const TrioHPLimits_t* getCurrentBMSLimits() {
    return &trioHPLimits;
}

const TrioHPDigitalInputs_t* getCurrentDigitalInputs() {
    return &trioHPInputs;
}

// === DEBUG FUNCTIONS ===

void printTrioHPLimitsStatus() {
    Serial.println("=== TRIO HP LIMITS STATUS ===");
    Serial.printf("DCCL BMS: %.1fA (threshold: %.1f%% = %.1fA effective)\n", 
                  trioHPLimits.dccl_bms, 
                  trioHPLimits.dccl_threshold * 100.0f,
                  getEffectiveCurrentLimit(false));
    Serial.printf("DDCL BMS: %.1fA (threshold: %.1f%% = %.1fA effective)\n", 
                  trioHPLimits.ddcl_bms, 
                  trioHPLimits.ddcl_threshold * 100.0f,
                  getEffectiveCurrentLimit(true));
    Serial.printf("Valid: %s, Last update: %lu ms ago\n", 
                  trioHPLimits.limits_valid ? "YES" : "NO", 
                  millis() - trioHPLimits.last_update);
}

void printTrioHPInputsStatus() {
    Serial.println("=== TRIO HP DIGITAL INPUTS STATUS ===");
    Serial.printf("E-STOP (Input 10): %s\n", trioHPInputs.estop_active ? "ACTIVE" : "INACTIVE");
    Serial.printf("AC Contactor (Input 9): %s\n", trioHPInputs.ac_contactor ? "CLOSED" : "OPEN");
    Serial.printf("Valid: %s, Last update: %lu ms ago\n", 
                  trioHPInputs.inputs_valid ? "YES" : "NO", 
                  millis() - trioHPInputs.last_update);
    Serial.printf("Safe for operation: %s\n", areInputsSafeForOperation() ? "YES" : "NO");
}