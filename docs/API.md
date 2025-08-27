# ESP32S3 CAN-Modbus TCP - API Documentation

**Project:** ESP32S3 CAN to Modbus TCP Bridge  
**Repository:** https://github.com/user/esp32s3-can-modbus-tcp  
**Version:** API Documentation v1.0  
**Created:** 27.08.2025 (Warsaw Time)  
**Last Updated:** 27.08.2025 (Warsaw Time)  
**Status:** Final

---

## Document Information

### Purpose:
This document provides comprehensive API documentation for all modules in the ESP32S3 CAN-Modbus TCP Bridge project, including function signatures, data structures, and usage examples.

### Scope:
- Public APIs for all system modules
- Data structures and type definitions
- Function parameters and return values
- Usage examples and best practices

### Audience:
- Embedded software developers
- System integrators
- Maintenance programmers
- Third-party developers extending the system

### Related Documents:
- [System Architecture](ARCHITECTURE.md) - Overall system design and module relationships
- [Setup Guide](SETUP.md) - Installation and configuration procedures
- [Main README](../README.md) - Project overview and specifications

---

## Module APIs

### 1. Config Module API (config.h/cpp)

#### Core Data Structures:
```cpp
typedef struct {
    bool configValid;
    char wifiSSID[32];
    char wifiPassword[64];
    uint8_t activeBmsNodes;
    uint8_t bmsNodeIds[MAX_BMS_NODES];
    uint16_t canSpeed;  // 125 or 500 kbps
    bool debugEnabled;
} SystemConfig_t;

extern SystemConfig_t systemConfig;  // Global configuration
```

#### Primary Functions:
```cpp
/**
 * @brief Load configuration from EEPROM
 * @return bool - true if configuration loaded successfully
 */
bool loadConfiguration();

/**
 * @brief Save current configuration to EEPROM
 * @return bool - true if configuration saved successfully
 */
bool saveConfiguration();

/**
 * @brief Reset configuration to factory defaults
 * @return bool - true if reset successful
 */
bool resetConfiguration();

/**
 * @brief Validate configuration parameters
 * @return bool - true if configuration is valid
 */
bool validateConfiguration();
```

#### Usage Example:
```cpp
#include "config.h"

void setup() {
    if (!loadConfiguration()) {
        Serial.println("Using default configuration");
        saveConfiguration();  // Save defaults to EEPROM
    }
    
    // Access configuration
    Serial.printf("WiFi SSID: %s\n", systemConfig.wifiSSID);
    Serial.printf("Active BMS nodes: %d\n", systemConfig.activeBmsNodes);
}
```

### 2. WiFi Manager Module API (wifi_manager.h/cpp)

#### Core Class:
```cpp
typedef enum {
    WIFI_STATE_DISCONNECTED,
    WIFI_STATE_CONNECTING,
    WIFI_STATE_CONNECTED,
    WIFI_STATE_AP_MODE,
    WIFI_STATE_ERROR
} WiFiState_t;

class WiFiManager {
public:
    /**
     * @brief Initialize WiFi manager
     * @return bool - true if initialization successful
     */
    bool begin();
    
    /**
     * @brief Process WiFi state machine (call in main loop)
     */
    void process();
    
    /**
     * @brief Check if WiFi is connected
     * @return bool - true if connected to station
     */
    bool isConnected();
    
    /**
     * @brief Check if AP mode is active
     * @return bool - true if AP mode running
     */
    bool isAPModeActive();
    
    /**
     * @brief Get current WiFi state
     * @return WiFiState_t - current state
     */
    WiFiState_t getState();
    
    /**
     * @brief Get signal strength (RSSI)
     * @return int - RSSI in dBm
     */
    int getRSSI();
    
    /**
     * @brief Get signal strength description
     * @return String - "Excellent", "Good", "Fair", "Poor"
     */
    String getSignalStrength();
    
    /**
     * @brief Start triggered AP mode (CAN-activated)
     */
    void startTriggeredAPMode();
    
    /**
     * @brief Stop triggered AP mode
     */
    void stopTriggeredAPMode();
    
    /**
     * @brief Set callback functions
     * @param onStateChange - called when WiFi state changes
     * @param onConnected - called when WiFi connects
     * @param onDisconnected - called when WiFi disconnects
     */
    void setCallbacks(
        void (*onStateChange)(WiFiState_t oldState, WiFiState_t newState),
        void (*onConnected)(String ip),
        void (*onDisconnected)()
    );
};

extern WiFiManager wifiManager;  // Global instance
```

#### Usage Example:
```cpp
#include "wifi_manager.h"

void onWiFiConnected(String ip) {
    Serial.printf("Connected! IP: %s\n", ip.c_str());
}

void setup() {
    wifiManager.setCallbacks(nullptr, onWiFiConnected, nullptr);
    wifiManager.begin();
}

void loop() {
    wifiManager.process();
    
    if (wifiManager.isConnected()) {
        // WiFi operations
        Serial.printf("Signal: %s (%d dBm)\n", 
                      wifiManager.getSignalStrength().c_str(),
                      wifiManager.getRSSI());
    }
}
```

