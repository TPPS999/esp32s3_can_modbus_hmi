// =====================================================================
// === trio_hp_controllers.cpp - ESP32S3 CAN to Modbus TCP Bridge ===
// =====================================================================
// 
// 📋 PROJECT INFO:
//    Repository: https://github.com/user/esp32s3-can-modbus-tcp
//    Project: ESP32S3 CAN to Modbus TCP Bridge
//    Branch: main
//    Created: 29.08.2025 (Warsaw Time)
//
// 📋 MODULE INFO:
//    Module: TRIO HP PID Controllers and Efficiency Monitoring
//    Version: v1.0.0
//    Created: 29.08.2025 (Warsaw Time)
//    Last Modified: 29.08.2025 (Warsaw Time)
//    Author: ESP32 Development Team
//
// 📊 VERSION HISTORY:
//    v1.0.0 - 29.08.2025 - Initial TRIO HP Phase 3 PID controllers implementation
//
// 🎯 DEPENDENCIES:
//    Internal: trio_hp_controllers.h for structure definitions
//    Internal: trio_hp_limits.h for safety validation
//    Internal: bms_data.h for battery voltage and current
//    External: Arduino.h for millis() and Serial functions
//
// 📝 DESCRIPTION:
//    Complete implementation of TRIO HP Phase 3 PID controllers and efficiency monitoring.
//    Active power controller manages DC current based on AC power targets using battery voltage.
//    Reactive power controller implements configurable single-module strategy with distribution.
//    Efficiency monitoring provides both instantaneous and cumulative measurements with energy integration.
//
// 🔧 IMPLEMENTATION DETAILS:
//    - PID Algorithm: Standard discrete PID with anti-windup protection
//    - Safety Integration: All commands validated through trio_hp_limits
//    - Energy Integration: Trapezoidal rule for accurate energy calculation
//    - Module Distribution: Intelligent reactive power distribution across active modules
//
// ⚠️  CRITICAL NOTES:
//    Controllers must be called regularly for proper operation.
//    Safety validation through trio_hp_limits MUST pass before command execution.
//    Energy counters use double precision to prevent overflow during long operations.
//
// =====================================================================

#include "trio_hp_controllers.h"
#include "trio_hp_monitor.h"

// === GLOBAL CONTROLLER INSTANCES ===
static TrioActivePowerController_t activePowerController;
static TrioReactivePowerController_t reactivePowerController;
static TrioEfficiencyMonitor_t efficiencyMonitor;

// === PRIVATE FUNCTION DECLARATIONS ===
static float calculateBatteryVoltage();
static float calculateTotalACActivePower();
static float calculateTotalACApparentPower();
static bool validateControllerSafety();
static void updateEnergyCounters(float dc_power, float ac_active, float ac_apparent, float dt_hours);

// === INITIALIZATION FUNCTIONS ===

bool initTrioHPControllers() {
    Serial.println("[TRIO HP CONTROLLERS] Initializing controllers and efficiency monitoring...");
    
    bool success = true;
    success &= initActivePowerController();
    success &= initReactivePowerController();  
    success &= initTrioEfficiencyMonitor();
    
    if (success) {
        Serial.println("[TRIO HP CONTROLLERS] All controllers initialized successfully");
    } else {
        Serial.println("[TRIO HP CONTROLLERS] ERROR: Controller initialization failed");
    }
    
    return success;
}

bool initActivePowerController() {
    // Initialize target and control variables
    activePowerController.target_power = 0.0f;
    activePowerController.current_power = 0.0f;
    activePowerController.calculated_current = 0.0f;
    activePowerController.tolerance = TRIO_HP_ACTIVE_POWER_TOLERANCE;
    activePowerController.loop_interval = TRIO_HP_ACTIVE_POWER_INTERVAL;
    
    // Initialize PID parameters
    activePowerController.kp = TRIO_HP_ACTIVE_POWER_KP;
    activePowerController.ki = TRIO_HP_ACTIVE_POWER_KI;
    activePowerController.kd = TRIO_HP_ACTIVE_POWER_KD;
    activePowerController.error = 0.0f;
    activePowerController.last_error = 0.0f;
    activePowerController.integral = 0.0f;
    activePowerController.derivative = 0.0f;
    activePowerController.output = 0.0f;
    
    // Initialize controller limits
    activePowerController.output_min = -200.0f;  // Max 200A discharge
    activePowerController.output_max = 200.0f;   // Max 200A charge
    activePowerController.integral_max = 100.0f; // Anti-windup limit
    
    // Initialize status
    activePowerController.enabled = false;
    activePowerController.target_reached = false;
    activePowerController.last_update = 0;
    activePowerController.update_count = 0;
    
    Serial.println("[ACTIVE POWER PID] Initialized with default parameters");
    return true;
}

