// =====================================================================
// === trio_hp_controllers.h - ESP32S3 CAN to Modbus TCP Bridge ===
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
//    Internal: trio_hp_limits.h for safety validation
//    Internal: bms_data.h for battery data access
//    Internal: config.h for system constants
//    External: Arduino.h for standard types
//
// 📝 DESCRIPTION:
//    TRIO HP Phase 3 PID controllers and efficiency monitoring system.
//    Implements active power PID controller (DC current control based on AC power target),
//    reactive power PID controller with configurable single-module limits, and comprehensive
//    efficiency monitoring with both instantaneous measurements and cumulative energy counters.
//    Integrates with safety limits system for validated operation.
//
// 🔧 CONTROLLER SPECIFICATIONS:
//    Active Power PID:
//    - Target: AC Power [W], Control: DC Current [A]
//    - Formula: I_target = P_target / V_battery
//    - Tolerance: ±300W (configurable)
//    - Loop: 3s interval (configurable)
//    
//    Reactive Power PID:
//    - Threshold: 1500VA (configurable)
//    - Single Module Max: 10kVAr (CONFIGURABLE)
//    - Max per Module: 14kVAr
//    - Tolerance: ±300VAr (configurable)
//    - Loop: 3s interval (configurable)
//
// 📈 EFFICIENCY MONITORING:
//    - Instantaneous: P_AC/P_DC efficiency calculation
//    - Energy Counters: DC/AC energy integration [Wh]
//    - Measurement Interval: Configurable (default 1s)
//    - Cumulative Efficiency: Weighted average over time
//
// ⚠️  PERFORMANCE CRITICAL:
//    PID calculations run every 3s. Efficiency monitoring configurable interval.
//    Energy counters use double precision for accuracy over long periods.
//
// =====================================================================

#ifndef TRIO_HP_CONTROLLERS_H
#define TRIO_HP_CONTROLLERS_H

#include <Arduino.h>
#include "trio_hp_limits.h"
#include "../include/bms_data.h"
#include "../include/config.h"

// === ACTIVE POWER PID CONTROLLER ===
typedef struct {
    // Target and control variables
    float target_power;        // Target AC power [W]
    float current_power;       // Current sum of AC power from all modules [W]
    float calculated_current;  // Calculated DC current for target power [A]
    float tolerance;           // Power tolerance ±300W (configurable)
    uint32_t loop_interval;    // Control loop interval 3000ms (configurable)
    
    // PID parameters
    float kp, ki, kd;          // PID gains
    float error;               // Current error (target - actual)
    float last_error;          // Previous error for derivative calculation
    float integral;            // Integral accumulator for I term
    float derivative;          // Derivative term
    float output;              // PID controller output
    
    // Controller limits
    float output_min;          // Minimum controller output [A]
    float output_max;          // Maximum controller output [A]
    float integral_max;        // Anti-windup integral limit
    
    // Status and timing
    bool enabled;              // Controller enabled/disabled
    bool target_reached;       // True if within tolerance
    unsigned long last_update; // Last controller execution [ms]
    uint32_t update_count;     // Number of controller updates
} TrioActivePowerController_t;

// === REACTIVE POWER PID CONTROLLER ===
typedef struct {
    // Target and control variables
    float target_reactive;     // Target reactive power [VAr]
    float current_reactive;    // Current sum of reactive power from all modules [VAr]
    float tolerance;           // Reactive power tolerance ±300VAr (configurable)
    uint32_t loop_interval;    // Control loop interval 3000ms (configurable)
    float module_threshold;    // Threshold for single module operation 1500VA (configurable)
    
    // ✅ CORRECTED: Configurable single module limits
    float single_module_max;   // Maximum reactive power for single module [kVAr] (CONFIGURABLE - default 10kVAr)
    float max_per_module;      // Absolute maximum per module [kVAr] (14kVAr limit)
    
    // PID parameters
    float kp, ki, kd;          // PID gains
    float error;               // Current error (target - actual)
    float last_error;          // Previous error for derivative calculation
    float integral;            // Integral accumulator for I term
    float derivative;          // Derivative term
    float output;              // PID controller output [VAr]
    
    // Distribution strategy
    bool use_single_module;    // True if using single module strategy
    uint8_t active_modules;    // Number of active modules for distribution
    float per_module_reactive[16]; // Reactive power distribution per module [VAr]
    
    // Status and timing
    bool enabled;              // Controller enabled/disabled
    bool target_reached;       // True if within tolerance
    unsigned long last_update; // Last controller execution [ms]
    uint32_t update_count;     // Number of controller updates
} TrioReactivePowerController_t;

