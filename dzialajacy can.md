Masz tutaj przyklad dzialajacego kodu z poprawnym podliczaniem node id dzialajacym podgladem debugowym tego co sie dzieje na canie

Najpierw kod, a potem zrzut z monitora. 

Jak widac oprocz can z bms jest tez heartbeat z trio hp ale to pochodzi z innego programu.

Jak uruchamiamy ten nasz program to mamy monitor jak na samym koncu tego plik




=== ESP32S3 CAN to MQTT Bridge - MQTT OPTIMIZED FIXED ===
VERSION: v2.2.1 - Fixed compilation errors
OPTIMIZATION: 80-95% MQTT bandwidth reduction
LIBRARY: limengdu/Arduino_CAN_BUS_MCP2515
======================================================

📊 MQTT Optimization Configuration:
   Idle interval: 10000 ms (10.0 sec)
   Active interval: 5000 ms (5.0 sec)
   Charging interval: 2000 ms (2.0 sec)
   Error interval: 500 ms (0.5 sec)
   Current threshold: 1.0 A
   Delta thresholds: V=0.1, I=0.5, SOC=1.0, T=1.0

📶 Connecting to WiFi: WNK3
[  1215][D][WiFiGeneric.cpp:1040] _eventCallback(): Arduino Event: 0 - WIFI_READY
[  1255][D][WiFiGeneric.cpp:1040] _eventCallback(): Arduino Event: 2 - STA_START
[  1472][D][WiFiGeneric.cpp:1040] _eventCallback(): Arduino Event: 4 - STA_CONNECTED
[  1500][D][WiFiGeneric.cpp:1040] _eventCallback(): Arduino Event: 7 - STA_GOT_IP
[  1507][D][WiFiGeneric.cpp:1103] _eventCallback(): STA IP: 192.168.148.42, MASK: 255.255.255.0, GW: 192.168.148.1
.
✅ WiFi connected! IP: 192.168.148.42
📶 Signal: -37 dBm
🔧 Configuring SPI pins for CAN Expansion Board...
   MOSI: GPIO9
   MISO: GPIO8
   SCK:  GPIO7
   CS:   GPIO44
✅ SPI pins configured
🚌 Initializing MCP2515 with fixed 125 kbps...
📍 CS Pin: GPIO44
✅ CS pin control OK
🔄 Initializing CAN at 125 kbps (fixed)...
✅ CAN initialized at 125 kbps (fixed)
📋 CAN controller ready at 125 kbps
🎯 Monitoring BMS Node IDs: 26(0x1A)
🔗 Connecting to MQTT... connected!
🆕 Publishing Home Assistant discovery...
🆕 Publishing HA discovery for BMS Node 26
✅ Discovery completed for BMS Node 26 (5 core entities)
🚀 MQTT Optimized CAN Bridge ready!
📊 Smart publishing: Adaptive intervals + Delta detection
🎯 Bandwidth savings: 80-95% reduction in MQTT traffic

