# TRIO HP FAZA 3: Plan Implementacji - Sterowanie i Limity

**Data utworzenia:** 28.08.2025 18:15 (Warsaw Time)  
**Status:** ZATWIERDZONY przez użytkownika  
**Faza:** 3 z 6 (Sterowanie i limity)  
**Szacowany czas:** 165-205 min (2.5-3.5 godzin)

---

## 🎯 CEL FAZA 3
Implementacja kompletnego systemu sterowania TRIO HP z:
- Integracją limitów BMS (DCCL/DDCL)
- PID regulatorami mocy czynnej i biernej  
- 10-krokową procedurą startu
- 2-krokową procedurą stopu
- Monitoringiem sprawności z licznikami całkującymi
- Systemem blokady parametrów

---

## 📊 CZĘŚĆ 1: Core Safety & Limits Integration (40-45 min)

### **Nowy plik: `src/trio_hp_limits.h`**
```cpp
// Struktury danych:
typedef struct {
    float dccl_bms;          // Z BMS (Dynamic Discharge Current Limit)  
    float ddcl_bms;          // Z BMS (Dynamic Charge Current Limit)
    float dccl_threshold;    // Konfigurowalny próg (max 100%)
    float ddcl_threshold;    // Konfigurowalny próg (max 100%) 
    bool limits_valid;       
    unsigned long last_update;
} TrioHPLimits_t;

// Digital inputs integration z BMS:
typedef struct {
    bool estop_active;       // Input 10 - E-STOP z bms_data.inputs
    bool ac_contactor;       // Input 9 - Stycznik AC z bms_data.inputs  
    bool inputs_valid;       
    unsigned long last_update;
} TrioHPDigitalInputs_t;

// Functions:
bool initTrioHPLimits();
bool updateBMSLimits(uint8_t bmsNodeId);              // Aktualizuj z bms_data.dccl/ddcl
bool updateDigitalInputs();                           // Aktualizuj z bms_data.inputs
bool validateRequestedCurrent(float current);         // Sprawdź czy prąd mieści się w limitach  
bool validateRequestedPower(float power);             // Sprawdź czy moc mieści się w limitach
float getEffectiveCurrentLimit(bool charging);        // Zwróć aktualny limit z progami
bool isEstopActive();                                 // Check input 10 (bit 2 w inputs)
bool isACContactorClosed();                          // Check input 9 (bit 1 w inputs)
bool areInputsSafeForOperation();                    // Comprehensive safety check
```

### **Nowy plik: `src/trio_hp_limits.cpp`**
```cpp
// Implementation:
- Połączenie z extern BMSData bmsNodes[16] z include/bms_data.h
- Aktualizacja limitów z bms_data.dccl/ddcl wszystkich BMS nodes
- Digital inputs parsing z bms_data.inputs (bit operations dla input 9,10)
- Safety validation przed każdą komendą prądową/mocową
- Konfigurowalne progi bezpieczeństwa (domyślnie 90% limitów BMS)
```

---

## 📊 CZĘŚĆ 2: Operational Readiness Control - POPRAWIONE (25-30 min)

### **Rozszerzenia w `src/trio_hp_manager.h`:**
```cpp
typedef enum {
    TRIO_SYSTEM_OFF = 0,           // 0x11 0x10 A1 
    TRIO_SYSTEM_OPERATIONAL = 1    // 0x11 0x10 A0
} TrioSystemState_t;

// Nowe funkcje:
bool setSystemOperationalReadiness(bool ready);
bool canSendCommand(uint16_t command);           // Check czy komenda dozwolona w obecnym stanie
bool isSystemOperational();                     // Status sprawdzenia
TrioSystemState_t getCurrentSystemState();
```

