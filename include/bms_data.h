/*
 * ESP32S3 CAN to Modbus TCP Bridge - Struktury Danych BMS
 * 
 * VERSION: v4.0.0 - MODULAR ARCHITECTURE
 * DATE: 2025-08-12
 * STATUS: ✅ READY FOR COMPILATION
 * 
 * OPIS: Definicje struktur danych dla wszystkich ramek protokołu IFS BMS
 */

#ifndef BMS_DATA_H
#define BMS_DATA_H

#include "config.h"
#include <Arduino.h>

// ================================
// === BMS DATA STRUCTURES ===
// ================================

/**
 * @brief Główna struktura danych dla jednego modułu BMS
 * 
 * Zawiera wszystkie dane z 9 typów ramek CAN plus informacje diagnostyczne
 * Każdy moduł BMS zajmuje 125 rejestrów Modbus (zgodnie z mapą rejestrów)
 */
struct BMSData {
    // ================================
    // === FRAME 0x190 - PODSTAWOWE DANE ===
    // ================================
    float batteryVoltage;               // Napięcie baterii [V] - skala 0.0625
    float batteryCurrent;               // Prąd baterii [A] - skala 0.0625  
    float remainingEnergy;              // Pozostała energia [kWh] - skala 0.1
    float soc;                          // Stan naładowania [%] - skala 1.0
    
    // Frame 0x190 error flags (8 flag z bajtu 7)
    bool masterError;                   // Bit 0: Błąd główny
    bool cellVoltageError;              // Bit 1: Błąd napięcia ogniw
    bool cellTempMinError;              // Bit 2: Błąd min. temperatury ogniw
    bool cellTempMaxError;              // Bit 3: Błąd max. temperatury ogniw
    bool cellVoltageMinError;           // Bit 4: Błąd min. napięcia ogniw
    bool cellVoltageMaxError;           // Bit 5: Błąd max. napięcia ogniw
    bool systemShutdown;                // Bit 6: Wyłączenie systemu
    bool ibbVoltageSupplyError;         // Bit 7: Błąd zasilania IBB
    
    // ================================
    // === FRAME 0x290 - NAPIĘCIA OGNIW ===
    // ================================
    float cellMinVoltage;               // Minimalne napięcie ogniwa [V] - skala 0.001
    float cellMeanVoltage;              // Średnie napięcie ogniwa [V] - skala 0.001
    uint8_t minVoltageBlock;            // Blok z min. napięciem
    uint8_t minVoltageCell;             // Ogniwo z min. napięciem
    uint8_t minVoltageString;           // String z min. napięciem
    uint8_t balancingTempMax;           // Maksymalna temperatura balansowania [°C]
    
    // ================================
    // === FRAME 0x310 - SOH, TEMPERATURA, IMPEDANCJA ===
    // ================================
    float soh;                          // Stan zdrowia baterii [%] - skala 0.1
    float cellVoltage;                  // Napięcie ogniwa [V] - skala 0.001
    float cellTemperature;              // Temperatura ogniwa [°C] - skala 0.1
    float dcir;                         // Wewnętrzna rezystancja DC [mΩ] - skala 0.1
    bool nonEqualStringsRamp;           // Rampa nierównych stringów
    bool dynamicLimitationTimer;        // Timer dynamicznych ograniczeń
    bool overcurrentTimer;              // Timer nadprądu
    uint16_t channelMultiplexor;        // Multiplekser kanału
    
    // ================================
    // === FRAME 0x390 - MAKSYMALNE NAPIĘCIA OGNIW ===
    // ================================
    float cellMaxVoltage;               // Maksymalne napięcie ogniwa [V] - skala 0.001
    float cellVoltageDelta;             // Delta napięć ogniw [V] - skala 0.001
    uint8_t maxVoltageBlock;            // Blok z max. napięciem
    uint8_t maxVoltageCell;             // Ogniwo z max. napięciem
    uint8_t maxVoltageString;           // String z max. napięciem
    uint8_t afeTemperatureMax;          // Maksymalna temperatura AFE [°C]
    
    // ================================
    // === FRAME 0x410 - TEMPERATURY, STANY GOTOWOŚCI ===
    // ================================
    float cellMaxTemperature;           // Maksymalna temperatura ogniwa [°C] - skala 0.1
    float cellTempDelta;                // Delta temperatur ogniw [°C] - skala 0.1
    uint8_t maxTempString;              // String z max. temperaturą
    uint8_t maxTempBlock;               // Blok z max. temperaturą
    uint8_t maxTempSensor;              // Sensor z max. temperaturą
    bool readyToCharge;                 // Gotowość do ładowania
    bool readyToDischarge;              // Gotowość do rozładowania
    
