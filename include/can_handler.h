/*
 * ESP32S3 CAN to Modbus TCP Bridge - Obsługa Komunikacji CAN
 * 
 * VERSION: v4.0.0 - MODULAR ARCHITECTURE
 * DATE: 2025-08-12
 * STATUS: ✅ READY FOR COMPILATION
 * 
 * OPIS: Moduł odpowiedzialny za komunikację CAN i parsowanie ramek IFS BMS
 */

#ifndef CAN_HANDLER_H
#define CAN_HANDLER_H

#include "config.h"
#include "bms_data.h"
#include <mcp_can.h>
#include <SPI.h>

// ================================
// === CAN HANDLER CLASS ===
// ================================

/**
 * @brief Klasa obsługująca komunikację CAN z modułami BMS
 */
class CANHandler {
private:
    MCP_CAN* canBus;                    // Wskaźnik do obiektu MCP_CAN
    BMSManager* bmsManager;             // Wskaźnik do managera BMS
    bool initialized;                   // Status inicjalizacji CAN
    unsigned long lastFrameTime;       // Czas ostatniej ramki
    uint32_t frameCounter;              // Licznik wszystkich ramek
    
    // Private methods
    bool initializeMCP2515();
    uint8_t extractNodeIdFromCanId(unsigned long canId, uint16_t baseId);
    bool isValidBMSFrame(unsigned long canId);
    void printCANFrame(unsigned long canId, unsigned char len, unsigned char* buf);
    void routeCANFrame(unsigned long canId, unsigned char len, unsigned char* buf);
    
public:
    // Constructor & Destructor
    CANHandler(BMSManager* manager);
    ~CANHandler();
    
    // Public interface
    bool initialize();
    void process();
    bool isInitialized() const { return initialized; }
    unsigned long getLastFrameTime() const { return lastFrameTime; }
    uint32_t getFrameCounter() const { return frameCounter; }
    
    // Statistics
    void printStatistics();
    void resetStatistics();
};

// ================================
// === FRAME PARSERS ===
// ================================

/**
 * @brief Klasa zawierająca parsery wszystkich typów ramek BMS
 */
class BMSFrameParsers {
private:
    BMSManager* bmsManager;
    
public:
    BMSFrameParsers(BMSManager* manager) : bmsManager(manager) {}
    
    // Frame parsers - każdy odpowiada za jeden typ ramki
    void parseFrame190(uint8_t nodeId, unsigned char* data);  // Podstawowe dane
    void parseFrame290(uint8_t nodeId, unsigned char* data);  // Napięcia ogniw
    void parseFrame310(uint8_t nodeId, unsigned char* data);  // SOH, temperatura, impedancja
    void parseFrame390(uint8_t nodeId, unsigned char* data);  // Maksymalne napięcia ogniw
    void parseFrame410(uint8_t nodeId, unsigned char* data);  // Temperatury, stany gotowości
    void parseFrame510(uint8_t nodeId, unsigned char* data);  // Limity mocy, stany I/O
    void parseFrame490(uint8_t nodeId, unsigned char* data);  // Dane multipleksowane
    void parseFrame1B0(uint8_t nodeId, unsigned char* data);  // Dane dodatkowe
    void parseFrame710(uint8_t nodeId, unsigned char* data);  // Stan CANopen
    
private:
    // Helper methods
    void updateBMSTimestamp(uint8_t nodeId);
    void incrementFrameCounter(uint8_t nodeId, uint8_t frameType);
    bool validateDataRange(float value, float min, float max, const char* paramName);
    void handleParseError(uint8_t nodeId, const char* frameType, const char* error);
};

// ================================
// === UTILITY FUNCTIONS ===
// ================================

/**
 * @brief Wyciąga Node ID z CAN ID na podstawie bazowego ID
 * @param canId Identyfikator CAN
 * @param baseId Bazowy identyfikator ramki (np. 0x180 dla Frame 190)
 * @return Node ID (1-16) lub 0 jeśli nieprawidłowy
 */
uint8_t extractNodeId(unsigned long canId, uint16_t baseId);

/**
 * @brief Sprawdza czy CAN ID należy do protokołu BMS
 * @param canId Identyfikator CAN do sprawdzenia
 * @return true jeśli to ramka BMS, false w przeciwnym razie
 */
bool isBMSFrame(unsigned long canId);

/**
 * @brief Pobiera typ ramki na podstawie CAN ID
 * @param canId Identyfikator CAN
 * @return Typ ramki (190, 290, 310, etc.) lub 0 jeśli nieznany
 */
uint16_t getFrameType(unsigned long canId);

/**
 * @brief Pobiera opis ramki na podstawie CAN ID
 * @param canId Identyfikator CAN
 * @return Opis ramki jako string
 */
const char* getFrameDescription(unsigned long canId);

/**
 * @brief Konwertuje 2 bajty little-endian na uint16_t
 * @param lowByte Młodszy bajt
 * @param highByte Starszy bajt
 * @return Wartość 16-bitowa
 */
