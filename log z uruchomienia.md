ELF file SHA256: dc1e094a18b4d8e9

Rebooting...
ESP-ROM:esp32s3-20210327
Build:Mar 27 2021
rst:0xc (RTC_SW_CPU_RST),boot:0x8 (SPI_FAST_FLASH_BOOT)
Saved PC:0x4207f5d2
  #0  0x4207f5d2 in esp_pm_impl_waiti at /home/runner/work/esp32-arduino-lib-builder/esp32-arduino-lib-builder/esp-idf/components/esp_pm/pm_impl.c:855

SPIWP:0xee
mode:DIO, clock div:1
load:0x3fce3808,len:0x4bc
load:0x403c9700,len:0xbd8
load:0x403cc700,len:0x2a0c
entry 0x403c98d0
[   189][I][esp32-hal-psram.c:96] psramInit(): PSRAM enabled

🚀 ESP32S3 STARTING...


🚀 ESP32S3 CAN to Modbus TCP Bridge
📋 Version: v4.0.2 - CAN Handler Removed
📅 Build Date: Aug 29 2025 23:37:57
🏭 Device: ESP32S3-CAN-MODBUS-TCP
🏗️ Architecture: Modular (5 modules)


📦 Module Overview:
   🔧 config.h/cpp         - System configuration
   📡 wifi_manager.h/cpp   - WiFi management
   🔗 modbus_tcp.h/cpp     - Modbus TCP server
   📊 bms_data.h           - 🔥 BMS data (80+ pól)
   🛠️ bms_protocol.h/cpp   - 🔥 CAN + BMS protocol (9 parserów + 54 mux)
   🛠️ utils.h/cpp          - Utility functions

❌ REMOVED MODULES:
   🚫 can_handler.h/cpp    - Duplikat (funkcje w bms_protocol)

🎯 System Capabilities:
   🔋 16 BMS modules support
   📊 3200 Modbus registers (200 per BMS)
   🚌 9 CAN frame types (190,290,310,390,410,510,490,1B0,710)
   🔥 54 multiplexer types (Frame 490)
   📡 WiFi + AP fallback mode
   🎯 CAN-triggered AP mode (CAN ID: 0xEF1)
   🔗 Modbus TCP Server (port 502)

🔧 Initializing ESP32S3 CAN to Modbus TCP Bridge...
📋 System Architecture: Modular v4.0.2 (can_handler removed)

[00:00:01] LED System: ✅ OK
✅ LED initialized on GPIO21
📚 Loading configuration from EEPROM...
🔍 Validating configuration...
✅ Configuration validated successfully
✅ Configuration loaded: WiFi=WNK3, BMS=4, CAN=13
[00:00:01] Configuration System: ✅ OK
[00:00:01] AP Trigger System: ✅ OK
📡 AP Trigger system initialized
   Trigger CAN ID: 0xEF1
   Required pattern: 0xFF 0xBB
   Required count: 3 within 1000 ms
   AP duration: 30000 ms
🔧 Initializing system modules...
📊 BMS Data Manager... ✅ OK
📡 WiFi Manager... 📡 WiFi callbacks configured
📡 Initializing WiFi Manager...
📡 WiFi credentials updated: SSID=WNK3
[  1239][D][WiFiGeneric.cpp:1040] _eventCallback(): Arduino Event: 0 - WIFI_READY
[  1280][D][WiFiGeneric.cpp:1040] _eventCallback(): Arduino Event: 2 - STA_START
📡 Connecting to WiFi: WNK3
📡 WiFi credentials updated: SSID=WNK3
📡 WiFi state changed: Disconnected → Connecting
📡 WiFi state changed: 0 -> 1
[  7419][D][WiFiGeneric.cpp:1040] _eventCallback(): Arduino Event: 5 - STA_DISCONNECTED
[  7427][W][WiFiGeneric.cpp:1062] _eventCallback(): Reason: 15 - 4WAY_HANDSHAKE_TIMEOUT
[  7435][D][WiFiGeneric.cpp:1082] _eventCallback(): WiFi Reconnect Running
❌ WiFi disconnected
📡 WiFi state changed: Connecting → Disconnected
📡 WiFi state changed: 1 -> 0
[  7824][D][WiFiGeneric.cpp:1040] _eventCallback(): Arduino Event: 4 - STA_CONNECTED
📡 WiFi station connected to AP
[  7844][D][WiFiGeneric.cpp:1040] _eventCallback(): Arduino Event: 7 - STA_GOT_IP
[  7851][D][WiFiGeneric.cpp:1103] _eventCallback(): STA IP: 192.168.148.42, MASK: 255.255.255.0, GW: 192.168.148.1
✅ WiFi connected to WNK3
📡 IP Address: 192.168.148.42
📶 RSSI: -38 dBm
📡 WiFi state changed: Disconnected → Connected
📡 WiFi state changed: 0 -> 2
✅ WiFi Manager initialized and connected
✅ OK
🚌 BMS Protocol + CAN... 🚌 Initializing BMS Protocol...
🚌 Initializing CAN controller...
Entering Configuration Mode Successful!
Setting Baudrate Successful!
✅ MCP2515 initialized successfully at 125kbps
✅ MCP2515 initialized: 125kbps, normal mode
✅ CAN controller initialized successfully
   📍 CS Pin: 44
   🚌 Baud Rate: 125 kbps
   🎯 Frame filters: BMS protocols
✅ BMS Protocol initialized successfully
   🎯 Monitoring 4 BMS nodes
   🚌 CAN Bus: 125 kbps, MCP2515 controller
   📊 Frame validation: enabled
✅ OK
⚡ TRIO HP Manager... TRIO HP Manager initialized successfully
✅ OK
⚙️  TRIO HP Config... TRIO HP configuration loaded from EEPROM
Configuration validation failed, applying safe defaults
Resetting to safe TRIO HP configuration
Default system configuration applied (including parameter locking and sequences)
TRIO HP configuration saved to EEPROM
TRIO HP Configuration initialized
✅ OK
📊 TRIO HP Monitor... TRIO HP Monitor initialized successfully
✅ OK
   🎯 Monitoring 4 BMS nodes at 125 kbps
   🔋 Node IDs: 1 2 3 4
⚡ TRIO HP Phase 3... 🔧 Initializing TRIO HP Phase 3 systems...
   🛡️  Safety Limits... [TRIO HP LIMITS] Initialized with safe defaults
[TRIO HP LIMITS] DCCL/DDCL thresholds: 90.0%/90.0%
✅ OK
   🎛️  PID Controllers... [TRIO HP CONTROLLERS] Initializing controllers and efficiency monitoring...
[ACTIVE POWER PID] Initialized with default parameters
[REACTIVE POWER PID] Initialized: single_max=10.0kVAr, threshold=1500VA
[EFFICIENCY MONITOR] Initialized with 1000ms measurement interval
[TRIO HP CONTROLLERS] All controllers initialized successfully
✅ OK
   📈 Efficiency Monitor... [EFFICIENCY MONITOR] Initialized with 1000ms measurement interval
✅ OK
   ⚙️  Configuration... ❌ FAILED
❌ FAILED
🔗 Modbus TCP Server... 🔗 Initializing Modbus TCP Server...
✅ Modbus TCP Server started on port 502
📊 Holding registers: 3200 (0x0000 - 0x0C7F)
🔋 BMS modules: 16 x 200 registers each
📊 === MODBUS REGISTER MAP ===
📍 Total Registers: 3200 (0x0000 - 0x0C7F)
🔋 BMS Modules: 16 x 200 registers each