    // ================================
    // === FRAME 0x510 - LIMITY MOCY, STANY I/O ===
    // ================================
    float dccl;                         // DC Charge Limit [A] - skala 0.0625
    float ddcl;                         // DC Discharge Limit [A] - skala 0.0625
    bool input_IN02;                    // Stan wejścia IN02
    bool input_IN01;                    // Stan wejścia IN01
    bool relay_AUX4;                    // Stan przekaźnika AUX4
    bool relay_AUX3;                    // Stan przekaźnika AUX3
    bool relay_AUX2;                    // Stan przekaźnika AUX2
    bool relay_AUX1;                    // Stan przekaźnika AUX1
    bool relay_R2;                      // Stan przekaźnika R2
    bool relay_R1;                      // Stan przekaźnika R1
    
    // ================================
    // === FRAME 0x490 - DANE MULTIPLEKSOWANE ===
    // ================================
    uint8_t mux490Type;                 // Typ multipleksera (0x00-0x35)
    uint16_t mux490Value;               // Wartość multipleksera (16-bit)
    
    // Konkretne zmienne z multipleksowanych danych Frame 0x490
    uint16_t serialNumber0;             // 0x00 - Numer seryjny low
    uint16_t serialNumber1;             // 0x01 - Numer seryjny high
    uint16_t hwVersion0;                // 0x02 - Wersja HW low
    uint16_t hwVersion1;                // 0x03 - Wersja HW high
    uint16_t swVersion0;                // 0x04 - Wersja SW low
    uint16_t swVersion1;                // 0x05 - Wersja SW high
    float factoryEnergy;                // 0x06 - Energia fabryczna [kWh] - skala 0.1
    float designCapacity;               // 0x07 - Pojemność projektowa [Ah] - skala 0.0625
    float systemDesignedEnergy;         // 0x0C - Energia systemowa [kWh] - skala 0.1
    float ballancerTempMaxBlock;        // 0x0D - Max temp balansera [°C] - skala 0.5
    float ltcTempMaxBlock;              // 0x0E - Max temp LTC [°C] - skala 0.5
    float inletTemperature;             // 0x0F - Temperatura wlotu [°C] - skala 0.5
    float outletTemperature;            // 0x0F - Temperatura wylotu [°C] - skala 0.5
    uint8_t humidity;                   // 0x10 - Wilgotność [%]
    uint16_t errorMap0;                 // 0x13 - Mapa błędów 0
    uint16_t errorMap1;                 // 0x14 - Mapa błędów 1
    uint16_t errorMap2;                 // 0x15 - Mapa błędów 2
    uint16_t errorMap3;                 // 0x16 - Mapa błędów 3
    uint16_t timeToFullCharge;          // 0x17 - Czas do pełnego ładowania [min]
    uint16_t timeToFullDischarge;       // 0x18 - Czas do pełnego rozładowania [min]
    uint16_t batteryCycles;             // 0x1A - Liczba cykli baterii
    
    // Dodatkowe dane multipleksowane (przykład - można rozszerzyć)
    float chargeEnergy0;                // 0x30 - Energia ładowania low [kWh]
    float chargeEnergy1;                // 0x31 - Energia ładowania high [kWh]
    float dischargeEnergy0;             // 0x32 - Energia rozładowania low [kWh]
    float dischargeEnergy1;             // 0x33 - Energia rozładowania high [kWh]
    float recuperativeEnergy0;          // 0x34 - Energia rekuperacyjna low [kWh]
    float recuperativeEnergy1;          // 0x35 - Energia rekuperacyjna high [kWh]
    
    // ================================
    // === FRAME 0x1B0 - DANE DODATKOWE ===
    // ================================
    uint8_t frame1B0Data[8];            // Raw data z ramki 0x1B0 (do przyszłego parsowania)
    
    // ================================
    // === FRAME 0x710 - STAN CANOPEN ===
    // ================================
    uint8_t canopenState;               // Stan protokołu CANopen
    
    // ================================
    // === KOMUNIKACJA I DIAGNOSTYKA ===
    // ================================
    unsigned long lastUpdate;           // Timestamp ostatniej aktualizacji [ms]
    bool communicationOk;               // Status komunikacji (true = OK)
    uint32_t packetsReceived;           // Liczba odebranych pakietów
    uint16_t parseErrors;               // Liczba błędów parsowania
    
