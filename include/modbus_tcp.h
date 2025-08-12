/*
 * modbus_tcp.h - ESP32S3 CAN to Modbus TCP Bridge Modbus Server
 * 
 * VERSION: v3.1.0
 * DATE: 2025-08-12
 * DESCRIPTION: Serwer Modbus TCP dla ESP32S3
 */

#ifndef MODBUS_TCP_H
#define MODBUS_TCP_H

#include "config.h"
#include "bms_data.h"
#include <WiFiServer.h>
#include <WiFiClient.h>

// === MODBUS TCP PROTOCOL CONSTANTS ===
#define MODBUS_TCP_PORT 502
#define MODBUS_SLAVE_ID 1
#define MODBUS_MAX_HOLDING_REGISTERS 2000
#define MODBUS_REGISTERS_PER_BMS 125
#define MODBUS_MAX_FRAME_SIZE 256
#define MODBUS_MBAP_HEADER_SIZE 6
#define MODBUS_PDU_HEADER_SIZE 2

// === MODBUS FUNCTION CODES ===
#define MODBUS_FC_READ_HOLDING_REGISTERS 0x03
#define MODBUS_FC_READ_INPUT_REGISTERS 0x04
#define MODBUS_FC_WRITE_SINGLE_REGISTER 0x06
#define MODBUS_FC_WRITE_MULTIPLE_REGISTERS 0x10

// === MODBUS EXCEPTION CODES ===
#define MODBUS_EXCEPTION_ILLEGAL_FUNCTION 0x01
#define MODBUS_EXCEPTION_ILLEGAL_DATA_ADDRESS 0x02
#define MODBUS_EXCEPTION_ILLEGAL_DATA_VALUE 0x03
#define MODBUS_EXCEPTION_SLAVE_DEVICE_FAILURE 0x04
#define MODBUS_EXCEPTION_ACKNOWLEDGE 0x05
#define MODBUS_EXCEPTION_SLAVE_DEVICE_BUSY 0x06
#define MODBUS_EXCEPTION_GATEWAY_PATH_UNAVAILABLE 0x0A
#define MODBUS_EXCEPTION_GATEWAY_TARGET_DEVICE_FAILED 0x0B

// === GLOBAL OBJECTS ===
extern WiFiServer modbusServer;
extern WiFiClient currentModbusClient;
extern uint16_t modbusRegisters[MODBUS_MAX_HOLDING_REGISTERS];

// === MODBUS STATISTICS ===
struct ModbusStats {
  unsigned long totalRequests;
  unsigned long successfulResponses;
  unsigned long errorResponses;
  unsigned long invalidRequests;
  unsigned long readHoldingRegisterRequests;
  unsigned long readInputRegisterRequests;
  unsigned long writeSingleRegisterRequests;
  unsigned long writeMultipleRegisterRequests;
  unsigned long lastRequestTime;
  unsigned long lastResponseTime;
  unsigned long clientConnections;
  unsigned long clientDisconnections;
  char lastError[64];
};

extern ModbusStats modbusStats;

// === FUNCTION DECLARATIONS ===

// === Initialization and Management ===
bool setupModbusTCP();
void stopModbusTCP();
bool restartModbusTCP();

// === Main Processing ===
void processModbusTCP();
void handleNewClients();
void handleClientRequests();
void checkClientConnections();

// === Request Processing ===
bool processModbusRequest(WiFiClient& client);
void handleModbusRequest(WiFiClient& client, uint8_t* buffer, size_t length);
bool validateModbusRequest(const uint8_t* buffer, size_t length);

// === Function Code Handlers ===
void handleReadHoldingRegisters(WiFiClient& client, uint16_t transactionId, 
                               uint16_t startAddress, uint16_t registerCount);
void handleReadInputRegisters(WiFiClient& client, uint16_t transactionId, 
                             uint16_t startAddress, uint16_t registerCount);
void handleWriteSingleRegister(WiFiClient& client, uint16_t transactionId, 
                              uint16_t registerAddress, uint16_t registerValue);
void handleWriteMultipleRegisters(WiFiClient& client, uint16_t transactionId, 
                                 uint16_t startAddress, uint16_t registerCount, 
                                 const uint8_t* registerData);

// === Response Generation ===
void sendModbusResponse(WiFiClient& client, uint16_t transactionId, uint8_t functionCode, 
                       const uint8_t* data, size_t dataLength);
void sendModbusException(WiFiClient& client, uint16_t transactionId, uint8_t functionCode, 
                        uint8_t exceptionCode);

// === Register Management ===
void updateModbusRegisters(uint8_t nodeId);
void updateAllModbusRegisters();
void clearModbusRegisters();
bool isValidRegisterAddress(uint16_t address, uint16_t count = 1);

// === Register Mapping ===
uint16_t getBMSBaseAddress(uint8_t nodeId);
void mapBMSDataToRegisters(uint8_t nodeId, const BMSData& bms);
void mapSystemStatusToRegisters();

// === Data Conversion ===
uint16_t floatToRegister(float value, float multiplier = 1000.0);
float registerToFloat(uint16_t reg, float divider = 1000.0);
void writeRegisterPair(uint16_t baseAddress, uint32_t value);
uint32_t readRegisterPair(uint16_t baseAddress);

// === Utility Functions ===
uint16_t calculateCRC16(const uint8_t* data, size_t length);
bool validateCRC(const uint8_t* data, size_t length);
void printModbusFrame(const uint8_t* frame, size_t length, bool isRequest = true);