### 3. Modbus TCP Module API (modbus_tcp.h/cpp)

#### Core Types:
```cpp
typedef enum {
    MODBUS_STATE_STOPPED,
    MODBUS_STATE_STARTING,
    MODBUS_STATE_RUNNING,
    MODBUS_STATE_ERROR
} ModbusState_t;

#define MODBUS_TCP_PORT 502
#define MODBUS_SLAVE_ID 1
#define MODBUS_MAX_HOLDING_REGISTERS 3200
```

#### Primary Functions:
```cpp
/**
 * @brief Initialize Modbus TCP server
 * @return bool - true if server started successfully
 */
bool setupModbusTCP();

/**
 * @brief Process Modbus TCP requests (call in main loop)
 */
void processModbusTCP();

/**
 * @brief Check if Modbus server is active
 * @return bool - true if server is running
 */
bool isModbusServerActive();

/**
 * @brief Get current Modbus server state
 * @return ModbusState_t - current server state
 */
ModbusState_t getModbusState();

/**
 * @brief Print Modbus statistics to serial
 */
void printModbusStatistics();

/**
 * @brief Get register value for specific BMS and offset
 * @param bmsId - BMS node ID (1-16)
 * @param registerOffset - offset within BMS registers (0-199)
 * @return uint16_t - register value
 */
uint16_t getModbusRegister(uint8_t bmsId, uint16_t registerOffset);
```

#### Register Address Calculation:
```cpp
/**
 * @brief Calculate Modbus register address for BMS parameter
 * @param bmsId - BMS node ID (1-16)
 * @param parameter - parameter offset (0-199)
 * @return uint16_t - absolute Modbus register address
 */
inline uint16_t calculateModbusAddress(uint8_t bmsId, uint8_t parameter) {
    return ((bmsId - 1) * 200) + parameter;
}

// Usage examples:
// BMS 1 SOC (offset 3): address = calculateModbusAddress(1, 3) = 3
// BMS 2 SOC (offset 3): address = calculateModbusAddress(2, 3) = 203
// BMS 10 voltage (offset 0): address = calculateModbusAddress(10, 0) = 1800
```

### 4. BMS Data Module API (bms_data.h)

#### Core Data Structure:
```cpp
typedef struct {
    // Basic Battery Parameters (Frame 0x190)
    uint16_t batteryVoltage;        // mV, register 0
    int16_t batteryCurrent;         // mA, register 1 
    uint16_t remainingEnergy;       // Wh×100, register 2
    uint16_t soc;                   // %×100, register 3
    uint16_t flags[6];              // System flags, registers 4-9
    
    // Cell Data (Frame 0x290)
    uint16_t cellMinVoltage;        // mV×1000, register 10
    uint16_t cellMeanVoltage;       // mV×1000, register 11
    uint8_t minVoltageBlock;        // Block ID, register 12 LSB
    uint8_t minVoltageCell;         // Cell ID, register 13 MSB
    
    // SOH and Temperature (Frame 0x310)
    uint16_t soh;                   // %×100, register 20
    uint16_t cellVoltage;           // mV, register 21
    int16_t cellTemperature;        // °C×10, register 22
    int8_t cellMinTemperature;      // °C, register 23 LSB
    int8_t cellMeanTemperature;     // °C, register 24 MSB
    uint16_t dcir;                  // mΩ, register 25
    
    // Maximum Values (Frame 0x390)
    uint16_t cellMaxVoltage;        // mV×1000, register 30
    uint16_t cellVoltageDelta;      // mV×1000, register 31
    
    // Extended Temperature (Frame 0x410)
    int16_t cellMaxTemperature;     // °C×10, register 40
    int16_t cellTempDelta;          // °C×10, register 41
    uint16_t readyFlags;            // Ready flags, register 45
    
    // Power Limits (Frame 0x510)
    uint16_t dccl;                  // A×100, register 50
    uint16_t ddcl;                  // A×100, register 51
    uint8_t inputs;                 // Digital inputs, register 52 LSB
    uint8_t outputs;                // Digital outputs, register 53 MSB
    
    // Multiplexed Data (Frame 0x490) - 54 types supported
    uint8_t mux490Type;             // Multiplexer type, register 60
    uint16_t mux490Value;           // Multiplexer value, register 61
    uint32_t serialNumber;          // 32-bit serial, registers 62-63
    uint32_t hwVersion;             // HW version, registers 64-65
    uint32_t swVersion;             // SW version, registers 66-67
    uint16_t factoryEnergy;         // kWh×100, register 68
    uint16_t designCapacity;        // Ah×100, register 69
    
    // Error Maps and Diagnostics (registers 80-89)
    uint16_t errorsMap[4];          // Error bitmaps, registers 80-83
    uint16_t timeToFullCharge;      // minutes, register 84
    uint16_t timeToFullDischarge;   // minutes, register 85
    uint16_t batteryCycles;         // cycles, register 86
    
    // Extended Data (registers 90-109)
    int16_t inletTemperature;       // °C×10, register 90
    int16_t outletTemperature;      // °C×10, register 91
    uint16_t humidity;              // %×10, register 92
    int16_t ballancerTempMax;       // °C×10, register 93
    int16_t ltcTempMax;             // °C×10, register 94
    uint16_t maxDischargePower;     // W, register 100
    uint16_t maxChargePower;        // W, register 101
    uint16_t balancingEnergy;       // Wh, register 102
    uint32_t chargeEnergy;          // kWh×100, registers 103-104
    uint32_t dischargeEnergy;       // kWh×100, registers 105-106
    
    // Communication Status (registers 110-124)
    uint8_t canopenState;           // CANopen state, register 110
    bool communicationOk;           // Comm status, register 111
    uint16_t packetsReceived;       // Packet count, register 112
    uint16_t parseErrors;           // Error count, register 113
    uint16_t totalFrames;           // Frame count, register 114
    uint32_t lastUpdateTime;        // Timestamp, registers 115-116
    uint16_t frameCounters[7];      // Frame type counters, registers 117-123
    
    // Reserved for future expansion (registers 125-199)
    uint16_t reserved[75];          // Future data expansion
    
} BMSData;

extern BMSData bmsModules[MAX_BMS_NODES];  // Global BMS data array
```

