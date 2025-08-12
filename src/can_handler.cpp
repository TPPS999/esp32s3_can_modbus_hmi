/*
 * ESP32S3 CAN to Modbus TCP Bridge - Implementacja Obsługi CAN
 * 
 * VERSION: v4.0.0 - MODULAR ARCHITECTURE
 * DATE: 2025-08-12
 * STATUS: ✅ READY FOR COMPILATION
 * 
 * OPIS: Implementacja klasy CANHandler i parserów ramek BMS
 */

#include "can_handler.h"
#include "config.h"

// ================================
// === CANHandler IMPLEMENTATION ===
// ================================

CANHandler::CANHandler(BMSManager* manager) 
    : bmsManager(manager), initialized(false), lastFrameTime(0), frameCounter(0) {
    canBus = new MCP_CAN(SPI_CS_PIN);
}

CANHandler::~CANHandler() {
    if (canBus) {
        delete canBus;
    }
}

bool CANHandler::initialize() {
    Serial.println("🔧 Configuring SPI pins for CAN Expansion Board...");
    Serial.printf("   MOSI: GPIO%d\n", SPI_MOSI_PIN);
    Serial.printf("   MISO: GPIO%d\n", SPI_MISO_PIN);
    Serial.printf("   SCK:  GPIO%d\n", SPI_SCK_PIN);
    Serial.printf("   CS:   GPIO%d\n", SPI_CS_PIN);
    
    // Konfiguracja SPI
    SPI.begin(SPI_SCK_PIN, SPI_MISO_PIN, SPI_MOSI_PIN, SPI_CS_PIN);
    delay(100);
    
    SPI.beginTransaction(SPISettings(10000000, MSBFIRST, SPI_MODE0));
    SPI.endTransaction();
    
    Serial.println("✅ SPI pins configured");
    
    // Inicjalizacja MCP2515
    if (!initializeMCP2515()) {
        Serial.println("❌ Failed to initialize MCP2515");
        return false;
    }
    
    initialized = true;
    
    Serial.printf("🎯 Monitoring BMS Node IDs: ");
    for (int i = 0; i < bmsManager->getActiveBmsCount(); i++) {
        uint8_t nodeId = bmsManager->getNodeId(i);
        Serial.printf("%d(0x%X) ", nodeId, nodeId);
    }
    Serial.println();
    
    return true;
}

bool CANHandler::initializeMCP2515() {
    Serial.println("🔧 Initializing MCP2515 CAN Controller...");
    
    // Kilka prób inicjalizacji
    for (int attempt = 1; attempt <= 3; attempt++) {
        Serial.printf("   Attempt %d/3...", attempt);
        
        if (CAN_OK == canBus->begin(CAN_SPEED)) {
            Serial.println(" ✅ SUCCESS");
            
            // Konfiguracja masek i filtrów (opcjonalne - na razie przyjmij wszystko)
            if (FEATURE_CAN_FILTERING) {
                // Można dodać filtry dla ramek BMS
                Serial.println("🔍 CAN filtering enabled (accepting all frames)");
            }
            
            Serial.println("🎯 MCP2515 ready - monitoring IFS BMS protocol");
            return true;
        }
        
        Serial.println(" ❌ FAILED");
        delay(100 * attempt); // Zwiększ opóźnienie z każdą próbą
    }
    
    Serial.println("💡 Troubleshooting tips:");
    Serial.println("   1. Check SPI wiring (CS, MOSI, MISO, SCK)");
    Serial.println("   2. Verify MCP2515 power supply (3.3V or 5V)");
    Serial.println("   3. Ensure CAN transceiver is connected");
    Serial.println("   4. Check CAN bus termination (120Ω resistors)");
    
    return false;
}