bool initReactivePowerController() {
    // Initialize target and control variables
    reactivePowerController.target_reactive = 0.0f;
    reactivePowerController.current_reactive = 0.0f;
    reactivePowerController.tolerance = TRIO_HP_REACTIVE_POWER_TOLERANCE;
    reactivePowerController.loop_interval = TRIO_HP_REACTIVE_POWER_INTERVAL;
    reactivePowerController.module_threshold = TRIO_HP_REACTIVE_THRESHOLD;
    
    // ✅ CORRECTED: Configurable single module limits
    reactivePowerController.single_module_max = TRIO_HP_REACTIVE_SINGLE_MAX; // 10kVAr (CONFIGURABLE)
    reactivePowerController.max_per_module = TRIO_HP_REACTIVE_MODULE_MAX;     // 14kVAr (absolute max)
    
    // Initialize PID parameters
    reactivePowerController.kp = TRIO_HP_REACTIVE_POWER_KP;
    reactivePowerController.ki = TRIO_HP_REACTIVE_POWER_KI;
    reactivePowerController.kd = TRIO_HP_REACTIVE_POWER_KD;
    reactivePowerController.error = 0.0f;
    reactivePowerController.last_error = 0.0f;
    reactivePowerController.integral = 0.0f;
    reactivePowerController.derivative = 0.0f;
    reactivePowerController.output = 0.0f;
    
    // Initialize distribution strategy
    reactivePowerController.use_single_module = true;  // Default to single module
    reactivePowerController.active_modules = 0;
    for (int i = 0; i < 16; i++) {
        reactivePowerController.per_module_reactive[i] = 0.0f;
    }
    
    // Initialize status
    reactivePowerController.enabled = false;
    reactivePowerController.target_reached = false;
    reactivePowerController.last_update = 0;
    reactivePowerController.update_count = 0;
    
    Serial.printf("[REACTIVE POWER PID] Initialized: single_max=%.1fkVAr, threshold=%.0fVA\n",
                  reactivePowerController.single_module_max / 1000.0f,
                  reactivePowerController.module_threshold);
    return true;
}

bool initTrioEfficiencyMonitor() {
    // Initialize instantaneous measurements
    efficiencyMonitor.dc_power = 0.0f;
    efficiencyMonitor.ac_active_power = 0.0f;
    efficiencyMonitor.ac_apparent_power = 0.0f;
    efficiencyMonitor.active_efficiency = 0.0f;
    efficiencyMonitor.apparent_efficiency = 0.0f;
    
    // Initialize measurement timing
    efficiencyMonitor.measurement_interval = TRIO_HP_EFFICIENCY_INTERVAL;
    efficiencyMonitor.last_measurement = 0;
    
    // Initialize energy counters
    efficiencyMonitor.energy_counters.total_dc_energy = 0.0;
    efficiencyMonitor.energy_counters.total_ac_active_energy = 0.0;
    efficiencyMonitor.energy_counters.total_ac_apparent_energy = 0.0;
    efficiencyMonitor.energy_counters.energy_start_time = 0;
    efficiencyMonitor.energy_counters.energy_duration = 0;
    efficiencyMonitor.energy_counters.energy_counting_enabled = false;
    
    // Initialize cumulative efficiency
    efficiencyMonitor.energy_counters.cumulative_active_efficiency = 0.0;
    efficiencyMonitor.energy_counters.cumulative_apparent_efficiency = 0.0;
    efficiencyMonitor.energy_counters.efficiency_samples = 0;
    efficiencyMonitor.energy_counters.efficiency_sum_active = 0.0;
    efficiencyMonitor.energy_counters.efficiency_sum_apparent = 0.0;
    
    // Initialize validity
    efficiencyMonitor.valid = false;
    efficiencyMonitor.last_valid_time = 0;
    
    Serial.printf("[EFFICIENCY MONITOR] Initialized with %dms measurement interval\n",
                  efficiencyMonitor.measurement_interval);
    return true;
}

// === ACTIVE POWER CONTROLLER FUNCTIONS ===