🗺️ REGISTER LAYOUT PER BMS MODULE:
   Base+0-9:   Frame 190 (voltage, current, energy, soc)
   Base+10-19: Frame 190 error flags
   Base+20-29: Frame 290 (cell voltages)
   Base+30-39: Frame 310 (soh, temperature, dcir)
   Base+40-49: Frame 390 (max voltages)
   Base+50-59: Frame 410 (temperatures, ready states)
   Base+60-69: Frame 510 (power limits, I/O)
   Base+70-89: Frame 490 (multiplexed data)
   Base+90-109: Error maps & versions
   Base+110-119: Frame 710 & communication
   Base+120-124: Reserved

🎯 EXAMPLE BMS MODULE ADDRESSES:
   BMS1: 0-199 (0x0000-0x00C7)
   BMS2: 200-399 (0x00C8-0x018F)
   BMS3: 400-599 (0x0190-0x0257)
   BMS4: 600-799 (0x0258-0x031F)
   ... (and more)
==============================
✅ OK
   🎯 Server running on port 502
   📊 3200 holding registers available
   🔋 16 BMS modules x 200 registers each

❌ Module initialization failed
❌ System initialization failed!
🚨 System entering error recovery mode...

📊 === SYSTEM STATUS ===
🔄 System State: Error
⏰ Boot Time: 9611 ms
💾 Free Heap: 157.4 KB
📶 WiFi Status: Connected
🚌 BMS Protocol: Healthy
🔗 Modbus Status: Running
🔋 Active BMS: 0/4


📊 Starting main processing loop...


Guru Meditation Error: Core  1 panic'ed (Double exception). 

Core  1 register dump:
PC      : 0x4037c88a  PS      : 0x00040936  A0      : 0x820039de  A1      : 0x3fcea160
  #0  0x4037c88a in _xt_context_save at /home/runner/work/esp32-arduino-lib-builder/esp32-arduino-lib-builder/esp-idf/components/freertos/port/xtensa/xtensa_context.S:195

A2      : 0x00040936  A3      : 0x00040026  A4      : 0x00000000  A5      : 0x00000000
A6      : 0x00000000  A7      : 0x3c097008  A8      : 0x3fcea220  A9      : 0x00000000
A10     : 0x00002665  A11     : 0x00000000  A12     : 0x00000000  A13     : 0x3fcec3a8
A14     : 0x00060023  A15     : 0x00000001  SAR     : 0x0000000a  EXCCAUSE: 0x00000002
EXCVADDR: 0x00000000  LBEG    : 0x40056f5c  LEND    : 0x40056f72  LCOUNT  : 0xffffffff


Backtrace: 0x4037c887:0x3fcea160 0x420039db:0x3fcea220 0x4037c887:0x3fcea240 0x4037407d:0x3fcea150 0x403785cc:0x3fcea180 0x4200ee3a:0x3fcea1a0 0x420039d5:0x3fcea1c0 0x420039db:0x3fcea1e0 0x420039db:0x3fcea200 0x420039db:0x3fcea220
  #0  0x4037c887 in _xt_context_save at /home/runner/work/esp32-arduino-lib-builder/esp32-arduino-lib-builder/esp-idf/components/freertos/port/xtensa/xtensa_context.S:194
  #1  0x420039db in processBMSProtocol() at src/bms_protocol.cpp:172
      (inlined by) processBMSProtocol() at src/bms_protocol.cpp:164
  #2  0x4037c887 in _xt_context_save at /home/runner/work/esp32-arduino-lib-builder/esp32-arduino-lib-builder/esp-idf/components/freertos/port/xtensa/xtensa_context.S:194
  #3  0x4037407d in _xt_alloca_exc at /home/runner/work/esp32-arduino-lib-builder/esp32-arduino-lib-builder/esp-idf/components/freertos/port/xtensa/xtensa_vectors.S:1806
  #4  0x403785cc in esp_timer_impl_get_time at /home/runner/work/esp32-arduino-lib-builder/esp32-arduino-lib-builder/esp-idf/components/esp_timer/src/esp_timer_impl_systimer.c:67
  #5  0x4200ee3a in millis at C:/Users/Elipsys5/.platformio/packages/framework-arduinoespressif32/cores/esp32/esp32-hal-misc.c:173
  #6  0x420039d5 in processBMSProtocol() at src/bms_protocol.cpp:169
      (inlined by) processBMSProtocol() at src/bms_protocol.cpp:164
  #7  0x420039db in processBMSProtocol() at src/bms_protocol.cpp:172
      (inlined by) processBMSProtocol() at src/bms_protocol.cpp:164
  #8  0x420039db in processBMSProtocol() at src/bms_protocol.cpp:172
      (inlined by) processBMSProtocol() at src/bms_protocol.cpp:164
  #9  0x420039db in processBMSProtocol() at src/bms_protocol.cpp:172
      (inlined by) processBMSProtocol() at src/bms_protocol.cpp:164





ELF file SHA256: dc1e094a18b4d8e9

Rebooting...
ESP-ROM:esp32s3-20210327
Build:Mar 27 2021
rst:0xc (RTC_SW_CPU_RST),boot:0x8 (SPI_FAST_FLASH_BOOT)
Saved PC:0x4207f5d2
  #0  0x4207f5d2 in esp_pm_impl_waiti at /home/runner/work/esp32-arduino-lib-builder/esp32-arduino-lib-builder/esp-idf/components/esp_pm/pm_impl.c:855

SPIWP:0xee
mode:DIO, clock div:1
load:0x3fce3808,len:0x4bc
load:0x403c9700,len:0xbd8
load:0x403cc700,len:0x2a0c
entry 0x403c98d0
[   189][I][esp32-hal-psram.c:96] psramInit(): PSRAM enabled

🚀 ESP32S3 STARTING...


🚀 ESP32S3 CAN to Modbus TCP Bridge
📋 Version: v4.0.2 - CAN Handler Removed
📅 Build Date: Aug 29 2025 23:37:57
🏭 Device: ESP32S3-CAN-MODBUS-TCP
🏗️ Architecture: Modular (5 modules)


📦 Module Overview:
   🔧 config.h/cpp         - System configuration
   📡 wifi_manager.h/cpp   - WiFi management
   🔗 modbus_tcp.h/cpp     - Modbus TCP server
   📊 bms_data.h           - 🔥 BMS data (80+ pól)
   🛠️ bms_protocol.h/cpp   - 🔥 CAN + BMS protocol (9 parserów + 54 mux)
   🛠️ utils.h/cpp          - Utility functions

❌ REMOVED MODULES:
   🚫 can_handler.h/cpp    - Duplikat (funkcje w bms_protocol)

🎯 System Capabilities:
   🔋 16 BMS modules support
   📊 3200 Modbus registers (200 per BMS)
   🚌 9 CAN frame types (190,290,310,390,410,510,490,1B0,710)
   🔥 54 multiplexer types (Frame 490)
   📡 WiFi + AP fallback mode
   🎯 CAN-triggered AP mode (CAN ID: 0xEF1)
   🔗 Modbus TCP Server (port 502)