🔍 CAN: ID=0x71A Len=1 Data=[05]
⚠️ Invalid frame length: 1 (expected 8)
🔍 CAN: ID=0x1BA Len=8 Data=[00 00 00 00 00 00 00 00] (Basic data)
🔍 CAN: ID=0x19A Len=8 Data=[92 0F FC FF 1D 00 12 00] (Basic data)
📊 BMS26-190: U=249.12V I=4095.75A SOC=18.0% E=2.9kWh MasterErr=NO
📤 Publishing BMS data (adaptive interval: 2000ms)...
📤 MQTT Optimized: Published 17 values, skipped 0 for BMS26 (0.0% reduction)
🔍 CAN: ID=0x19A Len=8 Data=[92 0F 00 00 1D 00 12 00] (Basic data)
📊 BMS26-190: U=249.12V I=0.00A SOC=18.0% E=2.9kWh MasterErr=NO
🔍 CAN: ID=0x1BA Len=8 Data=[00 00 00 00 00 00 00 00] (Basic data)
🔍 CAN: ID=0x29A Len=8 Data=[FE 86 01 03 07 2A 87 1C] (Cell voltages)
📊 BMS26-290: VMin=3.4558V VMean=3.4602V Block=3 Cell=7 String=1
🔍 CAN: ID=0x19A Len=8 Data=[92 0F FC FF 1D 00 12 00] (Basic data)
📊 BMS26-190: U=249.12V I=4095.75A SOC=18.0% E=2.9kWh MasterErr=NO
🔍 CAN: ID=0x29A Len=8 Data=[FE 86 01 03 07 2A 87 1C] (Cell voltages)
📊 BMS26-290: VMin=3.4558V VMean=3.4602V Block=3 Cell=7 String=1
🔍 CAN: ID=0x39A Len=8 Data=[3B 87 01 05 04 3D 00 1C] (Max voltages)
📊 BMS26-390: VMax=3.4619V VDelta=0.0061V Block=5 Cell=4 String=1
🔍 CAN: ID=0x41A Len=8 Data=[17 01 01 01 01 00 00 00] (Temperatures)
📊 BMS26-410: TMax=23.0°C TDelta=1.0°C Ready: Chg=❌ Dchg=❌
🔍 CAN: ID=0x1BA Len=8 Data=[00 00 00 00 00 00 00 00] (Basic data)
🔍 CAN: ID=0x51A Len=8 Data=[00 01 00 00 01 3B 01 00] (Power limits)
📊 BMS26-510: ChgLim=16.00A DchgLim=19.69A R1=OFF R2=OFF IN01=LOW IN02=LOW
🔍 CAN: ID=0x71A Len=1 Data=[05]
⚠️ Invalid frame length: 1 (expected 8)
🔍 CAN: ID=0x757F803 Len=8 Data=[01 00 10 00 0B 00 00 00]
⚠️ Non-BMS frame: 0x757F803
🔍 CAN: ID=0x19A Len=8 Data=[92 0F FD FF 1D 00 12 00] (Basic data)
📊 BMS26-190: U=249.12V I=4095.81A SOC=18.0% E=2.9kWh MasterErr=NO
🔍 CAN: ID=0x51A Len=8 Data=[00 01 00 00 01 3B 01 00] (Power limits)
📊 BMS26-510: ChgLim=16.00A DchgLim=19.69A R1=OFF R2=OFF IN01=LOW IN02=LOW
🔍 CAN: ID=0x49A Len=8 Data=[16 01 01 02 16 11 00 00]
🔍 CAN: ID=0x31A Len=8 Data=[48 00 2F 87 16 45 00 64] (Multiplexed)
📊 BMS26-310: SOH=100.0% CellV=3460.7mV CellT=22.0°C DCiR=6.9mΩ
🔍 CAN: ID=0x1BA Len=8 Data=[00 00 00 00 00 00 00 00] (Basic data)
🔍 CAN: ID=0x29A Len=8 Data=[FE 86 01 03 07 2A 87 1C] (Cell voltages)
📊 BMS26-290: VMin=3.4558V VMean=3.4602V Block=3 Cell=7 String=1
🔍 CAN: ID=0x19A Len=8 Data=[92 0F FD FF 1D 00 12 00] (Basic data)
📊 BMS26-190: U=249.12V I=4095.81A SOC=18.0% E=2.9kWh MasterErr=NO
🔍 CAN: ID=0x51A Len=8 Data=[00 01 00 00 01 3B 01 00] (Power limits)
📊 BMS26-510: ChgLim=16.00A DchgLim=19.69A R1=OFF R2=OFF IN01=LOW IN02=LOW
🔍 CAN: ID=0x49A Len=8 Data=[16 01 01 02 16 12 00 00]
🔍 CAN: ID=0x31A Len=8 Data=[49 00 2C 87 16 45 00 64] (Multiplexed)
📊 BMS26-310: SOH=100.0% CellV=3460.4mV CellT=22.0°C DCiR=6.9mΩ
🔍 CAN: ID=0x1BA Len=8 Data=[00 00 00 00 00 00 00 00] (Basic data)
🔍 CAN: ID=0x29A Len=8 Data=[FE 86 01 03 07 2A 87 1C] (Cell voltages)
📊 BMS26-290: VMin=3.4558V VMean=3.4602V Block=3 Cell=7 String=1
🔍 CAN: ID=0x19A Len=8 Data=[92 0F FC FF 1D 00 12 00] (Basic data)
📊 BMS26-190: U=249.12V I=4095.75A SOC=18.0% E=2.9kWh MasterErr=NO
🔍 CAN: ID=0x29A Len=8 Data=[FE 86 01 03 07 2A 87 1C] (Cell voltages)
📊 BMS26-290: VMin=3.4558V VMean=3.4602V Block=3 Cell=7 String=1
🔍 CAN: ID=0x39A Len=8 Data=[3B 87 01 05 04 3D 00 1C] (Max voltages)
📊 BMS26-390: VMax=3.4619V VDelta=0.0061V Block=5 Cell=4 String=1
🔍 CAN: ID=0x41A Len=8 Data=[17 01 01 01 01 00 00 00] (Temperatures)
📊 BMS26-410: TMax=23.0°C TDelta=1.0°C Ready: Chg=❌ Dchg=❌
🔍 CAN: ID=0x1BA Len=8 Data=[00 00 00 00 00 00 00 00] (Basic data)
🔍 CAN: ID=0x51A Len=8 Data=[00 01 00 00 01 3B 01 00] (Power limits)
📊 BMS26-510: ChgLim=16.00A DchgLim=19.69A R1=OFF R2=OFF IN01=LOW IN02=LOW
🔍 CAN: ID=0x19A Len=8 Data=[92 0F 01 00 1D 00 12 00] (Basic data)
📊 BMS26-190: U=249.12V I=0.06A SOC=18.0% E=2.9kWh MasterErr=NO
🔍 CAN: ID=0x29A Len=8 Data=[FE 86 01 03 07 2A 87 1C] (Cell voltages)
📊 BMS26-290: VMin=3.4558V VMean=3.4602V Block=3 Cell=7 String=1
🔍 CAN: ID=0x39A Len=8 Data=[3B 87 01 05 04 3D 00 1C] (Max voltages)
📊 BMS26-390: VMax=3.4619V VDelta=0.0061V Block=5 Cell=4 String=1
🔍 CAN: ID=0x41A Len=8 Data=[17 01 01 01 01 00 00 00] (Temperatures)
📊 BMS26-410: TMax=23.0°C TDelta=1.0°C Ready: Chg=❌ Dchg=❌
🔍 CAN: ID=0x1BA Len=8 Data=[00 00 00 00 00 00 00 00] (Basic data)
🔍 CAN: ID=0x51A Len=8 Data=[00 01 00 00 01 3B 01 00] (Power limits)
📊 BMS26-510: ChgLim=16.00A DchgLim=19.69A R1=OFF R2=OFF IN01=LOW IN02=LOW
🔍 CAN: ID=0x757F803 Len=8 Data=[01 00 10 00 0B 00 00 00]
⚠️ Non-BMS frame: 0x757F803
🔍 CAN: ID=0x19A Len=8 Data=[92 0F 01 00 1D 00 12 00] (Basic data)
📊 BMS26-190: U=249.12V I=0.06A SOC=18.0% E=2.9kWh MasterErr=NO
🔍 CAN: ID=0x29A Len=8 Data=[FE 86 01 03 07 2A 87 1C] (Cell voltages)
📊 BMS26-290: VMin=3.4558V VMean=3.4602V Block=3 Cell=7 String=1
🔍 CAN: ID=0x39A Len=8 Data=[3B 87 01 05 04 3D 00 1C] (Max voltages)
📊 BMS26-390: VMax=3.4619V VDelta=0.0061V Block=5 Cell=4 String=1
🔍 CAN: ID=0x51A Len=8 Data=[00 01 00 00 01 3B 01 00] (Power limits)
📊 BMS26-510: ChgLim=16.00A DchgLim=19.69A R1=OFF R2=OFF IN01=LOW IN02=LOW
🔍 CAN: ID=0x1BA Len=8 Data=[00 00 00 00 00 00 00 00] (Basic data)
🔍 CAN: ID=0x41A Len=8 Data=[17 01 01 01 01 00 00 00] (Temperatures)
📊 BMS26-410: TMax=23.0°C TDelta=1.0°C Ready: Chg=❌ Dchg=❌
🔍 CAN: ID=0x71A Len=1 Data=[05]
⚠️ Invalid frame length: 1 (expected 8)
🔍 CAN: ID=0x19A Len=8 Data=[92 0F FD FF 1D 00 12 00] (Basic data)
📊 BMS26-190: U=249.12V I=4095.81A SOC=18.0% E=2.9kWh MasterErr=NO
🔍 CAN: ID=0x29A Len=8 Data=[FB 86 01 03 07 2A 87 1C] (Cell voltages)
📊 BMS26-290: VMin=3.4555V VMean=3.4602V Block=3 Cell=7 String=1
🔍 CAN: ID=0x49A Len=8 Data=[16 01 01 02 16 16 00 00]
🔍 CAN: ID=0x31A Len=8 Data=[4D 00 32 87 00 45 00 64] (Multiplexed)
📊 BMS26-310: SOH=100.0% CellV=3461.0mV CellT=0.0°C DCiR=6.9mΩ
🔍 CAN: ID=0x1BA Len=8 Data=[00 00 00 00 00 00 00 00] (Basic data)
🔍 CAN: ID=0x51A Len=8 Data=[00 01 00 00 01 3B 01 00] (Power limits)
📊 BMS26-510: ChgLim=16.00A DchgLim=19.69A R1=OFF R2=OFF IN01=LOW IN02=LOW
🔍 CAN: ID=0x19A Len=8 Data=[92 0F FC FF 1D 00 12 00] (Basic data)
📊 BMS26-190: U=249.12V I=4095.75A SOC=18.0% E=2.9kWh MasterErr=NO
🔍 CAN: ID=0x29A Len=8 Data=[FB 86 01 03 07 2A 87 1C] (Cell voltages)
📊 BMS26-290: VMin=3.4555V VMean=3.4602V Block=3 Cell=7 String=1
🔍 CAN: ID=0x39A Len=8 Data=[3B 87 01 01 07 40 00 1C] (Max voltages)
📊 BMS26-390: VMax=3.4619V VDelta=0.0064V Block=1 Cell=7 String=1
🔍 CAN: ID=0x41A Len=8 Data=[17 01 01 01 01 00 00 00] (Temperatures)
📊 BMS26-410: TMax=23.0°C TDelta=1.0°C Ready: Chg=❌ Dchg=❌
🔍 CAN: ID=0x1BA Len=8 Data=[00 00 00 00 00 00 00 00] (Basic data)
🔍 CAN: ID=0x51A Len=8 Data=[00 01 00 00 01 3B 01 00] (Power limits)
📊 BMS26-510: ChgLim=16.00A DchgLim=19.69A R1=OFF R2=OFF IN01=LOW IN02=LOW
🔍 CAN: ID=0x19A Len=8 Data=[92 0F FF FF 1D 00 12 00] (Basic data)
📊 BMS26-190: U=249.12V I=4095.94A SOC=18.0% E=2.9kWh MasterErr=NO
🔍 CAN: ID=0x51A Len=8 Data=[00 01 00 00 01 3B 01 00] (Power limits)
📊 BMS26-510: ChgLim=16.00A DchgLim=19.69A R1=OFF R2=OFF IN01=LOW IN02=LOW
🔍 CAN: ID=0x49A Len=8 Data=[16 01 01 02 16 18 44 44]
🔍 CAN: ID=0x31A Len=8 Data=[4F 00 32 87 00 45 00 64] (Multiplexed)
📊 BMS26-310: SOH=100.0% CellV=3461.0mV CellT=0.0°C DCiR=6.9mΩ
🔍 CAN: ID=0x1BA Len=8 Data=[00 00 00 00 00 00 00 00] (Basic data)
🔍 CAN: ID=0x29A Len=8 Data=[FB 86 01 03 07 2A 87 1C] (Cell voltages)
📊 BMS26-290: VMin=3.4555V VMean=3.4602V Block=3 Cell=7 String=1
🔍 CAN: ID=0x757F803 Len=8 Data=[01 00 10 00 0B 00 00 00]
⚠️ Non-BMS frame: 0x757F803
🔍 CAN: ID=0x19A Len=8 Data=[92 0F FF FF 1D 00 12 00] (Basic data)
📊 BMS26-190: U=249.12V I=4095.94A SOC=18.0% E=2.9kWh MasterErr=NO
🔍 CAN: ID=0x51A Len=8 Data=[00 01 00 00 01 3B 01 00] (Power limits)
📊 BMS26-510: ChgLim=16.00A DchgLim=19.69A R1=OFF R2=OFF IN01=LOW IN02=LOW
🔍 CAN: ID=0x49A Len=8 Data=[16 01 01 02 16 19 DA 01]
🔍 CAN: ID=0x31A Len=8 Data=[50 00 32 87 00 45 00 64] (Multiplexed)
📊 BMS26-310: SOH=100.0% CellV=3461.0mV CellT=0.0°C DCiR=6.9mΩ
🔍 CAN: ID=0x1BA Len=8 Data=[00 00 00 00 00 00 00 00] (Basic data)
🔍 CAN: ID=0x29A Len=8 Data=[FB 86 01 03 07 2A 87 1C] (Cell voltages)
📊 BMS26-290: VMin=3.4555V VMean=3.4602V Block=3 Cell=7 String=1
🔍 CAN: ID=0x19A Len=8 Data=[92 0F FC FF 1D 00 12 00] (Basic data)
📊 BMS26-190: U=249.12V I=4095.75A SOC=18.0% E=2.9kWh MasterErr=NO
🔍 CAN: ID=0x29A Len=8 Data=[FB 86 01 03 07 2A 87 1C] (Cell voltages)
📊 BMS26-290: VMin=3.4555V VMean=3.4602V Block=3 Cell=7 String=1
🔍 CAN: ID=0x39A Len=8 Data=[3B 87 01 01 07 40 00 1C] (Max voltages)
📊 BMS26-390: VMax=3.4619V VDelta=0.0064V Block=1 Cell=7 String=1
🔍 CAN: ID=0x41A Len=8 Data=[17 01 01 01 01 00 00 00] (Temperatures)
📊 BMS26-410: TMax=23.0°C TDelta=1.0°C Ready: Chg=❌ Dchg=❌
🔍 CAN: ID=0x1BA Len=8 Data=[00 00 00 00 00 00 00 00] (Basic data)
🔍 CAN: ID=0x51A Len=8 Data=[00 01 00 00 01 3B 01 00] (Power limits)
📊 BMS26-510: ChgLim=16.00A DchgLim=19.69A R1=OFF R2=OFF IN01=LOW IN02=LOW
🔍 CAN: ID=0x71A Len=1 Data=[05]
⚠️ Invalid frame length: 1 (expected 8)
🔍 CAN: ID=0x19A Len=8 Data=[92 0F FD FF 1D 00 12 00] (Basic data)
📊 BMS26-190: U=249.12V I=4095.81A SOC=18.0% E=2.9kWh MasterErr=NO
🔍 CAN: ID=0x51A Len=8 Data=[00 01 00 00 01 3B 01 00] (Power limits)
📊 BMS26-510: ChgLim=16.00A DchgLim=19.69A R1=OFF R2=OFF IN01=LOW IN02=LOW
🔍 CAN: ID=0x49A Len=8 Data=[16 01 01 02 16 1B 94 78]
🔍 CAN: ID=0x31A Len=8 Data=[52 00 32 87 00 45 00 64] (Multiplexed)
📊 BMS26-310: SOH=100.0% CellV=3461.0mV CellT=0.0°C DCiR=6.9mΩ
🔍 CAN: ID=0x1BA Len=8 Data=[00 00 00 00 00 00 00 00] (Basic data)
🔍 CAN: ID=0x29A Len=8 Data=[FB 86 01 03 07 2A 87 1C] (Cell voltages)
📊 BMS26-290: VMin=3.4555V VMean=3.4602V Block=3 Cell=7 String=1
🔍 CAN: ID=0x19A Len=8 Data=[92 0F FE FF 1D 00 12 00] (Basic data)
📊 BMS26-190: U=249.12V I=4095.88A SOC=18.0% E=2.9kWh MasterErr=NO
🔍 CAN: ID=0x29A Len=8 Data=[FB 86 01 03 07 2A 87 1C] (Cell voltages)
📊 BMS26-290: VMin=3.4555V VMean=3.4602V Block=3 Cell=7 String=1
🔍 CAN: ID=0x39A Len=8 Data=[3B 87 01 01 07 40 00 1C] (Max voltages)
📊 BMS26-390: VMax=3.4619V VDelta=0.0064V Block=1 Cell=7 String=1
🔍 CAN: ID=0x41A Len=8 Data=[17 01 01 01 01 00 00 00] (Temperatures)
📊 BMS26-410: TMax=23.0°C TDelta=1.0°C Ready: Chg=❌ Dchg=❌
🔍 CAN: ID=0x1BA Len=8 Data=[00 00 00 00 00 00 00 00] (Basic data)
🔍 CAN: ID=0x51A Len=8 Data=[00 01 00 00 01 3B 01 00] (Power limits)
📊 BMS26-510: ChgLim=16.00A DchgLim=19.69A R1=OFF R2=OFF IN01=LOW IN02=LOW
🔍 CAN: ID=0x757F803 Len=8 Data=[01 00 10 00 0B 00 00 00]
⚠️ Non-BMS frame: 0x757F803
🔍 CAN: ID=0x19A Len=8 Data=[92 0F 00 00 1D 00 12 00] (Basic data)
📊 BMS26-190: U=249.12V I=0.00A SOC=18.0% E=2.9kWh MasterErr=NO
🔍 CAN: ID=0x51A Len=8 Data=[00 01 00 00 01 3B 01 00] (Power limits)
📊 BMS26-510: ChgLim=16.00A DchgLim=19.69A R1=OFF R2=OFF IN01=LOW IN02=LOW
🔍 CAN: ID=0x49A Len=8 Data=[16 01 01 02 16 1D 00 00]
🔍 CAN: ID=0x31A Len=8 Data=[54 00 00 00 00 00 00 64] (Multiplexed)
📊 BMS26-310: SOH=100.0% CellV=0.0mV CellT=0.0°C DCiR=0.0mΩ
🔍 CAN: ID=0x1BA Len=8 Data=[00 00 00 00 00 00 00 00] (Basic data)
🔍 CAN: ID=0x29A Len=8 Data=[FE 86 01 03 07 2A 87 1C] (Cell voltages)
📊 BMS26-290: VMin=3.4558V VMean=3.4602V Block=3 Cell=7 String=1
🔍 CAN: ID=0x19A Len=8 Data=[92 0F FC FF 1D 00 12 00] (Basic data)
📊 BMS26-190: U=249.12V I=4095.75A SOC=18.0% E=2.9kWh MasterErr=NO
🔍 CAN: ID=0x51A Len=8 Data=[00 01 00 00 01 3B 01 00] (Power limits)
📊 BMS26-510: ChgLim=16.00A DchgLim=19.69A R1=OFF R2=OFF IN01=LOW IN02=LOW
🔍 CAN: ID=0x49A Len=8 Data=[16 01 01 02 16 1E E1 98]
🔍 CAN: ID=0x31A Len=8 Data=[55 00 00 00 00 00 00 64] (Multiplexed)
📊 BMS26-310: SOH=100.0% CellV=0.0mV CellT=0.0°C DCiR=0.0mΩ
🔍 CAN: ID=0x1BA Len=8 Data=[00 00 00 00 00 00 00 00] (Basic data)
🔍 CAN: ID=0x29A Len=8 Data=[FE 86 01 03 07 2A 87 1C] (Cell voltages)
📊 BMS26-290: VMin=3.4558V VMean=3.4602V Block=3 Cell=7 String=1
🔍 CAN: ID=0x19A Len=8 Data=[92 0F FC FF 1D 00 12 00] (Basic data)
📊 BMS26-190: U=249.12V I=4095.75A SOC=18.0% E=2.9kWh MasterErr=NO
🔍 CAN: ID=0x51A Len=8 Data=[00 01 00 00 01 3B 01 00] (Power limits)
📊 BMS26-510: ChgLim=16.00A DchgLim=19.69A R1=OFF R2=OFF IN01=LOW IN02=LOW
🔍 CAN: ID=0x49A Len=8 Data=[16 01 01 02 16 1F 06 01]
🔍 CAN: ID=0x31A Len=8 Data=[56 00 00 00 00 00 00 64] (Multiplexed)
📊 BMS26-310: SOH=100.0% CellV=0.0mV CellT=0.0°C DCiR=0.0mΩ
🔍 CAN: ID=0x1BA Len=8 Data=[00 00 00 00 00 00 00 00] (Basic data)
🔍 CAN: ID=0x29A Len=8 Data=[FE 86 01 03 07 2A 87 1C] (Cell voltages)
📊 BMS26-290: VMin=3.4558V VMean=3.4602V Block=3 Cell=7 String=1
🔍 CAN: ID=0x71A Len=1 Data=[05]
⚠️ Invalid frame length: 1 (expected 8)
🔍 CAN: ID=0x19A Len=8 Data=[92 0F FC FF 1D 00 12 00] (Basic data)
📊 BMS26-190: U=249.12V I=4095.75A SOC=18.0% E=2.9kWh MasterErr=NO
🔍 CAN: ID=0x51A Len=8 Data=[00 01 00 00 01 3B 01 00] (Power limits)
📊 BMS26-510: ChgLim=16.00A DchgLim=19.69A R1=OFF R2=OFF IN01=LOW IN02=LOW
🔍 CAN: ID=0x49A Len=8 Data=[16 01 01 02 16 20 00 00]
🔍 CAN: ID=0x31A Len=8 Data=[57 00 00 00 00 00 00 64] (Multiplexed)
📊 BMS26-310: SOH=100.0% CellV=0.0mV CellT=0.0°C DCiR=0.0mΩ
🔍 CAN: ID=0x1BA Len=8 Data=[00 00 00 00 00 00 00 00] (Basic data)
🔍 CAN: ID=0x29A Len=8 Data=[FE 86 01 03 07 2A 87 1C] (Cell voltages)
📊 BMS26-290: VMin=3.4558V VMean=3.4602V Block=3 Cell=7 String=1
🔍 CAN: ID=0x757F803 Len=8 Data=[01 00 10 00 0B 00 00 00]
⚠️ Non-BMS frame: 0x757F803
🔍 CAN: ID=0x19A Len=8 Data=[92 0F FC FF 1D 00 12 00] (Basic data)
📊 BMS26-190: U=249.12V I=4095.75A SOC=18.0% E=2.9kWh MasterErr=NO
🔍 CAN: ID=0x29A Len=8 Data=[FE 86 01 03 07 2A 87 1C] (Cell voltages)
📊 BMS26-290: VMin=3.4558V VMean=3.4602V Block=3 Cell=7 String=1
🔍 CAN: ID=0x39A Len=8 Data=[3B 87 01 05 04 3D 00 1C] (Max voltages)
📊 BMS26-390: VMax=3.4619V VDelta=0.0061V Block=5 Cell=4 String=1
🔍 CAN: ID=0x41A Len=8 Data=[17 01 01 01 01 00 00 00] (Temperatures)
📊 BMS26-410: TMax=23.0°C TDelta=1.0°C Ready: Chg=❌ Dchg=❌
🔍 CAN: ID=0x1BA Len=8 Data=[00 00 00 00 00 00 00 00] (Basic data)
🔍 CAN: ID=0x51A Len=8 Data=[00 01 00 00 01 3B 01 00] (Power limits)
📊 BMS26-510: ChgLim=16.00A DchgLim=19.69A R1=OFF R2=OFF IN01=LOW IN02=LOW
🔍 CAN: ID=0x19A Len=8 Data=[92 0F FC FF 1D 00 12 00] (Basic data)
📊 BMS26-190: U=249.12V I=4095.75A SOC=18.0% E=2.9kWh MasterErr=NO
🔍 CAN: ID=0x51A Len=8 Data=[00 01 00 00 01 3B 01 00] (Power limits)
📊 BMS26-510: ChgLim=16.00A DchgLim=19.69A R1=OFF R2=OFF IN01=LOW IN02=LOW
🔍 CAN: ID=0x49A Len=8 Data=[16 01 01 02 16 22 00 22]
🔍 CAN: ID=0x31A Len=8 Data=[59 00 00 00 00 00 00 64] (Multiplexed)
📊 BMS26-310: SOH=100.0% CellV=0.0mV CellT=0.0°C DCiR=0.0mΩ
🔍 CAN: ID=0x1BA Len=8 Data=[00 00 00 00 00 00 00 00] (Basic data)
🔍 CAN: ID=0x29A Len=8 Data=[FE 86 01 03 07 2A 87 1C] (Cell voltages)
📊 BMS26-290: VMin=3.4558V VMean=3.4602V Block=3 Cell=7 String=1