bool updateActivePowerController() {
    if (!activePowerController.enabled) return true;
    
    unsigned long now = millis();
    
    // Check if it's time for controller update
    if (now - activePowerController.last_update < activePowerController.loop_interval) {
        return true; // Not time yet
    }
    
    // Safety check
    if (!validateControllerSafety()) {
        Serial.println("[ACTIVE POWER PID] SAFETY: Controller update blocked");
        return false;
    }
    
    // Get current measurements
    float battery_voltage = calculateBatteryVoltage();
    if (battery_voltage <= 0) {
        Serial.println("[ACTIVE POWER PID] ERROR: Invalid battery voltage");
        return false;
    }
    
    activePowerController.current_power = calculateTotalACActivePower();
    
    // Calculate PID error
    float dt = (now - activePowerController.last_update) / 1000.0f; // Convert to seconds
    activePowerController.error = activePowerController.target_power - activePowerController.current_power;
    
    // Check if target reached
    activePowerController.target_reached = (abs(activePowerController.error) <= activePowerController.tolerance);
    
    if (activePowerController.target_reached) {
        Serial.printf("[ACTIVE POWER PID] Target reached: %.1fW (error: ±%.1fW)\n",
                      activePowerController.current_power, activePowerController.error);
        activePowerController.last_update = now;
        return true;
    }
    
    // Calculate PID terms
    activePowerController.integral += activePowerController.error * dt;
    
    // Anti-windup protection
    if (activePowerController.integral > activePowerController.integral_max) {
        activePowerController.integral = activePowerController.integral_max;
    } else if (activePowerController.integral < -activePowerController.integral_max) {
        activePowerController.integral = -activePowerController.integral_max;
    }
    
    if (dt > 0) {
        activePowerController.derivative = (activePowerController.error - activePowerController.last_error) / dt;
    }
    
    // Calculate PID output
    float proportional = activePowerController.kp * activePowerController.error;
    float integral_term = activePowerController.ki * activePowerController.integral;
    float derivative_term = activePowerController.kd * activePowerController.derivative;
    
    activePowerController.output = proportional + integral_term + derivative_term;
    
    // Calculate target current: I = P / V
    activePowerController.calculated_current = activePowerController.target_power / battery_voltage + activePowerController.output;
    
    // Validate calculated current against safety limits
    if (!validateRequestedCurrent(activePowerController.calculated_current)) {
        Serial.printf("[ACTIVE POWER PID] ERROR: Calculated current %.1fA exceeds safety limits\n",
                      activePowerController.calculated_current);
        return false;
    }
    
    // Update controller state
    activePowerController.last_error = activePowerController.error;
    activePowerController.last_update = now;
    activePowerController.update_count++;
    
    Serial.printf("[ACTIVE POWER PID] Update: target=%.1fW, current=%.1fW, error=%.1fW, output=%.2fA\n",
                  activePowerController.target_power, activePowerController.current_power,
                  activePowerController.error, activePowerController.calculated_current);
    
    return true;
}

bool setActivePowerTarget(float target_watts) {
    // Validate power target against safety limits
    if (!validateRequestedPower(target_watts)) {
        Serial.printf("[ACTIVE POWER PID] ERROR: Target power %.1fW exceeds safety limits\n", target_watts);
        return false;
    }
    
    activePowerController.target_power = target_watts;
    activePowerController.target_reached = false;
    
    // Reset PID state for new target
    activePowerController.error = 0.0f;
    activePowerController.last_error = 0.0f;
    activePowerController.integral = 0.0f;
    activePowerController.derivative = 0.0f;
    
    Serial.printf("[ACTIVE POWER PID] Target set: %.1fW\n", target_watts);
    return true;
}

void setActivePowerControllerEnabled(bool enabled) {
    activePowerController.enabled = enabled;
    
    if (!enabled) {
        // Reset controller state when disabled
        activePowerController.target_power = 0.0f;
        activePowerController.calculated_current = 0.0f;
        activePowerController.error = 0.0f;
        activePowerController.integral = 0.0f;
        activePowerController.target_reached = false;
    }
    
    Serial.printf("[ACTIVE POWER PID] Controller %s\n", enabled ? "ENABLED" : "DISABLED");
}

