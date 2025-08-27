# ESP32S3 CAN-Modbus TCP - Setup Guide

**Project:** ESP32S3 CAN to Modbus TCP Bridge  
**Repository:** https://github.com/user/esp32s3-can-modbus-tcp  
**Version:** Setup Guide v1.0  
**Created:** 27.08.2025 (Warsaw Time)  
**Last Updated:** 27.08.2025 (Warsaw Time)  
**Status:** Final

---

## Document Information

### Purpose:
This document provides step-by-step setup and configuration instructions for the ESP32S3 CAN-Modbus TCP Bridge system.

### Scope:
- Hardware setup and connections
- Software installation and compilation
- System configuration and testing
- Troubleshooting common issues

### Audience:
- System integrators
- Field technicians
- Maintenance personnel
- End users setting up the system

### Related Documents:
- [System Architecture](ARCHITECTURE.md) - Technical system overview
- [API Documentation](API.md) - Programming interfaces
- [Main README](../README.md) - Project overview and specifications

---

## Hardware Requirements

### ESP32S3 Board:
- **Recommended**: Seeed Studio XIAO ESP32S3
- **CPU**: Dual-core 240 MHz
- **RAM**: 512KB
- **Flash**: 8MB
- **Connectivity**: WiFi 802.11 b/g/n

### CAN Bus Interface:
- **CAN Controller**: MCP2515
- **CAN Transceiver**: TJA1050 or compatible
- **Termination**: 120Ω resistors (if at bus endpoints)

### Additional Components:
- **Power Supply**: 3.3V regulated, minimum 500mA
- **Status LED**: Built-in LED on GPIO13 (optional external)
- **Debug**: USB connection for programming and serial monitor

## Hardware Setup

### 1. ESP32S3 to MCP2515 Connections

```
ESP32S3 Pin    MCP2515 Pin    Function        Wire Color
═══════════    ═══════════    ═══════════    ══════════
GPIO5          CS             Chip Select     Orange
GPIO6          SCK            SPI Clock       Yellow  
GPIO7          MOSI           SPI MOSI        Green
GPIO8          MISO           SPI MISO        Blue
GPIO9          INT            Interrupt       Purple
3.3V           VCC            Power Supply    Red
GND            GND            Ground          Black
```

### 2. CAN Bus Connections

```
CAN Pin    Signal    Color (Typical)    Description
═══════    ══════    ═════════════     ═══════════
1          CAN_H     Yellow            CAN High
2          CAN_L     Green             CAN Low  
3          GND       Black             Ground (optional)
4          +12V      Red               Power (optional)
```

**Important Notes:**
- Use twisted pair cable for CAN_H and CAN_L
- Add 120Ω termination resistors at both ends of CAN bus
- Maximum CAN bus length: 40m @ 500 kbps, 1000m @ 125 kbps
- Verify CAN bus voltage levels (2.5V ±2V differential)

### 3. Power Supply

```
Input: 5V DC (USB) or 3.3V regulated
Current Requirements:
├── Normal Operation: ~180mA
├── Peak WiFi TX: ~250mA  
├── Sleep Mode: ~10mA
└── Recommended PSU: 500mA minimum
```

## Software Setup

### 1. Development Environment

#### PlatformIO Installation:
```bash
# Install PlatformIO Core
pip install platformio

# Or install PlatformIO IDE extension for VSCode
# Search for "PlatformIO IDE" in VSCode extensions
```

#### Arduino IDE Alternative:
```bash
# Install ESP32 Arduino Core in Arduino IDE
# Add to Board Manager URLs:
# https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
```

### 2. Project Setup

#### Clone Repository:
```bash
git clone https://github.com/user/esp32s3-can-modbus-tcp.git
cd esp32s3-can-modbus-tcp
```

#### PlatformIO Commands:
```bash
# Initialize project (if needed)
pio init --project-dir . --board seeed_xiao_esp32s3

# Install dependencies
pio lib install

# Compile project
pio run

# Upload to ESP32S3
pio run --target upload

# Open serial monitor
pio device monitor --baud 115200 --filter esp32_exception_decoder
```

### 3. Configuration

#### WiFi Configuration (Method 1 - Code):
Edit `include/config.h`:
```cpp
const char* const WIFI_SSID = "YourWiFiNetwork";
const char* const WIFI_PASSWORD = "YourWiFiPassword";
```