MOnitor z kodu w ramach tego projektu

🚀 ESP32S3 STARTING...


🚀 ESP32S3 CAN to Modbus TCP Bridge
📋 Version: v4.0.2 - CAN Handler Removed
📅 Build Date: Aug 30 2025 14:56:39
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

🧹 CLEARING EEPROM to force new configuration...
✅ EEPROM cleared - will load Node 26 config
🔧 Initializing ESP32S3 CAN to Modbus TCP Bridge...
📋 System Architecture: Modular v4.0.2 (can_handler removed)

[00:00:01] LED System: ✅ OK
✅ LED initialized on GPIO21
📚 Loading configuration from EEPROM...
⚙️ First boot - initializing default configuration
🔧 Initializing default configuration...
✅ Default configuration initialized
💾 Saving configuration to EEPROM...
✅ Configuration saved successfully
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
[  1318][D][WiFiGeneric.cpp:1040] _eventCallback(): Arduino Event: 0 - WIFI_READY
[  1358][D][WiFiGeneric.cpp:1040] _eventCallback(): Arduino Event: 2 - STA_START
📡 Connecting to WiFi: WNK3
📡 WiFi credentials updated: SSID=WNK3
📡 WiFi state changed: Disconnected → Connecting
📡 WiFi state changed: 0 -> 1
[  1531][D][WiFiGeneric.cpp:1040] _eventCallback(): Arduino Event: 4 - STA_CONNECTED
📡 WiFi station connected to AP
[  1559][D][WiFiGeneric.cpp:1040] _eventCallback(): Arduino Event: 7 - STA_GOT_IP
[  1566][D][WiFiGeneric.cpp:1103] _eventCallback(): STA IP: 192.168.148.42, MASK: 255.255.255.0, GW: 192.168.148.1
✅ WiFi connected to WNK3
📡 IP Address: 192.168.148.42
📶 RSSI: -37 dBm
📡 WiFi state changed: Connecting → Connected
📡 WiFi state changed: 1 -> 2
✅ WiFi Manager initialized and connected
✅ OK
🚌 BMS Protocol + CAN... 🚌 Initializing BMS Protocol...
🚌 Initializing CAN controller...
🔧 Configuring SPI pins for CAN Expansion Board...
   MOSI: GPIO9
   MISO: GPIO8
   SCK:  GPIO7
   CS:   GPIO44