inline uint16_t bytesToUint16(uint8_t lowByte, uint8_t highByte) {
    return (highByte << 8) | lowByte;
}

/**
 * @brief Konwertuje 2 bajty little-endian na int16_t
 * @param lowByte Młodszy bajt
 * @param highByte Starszy bajt
 * @return Wartość 16-bitowa ze znakiem
 */
inline int16_t bytesToInt16(uint8_t lowByte, uint8_t highByte) {
    return (int16_t)((highByte << 8) | lowByte);
}

/**
 * @brief Sprawdza bit w bajcie
 * @param byte Bajt do sprawdzenia
 * @param bitPosition Pozycja bitu (0-7)
 * @return true jeśli bit jest ustawiony, false w przeciwnym razie
 */
inline bool isBitSet(uint8_t byte, uint8_t bitPosition) {
    return (byte & (1 << bitPosition)) != 0;
}

/**
 * @brief Formatuje dane CAN do wyświetlenia
 * @param data Wskaźnik na dane
 * @param length Długość danych
 * @return Sformatowany string hex
 */
String formatCANData(unsigned char* data, unsigned char length);

/**
 * @brief Sprawdza czy wartość mieści się w dozwolonym zakresie
 * @param value Wartość do sprawdzenia
 * @param min Minimalna wartość
 * @param max Maksymalna wartość
 * @return true jeśli wartość jest prawidłowa
 */
template<typename T>
bool isValueInRange(T value, T min, T max) {
    return value >= min && value <= max;
}

// ================================
// === FRAME TYPE DEFINITIONS ===
// ================================

/**
 * @brief Enum z typami ramek dla łatwiejszego zarządzania
 */
enum BMSFrameType {
    FRAME_TYPE_UNKNOWN = 0,
    FRAME_TYPE_190 = 190,    // Podstawowe dane
    FRAME_TYPE_290 = 290,    // Napięcia ogniw
    FRAME_TYPE_310 = 310,    // SOH, temperatura, impedancja
    FRAME_TYPE_390 = 390,    // Maksymalne napięcia ogniw
    FRAME_TYPE_410 = 410,    // Temperatury, stany gotowości
    FRAME_TYPE_510 = 510,    // Limity mocy, stany I/O
    FRAME_TYPE_490 = 490,    // Dane multipleksowane
    FRAME_TYPE_1B0 = 0x1B0,  // Dane dodatkowe
    FRAME_TYPE_710 = 710     // Stan CANopen
};

/**
 * @brief Struktura opisująca konfigurację typu ramki
 */
struct FrameConfig {
    BMSFrameType type;
    uint16_t baseId;
    const char* name;
    const char* description;
    uint16_t expectedFrequency;  // Oczekiwana częstotliwość [ms]
    bool critical;               // Czy ramka jest krytyczna dla działania
};

/**
 * @brief Tablica konfiguracji wszystkich typów ramek
 */
static const FrameConfig FRAME_CONFIGS[] = {
    {FRAME_TYPE_190, CAN_FRAME_190_BASE, "Frame 190", "Basic data (voltage, current, SOC)", CAN_FREQ_HIGH, true},
    {FRAME_TYPE_290, CAN_FRAME_290_BASE, "Frame 290", "Cell voltages (min, mean)", CAN_FREQ_MEDIUM, false},
    {FRAME_TYPE_310, CAN_FRAME_310_BASE, "Frame 310", "SOH, temperature, impedance", CAN_FREQ_MEDIUM, false},
    {FRAME_TYPE_390, CAN_FRAME_390_BASE, "Frame 390", "Max cell voltages", CAN_FREQ_MEDIUM, false},
    {FRAME_TYPE_410, CAN_FRAME_410_BASE, "Frame 410", "Temperatures, ready states", CAN_FREQ_MEDIUM, false},
    {FRAME_TYPE_510, CAN_FRAME_510_BASE, "Frame 510", "Power limits, I/O states", CAN_FREQ_MEDIUM, false},
    {FRAME_TYPE_490, CAN_FRAME_490_BASE, "Frame 490", "Multiplexed data", CAN_FREQ_LOW, false},
    {FRAME_TYPE_1B0, CAN_FRAME_1B0_BASE, "Frame 1B0", "Additional data", CAN_FREQ_LOW, false},
    {FRAME_TYPE_710, CAN_FRAME_710_BASE, "Frame 710", "CANopen state", CAN_FREQ_LOW, false}
};

/**
 * @brief Pobiera konfigurację ramki na podstawie typu
 * @param frameType Typ ramki
 * @return Wskaźnik do FrameConfig lub nullptr jeśli nie znaleziono
 */
const FrameConfig* getFrameConfig(BMSFrameType frameType);

/**
 * @brief Pobiera konfigurację ramki na podstawie CAN ID
 * @param canId Identyfikator CAN
 * @return Wskaźnik do FrameConfig lub nullptr jeśli nie znaleziono
 */
const FrameConfig* getFrameConfigByCanId(unsigned long canId);

#endif // CAN_HANDLER_H