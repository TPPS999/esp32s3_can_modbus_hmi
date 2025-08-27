# [SYSTEM_NAME] Troubleshooting Guide

> **Project:** ESP32S3 CAN to Modbus TCP Bridge  
> **System:** [SYSTEM_DESCRIPTION]  
> **Version:** v[VERSION]  
> **Created:** [DATE] (Warsaw Time)  
> **Last Updated:** [DATE] (Warsaw Time)

## 📋 Overview

This troubleshooting guide provides systematic approaches to diagnosing and resolving issues with the [SYSTEM_NAME] system. It covers hardware problems, software errors, communication issues, and performance problems commonly encountered in ESP32S3 embedded systems.

### How to Use This Guide

1. **Identify Symptoms** - Match your issue with symptoms described
2. **Follow Diagnostic Steps** - Execute diagnostic procedures in order
3. **Apply Solutions** - Implement suggested fixes
4. **Verify Resolution** - Confirm the problem is resolved
5. **Document Findings** - Record solutions for future reference

### Emergency Contacts

- **Hardware Issues**: Check physical connections first
- **Software Bugs**: Enable debug output and collect logs
- **System Crashes**: Capture stack traces and memory dumps

## 🔍 Diagnostic Tools and Commands

### Debug Output Activation

```c
// Enable comprehensive debugging
#define DEBUG_[MODULE_NAME] 1
#define DEBUG_CAN_FRAMES 1
#define DEBUG_MODBUS_REQUESTS 1
#define DEBUG_BMS_PARSING 1
#define DEBUG_WIFI_EVENTS 1
```

### System Diagnostic Commands

```c
// Print system status
[moduleName]PrintDiagnostics();

// Check memory usage  
Serial.printf("Free heap: %d bytes\n", ESP.getFreeHeap());

// Get system uptime
Serial.printf("Uptime: %lu ms\n", millis());

// Check stack usage
Serial.printf("Stack usage: %d bytes\n", uxTaskGetStackHighWaterMark(NULL));
```

### Hardware Testing Commands

```c
// Test CAN controller
bool canOk = testCANController();

// Test WiFi connectivity
bool wifiOk = testWiFiConnection();

// Test GPIO pins
bool gpioOk = testGPIOPins();
```

## ⚠️ Common Issues and Solutions

### 1. System Won't Start / Boot Issues

#### Symptoms:
- ESP32S3 doesn't respond to serial communication
- Device appears to boot loop continuously
- LED indicators don't activate
- No WiFi AP or station mode

#### Diagnostic Steps:
1. **Power Supply Check:**
   ```
   - Verify 3.3V power supply is stable
   - Check current capacity (minimum 500mA)
   - Measure voltage at ESP32S3 pins
   - Test with different power source
   ```

2. **Hardware Connections:**
   ```
   - Verify all GPIO connections
   - Check SPI connections to MCP2515:
     * CS Pin: GPIO 44
     * MOSI: GPIO 9  
     * MISO: GPIO 8
     * SCK: GPIO 7
     * INT: GPIO 2
   ```

3. **Firmware Issues:**
   ```
   - Check if bootloader is present
   - Verify correct partition table
   - Test with minimal firmware
   - Check for corrupted flash
   ```

#### Solutions:
- **Power Issues**: Use regulated 3.3V supply with adequate current
- **Hardware**: Re-check all connections with multimeter
- **Firmware**: Reflash bootloader and firmware using esptool
- **Flash Corruption**: Erase flash completely and reflash

### 2. CAN Bus Communication Problems

#### Symptoms:
- No CAN frames received
- CAN error flags set
- Bus-off condition
- Intermittent communication

#### Diagnostic Steps:
1. **Physical Layer Check:**
   ```
   - Verify CAN bus termination (120Ω resistors)
   - Check CANH/CANL voltage levels
   - Test cable continuity and impedance
   - Verify MCP2515 power supply (5V)
   ```

2. **Configuration Verification:**
   ```
   - Confirm CAN bit rate matches network
   - Check SPI communication to MCP2515
   - Verify interrupt pin connection
   - Test with CAN bus analyzer
   ```

