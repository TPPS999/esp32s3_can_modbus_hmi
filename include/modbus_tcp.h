/*
 * modbus_tcp.h - ESP32S3 CAN to Modbus TCP Bridge Modbus Server Header
 * 
 * VERSION: v4.0.1 - COMPLETE DECLARATIONS  
 * DATE: 2025-08-13
 * STATUS: ✅ READY - Wszystkie deklaracje funkcji z kompletnego modbus_tcp.cpp
 * 
 * DESCRIPTION: Kompletny interfejs dla serwera Modbus TCP
 * - Mapowanie 125 rejestrów per BMS (2000 rejestrów total)
 * - Wszystkie 54 typy multipleksera Frame 490
 * - Zaawansowane utility functions i statystyki
 * - Kompatybilność z oryginalnym kodem v3.0.0
 */

#ifndef MODBUS_TCP_H
#define MODBUS_TCP_H

#include "config.h"
#include "bms_data.h"
#include <WiFi.h>
#include <WiFiServer.h>
#include <WiFiClient.h>

// === 🔥 MODBUS TCP CONSTANTS (z v3.0.0) ===
#define MODBUS_TCP_PORT 502
#define MODBUS_SLAVE_ID 1
#define MODBUS_MAX_HOLDING_REGISTERS 2000  // 16 baterii * 125 rejestrów
#define MODBUS_REGISTERS_PER_BMS 125       // Rejestry na jedną baterię
#define MODBUS_MAX_FRAME_SIZE 260
#define MODBUS_CLIENT_TIMEOUT_MS 60000

// === 🔥 MODBUS FUNCTION CODES ===
#define MODBUS_FUNC_READ_HOLDING_REGISTERS   0x03
#define MODBUS_FUNC_READ_INPUT_REGISTERS     0x04
#define MODBUS_FUNC_WRITE_SINGLE_REGISTER    0x06
#define MODBUS_FUNC_WRITE_MULTIPLE_REGISTERS 0x10

// === 🔥 MODBUS EXCEPTION CODES ===
#define MODBUS_EXCEPTION_ILLEGAL_FUNCTION        0x01
#define MODBUS_EXCEPTION_ILLEGAL_DATA_ADDRESS    0x02
#define MODBUS_EXCEPTION_ILLEGAL_DATA_VALUE      0x03
#define MODBUS_EXCEPTION_SLAVE_DEVICE_FAILURE    0x04
#define MODBUS_EXCEPTION_ACKNOWLEDGE             0x05
#define MODBUS_EXCEPTION_SLAVE_DEVICE_BUSY       0x06

// === 🔥 MODBUS TCP STATISTICS STRUCTURE (z v3.0.0) ===
typedef struct {
  unsigned long totalRequests;
  unsigned long totalResponses;
  unsigned long invalidRequests;
  unsigned long exceptionResponses;
  unsigned long clientConnections;
  unsigned long clientTimeouts;
  unsigned long readHoldingRegisterRequests;
  unsigned long readInputRegisterRequests;
  unsigned long writeSingleRegisterRequests;
  unsigned long writeMultipleRegisterRequests;
  unsigned long lastRequestTime;
} ModbusStats;

// === 🔥 MODBUS STATE ENUMERATION ===
typedef enum {
  MODBUS_STATE_UNINITIALIZED = 0,
  MODBUS_STATE_INITIALIZING,
  MODBUS_STATE_RUNNING,
  MODBUS_STATE_ERROR,
  MODBUS_STATE_CLIENT_CONNECTED
} ModbusState_t;

// === 🔥 MBAP HEADER STRUCTURE ===
struct MBAPHeader {
  uint16_t transactionId;     // Transaction ID
  uint16_t protocolId;        // Protocol ID (always 0x0000)
  uint16_t length;            // PDU length + Slave ID
  uint8_t slaveId;            // Slave ID
};

