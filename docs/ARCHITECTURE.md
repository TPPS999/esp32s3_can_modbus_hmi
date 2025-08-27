# ESP32S3 CAN-Modbus TCP - System Architecture

**Project:** ESP32S3 CAN to Modbus TCP Bridge  
**Repository:** https://github.com/user/esp32s3-can-modbus-tcp  
**Version:** Architecture Documentation v1.0  
**Created:** 27.08.2025 (Warsaw Time)  
**Last Updated:** 27.08.2025 (Warsaw Time)  
**Status:** Final

---

## Document Information

### Purpose:
This document provides comprehensive system architecture overview for ESP32S3 CAN-Modbus TCP Bridge project, detailing all system components, their interactions, and design principles.

### Scope:
- System overview and design principles
- Module architecture and responsibilities
- Data flow and communication protocols
- Hardware abstraction and interfaces
- Performance characteristics and constraints

### Audience:
- Embedded system developers
- System architects
- Hardware integration engineers
- Maintenance and support personnel

### Related Documents:
- [API Documentation](API.md) - Module APIs and programming interfaces
- [Setup Guide](SETUP.md) - Installation and configuration instructions
- [Session Templates](../SESSION_TEMPLATES.md) - Development workflow patterns
- [Main README](../README.md) - Project overview and specifications

---

## System Overview

### High-Level Architecture

```
┌─────────────────────────────────────────────────────────────────┐
│                    ESP32S3 CAN-Modbus TCP Bridge                │
├─────────────────────────────────────────────────────────────────┤
│                                                                 │
│  ┌─────────────┐    ┌──────────────┐    ┌─────────────────┐    │
│  │   CAN Bus   │◄──►│  BMS Protocol│◄──►│   BMS Data      │    │
│  │  Interface  │    │   Processing │    │   Management    │    │
│  │ (MCP2515)   │    │              │    │                 │    │
│  └─────────────┘    └──────────────┘    └─────────────────┘    │
│         ▲                   ▲                      ▲            │
│         │                   │                      ▼            │
│  ┌──────▼─────┐    ┌────────▼──────┐    ┌─────────▼──────┐     │
│  │ Hardware   │    │ Main Control  │    │ Modbus TCP     │     │
│  │ Abstraction│    │ Loop          │    │ Server         │     │
│  │ Layer      │    │ (main.cpp)    │    │                │     │
│  └────────────┘    └───────────────┘    └────────────────┘     │
│                              ▲                      ▲            │
│  ┌─────────────┐    ┌────────▼──────┐              │            │
│  │ WiFi        │◄──►│ Configuration │              ▼            │
│  │ Management  │    │ & Utilities   │    ┌─────────────────┐    │
│  │ (AP/Station)│    │               │    │    Network      │    │
│  └─────────────┘    └───────────────┘    │   Clients       │    │
│                                          │ (SCADA/PLC)     │    │
│                                          └─────────────────┘    │
└─────────────────────────────────────────────────────────────────┘
```

### Core Design Principles:
1. **Modular Architecture**: Clear separation of concerns across 7 main modules
2. **Real-Time Processing**: CAN bus processing with 1ms loop responsiveness
3. **Fault Tolerance**: Automatic recovery and fallback mechanisms
4. **Scalability**: Support for up to 16 BMS modules with dynamic configuration
5. **Professional Standards**: Comprehensive documentation and error handling

## Module Architecture

### 1. config Module (config.h/cpp)
**Primary Responsibility:** System configuration and EEPROM persistence

```cpp
// Key Components:
- SystemConfig_t structure (WiFi, BMS, Modbus settings)
- EEPROM read/write operations
- Default configuration management
- Configuration validation
```

**Dependencies:**
- Internal: None (foundational module)
- External: Arduino EEPROM library

**Key Features:**
- Persistent storage of WiFi credentials
- BMS node configuration (up to 16 nodes)
- CAN bus speed settings (125/500 kbps)
- Configuration validation and error recovery

### 2. wifi_manager Module (wifi_manager.h/cpp)  
**Primary Responsibility:** WiFi connectivity with AP fallback mode

```cpp
// Key Components:
- WiFiManager class with state management
- Station mode with automatic reconnection
- AP mode fallback (ESP32S3-CAN-XXXXXX)
- CAN-triggered AP mode (ID: 0xEF1)
```

**Dependencies:**
- Internal: config module
- External: WiFi.h, ESP32 WiFi stack

**Key Features:**
- Automatic WiFi connection management
- Signal strength monitoring and diagnostics
- AP mode for configuration and emergency access
- CAN-triggered AP mode for remote configuration

### 3. modbus_tcp Module (modbus_tcp.h/cpp)
**Primary Responsibility:** Modbus TCP server implementation