    // Liczniki ramek (dla diagnostyki)
    uint32_t frame190Count;             // Licznik ramek 190
    uint32_t frame290Count;             // Licznik ramek 290
    uint32_t frame310Count;             // Licznik ramek 310
    uint32_t frame390Count;             // Licznik ramek 390
    uint32_t frame410Count;             // Licznik ramek 410
    uint32_t frame510Count;             // Licznik ramek 510
    uint32_t frame490Count;             // Licznik ramek 490
    uint32_t frame1B0Count;             // Licznik ramek 1B0
    uint32_t frame710Count;             // Licznik ramek 710
    
    // Statystyki komunikacji
    unsigned long firstFrameTime;       // Timestamp pierwszej ramki [ms]
    unsigned long lastFrameTime;        // Timestamp ostatniej ramki [ms]
    uint32_t totalFrames;               // Całkowita liczba ramek
    uint32_t invalidFrames;             // Liczba nieprawidłowych ramek
    
    // ================================
    // === KONSTRUKTOR ===
    // ================================
    BMSData() {
        // Inicjalizacja wartości domyślnych
        batteryVoltage = 0.0f;
        batteryCurrent = 0.0f;
        remainingEnergy = 0.0f;
        soc = 0.0f;
        
        // Inicjalizacja flag błędów
        masterError = false;
        cellVoltageError = false;
        cellTempMinError = false;
        cellTempMaxError = false;
        cellVoltageMinError = false;
        cellVoltageMaxError = false;
        systemShutdown = false;
        ibbVoltageSupplyError = false;
        
        // Inicjalizacja napięć ogniw
        cellMinVoltage = 0.0f;
        cellMeanVoltage = 0.0f;
        minVoltageBlock = 0;
        minVoltageCell = 0;
        minVoltageString = 0;
        balancingTempMax = 0;
        
        // Inicjalizacja SOH i temperatury
        soh = 0.0f;
        cellVoltage = 0.0f;
        cellTemperature = 0.0f;
        dcir = 0.0f;
        nonEqualStringsRamp = false;
        dynamicLimitationTimer = false;
        overcurrentTimer = false;
        channelMultiplexor = 0;
        
        // Inicjalizacja maksymalnych napięć
        cellMaxVoltage = 0.0f;
        cellVoltageDelta = 0.0f;
        maxVoltageBlock = 0;
        maxVoltageCell = 0;
        maxVoltageString = 0;
        afeTemperatureMax = 0;
        
        // Inicjalizacja temperatur i stanów
        cellMaxTemperature = 0.0f;
        cellTempDelta = 0.0f;
        maxTempString = 0;
        maxTempBlock = 0;
        maxTempSensor = 0;
        readyToCharge = false;
        readyToDischarge = false;
        
        // Inicjalizacja limitów mocy
        dccl = 0.0f;
        ddcl = 0.0f;
        input_IN02 = false;
        input_IN01 = false;
        relay_AUX4 = false;
        relay_AUX3 = false;
        relay_AUX2 = false;
        relay_AUX1 = false;
        relay_R2 = false;
        relay_R1 = false;
        
        // Inicjalizacja multipleksera
        mux490Type = 0;
        mux490Value = 0;
        serialNumber0 = 0;
        serialNumber1 = 0;
        hwVersion0 = 0;
        hwVersion1 = 0;
        swVersion0 = 0;
        swVersion1 = 0;
        factoryEnergy = 0.0f;
        designCapacity = 0.0f;
        systemDesignedEnergy = 0.0f;
        ballancerTempMaxBlock = 0.0f;
        ltcTempMaxBlock = 0.0f;
        inletTemperature = 0.0f;
        outletTemperature = 0.0f;
        humidity = 0;
        errorMap0 = 0;
        errorMap1 = 0;
        errorMap2 = 0;
        errorMap3 = 0;
        timeToFullCharge = 0;
        timeToFullDischarge = 0;
        batteryCycles = 0;
        chargeEnergy0 = 0.0f;
        chargeEnergy1 = 0.0f;
        dischargeEnergy0 = 0.0f;
        dischargeEnergy1 = 0.0f;
        recuperativeEnergy0 = 0.0f;
        recuperativeEnergy1 = 0.0f;
        
        // Inicjalizacja danych dodatkowych
        memset(frame1B0Data, 0, sizeof(frame1B0Data));
        canopenState = 0;
        
        // Inicjalizacja komunikacji i diagnostyki
        lastUpdate = 0;
        communicationOk = false;
        packetsReceived = 0;
        parseErrors = 0;
        frame190Count = 0;
        frame290Count = 0;
        frame310Count = 0;
        frame390Count = 0;
        frame410Count = 0;
        frame510Count = 0;
        frame490Count = 0;
        frame1B0Count = 0;
        frame710Count = 0;
        firstFrameTime = 0;
        lastFrameTime = 0;
        totalFrames = 0;
        invalidFrames = 0;
    }
};