🔧 Initializing ESP32S3 CAN to Modbus TCP Bridge...
📋 System Architecture: Modular v4.0.2 (can_handler removed)

[00:00:01] LED System: ✅ OK
✅ LED initialized on GPIO21
📚 Loading configuration from EEPROM...
🔍 Validating configuration...
✅ Configuration validated successfully
✅ Configuration loaded: WiFi=WNK3, BMS=4, CAN=13
[00:00:01] Configuration System: ✅ OK
[00:00:01] AP Trigger System: ✅ OK
📡 AP Trigger system initialized
   Trigger CAN ID: 0xEF1
   Required pattern: 0xFF 0xBB
   Required count: 3 within 1000 ms
   AP duration: 30000 ms
🔧 Initializing system modules...
📊 BMS Data Manager... ✅ OK
📡 WiFi Manager... 📡 WiFi callbacks configured
📡 Initializing WiFi Manager...
📡 WiFi credentials updated: SSID=WNK3
[  1240][D][WiFiGeneric.cpp:1040] _eventCallback(): Arduino Event: 0 - WIFI_READY
[  1279][D][WiFiGeneric.cpp:1040] _eventCallback(): Arduino Event: 2 - STA_START
📡 Connecting to WiFi: WNK3
📡 WiFi credentials updated: SSID=WNK3
📡 WiFi state changed: Disconnected → Connecting
📡 WiFi state changed: 0 -> 1
[  7434][D][WiFiGeneric.cpp:1040] _eventCallback(): Arduino Event: 5 - STA_DISCONNECTED
[  7442][W][WiFiGeneric.cpp:1062] _eventCallback(): Reason: 15 - 4WAY_HANDSHAKE_TIMEOUT
[  7449][D][WiFiGeneric.cpp:1082] _eventCallback(): WiFi Reconnect Running
❌ WiFi disconnected
📡 WiFi state changed: Connecting → Disconnected
📡 WiFi state changed: 1 -> 0
[  7591][D][WiFiGeneric.cpp:1040] _eventCallback(): Arduino Event: 4 - STA_CONNECTED
📡 WiFi station connected to AP
[  7600][D][WiFiGeneric.cpp:1040] _eventCallback(): Arduino Event: 7 - STA_GOT_IP
[  7608][D][WiFiGeneric.cpp:1103] _eventCallback(): STA IP: 192.168.148.42, MASK: 255.255.255.0, GW: 192.168.148.1
✅ WiFi connected to WNK3
📡 IP Address: 192.168.148.42
📶 RSSI: -38 dBm
📡 WiFi state changed: Disconnected → Connected
📡 WiFi state changed: 0 -> 2
✅ WiFi Manager initialized and connected
✅ OK
🚌 BMS Protocol + CAN... 🚌 Initializing BMS Protocol...
🚌 Initializing CAN controller...
Entering Configuration Mode Successful!
Setting Baudrate Successful!
✅ MCP2515 initialized successfully at 125kbps
✅ MCP2515 initialized: 125kbps, normal mode
✅ CAN controller initialized successfully
   📍 CS Pin: 44
   🚌 Baud Rate: 125 kbps
   🎯 Frame filters: BMS protocols
✅ BMS Protocol initialized successfully
   🎯 Monitoring 4 BMS nodes
   🚌 CAN Bus: 125 kbps, MCP2515 controller
   📊 Frame validation: enabled
✅ OK
⚡ TRIO HP Manager... TRIO HP Manager initialized successfully
✅ OK
⚙️  TRIO HP Config... TRIO HP configuration loaded from EEPROM
Configuration validation failed, applying safe defaults
Resetting to safe TRIO HP configuration
Default system configuration applied (including parameter locking and sequences)
TRIO HP configuration saved to EEPROM
TRIO HP Configuration initialized
✅ OK
📊 TRIO HP Monitor... TRIO HP Monitor initialized successfully
✅ OK
   🎯 Monitoring 4 BMS nodes at 125 kbps
   🔋 Node IDs: 1 2 3 4
⚡ TRIO HP Phase 3... 🔧 Initializing TRIO HP Phase 3 systems...
   🛡️  Safety Limits... [TRIO HP LIMITS] Initialized with safe defaults
[TRIO HP LIMITS] DCCL/DDCL thresholds: 90.0%/90.0%
✅ OK
   🎛️  PID Controllers... [TRIO HP CONTROLLERS] Initializing controllers and efficiency monitoring...
[ACTIVE POWER PID] Initialized with default parameters
[REACTIVE POWER PID] Initialized: single_max=10.0kVAr, threshold=1500VA
[EFFICIENCY MONITOR] Initialized with 1000ms measurement interval
[TRIO HP CONTROLLERS] All controllers initialized successfully
✅ OK
   📈 Efficiency Monitor... [EFFICIENCY MONITOR] Initialized with 1000ms measurement interval
✅ OK
   ⚙️  Configuration... ❌ FAILED
❌ FAILED
🔗 Modbus TCP Server... 🔗 Initializing Modbus TCP Server...
✅ Modbus TCP Server started on port 502
📊 Holding registers: 3200 (0x0000 - 0x0C7F)
🔋 BMS modules: 16 x 200 registers each
📊 === MODBUS REGISTER MAP ===
📍 Total Registers: 3200 (0x0000 - 0x0C7F)
🔋 BMS Modules: 16 x 200 registers each

🗺️ REGISTER LAYOUT PER BMS MODULE:
   Base+0-9:   Frame 190 (voltage, current, energy, soc)
   Base+10-19: Frame 190 error flags
   Base+20-29: Frame 290 (cell voltages)
   Base+30-39: Frame 310 (soh, temperature, dcir)
   Base+40-49: Frame 390 (max voltages)
   Base+50-59: Frame 410 (temperatures, ready states)
   Base+60-69: Frame 510 (power limits, I/O)
   Base+70-89: Frame 490 (multiplexed data)
   Base+90-109: Error maps & versions
   Base+110-119: Frame 710 & communication
   Base+120-124: Reserved

🎯 EXAMPLE BMS MODULE ADDRESSES:
   BMS1: 0-199 (0x0000-0x00C7)
   BMS2: 200-399 (0x00C8-0x018F)
   BMS3: 400-599 (0x0190-0x0257)
   BMS4: 600-799 (0x0258-0x031F)
   ... (and more)
==============================
✅ OK
   🎯 Server running on port 502
   📊 3200 holding registers available
   🔋 16 BMS modules x 200 registers each

❌ Module initialization failed
❌ System initialization failed!
🚨 System entering error recovery mode...

📊 === SYSTEM STATUS ===
🔄 System State: Error
⏰ Boot Time: 9409 ms
💾 Free Heap: 157.4 KB
📶 WiFi Status: Connected
🚌 BMS Protocol: Healthy
🔗 Modbus Status: Running
🔋 Active BMS: 0/4


📊 Starting main processing loop...


Guru Meditation Error: Core  1 panic'ed (Double exception). 

Core  1 register dump:
PC      : 0x4037c88a  PS      : 0x00040936  A0      : 0x8200ee3d  A1      : 0x3fcea210
  #0  0x4037c88a in _xt_context_save at /home/runner/work/esp32-arduino-lib-builder/esp32-arduino-lib-builder/esp-idf/components/freertos/port/xtensa/xtensa_context.S:195

