void ModbusTCPServer::resetStatistics() {
    requestCounter = 0;
    responseCounter = 0;
    errorCounter = 0;
    lastRequestTime = 0;
}

// ================================
// === ModbusRegisterMapper IMPLEMENTATION ===
// ================================

ModbusRegisterMapper::ModbusRegisterMapper(BMSManager* manager, uint16_t* holdingRegisters) 
    : bmsManager(manager), registers(holdingRegisters) {
}

void ModbusRegisterMapper::mapAllBMSToRegisters() {
    for (uint8_t i = 0; i < bmsManager->getActiveBmsCount(); i++) {
        mapBMSToRegisters(i);
    }
}

void ModbusRegisterMapper::mapBMSToRegisters(uint8_t bmsIndex) {
    BMSData* bms = bmsManager->getBMSData(bmsIndex);
    if (!bms) return;
    
    mapFrame190ToRegisters(bmsIndex, bms);
    mapFrame190FlagsToRegisters(bmsIndex, bms);
    mapFrame290ToRegisters(bmsIndex, bms);
    mapFrame310ToRegisters(bmsIndex, bms);
    mapFrame390ToRegisters(bmsIndex, bms);
    mapFrame410ToRegisters(bmsIndex, bms);
    mapFrame510ToRegisters(bmsIndex, bms);
    mapFrame490ToRegisters(bmsIndex, bms);
    mapErrorMapsToRegisters(bmsIndex, bms);
    mapFrame710ToRegisters(bmsIndex, bms);
}

void ModbusRegisterMapper::mapFrame190ToRegisters(uint8_t bmsIndex, const BMSData* bms) {
    uint16_t baseAddr = getBaseAddress(bmsIndex) + MODBUS_BASE_FRAME_190;
    
    setFloatRegister(baseAddr + 0, bms->batteryVoltage, 1000.0f);      // mV
    setFloatRegister(baseAddr + 1, bms->batteryCurrent, 1000.0f);      // mA (signed)
    setFloatRegister(baseAddr + 2, bms->remainingEnergy, 100.0f);      // 0.01kWh
    setFloatRegister(baseAddr + 3, bms->soc, 10.0f);                   // 0.1%
    
    // Rejestry 4-9 zarezerwowane dla przyszłych rozszerzeń Frame 190
    for (int i = 4; i < 10; i++) {
        setRegister(baseAddr + i, 0);
    }
}

void ModbusRegisterMapper::mapFrame190FlagsToRegisters(uint8_t bmsIndex, const BMSData* bms) {
    uint16_t baseAddr = getBaseAddress(bmsIndex) + MODBUS_BASE_FRAME_190_FLAGS;
    
    setBoolRegister(baseAddr + 0, bms->masterError);
    setBoolRegister(baseAddr + 1, bms->cellVoltageError);
    setBoolRegister(baseAddr + 2, bms->cellTempMinError);
    setBoolRegister(baseAddr + 3, bms->cellTempMaxError);
    setBoolRegister(baseAddr + 4, bms->cellVoltageMinError);
    setBoolRegister(baseAddr + 5, bms->cellVoltageMaxError);
    setBoolRegister(baseAddr + 6, bms->systemShutdown);
    setBoolRegister(baseAddr + 7, bms->ibbVoltageSupplyError);
    
    // Rejestry 8-9 zarezerwowane
    for (int i = 8; i < 10; i++) {
        setRegister(baseAddr + i, 0);
    }
}

void ModbusRegisterMapper::mapFrame290ToRegisters(uint8_t bmsIndex, const BMSData* bms) {
    uint16_t baseAddr = getBaseAddress(bmsIndex) + MODBUS_BASE_FRAME_290;
    
    setFloatRegister(baseAddr + 0, bms->cellMinVoltage, 1000.0f);      // mV
    setFloatRegister(baseAddr + 1, bms->cellMeanVoltage, 1000.0f);     // mV
    setRegister(baseAddr + 2, bms->minVoltageBlock);
    setRegister(baseAddr + 3, bms->minVoltageCell);
    setRegister(baseAddr + 4, bms->minVoltageString);
    setRegister(baseAddr + 5, bms->balancingTempMax);
    
    // Rejestry 6-9 zarezerwowane
    for (int i = 6; i < 10; i++) {
        setRegister(baseAddr + i, 0);
    }
}