// ================================
// === SYSTEM STATISTICS ===
// ================================

/**
 * @brief Struktura statystyk systemu CAN i Modbus
 */
struct SystemStats {
    // Statystyki CAN
    uint32_t totalFramesReceived;       // Wszystkie ramki CAN
    uint32_t validBmsFrames;            // Poprawne ramki BMS  
    uint32_t invalidFrames;             // Ramki spoza protokołu
    uint32_t parseErrors;               // Błędy parsowania
    uint32_t canErrors;                 // Błędy komunikacji CAN
    
    // Statystyki Modbus TCP
    uint32_t modbusRequests;            // Zapytania Modbus TCP
    uint32_t modbusResponses;           // Odpowiedzi Modbus TCP
    uint32_t modbusErrors;              // Błędy Modbus TCP
    uint32_t modbusTimeouts;            // Timeouty Modbus
    uint32_t activeConnections;         // Aktywne połączenia TCP
    
    // Statystyki czasowe
    unsigned long systemStartTime;      // Czas startu systemu [ms]
    unsigned long lastFrameTime;        // Ostatnia ramka CAN [ms]
    unsigned long lastModbusRequest;    // Ostatnie zapytanie Modbus [ms]
    unsigned long lastHeartbeat;        // Ostatni heartbeat [ms]
    
    // Statystyki systemowe
    uint32_t systemResets;              // Liczba resetów systemu
    uint32_t wifiReconnects;            // Liczba reconnect WiFi
    uint32_t memoryLeaks;               // Wycieki pamięci
    uint32_t watchdogResets;            // Resety watchdog
    
    // Performance metrics
    uint32_t maxLoopTime;               // Maksymalny czas pętli [μs]
    uint32_t avgLoopTime;               // Średni czas pętli [μs]
    uint32_t freeHeapMin;               // Minimalna wolna pamięć [bytes]
    uint32_t freeHeapCurrent;           // Aktualna wolna pamięć [bytes]
    
    // Constructor
    SystemStats() {
        totalFramesReceived = 0;
        validBmsFrames = 0;
        invalidFrames = 0;
        parseErrors = 0;
        canErrors = 0;
        modbusRequests = 0;
        modbusResponses = 0;
        modbusErrors = 0;
        modbusTimeouts = 0;
        activeConnections = 0;
        systemStartTime = millis();
        lastFrameTime = 0;
        lastModbusRequest = 0;
        lastHeartbeat = 0;
        systemResets = 0;
        wifiReconnects = 0;
        memoryLeaks = 0;
        watchdogResets = 0;
        maxLoopTime = 0;
        avgLoopTime = 0;
        freeHeapMin = ESP.getFreeHeap();
        freeHeapCurrent = ESP.getFreeHeap();
    }
};

// ================================
// === BMS MANAGER CLASS ===
// ================================

/**
 * @brief Klasa zarządzająca danymi wszystkich modułów BMS
 */
class BMSManager {
private:
    BMSData bmsModules[MAX_BMS_NODES];  // Tablica modułów BMS
    SystemStats stats;                  // Statystyki systemu
    uint8_t activeBmsNodes;            // Liczba aktywnych węzłów
    uint8_t bmsNodeIds[MAX_BMS_NODES]; // ID węzłów BMS
    
public:
    // Constructor
    BMSManager() : activeBmsNodes(ACTIVE_BMS_NODES) {
        // Inicjalizacja domyślnych ID węzłów
        for (int i = 0; i < MAX_BMS_NODES; i++) {
            bmsNodeIds[i] = DEFAULT_BMS_NODE_IDS[i];
        }
    }
    
    // ================================
    // === GETTERY I SETTERY ===
    // ================================
    