A2      : 0x00060b30  A3      : 0x00040023  A4      : 0x00000000  A5      : 0x00000000
A6      : 0x00000001  A7      : 0x3fcec04c  A8      : 0x60023000  A9      : 0x60023000
A10     : 0x40000000  A11     : 0x00000000  A12     : 0x00000000  A13     : 0x00000000
A14     : 0x00000064  A15     : 0x3fcbb3c8  SAR     : 0x0000000a  EXCCAUSE: 0x00000002
EXCVADDR: 0xfffffff0  LBEG    : 0x40056f5c  LEND    : 0x40056f72  LCOUNT  : 0xffffffff


Backtrace: 0x4037c887:0x3fcea210 0x4200ee3a:0x3fcea240 0x4037407d:0x3fcea260 0x420039d5:0x3fcea280 0x420039db:0x3fcea2a0 0x420039db:0x3fcea2c0 0x420039db:0x3fcea2e0 0x420039db:0x3fcea300 0x420039db:0x3fcea320 0x420039db:0x3fcea340 0x420039db:0x3fcea360 0x420039db:0x3fcea380 0x420039db:0x3fcea3a0 0x420039db:0x3fcea3c0 0x420039db:0x3fcea3e0 0x420039db:0x3fcea400 0x420039db:0x3fcea420 0x420039db:0x3fcea440 0x420039db:0x3fcea460 0x420039db:0x3fcea480 0x420039db:0x3fcea4a0 0x420039db:0x3fcea4c0 0x420039db:0x3fcea4e0 0x420039db:0x3fcea500 0x420039db:0x3fcea520 0x420039db:0x3fcea540 0x420039db:0x3fcea560 0x420039db:0x3fcea580 0x420039db:0x3fcea5a0 0x420039db:0x3fcea5c0 0x420039db:0x3fcea5e0 0x420039db:0x3fcea600 0x420039db:0x3fcea620 0x420039db:0x3fcea640 0x420039db:0x3fcea660 0x420039db:0x3fcea680 0x420039db:0x3fcea6a0 0x420039db:0x3fcea6c0 0x420039db:0x3fcea6e0 0x420039db:0x3fcea700 0x420039db:0x3fcea720 0x420039db:0x3fcea740 0x420039db:0x3fcea760 0x420039db:0x3fcea780 0x420039db:0x3fcea7a0 0x420039db:0x3fcea7c0 0x420039db:0x3fcea7e0 0x420039db:0x3fcea800 0x420039db:0x3fcea820 0x420039db:0x3fcea840 0x420039db:0x3fcea860 0x420039db:0x3fcea880 0x420039db:0x3fcea8a0 0x420039db:0x3fcea8c0 0x420039db:0x3fcea8e0 0x420039db:0x3fcea900 0x420039db:0x3fcea920 0x420039db:0x3fcea940 0x420039db:0x3fcea960 0x420039db:0x3fcea980 0x420039db:0x3fcea9a0 0x420039db:0x3fcea9c0 0x420039db:0x3fcea9e0 0x420039db:0x3fceaa00 0x420039db:0x3fceaa20 0x420039db:0x3fceaa40 0x420039db:0x3fceaa60 0x420039db:0x3fceaa80 0x420039db:0x3fceaaa0 0x420039db:0x3fceaac0 0x420039db:0x3fceaae0 0x420039db:0x3fceab00 0x420039db:0x3fceab20 0x420039db:0x3fceab40 0x420039db:0x3fceab60 0x420039db:0x3fceab80 0x420039db:0x3fceaba0 0x420039db:0x3fceabc0 0x420039db:0x3fceabe0 0x420039db:0x3fceac00 0x420039db:0x3fceac20 0x420039db:0x3fceac40 0x420039db:0x3fceac60 0x420039db:0x3fceac80 0x420039db:0x3fceaca0 0x420039db:0x3fceacc0 0x420039db:0x3fceace0 0x420039db:0x3fcead00 0x420039db:0x3fcead20 0x420039db:0x3fcead40 0x420039db:0x3fcead60 0x420039db:0x3fcead80 0x420039db:0x3fceada0 0x420039db:0x3fceadc0 0x420039db:0x3fceade0 0x420039db:0x3fceae00 0x420039db:0x3fceae20 0x420039db:0x3fceae40 0x420039db:0x3fceae60 0x420039db:0x3fceae80 0x420039db:0x3fceaea0 |<-CONTINUES
  #0  0x4037c887 in _xt_context_save at /home/runner/work/esp32-arduino-lib-builder/esp32-arduino-lib-builder/esp-idf/components/freertos/port/xtensa/xtensa_context.S:194
  #1  0x4200ee3a in millis at C:/Users/Elipsys5/.platformio/packages/framework-arduinoespressif32/cores/esp32/esp32-hal-misc.c:173
  #2  0x4037407d in _xt_alloca_exc at /home/runner/work/esp32-arduino-lib-builder/esp32-arduino-lib-builder/esp-idf/components/freertos/port/xtensa/xtensa_vectors.S:1806
  #3  0x420039d5 in processBMSProtocol() at src/bms_protocol.cpp:169
      (inlined by) processBMSProtocol() at src/bms_protocol.cpp:164
  #4  0x420039db in processBMSProtocol() at src/bms_protocol.cpp:172
      (inlined by) processBMSProtocol() at src/bms_protocol.cpp:164
  #5  0x420039db in processBMSProtocol() at src/bms_protocol.cpp:172
      (inlined by) processBMSProtocol() at src/bms_protocol.cpp:164
  #6  0x420039db in processBMSProtocol() at src/bms_protocol.cpp:172
      (inlined by) processBMSProtocol() at src/bms_protocol.cpp:164
  #7  0x420039db in processBMSProtocol() at src/bms_protocol.cpp:172
      (inlined by) processBMSProtocol() at src/bms_protocol.cpp:164
  #8  0x420039db in processBMSProtocol() at src/bms_protocol.cpp:172
      (inlined by) processBMSProtocol() at src/bms_protocol.cpp:164
  #9  0x420039db in processBMSProtocol() at src/bms_protocol.cpp:172
      (inlined by) processBMSProtocol() at src/bms_protocol.cpp:164
  #10 0x420039db in processBMSProtocol() at src/bms_protocol.cpp:172
      (inlined by) processBMSProtocol() at src/bms_protocol.cpp:164
  #11 0x420039db in processBMSProtocol() at src/bms_protocol.cpp:172
      (inlined by) processBMSProtocol() at src/bms_protocol.cpp:164
  #12 0x420039db in processBMSProtocol() at src/bms_protocol.cpp:172
      (inlined by) processBMSProtocol() at src/bms_protocol.cpp:164
  #13 0x420039db in processBMSProtocol() at src/bms_protocol.cpp:172
      (inlined by) processBMSProtocol() at src/bms_protocol.cpp:164
  #14 0x420039db in processBMSProtocol() at src/bms_protocol.cpp:172
      (inlined by) processBMSProtocol() at src/bms_protocol.cpp:164
  #15 0x420039db in processBMSProtocol() at src/bms_protocol.cpp:172
      (inlined by) processBMSProtocol() at src/bms_protocol.cpp:164
  #16 0x420039db in processBMSProtocol() at src/bms_protocol.cpp:172
      (inlined by) processBMSProtocol() at src/bms_protocol.cpp:164
  #17 0x420039db in processBMSProtocol() at src/bms_protocol.cpp:172
      (inlined by) processBMSProtocol() at src/bms_protocol.cpp:164
  #18 0x420039db in processBMSProtocol() at src/bms_protocol.cpp:172
      (inlined by) processBMSProtocol() at src/bms_protocol.cpp:164
  #19 0x420039db in processBMSProtocol() at src/bms_protocol.cpp:172
      (inlined by) processBMSProtocol() at src/bms_protocol.cpp:164
  #20 0x420039db in processBMSProtocol() at src/bms_protocol.cpp:172
      (inlined by) processBMSProtocol() at src/bms_protocol.cpp:164
  #21 0x420039db in processBMSProtocol() at src/bms_protocol.cpp:172
      (inlined by) processBMSProtocol() at src/bms_protocol.cpp:164
  #22 0x420039db in processBMSProtocol() at src/bms_protocol.cpp:172
      (inlined by) processBMSProtocol() at src/bms_protocol.cpp:164
  #23 0x420039db in processBMSProtocol() at src/bms_protocol.cpp:172
      (inlined by) processBMSProtocol() at src/bms_protocol.cpp:164
  #24 0x420039db in processBMSProtocol() at src/bms_protocol.cpp:172
      (inlined by) processBMSProtocol() at src/bms_protocol.cpp:164
  #25 0x420039db in processBMSProtocol() at src/bms_protocol.cpp:172
      (inlined by) processBMSProtocol() at src/bms_protocol.cpp:164
  #26 0x420039db in processBMSProtocol() at src/bms_protocol.cpp:172
      (inlined by) processBMSProtocol() at src/bms_protocol.cpp:164
  #27 0x420039db in processBMSProtocol() at src/bms_protocol.cpp:172
      (inlined by) processBMSProtocol() at src/bms_protocol.cpp:164
  #28 0x420039db in processBMSProtocol() at src/bms_protocol.cpp:172
      (inlined by) processBMSProtocol() at src/bms_protocol.cpp:164
  #29 0x420039db in processBMSProtocol() at src/bms_protocol.cpp:172
      (inlined by) processBMSProtocol() at src/bms_protocol.cpp:164
  #30 0x420039db in processBMSProtocol() at src/bms_protocol.cpp:172
      (inlined by) processBMSProtocol() at src/bms_protocol.cpp:164
  #31 0x420039db in processBMSProtocol() at src/bms_protocol.cpp:172
      (inlined by) processBMSProtocol() at src/bms_protocol.cpp:164
  #32 0x420039db in processBMSProtocol() at src/bms_protocol.cpp:172
      (inlined by) processBMSProtocol() at src/bms_protocol.cpp:164
  #33 0x420039db in processBMSProtocol() at src/bms_protocol.cpp:172
      (inlined by) processBMSProtocol() at src/bms_protocol.cpp:164
  #34 0x420039db in processBMSProtocol() at src/bms_protocol.cpp:172
      (inlined by) processBMSProtocol() at src/bms_protocol.cpp:164
  #35 0x420039db in processBMSProtocol() at src/bms_protocol.cpp:172
      (inlined by) processBMSProtocol() at src/bms_protocol.cpp:164
  #36 0x420039db in processBMSProtocol() at src/bms_protocol.cpp:172
      (inlined by) processBMSProtocol() at src/bms_protocol.cpp:164
  #37 0x420039db in processBMSProtocol() at src/bms_protocol.cpp:172
      (inlined by) processBMSProtocol() at src/bms_protocol.cpp:164
  #38 0x420039db in processBMSProtocol() at src/bms_protocol.cpp:172
      (inlined by) processBMSProtocol() at src/bms_protocol.cpp:164
  #39 0x420039db in processBMSProtocol() at src/bms_protocol.cpp:172
      (inlined by) processBMSProtocol() at src/bms_protocol.cpp:164
  #40 0x420039db in processBMSProtocol() at src/bms_protocol.cpp:172
      (inlined by) processBMSProtocol() at src/bms_protocol.cpp:164
  #41 0x420039db in processBMSProtocol() at src/bms_protocol.cpp:172
      (inlined by) processBMSProtocol() at src/bms_protocol.cpp:164
  #42 0x420039db in processBMSProtocol() at src/bms_protocol.cpp:172
      (inlined by) processBMSProtocol() at src/bms_protocol.cpp:164
  #43 0x420039db in processBMSProtocol() at src/bms_protocol.cpp:172
      (inlined by) processBMSProtocol() at src/bms_protocol.cpp:164
  #44 0x420039db in processBMSProtocol() at src/bms_protocol.cpp:172
      (inlined by) processBMSProtocol() at src/bms_protocol.cpp:164
  #45 0x420039db in processBMSProtocol() at src/bms_protocol.cpp:172
      (inlined by) processBMSProtocol() at src/bms_protocol.cpp:164
  #46 0x420039db in processBMSProtocol() at src/bms_protocol.cpp:172
      (inlined by) processBMSProtocol() at src/bms_protocol.cpp:164
  #47 0x420039db in processBMSProtocol() at src/bms_protocol.cpp:172
      (inlined by) processBMSProtocol() at src/bms_protocol.cpp:164
  #48 0x420039db in processBMSProtocol() at src/bms_protocol.cpp:172
      (inlined by) processBMSProtocol() at src/bms_protocol.cpp:164
  #49 0x420039db in processBMSProtocol() at src/bms_protocol.cpp:172
      (inlined by) processBMSProtocol() at src/bms_protocol.cpp:164
  #50 0x420039db in processBMSProtocol() at src/bms_protocol.cpp:172
      (inlined by) processBMSProtocol() at src/bms_protocol.cpp:164
  #51 0x420039db in processBMSProtocol() at src/bms_protocol.cpp:172
      (inlined by) processBMSProtocol() at src/bms_protocol.cpp:164
  #52 0x420039db in processBMSProtocol() at src/bms_protocol.cpp:172
      (inlined by) processBMSProtocol() at src/bms_protocol.cpp:164
  #53 0x420039db in processBMSProtocol() at src/bms_protocol.cpp:172
      (inlined by) processBMSProtocol() at src/bms_protocol.cpp:164
  #54 0x420039db in processBMSProtocol() at src/bms_protocol.cpp:172
      (inlined by) processBMSProtocol() at src/bms_protocol.cpp:164
  #55 0x420039db in processBMSProtocol() at src/bms_protocol.cpp:172
      (inlined by) processBMSProtocol() at src/bms_protocol.cpp:164
  #56 0x420039db in processBMSProtocol() at src/bms_protocol.cpp:172
      (inlined by) processBMSProtocol() at src/bms_protocol.cpp:164
  #57 0x420039db in processBMSProtocol() at src/bms_protocol.cpp:172
      (inlined by) processBMSProtocol() at src/bms_protocol.cpp:164
  #58 0x420039db in processBMSProtocol() at src/bms_protocol.cpp:172
      (inlined by) processBMSProtocol() at src/bms_protocol.cpp:164
  #59 0x420039db in processBMSProtocol() at src/bms_protocol.cpp:172
      (inlined by) processBMSProtocol() at src/bms_protocol.cpp:164
  #60 0x420039db in processBMSProtocol() at src/bms_protocol.cpp:172
      (inlined by) processBMSProtocol() at src/bms_protocol.cpp:164
  #61 0x420039db in processBMSProtocol() at src/bms_protocol.cpp:172
      (inlined by) processBMSProtocol() at src/bms_protocol.cpp:164
  #62 0x420039db in processBMSProtocol() at src/bms_protocol.cpp:172
      (inlined by) processBMSProtocol() at src/bms_protocol.cpp:164
  #63 0x420039db in processBMSProtocol() at src/bms_protocol.cpp:172
      (inlined by) processBMSProtocol() at src/bms_protocol.cpp:164
  #64 0x420039db in processBMSProtocol() at src/bms_protocol.cpp:172
      (inlined by) processBMSProtocol() at src/bms_protocol.cpp:164
  #65 0x420039db in processBMSProtocol() at src/bms_protocol.cpp:172
      (inlined by) processBMSProtocol() at src/bms_protocol.cpp:164
  #66 0x420039db in processBMSProtocol() at src/bms_protocol.cpp:172
      (inlined by) processBMSProtocol() at src/bms_protocol.cpp:164
  #67 0x420039db in processBMSProtocol() at src/bms_protocol.cpp:172
      (inlined by) processBMSProtocol() at src/bms_protocol.cpp:164
  #68 0x420039db in processBMSProtocol() at src/bms_protocol.cpp:172
      (inlined by) processBMSProtocol() at src/bms_protocol.cpp:164
  #69 0x420039db in processBMSProtocol() at src/bms_protocol.cpp:172
      (inlined by) processBMSProtocol() at src/bms_protocol.cpp:164
  #70 0x420039db in processBMSProtocol() at src/bms_protocol.cpp:172
      (inlined by) processBMSProtocol() at src/bms_protocol.cpp:164
  #71 0x420039db in processBMSProtocol() at src/bms_protocol.cpp:172
      (inlined by) processBMSProtocol() at src/bms_protocol.cpp:164
  #72 0x420039db in processBMSProtocol() at src/bms_protocol.cpp:172
      (inlined by) processBMSProtocol() at src/bms_protocol.cpp:164
  #73 0x420039db in processBMSProtocol() at src/bms_protocol.cpp:172
      (inlined by) processBMSProtocol() at src/bms_protocol.cpp:164
  #74 0x420039db in processBMSProtocol() at src/bms_protocol.cpp:172
      (inlined by) processBMSProtocol() at src/bms_protocol.cpp:164
  #75 0x420039db in processBMSProtocol() at src/bms_protocol.cpp:172
      (inlined by) processBMSProtocol() at src/bms_protocol.cpp:164
  #76 0x420039db in processBMSProtocol() at src/bms_protocol.cpp:172
      (inlined by) processBMSProtocol() at src/bms_protocol.cpp:164
  #77 0x420039db in processBMSProtocol() at src/bms_protocol.cpp:172
      (inlined by) processBMSProtocol() at src/bms_protocol.cpp:164
  #78 0x420039db in processBMSProtocol() at src/bms_protocol.cpp:172
      (inlined by) processBMSProtocol() at src/bms_protocol.cpp:164
  #79 0x420039db in processBMSProtocol() at src/bms_protocol.cpp:172
      (inlined by) processBMSProtocol() at src/bms_protocol.cpp:164
  #80 0x420039db in processBMSProtocol() at src/bms_protocol.cpp:172
      (inlined by) processBMSProtocol() at src/bms_protocol.cpp:164
  #81 0x420039db in processBMSProtocol() at src/bms_protocol.cpp:172
      (inlined by) processBMSProtocol() at src/bms_protocol.cpp:164
  #82 0x420039db in processBMSProtocol() at src/bms_protocol.cpp:172
      (inlined by) processBMSProtocol() at src/bms_protocol.cpp:164
  #83 0x420039db in processBMSProtocol() at src/bms_protocol.cpp:172
      (inlined by) processBMSProtocol() at src/bms_protocol.cpp:164
  #84 0x420039db in processBMSProtocol() at src/bms_protocol.cpp:172
      (inlined by) processBMSProtocol() at src/bms_protocol.cpp:164
  #85 0x420039db in processBMSProtocol() at src/bms_protocol.cpp:172
      (inlined by) processBMSProtocol() at src/bms_protocol.cpp:164
  #86 0x420039db in processBMSProtocol() at src/bms_protocol.cpp:172
      (inlined by) processBMSProtocol() at src/bms_protocol.cpp:164
  #87 0x420039db in processBMSProtocol() at src/bms_protocol.cpp:172
      (inlined by) processBMSProtocol() at src/bms_protocol.cpp:164
  #88 0x420039db in processBMSProtocol() at src/bms_protocol.cpp:172
      (inlined by) processBMSProtocol() at src/bms_protocol.cpp:164
  #89 0x420039db in processBMSProtocol() at src/bms_protocol.cpp:172
      (inlined by) processBMSProtocol() at src/bms_protocol.cpp:164
  #90 0x420039db in processBMSProtocol() at src/bms_protocol.cpp:172
      (inlined by) processBMSProtocol() at src/bms_protocol.cpp:164
  #91 0x420039db in processBMSProtocol() at src/bms_protocol.cpp:172
      (inlined by) processBMSProtocol() at src/bms_protocol.cpp:164
  #92 0x420039db in processBMSProtocol() at src/bms_protocol.cpp:172
      (inlined by) processBMSProtocol() at src/bms_protocol.cpp:164
  #93 0x420039db in processBMSProtocol() at src/bms_protocol.cpp:172
      (inlined by) processBMSProtocol() at src/bms_protocol.cpp:164
  #94 0x420039db in processBMSProtocol() at src/bms_protocol.cpp:172
      (inlined by) processBMSProtocol() at src/bms_protocol.cpp:164
  #95 0x420039db in processBMSProtocol() at src/bms_protocol.cpp:172
      (inlined by) processBMSProtocol() at src/bms_protocol.cpp:164
  #96 0x420039db in processBMSProtocol() at src/bms_protocol.cpp:172
      (inlined by) processBMSProtocol() at src/bms_protocol.cpp:164
  #97 0x420039db in processBMSProtocol() at src/bms_protocol.cpp:172
      (inlined by) processBMSProtocol() at src/bms_protocol.cpp:164
  #98 0x420039db in processBMSProtocol() at src/bms_protocol.cpp:172
      (inlined by) processBMSProtocol() at src/bms_protocol.cpp:164
  #99 0x420039db in processBMSProtocol() at src/bms_protocol.cpp:172
      (inlined by) processBMSProtocol() at src/bms_protocol.cpp:164
  #100 0x420039db in processBMSProtocol() at src/bms_protocol.cpp:172
      (inlined by) processBMSProtocol() at src/bms_protocol.cpp:164