#### Primary Functions:
```cpp
/**
 * @brief Initialize BMS data structures
 * @return bool - true if initialization successful
 */
bool initializeBMSData();

/**
 * @brief Get BMS data pointer by node ID
 * @param nodeId - BMS node ID (1-16)
 * @return BMSData* - pointer to BMS data or nullptr
 */
BMSData* getBMSData(uint8_t nodeId);

/**
 * @brief Get BMS array index by node ID
 * @param nodeId - BMS node ID (1-16)
 * @return int - array index or -1 if not found
 */
int getBMSIndexByNodeId(uint8_t nodeId);

/**
 * @brief Get count of active (communicating) BMS modules
 * @return int - number of active BMS modules
 */
int getActiveBMSCount();

/**
 * @brief Print detailed BMS heartbeat information
 * @param nodeId - BMS node ID to print
 */
void printBMSHeartbeatExtended(uint8_t nodeId);

/**
 * @brief Check communication timeouts for all BMS modules
 */
void checkCommunicationTimeouts();
```

### 5. BMS Protocol Module API (bms_protocol.h/cpp)

#### Primary Functions:
```cpp
/**
 * @brief Setup BMS protocol and CAN interface
 * @return bool - true if setup successful
 */
bool setupBMSProtocol();

/**
 * @brief Process BMS protocol messages (call in main loop)
 */
void processBMSProtocol();

/**
 * @brief Check if BMS protocol is healthy
 * @return bool - true if protocol is functioning
 */
bool isBMSProtocolHealthy();

/**
 * @brief Print BMS protocol statistics
 */
void printBMSProtocolStatistics();

/**
 * @brief Parse specific CAN frame type
 * @param frameId - CAN frame ID
 * @param data - 8-byte CAN data payload
 * @param nodeId - BMS node ID
 * @return bool - true if frame parsed successfully
 */
bool parseCANFrame(uint32_t frameId, uint8_t* data, uint8_t nodeId);
```

#### Supported CAN Frame Types:
```cpp
// Frame ID ranges and their purposes:
#define FRAME_190_RANGE 0x190  // Basic battery parameters
#define FRAME_290_RANGE 0x290  // Cell voltage data
#define FRAME_310_RANGE 0x310  // SOH and temperature
#define FRAME_390_RANGE 0x390  // Maximum values
#define FRAME_410_RANGE 0x410  // Extended temperature
#define FRAME_510_RANGE 0x510  // Power limits and I/O
#define FRAME_490_RANGE 0x490  // Multiplexed data (54 types)
#define FRAME_1B0_RANGE 0x1B0  // Additional data
#define FRAME_710_RANGE 0x710  // CANopen protocol
```

### 6. Utils Module API (utils.h/cpp)

