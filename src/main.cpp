/*
 * ESP32S3 CAN to Modbus TCP Bridge - Główny Program
 * 
 * VERSION: v4.0.0 - MODULAR ARCHITECTURE
 * DATE: 2025-08-12
 * STATUS: ✅ READY FOR COMPILATION
 * 
 * OPIS: Główny program łączący wszystkie moduły systemu w spójną całość
 */

#include "config.h"
#include "bms_data.h"
#include "can_handler.h"
#include "modbus_tcp.h"
#include "wifi_manager.h"

// ================================
// === GLOBAL OBJECTS ===
// ================================

BMSManager bmsManager;              // Manager danych BMS
CANHandler canHandler(&bmsManager); // Handler komunikacji CAN
ModbusTCPServer modbusServer(&bmsManager); // Serwer Modbus TCP
WiFiManager wifiManager;            // Manager WiFi

// Zmienne czasowe dla heartbeat i diagnostyki
unsigned long lastHeartbeat = 0;
unsigned long lastLEDHeartbeat = 0;
unsigned long lastCommunicationCheck = 0;
unsigned long lastStatsUpdate = 0;
unsigned long loopStartTime = 0;

// ================================
// === SYSTEM INITIALIZATION ===
// ================================

void setup() {
    // Inicjalizacja Serial z timeoutem
    Serial.begin(SERIAL_BAUDRATE);
    
    // KLUCZOWE: Poczekaj na połączenie Serial Monitor lub timeout
    unsigned long serialTimeout = millis() + SERIAL_INIT_TIMEOUT_MS;
    while (!Serial && millis() < serialTimeout) {
        delay(10);
    }
    
    delay(1000); // Dodatkowe opóźnienie dla stabilności
    
    // Wyczyść bufor i wyślij znak startowy
    Serial.flush();
    Serial.println();
    Serial.println("🚀 ESP32S3 STARTING...");
    
    printSystemHeader();
    
    // Inicjalizacja LED diagnostyki
    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, LOW);
    
    // Powitanie migające LED
    blinkLED(5, 200);
    
    // Inicjalizacja modułów w odpowiedniej kolejności
    if (!initializeSystem()) {
        Serial.println("❌ System initialization failed!");
        handleCriticalError();
        return;
    }
    
    Serial.println("🚀 System initialization completed successfully!");
    Serial.println("📡 Monitoring CAN bus for BMS frames...");
    Serial.println("🌐 Modbus TCP server ready for connections");
    
    // Finalne miganie LED sygnalizujące gotowość
    blinkLED(3, 500);
    
    // Inicjalizacja zmiennych czasowych
    lastHeartbeat = millis();
    lastLEDHeartbeat = millis();
    lastCommunicationCheck = millis();
    lastStatsUpdate = millis();
    
    Serial.println("✅ System ready - entering main loop");
}

// ================================
// === MAIN LOOP ===
// ================================

void loop() {
    loopStartTime = micros();
    
    // Główne zadania systemu
    processSystemTasks();
    
    // Zadania czasowe
    processTimedTasks();
    
    // Monitorowanie wydajności
    updatePerformanceMetrics();
    
    // Małe opóźnienie dla stabilności
    delay(1);
}

// ================================
// === SYSTEM TASKS ===
// ================================

void processSystemTasks() {
    // 1. Obsługa WiFi (najwyższy priorytet)
    wifiManager.process();
    
    // 2. Obsługa komunikacji CAN
    canHandler.process();
    
    // 3. Obsługa serwera Modbus TCP (tylko jeśli WiFi działa)
    if (wifiManager.isConnected()) {
        modbusServer.process();
    }
}