ELF file SHA256: dc1e094a18b4d8e9

Rebooting...
ESP-ROM:esp32s3-20210327
Build:Mar 27 2021
rst:0xc (RTC_SW_CPU_RST),boot:0x8 (SPI_FAST_FLASH_BOOT)
Saved PC:0x4207f5d2
  #0  0x4207f5d2 in esp_pm_impl_waiti at /home/runner/work/esp32-arduino-lib-builder/esp32-arduino-lib-builder/esp-idf/components/esp_pm/pm_impl.c:855

SPIWP:0xee
mode:DIO, clock div:1
load:0x3fce3808,len:0x4bc
load:0x403c9700,len:0xbd8
load:0x403cc700,len:0x2a0c
entry 0x403c98d0
[   189][I][esp32-hal-psram.c:96] psramInit(): PSRAM enabled

🚀 ESP32S3 STARTING...


🚀 ESP32S3 CAN to Modbus TCP Bridge
📋 Version: v4.0.2 - CAN Handler Removed
📅 Build Date: Aug 29 2025 23:37:57
🏭 Device: ESP32S3-CAN-MODBUS-TCP
🏗️ Architecture: Modular (5 modules)


📦 Module Overview:
   🔧 config.h/cpp         - System configuration
   📡 wifi_manager.h/cpp   - WiFi management
   🔗 modbus_tcp.h/cpp     - Modbus TCP server
   📊 bms_data.h           - 🔥 BMS data (80+ pól)
   🛠️ bms_protocol.h/cpp   - 🔥 CAN + BMS protocol (9 parserów + 54 mux)
   🛠️ utils.h/cpp          - Utility functions