### **POPRAWIONA LOGIC w `src/trio_hp_manager.cpp`:**
```cpp
bool canSendCommand(uint16_t command) {
    TrioSystemState_t state = getCurrentSystemState();
    
    if (state == TRIO_SYSTEM_OFF) {
        // ✅ POPRAWKA: OFF state = WSZYSTKIE komendy dozwolone BEZ WYJĄTKU
        return true;  
    }
    
    if (state == TRIO_SYSTEM_OPERATIONAL) {
        // ON state: TYLKO operational commands dozwolone
        return (command == 0x1002 ||  // System current
                command == 0x2108 ||  // Reactive power
                command == 0x2110 ||  // Work mode 
                command == 0x2117);   // Reactive type
    }
    return false;
}
```

---

## 📊 CZĘŚĆ 3: PID Controllers - Z POPRAWKAMI (45-50 min)

### **Nowy plik: `src/trio_hp_controllers.h`**
```cpp
// PID Controller dla mocy czynnej:
typedef struct {
    float target_power;        // Zadana moc AC [W]
    float current_power;       // Rzeczywista suma mocy AC [W] 
    float calculated_current;  // Obliczony prąd DC [A]
    float tolerance;           // Tolerancja ±300W (konfigurowalna)
    uint32_t loop_interval;    // Pętla co 3s (konfigurowalna)
    
    // PID parameters
    float kp, ki, kd;          // PID gains
    float error, last_error, integral; // PID state
    unsigned long last_update;
} TrioActivePowerController_t;

// PID Controller dla mocy biernej - Z POPRAWKAMI:
typedef struct {
    float target_reactive;     // Zadana moc bierna [VAr]
    float current_reactive;    // Rzeczywista suma mocy biernej [VAr]
    float tolerance;           // ±300VAr (konfigurowalna)
    uint32_t loop_interval;    // Co 3s (konfigurowalna)
    float module_threshold;    // 1500VA próg (konfigurowalny)
    
    // ✅ POPRAWKA: single_module_max jako konfigurowalna wartość
    float single_module_max;   // 10kVAr próg podziału (KONFIGUROWALNY)
    float max_per_module;      // 14kVAr maksimum na moduł
    
    // PID parameters  
    float kp, ki, kd;
    float error, last_error, integral;
    unsigned long last_update;
} TrioReactivePowerController_t;
```

---

## 📊 CZĘŚĆ 4: Efficiency Monitoring - ROZSZERZONE (20-25 min)

### **ROZSZERZONY `src/trio_hp_controllers.h`:**
```cpp
// ✅ ROZSZERZENIE: Efficiency monitoring z czasem pomiaru + liczniki całkujące  
typedef struct {
    // Chwilowe pomiary
    float dc_power;            // P_DC = I_battery × V_battery
    float ac_active_power;     // Suma P_AC z modułów
    float ac_apparent_power;   // Suma S_AC z modułów  
    
    // Efficiency wskaźniki chwilowe
    float active_efficiency;   // P_AC / P_DC  
    float apparent_efficiency; // S_AC / P_DC
    
    // ✅ NOWE: Czas chwilowego pomiaru (konfigurowalny)
    uint32_t measurement_interval;  // Czas pomiaru chwilowego (ms) - KONFIGUROWALNY
    unsigned long last_measurement; // Timestamp ostatniego pomiaru
    
    // ✅ NOWE: Liczniki całkujące (jeśli starczy zasobów)
    struct {
        double total_dc_energy;        // Całkowita energia DC [Wh] 
        double total_ac_active_energy; // Całkowita energia AC active [Wh]
        double total_ac_apparent_energy; // Całkowita energia AC apparent [VAh]
        unsigned long energy_start_time; // Start liczenia energii
        unsigned long energy_duration;  // Czas liczenia [ms]
        bool energy_counting_enabled;    // Czy liczenie włączone
        
        // Sprawności całkujące
        double cumulative_active_efficiency;   // Średnia sprawność active
        double cumulative_apparent_efficiency; // Średnia sprawność apparent
        uint32_t efficiency_samples;           // Liczba próbek do średniej
    } energy_counters;
    
    bool valid;                // Status ważności pomiarów
} TrioEfficiencyMonitor_t;

// Functions:
bool initTrioEfficiencyMonitor();
bool updateEfficiencyMeasurement();     // Pomiar chwilowy
bool resetEnergyCounters();             // Reset liczników całkujących  
bool startEnergyCounting();             // Start liczenia energii
bool stopEnergyCounting();              // Stop liczenia energii
double getTotalEnergyEfficiency();      // Sprawność całkowa energetyczna
double getAverageActiveEfficiency();    // Średnia sprawność mocy czynnej
double getAverageApparentEfficiency();  // Średnia sprawność mocy pozornej
```