void ModbusRegisterMapper::mapFrame310ToRegisters(uint8_t bmsIndex, const BMSData* bms) {
    uint16_t baseAddr = getBaseAddress(bmsIndex) + MODBUS_BASE_FRAME_310;
    
    setFloatRegister(baseAddr + 0, bms->soh, 10.0f);                   // 0.1%
    setFloatRegister(baseAddr + 1, bms->cellVoltage, 1000.0f);         // mV
    setFloatRegister(baseAddr + 2, bms->cellTemperature, 10.0f);       // 0.1°C
    setFloatRegister(baseAddr + 3, bms->dcir, 10.0f);                  // 0.1mΩ
    setBoolRegister(baseAddr + 4, bms->nonEqualStringsRamp);
    setBoolRegister(baseAddr + 5, bms->dynamicLimitationTimer);
    setBoolRegister(baseAddr + 6, bms->overcurrentTimer);
    setRegister(baseAddr + 7, bms->channelMultiplexor);
    
    // Rejestry 8-9 zarezerwowane
    for (int i = 8; i < 10; i++) {
        setRegister(baseAddr + i, 0);
    }
}

void ModbusRegisterMapper::mapFrame390ToRegisters(uint8_t bmsIndex, const BMSData* bms) {
    uint16_t baseAddr = getBaseAddress(bmsIndex) + MODBUS_BASE_FRAME_390;
    
    setFloatRegister(baseAddr + 0, bms->cellMaxVoltage, 1000.0f);      // mV
    setFloatRegister(baseAddr + 1, bms->cellVoltageDelta, 1000.0f);    // mV
    setRegister(baseAddr + 2, bms->maxVoltageBlock);
    setRegister(baseAddr + 3, bms->maxVoltageCell);
    setRegister(baseAddr + 4, bms->maxVoltageString);
    setRegister(baseAddr + 5, bms->afeTemperatureMax);
    
    // Rejestry 6-9 zarezerwowane
    for (int i = 6; i < 10; i++) {
        setRegister(baseAddr + i, 0);
    }
}

void ModbusRegisterMapper::mapFrame410ToRegisters(uint8_t bmsIndex, const BMSData* bms) {
    uint16_t baseAddr = getBaseAddress(bmsIndex) + MODBUS_BASE_FRAME_410;
    
    setFloatRegister(baseAddr + 0, bms->cellMaxTemperature, 10.0f);    // 0.1°C
    setFloatRegister(baseAddr + 1, bms->cellTempDelta, 10.0f);         // 0.1°C
    setRegister(baseAddr + 2, bms->maxTempString);
    setRegister(baseAddr + 3, bms->maxTempBlock);
    setRegister(baseAddr + 4, bms->maxTempSensor);
    setBoolRegister(baseAddr + 5, bms->readyToCharge);
    setBoolRegister(baseAddr + 6, bms->readyToDischarge);
    
    // Rejestry 7-9 zarezerwowane
    for (int i = 7; i < 10; i++) {
        setRegister(baseAddr + i, 0);
    }
}

void ModbusRegisterMapper::mapFrame510ToRegisters(uint8_t bmsIndex, const BMSData* bms) {
    uint16_t baseAddr = getBaseAddress(bmsIndex) + MODBUS_BASE_FRAME_510;
    
    setFloatRegister(baseAddr + 0, bms->dccl, 1000.0f);               // mA
    setFloatRegister(baseAddr + 1, bms->ddcl, 1000.0f);               // mA
    setBoolRegister(baseAddr + 2, bms->input_IN01);
    setBoolRegister(baseAddr + 3, bms->input_IN02);
    setBoolRegister(baseAddr + 4, bms->relay_R1);
    setBoolRegister(baseAddr + 5, bms->relay_R2);
    setBoolRegister(baseAddr + 6, bms->relay_AUX1);
    setBoolRegister(baseAddr + 7, bms->relay_AUX2);
    setBoolRegister(baseAddr + 8, bms->relay_AUX3);
    setBoolRegister(baseAddr + 9, bms->relay_AUX4);
}