// === Statistics and Diagnostics ===
void updateModbusStatistics(bool success, uint8_t functionCode = 0);
void printModbusStatistics();
void resetModbusStatistics();

// === Error Handling ===
void handleModbusError(const char* error);
void logModbusError(uint16_t transactionId, uint8_t functionCode, uint8_t exceptionCode);

// === Client Management ===
bool acceptNewClient();
void disconnectClient(WiFiClient& client);
bool isClientConnected(WiFiClient& client);
void cleanupDisconnectedClients();

// === Protocol Parsing ===
struct ModbusRequest {
  uint16_t transactionId;
  uint16_t protocolId;
  uint16_t length;
  uint8_t slaveId;
  uint8_t functionCode;
  uint16_t startAddress;
  uint16_t registerCount;
  uint16_t registerValue;
  const uint8_t* registerData;
  bool valid;
};

ModbusRequest parseModbusRequest(const uint8_t* buffer, size_t length);
bool validateModbusHeader(const ModbusRequest& request);

// === Register Definitions ===
// BMS Data Register Offsets (per BMS module, base = nodeIndex * 125)
#define REG_BATTERY_VOLTAGE         0    // Battery voltage (mV)
#define REG_BATTERY_CURRENT         1    // Battery current (mA)
#define REG_REMAINING_ENERGY        2    // Remaining energy (Wh*10)
#define REG_SOC                     3    // State of charge (%)
#define REG_SOH                     4    // State of health (0.1%)

// Error flags (registers 10-17)
#define REG_ERROR_MASTER           10    // Master error flag
#define REG_ERROR_CELL_VOLTAGE     11    // Cell voltage error
#define REG_ERROR_UNDER_VOLTAGE    12    // Under voltage error
#define REG_ERROR_OVER_VOLTAGE     13    // Over voltage error
#define REG_ERROR_CELL_IMBALANCE   14    // Cell imbalance error
#define REG_ERROR_UNDER_TEMP       15    // Under temperature error
#define REG_ERROR_OVER_TEMP        16    // Over temperature error
#define REG_ERROR_OVER_CURRENT     17    // Over current error

// Cell voltages (registers 20-25)
#define REG_MIN_CELL_VOLTAGE       20    // Min cell voltage (0.1mV)
#define REG_AVG_CELL_VOLTAGE       21    // Average cell voltage (0.1mV)
#define REG_MAX_CELL_VOLTAGE       22    // Max cell voltage (0.1mV)
#define REG_MAX_CELL_ID            23    // Max cell voltage ID
#define REG_MIN_CELL_ID            24    // Min cell voltage ID
#define REG_DELTA_CELL_VOLTAGE     25    // Delta cell voltage (0.1mV)

// Temperatures (registers 30-35)
#define REG_AVG_TEMPERATURE        30    // Average temperature (°C + 273)
#define REG_TEMPERATURE_1          31    // Temperature sensor 1 (°C + 273)
#define REG_TEMPERATURE_2          32    // Temperature sensor 2 (°C + 273)
#define REG_TEMPERATURE_3          33    // Temperature sensor 3 (°C + 273)

// Limits (registers 40-43)
#define REG_MAX_CHARGE_CURRENT     40    // Max allowed charge current (mA)
#define REG_MAX_DISCHARGE_CURRENT  41    // Max allowed discharge current (mA)
#define REG_MAX_CHARGE_VOLTAGE     42    // Max allowed charge voltage (mV)
#define REG_MAX_DISCHARGE_VOLTAGE  43    // Max allowed discharge voltage (mV)

// Power limits (registers 50-53)
#define REG_MAX_CHARGE_POWER       50    // Max charge power (W)
#define REG_MAX_DISCHARGE_POWER    51    // Max discharge power (W)
#define REG_DIGITAL_INPUTS         52    // Digital inputs state
#define REG_DIGITAL_OUTPUTS        53    // Digital outputs state

// Status flags (registers 60-63)
#define REG_GENERAL_READY          60    // General ready flag
#define REG_CHARGE_READY           61    // Charge ready flag
#define REG_COMMUNICATION_OK       62    // Communication status
#define REG_PACKETS_RECEIVED_LOW   63    // Packets received (low word)
#define REG_PACKETS_RECEIVED_HIGH  64    // Packets received (high word)

// System registers (starting at 1900)
#define REG_SYSTEM_STATUS          1900  // System status
#define REG_TOTAL_BATTERIES        1901  // Total configured batteries
#define REG_ONLINE_BATTERIES       1902  // Online batteries count
#define REG_ERROR_BATTERIES        1903  // Batteries with errors
#define REG_UPTIME_LOW             1904  // System uptime (low word)
#define REG_UPTIME_HIGH            1905  // System uptime (high word)
#define REG_CAN_FRAMES_LOW         1906  // CAN frames received (low word)
#define REG_CAN_FRAMES_HIGH        1907  // CAN frames received (high word)
#define REG_MODBUS_REQUESTS_LOW    1908  // Modbus requests (low word)
#define REG_MODBUS_REQUESTS_HIGH   1909  // Modbus requests (high word)

// Configuration registers (starting at 1950)
#define REG_FIRMWARE_VERSION       1950  // Firmware version
#define REG_DEVICE_ID              1951  // Device ID
#define REG_CAN_SPEED              1952  // CAN speed setting
#define REG_ACTIVE_BMS_COUNT       1953  // Active BMS count

// === ADVANCED FEATURES ===
void enableModbusLogging(bool enable);
void setModbusTimeout(unsigned long timeoutMs);
bool testModbusConnection();

#endif // MODBUS_TCP_H