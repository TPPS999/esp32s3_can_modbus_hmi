/*
 * ESP32S3 CAN to Modbus TCP Bridge - Serwer Modbus TCP
 * 
 * VERSION: v4.0.0 - MODULAR ARCHITECTURE
 * DATE: 2025-08-12
 * STATUS: ✅ READY FOR COMPILATION
 * 
 * OPIS: Moduł implementujący serwer Modbus TCP dla dostępu do danych BMS
 */

#ifndef MODBUS_TCP_H
#define MODBUS_TCP_H

#include "config.h"
#include "bms_data.h"
#include <WiFi.h>
#include <WiFiServer.h>
#include <WiFiClient.h>

// ================================
// === MODBUS TCP PROTOCOL ===
// ================================

/**
 * @brief Struktura nagłówka MBAP (Modbus Application Protocol)
 */
struct MBAPHeader {
    uint16_t transactionId;     // ID transakcji
    uint16_t protocolId;        // ID protokołu (zawsze 0x0000)
    uint16_t length;            // Długość ramki PDU + Slave ID
    uint8_t slaveId;            // ID slave (domyślnie 1)
};

/**
 * @brief Struktura PDU (Protocol Data Unit) dla funkcji Read Holding Registers
 */
struct ModbusReadPDU {
    uint8_t functionCode;       // Kod funkcji (0x03)
    uint16_t startAddress;      // Adres początkowy
    uint16_t registerCount;     // Liczba rejestrów do odczytu
};

/**
 * @brief Struktura PDU dla funkcji Write Single Register
 */
struct ModbusWritePDU {
    uint8_t functionCode;       // Kod funkcji (0x06)
    uint16_t registerAddress;   // Adres rejestru
    uint16_t registerValue;     // Wartość rejestru
};

/**
 * @brief Enum z kodami błędów Modbus
 */
enum ModbusExceptionCode {
    MODBUS_EXCEPTION_NONE = 0x00,
    MODBUS_EXCEPTION_ILLEGAL_FUNCTION = 0x01,
    MODBUS_EXCEPTION_ILLEGAL_DATA_ADDRESS = 0x02,
    MODBUS_EXCEPTION_ILLEGAL_DATA_VALUE = 0x03,
    MODBUS_EXCEPTION_SLAVE_DEVICE_FAILURE = 0x04,
    MODBUS_EXCEPTION_ACKNOWLEDGE = 0x05,
    MODBUS_EXCEPTION_SLAVE_DEVICE_BUSY = 0x06,
    MODBUS_EXCEPTION_GATEWAY_PATH_UNAVAILABLE = 0x0A,
    MODBUS_EXCEPTION_GATEWAY_TARGET_DEVICE_FAILED = 0x0B
};

// ================================
// === MODBUS TCP SERVER ===
// ================================

/**
 * @brief Klasa implementująca serwer Modbus TCP
 */
class ModbusTCPServer {
private:
    WiFiServer* server;                 // Serwer TCP
    WiFiClient clients[MODBUS_MAX_CLIENTS]; // Tablica klientów
    BMSManager* bmsManager;            // Manager danych BMS
    uint16_t* holdingRegisters;        // Tablica holding registers
    bool initialized;                  // Status inicjalizacji
    unsigned long lastRequestTime;     // Czas ostatniego zapytania
    uint32_t requestCounter;           // Licznik zapytań
    uint32_t responseCounter;          // Licznik odpowiedzi
    uint32_t errorCounter;             // Licznik błędów
    
    // Private methods
    void handleClient(WiFiClient& client);
    void processModbusRequest(WiFiClient& client, uint8_t* request, size_t length);
    void sendModbusResponse(WiFiClient& client, uint8_t* response, size_t length);
    void sendModbusException(WiFiClient& client, uint16_t transactionId, uint8_t functionCode, ModbusExceptionCode exceptionCode);
    
    // PDU handlers
    void handleReadHoldingRegisters(WiFiClient& client, MBAPHeader& mbap, ModbusReadPDU& pdu);
    void handleWriteSingleRegister(WiFiClient& client, MBAPHeader& mbap, ModbusWritePDU& pdu);
    void handleWriteMultipleRegisters(WiFiClient& client, MBAPHeader& mbap, uint8_t* data, size_t dataLength);
    
    // Data conversion
    void updateHoldingRegisters();
    void updateBMSRegisters(uint8_t bmsIndex);
    uint16_t floatToRegister(float value, float scale = 1.0f);
    uint16_t boolToRegister(bool value);
    float registerToFloat(uint16_t regValue, float scale = 1.0f);
    bool registerToBool(uint16_t regValue);
    