    /**
     * @brief Pobiera referencję do danych modułu BMS o podanym indeksie
     * @param index Indeks modułu (0-15)
     * @return Referencja do BMSData lub nullptr jeśli nieprawidłowy indeks
     */
    BMSData* getBMSData(uint8_t index) {
        if (index >= MAX_BMS_NODES) return nullptr;
        return &bmsModules[index];
    }
    
    /**
     * @brief Pobiera referencję do danych modułu BMS o podanym Node ID
     * @param nodeId Node ID modułu BMS (1-16)
     * @return Referencja do BMSData lub nullptr jeśli nieznaleziony
     */
    BMSData* getBMSDataByNodeId(uint8_t nodeId) {
        int index = getBatteryIndexFromNodeId(nodeId);
        if (index < 0) return nullptr;
        return &bmsModules[index];
    }
    
    /**
     * @brief Pobiera statystyki systemu
     * @return Referencja do SystemStats
     */
    SystemStats* getSystemStats() {
        return &stats;
    }
    
    /**
     * @brief Pobiera liczbę aktywnych węzłów BMS
     * @return Liczba aktywnych węzłów
     */
    uint8_t getActiveBmsCount() const {
        return activeBmsNodes;
    }
    
    /**
     * @brief Pobiera Node ID dla podanego indeksu
     * @param index Indeks (0-15)
     * @return Node ID lub 0 jeśli nieprawidłowy indeks
     */
    uint8_t getNodeId(uint8_t index) const {
        if (index >= MAX_BMS_NODES) return 0;
        return bmsNodeIds[index];
    }
    
    // ================================
    // === UTILITY FUNCTIONS ===
    // ================================
    
    /**
     * @brief Konwertuje Node ID na indeks tablicy
     * @param nodeId Node ID modułu BMS (1-16)
     * @return Indeks (0-15) lub -1 jeśli nie znaleziono
     */
    int getBatteryIndexFromNodeId(uint8_t nodeId) {
        for (int i = 0; i < activeBmsNodes; i++) {
            if (bmsNodeIds[i] == nodeId) {
                return i;
            }
        }
        return -1; // Nie znaleziono
    }
    
    /**
     * @brief Sprawdza czy Node ID jest aktywny
     * @param nodeId Node ID do sprawdzenia
     * @return true jeśli aktywny, false w przeciwnym razie
     */
    bool isNodeIdActive(uint8_t nodeId) {
        return getBatteryIndexFromNodeId(nodeId) >= 0;
    }
    
    /**
     * @brief Sprawdza komunikację ze wszystkimi modułami BMS
     * @param timeoutMs Timeout w milisekundach
     * @return Liczba modułów z aktywną komunikacją
     */
    uint8_t checkCommunication(unsigned long timeoutMs = BMS_COMMUNICATION_TIMEOUT_MS) {
        uint8_t activeCount = 0;
        unsigned long currentTime = millis();
        
        for (int i = 0; i < activeBmsNodes; i++) {
            BMSData* bms = &bmsModules[i];
            if (bms->lastUpdate > 0 && (currentTime - bms->lastUpdate) < timeoutMs) {
                bms->communicationOk = true;
                activeCount++;
            } else {
                bms->communicationOk = false;
            }
        }
        
        return activeCount;
    }
    
    /**
     * @brief Resetuje statystyki wszystkich modułów BMS
     */
    void resetAllStats() {
        for (int i = 0; i < MAX_BMS_NODES; i++) {
            bmsModules[i].parseErrors = 0;
            bmsModules[i].frame190Count = 0;
            bmsModules[i].frame290Count = 0;
            bmsModules[i].frame310Count = 0;
            bmsModules[i].frame390Count = 0;
            bmsModules[i].frame410Count = 0;
            bmsModules[i].frame510Count = 0;
            bmsModules[i].frame490Count = 0;
            bmsModules[i].frame1B0Count = 0;
            bmsModules[i].frame710Count = 0;
            bmsModules[i].totalFrames = 0;
            bmsModules[i].invalidFrames = 0;
        }
        
        // Reset system stats
        stats.totalFramesReceived = 0;
        stats.validBmsFrames = 0;
        stats.invalidFrames = 0;
        stats.parseErrors = 0;
        stats.canErrors = 0;
        stats.modbusRequests = 0;
        stats.modbusResponses = 0;
        stats.modbusErrors = 0;
        stats.modbusTimeouts = 0;
    }
    