❌ REMOVED MODULES:
   🚫 can_handler.h/cpp    - Duplikat (funkcje w bms_protocol)

🎯 System Capabilities:
   🔋 16 BMS modules support
   📊 3200 Modbus registers (200 per BMS)
   🚌 9 CAN frame types (190,290,310,390,410,510,490,1B0,710)
   🔥 54 multiplexer types (Frame 490)
   📡 WiFi + AP fallback mode
   🎯 CAN-triggered AP mode (CAN ID: 0xEF1)
   🔗 Modbus TCP Server (port 502)

🔧 Initializing ESP32S3 CAN to Modbus TCP Bridge...
📋 System Architecture: Modular v4.0.2 (can_handler removed)

[00:00:01] LED System: ✅ OK
✅ LED initialized on GPIO21
📚 Loading configuration from EEPROM...
🔍 Validating configuration...
✅ Configuration validated successfully
✅ Configuration loaded: WiFi=WNK3, BMS=4, CAN=13
[00:00:01] Configuration System: ✅ OK
[00:00:01] AP Trigger System: ✅ OK
📡 AP Trigger system initialized
   Trigger CAN ID: 0xEF1
   Required pattern: 0xFF 0xBB
   Required count: 3 within 1000 ms
   AP duration: 30000 ms
🔧 Initializing system modules...
📊 BMS Data Manager... ✅ OK
📡 WiFi Manager... 📡 WiFi callbacks configured
📡 Initializing WiFi Manager...
📡 WiFi credentials updated: SSID=WNK3
[  1238][D][WiFiGeneric.cpp:1040] _eventCallback(): Arduino Event: 0 - WIFI_READY
[  1278][D][WiFiGeneric.cpp:1040] _eventCallback(): Arduino Event: 2 - STA_START
📡 Connecting to WiFi: WNK3
📡 WiFi credentials updated: SSID=WNK3
📡 WiFi state changed: Disconnected → Connecting
📡 WiFi state changed: 0 -> 1
[  1463][D][WiFiGeneric.cpp:1040] _eventCallback(): Arduino Event: 4 - STA_CONNECTED
📡 WiFi station connected to AP
[  1488][D][WiFiGeneric.cpp:1040] _eventCallback(): Arduino Event: 7 - STA_GOT_IP
[  1496][D][WiFiGeneric.cpp:1103] _eventCallback(): STA IP: 192.168.148.42, MASK: 255.255.255.0, GW: 192.168.148.1
✅ WiFi connected to WNK3
📡 IP Address: 192.168.148.42
📶 RSSI: -37 dBm
📡 WiFi state changed: Connecting → Connected
📡 WiFi state changed: 1 -> 2
✅ WiFi Manager initialized and connected
✅ OK
🚌 BMS Protocol + CAN... 🚌 Initializing BMS Protocol...
🚌 Initializing CAN controller...
Entering Configuration Mode Successful!
Setting Baudrate Successful!
✅ MCP2515 initialized successfully at 125kbps
✅ MCP2515 initialized: 125kbps, normal mode
✅ CAN controller initialized successfully
   📍 CS Pin: 44
   🚌 Baud Rate: 125 kbps
   🎯 Frame filters: BMS protocols