---

## 📊 CZĘŚĆ 5: Parameter Locking System (10-15 min)

### **Rozszerzenia w `src/trio_hp_config.h`:**
```cpp
// Parameter locking system:
typedef struct {
    bool parameters_locked;    // Master lock/unlock switch (rejestr Modbus + WWW)
    bool allow_power_changes;  // Czy można zmieniać moc
    bool allow_mode_changes;   // Czy można zmieniać tryby
    uint8_t lock_level;        // 0=unlocked, 1=basic_lock, 2=full_lock
    unsigned long lock_timestamp; // Kiedy zablokowano
} TrioParameterLock_t;

// Functions:
bool setParameterLockMode(uint8_t lock_level);
bool canModifyParameter(uint16_t parameter_id);
bool isParameterLocked(const char* parameter_name);
```

---

## 📊 CZĘŚĆ 6: Main Integration (20-25 min)

### **Modyfikacje w `src/main.cpp`:**
```cpp
// Dodaj nowe includes:
#include "trio_hp_limits.h"
#include "trio_hp_controllers.h"

// W setup():
bool setupTrioHPPhase3() {
    if (!initTrioHPLimits()) return false;
    if (!initTrioHPControllers()) return false;  
    if (!initTrioEfficiencyMonitor()) return false;
    if (!loadTrioHPConfiguration()) return false;
    return true;
}

// W loop():  
void processTrioHPPhase3() {
    // Update z BMS data co loop
    updateBMSLimits(getCurrentBMSNode());
    updateDigitalInputs(); // E-STOP + AC contactor
    
    // PID controllers (co 3s lub konfigurowalny interval)
    updateActivePowerController(); 
    updateReactivePowerController();  
    
    // Efficiency monitoring (co measurement_interval)
    updateEfficiencyMeasurement();
}
```

---

## 🗂️ KOMPLETNA LISTA PLIKÓW - FINALNA

### **4 NOWE PLIKI (1,200+ linii kodu):**
1. **`src/trio_hp_limits.h`** - Headers, struktury, enums (150 linii)
2. **`src/trio_hp_limits.cpp`** - Implementation safety + BMS integration (300 linii)  
3. **`src/trio_hp_controllers.h`** - PID headers + efficiency structures (200 linii)
4. **`src/trio_hp_controllers.cpp`** - Implementation PID + efficiency (550 linii)

### **5 ROZSZERZONYCH PLIKÓW (400+ nowych linii):**
5. **`src/trio_hp_manager.h`** - Operational readiness logic (50 nowych linii)
6. **`src/trio_hp_manager.cpp`** - Implementation poprawionej logic (100 nowych linii)
7. **`src/trio_hp_config.h`** - Startup sequences + locking (80 nowych linii) 
8. **`src/trio_hp_config.cpp`** - Implementation sequences (120 nowych linii)
9. **`src/main.cpp`** - Integration Phase 3 systemów (50 nowych linii)

### **AKTUALIZACJA DOKUMENTACJI:**
10. **`TRIO_HP_PHASE_3_REQUIREMENTS.md`** - ✅ Już utworzony
11. **`TRIO_HP_PHASE_3_IMPLEMENTATION_PLAN.md`** - ✅ Ten plik