✅ SPI pins configured
📍 CS Pin: GPIO44
✅ CS pin control OK
🔄 Initializing CAN at 125 kbps (fixed)...
✅ CAN initialized at 125 kbps (fixed)
📋 CAN controller ready at 125 kbps
🎯 Monitoring BMS Node IDs: 26(0x1A)
✅ CAN controller initialized successfully
   📍 CS Pin: 44
   🚌 Baud Rate: 125 kbps
   🎯 Frame filters: BMS protocols
✅ BMS Protocol initialized successfully
   🎯 Monitoring 1 BMS nodes
   🚌 CAN Bus: 125 kbps, MCP2515 controller
   📊 Frame validation: enabled
✅ OK
⚡ TRIO HP Manager... TRIO HP Manager initialized successfully
✅ OK
⚙️  TRIO HP Config... TRIO HP configuration loaded from EEPROM
TRIO HP Config validation failed:
  Timing valid: NO
  Module config valid: YES
  Threshold valid: NO
  Error count: 3
  Error 1: Invalid timing configuration
  Error 2: Invalid voltage limits
  Error 3: Invalid Modbus register range
WARNING: Allowing startup despite validation issues (temporary fix)
TRIO HP Configuration initialized
✅ OK
📊 TRIO HP Monitor... TRIO HP Monitor initialized successfully
✅ OK
   🎯 Monitoring 1 BMS nodes at 125 kbps
   🔋 Node IDs: 26