void processTimedTasks() {
    unsigned long currentTime = millis();
    
    // Heartbeat systemowy (co 60 sekund)
    if (currentTime - lastHeartbeat >= HEARTBEAT_INTERVAL_MS) {
        printSystemHeartbeat();
        lastHeartbeat = currentTime;
    }
    
    // LED heartbeat (co 5 sekund)
    if (currentTime - lastLEDHeartbeat >= LED_HEARTBEAT_INTERVAL_MS) {
        performLEDHeartbeat();
        lastLEDHeartbeat = currentTime;
    }
    
    // Sprawdzenie komunikacji z BMS (co 10 sekund)
    if (currentTime - lastCommunicationCheck >= COMMUNICATION_CHECK_MS) {
        checkBMSCommunication();
        lastCommunicationCheck = currentTime;
    }
    
    // Aktualizacja statystyk (co sekundę)
    if (currentTime - lastStatsUpdate >= STATISTICS_UPDATE_MS) {
        updateSystemStatistics();
        lastStatsUpdate = currentTime;
    }
}

// ================================
// === INITIALIZATION FUNCTIONS ===
// ================================

bool initializeSystem() {
    Serial.println("🔧 Initializing system modules...");
    
    // 1. Inicjalizacja WiFi
    Serial.println("📶 Initializing WiFi...");
    if (!wifiManager.initialize()) {
        Serial.println("❌ WiFi initialization failed");
        return false;
    }
    
    // 2. Inicjalizacja CAN
    Serial.println("🚌 Initializing CAN bus...");
    if (!canHandler.initialize()) {
        Serial.println("❌ CAN initialization failed");
        return false;
    }
    
    // 3. Inicjalizacja Modbus TCP (po połączeniu WiFi)
    Serial.println("🔗 Waiting for WiFi connection...");
    unsigned long wifiTimeout = millis() + WIFI_CONNECT_TIMEOUT;
    while (!wifiManager.isConnected() && millis() < wifiTimeout) {
        wifiManager.process();
        delay(100);
    }
    
    if (wifiManager.isConnected()) {
        Serial.println("📊 Initializing Modbus TCP server...");
        if (!modbusServer.initialize()) {
            Serial.println("⚠️ Modbus TCP initialization failed - continuing without network access");
        }
    } else {
        Serial.println("⚠️ WiFi not connected - Modbus TCP server will start when WiFi connects");
    }
    
    // 4. Konfiguracja BMS Manager
    Serial.printf("🔋 Configuring BMS Manager for %d batteries...\n", bmsManager.getActiveBmsCount());
    
    return true;
}

void printSystemHeader() {
    Serial.println();
    Serial.println(F("================================================================"));
    Serial.println(F("=== ESP32S3 CAN to Modbus TCP Bridge - MODULAR VERSION ==="));
    Serial.printf("=== VERSION: %s - %s ===\n", SYSTEM_VERSION, BUILD_DATE);
    Serial.printf("=== PROTOCOL: %s ===\n", PROTOCOL_DESCRIPTION);
    Serial.printf("=== CAPACITY: %d baterii × %d rejestrów = %d rejestrów ===\n", 
        MAX_BMS_NODES, MODBUS_REGISTERS_PER_BATTERY, MODBUS_MAX_HOLDING_REGISTERS);
    Serial.println(F("=== ARCHITECTURE: Modular Design with Separated Components ==="));
    Serial.println(F("================================================================"));
    Serial.println();
    
    // Informacje o konfiguracji
    Serial.println("🔧 SYSTEM CONFIGURATION:");
    Serial.printf("   Max BMS nodes: %d\n", MAX_BMS_NODES);
    Serial.printf("   Active BMS nodes: %d\n", ACTIVE_BMS_NODES);
    Serial.printf("   CAN speed: %s\n", "125 kbps");
    Serial.printf("   Modbus TCP port: %d\n", MODBUS_TCP_PORT);
    Serial.printf("   WiFi SSID: %s\n", WIFI_SSID);
    Serial.printf("   Debug level: %d\n", DEBUG_LEVEL);
    Serial.printf("   Free heap: %d bytes\n", ESP.getFreeHeap());
    Serial.println();
}

// ================================
// === HEARTBEAT & DIAGNOSTICS ===
// ================================