#### WiFi Configuration (Method 2 - CAN Trigger):
1. Send CAN trigger frames (ID: 0xEF1, Data: 0xFF 0xBB)
2. Connect to AP: "ESP32S3-CAN-XXXXXX-TRIGGER"
3. Password: "esp32modbus"  
4. Open browser: http://192.168.4.1/wifi
5. Enter WiFi credentials and save

#### BMS Configuration:
Edit `include/config.h` or use web interface:
```cpp
#define MAX_BMS_NODES 16                    // Maximum BMS modules
#define BMS_COMMUNICATION_TIMEOUT_MS 30000  // Communication timeout

// Configure active BMS nodes
systemConfig.activeBmsNodes = 4;
systemConfig.bmsNodeIds[0] = 1;  // First BMS at Node ID 1
systemConfig.bmsNodeIds[1] = 2;  // Second BMS at Node ID 2
systemConfig.bmsNodeIds[2] = 3;  // Third BMS at Node ID 3  
systemConfig.bmsNodeIds[3] = 4;  // Fourth BMS at Node ID 4
```

## System Testing

### 1. Basic Functionality Test

#### Serial Monitor Output:
```
🚀 ESP32S3 STARTING...
═══════════════════════════════════════════════════════════
🚀 ESP32S3 CAN to Modbus TCP Bridge
📋 Version: v4.0.2
📅 Build Date: Aug 27 2025
═══════════════════════════════════════════════════════════

✅ LED System OK
✅ Configuration System OK
✅ BMS Data Manager OK  
✅ WiFi Manager OK
✅ BMS Protocol + CAN OK
✅ Modbus TCP Server OK

✅ System initialization completed successfully!
🚀 ESP32S3 CAN to Modbus TCP Bridge is READY!
```

### 2. Network Connectivity Test

#### Check WiFi Connection:
```bash
# Ping ESP32S3 (replace with actual IP)
ping 192.168.1.100

# Test Modbus TCP connection (using modpoll or similar)
modpoll -m tcp -a 1 -r 1 -c 10 192.168.1.100
```

### 3. CAN Bus Communication Test

#### Monitor Serial Output:
```
📊 Active BMS: 4/4
🔋 ACTIVE BATTERIES STATUS:
🔋 BMS1 [Modbus:0]: 48.2V, 15.3A, SOC: 85.2%, SOH: 98.1%
🔋 BMS2 [Modbus:200]: 48.4V, 12.8A, SOC: 87.5%, SOH: 97.8%
🔋 BMS3 [Modbus:400]: 48.1V, 14.2A, SOC: 84.8%, SOH: 98.5%
🔋 BMS4 [Modbus:600]: 48.3V, 13.9A, SOC: 86.1%, SOH: 98.2%
```

### 4. Modbus Register Test

#### Using Python (pymodbus):
```python
from pymodbus.client.sync import ModbusTcpClient

client = ModbusTcpClient('192.168.1.100', port=502)
client.connect()

# Read BMS 1 voltage (register 0)
result = client.read_holding_registers(0, 1, unit=1)
voltage = result.registers[0] / 1000.0
print(f"BMS 1 Voltage: {voltage:.2f} V")

# Read BMS 2 SOC (register 203 = 200 + 3)
result = client.read_holding_registers(203, 1, unit=1)
soc = result.registers[0] / 100.0
print(f"BMS 2 SOC: {soc:.1f} %")

client.close()
```

## Configuration Options

### 1. System Parameters

| Parameter | Default | Range | Description |
|-----------|---------|-------|-------------|
| MAX_BMS_NODES | 16 | 1-16 | Maximum supported BMS modules |
| CAN_SPEED | 125000 | 125000/500000 | CAN bus speed in bps |
| MODBUS_TCP_PORT | 502 | 1-65535 | Modbus TCP server port |
| WIFI_TIMEOUT | 10000 | 5000-30000 | WiFi connection timeout (ms) |
| AP_TIMEOUT | 30000 | 10000-300000 | AP mode timeout (ms) |

### 2. Modbus Register Mapping