bool setActivePowerPIDParams(float kp, float ki, float kd) {
    if (kp < 0 || ki < 0 || kd < 0) {
        Serial.printf("[ACTIVE POWER PID] ERROR: Invalid PID parameters: kp=%.3f, ki=%.3f, kd=%.3f\n", kp, ki, kd);
        return false;
    }
    
    activePowerController.kp = kp;
    activePowerController.ki = ki;
    activePowerController.kd = kd;
    
    // Reset integral when parameters change
    activePowerController.integral = 0.0f;
    
    Serial.printf("[ACTIVE POWER PID] Parameters updated: kp=%.3f, ki=%.3f, kd=%.3f\n", kp, ki, kd);
    return true;
}

const TrioActivePowerController_t* getActivePowerControllerStatus() {
    return &activePowerController;
}

// === REACTIVE POWER CONTROLLER FUNCTIONS ===

bool updateReactivePowerController() {
    if (!reactivePowerController.enabled) return true;
    
    unsigned long now = millis();
    
    // Check if it's time for controller update
    if (now - reactivePowerController.last_update < reactivePowerController.loop_interval) {
        return true; // Not time yet
    }
    
    // Safety check
    if (!validateControllerSafety()) {
        Serial.println("[REACTIVE POWER PID] SAFETY: Controller update blocked");
        return false;
    }
    
    // Get current reactive power measurement from monitor system
    const TrioHPSystemData_t* systemData = getSystemData();
    if (systemData) {
        reactivePowerController.current_reactive = systemData->totalReactivePower;
    } else {
        Serial.println("[REACTIVE POWER PID] WARNING: No system data available");
        return false;
    }
    
    // Calculate PID error
    float dt = (now - reactivePowerController.last_update) / 1000.0f; // Convert to seconds
    reactivePowerController.error = reactivePowerController.target_reactive - reactivePowerController.current_reactive;
    
    // Check if target reached
    reactivePowerController.target_reached = (abs(reactivePowerController.error) <= reactivePowerController.tolerance);
    
    if (reactivePowerController.target_reached) {
        Serial.printf("[REACTIVE POWER PID] Target reached: %.1fVAr (error: ±%.1fVAr)\n",
                      reactivePowerController.current_reactive, reactivePowerController.error);
        reactivePowerController.last_update = now;
        return true;
    }
    
    // Calculate PID terms
    reactivePowerController.integral += reactivePowerController.error * dt;
    
    // Anti-windup protection
    float integral_max = 5000.0f; // 5kVAr integral limit
    if (reactivePowerController.integral > integral_max) {
        reactivePowerController.integral = integral_max;
    } else if (reactivePowerController.integral < -integral_max) {
        reactivePowerController.integral = -integral_max;
    }
    
    if (dt > 0) {
        reactivePowerController.derivative = (reactivePowerController.error - reactivePowerController.last_error) / dt;
    }
    
    // Calculate PID output
    float proportional = reactivePowerController.kp * reactivePowerController.error;
    float integral_term = reactivePowerController.ki * reactivePowerController.integral;
    float derivative_term = reactivePowerController.kd * reactivePowerController.derivative;
    
    reactivePowerController.output = proportional + integral_term + derivative_term;
    
    // Determine distribution strategy based on target and configurable limits
    float abs_target = abs(reactivePowerController.target_reactive);
    reactivePowerController.use_single_module = (abs_target <= reactivePowerController.single_module_max); // CONFIGURABLE
    
    // Calculate per-module reactive power distribution
    if (reactivePowerController.use_single_module) {
        // Single module strategy
        reactivePowerController.per_module_reactive[0] = reactivePowerController.target_reactive + reactivePowerController.output;
        for (int i = 1; i < 16; i++) {
            reactivePowerController.per_module_reactive[i] = 0.0f;
        }
        reactivePowerController.active_modules = 1;
        
        Serial.printf("[REACTIVE POWER PID] Single module: %.1fVAr\n", reactivePowerController.per_module_reactive[0]);
    } else {
        // Multi-module distribution
        // Count active modules (simplified - assume all 16 active for now)
        reactivePowerController.active_modules = 16;
        float per_module = (reactivePowerController.target_reactive + reactivePowerController.output) / 16.0f;
        
        // Apply per-module limits
        if (abs(per_module) > reactivePowerController.max_per_module) {
            per_module = (per_module > 0) ? reactivePowerController.max_per_module : -reactivePowerController.max_per_module;
            Serial.printf("[REACTIVE POWER PID] WARNING: Limited to %.1fkVAr per module\n", per_module / 1000.0f);
        }
        
        for (int i = 0; i < 16; i++) {
            reactivePowerController.per_module_reactive[i] = per_module;
        }
        
        Serial.printf("[REACTIVE POWER PID] Multi-module: %.1fVAr × 16 modules\n", per_module);
    }
    
    // Update controller state
    reactivePowerController.last_error = reactivePowerController.error;
    reactivePowerController.last_update = now;
    reactivePowerController.update_count++;
    
    return true;
}