3. **Software Diagnostics:**
   ```c
   // Check CAN controller status
   uint8_t canStatus = CAN.readRegister(CANSTAT);
   Serial.printf("CAN Status: 0x%02X\n", canStatus);
   
   // Read error counters
   uint8_t txErrors = CAN.readRegister(TXB0CTRL);
   uint8_t rxErrors = CAN.readRegister(RXB0CTRL);
   ```

#### Solutions:
- **Bus Termination**: Add proper 120Ω terminating resistors
- **Bit Rate Mismatch**: Configure correct CAN speed (125/500 kbps)
- **Hardware Issues**: Check MCP2515 crystal frequency (8/16 MHz)
- **Software**: Implement proper error handling and recovery

### 3. WiFi Connection Issues

#### Symptoms:
- Cannot connect to WiFi network
- Frequent disconnections
- Weak signal strength
- AP mode not working

#### Diagnostic Steps:
1. **Network Configuration:**
   ```c
   // Print WiFi diagnostics
   Serial.printf("SSID: %s\n", WiFi.SSID().c_str());
   Serial.printf("Signal: %d dBm\n", WiFi.RSSI());
   Serial.printf("IP: %s\n", WiFi.localIP().toString().c_str());
   Serial.printf("Status: %d\n", WiFi.status());
   ```

2. **Hardware Check:**
   ```
   - Verify antenna connection
   - Check for interference sources
   - Test in different locations
   - Measure supply voltage under load
   ```

3. **Configuration Validation:**
   ```c
   // Check stored credentials
   Serial.printf("Stored SSID: %s\n", systemConfig.wifiSSID);
   // Don't print password for security
   Serial.println("Password configured: Yes/No");
   ```

#### Solutions:
- **Signal Issues**: Move closer to router or use WiFi extender
- **Credentials**: Verify SSID and password are correct
- **Power Issues**: Ensure stable power during WiFi transmission
- **Interference**: Change WiFi channel or use 5GHz if available

### 4. Modbus TCP Communication Problems

#### Symptoms:
- Modbus clients cannot connect
- Register read/write failures
- Timeout errors
- Incorrect data values

#### Diagnostic Steps:
1. **Network Connectivity:**
   ```c
   // Test TCP server status
   Serial.printf("Modbus server running: %s\n", 
                 server.status() ? "Yes" : "No");
   
   // Check active connections
   Serial.printf("Active clients: %d\n", getActiveClientCount());
   ```

2. **Register Mapping:**
   ```c
   // Verify register addresses
   for (int i = 0; i < MAX_REGISTERS; i++) {
       if (registerMap[i].valid) {
           Serial.printf("Register %d: Value=%d\n", i, registerMap[i].value);
       }
   }
   ```

3. **Data Validation:**
   ```c
   // Check BMS data freshness
   unsigned long dataAge = millis() - lastBMSUpdate;
   Serial.printf("BMS data age: %lu ms\n", dataAge);
   ```

#### Solutions:
- **Network**: Verify ESP32S3 IP address and port 502 accessibility
- **Firewall**: Ensure client firewall allows Modbus TCP connections
- **Data Mapping**: Verify register addresses match client expectations
- **Timeouts**: Increase timeout values for slow networks

### 5. BMS Data Issues

#### Symptoms:
- Missing or stale BMS data
- Incorrect sensor readings
- Node offline status
- Data parsing errors

#### Diagnostic Steps:
1. **CAN Frame Analysis:**
   ```c
   // Enable CAN frame debugging
   #define DEBUG_CAN_FRAMES 1
   
   // Monitor specific frame types
   if (canId >= 0x181 && canId <= 0x190) {
       Serial.printf("BMS Frame 190: ID=0x%03X, Data=", canId);
       for (int i = 0; i < len; i++) {
           Serial.printf("%02X ", buf[i]);
       }
       Serial.println();
   }
   ```

2. **Node Status Check:**
   ```c
   // Check each BMS node
   for (int i = 0; i < systemConfig.activeBmsNodes; i++) {
       uint8_t nodeId = systemConfig.bmsNodeIds[i];
       bool online = isBMSNodeOnline(nodeId);
       Serial.printf("BMS Node %d: %s\n", nodeId, online ? "Online" : "Offline");
   }
   ```