```
BMS Module Register Layout (200 registers per BMS):
├── BMS 1: Registers 0-199      (Node ID 1)
├── BMS 2: Registers 200-399    (Node ID 2)  
├── BMS 3: Registers 400-599    (Node ID 3)
├── ...
└── BMS 16: Registers 3000-3199 (Node ID 16)

Key Register Offsets (add to base address):
├── Voltage: +0     (mV)
├── Current: +1     (mA, signed)
├── Energy: +2      (Wh×100)
├── SOC: +3         (percentage×100)
├── SOH: +20        (percentage×100)
├── Temperature: +22 (°C×10, signed)
└── Communication: +111 (0/1 boolean)
```

## Troubleshooting

### 1. Common Issues

#### WiFi Connection Problems:
```
Symptoms: "WiFi disconnected" in serial output
Solutions:
├── Check SSID and password in config.h
├── Verify WiFi network is 2.4GHz (not 5GHz)
├── Check signal strength (RSSI > -70 dBm)
├── Try AP mode for configuration
└── Reset configuration to defaults
```

#### CAN Bus Communication Issues:
```
Symptoms: "No active BMS communication detected"
Solutions:
├── Check hardware connections (especially CS and INT)
├── Verify CAN bus termination (120Ω resistors)
├── Check CAN bus speed configuration (125/500 kbps)
├── Verify BMS node IDs match configuration
└── Use oscilloscope to check CAN signal integrity
```

#### Modbus TCP Connection Problems:
```
Symptoms: Modbus client cannot connect
Solutions:
├── Check network connectivity (ping test)
├── Verify port 502 is not blocked by firewall
├── Check Modbus client configuration (slave ID = 1)
├── Verify register addresses are correct
└── Monitor serial output for connection logs
```

### 2. Diagnostic Commands

#### Serial Monitor Debug:
```bash
# Open serial monitor with exception decoder
pio device monitor --baud 115200 --filter esp32_exception_decoder

# Look for these debug messages:
# - System initialization status
# - WiFi connection details
# - CAN frame reception logs
# - Modbus request/response logs
# - Error messages and stack traces
```

#### Network Testing:
```bash
# Test network connectivity
ping [ESP32_IP_ADDRESS]

# Test Modbus TCP (using modpoll)
modpoll -m tcp -a 1 -r 0 -c 4 [ESP32_IP_ADDRESS]

# Scan for CAN-triggered AP
# Send: cansend can0 EF1#FFBB000000000000 (3 times)
# Look for: ESP32S3-CAN-XXXXXX-TRIGGER network
```

### 3. Factory Reset

#### Hardware Reset:
1. Hold BOOT button while pressing RESET
2. Release RESET, then release BOOT
3. Device will start with default configuration

#### Software Reset:
```cpp
// In code, call configuration reset
resetConfiguration();
saveConfiguration();
ESP.restart();
```

## Maintenance

### 1. Regular Monitoring

#### Key Metrics to Monitor:
- WiFi signal strength (RSSI)
- CAN bus error rates  
- Memory usage (heap free)
- Communication timeouts
- BMS data freshness

#### Log Analysis:
```bash
# Monitor system health via serial
pio device monitor --baud 115200 | grep "💓\|❌\|⚠️"

# Watch for error patterns:
# - Repeated WiFi disconnections
# - CAN communication timeouts
# - Memory allocation failures
# - Modbus client errors
```

### 2. Firmware Updates

#### Update Procedure:
1. Download latest firmware
2. Connect ESP32S3 via USB
3. Compile and upload: `pio run --target upload`
4. Verify functionality via serial monitor
5. Test all communication interfaces

### 3. Configuration Backup

#### Export Configuration:
```bash
# Via web interface (when in AP mode):
# http://192.168.4.1/status
# Click "Export Configuration" button

# Via serial monitor:
# Look for configuration dump in startup logs
```

---

## Support

### Getting Help:
- **Documentation**: Check docs/ folder for detailed guides  
- **Issues**: Report bugs via GitHub issues
- **Serial Logs**: Include serial monitor output in bug reports
- **Network Tools**: Use Wireshark for Modbus TCP debugging

### Maintenance Schedule:
- **Daily**: Monitor system status via serial or network
- **Weekly**: Check CAN bus signal quality and error rates
- **Monthly**: Verify configuration backup and update if needed
- **Quarterly**: Review system performance and plan improvements

---

*Setup guide maintained as part of Universal Workflow standards*  
*For technical details, see [System Architecture](ARCHITECTURE.md)*  
*For programming information, see [API Documentation](API.md)*