void ModbusRegisterMapper::mapFrame490ToRegisters(uint8_t bmsIndex, const BMSData* bms) {
    uint16_t baseAddr = getBaseAddress(bmsIndex) + MODBUS_BASE_FRAME_490;
    
    // Dane multipleksera - aktualne
    setRegister(baseAddr + 0, bms->mux490Type);
    setRegister(baseAddr + 1, bms->mux490Value);
    
    // Konkretne parametry z multipleksera
    setRegister(baseAddr + 2, bms->serialNumber0);                     // Serial number low
    setRegister(baseAddr + 3, bms->serialNumber1);                     // Serial number high
    setRegister(baseAddr + 4, bms->hwVersion0);                        // HW version low
    setRegister(baseAddr + 5, bms->hwVersion1);                        // HW version high
    setRegister(baseAddr + 6, bms->swVersion0);                        // SW version low
    setRegister(baseAddr + 7, bms->swVersion1);                        // SW version high
    setFloatRegister(baseAddr + 8, bms->factoryEnergy, 100.0f);        // 0.01kWh
    setFloatRegister(baseAddr + 9, bms->designCapacity, 1000.0f);      // mAh
    setFloatRegister(baseAddr + 10, bms->inletTemperature, 10.0f);     // 0.1°C
    setFloatRegister(baseAddr + 11, bms->outletTemperature, 10.0f);    // 0.1°C
    setRegister(baseAddr + 12, bms->humidity);                         // %
    setRegister(baseAddr + 13, bms->timeToFullCharge);                 // min
    setRegister(baseAddr + 14, bms->timeToFullDischarge);              // min
    setRegister(baseAddr + 15, bms->batteryCycles);                    // cycles
    setFloatRegister(baseAddr + 16, bms->chargeEnergy0, 100.0f);       // 0.01kWh
    setFloatRegister(baseAddr + 17, bms->chargeEnergy1, 100.0f);       // 0.01kWh
    setFloatRegister(baseAddr + 18, bms->dischargeEnergy0, 100.0f);    // 0.01kWh
    setFloatRegister(baseAddr + 19, bms->dischar/*
 * ESP32S3 CAN to Modbus TCP Bridge - Implementacja Serwera Modbus TCP
 * 
 * VERSION: v4.0.0 - MODULAR ARCHITECTURE
 * DATE: 2025-08-12
 * STATUS: ✅ READY FOR COMPILATION
 * 
 * OPIS: Implementacja klasy ModbusTCPServer i ModbusRegisterMapper
 */

#include "modbus_tcp.h"
#include "config.h"

// ================================
// === REGISTER MAPPING TABLE ===
// ================================

const RegisterMapping BMS_REGISTER_MAP[] = {
    // Frame 190 - Basic Data (offsets 0-9)
    {0, "Battery Voltage", "mV", 1000.0f, true, "Battery voltage in millivolts"},
    {1, "Battery Current", "mA", 1000.0f, true, "Battery current in milliamps (signed)"},
    {2, "Remaining Energy", "0.01kWh", 100.0f, true, "Remaining energy in 0.01kWh units"},
    {3, "State of Charge", "0.1%", 10.0f, true, "State of charge in 0.1% units"},
    {4, "Reserved 190-4", "", 1.0f, true, "Reserved register"},
    {5, "Reserved 190-5", "", 1.0f, true, "Reserved register"},
    {6, "Reserved 190-6", "", 1.0f, true, "Reserved register"},
    {7, "Reserved 190-7", "", 1.0f, true, "Reserved register"},
    {8, "Reserved 190-8", "", 1.0f, true, "Reserved register"},
    {9, "Reserved 190-9", "", 1.0f, true, "Reserved register"},
    
    // Frame 190 Flags (offsets 10-19)
    {10, "Master Error", "", 1.0f, true, "Master error flag"},
    {11, "Cell Voltage Error", "", 1.0f, true, "Cell voltage error flag"},
    {12, "Cell Temp Min Error", "", 1.0f, true, "Cell minimum temperature error"},
    {13, "Cell Temp Max Error", "", 1.0f, true, "Cell maximum temperature error"},
    {14, "Cell Voltage Min Error", "", 1.0f, true, "Cell minimum voltage error"},
    {15, "Cell Voltage Max Error", "", 1.0f, true, "Cell maximum voltage error"},
    {16, "System Shutdown", "", 1.0f, true, "System shutdown flag"},
    {17, "IBB Voltage Supply Error", "", 1.0f, true, "IBB voltage supply error"},
    {18, "Reserved Flag-8", "", 1.0f, true, "Reserved error flag"},
    {19, "Reserved Flag-9", "", 1.0f, true, "Reserved error flag"},
    
    // Frame 290 - Cell Voltages (offsets 20-29)
    {20, "Cell Min Voltage", "mV", 1000.0f, true, "Minimum cell voltage in millivolts"},
    {21, "Cell Mean Voltage", "mV", 1000.0f, true, "Mean cell voltage in millivolts"},
    {22, "Min Voltage Block", "", 1.0f, true, "Block number with minimum voltage"},
    {23, "Min Voltage Cell", "", 1.0f, true, "Cell number with minimum voltage"},
    {24, "Min Voltage String", "", 1.0f, true, "String number with minimum voltage"},
    {25, "Balancing Temp Max", "°C", 1.0f, true, "Maximum balancing temperature"},
    {26, "Reserved 290-6", "", 1.0f, true, "Reserved register"},
    {27, "Reserved 290-7", "", 1.0f, true, "Reserved register"},
    {28, "Reserved 290-8", "", 1.0f, true, "Reserved register"},
    {29, "Reserved 290-9", "", 1.0f, true, "Reserved register"},
    
    // Frame 310 - SOH, Temperature, Impedance (offsets 30-39)
    {30, "State of Health", "0.1%", 10.0f, true, "State of health in 0.1% units"},
    {31, "Cell Voltage", "mV", 1000.0f, true, "Cell voltage in millivolts"},
    {32, "Cell Temperature", "0.1°C", 10.0f, true, "Cell temperature in 0.1°C units"},
    {33, "DC Internal Resistance", "0.1mΩ", 10.0f, true, "DC internal resistance in 0.1mΩ"},
    {34, "Non Equal Strings Ramp", "", 1.0f, true, "Non equal strings ramp flag"},
    {35, "Dynamic Limitation Timer", "", 1.0f, true, "Dynamic limitation timer flag"},
    {36, "Overcurrent Timer", "", 1.0f, true, "Overcurrent timer flag"},
    {37, "Channel Multiplexor", "", 1.0f, true, "Channel multiplexor value"},
    {38, "Reserved 310-8", "", 1.0f, true, "Reserved register"},
    {39, "Reserved 310-9", "", 1.0f, true, "Reserved register"}
};

// ================================
// === ModbusTCPServer IMPLEMENTATION ===
// ================================

ModbusTCPServer::ModbusTCPServer(BMSManager* manager) 
    : bmsManager(manager), initialized(false), lastRequestTime(0), 
      requestCounter(0), responseCounter(0), errorCounter(0) {
    
    server = new WiFiServer(MODBUS_TCP_PORT);
    holdingRegisters = new uint16_t[MODBUS_MAX_HOLDING_REGISTERS];
    
    // Inicjalizacja rejestrów na 0
    memset(holdingRegisters, 0, MODBUS_MAX_HOLDING_REGISTERS * sizeof(uint16_t));
    
    // Inicjalizacja tablicy klientów
    for (int i = 0; i < MODBUS_MAX_CLIENTS; i++) {
        clients[i] = WiFiClient();
    }
}

ModbusTCPServer::~ModbusTCPServer() {
    shutdown();
    if (server) delete server;
    if (holdingRegisters) delete[] holdingRegisters;
}

bool ModbusTCPServer::initialize() {
    Serial.println("🔧 Initializing Modbus TCP Server...");
    
    if (!WiFi.isConnected()) {
        Serial.println("❌ WiFi not connected - cannot start Modbus TCP server");
        return false;
    }
    
    server->begin();
    Serial.printf("✅ Modbus TCP server started on port %d\n", MODBUS_TCP_PORT);
    Serial.printf("🎯 Server IP: %s:%d\n", WiFi.localIP().toString().c_str(), MODBUS_TCP_PORT);
    Serial.printf("📊 Registers available: %d (16 BMS × 125 registers each)\n", MODBUS_MAX_HOLDING_REGISTERS);
    
    // Inicjalizacja mappera rejestrów
    ModbusRegisterMapper mapper(bmsManager, holdingRegisters);
    mapper.mapAllBMSToRegisters();
    
    initialized = true;
    
    Serial.println("🚀 Modbus TCP server ready for connections!");
    return true;
}

void ModbusTCPServer::process() {
    if (!initialized) return;
    
    // Aktualizuj rejestry z najnowszymi danymi BMS
    updateHoldingRegisters();
    
    // Sprawdź nowych klientów
    WiFiClient newClient = server->available();
    if (newClient) {
        // Znajdź wolne miejsce w tablicy klientów
        bool clientAdded = false;
        for (int i = 0; i < MODBUS_MAX_CLIENTS; i++) {
            if (!clients[i] || !clients[i].connected()) {
                clients[i] = newClient;
                clientAdded = true;
                Serial.printf("🔗 New Modbus TCP client connected: %s (slot %d)\n", 
                    newClient.remoteIP().toString().c_str(), i);
                break;
            }
        }
        
        if (!clientAdded) {
            Serial.println("⚠️ Maximum number of Modbus clients reached - rejecting connection");
            newClient.stop();
        }
    }
    
    // Obsłuż istniejących klientów
    for (int i = 0; i < MODBUS_MAX_CLIENTS; i++) {
        if (clients[i] && clients[i].connected()) {
            handleClient(clients[i]);
        } else if (clients[i]) {
            // Klient się rozłączył
            Serial.printf("📴 Modbus client disconnected (slot %d)\n", i);
            clients[i] = WiFiClient();
        }
    }
}

void ModbusTCPServer::handleClient(WiFiClient& client) {
    if (!client.available()) return;
    
    uint8_t buffer[256];
    size_t bytesRead = client.readBytes(buffer, sizeof(buffer));
    
    if (bytesRead < 7) { // Minimalna długość ramki Modbus TCP (MBAP header)
        Serial.printf("⚠️ Modbus request too short: %d bytes\n", bytesRead);
        errorCounter++;
        return;
    }
    
    if (DEBUG_MODBUS_REQUESTS) {
        Serial.printf("📥 Modbus Request: %s\n", formatModbusFrame(buffer, bytesRead, true).c_str());
    }
    
    processModbusRequest(client, buffer, bytesRead);
}

void ModbusTCPServer::processModbusRequest(WiFiClient& client, uint8_t* request, size_t length) {
    requestCounter++;
    lastRequestTime = millis();
    
    // Parsuj nagłówek MBAP
    MBAPHeader mbap;
    if (!parseMBAPHeader(request, mbap)) {
        Serial.println("❌ Invalid MBAP header");
        errorCounter++;
        return;
    }
    
    // Sprawdź Protocol ID
    if (mbap.protocolId != 0x0000) {
        Serial.printf("❌ Invalid protocol ID: 0x%04X (expected 0x0000)\n", mbap.protocolId);
        sendModbusException(client, mbap.transactionId, 0, MODBUS_EXCEPTION_ILLEGAL_FUNCTION);
        errorCounter++;
        return;
    }
    
    // Sprawdź Slave ID
    if (mbap.slaveId != MODBUS_SLAVE_ID) {
        Serial.printf("❌ Invalid slave ID: %d (expected %d)\n", mbap.slaveId, MODBUS_SLAVE_ID);
        sendModbusException(client, mbap.transactionId, 0, MODBUS_EXCEPTION_ILLEGAL_DATA_ADDRESS);
        errorCounter++;
        return;
    }
    
    // Parsuj PDU na podstawie function code
    uint8_t functionCode = request[7]; // Pierwszy bajt PDU
    
    switch (functionCode) {
        case MODBUS_FUNC_READ_HOLDING_REGISTERS: {
            if (length < 12) { // MBAP + Function Code + Address + Count
                Serial.println("⚠️ Read Holding Registers request too short");
                sendModbusException(client, mbap.transactionId, functionCode, MODBUS_EXCEPTION_ILLEGAL_DATA_VALUE);
                errorCounter++;
                return;
            }
            
            ModbusReadPDU pdu;
            pdu.functionCode = functionCode;
            pdu.startAddress = bytesToUint16BE(request[8], request[9]);
            pdu.registerCount = bytesToUint16BE(request[10], request[11]);
            
            handleReadHoldingRegisters(client, mbap, pdu);
            break;
        }
        
        case MODBUS_FUNC_WRITE_SINGLE_REGISTER: {
            if (!FEATURE_MODBUS_WRITE) {
                sendModbusException(client, mbap.transactionId, functionCode, MODBUS_EXCEPTION_ILLEGAL_FUNCTION);
                errorCounter++;
                return;
            }
            
            if (length < 12) {
                Serial.println("⚠️ Write Single Register request too short");
                sendModbusException(client, mbap.transactionId, functionCode, MODBUS_EXCEPTION_ILLEGAL_DATA_VALUE);
                errorCounter++;
                return;
            }
            
            ModbusWritePDU pdu;
            pdu.functionCode = functionCode;
            pdu.registerAddress = bytesToUint16BE(request[8], request[9]);
            pdu.registerValue = bytesToUint16BE(request[10], request[11]);
            
            handleWriteSingleRegister(client, mbap, pdu);
            break;
        }
        
        case MODBUS_FUNC_WRITE_MULTIPLE_REGISTERS: {
            if (!FEATURE_MODBUS_WRITE) {
                sendModbusException(client, mbap.transactionId, functionCode, MODBUS_EXCEPTION_ILLEGAL_FUNCTION);
                errorCounter++;
                return;
            }
            
            handleWriteMultipleRegisters(client, mbap, request + 7, length - 7);
            break;
        }
        
        default:
            Serial.printf("❌ Unsupported function code: 0x%02X\n", functionCode);
            sendModbusException(client, mbap.transactionId, functionCode, MODBUS_EXCEPTION_ILLEGAL_FUNCTION);
            errorCounter++;
            break;
    }
}

void ModbusTCPServer::handleReadHoldingRegisters(WiFiClient& client, MBAPHeader& mbap, ModbusReadPDU& pdu) {
    // Walidacja adresu i liczby rejestrów
    if (!validateAddress(pdu.startAddress, pdu.registerCount)) {
        Serial.printf("❌ Invalid address range: %d-%d\n", pdu.startAddress, pdu.startAddress + pdu.registerCount - 1);
        sendModbusException(client, mbap.transactionId, pdu.functionCode, MODBUS_EXCEPTION_ILLEGAL_DATA_ADDRESS);
        errorCounter++;
        return;
    }
    
    if (pdu.registerCount == 0 || pdu.registerCount > 125) {
        Serial.printf("❌ Invalid register count: %d\n", pdu.registerCount);
        sendModbusException(client, mbap.transactionId, pdu.functionCode, MODBUS_EXCEPTION_ILLEGAL_DATA_VALUE);
        errorCounter++;
        return;
    }
    
    // Przygotuj odpowiedź
    uint8_t responseBuffer[256];
    size_t responseLength = 0;
    
    // MBAP Header
    responseLength += createMBAPHeader(responseBuffer, mbap.transactionId, 3 + pdu.registerCount * 2, mbap.slaveId);
    
    // PDU
    responseBuffer[responseLength++] = pdu.functionCode;  // Function Code
    responseBuffer[responseLength++] = pdu.registerCount * 2;  // Byte Count
    
    // Register values
    for (uint16_t i = 0; i < pdu.registerCount; i++) {
        uint16_t registerValue = holdingRegisters[pdu.startAddress + i];
        uint8_t highByte, lowByte;
        uint16ToBytes(registerValue, highByte, lowByte);
        responseBuffer[responseLength++] = highByte;
        responseBuffer[responseLength++] = lowByte;
    }
    
    // Wyślij odpowiedź
    sendModbusResponse(client, responseBuffer, responseLength);
    
    if (DEBUG_MODBUS_REQUESTS) {
        Serial.printf("📊 Read %d registers from address %d\n", pdu.registerCount, pdu.startAddress);
    }
}

void ModbusTCPServer::handleWriteSingleRegister(WiFiClient& client, MBAPHeader& mbap, ModbusWritePDU& pdu) {
    // Walidacja adresu
    if (!validateAddress(pdu.registerAddress, 1)) {
        sendModbusException(client, mbap.transactionId, pdu.functionCode, MODBUS_EXCEPTION_ILLEGAL_DATA_ADDRESS);
        errorCounter++;
        return;
    }
    
    // Sprawdź czy rejestr jest zapisywalny (większość rejestrów BMS jest tylko do odczytu)
    const RegisterMapping* mapping = getRegisterMapping(pdu.registerAddress / MODBUS_REGISTERS_PER_BATTERY, 
                                                       pdu.registerAddress % MODBUS_REGISTERS_PER_BATTERY);
    if (mapping && mapping->readOnly) {
        Serial.printf("❌ Attempt to write to read-only register %d (%s)\n", pdu.registerAddress, mapping->name);
        sendModbusException(client, mbap.transactionId, pdu.functionCode, MODBUS_EXCEPTION_ILLEGAL_DATA_ADDRESS);
        errorCounter++;
        return;
    }
    
    // Zapisz wartość
    holdingRegisters[pdu.registerAddress] = pdu.registerValue;
    
    // Echo response (taka sama jak request)
    uint8_t responseBuffer[12];
    size_t responseLength = 0;
    
    responseLength += createMBAPHeader(responseBuffer, mbap.transactionId, 6, mbap.slaveId);
    responseBuffer[responseLength++] = pdu.functionCode;
    uint8_t highByte, lowByte;
    uint16ToBytes(pdu.registerAddress, highByte, lowByte);
    responseBuffer[responseLength++] = highByte;
    responseBuffer[responseLength++] = lowByte;
    uint16ToBytes(pdu.registerValue, highByte, lowByte);
    responseBuffer[responseLength++] = highByte;
    responseBuffer[responseLength++] = lowByte;
    
    sendModbusResponse(client, responseBuffer, responseLength);
    
    Serial.printf("📝 Written register %d = %d\n", pdu.registerAddress, pdu.registerValue);
}

void ModbusTCPServer::handleWriteMultipleRegisters(WiFiClient& client, MBAPHeader& mbap, uint8_t* data, size_t dataLength) {
    // TODO: Implementacja zapisu wielu rejestrów (jeśli potrzebne)
    Serial.println("⚠️ Write Multiple Registers not implemented");
    sendModbusException(client, mbap.transactionId, MODBUS_FUNC_WRITE_MULTIPLE_REGISTERS, MODBUS_EXCEPTION_ILLEGAL_FUNCTION);
    errorCounter++;
}

void ModbusTCPServer::sendModbusResponse(WiFiClient& client, uint8_t* response, size_t length) {
    client.write(response, length);
    responseCounter++;
    
    SystemStats* stats = bmsManager->getSystemStats();
    stats->modbusResponses++;
    
    if (DEBUG_MODBUS_REQUESTS) {
        Serial.printf("📤 Modbus Response sent: %d bytes\n", length);
    }
}

void ModbusTCPServer::sendModbusException(WiFiClient& client, uint16_t transactionId, uint8_t functionCode, ModbusExceptionCode exceptionCode) {
    uint8_t responseBuffer[9];
    size_t responseLength = 0;
    
    // MBAP Header
    responseLength += createMBAPHeader(responseBuffer, transactionId, 3, MODBUS_SLAVE_ID);
    
    // Exception PDU
    responseBuffer[responseLength++] = functionCode | 0x80;  // Function Code + 0x80 dla exception
    responseBuffer[responseLength++] = (uint8_t)exceptionCode;
    
    client.write(responseBuffer, responseLength);
    
    Serial.printf("❌ Modbus Exception: Function 0x%02X, Code 0x%02X (%s)\n", 
        functionCode, exceptionCode, getModbusExceptionName(exceptionCode));
    
    errorCounter++;
    SystemStats* stats = bmsManager->getSystemStats();
    stats->modbusErrors++;
}

void ModbusTCPServer::updateHoldingRegisters() {
    ModbusRegisterMapper mapper(bmsManager, holdingRegisters);
    mapper.mapAllBMSToRegisters();
}

void ModbusTCPServer::updateBMSRegisters(uint8_t bmsIndex) {
    ModbusRegisterMapper mapper(bmsManager, holdingRegisters);
    mapper.mapBMSToRegisters(bmsIndex);
}

bool ModbusTCPServer::validateAddress(uint16_t address, uint16_t count) {
    return (address + count) <= MODBUS_MAX_HOLDING_REGISTERS;
}

bool ModbusTCPServer::validateFunctionCode(uint8_t functionCode) {
    switch (functionCode) {
        case MODBUS_FUNC_READ_HOLDING_REGISTERS:
            return true;
        case MODBUS_FUNC_WRITE_SINGLE_REGISTER:
        case MODBUS_FUNC_WRITE_MULTIPLE_REGISTERS:
            return FEATURE_MODBUS_WRITE;
        default:
            return false;
    }
}

uint16_t ModbusTCPServer::floatToRegister(float value, float scale) {
    int32_t scaledValue = (int32_t)(value * scale);
    return (uint16_t)constrain(scaledValue, 0, 65535);
}

uint16_t ModbusTCPServer::boolToRegister(bool value) {
    return value ? 1 : 0;
}

float ModbusTCPServer::registerToFloat(uint16_t regValue, float scale) {
    return regValue / scale;
}

bool ModbusTCPServer::registerToBool(uint16_t regValue) {
    return regValue != 0;
}

uint8_t ModbusTCPServer::getActiveClientCount() const {
    uint8_t count = 0;
    for (int i = 0; i < MODBUS_MAX_CLIENTS; i++) {
        if (clients[i] && clients[i].connected()) {
            count++;
        }
    }
    return count;
}

uint16_t ModbusTCPServer::getHoldingRegister(uint16_t address) {
    if (address >= MODBUS_MAX_HOLDING_REGISTERS) return 0;
    return holdingRegisters[address];
}

bool ModbusTCPServer::setHoldingRegister(uint16_t address, uint16_t value) {
    if (address >= MODBUS_MAX_HOLDING_REGISTERS) return false;
    holdingRegisters[address] = value;
    return true;
}

void ModbusTCPServer::shutdown() {
    if (initialized) {
        // Rozłącz wszystkich klientów
        for (int i = 0; i < MODBUS_MAX_CLIENTS; i++) {
            if (clients[i] && clients[i].connected()) {
                clients[i].stop();
            }
        }
        
        server->stop();
        initialized = false;
        Serial.println("📴 Modbus TCP server stopped");
    }
}

void ModbusTCPServer::printStatistics() {
    Serial.println("📊 MODBUS TCP STATISTICS:");
    Serial.printf("   Requests: %lu\n", requestCounter);
    Serial.printf("   Responses: %lu\n", responseCounter);
    Serial.printf("   Errors: %lu\n", errorCounter);
    Serial.printf("   Active clients: %d/%d\n", getActiveClientCount(), MODBUS_MAX_CLIENTS);
    Serial.printf("   Last request: %lu ms ago\n", lastRequestTime > 0 ? millis() - lastRequestTime : 0);
    Serial.printf("   Server status: %s\n", initialized ? "Running" : "Stopped");
}

void ModbusTCPServer::resetStatistics() {
    requestCounter = 0;
    responseCounter = 0;
    errorCounter = 0;
    lastRequestTime = 0;
}