    // Validation
    bool validateAddress(uint16_t address, uint16_t count);
    bool validateFunctionCode(uint8_t functionCode);
    
public:
    // Constructor & Destructor
    ModbusTCPServer(BMSManager* manager);
    ~ModbusTCPServer();
    
    // Public interface
    bool initialize();
    void process();
    void shutdown();
    
    // Status
    bool isInitialized() const { return initialized; }
    uint32_t getRequestCount() const { return requestCounter; }
    uint32_t getResponseCount() const { return responseCounter; }
    uint32_t getErrorCount() const { return errorCounter; }
    unsigned long getLastRequestTime() const { return lastRequestTime; }
    uint8_t getActiveClientCount() const;
    
    // Register access
    uint16_t getHoldingRegister(uint16_t address);
    bool setHoldingRegister(uint16_t address, uint16_t value);
    uint16_t* getHoldingRegistersPtr() { return holdingRegisters; }
    
    // Statistics
    void printStatistics();
    void resetStatistics();
};

// ================================
// === REGISTER MAPPING ===
// ================================

/**
 * @brief Klasa odpowiedzialna za mapowanie danych BMS na rejestry Modbus
 */
class ModbusRegisterMapper {
private:
    BMSManager* bmsManager;
    uint16_t* registers;
    
public:
    ModbusRegisterMapper(BMSManager* manager, uint16_t* holdingRegisters);
    
    // Mapowanie danych BMS na rejestry
    void mapBMSToRegisters(uint8_t bmsIndex);
    void mapAllBMSToRegisters();
    
    // Mapowanie poszczególnych ramek
    void mapFrame190ToRegisters(uint8_t bmsIndex, const BMSData* bms);
    void mapFrame190FlagsToRegisters(uint8_t bmsIndex, const BMSData* bms);
    void mapFrame290ToRegisters(uint8_t bmsIndex, const BMSData* bms);
    void mapFrame310ToRegisters(uint8_t bmsIndex, const BMSData* bms);
    void mapFrame390ToRegisters(uint8_t bmsIndex, const BMSData* bms);
    void mapFrame410ToRegisters(uint8_t bmsIndex, const BMSData* bms);
    void mapFrame510ToRegisters(uint8_t bmsIndex, const BMSData* bms);
    void mapFrame490ToRegisters(uint8_t bmsIndex, const BMSData* bms);
    void mapErrorMapsToRegisters(uint8_t bmsIndex, const BMSData* bms);
    void mapFrame710ToRegisters(uint8_t bmsIndex, const BMSData* bms);
    
    // Utility functions
    uint16_t getBaseAddress(uint8_t bmsIndex) const;
    uint16_t getBMSRegisterAddress(uint8_t bmsIndex, uint16_t offset) const;
    bool isValidBMSAddress(uint16_t address, uint8_t& bmsIndex, uint16_t& offset) const;
    
private:
    // Helper methods
    void setRegister(uint16_t address, uint16_t value);
    void setFloatRegister(uint16_t address, float value, float scale = 1.0f);
    void setBoolRegister(uint16_t address, bool value);
    void setUint32Register(uint16_t address, uint32_t value); // Dla 32-bit values w 2 rejestrach
};

// ================================
// === UTILITY FUNCTIONS ===
// ================================

/**
 * @brief Oblicza CRC16 dla ramki Modbus RTU (nie używane w TCP, ale przydatne)
 * @param data Wskaźnik na dane
 * @param length Długość danych
 * @return Wartość CRC16
 */
uint16_t calculateModbusCRC(uint8_t* data, size_t length);

/**
 * @brief Konwertuje 16-bit value na 2 bajty big-endian
 * @param value Wartość 16-bitowa
 * @param highByte Referencja na starszy bajt
 * @param lowByte Referencja na młodszy bajt
 */
void uint16ToBytes(uint16_t value, uint8_t& highByte, uint8_t& lowByte);

/**
 * @brief Konwertuje 2 bajty big-endian na 16-bit value
 * @param highByte Starszy bajt
 * @param lowByte Młodszy bajt
 * @return Wartość 16-bitowa
 */
uint16_t bytesToUint16BE(uint8_t highByte, uint8_t lowByte);

/**
 * @brief Parsuje nagłówek MBAP z bufora
 * @param buffer Bufor z danymi
 * @param mbap Referencja na strukturę MBAP do wypełnienia
 * @return true jeśli parsing się powiódł
 */