⚡ TRIO HP Phase 3... 🔧 Initializing TRIO HP Phase 3 systems...
   🛡️  Safety Limi ts... [TRIO HP LIMITS] Initialized with safe defaults
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
   ⚙️  Configuration... ✅ OK
✅ TRIO HP Phase 3 initialization completed
   🛡️  BMS safety limits integrated
   🎛️  Active & Reactive power controllers ready
   📈 Efficiency monitoring active
   🔒 Parameter locking system configured
✅ OK
   🛡️  Safety limits and controllers initialized
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
🌐 Web Server... ⚠️ DISABLED (memory optimization)

✅ All modules initialized successfully
✅ System initialization completed successfully!
🚀 ESP32S3 CAN to Modbus TCP Bridge is READY!

📊 === SYSTEM STATUS ===
🔄 System State: Running
⏰ Boot Time: 3444 ms
💾 Free Heap: 157.1 KB
📶 WiFi Status: Connected
🚌 BMS Protocol: Healthy
🔗 Modbus Status: Running
🔋 Active BMS: 0/1


📊 Starting main processing loop...


[TRIO HP MANAGER] Command queue: Module 0 not found, removing from queue
[TRIO HP MANAGER] Command queue: Module 0 not found, removing from queue
[TRIO HP MANAGER] Command queue: Module 0 not found, removing from queue
[TRIO HP MANAGER] Command queue: Module 0 not found, removing from queue
[TRIO HP MANAGER] Command queue: Module 0 not found, removing from queue
[TRIO HP MANAGER] Command queue: Module 0 not found, removing from queue
[TRIO HP MANAGER] Command queue: Module 0 not found, removing from queue
[TRIO HP MANAGER] Command queue: Module 0 not found, removing from queue
[TRIO HP MANAGER] Command queue: Module 0 not found, removing from queue
[TRIO HP MANAGER] Command queue: Module 0 not found, removing from queue
[TRIO HP MANAGER] Command queue: Module 0 not found, removing from queue
[TRIO HP MANAGER] Command queue: Module 0 not found, removing from queue
[TRIO HP MANAGER] Command queue: Module 0 not found, removing from queue
[TRIO HP MANAGER] Command queue: Module 0 not found, removing from queue
[TRIO HP MANAGER] Command queue: Module 0 not found, removing from queue
[TRIO HP MANAGER] Command queue: Module 0 not found, removing from queue
[TRIO HP MANAGER] Command queue: Module 0 not found, removing from queue
[TRIO HP MANAGER] Command queue: Module 0 not found, removing from queue
[TRIO HP MANAGER] Command queue: Module 0 not found, removing from queue
[TRIO HP MANAGER] Command queue: Module 0 not found, removing from queue
[TRIO HP MANAGER] Command queue: Module 0 not found, removing from queue
[TRIO HP MANAGER] Command queue: Module 0 not found, removing from queue
[TRIO HP MANAGER] Command queue: Module 0 not found, removing from queue
[TRIO HP MANAGER] Command queue: Module 0 not found, removing from queue
[TRIO HP MANAGER] Command queue: Module 0 not found, removing from queue
[TRIO HP MANAGER] Command queue: Module 0 not found, removing from queue
[TRIO HP MANAGER] Command queue: Module 0 not found, removing from queue
[TRIO HP MANAGER] Command queue: Module 0 not found, removing from queue
[TRIO HP MANAGER] Command queue: Module 0 not found, removing from queue
[TRIO HP MANAGER] Command queue: Module 0 not found, removing from queue
[TRIO HP MANAGER] Command queue: Module 0 not found, removing from queue
[TRIO HP MANAGER] Command queue: Module 0 not found, removing from queue
[TRIO HP MANAGER] Command queue: Module 0 not found, removing from queue
[TRIO HP MANAGER] Command queue: Module 0 not found, removing from queue
[TRIO HP MANAGER] Command queue: Module 0 not found, removing from queue
[TRIO HP MANAGER] Command queue: Module 0 not found, removing from queue
[TRIO HP MANAGER] Command queue: Module 0 not found, removing from queue
[TRIO HP MANAGER] Command queue: Module 0 not found, removing from queue
[TRIO HP MANAGER] Command queue: Module 0 not found, removing from queue
[TRIO HP MANAGER] Command queue: Module 0 not found, removing from queue
[TRIO HP MANAGER] Command queue: Module 0 not found, removing from queue
[TRIO HP MANAGER] Command queue: Module 0 not found, removing from queue
[TRIO HP MANAGER] Command queue: Module 0 not found, removing from queue
[TRIO HP MANAGER] Command queue: Module 0 not found, removing from queue
[TRIO HP MANAGER] Command queue: Module 0 not found, removing from queue
[TRIO HP MANAGER] Command queue: Module 0 not found, removing from queue
[TRIO HP MANAGER] Command queue: Module 0 not found, removing from queue
[TRIO HP MANAGER] Command queue: Module 0 not found, removing from queue
[TRIO HP LIMITS] WARNING: BMS node 26 data is stale
[TRIO HP LIMITS] WARNING: No valid digital inputs available
[EFFICIENCY MONITOR] ERROR: Invalid battery voltage
[TRIO HP LIMITS] WARNING: BMS node 26 data is stale
[TRIO HP LIMITS] WARNING: No valid digital inputs available
[EFFICIENCY MONITOR] ERROR: Invalid battery voltage
Executing system broadcast poll...
System poll cmd 0x1001 (DC Voltage)
System poll cmd 0x1002 (DC Current)
System poll cmd 0x1010 (Unknown)
[TRIO HP LIMITS] WARNING: BMS node 26 data is stale
[TRIO HP LIMITS] WARNING: No valid digital inputs available
[EFFICIENCY MONITOR] ERROR: Invalid battery voltage
[TRIO HP LIMITS] WARNING: BMS node 26 data is stale
[TRIO HP LIMITS] WARNING: No valid digital inputs available
[EFFICIENCY MONITOR] ERROR: Invalid battery voltage
[TRIO HP LIMITS] WARNING: BMS node 26 data is stale
[TRIO HP LIMITS] WARNING: No valid digital inputs available
[EFFICIENCY MONITOR] ERROR: Invalid battery voltage
[TRIO HP LIMITS] WARNING: BMS node 26 data is stale
[TRIO HP LIMITS] WARNING: No valid digital inputs available
[EFFICIENCY MONITOR] ERROR: Invalid battery voltage
[TRIO HP LIMITS] WARNING: BMS node 26 data is stale
[TRIO HP LIMITS] WARNING: No valid digital inputs available
[EFFICIENCY MONITOR] ERROR: Invalid battery voltage
Executing system broadcast poll...
System poll cmd 0x1001 (DC Voltage)
System poll cmd 0x1002 (DC Current)
System poll cmd 0x1010 (Unknown)
[TRIO HP LIMITS] WARNING: BMS node 26 data is stale
[TRIO HP LIMITS] WARNING: No valid digital inputs available
[EFFICIENCY MONITOR] ERROR: Invalid battery voltage
[TRIO HP LIMITS] WARNING: BMS node 26 data is stale
[TRIO HP LIMITS] WARNING: No valid digital inputs available
[EFFICIENCY MONITOR] ERROR: Invalid battery voltage
[TRIO HP LIMITS] WARNING: BMS node 26 data is stale
[TRIO HP LIMITS] WARNING: No valid digital inputs available
[EFFICIENCY MONITOR] ERROR: Invalid battery voltage
[TRIO HP LIMITS] WARNING: BMS node 26 data is stale
[TRIO HP LIMITS] WARNING: No valid digital inputs available
[EFFICIENCY MONITOR] ERROR: Invalid battery voltage
[TRIO HP LIMITS] WARNING: BMS node 26 data is stale
[TRIO HP LIMITS] WARNING: No valid digital inputs available
[EFFICIENCY MONITOR] ERROR: Invalid battery voltage
Executing system broadcast poll...
System poll cmd 0x1001 (DC Voltage)
System poll cmd 0x1002 (DC Current)
System poll cmd 0x1010 (Unknown)
[TRIO HP LIMITS] WARNING: BMS node 26 data is stale
[TRIO HP LIMITS] WARNING: No valid digital inputs available
[EFFICIENCY MONITOR] ERROR: Invalid battery voltage
[TRIO HP LIMITS] WARNING: BMS node 26 data is stale
[TRIO HP LIMITS] WARNING: No valid digital inputs available
[EFFICIENCY MONITOR] ERROR: Invalid battery voltage
[TRIO HP LIMITS] WARNING: BMS node 26 data is stale
[TRIO HP LIMITS] WARNING: No valid digital inputs available
[EFFICIENCY MONITOR] ERROR: Invalid battery voltage
[TRIO HP LIMITS] WARNING: BMS node 26 data is stale
[TRIO HP LIMITS] WARNING: No valid digital inputs available
[EFFICIENCY MONITOR] ERROR: Invalid battery voltage
[TRIO HP LIMITS] WARNING: BMS node 26 data is stale
[TRIO HP LIMITS] WARNING: No valid digital inputs available
[EFFICIENCY MONITOR] ERROR: Invalid battery voltage
Executing system broadcast poll...
System poll cmd 0x1001 (DC Voltage)
System poll cmd 0x1002 (DC Current)
System poll cmd 0x1010 (Unknown)
[TRIO HP LIMITS] WARNING: BMS node 26 data is stale
[TRIO HP LIMITS] WARNING: No valid digital inputs available
[EFFICIENCY MONITOR] ERROR: Invalid battery voltage
[TRIO HP LIMITS] WARNING: BMS node 26 data is stale
[TRIO HP LIMITS] WARNING: No valid digital inputs available
[EFFICIENCY MONITOR] ERROR: Invalid battery voltage
[TRIO HP LIMITS] WARNING: BMS node 26 data is stale
[TRIO HP LIMITS] WARNING: No valid digital inputs available
[EFFICIENCY MONITOR] ERROR: Invalid battery voltage
[TRIO HP LIMITS] WARNING: BMS node 26 data is stale
[TRIO HP LIMITS] WARNING: No valid digital inputs available
[EFFICIENCY MONITOR] ERROR: Invalid battery voltage
[TRIO HP LIMITS] WARNING: BMS node 26 data is stale
[TRIO HP LIMITS] WARNING: No valid digital inputs available
[EFFICIENCY MONITOR] ERROR: Invalid battery voltage
Executing system broadcast poll...
System poll cmd 0x1001 (DC Voltage)
System poll cmd 0x1002 (DC Current)
System poll cmd 0x1010 (Unknown)
[TRIO HP LIMITS] WARNING: BMS node 26 data is stale
[TRIO HP LIMITS] WARNING: No valid digital inputs available
[EFFICIENCY MONITOR] ERROR: Invalid battery voltage
[TRIO HP LIMITS] WARNING: BMS node 26 data is stale
[TRIO HP LIMITS] WARNING: No valid digital inputs available
[EFFICIENCY MONITOR] ERROR: Invalid battery voltage
[TRIO HP LIMITS] WARNING: BMS node 26 data is stale
[TRIO HP LIMITS] WARNING: No valid digital inputs available
[EFFICIENCY MONITOR] ERROR: Invalid battery voltage
[TRIO HP LIMITS] WARNING: BMS node 26 data is stale
[TRIO HP LIMITS] WARNING: No valid digital inputs available
[EFFICIENCY MONITOR] ERROR: Invalid battery voltage
[TRIO HP LIMITS] WARNING: BMS node 26 data is stale
[TRIO HP LIMITS] WARNING: No valid digital inputs available
[EFFICIENCY MONITOR] ERROR: Invalid battery voltage
⚠️ System health degraded - entering error state
   ❌ BMS Protocol/CAN communication issues
   ❌ No active BMS communication