// === 🔥 GLOBAL VARIABLES (extern declarations) ===
extern WiFiServer modbusServer;
extern WiFiClient currentModbusClient;
extern uint16_t holdingRegisters[MODBUS_MAX_HOLDING_REGISTERS];
extern ModbusStats modbusStats;
extern ModbusState_t modbusState;

// === 🔥 MAIN SETUP & PROCESSING FUNCTIONS ===

/**
 * @brief Inicjalizuje serwer Modbus TCP
 * @return true jeśli inicjalizacja się powiodła
 */
bool setupModbusTCP();

/**
 * @brief Przetwarza zapytania Modbus TCP (wywoływane w głównej pętli)
 */
void processModbusTCP();

// === 🔥 KOMPLETNE MAPOWANIE MODBUS (z v3.0.0) ===

/**
 * @brief Aktualizuje rejestry Modbus dla danego BMS (125 rejestrów)
 * @param nodeId ID węzła BMS
 */
void updateModbusRegisters(uint8_t nodeId);

/**
 * @brief Aktualizuje pojedynczy rejestr Modbus
 * @param address Adres rejestru (0-1999)
 * @param value Wartość rejestru
 */
void updateModbusRegister(uint16_t address, uint16_t value);

/**
 * @brief Odczytuje wartość rejestru Modbus
 * @param address Adres rejestru (0-1999)
 * @return Wartość rejestru
 */
uint16_t readModbusRegister(uint16_t address);

// === 🔥 UTILITY FUNCTIONS (z v3.0.0) ===

/**
 * @brief Konwertuje float na rejestr Modbus z skalowaniem
 * @param value Wartość float
 * @param scale Skala (domyślnie 100.0)
 * @return Wartość 16-bitowa ze znakiem
 */
uint16_t floatToModbusRegister(float value, float scale);

/**
 * @brief Konwertuje rejestr Modbus na float z skalowaniem
 * @param reg Wartość rejestru
 * @param scale Skala (domyślnie 100.0)
 * @return Wartość float
 */
float modbusRegisterToFloat(uint16_t reg, float scale);

// === 🔥 STATE MANAGEMENT ===

/**
 * @brief Ustawia stan serwera Modbus
 * @param newState Nowy stan
 */
void setModbusState(ModbusState_t newState);

/**
 * @brief Pobiera aktualny stan serwera Modbus
 * @return Aktualny stan
 */
ModbusState_t getModbusState();

// === 🔥 DEBUG & DIAGNOSTICS ===

/**
 * @brief Włącza/wyłącza debug logowania Modbus
 * @param enable true aby włączyć
 */
void enableModbusDebug(bool enable);

/**
 * @brief Wyświetla statystyki serwera Modbus
 */
void printModbusStatistics();

// === 🔥 EXTENDED HEARTBEAT & MAPPING (NOWE z v3.0.0) ===

/**
 * @brief Rozszerzony heartbeat z danymi multipleksera
 * @param nodeId ID węzła BMS
 */
void printBMSHeartbeatExtended(uint8_t nodeId);

/**
 * @brief Wyświetla kompletną mapę rejestrów Modbus
 */
void printCompleteModbusRegisterMap();

// === 🔥 REGISTER MAP DEFINITIONS (z v3.0.0) ===
// Każda bateria zajmuje 125 rejestrów (base = battery_index * 125)

// BASIC DATA & ERROR FLAGS (0-19)
#define MODBUS_REG_BATTERY_VOLTAGE       0    // mV
#define MODBUS_REG_BATTERY_CURRENT       1    // mA
#define MODBUS_REG_REMAINING_ENERGY      2    // 0.01 kWh
#define MODBUS_REG_SOC                   3    // 0.1%
#define MODBUS_REG_MASTER_ERROR          10   // 0/1
#define MODBUS_REG_CELL_VOLTAGE_ERROR    11   // 0/1
#define MODBUS_REG_CELL_TEMP_MIN_ERROR   12   // 0/1
#define MODBUS_REG_CELL_TEMP_MAX_ERROR   13   // 0/1
#define MODBUS_REG_SYSTEM_SHUTDOWN       16   // 0/1