void CANHandler::process() {
    if (!initialized) return;
    
    unsigned char len = 0;
    unsigned char buf[8];
    unsigned long canId;
    
    // Sprawdź czy są dostępne ramki
    if (CAN_MSGAVAIL == canBus->checkReceive()) {
        if (CAN_OK == canBus->readMsgBuf(&len, buf)) {
            canId = canBus->getCanId();
            
            // Aktualizuj statystyki
            frameCounter++;
            lastFrameTime = millis();
            SystemStats* stats = bmsManager->getSystemStats();
            stats->totalFramesReceived++;
            stats->lastFrameTime = lastFrameTime;
            
            // Włącz LED na czas parsowania
            digitalWrite(LED_PIN, HIGH);
            
            if (DEBUG_CAN_FRAMES) {
                printCANFrame(canId, len, buf);
            }
            
            // Sprawdź czy to ramka BMS i parsuj
            if (isValidBMSFrame(canId) && len == CAN_FRAME_LENGTH) {
                routeCANFrame(canId, len, buf);
                stats->validBmsFrames++;
            } else {
                stats->invalidFrames++;
                if (DEBUG_CAN_FRAMES) {
                    Serial.printf("⚠️ Non-BMS frame: 0x%lX (len=%d)\n", canId, len);
                }
            }
            
            digitalWrite(LED_PIN, LOW);
        } else {
            Serial.println("❌ CAN read error!");
            SystemStats* stats = bmsManager->getSystemStats();
            stats->canErrors++;
        }
    }
}

bool CANHandler::isValidBMSFrame(unsigned long canId) {
    return isBMSFrame(canId);
}

uint8_t CANHandler::extractNodeIdFromCanId(unsigned long canId, uint16_t baseId) {
    return extractNodeId(canId, baseId);
}

void CANHandler::printCANFrame(unsigned long canId, unsigned char len, unsigned char* buf) {
    Serial.printf("🔍 CAN: ID=0x%03lX Len=%d Data=[", canId, len);
    for (int i = 0; i < len; i++) {
        Serial.printf("%02X", buf[i]);
        if (i < len - 1) Serial.print(" ");
    }
    Serial.print("]");
    
    // Dodaj opis ramki
    const char* description = getFrameDescription(canId);
    if (description) {
        Serial.printf(" (%s)", description);
    }
    
    Serial.println();
}

void CANHandler::routeCANFrame(unsigned long canId, unsigned char len, unsigned char* buf) {
    BMSFrameParsers parsers(bmsManager);
    uint16_t frameType = getFrameType(canId);
    uint8_t nodeId = 0;
    
    // Wyciągnij Node ID na podstawie typu ramki
    switch (frameType) {
        case 190:
            nodeId = extractNodeId(canId, CAN_FRAME_190_BASE);
            if (nodeId > 0) parsers.parseFrame190(nodeId, buf);
            break;
            
        case 290:
            nodeId = extractNodeId(canId, CAN_FRAME_290_BASE);
            if (nodeId > 0) parsers.parseFrame290(nodeId, buf);
            break;
            
        case 310:
            nodeId = extractNodeId(canId, CAN_FRAME_310_BASE);
            if (nodeId > 0) parsers.parseFrame310(nodeId, buf);
            break;
            
        case 390:
            nodeId = extractNodeId(canId, CAN_FRAME_390_BASE);
            if (nodeId > 0) parsers.parseFrame390(nodeId, buf);
            break;
            
        case 410:
            nodeId = extractNodeId(canId, CAN_FRAME_410_BASE);
            if (nodeId > 0) parsers.parseFrame410(nodeId, buf);
            break;
            
        case 510:
            nodeId = extractNodeId(canId, CAN_FRAME_510_BASE);
            if (nodeId > 0) parsers.parseFrame510(nodeId, buf);
            break;
            
        case 490:
            nodeId = extractNodeId(canId, CAN_FRAME_490_BASE);
            if (nodeId > 0) parsers.parseFrame490(nodeId, buf);
            break;
            
        case 0x1B0:
            nodeId = extractNodeId(canId, CAN_FRAME_1B0_BASE);
            if (nodeId > 0) parsers.parseFrame1B0(nodeId, buf);
            break;
            
        case 710:
            nodeId = extractNodeId(canId, CAN_FRAME_710_BASE);
            if (nodeId > 0) parsers.parseFrame710(nodeId, buf);
            break;
            
        default:
            if (DEBUG_CAN_FRAMES) {
                Serial.printf("⚠️ Unknown frame type: %d (CAN ID: 0x%lX)\n", frameType, canId);
            }
            break;
    }
}