```cpp
// Key Components:
- Modbus TCP server on port 502
- 3200 holding registers (16 BMS × 200 registers each)
- Function codes: 0x03 (Read), 0x06 (Write Single), 0x10 (Write Multiple)
- Register mapping and data conversion
```

**Dependencies:**
- Internal: bms_data, config modules
- External: AsyncTCP, WiFi connectivity

**Key Features:**
- Standard Modbus TCP protocol compliance
- Real-time data mapping from BMS modules
- Concurrent client support
- Register address calculation and validation

### 4. bms_data Module (bms_data.h)
**Primary Responsibility:** BMS data structures and management (Header-only)

```cpp
// Key Components:
- BMSData structure (80+ fields per BMS)
- Data validation and range checking
- Timestamp management
- Communication status tracking
```

**Dependencies:**
- Internal: None (header-only design)
- External: Arduino standard types

**Key Features:**
- Comprehensive BMS parameter storage
- Data integrity validation
- Memory-efficient structure design
- Type-safe data access

### 5. bms_protocol Module (bms_protocol.h/cpp)
**Primary Responsibility:** CAN protocol parsing and BMS communication

```cpp
// Key Components:
- CAN message parsing (9 frame types)
- MCP2515 interface management
- BMS protocol state machine
- Multiplexer data handling (54 types)
```

**Dependencies:**
- Internal: bms_data, config modules
- External: MCP2515 library, SPI interface

**Key Features:**
- Multi-frame BMS protocol support (0x190, 0x290, 0x310, etc.)
- Real-time CAN message processing
- Hardware CAN controller management
- Protocol-specific data parsing and validation

### 6. utils Module (utils.h/cpp)
**Primary Responsibility:** System utilities and diagnostics

```cpp
// Key Components:
- System diagnostics and monitoring
- LED status indication
- Time and memory formatting utilities
- Debug and logging functions
```

**Dependencies:**
- Internal: None (utility module)
- External: Arduino core libraries

**Key Features:**
- System health monitoring
- Performance measurement tools
- User feedback through LED indication
- Development and debugging support

### 7. statistics Module (statistics.h/cpp)
**Primary Responsibility:** System performance and usage statistics

```cpp
// Key Components:
- Communication statistics tracking
- Performance metrics collection
- Error rate monitoring
- Historical data analysis
```

**Dependencies:**
- Internal: bms_data, modbus_tcp modules
- External: Arduino core

**Key Features:**
- Real-time statistics collection
- Performance trend analysis
- System health metrics
- Diagnostic data for maintenance

### 8. web_server Module (web_server.h/cpp)
**Primary Responsibility:** Web-based configuration interface

```cpp
// Key Components:
- HTTP server for configuration pages
- JSON API endpoints
- Real-time status monitoring
- Configuration form handling
```

**Dependencies:**
- Internal: config, wifi_manager, bms_data modules
- External: ESP Async WebServer library

**Key Features:**
- User-friendly web configuration interface
- Real-time system status display
- Configuration export/import functionality
- Mobile-responsive design

## Data Flow Architecture

### CAN Bus to Modbus TCP Data Path:

```
CAN Frame → MCP2515 → SPI → ESP32S3 → BMS Protocol Parser → BMS Data Structure → Modbus Register Map → TCP Client
```

**Step-by-Step Data Flow:**
1. **CAN Reception**: MCP2515 receives CAN frames from BMS modules
2. **Hardware Interface**: SPI communication transfers data to ESP32S3
3. **Protocol Processing**: bms_protocol module parses frame types and multiplexed data
4. **Data Storage**: Parsed data stored in BMSData structures with validation
5. **Register Mapping**: BMS data mapped to Modbus holding registers (200 per BMS)
6. **Network Transmission**: modbus_tcp module serves data to network clients

### Configuration Data Flow:

```
Web Interface → HTTP POST → Config Module → EEPROM → System Restart → Active Configuration
```

## Communication Protocols

### CAN Bus Protocol Details:
- **Speed**: 125 kbps (default), 500 kbps configurable
- **Frame Types**: Standard 11-bit identifiers
- **Supported Frames**:
  - 0x190-0x19F: Basic BMS data (voltage, current, SOC)
  - 0x290-0x29F: Cell voltage data
  - 0x310-0x31F: Temperature and SOH data
  - 0x390-0x39F: Maximum values
  - 0x410-0x41F: Extended temperature data
  - 0x510-0x51F: Power limits
  - 0x490-0x49F: Multiplexed data (54 types)
  - 0x1B0-0x1BF: Additional data
  - 0x710-0x71F: CANopen protocol frames