✅ BMS Protocol initialized successfully
   🎯 Monitoring 4 BMS nodes
   🚌 CAN Bus: 125 kbps, MCP2515 controller
   📊 Frame validation: enabled
✅ OK
⚡ TRIO HP Manager... TRIO HP Manager initialized successfully
✅ OK
⚙️  TRIO HP Config... TRIO HP configuration loaded from EEPROM
Configuration validation failed, applying safe defaults
Resetting to safe TRIO HP configuration
Default system configuration applied (including parameter locking and sequences)
TRIO HP configuration saved to EEPROM
TRIO HP Configuration initialized
✅ OK
📊 TRIO HP Monitor... TRIO HP Monitor initialized successfully
✅ OK
   🎯 Monitoring 4 BMS nodes at 125 kbps
   🔋 Node IDs: 1 2 3 4
⚡ TRIO HP Phase 3... 🔧 Initializing TRIO HP Phase 3 systems...
   🛡️  Safety Limits... [TRIO HP LIMITS] Initialized with safe defaults
[TRIO HP LIMITS] DCCL/DDCL thresholds: 90.0%/90.0%
✅ OK
   🎛️  PID Controllers... [TRIO HP CONTROLLERS] Initializing controllers and efficiency monitoring...
[ACTIVE POWER PID] Initialized with default parameters
[REACTIVE POWER PID] Initialized: single_max=10.0kVAr, threshold=1500VA
[EFFICIENCY MONITOR] Initialized with 1000ms measurement interval
[TRIO HP CONTROLLERS] All controllers initialized successfully
✅ OK
   📈 Efficiency Monitor... [EFFICIENCY MONITOR] Initialized with 1000ms measurement interval
✅ OK
   ⚙️  Configuration... ❌ FAILED
❌ FAILED
🔗 Modbus TCP Server... 🔗 Initializing Modbus TCP Server...
✅ Modbus TCP Server started on port 502
📊 Holding registers: 3200 (0x0000 - 0x0C7F)
🔋 BMS modules: 16 x 200 registers each
📊 === MODBUS REGISTER MAP ===
📍 Total Registers: 3200 (0x0000 - 0x0C7F)
🔋 BMS Modules: 16 x 200 registers each

🗺️ REGISTER LAYOUT PER BMS MODULE:
   Base+0-9:   Frame 190 (voltage, current, energy, soc)
   Base+10-19: Frame 190 error flags
   Base+20-29: Frame 290 (cell voltages)
   Base+30-39: Frame 310 (soh, temperature, dcir)
   Base+40-49: Frame 390 (max voltages)
   Base+50-59: Frame 410 (temperatures, ready states)
   Base+60-69: Frame 510 (power limits, I/O)
   Base+70-89: Frame 490 (multiplexed data)
   Base+90-109: Error maps & versions
   Base+110-119: Frame 710 & communication
   Base+120-124: Reserved

🎯 EXAMPLE BMS MODULE ADDRESSES:
   BMS1: 0-199 (0x0000-0x00C7)
   BMS2: 200-399 (0x00C8-0x018F)
   BMS3: 400-599 (0x0190-0x0257)
   BMS4: 600-799 (0x0258-0x031F)
   ... (and more)
==============================
✅ OK
   🎯 Server running on port 502
   📊 3200 holding registers available
   🔋 16 BMS modules x 200 registers each