// === EFFICIENCY MONITORING SYSTEM ===
typedef struct {
    // Instantaneous measurements
    float dc_power;            // P_DC = I_battery × V_battery [W]
    float ac_active_power;     // Sum of P_AC from all modules [W]
    float ac_apparent_power;   // Sum of S_AC from all modules [VA]
    
    // Instantaneous efficiency indicators
    float active_efficiency;   // P_AC / P_DC ratio
    float apparent_efficiency; // S_AC / P_DC ratio
    
    // ✅ EXTENDED: Configurable measurement timing
    uint32_t measurement_interval;  // Measurement interval [ms] (CONFIGURABLE - default 1000ms)
    unsigned long last_measurement; // Timestamp of last measurement [ms]
    
    // ✅ EXTENDED: Energy integration counters
    struct {
        double total_dc_energy;        // Cumulative DC energy [Wh]
        double total_ac_active_energy; // Cumulative AC active energy [Wh]
        double total_ac_apparent_energy; // Cumulative AC apparent energy [VAh]
        unsigned long energy_start_time; // Start of energy counting [ms]
        unsigned long energy_duration;  // Total counting duration [ms]
        bool energy_counting_enabled;    // Energy counting active flag
        
        // Cumulative efficiency calculations
        double cumulative_active_efficiency;   // Weighted average active efficiency
        double cumulative_apparent_efficiency; // Weighted average apparent efficiency
        uint32_t efficiency_samples;           // Number of efficiency samples for averaging
        double efficiency_sum_active;          // Sum for active efficiency averaging
        double efficiency_sum_apparent;        // Sum for apparent efficiency averaging
    } energy_counters;
    
    // Measurement validity
    bool valid;                // Measurement validity flag
    unsigned long last_valid_time; // Last time measurements were valid [ms]
} TrioEfficiencyMonitor_t;

// === CONFIGURATION CONSTANTS ===

// Active Power PID defaults
#define TRIO_HP_ACTIVE_POWER_KP         0.1f     // Proportional gain
#define TRIO_HP_ACTIVE_POWER_KI         0.02f    // Integral gain  
#define TRIO_HP_ACTIVE_POWER_KD         0.01f    // Derivative gain
#define TRIO_HP_ACTIVE_POWER_TOLERANCE  300.0f   // ±300W tolerance
#define TRIO_HP_ACTIVE_POWER_INTERVAL   3000     // 3s loop interval [ms]

// Reactive Power PID defaults
#define TRIO_HP_REACTIVE_POWER_KP       0.08f    // Proportional gain
#define TRIO_HP_REACTIVE_POWER_KI       0.015f   // Integral gain
#define TRIO_HP_REACTIVE_POWER_KD       0.005f   // Derivative gain
#define TRIO_HP_REACTIVE_POWER_TOLERANCE 300.0f  // ±300VAr tolerance
#define TRIO_HP_REACTIVE_POWER_INTERVAL  3000    // 3s loop interval [ms]
#define TRIO_HP_REACTIVE_THRESHOLD       1500.0f // 1500VA threshold (configurable)
#define TRIO_HP_REACTIVE_SINGLE_MAX      10000.0f // 10kVAr single module (CONFIGURABLE)
#define TRIO_HP_REACTIVE_MODULE_MAX      14000.0f // 14kVAr absolute maximum per module

// Efficiency Monitoring defaults
#define TRIO_HP_EFFICIENCY_INTERVAL     1000     // 1s measurement interval [ms]
#define TRIO_HP_EFFICIENCY_TIMEOUT      5000     // 5s timeout for valid measurements [ms]

// === FUNCTION DECLARATIONS ===

// === INITIALIZATION FUNCTIONS ===

/**
 * @brief Initialize all TRIO HP controllers and efficiency monitoring
 * @return true if initialization successful, false otherwise
 */
bool initTrioHPControllers();

/**
 * @brief Initialize active power PID controller with default parameters
 * @return true if initialization successful, false otherwise
 */
bool initActivePowerController();

/**
 * @brief Initialize reactive power PID controller with default parameters
 * @return true if initialization successful, false otherwise
 */
bool initReactivePowerController();

/**
 * @brief Initialize efficiency monitoring system
 * @return true if initialization successful, false otherwise
 */
bool initTrioEfficiencyMonitor();

// === ACTIVE POWER CONTROLLER FUNCTIONS ===

/**
 * @brief Update active power PID controller (call every loop_interval)
 * @return true if controller updated successfully, false otherwise
 */
bool updateActivePowerController();

/**
 * @brief Set target active power for PID controller
 * @param target_watts Target AC power [W]
 * @return true if target set successfully, false if invalid
 */
bool setActivePowerTarget(float target_watts);

/**
 * @brief Enable/disable active power controller
 * @param enabled true to enable, false to disable
 */
void setActivePowerControllerEnabled(bool enabled);

/**
 * @brief Configure active power PID parameters
 * @param kp Proportional gain
 * @param ki Integral gain  
 * @param kd Derivative gain
 * @return true if parameters valid and set, false otherwise
 */
bool setActivePowerPIDParams(float kp, float ki, float kd);

/**
 * @brief Get current active power controller status
 * @return Pointer to current controller structure (read-only)
 */