🔄 Attempting system recovery...
🐕 Watchdog reset - system healthy
Executing system broadcast poll...
System poll cmd 0x1001 (DC Voltage)
System poll cmd 0x1002 (DC Current)
System poll cmd 0x1010 (Unknown)
[TRIO HP LIMITS] WARNING: BMS node 26 data is stale
[TRIO HP LIMITS] WARNING: No valid digital inputs available
[EFFICIENCY MONITOR] ERROR: Invalid battery voltage
[TRIO HP LIMITS] WARNING: BMS node 26 data is stale
[TRIO HP LIMITS] WARNING: No valid digital inputs available
[EFFICIENCY MONITOR] ERROR: Invalid battery voltage
[TRIO HP LIMITS] WARNING: BMS node 26 data is stale
[TRIO HP LIMITS] WARNING: No valid digital inputs available
[EFFICIENCY MONITOR] ERROR: Invalid battery voltage
[TRIO HP LIMITS] WARNING: BMS node 26 data is stale
[TRIO HP LIMITS] WARNING: No valid digital inputs available
[EFFICIENCY MONITOR] ERROR: Invalid battery voltage
[TRIO HP LIMITS] WARNING: BMS node 26 data is stale
[TRIO HP LIMITS] WARNING: No valid digital inputs available
[EFFICIENCY MONITOR] ERROR: Invalid battery voltage
Executing system broadcast poll...
System poll cmd 0x1001 (DC Voltage)
System poll cmd 0x1002 (DC Current)
System poll cmd 0x1010 (Unknown)
[TRIO HP LIMITS] WARNING: BMS node 26 data is stale
[TRIO HP LIMITS] WARNING: No valid digital inputs available
[EFFICIENCY MONITOR] ERROR: Invalid battery voltage
[TRIO HP LIMITS] WARNING: BMS node 26 data is stale
[TRIO HP LIMITS] WARNING: No valid digital inputs available
[EFFICIENCY MONITOR] ERROR: Invalid battery voltage
[TRIO HP LIMITS] WARNING: BMS node 26 data is stale
[TRIO HP LIMITS] WARNING: No valid digital inputs available
[EFFICIENCY MONITOR] ERROR: Invalid battery voltage
[TRIO HP LIMITS] WARNING: BMS node 26 data is stale
[TRIO HP LIMITS] WARNING: No valid digital inputs available
[EFFICIENCY MONITOR] ERROR: Invalid battery voltage
[TRIO HP LIMITS] WARNING: BMS node 26 data is stale
[TRIO HP LIMITS] WARNING: No valid digital inputs available
[EFFICIENCY MONITOR] ERROR: Invalid battery voltage
Executing system broadcast poll...
System poll cmd 0x1001 (DC Voltage)
System poll cmd 0x1002 (DC Current)
System poll cmd 0x1010 (Unknown)
[TRIO HP LIMITS] WARNING: BMS node 26 data is stale
[TRIO HP LIMITS] WARNING: No valid digital inputs available
[EFFICIENCY MONITOR] ERROR: Invalid battery voltage
[TRIO HP LIMITS] WARNING: BMS node 26 data is stale
[TRIO HP LIMITS] WARNING: No valid digital inputs available
[EFFICIENCY MONITOR] ERROR: Invalid battery voltage
[TRIO HP LIMITS] WARNING: BMS node 26 data is stale
[TRIO HP LIMITS] WARNING: No valid digital inputs available
[EFFICIENCY MONITOR] ERROR: Invalid battery voltage
[TRIO HP LIMITS] WARNING: BMS node 26 data is stale
[TRIO HP LIMITS] WARNING: No valid digital inputs available
[EFFICIENCY MONITOR] ERROR: Invalid battery voltage
[TRIO HP LIMITS] WARNING: BMS node 26 data is stale
[TRIO HP LIMITS] WARNING: No valid digital inputs available
[EFFICIENCY MONITOR] ERROR: Invalid battery voltage
Executing system broadcast poll...
System poll cmd 0x1001 (DC Voltage)
System poll cmd 0x1002 (DC Current)
System poll cmd 0x1010 (Unknown)
[TRIO HP LIMITS] WARNING: BMS node 26 data is stale
[TRIO HP LIMITS] WARNING: No valid digital inputs available
[EFFICIENCY MONITOR] ERROR: Invalid battery voltage
[TRIO HP LIMITS] WARNING: BMS node 26 data is stale
[TRIO HP LIMITS] WARNING: No valid digital inputs available
[EFFICIENCY MONITOR] ERROR: Invalid battery voltage
[TRIO HP LIMITS] WARNING: BMS node 26 data is stale
[TRIO HP LIMITS] WARNING: No valid digital inputs available
[EFFICIENCY MONITOR] ERROR: Invalid battery voltage
[TRIO HP LIMITS] WARNING: BMS node 26 data is stale
[TRIO HP LIMITS] WARNING: No valid digital inputs available
[EFFICIENCY MONITOR] ERROR: Invalid battery voltage
[TRIO HP LIMITS] WARNING: BMS node 26 data is stale
[TRIO HP LIMITS] WARNING: No valid digital inputs available
[EFFICIENCY MONITOR] ERROR: Invalid battery voltage
Executing system broadcast poll...
System poll cmd 0x1001 (DC Voltage)
System poll cmd 0x1002 (DC Current)
System poll cmd 0x1010 (Unknown)
[TRIO HP LIMITS] WARNING: BMS node 26 data is stale
[TRIO HP LIMITS] WARNING: No valid digital inputs available
[EFFICIENCY MONITOR] ERROR: Invalid battery voltage
[TRIO HP LIMITS] WARNING: BMS node 26 data is stale
[TRIO HP LIMITS] WARNING: No valid digital inputs available
[EFFICIENCY MONITOR] ERROR: Invalid battery voltage
[TRIO HP LIMITS] WARNING: BMS node 26 data is stale
[TRIO HP LIMITS] WARNING: No valid digital inputs available
[EFFICIENCY MONITOR] ERROR: Invalid battery voltage
[TRIO HP LIMITS] WARNING: BMS node 26 data is stale
[TRIO HP LIMITS] WARNING: No valid digital inputs available
[EFFICIENCY MONITOR] ERROR: Invalid battery voltage
[TRIO HP LIMITS] WARNING: BMS node 26 data is stale
[TRIO HP LIMITS] WARNING: No valid digital inputs available
[EFFICIENCY MONITOR] ERROR: Invalid battery voltage
Executing system broadcast poll...
System poll cmd 0x1001 (DC Voltage)
System poll cmd 0x1002 (DC Current)
System poll cmd 0x1010 (Unknown)
[TRIO HP LIMITS] WARNING: BMS node 26 data is stale
[TRIO HP LIMITS] WARNING: No valid digital inputs available
[EFFICIENCY MONITOR] ERROR: Invalid battery voltage