void printSystemHeartbeat() {
    if (!DEBUG_HEARTBEAT_EXTENDED) return;
    
    SystemStats* stats = bmsManager.getSystemStats();
    unsigned long uptime = (millis() - stats->systemStartTime) / 1000;
    uint8_t activeBatteries = bmsManager.checkCommunication();
    
    Serial.println();
    Serial.println(F("💓 ===== MODBUS TCP BRIDGE HEARTBEAT ====="));
    
    // Statystyki CAN
    Serial.printf("🚌 CAN frames: %lu total, %lu BMS valid, %lu invalid, %lu errors\n",
        stats->totalFramesReceived, stats->validBmsFrames, stats->invalidFrames, stats->parseErrors);
    
    // Statystyki Modbus TCP
    Serial.printf("🔗 Modbus TCP: %lu requests, %lu responses, %lu errors\n",
        stats->modbusRequests, stats->modbusResponses, stats->modbusErrors);
    
    // Ostatnie aktywności
    unsigned long lastCanFrame = stats->lastFrameTime > 0 ? (millis() - stats->lastFrameTime) / 1000 : 0;
    unsigned long lastModbusReq = stats->lastModbusRequest > 0 ? (millis() - stats->lastModbusRequest) / 1000 : 0;
    Serial.printf("📡 Last CAN frame: %lu sec ago\n", lastCanFrame);
    Serial.printf("📞 Last Modbus request: %lu sec ago\n", lastModbusReq);
    
    // Status sieciowy
    if (wifiManager.isConnected()) {
        Serial.printf("📶 WiFi: Connected (RSSI: %d dBm)\n", wifiManager.getRSSI());
        Serial.printf("🎯 Modbus TCP Server: %s:%d\n", 
            wifiManager.getLocalIP().toString().c_str(), MODBUS_TCP_PORT);
    } else {
        Serial.printf("📶 WiFi: %s\n", wifiManager.getStateString().c_str());
    }
    
    // System info
    Serial.printf("⏰ Uptime: %lu min\n", uptime / 60);
    Serial.printf("💾 Free heap: %lu bytes\n", ESP.getFreeHeap());
    Serial.printf("🔄 Max loop time: %lu μs\n", stats->maxLoopTime);
    Serial.printf("📊 Avg loop time: %lu μs\n", stats->avgLoopTime);
    
    Serial.println();
    Serial.printf("🔋 ACTIVE BATTERIES STATUS (%d/%d):\n", activeBatteries, bmsManager.getActiveBmsCount());
    
    // Status każdej baterii
    for (int i = 0; i < bmsManager.getActiveBmsCount(); i++) {
        BMSData* bms = bmsManager.getBMSData(i);
        uint8_t nodeId = bmsManager.getNodeId(i);
        uint16_t modbusBase = i * MODBUS_REGISTERS_PER_BATTERY;
        
        if (bms && bms->communicationOk) {
            Serial.printf("   BMS%d [Modbus:%d]: %.2fV %.2fA %.1f%% SOH:%.1f%% ✅OK",
                nodeId, modbusBase, bms->batteryVoltage, bms->batteryCurrent, 
                bms->soc, bms->soh);
            
            // Dodaj info o multiplekserze jeśli dostępne
            if (bms->frame490Count > 0) {
                Serial.printf(" Mux490:%lu [Type:0x%02X=0x%04X]",
                    bms->frame490Count, bms->mux490Type, bms->mux490Value);
            }
            
            // Dodaj flagę błędu głównego
            if (bms->masterError) {
                Serial.print(" ⚠️ERR");
            }
            
            Serial.println();
        } else {
            Serial.printf("   BMS%d [Modbus:%d]: OFFLINE\n", nodeId, modbusBase);
        }
    }
    
    Serial.println(F("=========================================="));
    Serial.println();
}

void performLEDHeartbeat() {
    // Liczba mrugnięć = liczba aktywnych baterii
    uint8_t activeBatteries = bmsManager.checkCommunication();
    
    if (activeBatteries > 0) {
        // Normalne mrugnięcia dla aktywnych baterii
        for (int i = 0; i < activeBatteries && i < 16; i++) {
            digitalWrite(LED_PIN, HIGH);
            delay(50);
            digitalWrite(LED_PIN, LOW);
            delay(100);
        }
    } else {
        // Długie mrugnięcie gdy brak komunikacji
        digitalWrite(LED_PIN, HIGH);
        delay(500);
        digitalWrite(LED_PIN, LOW);
    }
}