void CANHandler::printStatistics() {
    SystemStats* stats = bmsManager->getSystemStats();
    unsigned long uptime = millis() - stats->systemStartTime;
    
    Serial.println("📊 CAN STATISTICS:");
    Serial.printf("   Total frames: %lu\n", stats->totalFramesReceived);
    Serial.printf("   Valid BMS frames: %lu\n", stats->validBmsFrames);
    Serial.printf("   Invalid frames: %lu\n", stats->invalidFrames);
    Serial.printf("   Parse errors: %lu\n", stats->parseErrors);
    Serial.printf("   CAN errors: %lu\n", stats->canErrors);
    Serial.printf("   Last frame: %lu ms ago\n", stats->lastFrameTime > 0 ? millis() - stats->lastFrameTime : 0);
    Serial.printf("   Uptime: %lu seconds\n", uptime / 1000);
}

void CANHandler::resetStatistics() {
    bmsManager->resetAllStats();
    frameCounter = 0;
    lastFrameTime = 0;
}

// ================================
// === BMSFrameParsers IMPLEMENTATION ===
// ================================

void BMSFrameParsers::parseFrame190(uint8_t nodeId, unsigned char* data) {
    BMSData* bms = bmsManager->getBMSDataByNodeId(nodeId);
    if (!bms) return;
    
    try {
        // Parsowanie podstawowych danych (Frame 190)
        bms->batteryVoltage = bytesToUint16(data[0], data[1]) * 0.0625f;  // [V]
        bms->batteryCurrent = bytesToInt16(data[2], data[3]) * 0.0625f;   // [A] - może być ujemny
        bms->remainingEnergy = bytesToUint16(data[4], data[5]) * 0.1f;    // [kWh]
        bms->soc = data[6] * 1.0f;                                        // [%]
        
        // Parsowanie flag błędów z bajtu 7
        uint8_t errorFlags = data[7];
        bms->masterError = isBitSet(errorFlags, 0);
        bms->cellVoltageError = isBitSet(errorFlags, 1);
        bms->cellTempMinError = isBitSet(errorFlags, 2);
        bms->cellTempMaxError = isBitSet(errorFlags, 3);
        bms->cellVoltageMinError = isBitSet(errorFlags, 4);
        bms->cellVoltageMaxError = isBitSet(errorFlags, 5);
        bms->systemShutdown = isBitSet(errorFlags, 6);
        bms->ibbVoltageSupplyError = isBitSet(errorFlags, 7);
        
        // Walidacja danych
        if (!validateDataRange(bms->batteryVoltage, BMS_MIN_VOLTAGE, BMS_MAX_VOLTAGE, "batteryVoltage")) return;
        if (!validateDataRange(abs(bms->batteryCurrent), 0.0f, BMS_MAX_CURRENT, "batteryCurrent")) return;
        if (!validateDataRange(bms->soc, 0.0f, BMS_MAX_SOC, "soc")) return;
        
        updateBMSTimestamp(nodeId);
        incrementFrameCounter(nodeId, 190);
        
        if (DEBUG_BMS_PARSING) {
            Serial.printf("📊 BMS%d-190: U=%.2fV I=%.2fA SOC=%.1f%% E=%.2fkWh MasterErr=%s\n",
                nodeId, bms->batteryVoltage, bms->batteryCurrent, bms->soc, bms->remainingEnergy,
                bms->masterError ? "YES" : "NO");
        }
        
    } catch (...) {
        handleParseError(nodeId, "Frame190", "Data parsing exception");
    }
}