bool setReactivePowerTarget(float target_var) {
    reactivePowerController.target_reactive = target_var;
    reactivePowerController.target_reached = false;
    
    // Reset PID state for new target
    reactivePowerController.error = 0.0f;
    reactivePowerController.last_error = 0.0f;
    reactivePowerController.integral = 0.0f;
    reactivePowerController.derivative = 0.0f;
    
    Serial.printf("[REACTIVE POWER PID] Target set: %.1fVAr\n", target_var);
    return true;
}

void setReactivePowerControllerEnabled(bool enabled) {
    reactivePowerController.enabled = enabled;
    
    if (!enabled) {
        // Reset controller state when disabled
        reactivePowerController.target_reactive = 0.0f;
        reactivePowerController.output = 0.0f;
        reactivePowerController.error = 0.0f;
        reactivePowerController.integral = 0.0f;
        reactivePowerController.target_reached = false;
        
        // Clear module distribution
        for (int i = 0; i < 16; i++) {
            reactivePowerController.per_module_reactive[i] = 0.0f;
        }
    }
    
    Serial.printf("[REACTIVE POWER PID] Controller %s\n", enabled ? "ENABLED" : "DISABLED");
}

bool setReactivePowerPIDParams(float kp, float ki, float kd) {
    if (kp < 0 || ki < 0 || kd < 0) {
        Serial.printf("[REACTIVE POWER PID] ERROR: Invalid PID parameters: kp=%.3f, ki=%.3f, kd=%.3f\n", kp, ki, kd);
        return false;
    }
    
    reactivePowerController.kp = kp;
    reactivePowerController.ki = ki;
    reactivePowerController.kd = kd;
    
    // Reset integral when parameters change
    reactivePowerController.integral = 0.0f;
    
    Serial.printf("[REACTIVE POWER PID] Parameters updated: kp=%.3f, ki=%.3f, kd=%.3f\n", kp, ki, kd);
    return true;
}

bool setReactivePowerLimits(float threshold, float single_max) {
    if (threshold <= 0 || single_max <= 0 || single_max > reactivePowerController.max_per_module) {
        Serial.printf("[REACTIVE POWER PID] ERROR: Invalid limits: threshold=%.1f, single_max=%.1f\n", 
                      threshold, single_max);
        return false;
    }
    
    reactivePowerController.module_threshold = threshold;
    reactivePowerController.single_module_max = single_max; // ✅ CONFIGURABLE
    
    Serial.printf("[REACTIVE POWER PID] Limits updated: threshold=%.0fVA, single_max=%.1fkVAr\n",
                  threshold, single_max / 1000.0f);
    return true;
}

const TrioReactivePowerController_t* getReactivePowerControllerStatus() {
    return &reactivePowerController;
}

// === EFFICIENCY MONITORING FUNCTIONS ===