// CELL VOLTAGES (20-29)
#define MODBUS_REG_CELL_MIN_VOLTAGE      20   // 0.1mV
#define MODBUS_REG_CELL_MEAN_VOLTAGE     21   // 0.1mV
#define MODBUS_REG_MIN_VOLTAGE_BLOCK     22
#define MODBUS_REG_MIN_VOLTAGE_CELL      23

// SOH & TEMPERATURE (30-39)
#define MODBUS_REG_SOH                   30   // 0.1%
#define MODBUS_REG_CELL_VOLTAGE          31   // 0.1mV
#define MODBUS_REG_CELL_TEMPERATURE      32   // 0.1°C
#define MODBUS_REG_DCIR                  33   // 0.1mΩ

// MAX VOLTAGES (40-49)
#define MODBUS_REG_CELL_MAX_VOLTAGE      40   // 0.1mV
#define MODBUS_REG_CELL_VOLTAGE_DELTA    41   // 0.1mV
#define MODBUS_REG_MAX_VOLTAGE_BLOCK     42
#define MODBUS_REG_MAX_VOLTAGE_CELL      43

// TEMPERATURES & READY STATES (50-59)
#define MODBUS_REG_CELL_MAX_TEMPERATURE  50   // 0.1°C
#define MODBUS_REG_CELL_TEMP_DELTA       51   // 0.1°C
#define MODBUS_REG_READY_TO_CHARGE       55   // 0/1
#define MODBUS_REG_READY_TO_DISCHARGE    56   // 0/1

// POWER LIMITS & I/O (60-69)
#define MODBUS_REG_DCCL                  60   // mA (discharge limit)
#define MODBUS_REG_DDCL                  61   // mA (charge limit)
#define MODBUS_REG_RELAY_R1              69   // 0/1
#define MODBUS_REG_RELAY_R2              68   // 0/1

// 🔥 MULTIPLEXER DATA (70-89) - KLUCZOWE NOWE REJESTRY
#define MODBUS_REG_MUX490_TYPE           70   // Typ multipleksera
#define MODBUS_REG_MUX490_VALUE          71   // Wartość multipleksera
#define MODBUS_REG_SERIAL_NUMBER_LOW     72   // Serial number niski
#define MODBUS_REG_SERIAL_NUMBER_HIGH    73   // Serial number wysoki
#define MODBUS_REG_HW_VERSION_LOW        74   // Wersja HW niska
#define MODBUS_REG_HW_VERSION_HIGH       75   // Wersja HW wysoka
#define MODBUS_REG_SW_VERSION_LOW        76   // Wersja SW niska
#define MODBUS_REG_SW_VERSION_HIGH       77   // Wersja SW wysoka
#define MODBUS_REG_FACTORY_ENERGY        78   // 0.1 kWh
#define MODBUS_REG_DESIGN_CAPACITY       79   // mAh
#define MODBUS_REG_SYSTEM_ENERGY         80   // 0.1 kWh
#define MODBUS_REG_HUMIDITY              85   // %
#define MODBUS_REG_TIME_TO_FULL_CHARGE   86   // min
#define MODBUS_REG_TIME_TO_FULL_DISCHARGE 87  // min
#define MODBUS_REG_BATTERY_CYCLES        88   // cycles
#define MODBUS_REG_DETECTED_IMBS         89   // count