void BMSFrameParsers::parseFrame290(uint8_t nodeId, unsigned char* data) {
    BMSData* bms = bmsManager->getBMSDataByNodeId(nodeId);
    if (!bms) return;
    
    try {
        // Parsowanie napięć ogniw (Frame 290)
        bms->cellMinVoltage = bytesToUint16(data[0], data[1]) * 0.001f;   // [V]
        bms->cellMeanVoltage = bytesToUint16(data[2], data[3]) * 0.001f;  // [V]
        bms->minVoltageBlock = data[4];                                   // Numer bloku
        bms->minVoltageCell = data[5];                                    // Numer ogniwa
        bms->minVoltageString = data[6];                                  // Numer stringa
        bms->balancingTempMax = data[7];                                  // [°C]
        
        // Walidacja
        if (!validateDataRange(bms->cellMinVoltage, 2.5f, 4.5f, "cellMinVoltage")) return;
        if (!validateDataRange(bms->cellMeanVoltage, 2.5f, 4.5f, "cellMeanVoltage")) return;
        
        updateBMSTimestamp(nodeId);
        incrementFrameCounter(nodeId, 290);
        
        if (DEBUG_BMS_PARSING) {
            Serial.printf("📊 BMS%d-290: CellMin=%.3fV CellMean=%.3fV Block=%d Cell=%d String=%d\n",
                nodeId, bms->cellMinVoltage, bms->cellMeanVoltage, 
                bms->minVoltageBlock, bms->minVoltageCell, bms->minVoltageString);
        }
        
    } catch (...) {
        handleParseError(nodeId, "Frame290", "Data parsing exception");
    }
}

void BMSFrameParsers::parseFrame310(uint8_t nodeId, unsigned char* data) {
    BMSData* bms = bmsManager->getBMSDataByNodeId(nodeId);
    if (!bms) return;
    
    try {
        // Parsowanie SOH, temperatury, impedancji (Frame 310)
        bms->soh = bytesToUint16(data[0], data[1]) * 0.1f;               // [%]
        bms->cellVoltage = bytesToUint16(data[2], data[3]) * 0.001f;     // [V]
        bms->cellTemperature = bytesToInt16(data[4], data[5]) * 0.1f;    // [°C] - może być ujemna
        
        // Bajt 6 zawiera flagi
        uint8_t flags = data[6];
        bms->nonEqualStringsRamp = isBitSet(flags, 0);
        bms->dynamicLimitationTimer = isBitSet(flags, 1);
        bms->overcurrentTimer = isBitSet(flags, 2);
        
        // Bajt 7 - dodatkowe dane lub część DCIR
        bms->dcir = data[7] * 0.1f;  // [mΩ] - uproszczone, może być bardziej złożone
        
        // Walidacja
        if (!validateDataRange(bms->soh, 0.0f, BMS_MAX_SOH, "soh")) return;
        if (!validateDataRange(bms->cellVoltage, 2.5f, 4.5f, "cellVoltage")) return;
        if (!validateDataRange(bms->cellTemperature, -40.0f, BMS_MAX_TEMPERATURE, "cellTemperature")) return;
        
        updateBMSTimestamp(nodeId);
        incrementFrameCounter(nodeId, 310);
        
        if (DEBUG_BMS_PARSING) {
            Serial.printf("📊 BMS%d-310: SOH=%.1f%% CellV=%.3fV CellT=%.1f°C DCIR=%.1fmΩ\n",
                nodeId, bms->soh, bms->cellVoltage, bms->cellTemperature, bms->dcir);
        }
        
    } catch (...) {
        handleParseError(nodeId, "Frame310", "Data parsing exception");
    }
}