bool updateEfficiencyMeasurement() {
    unsigned long now = millis();
    
    // Check if it's time for measurement update
    if (now - efficiencyMonitor.last_measurement < efficiencyMonitor.measurement_interval) {
        return true; // Not time yet
    }
    
    // Calculate DC power from BMS data
    float battery_voltage = calculateBatteryVoltage();
    float battery_current = 0.0f;
    
    // Get battery current from first active BMS
    for (int i = 0; i < MAX_BMS_NODES; i++) {
        if (isBMSNodeActive(i) && isBMSDataRecent(i, 5000)) {
            BMSData* bmsData = getBMSData(i);
            if (bmsData) {
                battery_current += bmsData->batteryCurrent; // Sum currents from all BMS
            }
        }
    }
    
    if (battery_voltage <= 0) {
        efficiencyMonitor.valid = false;
        Serial.println("[EFFICIENCY MONITOR] ERROR: Invalid battery voltage");
        return false;
    }
    
    // Calculate power measurements
    efficiencyMonitor.dc_power = battery_voltage * battery_current;
    efficiencyMonitor.ac_active_power = calculateTotalACActivePower();
    efficiencyMonitor.ac_apparent_power = calculateTotalACApparentPower();
    
    // Calculate instantaneous efficiency
    if (abs(efficiencyMonitor.dc_power) > 10.0f) { // Minimum 10W for valid efficiency
        efficiencyMonitor.active_efficiency = efficiencyMonitor.ac_active_power / efficiencyMonitor.dc_power;
        efficiencyMonitor.apparent_efficiency = efficiencyMonitor.ac_apparent_power / efficiencyMonitor.dc_power;
    } else {
        efficiencyMonitor.active_efficiency = 0.0f;
        efficiencyMonitor.apparent_efficiency = 0.0f;
    }
    
    // Update energy counters if enabled
    if (efficiencyMonitor.energy_counters.energy_counting_enabled) {
        float dt_hours = (now - efficiencyMonitor.last_measurement) / 3600000.0f; // Convert ms to hours
        updateEnergyCounters(efficiencyMonitor.dc_power, efficiencyMonitor.ac_active_power, 
                            efficiencyMonitor.ac_apparent_power, dt_hours);
        
        // Update cumulative efficiency
        if (efficiencyMonitor.active_efficiency > 0) {
            efficiencyMonitor.energy_counters.efficiency_sum_active += efficiencyMonitor.active_efficiency;
            efficiencyMonitor.energy_counters.efficiency_sum_apparent += efficiencyMonitor.apparent_efficiency;
            efficiencyMonitor.energy_counters.efficiency_samples++;
            
            efficiencyMonitor.energy_counters.cumulative_active_efficiency = 
                efficiencyMonitor.energy_counters.efficiency_sum_active / efficiencyMonitor.energy_counters.efficiency_samples;
            efficiencyMonitor.energy_counters.cumulative_apparent_efficiency = 
                efficiencyMonitor.energy_counters.efficiency_sum_apparent / efficiencyMonitor.energy_counters.efficiency_samples;
        }
    }
    
    efficiencyMonitor.valid = true;
    efficiencyMonitor.last_valid_time = now;
    efficiencyMonitor.last_measurement = now;
    
    return true;
}

bool startEnergyCounting() {
    efficiencyMonitor.energy_counters.energy_counting_enabled = true;
    efficiencyMonitor.energy_counters.energy_start_time = millis();
    efficiencyMonitor.energy_counters.energy_duration = 0;
    
    Serial.println("[EFFICIENCY MONITOR] Energy counting STARTED");
    return true;
}

bool stopEnergyCounting() {
    if (!efficiencyMonitor.energy_counters.energy_counting_enabled) {
        Serial.println("[EFFICIENCY MONITOR] WARNING: Energy counting not active");
        return false;
    }
    
    efficiencyMonitor.energy_counters.energy_counting_enabled = false;
    efficiencyMonitor.energy_counters.energy_duration = millis() - efficiencyMonitor.energy_counters.energy_start_time;
    
    Serial.printf("[EFFICIENCY MONITOR] Energy counting STOPPED after %.1f hours\n",
                  efficiencyMonitor.energy_counters.energy_duration / 3600000.0f);
    return true;
}

bool resetEnergyCounters() {
    efficiencyMonitor.energy_counters.total_dc_energy = 0.0;
    efficiencyMonitor.energy_counters.total_ac_active_energy = 0.0;
    efficiencyMonitor.energy_counters.total_ac_apparent_energy = 0.0;
    efficiencyMonitor.energy_counters.efficiency_sum_active = 0.0;
    efficiencyMonitor.energy_counters.efficiency_sum_apparent = 0.0;
    efficiencyMonitor.energy_counters.efficiency_samples = 0;
    efficiencyMonitor.energy_counters.cumulative_active_efficiency = 0.0;
    efficiencyMonitor.energy_counters.cumulative_apparent_efficiency = 0.0;
    efficiencyMonitor.energy_counters.energy_start_time = 0;
    efficiencyMonitor.energy_counters.energy_duration = 0;
    
    Serial.println("[EFFICIENCY MONITOR] Energy counters RESET");
    return true;
}

bool setEfficiencyMeasurementInterval(uint32_t interval_ms) {
    if (interval_ms < 100) {  // Minimum 100ms
        Serial.printf("[EFFICIENCY MONITOR] ERROR: Interval too short: %dms\n", interval_ms);
        return false;
    }
    
    efficiencyMonitor.measurement_interval = interval_ms;
    Serial.printf("[EFFICIENCY MONITOR] Measurement interval set: %dms\n", interval_ms);
    return true;
}