#### System Utilities:
```cpp
/**
 * @brief Setup LED for status indication
 */
void setupLED();

/**
 * @brief Blink LED specified number of times
 * @param count - number of blinks
 * @param delayMs - delay between blinks in milliseconds
 */
void blinkLED(int count, int delayMs);

/**
 * @brief Format uptime duration as human-readable string
 * @param uptimeMs - uptime in milliseconds
 * @return String - formatted uptime (e.g., "1d 2h 30m")
 */
String formatUptime(unsigned long uptimeMs);

/**
 * @brief Format byte count as human-readable string
 * @param bytes - byte count
 * @return String - formatted size (e.g., "1.5KB", "2.3MB")
 */
String formatBytes(size_t bytes);

/**
 * @brief Convert system state enum to string
 * @param state - SystemState_t value
 * @return String - state name
 */
String systemStateToString(SystemState_t state);

/**
 * @brief Print boot progress message
 * @param module - module name
 * @param success - initialization result
 */
void printBootProgress(const char* module, bool success);
```

### 7. Statistics Module API (statistics.h/cpp)

#### Statistics Functions:
```cpp
/**
 * @brief Initialize statistics collection system
 * @return bool - true if initialization successful
 */
bool initializeStatistics();

/**
 * @brief Update communication statistics
 * @param nodeId - BMS node ID
 * @param frameType - CAN frame type
 * @param success - whether frame was processed successfully
 */
void updateCommStats(uint8_t nodeId, uint16_t frameType, bool success);

/**
 * @brief Print comprehensive statistics report
 */
void printStatisticsReport();

/**
 * @brief Get error rate for specific BMS node
 * @param nodeId - BMS node ID (1-16)
 * @return float - error rate as percentage
 */
float getErrorRate(uint8_t nodeId);
```

### 8. Web Server Module API (web_server.h/cpp)

#### Web Server Functions:
```cpp
/**
 * @brief Initialize web server
 * @return bool - true if server started successfully
 */
bool setupWebServer();

/**
 * @brief Process web server requests (call in main loop)
 */
void processWebServer();

/**
 * @brief Check if web server is active
 * @return bool - true if server is running
 */
bool isWebServerActive();

/**
 * @brief Start web server (typically in AP mode)
 */
void startWebServer();

/**
 * @brief Stop web server
 */
void stopWebServer();
```

---

## Usage Examples

### Complete System Initialization:
```cpp
#include "config.h"
#include "wifi_manager.h"
#include "modbus_tcp.h"
#include "bms_protocol.h"
#include "utils.h"

void setup() {
    Serial.begin(115200);
    
    // Initialize LED
    setupLED();
    
    // Load configuration
    if (!loadConfiguration()) {
        Serial.println("Using default configuration");
        saveConfiguration();
    }
    
    // Initialize BMS data
    initializeBMSData();
    
    // Start WiFi
    wifiManager.begin();
    
    // Setup BMS protocol
    if (!setupBMSProtocol()) {
        Serial.println("Failed to setup BMS protocol");
        return;
    }
    
    // Start Modbus TCP server
    if (!setupModbusTCP()) {
        Serial.println("Failed to setup Modbus TCP");
        return;
    }
    
    Serial.println("System ready!");
}

void loop() {
    // Process all modules
    wifiManager.process();
    processBMSProtocol();
    processModbusTCP();
    
    delay(1);  // Minimal delay for optimal responsiveness
}
```

### Reading BMS Data:
```cpp
void printBMSStatus() {
    for (int i = 0; i < systemConfig.activeBmsNodes; i++) {
        uint8_t nodeId = systemConfig.bmsNodeIds[i];
        BMSData* bms = getBMSData(nodeId);
        
        if (bms && bms->communicationOk) {
            float voltage = bms->batteryVoltage / 1000.0;
            float current = bms->batteryCurrent / 1000.0;
            float soc = bms->soc / 100.0;
            
            Serial.printf("BMS %d: %.2fV, %.2fA, SOC: %.1f%%\n", 
                         nodeId, voltage, current, soc);
        }
    }
}
```

### Modbus Register Access:
```cpp
// Read BMS 1 voltage (register 0)
uint16_t voltage_raw = getModbusRegister(1, 0);
float voltage = voltage_raw / 1000.0;  // Convert mV to V

// Calculate address for BMS 3 SOC (register offset 3)
uint16_t address = calculateModbusAddress(3, 3);  // Result: 403
```

---

## Error Handling

### Common Error Codes:
- **Configuration Errors**: Invalid parameters in SystemConfig_t
- **Communication Errors**: CAN bus timeout, WiFi disconnection
- **Protocol Errors**: Invalid Modbus requests, malformed CAN frames
- **Resource Errors**: Memory allocation failures, EEPROM errors

### Best Practices:
1. Always check return values from setup functions
2. Implement timeout handling for communication functions
3. Validate input parameters before processing
4. Use defensive programming for pointer operations
5. Monitor system resources (memory, stack usage)

---

*API documentation maintained as part of Universal Workflow standards*  
*For architectural overview, see [System Architecture](ARCHITECTURE.md)*  
*For usage instructions, see [Setup Guide](SETUP.md)*