void BMSFrameParsers::parseFrame390(uint8_t nodeId, unsigned char* data) {
    BMSData* bms = bmsManager->getBMSDataByNodeId(nodeId);
    if (!bms) return;
    
    try {
        // Parsowanie maksymalnych napięć ogniw (Frame 390)
        bms->cellMaxVoltage = bytesToUint16(data[0], data[1]) * 0.001f;  // [V]
        bms->cellVoltageDelta = bytesToUint16(data[2], data[3]) * 0.001f; // [V]
        bms->maxVoltageBlock = data[4];
        bms->maxVoltageCell = data[5];
        bms->maxVoltageString = data[6];
        bms->afeTemperatureMax = data[7];                                // [°C]
        
        // Walidacja
        if (!validateDataRange(bms->cellMaxVoltage, 2.5f, 4.5f, "cellMaxVoltage")) return;
        if (!validateDataRange(bms->cellVoltageDelta, 0.0f, 1.0f, "cellVoltageDelta")) return;
        
        updateBMSTimestamp(nodeId);
        incrementFrameCounter(nodeId, 390);
        
        if (DEBUG_BMS_PARSING) {
            Serial.printf("📊 BMS%d-390: CellMax=%.3fV Delta=%.3fV Block=%d Cell=%d AFE_T=%d°C\n",
                nodeId, bms->cellMaxVoltage, bms->cellVoltageDelta,
                bms->maxVoltageBlock, bms->maxVoltageCell, bms->afeTemperatureMax);
        }
        
    } catch (...) {
        handleParseError(nodeId, "Frame390", "Data parsing exception");
    }
}

void BMSFrameParsers::parseFrame410(uint8_t nodeId, unsigned char* data) {
    BMSData* bms = bmsManager->getBMSDataByNodeId(nodeId);
    if (!bms) return;
    
    try {
        // Parsowanie temperatur i stanów gotowości (Frame 410)
        bms->cellMaxTemperature = bytesToInt16(data[0], data[1]) * 0.1f; // [°C]
        bms->cellTempDelta = bytesToUint16(data[2], data[3]) * 0.1f;     // [°C]
        bms->maxTempString = data[4];
        bms->maxTempBlock = data[5];
        bms->maxTempSensor = data[6];
        
        // Bajt 7 zawiera stany gotowości
        uint8_t readyFlags = data[7];
        bms->readyToCharge = isBitSet(readyFlags, 0);
        bms->readyToDischarge = isBitSet(readyFlags, 1);
        
        // Walidacja
        if (!validateDataRange(bms->cellMaxTemperature, -40.0f, BMS_MAX_TEMPERATURE, "cellMaxTemperature")) return;
        if (!validateDataRange(bms->cellTempDelta, 0.0f, 100.0f, "cellTempDelta")) return;
        
        updateBMSTimestamp(nodeId);
        incrementFrameCounter(nodeId, 410);
        
        if (DEBUG_BMS_PARSING) {
            Serial.printf("📊 BMS%d-410: MaxT=%.1f°C DeltaT=%.1f°C RdyChg=%s RdyDis=%s\n",
                nodeId, bms->cellMaxTemperature, bms->cellTempDelta,
                bms->readyToCharge ? "YES" : "NO", bms->readyToDischarge ? "YES" : "NO");
        }
        
    } catch (...) {
        handleParseError(nodeId, "Frame410", "Data parsing exception");
    }
}