void checkBMSCommunication() {
    uint8_t activeBatteries = bmsManager.checkCommunication(BMS_COMMUNICATION_TIMEOUT_MS);
    
    // Loguj timeouty komunikacji
    for (int i = 0; i < bmsManager.getActiveBmsCount(); i++) {
        BMSData* bms = bmsManager.getBMSData(i);
        uint8_t nodeId = bmsManager.getNodeId(i);
        
        if (bms && !bms->communicationOk && bms->lastUpdate > 0) {
            unsigned long timeSinceLastFrame = millis() - bms->lastUpdate;
            if (timeSinceLastFrame > BMS_COMMUNICATION_TIMEOUT_MS) {
                Serial.printf("⚠️ BMS%d communication timeout! Last frame: %lu ms ago\n", 
                    nodeId, timeSinceLastFrame);
            }
        }
    }
    
    // Loguj zmiany w liczbie aktywnych baterii
    static uint8_t lastActiveBatteries = 0;
    if (activeBatteries != lastActiveBatteries) {
        Serial.printf("🔋 Active batteries changed: %d → %d\n", lastActiveBatteries, activeBatteries);
        lastActiveBatteries = activeBatteries;
    }
}

void updateSystemStatistics() {
    SystemStats* stats = bmsManager.getSystemStats();
    
    // Aktualizuj statystyki Modbus z serwera
    stats->modbusRequests = modbusServer.getRequestCount();
    stats->modbusResponses = modbusServer.getResponseCount();
    stats->modbusErrors = modbusServer.getErrorCount();
    stats->lastModbusRequest = modbusServer.getLastRequestTime();
    stats->activeConnections = modbusServer.getActiveClientCount();
    
    // Aktualizuj statystyki CAN z handlera
    stats->lastFrameTime = canHandler.getLastFrameTime();
    
    // Aktualizuj statystyki pamięci
    uint32_t currentHeap = ESP.getFreeHeap();
    stats->freeHeapCurrent = currentHeap;
    if (currentHeap < stats->freeHeapMin) {
        stats->freeHeapMin = currentHeap;
    }
}

void updatePerformanceMetrics() {
    unsigned long loopTime = micros() - loopStartTime;
    bmsManager.updatePerformanceStats(loopTime);
}

// ================================
// === ERROR HANDLING ===
// ================================

void handleCriticalError() {
    Serial.println("💀 CRITICAL ERROR - System halted");
    Serial.println("🔧 Troubleshooting suggestions:");
    Serial.println("   1. Check power supply and connections");
    Serial.println("   2. Verify CAN bus wiring and termination");
    Serial.println("   3. Check WiFi credentials and network");
    Serial.println("   4. Review Serial Monitor for error details");
    Serial.println("   5. Try power cycle (disconnect and reconnect power)");
    
    // Migaj LED błędu w nieskończonej pętli
    while (true) {
        blinkLED(3, 200);  // 3 szybkie mrugnięcia
        delay(2000);       // 2 sekundy przerwy
    }
}

// ================================
// === UTILITY FUNCTIONS ===
// ================================

void blinkLED(int count, int delayMs) {
    for (int i = 0; i < count; i++) {
        digitalWrite(LED_PIN, HIGH);
        delay(delayMs);
        digitalWrite(LED_PIN, LOW);
        if (i < count - 1) delay(delayMs);
    }
}

// ================================
// === SERIAL COMMANDS (OPCJONALNE) ===
// ================================