// 🔥 ERROR MAPS & VERSIONS (90-109) - NOWE REJESTRY DIAGNOSTYCZNE
#define MODBUS_REG_ERROR_MAP_0           90   // Error map bits 0-15
#define MODBUS_REG_ERROR_MAP_1           91   // Error map bits 16-31
#define MODBUS_REG_ERROR_MAP_2           92   // Error map bits 32-47
#define MODBUS_REG_ERROR_MAP_3           93   // Error map bits 48-63
#define MODBUS_REG_BL_VERSION_LOW        94   // Bootloader version low
#define MODBUS_REG_BL_VERSION_HIGH       95   // Bootloader version high
#define MODBUS_REG_CONFIG_CRC            100  // Configuration CRC
#define MODBUS_REG_IOT_STATUS            101  // IoT status
#define MODBUS_REG_POWER_ON_COUNTER      102  // Power on counter

// 🔥 COMMUNICATION STATUS (110-119) - NOWE REJESTRY
#define MODBUS_REG_CANOPEN_STATE         110  // CANopen state
#define MODBUS_REG_COMMUNICATION_OK      111  // Communication OK (0/1)
#define MODBUS_REG_PACKETS_RECEIVED_LOW  112  // Packets received low
#define MODBUS_REG_PACKETS_RECEIVED_HIGH 113  // Packets received high
#define MODBUS_REG_PARSE_ERRORS          114  // Parse errors
#define MODBUS_REG_FRAME_190_COUNT       115  // Frame 190 counter
#define MODBUS_REG_FRAME_290_COUNT       116  // Frame 290 counter
#define MODBUS_REG_FRAME_310_COUNT       117  // Frame 310 counter
#define MODBUS_REG_FRAME_490_COUNT       118  // Frame 490 counter
#define MODBUS_REG_FRAME_710_COUNT       119  // Frame 710 counter

// 🔥 EXTENDED COUNTERS (120-124) - NOWE REJESTRY
#define MODBUS_REG_FRAME_390_COUNT       120  // Frame 390 counter
#define MODBUS_REG_FRAME_410_COUNT       121  // Frame 410 counter
#define MODBUS_REG_FRAME_510_COUNT       122  // Frame 510 counter
#define MODBUS_REG_FRAME_1B0_COUNT       123  // Frame 1B0 counter
#define MODBUS_REG_NODE_ID               124  // Node ID

// === 🔥 REGISTER CALCULATION MACROS ===

/**
 * @brief Oblicza bazowy adres rejestrów dla danej baterii
 * @param batteryIndex Indeks baterii (0-15)
 * @return Bazowy adres (batteryIndex * 125)
 */
#define MODBUS_BATTERY_BASE_ADDR(batteryIndex) ((batteryIndex) * 125)

/**
 * @brief Oblicza adres rejestru dla danej baterii
 * @param batteryIndex Indeks baterii (0-15)
 * @param regOffset Offset rejestru (0-124)
 * @return Pełny adres rejestru
 */
#define MODBUS_BATTERY_REG_ADDR(batteryIndex, regOffset) \
  (MODBUS_BATTERY_BASE_ADDR(batteryIndex) + (regOffset))

/**
 * @brief Sprawdza czy adres rejestru jest prawidłowy
 * @param address Adres rejestru
 * @return true jeśli adres jest prawidłowy
 */
#define MODBUS_IS_VALID_ADDRESS(address) \
  ((address) < MODBUS_MAX_HOLDING_REGISTERS)

// === 🔥 BATTERY EXAMPLES (z v3.0.0) ===
// Przykłady adresów dla różnych baterii:
// Bateria 0: 0-124      | Bateria 1: 125-249    | Bateria 2: 250-374
// Bateria 3: 375-499    | Bateria 4: 500-624    | Bateria 5: 625-749
// ...
// Bateria 15: 1875-1999

// === 🔥 ADVANCED UTILITY FUNCTIONS (NOWE) ===

/**
 * @brief Pobiera indeks baterii z adresu rejestru
 * @param address Adres rejestru
 * @return Indeks baterii (0-15) lub -1 jeśli nieprawidłowy
 */
int getBatteryIndexFromRegisterAddress(uint16_t address);