❌ Module initialization failed
❌ System initialization failed!
🚨 System entering error recovery mode...

📊 === SYSTEM STATUS ===
🔄 System State: Error
⏰ Boot Time: 3216 ms
💾 Free Heap: 157.4 KB
📶 WiFi Status: Connected
🚌 BMS Protocol: Healthy
🔗 Modbus Status: Running
🔋 Active BMS: 0/4


📊 StartGuru Meditation Error: Core  1 panic'ed (Double exception).

Core  1 register dump:
PC      : 0x4037c88a  PS      : 0x00040d36  A0      : 0x820039de  A1      : 0x3fcea160
  #0  0x4037c88a in _xt_context_save at /home/runner/work/esp32-arduino-lib-builder/esp32-arduino-lib-builder/esp-idf/components/freertos/port/xtensa/xtensa_context.S:195

A2      : 0x00040d36  A3      : 0x00040026  A4      : 0x00000000  A5      : 0x00000000
A6      : 0x00000001  A7      : 0x3fcec04c  A8      : 0x3fcea220  A9      : 0x00000001
A10     : 0x00000d6a  A11     : 0x00000000  A12     : 0x00000000  A13     : 0x00060023
A14     : 0x00000064  A15     : 0x3fcbb3c8  SAR     : 0x0000000a  EXCCAUSE: 0x00000002
EXCVADDR: 0x00000000  LBEG    : 0x40056f08  LEND    : 0x40056f12  LCOUNT  : 0x00000000


Backtrace: 0x4037c887:0x3fcea160 0x420039db:0x3fcea220 0x4037c887:0x3fcea240 0x4037407d:0x3fcea150 0x403785cc:0x3fcea180 0x4200ee3a:0x3fcea1a0 0x420039d5:0x3fcea1c0 0x420039db:0x3fcea1e0 0x420039db:0x3fcea200 0x420039db:0x3fcea220
  #0  0x4037c887 in _xt_context_save at /home/runner/work/esp32-arduino-lib-builder/esp32-arduino-lib-builder/esp-idf/components/freertos/port/xtensa/xtensa_context.S:194
  #1  0x420039db in processBMSProtocol() at src/bms_protocol.cpp:172
      (inlined by) processBMSProtocol() at src/bms_protocol.cpp:164
  #2  0x4037c887 in _xt_context_save at /home/runner/work/esp32-arduino-lib-builder/esp32-arduino-lib-builder/esp-idf/components/freertos/port/xtensa/xtensa_context.S:194
  #3  0x4037407d in _xt_alloca_exc at /home/runner/work/esp32-arduino-lib-builder/esp32-arduino-lib-builder/esp-idf/components/freertos/port/xtensa/xtensa_vectors.S:1806
  #4  0x403785cc in esp_timer_impl_get_time at /home/runner/work/esp32-arduino-lib-builder/esp32-arduino-lib-builder/esp-idf/components/esp_timer/src/esp_timer_impl_systimer.c:67
  #5  0x4200ee3a in millis at C:/Users/Elipsys5/.platformio/packages/framework-arduinoespressif32/cores/esp32/esp32-hal-misc.c:173
  #6  0x420039d5 in processBMSProtocol() at src/bms_protocol.cpp:169
      (inlined by) processBMSProtocol() at src/bms_protocol.cpp:164
  #7  0x420039db in processBMSProtocol() at src/bms_protocol.cpp:172
      (inlined by) processBMSProtocol() at src/bms_protocol.cpp:164
  #8  0x420039db in processBMSProtocol() at src/bms_protocol.cpp:172
      (inlined by) processBMSProtocol() at src/bms_protocol.cpp:164
  #9  0x420039db in processBMSProtocol() at src/bms_protocol.cpp:172
      (inlined by) processBMSProtocol() at src/bms_protocol.cpp:164





ELF file SHA256: dc1e094a18b4d8e9

Rebooting...
ESP-ROM:esp32s3-20210327
Build:Mar 27 2021
rst:0xc (RTC_SW_CPU_RST),boot:0x8 (SPI_FAST_FLASH_BOOT)
Saved PC:0x4207f5d2
  #0  0x4207f5d2 in esp_pm_impl_waiti at /home/runner/work/esp32-arduino-lib-builder/esp32-arduino-lib-builder/esp-idf/components/esp_pm/pm_impl.c:855

SPIWP:0xee
mode:DIO, clock div:1
load:0x3fce3808,len:0x4bc
load:0x403c9700,len:0xbd8
load:0x403cc700,len:0x2a0c
entry 0x403c98d0
[   189][I][esp32-hal-psram.c:96] psramInit(): PSRAM enabled

🚀 ESP32S3 STARTING...


🚀 ESP32S3 CAN to Modbus TCP Bridge
📋 Version: v4.0.2 - CAN Handler Removed
📅 Build Date: Aug 29 2025 23:37:57
🏭 Device: ESP32S3-CAN-MODBUS-TCP
🏗️ Architecture: Modular (5 modules)


📦 Module Overview:
   🔧 config.h/cpp         - System configuration
   📡 wifi_manager.h/cpp   - WiFi management
   🔗 modbus_tcp.h/cpp     - Modbus TCP server
   📊 bms_data.h           - 🔥 BMS data (80+ pól)
   🛠️ bms_protocol.h/cpp   - 🔥 CAN + BMS protocol (9 parserów + 54 mux)
   🛠️ utils.h/cpp          - Utility functions

❌ REMOVED MODULES:
   🚫 can_handler.h/cpp    - Duplikat (funkcje w bms_protocol)

🎯 System Capabilities:
   🔋 16 BMS modules support
   📊 3200 Modbus registers (200 per BMS)
   🚌 9 CAN frame types (190,290,310,390,410,510,490,1B0,710)
   🔥 54 multiplexer types (Frame 490)
   📡 WiFi + AP fallback mode
   🎯 CAN-triggered AP mode (CAN ID: 0xEF1)
   🔗 Modbus TCP Server (port 502)

🔧 Initializing ESP32S3 CAN to Modbus TCP Bridge...
📋 System Architecture: Modular v4.0.2 (can_handler removed)

[00:00:01] LED System: ✅ OK
✅ LED initialized on GPIO21
📚 Loading configuration from EEPROM...
🔍 Validating configuration...
✅ Configuration validated successfully
✅ Configuration loaded: WiFi=WNK3, BMS=4, CAN=13
[00:00:01] Configuration System: ✅ OK
[00:00:01] AP Trigger System: ✅ OK
📡 AP Trigger system initialized
   Trigger CAN ID: 0xEF1
   Required pattern: 0xFF 0xBB
   Required count: 3 within 1000 ms
   AP duration: 30000 ms
🔧 Initializing system modules...
📊 BMS Data Manager... ✅ OK
📡 WiFi Manager... 📡 WiFi callbacks configured
📡 Initializing WiFi Manager...
📡 WiFi credentials updated: SSID=WNK3
[  1238][D][WiFiGeneric.cpp:1040] _eventCallback(): Arduino Event: 0 - WIFI_READY
[  1278][D][WiFiGeneric.cpp:1040] _eventCallback(): Arduino Event: 2 - STA_START
📡 Connecting to WiFi: WNK3
📡 WiFi credentials updated: SSID=WNK3
📡 WiFi state changed: Disconnected → Connecting
📡 WiFi state changed: 0 -> 1