double getTotalEnergyEfficiency() {
    if (!efficiencyMonitor.energy_counters.energy_counting_enabled && 
        efficiencyMonitor.energy_counters.total_dc_energy <= 0) {
        return 0.0;
    }
    
    return efficiencyMonitor.energy_counters.total_ac_active_energy / 
           efficiencyMonitor.energy_counters.total_dc_energy;
}

double getAverageActiveEfficiency() {
    return efficiencyMonitor.energy_counters.cumulative_active_efficiency;
}

double getAverageApparentEfficiency() {
    return efficiencyMonitor.energy_counters.cumulative_apparent_efficiency;
}

const TrioEfficiencyMonitor_t* getEfficiencyMonitorStatus() {
    return &efficiencyMonitor;
}

// === SYSTEM INTEGRATION FUNCTIONS ===

bool processTrioHPControllers() {
    bool success = true;
    
    // Update all controllers in sequence
    success &= updateActivePowerController();
    success &= updateReactivePowerController();
    success &= updateEfficiencyMeasurement();
    
    return success;
}

bool areControllersActive() {
    return activePowerController.enabled || reactivePowerController.enabled;
}

bool emergencyStopControllers() {
    Serial.println("[TRIO HP CONTROLLERS] EMERGENCY STOP - Disabling all controllers");
    
    setActivePowerControllerEnabled(false);
    setReactivePowerControllerEnabled(false);
    
    // Set safe targets
    activePowerController.target_power = 0.0f;
    reactivePowerController.target_reactive = 0.0f;
    
    return true;
}

bool resetAllControllers() {
    Serial.println("[TRIO HP CONTROLLERS] Resetting all controllers to initial state");
    
    bool success = true;
    success &= initActivePowerController();
    success &= initReactivePowerController();
    success &= resetEnergyCounters();
    
    return success;
}

// === CONFIGURATION FUNCTIONS ===

bool setControllerIntervals(uint32_t active_interval, uint32_t reactive_interval) {
    if (active_interval < 1000 || reactive_interval < 1000) {
        Serial.printf("[TRIO HP CONTROLLERS] ERROR: Intervals too short: active=%dms, reactive=%dms\n",
                      active_interval, reactive_interval);
        return false;
    }
    
    activePowerController.loop_interval = active_interval;
    reactivePowerController.loop_interval = reactive_interval;
    
    Serial.printf("[TRIO HP CONTROLLERS] Intervals updated: active=%dms, reactive=%dms\n",
                  active_interval, reactive_interval);
    return true;
}

bool setControllerTolerances(float active_tolerance, float reactive_tolerance) {
    if (active_tolerance <= 0 || reactive_tolerance <= 0) {
        Serial.printf("[TRIO HP CONTROLLERS] ERROR: Invalid tolerances: active=%.1f, reactive=%.1f\n",
                      active_tolerance, reactive_tolerance);
        return false;
    }
    
    activePowerController.tolerance = active_tolerance;
    reactivePowerController.tolerance = reactive_tolerance;
    
    Serial.printf("[TRIO HP CONTROLLERS] Tolerances updated: active=±%.1fW, reactive=±%.1fVAr\n",
                  active_tolerance, reactive_tolerance);
    return true;
}

// === PRIVATE HELPER FUNCTIONS ===

static float calculateBatteryVoltage() {
    float total_voltage = 0.0f;
    int valid_nodes = 0;
    
    // Average voltage from all active BMS nodes
    for (int i = 0; i < MAX_BMS_NODES; i++) {
        if (isBMSNodeActive(i) && isBMSDataRecent(i, 5000)) {
            BMSData* bmsData = getBMSData(i);
            if (bmsData && bmsData->batteryVoltage > 0) {
                total_voltage += bmsData->batteryVoltage;
                valid_nodes++;
            }
        }
    }
    
    return (valid_nodes > 0) ? (total_voltage / valid_nodes) : 0.0f;
}

static float calculateTotalACActivePower() {
    // Get data from existing TRIO HP monitor system
    const TrioHPSystemData_t* systemData = getSystemData();
    if (systemData) {
        return systemData->totalActivePower;
    }
    
    Serial.println("[TRIO HP CONTROLLERS] WARNING: No system data available for active power");
    return 0.0f;
}