bool parseMBAPHeader(uint8_t* buffer, MBAPHeader& mbap);

/**
 * @brief Tworzy nagłówek MBAP w buforze
 * @param buffer Bufor docelowy
 * @param transactionId ID transakcji
 * @param length Długość PDU + Slave ID
 * @param slaveId ID slave
 * @return Liczba zapisanych bajtów (zawsze 7)
 */
size_t createMBAPHeader(uint8_t* buffer, uint16_t transactionId, uint16_t length, uint8_t slaveId = MODBUS_SLAVE_ID);

/**
 * @brief Formatuje dane Modbus do wyświetlenia (debug)
 * @param data Wskaźnik na dane
 * @param length Długość danych
 * @param isRequest true jeśli to zapytanie, false jeśli odpowiedź
 * @return Sformatowany string
 */
String formatModbusFrame(uint8_t* data, size_t length, bool isRequest = true);

/**
 * @brief Pobiera opis funkcji Modbus na podstawie kodu
 * @param functionCode Kod funkcji
 * @return Opis funkcji
 */
const char* getModbusFunctionName(uint8_t functionCode);

/**
 * @brief Pobiera opis wyjątku Modbus
 * @param exceptionCode Kod wyjątku
 * @return Opis wyjątku
 */
const char* getModbusExceptionName(ModbusExceptionCode exceptionCode);

// ================================
// === REGISTER LAYOUT CONSTANTS ===
// ================================

/**
 * @brief Makra dla łatwego dostępu do rejestrów BMS
 */
#define BMS_REG_VOLTAGE(bms)              ((bms) * MODBUS_REGISTERS_PER_BATTERY + MODBUS_BASE_FRAME_190 + 0)
#define BMS_REG_CURRENT(bms)              ((bms) * MODBUS_REGISTERS_PER_BATTERY + MODBUS_BASE_FRAME_190 + 1)
#define BMS_REG_ENERGY(bms)               ((bms) * MODBUS_REGISTERS_PER_BATTERY + MODBUS_BASE_FRAME_190 + 2)
#define BMS_REG_SOC(bms)                  ((bms) * MODBUS_REGISTERS_PER_BATTERY + MODBUS_BASE_FRAME_190 + 3)
#define BMS_REG_SOH(bms)                  ((bms) * MODBUS_REGISTERS_PER_BATTERY + MODBUS_BASE_FRAME_310 + 0)
#define BMS_REG_MASTER_ERROR(bms)         ((bms) * MODBUS_REGISTERS_PER_BATTERY + MODBUS_BASE_FRAME_190_FLAGS + 0)
#define BMS_REG_READY_TO_CHARGE(bms)      ((bms) * MODBUS_REGISTERS_PER_BATTERY + MODBUS_BASE_FRAME_410 + 6)
#define BMS_REG_READY_TO_DISCHARGE(bms)   ((bms) * MODBUS_REGISTERS_PER_BATTERY + MODBUS_BASE_FRAME_410 + 7)
#define BMS_REG_COMMUNICATION_OK(bms)     ((bms) * MODBUS_REGISTERS_PER_BATTERY + MODBUS_BASE_FRAME_710 + 1)
#define BMS_REG_SERIAL_NUMBER_LOW(bms)    ((bms) * MODBUS_REGISTERS_PER_BATTERY + MODBUS_BASE_FRAME_490 + 2)
#define BMS_REG_SERIAL_NUMBER_HIGH(bms)   ((bms) * MODBUS_REGISTERS_PER_BATTERY + MODBUS_BASE_FRAME_490 + 3)

/**
 * @brief Struktura opisująca mapowanie rejestru
 */
struct RegisterMapping {
    uint16_t offset;            // Offset w ramach baterii
    const char* name;           // Nazwa rejestru
    const char* unit;           // Jednostka
    float scale;                // Skala (register_value = real_value * scale)
    bool readOnly;              // Czy tylko do odczytu
    const char* description;    // Opis rejestru
};

/**
 * @brief Tablica mapowania rejestrów (dla dokumentacji i debugowania)
 */
extern const RegisterMapping BMS_REGISTER_MAP[];

/**
 * @brief Pobiera informacje o mapowaniu rejestru
 * @param bmsIndex Indeks BMS (0-15)
 * @param offset Offset rejestru w ramach BMS (0-124)
 * @return Wskaźnik na RegisterMapping lub nullptr jeśli nie znaleziono
 */
const RegisterMapping* getRegisterMapping(uint8_t bmsIndex, uint16_t offset);

#endif // MODBUS_TCP_H