void BMSFrameParsers::parseFrame510(uint8_t nodeId, unsigned char* data) {
    BMSData* bms = bmsManager->getBMSDataByNodeId(nodeId);
    if (!bms) return;
    
    try {
        // Parsowanie limitów mocy i stanów I/O (Frame 510)
        bms->dccl = bytesToUint16(data[0], data[1]) * 0.0625f;  // DC Charge Limit [A]
        bms->ddcl = bytesToUint16(data[2], data[3]) * 0.0625f;  // DC Discharge Limit [A]
        
        // Bajty 4-5 - stany wejść
        uint8_t inputFlags = data[4];
        bms->input_IN02 = isBitSet(inputFlags, 1);
        bms->input_IN01 = isBitSet(inputFlags, 0);
        
        // Bajty 6-7 - stany przekaźników
        uint8_t relayFlags1 = data[6];
        uint8_t relayFlags2 = data[7];
        bms->relay_AUX4 = isBitSet(relayFlags1, 3);
        bms->relay_AUX3 = isBitSet(relayFlags1, 2);
        bms->relay_AUX2 = isBitSet(relayFlags1, 1);
        bms->relay_AUX1 = isBitSet(relayFlags1, 0);
        bms->relay_R2 = isBitSet(relayFlags2, 1);
        bms->relay_R1 = isBitSet(relayFlags2, 0);
        
        // Walidacja
        if (!validateDataRange(bms->dccl, 0.0f, BMS_MAX_CURRENT, "dccl")) return;
        if (!validateDataRange(bms->ddcl, 0.0f, BMS_MAX_CURRENT, "ddcl")) return;
        
        updateBMSTimestamp(nodeId);
        incrementFrameCounter(nodeId, 510);
        
        if (DEBUG_BMS_PARSING) {
            Serial.printf("📊 BMS%d-510: DCCL=%.1fA DDCL=%.1fA IN01=%d IN02=%d R1=%d R2=%d\n",
                nodeId, bms->dccl, bms->ddcl,
                bms->input_IN01, bms->input_IN02, bms->relay_R1, bms->relay_R2);
        }
        
    } catch (...) {
        handleParseError(nodeId, "Frame510", "Data parsing exception");
    }
}

void BMSFrameParsers::parseFrame490(uint8_t nodeId, unsigned char* data) {
    BMSData* bms = bmsManager->getBMSDataByNodeId(nodeId);
    if (!bms) return;
    
    try {
        // Parsowanie danych multipleksowanych (Frame 490)
        bms->mux490Type = data[5];                                      // Typ multipleksera
        bms->mux490Value = bytesToUint16(data[6], data[7]);            // Wartość 16-bit
        
        // Konwersja i zapisanie do odpowiedniej zmiennej na podstawie typu
        float convertedValue = convertMultiplexerValue(bms->mux490Type, bms->mux490Value);
        
        switch (bms->mux490Type) {
            case MUX490_SERIAL_NUMBER_0: bms->serialNumber0 = bms->mux490Value; break;
            case MUX490_SERIAL_NUMBER_1: bms->serialNumber1 = bms->mux490Value; break;
            case MUX490_HW_VERSION_0: bms->hwVersion0 = bms->mux490Value; break;
            case MUX490_HW_VERSION_1: bms->hwVersion1 = bms->mux490Value; break;
            case MUX490_SW_VERSION_0: bms->swVersion0 = bms->mux490Value; break;
            case MUX490_SW_VERSION_1: bms->swVersion1 = bms->mux490Value; break;
            case MUX490_FACTORY_ENERGY: bms->factoryEnergy = convertedValue; break;
            case MUX490_DESIGN_CAPACITY: bms->designCapacity = convertedValue; break;
            case MUX490_INLET_TEMPERATURE: bms->inletTemperature = convertedValue; break;
            case MUX490_HUMIDITY: bms->humidity = (uint8_t)bms->mux490Value; break;
            case MUX490_TIME_TO_FULL_CHARGE: bms->timeToFullCharge = bms->mux490Value; break;
            case MUX490_TIME_TO_FULL_DISCHARGE: bms->timeToFullDischarge = bms->mux490Value; break;
            case MUX490_BATTERY_CYCLES: bms->batteryCycles = bms->mux490Value; break;
            
            // Dodatkowe typy multipleksera można dodać tutaj
            case 0x13: bms->errorMap0 = bms->mux490Value; break;
            case 0x14: bms->errorMap1 = bms->mux490Value; break;
            case 0x15: bms->errorMap2 = bms->mux490Value; break;
            case 0x16: bms->errorMap3 = bms->mux490Value; break;
            
            default:
                // Nieznany typ multipleksera - można zalogować
                if (DEBUG_BMS_PARSING) {
                    Serial.printf("⚠️ BMS%d-490: Unknown multiplexer type 0x%02X = 0x%04X\n",
                        nodeId, bms->mux490Type, bms->mux490Value);
                }
                break;
        }
        
        updateBMSTimestamp(nodeId);
        incrementFrameCounter(nodeId, 490);
        
        if (DEBUG_BMS_PARSING) {
            const MultiplexerType* typeInfo = getMultiplexerTypeInfo(bms->mux490Type);
            if (typeInfo) {
                Serial.printf("📊 BMS%d-490: %s = %.2f %s [Type:0x%02X Raw:0x%04X]\n",
                    nodeId, typeInfo->name, convertedValue, typeInfo->unit,
                    bms->mux490Type, bms->mux490Value);
            }
        }
        
    } catch (...) {
        handleParseError(nodeId, "Frame490", "Data parsing exception");
    }
}