const TrioActivePowerController_t* getActivePowerControllerStatus();

// === REACTIVE POWER CONTROLLER FUNCTIONS ===

/**
 * @brief Update reactive power PID controller (call every loop_interval)
 * @return true if controller updated successfully, false otherwise
 */
bool updateReactivePowerController();

/**
 * @brief Set target reactive power for PID controller
 * @param target_var Target reactive power [VAr]
 * @return true if target set successfully, false if invalid
 */
bool setReactivePowerTarget(float target_var);

/**
 * @brief Enable/disable reactive power controller
 * @param enabled true to enable, false to disable
 */
void setReactivePowerControllerEnabled(bool enabled);

/**
 * @brief Configure reactive power PID parameters
 * @param kp Proportional gain
 * @param ki Integral gain
 * @param kd Derivative gain
 * @return true if parameters valid and set, false otherwise
 */
bool setReactivePowerPIDParams(float kp, float ki, float kd);

/**
 * @brief Set configurable reactive power parameters
 * @param threshold Module threshold [VA] (default 1500VA)
 * @param single_max Single module maximum [kVAr] (CONFIGURABLE - default 10kVAr)
 * @return true if parameters valid and set, false otherwise
 */
bool setReactivePowerLimits(float threshold, float single_max);

/**
 * @brief Get current reactive power controller status
 * @return Pointer to current controller structure (read-only)
 */
const TrioReactivePowerController_t* getReactivePowerControllerStatus();

// === EFFICIENCY MONITORING FUNCTIONS ===

/**
 * @brief Update efficiency measurements (call every measurement_interval)
 * @return true if measurements updated successfully, false otherwise
 */
bool updateEfficiencyMeasurement();

/**
 * @brief Start energy counting for cumulative efficiency
 * @return true if counting started, false otherwise
 */
bool startEnergyCounting();

/**
 * @brief Stop energy counting and finalize calculations
 * @return true if counting stopped, false otherwise
 */
bool stopEnergyCounting();

/**
 * @brief Reset all energy counters to zero
 * @return true if counters reset successfully, false otherwise
 */
bool resetEnergyCounters();

/**
 * @brief Configure efficiency measurement interval
 * @param interval_ms Measurement interval [ms] (minimum 100ms)
 * @return true if interval valid and set, false otherwise
 */
bool setEfficiencyMeasurementInterval(uint32_t interval_ms);

/**
 * @brief Get total energy efficiency (AC_energy / DC_energy)
 * @return Total energy efficiency ratio, 0.0 if invalid
 */
double getTotalEnergyEfficiency();

/**
 * @brief Get average active power efficiency over counting period
 * @return Average active efficiency ratio, 0.0 if invalid
 */
double getAverageActiveEfficiency();

/**
 * @brief Get average apparent power efficiency over counting period
 * @return Average apparent efficiency ratio, 0.0 if invalid
 */
double getAverageApparentEfficiency();

/**
 * @brief Get current efficiency monitor status
 * @return Pointer to current efficiency structure (read-only)
 */
const TrioEfficiencyMonitor_t* getEfficiencyMonitorStatus();

// === SYSTEM INTEGRATION FUNCTIONS ===

/**
 * @brief Process all TRIO HP controllers (call from main loop)
 * @return true if all controllers processed successfully, false otherwise
 */
bool processTrioHPControllers();

/**
 * @brief Check if any controller is active and needs attention
 * @return true if controllers are active, false if all idle
 */
bool areControllersActive();

/**
 * @brief Emergency stop all controllers
 * @return true if all controllers stopped safely, false otherwise
 */
bool emergencyStopControllers();

/**
 * @brief Reset all controllers to initial state
 * @return true if reset successful, false otherwise
 */
bool resetAllControllers();

// === CONFIGURATION FUNCTIONS ===

/**
 * @brief Configure controller loop intervals
 * @param active_interval Active power loop interval [ms]
 * @param reactive_interval Reactive power loop interval [ms]
 * @return true if intervals valid and set, false otherwise
 */
bool setControllerIntervals(uint32_t active_interval, uint32_t reactive_interval);

/**
 * @brief Configure power tolerances
 * @param active_tolerance Active power tolerance [W]
 * @param reactive_tolerance Reactive power tolerance [VAr]
 * @return true if tolerances valid and set, false otherwise
 */
bool setControllerTolerances(float active_tolerance, float reactive_tolerance);

// === DEBUG FUNCTIONS ===

/**
 * @brief Print active power controller status to Serial
 */
void printActivePowerControllerStatus();

/**
 * @brief Print reactive power controller status to Serial
 */
void printReactivePowerControllerStatus();

/**
 * @brief Print efficiency monitoring status to Serial
 */
void printEfficiencyMonitorStatus();

/**
 * @brief Print comprehensive controllers status to Serial
 */
void printAllControllersStatus();

#endif // TRIO_HP_CONTROLLERS_H