/**
 * @brief Pobiera offset rejestru w ramach baterii
 * @param address Adres rejestru
 * @return Offset rejestru (0-124) lub -1 jeśli nieprawidłowy
 */
int getRegisterOffsetFromAddress(uint16_t address);

/**
 * @brief Pobiera opis rejestru na podstawie offsetu
 * @param offset Offset rejestru (0-124)
 * @return Opis rejestru
 */
const char* getRegisterDescription(int offset);

/**
 * @brief Pobiera jednostkę rejestru na podstawie offsetu
 * @param offset Offset rejestru (0-124)
 * @return Jednostka rejestru
 */
const char* getRegisterUnit(int offset);

/**
 * @brief Sprawdza czy rejestr jest typu read-only
 * @param offset Offset rejestru (0-124)
 * @return true jeśli rejestr jest read-only
 */
bool isRegisterReadOnly(int offset);

// === 🔥 STATISTICS & MONITORING (NOWE) ===

/**
 * @brief Resetuje statystyki Modbus
 */
void resetModbusStatistics();

/**
 * @brief Pobiera referencję do strukturi statystyk
 * @return Referencja do statystyk
 */
const ModbusStats& getModbusStatistics();

/**
 * @brief Sprawdza czy serwer Modbus jest aktywny
 * @return true jeśli serwer jest aktywny
 */
bool isModbusServerActive();

/**
 * @brief Sprawdza czy są podłączeni klienci
 * @return true jeśli są podłączeni klienci
 */
bool hasConnectedClients();

/**
 * @brief Pobiera czas ostatniego zapytania Modbus
 * @return Czas w ms od uruchomienia systemu
 */
unsigned long getLastModbusRequestTime();

// === 🔥 REGISTER VALIDATION (NOWE) ===

/**
 * @brief Waliduje zakres adresów rejestrów
 * @param startAddress Adres początkowy
 * @param count Liczba rejestrów
 * @return true jeśli zakres jest prawidłowy
 */
bool validateRegisterRange(uint16_t startAddress, uint16_t count);

/**
 * @brief Waliduje wartość rejestru dla danego offsetu
 * @param offset Offset rejestru
 * @param value Wartość do walidacji
 * @return true jeśli wartość jest prawidłowa
 */
bool validateRegisterValue(int offset, uint16_t value);

// === 🔥 MODBUS FRAME UTILITIES (NOWE) ===

/**
 * @brief Parsuje nagłówek MBAP
 * @param buffer Bufor z danymi
 * @param mbap Struktura do wypełnienia
 * @return true jeśli parsowanie się powiodło
 */
bool parseMBAPHeader(uint8_t* buffer, MBAPHeader& mbap);

/**
 * @brief Tworzy nagłówek MBAP
 * @param buffer Bufor docelowy
 * @param transactionId ID transakcji
 * @param length Długość PDU
 * @param slaveId ID slave
 * @return Liczba zapisanych bajtów
 */
int createMBAPHeader(uint8_t* buffer, uint16_t transactionId, uint16_t length, uint8_t slaveId);

/**
 * @brief Pobiera nazwę funkcji Modbus
 * @param functionCode Kod funkcji
 * @return Nazwa funkcji
 */
const char* getModbusFunctionName(uint8_t functionCode);

/**
 * @brief Pobiera nazwę wyjątku Modbus
 * @param exceptionCode Kod wyjątku
 * @return Nazwa wyjątku
 */
const char* getModbusExceptionName(uint8_t exceptionCode);

// === 🔥 BACKWARD COMPATIBILITY (z v3.0.0) ===
// Aliasy dla zachowania kompatybilności z oryginalnym kodem
#define MODBUS_MAX_HOLDING_REGISTERS MODBUS_MAX_HOLDING_REGISTERS
#define MODBUS_SLAVE_ID MODBUS_SLAVE_ID

#endif // MODBUS_TCP_H