3. **Data Validation:**
   ```c
   // Verify data ranges
   if (voltage < 10.0 || voltage > 60.0) {
       Serial.printf("Invalid voltage: %.2f V\n", voltage);
   }
   ```

#### Solutions:
- **Missing Data**: Check CAN bus connectivity and node addressing
- **Invalid Data**: Implement data validation and filtering
- **Parsing Errors**: Verify frame format matches BMS protocol
- **Node Offline**: Check individual node power and CAN connections

### 6. Memory Issues

#### Symptoms:
- System crashes or resets
- Out of memory errors
- Degraded performance
- Memory leaks

#### Diagnostic Steps:
1. **Memory Usage Analysis:**
   ```c
   // Monitor heap usage
   Serial.printf("Free heap: %d bytes\n", ESP.getFreeHeap());
   Serial.printf("Min free heap: %d bytes\n", ESP.getMinFreeHeap());
   Serial.printf("Max alloc heap: %d bytes\n", ESP.getMaxAllocHeap());
   
   // Check stack usage
   Serial.printf("Stack high water: %d bytes\n", 
                 uxTaskGetStackHighWaterMark(NULL));
   ```

2. **Memory Leak Detection:**
   ```c
   // Monitor heap over time
   static unsigned long lastCheck = 0;
   static uint32_t lastHeap = 0;
   
   if (millis() - lastCheck > 10000) {
       uint32_t currentHeap = ESP.getFreeHeap();
       int32_t heapDelta = currentHeap - lastHeap;
       Serial.printf("Heap change: %d bytes\n", heapDelta);
       lastHeap = currentHeap;
       lastCheck = millis();
   }
   ```

#### Solutions:
- **Low Memory**: Optimize data structures and reduce buffer sizes
- **Memory Leaks**: Review dynamic allocations and ensure proper cleanup
- **Stack Overflow**: Increase task stack sizes or reduce local variables
- **Fragmentation**: Use static allocation where possible

### 7. Performance Issues

#### Symptoms:
- Slow response times
- Missed CAN frames
- Modbus timeouts
- Watchdog resets

#### Diagnostic Steps:
1. **CPU Usage Analysis:**
   ```c
   // Monitor task execution times
   unsigned long startTime = micros();
   performTask();
   unsigned long taskTime = micros() - startTime;
   Serial.printf("Task execution: %lu μs\n", taskTime);
   ```

2. **Interrupt Analysis:**
   ```c
   // Check interrupt frequency
   static volatile uint32_t interruptCount = 0;
   
   void IRAM_ATTR canInterrupt() {
       interruptCount++;
   }
   
   // Print interrupt rate
   Serial.printf("Interrupts per second: %lu\n", interruptCount);
   interruptCount = 0;
   ```

#### Solutions:
- **CPU Overload**: Optimize algorithms and reduce processing frequency
- **Interrupt Storm**: Implement proper interrupt handling and debouncing
- **Blocking Operations**: Use non-blocking operations and task scheduling
- **Watchdog**: Increase watchdog timeout or add task yield points

## 🔧 Advanced Diagnostics

### System Health Monitoring

```c
void printSystemHealth() {
    Serial.println("\n=== SYSTEM HEALTH REPORT ===");
    
    // Memory
    Serial.printf("Free Heap: %d bytes\n", ESP.getFreeHeap());
    Serial.printf("Min Free Heap: %d bytes\n", ESP.getMinFreeHeap());
    
    // CPU
    Serial.printf("CPU Frequency: %d MHz\n", ESP.getCpuFreqMHz());
    Serial.printf("Chip Temperature: %.1f °C\n", temperatureRead());
    
    // Network
    Serial.printf("WiFi Status: %d\n", WiFi.status());
    Serial.printf("WiFi RSSI: %d dBm\n", WiFi.RSSI());
    
    // CAN Bus
    Serial.printf("CAN Frames RX: %lu\n", canFramesReceived);
    Serial.printf("CAN Errors: %lu\n", canErrorCount);
    
    // Modbus
    Serial.printf("Modbus Requests: %lu\n", modbusRequestCount);
    Serial.printf("Modbus Errors: %lu\n", modbusErrorCount);
    
    // BMS
    for (int i = 0; i < systemConfig.activeBmsNodes; i++) {
        uint8_t nodeId = systemConfig.bmsNodeIds[i];
        Serial.printf("BMS Node %d: %s\n", nodeId, 
                     isBMSNodeOnline(nodeId) ? "Online" : "Offline");
    }
    
    Serial.println("=========================\n");
}
```