void BMSFrameParsers::parseFrame1B0(uint8_t nodeId, unsigned char* data) {
    BMSData* bms = bmsManager->getBMSDataByNodeId(nodeId);
    if (!bms) return;
    
    try {
        // Parsowanie danych dodatkowych (Frame 1B0) - na razie zapisz raw data
        memcpy(bms->frame1B0Data, data, 8);
        
        updateBMSTimestamp(nodeId);
        incrementFrameCounter(nodeId, 0x1B0);
        
        if (DEBUG_BMS_PARSING) {
            Serial.printf("📊 BMS%d-1B0: Raw data = %s\n", 
                nodeId, formatCANData(data, 8).c_str());
        }
        
    } catch (...) {
        handleParseError(nodeId, "Frame1B0", "Data parsing exception");
    }
}

void BMSFrameParsers::parseFrame710(uint8_t nodeId, unsigned char* data) {
    BMSData* bms = bmsManager->getBMSDataByNodeId(nodeId);
    if (!bms) return;
    
    try {
        // Parsowanie stanu CANopen (Frame 710)
        bms->canopenState = data[0];  // Pierwszy bajt zawiera stan CANopen
        
        updateBMSTimestamp(nodeId);
        incrementFrameCounter(nodeId, 710);
        
        if (DEBUG_BMS_PARSING) {
            const char* stateStr = "Unknown";
            switch (bms->canopenState) {
                case 0x00: stateStr = "Boot-up"; break;
                case 0x04: stateStr = "Stopped"; break;
                case 0x05: stateStr = "Operational"; break;
                case 0x7F: stateStr = "Pre-operational"; break;
                default: stateStr = "Unknown"; break;
            }
            Serial.printf("📊 BMS%d-710: CANopen state = 0x%02X (%s)\n",
                nodeId, bms->canopenState, stateStr);
        }
        
    } catch (...) {
        handleParseError(nodeId, "Frame710", "Data parsing exception");
    }
}

// ================================
// === HELPER METHODS ===
// ================================

void BMSFrameParsers::updateBMSTimestamp(uint8_t nodeId) {
    BMSData* bms = bmsManager->getBMSDataByNodeId(nodeId);
    if (bms) {
        unsigned long currentTime = millis();
        bms->lastUpdate = currentTime;
        bms->lastFrameTime = currentTime;
        bms->communicationOk = true;
        bms->packetsReceived++;
        
        if (bms->firstFrameTime == 0) {
            bms->firstFrameTime = currentTime;
        }
    }
}

void BMSFrameParsers::incrementFrameCounter(uint8_t nodeId, uint8_t frameType) {
    BMSData* bms = bmsManager->getBMSDataByNodeId(nodeId);
    if (!bms) return;
    
    bms->totalFrames++;
    
    switch (frameType) {
        case 190: bms->frame190Count++; break;
        case 290: bms->frame290Count++; break;
        case 310: bms->frame310Count++; break;
        case 390: bms->frame390Count++; break;
        case 410: bms->frame410Count++; break;
        case 510: bms->frame510Count++; break;
        case 490: bms->frame490Count++; break;
        case 0x1B0: bms->frame1B0Count++; break;
        case 710: bms->frame710Count++; break;
    }
}