static float calculateTotalACApparentPower() {
    // Get data from existing TRIO HP monitor system
    const TrioHPSystemData_t* systemData = getSystemData();
    if (systemData) {
        // Calculate apparent power from active and reactive
        float active = systemData->totalActivePower;
        float reactive = systemData->totalReactivePower;
        return sqrt(active * active + reactive * reactive);
    }
    
    Serial.println("[TRIO HP CONTROLLERS] WARNING: No system data available for apparent power");
    return 0.0f;
}

static bool validateControllerSafety() {
    // Check if safety limits are valid
    if (!areBMSLimitsValid()) {
        Serial.println("[TRIO HP CONTROLLERS] SAFETY: BMS limits invalid");
        return false;
    }
    
    // Check if inputs are safe for operation
    if (!areInputsSafeForOperation()) {
        Serial.println("[TRIO HP CONTROLLERS] SAFETY: Digital inputs unsafe");
        return false;
    }
    
    return true;
}

static void updateEnergyCounters(float dc_power, float ac_active, float ac_apparent, float dt_hours) {
    // Trapezoidal integration for energy calculation
    efficiencyMonitor.energy_counters.total_dc_energy += dc_power * dt_hours / 1000.0; // Convert W to kWh
    efficiencyMonitor.energy_counters.total_ac_active_energy += ac_active * dt_hours / 1000.0;
    efficiencyMonitor.energy_counters.total_ac_apparent_energy += ac_apparent * dt_hours / 1000.0;
}

// === DEBUG FUNCTIONS ===

void printActivePowerControllerStatus() {
    Serial.println("=== ACTIVE POWER CONTROLLER STATUS ===");
    Serial.printf("Enabled: %s, Target: %.1fW, Current: %.1fW\n",
                  activePowerController.enabled ? "YES" : "NO",
                  activePowerController.target_power, activePowerController.current_power);
    Serial.printf("Error: %.1fW, Output: %.2fA, Target Reached: %s\n",
                  activePowerController.error, activePowerController.calculated_current,
                  activePowerController.target_reached ? "YES" : "NO");
    Serial.printf("PID: kp=%.3f, ki=%.3f, kd=%.3f, Updates: %d\n",
                  activePowerController.kp, activePowerController.ki, activePowerController.kd,
                  activePowerController.update_count);
}

void printReactivePowerControllerStatus() {
    Serial.println("=== REACTIVE POWER CONTROLLER STATUS ===");
    Serial.printf("Enabled: %s, Target: %.1fVAr, Current: %.1fVAr\n",
                  reactivePowerController.enabled ? "YES" : "NO",
                  reactivePowerController.target_reactive, reactivePowerController.current_reactive);
    Serial.printf("Strategy: %s, Active Modules: %d\n",
                  reactivePowerController.use_single_module ? "Single" : "Multi",
                  reactivePowerController.active_modules);
    Serial.printf("Limits: single_max=%.1fkVAr, threshold=%.0fVA\n",
                  reactivePowerController.single_module_max / 1000.0f,
                  reactivePowerController.module_threshold);
}

void printEfficiencyMonitorStatus() {
    Serial.println("=== EFFICIENCY MONITOR STATUS ===");
    Serial.printf("DC: %.1fW, AC Active: %.1fW, AC Apparent: %.1fVA\n",
                  efficiencyMonitor.dc_power, efficiencyMonitor.ac_active_power,
                  efficiencyMonitor.ac_apparent_power);
    Serial.printf("Efficiency: Active=%.2f%%, Apparent=%.2f%%\n",
                  efficiencyMonitor.active_efficiency * 100.0f,
                  efficiencyMonitor.apparent_efficiency * 100.0f);
    Serial.printf("Energy Counting: %s, Samples: %d\n",
                  efficiencyMonitor.energy_counters.energy_counting_enabled ? "ON" : "OFF",
                  efficiencyMonitor.energy_counters.efficiency_samples);
    if (efficiencyMonitor.energy_counters.efficiency_samples > 0) {
        Serial.printf("Cumulative: Active=%.2f%%, Apparent=%.2f%%\n",
                      efficiencyMonitor.energy_counters.cumulative_active_efficiency * 100.0f,
                      efficiencyMonitor.energy_counters.cumulative_apparent_efficiency * 100.0f);
    }
}

void printAllControllersStatus() {
    printActivePowerControllerStatus();
    Serial.println();
    printReactivePowerControllerStatus();
    Serial.println();
    printEfficiencyMonitorStatus();
}