### Modbus TCP Protocol Details:
- **Port**: 502 (standard Modbus TCP)
- **Slave ID**: 1
- **Function Codes**: 0x03 (Read Holding), 0x06 (Write Single), 0x10 (Write Multiple)
- **Register Layout**: 3200 registers total (16 BMS × 200 registers each)
- **Data Types**: 16-bit signed/unsigned integers with scaling factors

### WiFi Protocol Details:
- **Standards**: 802.11 b/g/n (2.4 GHz)
- **Modes**: Station (primary), AP (fallback/configuration)
- **Security**: WPA2-PSK for station mode
- **DHCP**: Client mode with static IP option

## Hardware Architecture

### ESP32S3 System Resources:
- **CPU**: Dual-core Xtensa LX7, 240 MHz
- **Memory**: 512KB SRAM, 384KB ROM
- **Flash**: 8MB external flash memory
- **Connectivity**: WiFi 802.11 b/g/n, Bluetooth 5.0

### Pin Assignments:
```
CAN Bus Interface (MCP2515):
├── GPIO5  → CS (Chip Select)
├── GPIO6  → SCK (SPI Clock)
├── GPIO7  → MOSI (SPI Master Out)
├── GPIO8  → MISO (SPI Master In)
├── GPIO9  → INT (Interrupt)
├── 3.3V   → VCC (Power Supply)
└── GND    → GND (Ground)

Status LED:
└── GPIO13 → Built-in LED (Status indication)

Debug Interface:
├── GPIO43 → UART0 TX (Serial debug output)
└── GPIO44 → UART0 RX (Serial debug input)
```

### Power Requirements:
- **Operating Voltage**: 3.3V (regulated)
- **Current Consumption**: 
  - Active (WiFi + CAN): ~180mA
  - Sleep mode: ~10mA
  - Peak transmission: ~250mA

## Security Considerations

### Network Security:
- WPA2-PSK encryption for WiFi connections
- Configurable WiFi credentials stored in encrypted EEPROM
- AP mode with password protection
- Network isolation through VLAN support (hardware dependent)

### Data Security:
- Input validation for all Modbus requests
- Range checking for configuration parameters
- CRC validation for critical data structures
- Secure boot capability (hardware dependent)

### Access Control:
- Physical access required for initial configuration
- Web interface password protection
- CAN-triggered AP mode for emergency access only
- Session timeout for web configuration

## Performance Characteristics

### Real-Time Performance:
- **CAN Processing**: < 1ms response time
- **Modbus Response**: < 10ms typical
- **WiFi Reconnection**: < 5 seconds
- **System Boot Time**: < 3 seconds

### Memory Usage:
- **RAM Usage**: ~18% (with web server active)
- **Flash Usage**: ~30% (including all libraries)
- **EEPROM Usage**: < 1KB for configuration
- **Stack Usage**: < 4KB per task

### Throughput Capabilities:
- **CAN Bus**: 125/500 kbps line rate
- **Modbus TCP**: 100+ requests/second
- **WiFi**: Up to 54 Mbps (802.11g)
- **Concurrent Clients**: Up to 8 Modbus connections

### Scaling Limits:
- **Maximum BMS Modules**: 16 (hardware/protocol limit)
- **Maximum Modbus Registers**: 3200 (system design)
- **Maximum Network Clients**: 8 concurrent connections
- **Configuration Storage**: 4KB EEPROM capacity

## Error Handling and Recovery

### Automatic Recovery Mechanisms:
1. **CAN Communication Errors**: Automatic retry with exponential backoff
2. **WiFi Disconnection**: Automatic reconnection with AP fallback
3. **Modbus Timeout**: Graceful client disconnection and cleanup
4. **Configuration Corruption**: Automatic default configuration restoration

### Diagnostic Capabilities:
- Real-time system health monitoring
- Communication statistics and error rates
- Memory usage and performance metrics
- Hardware component status verification

### Maintenance Features:
- Remote configuration via CAN-triggered AP mode
- Web-based diagnostic interface
- Serial debug output for development
- System restart and recovery commands

---

## Conclusion

The ESP32S3 CAN-Modbus TCP Bridge implements a robust, scalable architecture designed for industrial BMS integration applications. The modular design ensures maintainability and extensibility while meeting real-time performance requirements.

### Key Architectural Benefits:
- **Reliability**: Fault-tolerant design with automatic recovery
- **Scalability**: Support for up to 16 BMS modules
- **Maintainability**: Clear module separation and comprehensive documentation
- **Performance**: Real-time processing with minimal latency
- **Flexibility**: Configurable parameters and multiple access methods

---

*Architecture documentation maintained as part of Universal Workflow standards*  
*For implementation details, see [API Documentation](API.md)*  
*For setup instructions, see [Setup Guide](SETUP.md)*