bool BMSFrameParsers::validateDataRange(float value, float min, float max, const char* paramName) {
    if (value < min || value > max) {
        if (DEBUG_BMS_PARSING) {
            Serial.printf("⚠️ Value out of range: %s = %.2f (expected %.2f - %.2f)\n", 
                paramName, value, min, max);
        }
        return false;
    }
    return true;
}

void BMSFrameParsers::handleParseError(uint8_t nodeId, const char* frameType, const char* error) {
    BMSData* bms = bmsManager->getBMSDataByNodeId(nodeId);
    if (bms) {
        bms->parseErrors++;
    }
    
    SystemStats* stats = bmsManager->getSystemStats();
    stats->parseErrors++;
    
    Serial.printf("❌ Parse error BMS%d %s: %s\n", nodeId, frameType, error);
}

// ================================
// === UTILITY FUNCTIONS ===
// ================================

uint8_t extractNodeId(unsigned long canId, uint16_t baseId) {
    if ((canId & 0xFFF0) == (baseId & 0xFFF0)) {
        uint8_t nodeId = canId - baseId + 1;  // +1 bo Node ID zaczyna się od 1
        if (IS_VALID_BMS_NODE_ID(nodeId)) {
            return nodeId;
        }
    }
    return 0; // Nieprawidłowy Node ID
}

bool isBMSFrame(unsigned long canId) {
    return IS_VALID_BMS_CAN_ID(canId);
}

uint16_t getFrameType(unsigned long canId) {
    if ((canId & 0xFFF0) == (CAN_FRAME_190_BASE & 0xFFF0)) return 190;
    if ((canId & 0xFFF0) == (CAN_FRAME_290_BASE & 0xFFF0)) return 290;
    if ((canId & 0xFFF0) == (CAN_FRAME_310_BASE & 0xFFF0)) return 310;
    if ((canId & 0xFFF0) == (CAN_FRAME_390_BASE & 0xFFF0)) return 390;
    if ((canId & 0xFFF0) == (CAN_FRAME_410_BASE & 0xFFF0)) return 410;
    if ((canId & 0xFFF0) == (CAN_FRAME_510_BASE & 0xFFF0)) return 510;
    if ((canId & 0xFFF0) == (CAN_FRAME_490_BASE & 0xFFF0)) return 490;
    if ((canId & 0xFFF0) == (CAN_FRAME_1B0_BASE & 0xFFF0)) return 0x1B0;
    if ((canId & 0xFFF0) == (CAN_FRAME_710_BASE & 0xFFF0)) return 710;
    return 0; // Nieznany typ
}

const char* getFrameDescription(unsigned long canId) {
    uint16_t frameType = getFrameType(canId);
    const FrameConfig* config = getFrameConfig((BMSFrameType)frameType);
    return config ? config->description : "Unknown frame";
}

String formatCANData(unsigned char* data, unsigned char length) {
    String result = "";
    for (int i = 0; i < length; i++) {
        if (i > 0) result += " ";
        result += String(data[i], HEX);
        if (data[i] < 0x10) result = result.substring(0, result.length()-1) + "0" + result.substring(result.length()-1);
    }
    result.toUpperCase();
    return result;
}

const FrameConfig* getFrameConfig(BMSFrameType frameType) {
    for (size_t i = 0; i < ARRAY_SIZE(FRAME_CONFIGS); i++) {
        if (FRAME_CONFIGS[i].type == frameType) {
            return &FRAME_CONFIGS[i];
        }
    }
    return nullptr;
}

const FrameConfig* getFrameConfigByCanId(unsigned long canId) {
    uint16_t frameType = getFrameType(canId);
    return getFrameConfig((BMSFrameType)frameType);
}