### Network Diagnostics

```c
void networkDiagnostics() {
    // WiFi Information
    Serial.printf("SSID: %s\n", WiFi.SSID().c_str());
    Serial.printf("IP Address: %s\n", WiFi.localIP().toString().c_str());
    Serial.printf("Gateway: %s\n", WiFi.gatewayIP().toString().c_str());
    Serial.printf("DNS: %s\n", WiFi.dnsIP().toString().c_str());
    
    // Connection Test
    WiFiClient client;
    bool connected = client.connect(WiFi.gatewayIP(), 80);
    Serial.printf("Gateway Reachable: %s\n", connected ? "Yes" : "No");
    client.stop();
}
```

### Hardware Test Suite

```c
bool runHardwareTests() {
    bool allTestsPassed = true;
    
    Serial.println("Running Hardware Test Suite...");
    
    // Test 1: GPIO Pins
    Serial.print("GPIO Test: ");
    if (testGPIOPins()) {
        Serial.println("PASS");
    } else {
        Serial.println("FAIL");
        allTestsPassed = false;
    }
    
    // Test 2: SPI Communication
    Serial.print("SPI Test: ");
    if (testSPICommunication()) {
        Serial.println("PASS");
    } else {
        Serial.println("FAIL");
        allTestsPassed = false;
    }
    
    // Test 3: CAN Controller
    Serial.print("CAN Test: ");
    if (testCANController()) {
        Serial.println("PASS");
    } else {
        Serial.println("FAIL");
        allTestsPassed = false;
    }
    
    // Test 4: WiFi Module
    Serial.print("WiFi Test: ");
    if (testWiFiModule()) {
        Serial.println("PASS");
    } else {
        Serial.println("FAIL");
        allTestsPassed = false;
    }
    
    return allTestsPassed;
}
```

## 📞 Support and Recovery

### Emergency Recovery Procedures

#### Factory Reset
1. Hold reset button for 10 seconds
2. Device will enter factory reset mode
3. All configuration will be restored to defaults
4. WiFi credentials will be cleared

#### Firmware Recovery
1. Enter download mode (GPIO0 low during reset)
2. Use esptool to flash firmware:
   ```bash
   esptool.py --chip esp32s3 --port COM3 --baud 921600 write_flash 0x0 firmware.bin
   ```

#### Configuration Backup/Restore
```c
// Backup configuration
void backupConfiguration() {
    File file = SPIFFS.open("/config_backup.json", "w");
    // Serialize configuration to JSON
    file.close();
}

// Restore configuration
void restoreConfiguration() {
    File file = SPIFFS.open("/config_backup.json", "r");
    // Deserialize configuration from JSON
    file.close();
}
```

### Support Resources

- **Documentation**: Complete system documentation in `/docs` folder
- **Source Code**: All source code available with inline comments
- **Test Scripts**: Automated test scripts for validation
- **Community**: Project repository issues and discussions

### Creating Support Requests

When reporting issues, include:

1. **System Information**: Hardware version, firmware version
2. **Error Messages**: Exact error messages and codes
3. **Steps to Reproduce**: Detailed reproduction steps
4. **Environment**: Network setup, BMS configuration
5. **Logs**: Debug output and error logs
6. **Measurements**: Voltage levels, signal measurements

---

**Template Usage Instructions:**

To use this troubleshooting template:
1. Replace [PLACEHOLDER] tokens with system-specific values
2. Add system-specific diagnostic procedures
3. Include actual error codes and messages encountered
4. Add hardware-specific test procedures
5. Update support contact information
6. Include real troubleshooting cases from experience
7. Remove this instruction section

Key areas to customize:
- System-specific symptoms and solutions
- Hardware test procedures
- Diagnostic commands and tools
- Recovery procedures
- Support resources and contacts
- Performance benchmarks and thresholds