void processSerialCommands() {
    if (!Serial.available()) return;
    
    String command = Serial.readStringUntil('\n');
    command.trim();
    command.toLowerCase();
    
    if (command == "help" || command == "?") {
        printHelpMenu();
    } else if (command == "status") {
        printSystemStatus();
    } else if (command == "stats") {
        printDetailedStatistics();
    } else if (command == "reset") {
        resetAllStatistics();
    } else if (command == "wifi") {
        wifiManager.printStatistics();
    } else if (command == "scan") {
        wifiManager.scanNetworks();
    } else if (command == "reboot") {
        Serial.println("🔄 Rebooting system...");
        delay(1000);
        ESP.restart();
    } else if (command.startsWith("debug ")) {
        handleDebugCommand(command);
    } else {
        Serial.printf("❓ Unknown command: %s (type 'help' for available commands)\n", command.c_str());
    }
}

void printHelpMenu() {
    Serial.println("📋 AVAILABLE COMMANDS:");
    Serial.println("   help     - Show this help menu");
    Serial.println("   status   - Show system status");
    Serial.println("   stats    - Show detailed statistics");
    Serial.println("   reset    - Reset all statistics");
    Serial.println("   wifi     - Show WiFi statistics");
    Serial.println("   scan     - Scan for WiFi networks");
    Serial.println("   reboot   - Restart system");
    Serial.println("   debug <level> - Set debug level (0-4)");
}

void printSystemStatus() {
    Serial.println("📊 SYSTEM STATUS:");
    Serial.printf("   System version: %s\n", SYSTEM_VERSION);
    Serial.printf("   Uptime: %lu seconds\n", millis() / 1000);
    Serial.printf("   WiFi: %s\n", wifiManager.getStateString().c_str());
    Serial.printf("   CAN initialized: %s\n", canHandler.isInitialized() ? "Yes" : "No");
    Serial.printf("   Modbus server: %s\n", modbusServer.isInitialized() ? "Running" : "Stopped");
    Serial.printf("   Active BMS: %d/%d\n", bmsManager.checkCommunication(), bmsManager.getActiveBmsCount());
    Serial.printf("   Free heap: %lu bytes\n", ESP.getFreeHeap());
}

void printDetailedStatistics() {
    canHandler.printStatistics();
    Serial.println();
    modbusServer.printStatistics();
    Serial.println();
    wifiManager.printStatistics();
}

void resetAllStatistics() {
    bmsManager.resetAllStats();
    canHandler.resetStatistics();
    modbusServer.resetStatistics();
    wifiManager.resetStatistics();
    Serial.println("✅ All statistics reset");
}

void handleDebugCommand(const String& command) {
    int spaceIndex = command.indexOf(' ');
    if (spaceIndex > 0) {
        String levelStr = command.substring(spaceIndex + 1);
        int level = levelStr.toInt();
        if (level >= 0 && level <= 4) {
            Serial.printf("🔧 Debug level set to %d\n", level);
            // TODO: Implementacja zmiany poziomu debug w runtime
        } else {
            Serial.println("❌ Invalid debug level (use 0-4)");
        }
    }
}

// ================================
// === ADVANCED FEATURES ===
// ================================

void checkMemoryLeaks() {
    static uint32_t lastHeapSize = ESP.getFreeHeap();
    uint32_t currentHeapSize = ESP.getFreeHeap();
    
    if (currentHeapSize < lastHeapSize - 1000) { // Spadek o więcej niż 1KB
        Serial.printf("⚠️ Potential memory leak detected: %lu → %lu bytes\n", 
            lastHeapSize, currentHeapSize);
    }
    
    lastHeapSize = currentHeapSize;
}

void performSystemHealthCheck() {
    // Sprawdź stan wszystkich modułów
    bool systemHealthy = true;
    
    if (!canHandler.isInitialized()) {
        Serial.println("❌ CAN handler not initialized");
        systemHealthy = false;
    }
    
    if (wifiManager.isConnected() && !modbusServer.isInitialized()) {
        Serial.println("⚠️ Modbus server not running despite WiFi connection");
    }
    
    // Sprawdź czy są jakiekolwiek dane BMS
    if (bmsManager.checkCommunication() == 0) {
        unsigned long timeSinceStart = millis();
        if (timeSinceStart > 60000) { // Po minucie od startu
            Serial.println("⚠️ No BMS communication detected after 1 minute");
        }
    }
    
    if (systemHealthy) {
        Serial.println("✅ System health check passed");
    }
}