    /**
     * @brief Aktualizuje statystyki wydajności
     * @param loopTime Czas wykonania pętli w mikrosekundach
     */
    void updatePerformanceStats(uint32_t loopTime) {
        if (loopTime > stats.maxLoopTime) {
            stats.maxLoopTime = loopTime;
        }
        
        // Prosta średnia krocząca (można ulepszyć)
        stats.avgLoopTime = (stats.avgLoopTime * 9 + loopTime) / 10;
        
        // Aktualizacja pamięci
        uint32_t currentHeap = ESP.getFreeHeap();
        stats.freeHeapCurrent = currentHeap;
        if (currentHeap < stats.freeHeapMin) {
            stats.freeHeapMin = currentHeap;
        }
    }
    
    /**
     * @brief Pobiera podsumowanie stanu systemu
     * @return String z podsumowaniem
     */
    String getSystemSummary() {
        uint8_t activeBatteries = checkCommunication();
        unsigned long uptime = (millis() - stats.systemStartTime) / 1000; // sekundy
        
        String summary = "System: " + String(activeBatteries) + "/" + String(activeBmsNodes) + " BMS active, ";
        summary += "Uptime: " + String(uptime) + "s, ";
        summary += "CAN: " + String(stats.validBmsFrames) + " frames, ";
        summary += "Modbus: " + String(stats.modbusRequests) + " requests, ";
        summary += "Heap: " + String(stats.freeHeapCurrent) + " bytes";
        
        return summary;
    }
};

// ================================
// === MULTIPLEXER UTILITIES ===
// ================================

/**
 * @brief Struktura opisująca typ multipleksera Frame 490
 */
struct MultiplexerType {
    uint8_t type;               // Typ multipleksera (0x00-0x35)
    const char* name;           // Nazwa parametru
    const char* unit;           // Jednostka
    float scale;                // Skala konwersji
    bool isSigned;              // Czy wartość jest ze znakiem
};

/**
 * @brief Tablica opisów typów multipleksera Frame 490
 */
static const MultiplexerType MULTIPLEXER_TYPES[] = {
    {0x00, "Serial Number 0", "", 1.0f, false},
    {0x01, "Serial Number 1", "", 1.0f, false},
    {0x02, "HW Version 0", "", 1.0f, false},
    {0x03, "HW Version 1", "", 1.0f, false},
    {0x04, "SW Version 0", "", 1.0f, false},
    {0x05, "SW Version 1", "", 1.0f, false},
    {0x06, "Factory Energy", "kWh", 0.1f, false},
    {0x07, "Design Capacity", "Ah", 0.0625f, false},
    {0x0C, "System Designed Energy", "kWh", 0.1f, false},
    {0x0D, "Ballancer Temp Max Block", "°C", 0.5f, true},
    {0x0E, "LTC Temp Max Block", "°C", 0.5f, true},
    {0x0F, "Inlet/Outlet Temperature", "°C", 0.5f, true},
    {0x10, "Humidity", "%", 1.0f, false},
    {0x13, "Error Map 0", "", 1.0f, false},
    {0x14, "Error Map 1", "", 1.0f, false},
    {0x15, "Error Map 2", "", 1.0f, false},
    {0x16, "Error Map 3", "", 1.0f, false},
    {0x17, "Time to Full Charge", "min", 1.0f, false},
    {0x18, "Time to Full Discharge", "min", 1.0f, false},
    {0x1A, "Battery Cycles", "", 1.0f, false},
    {0x30, "Charge Energy 0", "kWh", 0.1f, false},
    {0x31, "Charge Energy 1", "kWh", 0.1f, false},
    {0x32, "Discharge Energy 0", "kWh", 0.1f, false},
    {0x33, "Discharge Energy 1", "kWh", 0.1f, false},
    {0x34, "Recuperative Energy 0", "kWh", 0.1f, false},
    {0x35, "Recuperative Energy 1", "kWh", 0.1f, false}
};

/**
 * @brief Pobiera opis typu multipleksera
 * @param type Typ multipleksera (0x00-0x35)
 * @return Wskaźnik do MultiplexerType lub nullptr jeśli nie znaleziono
 */
const MultiplexerType* getMultiplexerTypeInfo(uint8_t type);

/**
 * @brief Konwertuje wartość multipleksera na float z odpowiednią skalą
 * @param type Typ multipleksera
 * @param rawValue Surowa wartość 16-bit
 * @return Skonwertowana wartość float
 */
float convertMultiplexerValue(uint8_t type, uint16_t rawValue);

#endif // BMS_DATA_H