---

## 🚀 STARTUP SEQUENCE - 10 KROKÓW (Referencja)

1. **E-STOP Check:** Sprawdź input 10 w bms_data.inputs  
2. **Ready to Charge:** Sprawdź bms_data.readyToCharge
3. **AC Contactor:** Sprawdź input 9 w bms_data.inputs
4. **Heartbeat Detection:** Wykryj moduły przez 0x0757F7xx
5. **Broadcast Settings:** Wyślij ustawienia systemowe
6. **Module State Read:** Start odczytu 0x23 commands  
7. **Multicast Settings:** Wyślij ustawienia modułów
8. **Calculate Current:** Oblicz wymagany prąd z target power
9. **Send Power Commands:** Wyślij prąd + kierunek + reactive (jeśli zadana)
10. **Operational ON:** Set 0x11 0x10 A0 (system ready)

## 🛑 SHUTDOWN SEQUENCE - 2 KROKI (Referencja)

1. **Current to Zero:** Najpierw prąd = 0
2. **Operational OFF:** Potem 0x11 0x10 A1

---

## 🎛️ REGULATORY SPECS (Referencja)

### **Active Power PID:**
- **Target:** Moc AC [W] 
- **Control:** Prąd DC [A]  
- **Formula:** I_target = P_target / V_battery
- **Tolerance:** ±300W (konfigurowalna)
- **Loop:** Co 3s (konfigurowalna)

### **Reactive Power PID:**  
- **Threshold:** 1500VA (konfigurowalna)
- **Single Module:** ≤10kVAr (KONFIGUROWALNA!)
- **Max per Module:** 14kVAr  
- **Tolerance:** ±300VAr (konfigurowalna)
- **Loop:** Co 3s (konfigurowalna)

---

## 📈 EFFICIENCY MONITORING SPECS (Rozszerzone)

### **Chwilowe Pomiary:**
- **DC Power:** I_battery × V_battery
- **AC Active:** Suma P_AC modułów
- **AC Apparent:** Suma S_AC modułów
- **Interval:** Konfigurowalny (domyślnie co 1s)

### **Liczniki Całkujące (jeśli starczy zasobów):**
- **Total DC Energy [Wh]**
- **Total AC Active Energy [Wh]** 
- **Total AC Apparent Energy [VAh]**
- **Cumulative Efficiency** (średnie ważone)
- **Energy Duration Counter**

---

## ⚠️ KLUCZOWE POPRAWKI UŻYTKOWNIKA

1. **✅ OFF State Logic:** WSZYSTKIE komendy dozwolone bez wyjątku przy wyłączeniu
2. **✅ Konfigurowalna single_module_max:** 10kVAr jako zmienna wejściowa  
3. **✅ Extended Efficiency:** Czas pomiaru + liczniki całkujące energii (jeśli starczy zasobów)

---

## 🔗 INTEGRATION POINTS

### **Z BMS System:**
- `bms_data.dccl/ddcl` → trio_hp_limits  
- `bms_data.inputs` → digital I/O (E-STOP, AC contactor)
- `bms_data.readyToCharge` → startup procedure

### **Z Existing TRIO HP:**
- `trio_hp_protocol.h` - Utilize existing commands
- `trio_hp_monitor.h` - Extend polling z nowymi requirements  
- `trio_hp_config.h` - Add configuration dla controllers

### **Z Main System:**
- `main.cpp` - Integration w main loop
- `modbus_tcp.cpp` - Expose nowe parametry w rejestrach
- Web interface - Kontrola parameter locking

---

**Plik ten służy jako kompletna referenca podczas implementacji TRIO HP FAZA 3.**
**Każda sekcja zawiera dokładne specifikacje zgodne z wymaganiami użytkownika.**

**Status:** GOTOWY DO IMPLEMENTACJI ✅