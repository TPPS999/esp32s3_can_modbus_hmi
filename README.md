# ESP32S3 CAN to Modbus TCP Bridge

## 📋 Przegląd Projektu

Ten projekt implementuje most między protokołami CAN Bus i Modbus TCP używając modułu ESP32S3. System umożliwia monitorowanie i zarządzanie systemami BMS (Battery Management System) poprzez standardowy protokół Modbus TCP przez sieć WiFi.

### ✨ Główne Funkcje

- **CAN Bus Interface**: Odczyt danych z urządzeń BMS przez protokół CAN
- **Modbus TCP Server**: Serwer Modbus TCP na porcie 502 
- **WiFi Connectivity**: Połączenie bezprzewodowe z automatycznym reconnect
- **🔥 CAN-Triggered AP Mode**: Tryb AP wyzwalany przez specjalne ramki CAN (ID: 0xEF1)
- **🌐 Web Configuration Interface**: Kompletny interfejs konfiguracyjny w trybie AP
- **⚙️ CAN Speed Configuration**: Konfiguracja prędkości CAN (125/500 kbps)
- **🔋 Dynamic BMS Setup**: Dynamiczna konfiguracja liczby i ID baterii
- **📊 CAN Frame Monitoring**: Podgląd adresów ramek CAN dla każdego BMS
- **Multi-BMS Support**: Obsługa do 16 modułów BMS jednocześnie
- **Real-time Monitoring**: Monitoring w czasie rzeczywistym z diagnostyką systemu

## 🔧 Specyfikacja Techniczna

### Hardware Requirements
- **Mikrokontroler**: ESP32S3 (Seeed Studio XIAO ESP32S3)
- **CAN Controller**: MCP2515 + TJA1050 transceiver
- **Częstotliwość**: 240 MHz CPU
- **Pamięć**: 320KB RAM, 8MB Flash
- **CAN Bus Speed**: 500 kbps (domyślnie), 125 kbps (konfigurowalny)

### Wykorzystanie Zasobów
- **RAM**: ~18% (z web serverem w trybie AP)
- **Flash**: ~30% (including AsyncWebServer libraries)
- **Kompilacja**: Bez problemów po naprawach

## 📦 Architektura Systemu

### Struktura Modułów

```
src/
├── main.cpp              # Główna pętla aplikacji
├── config.cpp            # Konfiguracja systemu i EEPROM
├── wifi_manager.cpp      # Zarządzanie połączeniem WiFi
├── modbus_tcp.cpp        # Serwer Modbus TCP
├── bms_protocol.cpp      # Protokół BMS + obsługa CAN
├── bms_data.cpp          # Struktury danych BMS
├── utils.cpp             # Funkcje pomocnicze
└── web_server.cpp        # Serwer WWW konfiguracyjny

include/
├── config.h              # Definicje konfiguracyjne
├── wifi_manager.h        # WiFi management API
├── modbus_tcp.h          # Modbus TCP API
├── bms_protocol.h        # BMS protocol API
├── bms_data.h            # Struktury danych BMS
├── utils.h               # Utility functions
└── web_server.h          # Web server API
```

## 🔌 Połączenia Hardware

### CAN Bus (MCP2515)
```
ESP32S3 Pin    MCP2515 Pin    Funkcja
GPIO5          CS             Chip Select
GPIO6          SCK            SPI Clock
GPIO7          MOSI           SPI MOSI
GPIO8          MISO           SPI MISO
GPIO9          INT            Interrupt
3.3V           VCC            Zasilanie
GND            GND            Masa
```

### CAN Bus Connector
```
Pin    Sygnał      Kolor (typowy)
1      CAN_H       Żółty
2      CAN_L       Zielony
3      GND         Czarny
4      +12V        Czerwony (opcjonalny)
```

## ⚙️ Konfiguracja

### WiFi Settings
```cpp
// W include/config.h
const char* const WIFI_SSID = "TwojSSID";
const char* const WIFI_PASSWORD = "TwojeHaslo";
```

### BMS Configuration
```cpp
#define MAX_BMS_NODES 16                    // Maksymalna liczba modułów BMS
#define BMS_COMMUNICATION_TIMEOUT_MS 30000  // Timeout komunikacji
```

### Modbus TCP Settings
```cpp
#define MODBUS_TCP_PORT 502                 // Port Modbus TCP
#define MODBUS_SLAVE_ID 1                   // ID slave'a
#define MODBUS_MAX_HOLDING_REGISTERS 3200   // 16 BMS × 200 rejestrów
```

## 📊 Mapa Rejestrów Modbus

Każdy moduł BMS zajmuje 200 rejestrów Modbus (16-bit każdy):

### Mapowanie adresów:
- **BMS 1** (Node ID 1): Rejestry 0-199
- **BMS 2** (Node ID 2): Rejestry 200-399
- **BMS 3** (Node ID 3): Rejestry 400-599
- **BMS 4** (Node ID 4): Rejestry 600-799
- **BMS 5** (Node ID 5): Rejestry 800-999
- **BMS 6** (Node ID 6): Rejestry 1000-1199
- **BMS 7** (Node ID 7): Rejestry 1200-1399
- **BMS 8** (Node ID 8): Rejestry 1400-1599
- **BMS 9** (Node ID 9): Rejestry 1600-1799
- **BMS 10** (Node ID 10): Rejestry 1800-1999
- **BMS 11** (Node ID 11): Rejestry 2000-2199
- **BMS 12** (Node ID 12): Rejestry 2200-2399
- **BMS 13** (Node ID 13): Rejestry 2400-2599
- **BMS 14** (Node ID 14): Rejestry 2600-2799
- **BMS 15** (Node ID 15): Rejestry 2800-2999
- **BMS 16** (Node ID 16): Rejestry 3000-3199

### 🎯 Zalety Nowego Mapowania (200 rejestrów na BMS):
- **Łatwe obliczenia**: BMS_ID × 200 = adres bazowy
- **Czytelność**: Okrągłe liczby (0, 200, 400, 600...)
- **Rezerwa na przyszłość**: 75 dodatkowych rejestrów na każdy BMS
- **Kompatybilność SCADA**: Standardowe przesunięcia 200

### 📋 Szczegółowa Mapa Rejestrów BMS

#### 🔋 Podstawowe Parametry Baterii (Frame 0x190)

| Adres | Nazwa | Typ Danych | Jednostka | Skala | Format | Opis |
|-------|-------|------------|-----------|-------|--------|------|
| 0 | batteryVoltage | uint16 | mV | ×1000 | LSB | Napięcie pack baterii |
| 1 | batteryCurrent | int16 | mA | ×1000 | LSB | Prąd baterii (+ ładowanie, - rozładowanie) |
| 2 | remainingEnergy | uint16 | Wh | ×100 | LSB | Pozostała energia |
| 3 | soc | uint16 | % | ×100 | LSB | Stan naładowania (0-100%) |
| 4-9 | flags | uint16 | - | - | bits | Flagi błędów systemowych |

#### 🔋 Dane Ogniw (Frame 0x290)

| Adres | Nazwa | Typ Danych | Jednostka | Skala | Format | Opis |
|-------|-------|------------|-----------|-------|--------|------|
| 10 | cellMinVoltage | uint16 | mV | ×1000 | LSB | Minimalne napięcie ogniwa |
| 11 | cellMeanVoltage | uint16 | mV | ×1000 | LSB | Średnie napięcie ogniwa |
| 12 | minVoltageBlock | uint8 | - | ×1 | LSB | ID bloku z min napięciem |
| 13 | minVoltageCell | uint8 | - | ×1 | MSB | ID ogniwa z min napięciem |
| 14 | minVoltageString | uint8 | - | ×1 | LSB | ID stringa z min napięciem |
| 15 | balancingTempMax | uint8 | °C | ×1 | MSB | Max temperatura balansowania |

#### 🌡️ SOH i Temperatura (Frame 0x310)

| Adres | Nazwa | Typ Danych | Jednostka | Skala | Format | Opis |
|-------|-------|------------|-----------|-------|--------|------|
| 20 | soh | uint16 | % | ×100 | LSB | Stan zdrowia baterii |
| 21 | cellVoltage | uint16 | mV | ×1 | LSB | Napięcie ogniwa pomiarowego |
| 22 | cellTemperature | int16 | °C | ×10 | LSB | Temperatura ogniwa |
| 23 | cellMinTemperature | int8 | °C | ×1 | LSB | Minimalna temperatura |
| 24 | cellMeanTemperature | int8 | °C | ×1 | MSB | Średnia temperatura |
| 25 | dcir | uint16 | mΩ | ×1 | LSB | Impedancja wewnętrzna DC |

#### ⚡ Maksymalne Wartości (Frame 0x390)

| Adres | Nazwa | Typ Danych | Jednostka | Skala | Format | Opis |
|-------|-------|------------|-----------|-------|--------|------|
| 30 | cellMaxVoltage | uint16 | mV | ×1000 | LSB | Maksymalne napięcie ogniwa |
| 31 | cellVoltageDelta | uint16 | mV | ×1000 | LSB | Delta napięć ogniw |
| 32 | maxVoltageBlock | uint8 | - | ×1 | LSB | ID bloku z max napięciem |
| 33 | maxVoltageCell | uint8 | - | ×1 | MSB | ID ogniwa z max napięciem |
| 34 | maxVoltageString | uint8 | - | ×1 | LSB | ID stringa z max napięciem |
| 35 | afeTemperatureMax | uint8 | °C | ×1 | MSB | Max temperatura AFE |

#### 🌡️ Temperatury i Gotowość (Frame 0x410)

| Adres | Nazwa | Typ Danych | Jednostka | Skala | Format | Opis |
|-------|-------|------------|-----------|-------|--------|------|
| 40 | cellMaxTemperature | int16 | °C | ×10 | LSB | Maksymalna temperatura ogniwa |
| 41 | cellTempDelta | int16 | °C | ×10 | LSB | Delta temperatur ogniw |
| 42 | maxTempString | uint8 | - | ×1 | LSB | String z max temperaturą |
| 43 | maxTempBlock | uint8 | - | ×1 | MSB | Blok z max temperaturą |
| 44 | maxTempSensor | uint8 | - | ×1 | LSB | Sensor z max temperaturą |
| 45 | readyFlags | uint16 | - | - | bits | Flagi gotowości (bit0=charge, bit1=discharge) |

#### ⚡ Limity Mocy i I/O (Frame 0x510)

| Adres | Nazwa | Typ Danych | Jednostka | Skala | Format | Opis |
|-------|-------|------------|-----------|-------|--------|------|
| 50 | dccl | uint16 | A | ×100 | LSB | Ciągły limit prądu rozładowania |
| 51 | ddcl | uint16 | A | ×100 | LSB | Ciągły limit mocy rozładowania |
| 52 | inputs | uint8 | - | - | bits | Stan wejść cyfrowych |
| 53 | outputs | uint8 | - | - | bits | Stan wyjść cyfrowych |
| 54-59 | ioFlags | uint16 | - | - | bits | Szczegółowe flagi I/O |

#### 🔀 Dane Multipleksowane (Frame 0x490)

| Adres | Nazwa | Typ Danych | Jednostka | Skala | Format | Opis |
|-------|-------|------------|-----------|-------|--------|------|
| 60 | mux490Type | uint8 | - | ×1 | LSB | Typ multipleksera (0x00-0x35) |
| 61 | mux490Value | uint16 | var | var | LSB | Wartość multipleksera |
| 62 | serialNumber0 | uint16 | - | ×1 | LSB | Numer seryjny (młodsze 16 bit) |
| 63 | serialNumber1 | uint16 | - | ×1 | MSB | Numer seryjny (starsze 16 bit) |
| 64 | hwVersion0 | uint16 | - | ×1 | LSB | Wersja HW (młodsze 16 bit) |
| 65 | hwVersion1 | uint16 | - | ×1 | MSB | Wersja HW (starsze 16 bit) |
| 66 | swVersion0 | uint16 | - | ×1 | LSB | Wersja SW (młodsze 16 bit) |
| 67 | swVersion1 | uint16 | - | ×1 | MSB | Wersja SW (starsze 16 bit) |
| 68 | factoryEnergy | uint16 | kWh | ×100 | LSB | Energia fabryczna |
| 69 | designCapacity | uint16 | Ah | ×100 | LSB | Pojemność projektowa |

#### 🏥 Error Maps i Diagnostyka

| Adres | Nazwa | Typ Danych | Jednostka | Skala | Format | Opis |
|-------|-------|------------|-----------|-------|--------|------|
| 80 | errorsMap0 | uint16 | - | - | bits | Mapa błędów (bity 0-15) |
| 81 | errorsMap1 | uint16 | - | - | bits | Mapa błędów (bity 16-31) |
| 82 | errorsMap2 | uint16 | - | - | bits | Mapa błędów (bity 32-47) |
| 83 | errorsMap3 | uint16 | - | - | bits | Mapa błędów (bity 48-63) |
| 84 | timeToFullCharge | uint16 | min | ×1 | LSB | Czas do pełnego naładowania |
| 85 | timeToFullDischarge | uint16 | min | ×1 | LSB | Czas do pełnego rozładowania |
| 86 | batteryCycles | uint16 | - | ×1 | LSB | Liczba cykli baterii |

#### 🌡️ Temperatura Rozszerzona

| Adres | Nazwa | Typ Danych | Jednostka | Skala | Format | Opis |
|-------|-------|------------|-----------|-------|--------|------|
| 90 | inletTemperature | int16 | °C | ×10 | LSB | Temperatura wlotu |
| 91 | outletTemperature | int16 | °C | ×10 | LSB | Temperatura wylotu |
| 92 | humidity | uint16 | % | ×10 | LSB | Wilgotność |
| 93 | ballancerTempMax | int16 | °C | ×10 | LSB | Max temperatura balansera |
| 94 | ltcTempMax | int16 | °C | ×10 | LSB | Max temperatura LTC |

#### ⚡ Energia i Moc Rozszerzona

| Adres | Nazwa | Typ Danych | Jednostka | Skala | Format | Opis |
|-------|-------|------------|-----------|-------|--------|------|
| 100 | maxDischargePower | uint16 | W | ×1 | LSB | Maksymalna moc rozładowania |
| 101 | maxChargePower | uint16 | W | ×1 | LSB | Maksymalna moc ładowania |
| 102 | balancingEnergy | uint16 | Wh | ×1 | LSB | Energia balansowania |
| 103 | chargeEnergy0 | uint16 | kWh | ×100 | LSB | Energia ładowania (młodsze) |
| 104 | chargeEnergy1 | uint16 | kWh | ×100 | MSB | Energia ładowania (starsze) |
| 105 | dischargeEnergy0 | uint16 | kWh | ×100 | LSB | Energia rozładowania (młodsze) |
| 106 | dischargeEnergy1 | uint16 | kWh | ×100 | MSB | Energia rozładowania (starsze) |

#### 📡 Stan Komunikacji i Diagnostyka

| Adres | Nazwa | Typ Danych | Jednostka | Skala | Format | Opis |
|-------|-------|------------|-----------|-------|--------|------|
| 110 | canopenState | uint8 | - | ×1 | LSB | Stan protokołu CANopen |
| 111 | communicationOk | uint16 | - | - | bool | Status komunikacji (0/1) |
| 112 | packetsReceived | uint16 | - | ×1 | LSB | Liczba odebranych pakietów |
| 113 | parseErrors | uint16 | - | ×1 | LSB | Liczba błędów parsowania |
| 114 | totalFrames | uint16 | - | ×1 | LSB | Całkowita liczba ramek |
| 115 | lastUpdateTime | uint32 | ms | ×1 | LSB+MSB | Timestamp ostatniej aktualizacji |
| 117 | frame190Count | uint16 | - | ×1 | LSB | Licznik ramek 0x190 |
| 118 | frame290Count | uint16 | - | ×1 | LSB | Licznik ramek 0x290 |
| 119 | frame310Count | uint16 | - | ×1 | LSB | Licznik ramek 0x310 |
| 120 | frame390Count | uint16 | - | ×1 | LSB | Licznik ramek 0x390 |
| 121 | frame410Count | uint16 | - | ×1 | LSB | Licznik ramek 0x410 |
| 122 | frame510Count | uint16 | - | ×1 | LSB | Licznik ramek 0x510 |
| 123 | frame490Count | uint16 | - | ×1 | LSB | Licznik ramek 0x490 |
| 124 | reserved | uint16 | - | ×1 | LSB | Rezerwa |

#### 🔮 Rejestry Rozszerzone i Rezerwa (125-199)

| Adres | Nazwa | Typ Danych | Jednostka | Skala | Format | Opis |
|-------|-------|------------|-----------|-------|--------|------|
| 125-149 | future_data | uint16 | var | var | LSB | Rezerwa na przyszłe dane BMS |
| 150-174 | user_defined | uint16 | var | var | LSB | Rejestry definiowane przez użytkownika |
| 175-199 | system_reserved | uint16 | - | ×1 | LSB | Rezerwa systemowa |

**Korzyści z rozszerzenia do 200 rejestrów:**
- 🔮 **75 dodatkowych rejestrów** na każdy BMS dla przyszłych funkcji
- 📏 **Okrągłe adresy** dla łatwego programowania (BMS1=0, BMS2=200, BMS3=400...)
- 🔧 **Kompatybilność** z systemami SCADA preferującymi standardowe przesunięcia
- 🚀 **Skalowalność** dla nowych parametrów BMS bez zmian architektury

### 🔄 Konwersje Danych

#### Typy Formatów:
- **LSB**: Least Significant Byte (młodszy bajt)
- **MSB**: Most Significant Byte (starszy bajt) 
- **LSB+MSB**: Dane 32-bitowe w dwóch rejestrach (LSB w niższym adresie)
- **bits**: Dane bitowe (flagi)

#### Konwersje Skalowania:
```cpp
// Obliczanie adresu bazowego dla BMS
uint16_t base_address = (bms_id - 1) * 200;  // BMS 1=0, BMS 2=200, BMS 3=400...

// Napięcie: rejestr → rzeczywista wartość
float voltage = register_value / 1000.0;  // mV → V

// Temperatura: rejestr → rzeczywista wartość  
float temperature = (int16_t)register_value / 10.0;  // °C×10 → °C

// SOC/SOH: rejestr → rzeczywista wartość
float soc = register_value / 100.0;  // %×100 → %

// Energia: rejestry 32-bit → rzeczywista wartość
uint32_t energy_raw = (register_high << 16) | register_low;
float energy = energy_raw / 100.0;  // Wh×100 → kWh

// Przykłady konkretnych adresów:
// BMS 1 SOC: adres 3 (0 + 3)
// BMS 2 SOC: adres 203 (200 + 3)  
// BMS 3 SOC: adres 403 (400 + 3)
// BMS 10 SOC: adres 1803 (1800 + 3)
```

### 🔀 Tabela Typów Multipleksera (Frame 0x490)

Frame 0x490 zawiera dane multipleksowane, gdzie pierwszy bajt określa typ danych:

| Typ | Nazwa | Typ Danych | Jednostka | Skala | Opis |
|-----|-------|------------|-----------|-------|------|
| 0x00 | serialNumber0 | uint16 | - | ×1 | Numer seryjny (młodsze 16 bit) |
| 0x01 | serialNumber1 | uint16 | - | ×1 | Numer seryjny (starsze 16 bit) |
| 0x02 | hwVersion0 | uint16 | - | ×1 | Wersja hardware (młodsze 16 bit) |
| 0x03 | hwVersion1 | uint16 | - | ×1 | Wersja hardware (starsze 16 bit) |
| 0x04 | swVersion0 | uint16 | - | ×1 | Wersja software (młodsze 16 bit) |
| 0x05 | swVersion1 | uint16 | - | ×1 | Wersja software (starsze 16 bit) |
| 0x06 | factoryEnergy | uint16 | kWh | ×100 | Energia fabryczna |
| 0x07 | designCapacity | uint16 | Ah | ×100 | Pojemność projektowa |
| 0x08 | blVersion0 | uint16 | - | ×1 | Wersja bootloader (młodsze) |
| 0x09 | blVersion1 | uint16 | - | ×1 | Wersja bootloader (starsze) |
| 0x0A | appVersion0 | uint16 | - | ×1 | Wersja aplikacji (młodsze) |
| 0x0B | appVersion1 | uint16 | - | ×1 | Wersja aplikacji (starsze) |
| 0x0C | systemDesignedEnergy | uint16 | kWh | ×100 | Energia projektowa systemu |
| 0x0D | ballancerTempMax | int16 | °C | ×10 | Max temperatura balansera |
| 0x0E | ltcTempMax | int16 | °C | ×10 | Max temperatura LTC |
| 0x0F | inletTemperature | int16 | °C | ×10 | Temperatura wlotu chłodziwa |
| 0x0F | outletTemperature | int16 | °C | ×10 | Temperatura wylotu chłodziwa |
| 0x10 | humidity | uint16 | % | ×10 | Wilgotność |
| 0x11 | crcApp | uint16 | - | ×1 | CRC aplikacji |
| 0x12 | crcBoot | uint16 | - | ×1 | CRC bootloader |
| 0x13 | errorsMap0 | uint16 | - | - | Mapa błędów (bity 0-15) |
| 0x14 | errorsMap1 | uint16 | - | - | Mapa błędów (bity 16-31) |
| 0x15 | errorsMap2 | uint16 | - | - | Mapa błędów (bity 32-47) |
| 0x16 | errorsMap3 | uint16 | - | - | Mapa błędów (bity 48-63) |
| 0x17 | timeToFullCharge | uint16 | min | ×1 | Czas do pełnego naładowania |
| 0x18 | timeToFullDischarge | uint16 | min | ×1 | Czas do pełnego rozładowania |
| 0x19 | numberOfDetectedIMBs | uint16 | - | ×1 | Liczba wykrytych modułów IMB |
| 0x1A | batteryCycles | uint16 | - | ×1 | Liczba cykli baterii |
| 0x1B | numberOfDetectedIMBs | uint16 | - | ×1 | Liczba wykrytych IMB |
| 0x1C | balancingEnergy | uint16 | Wh | ×1 | Energia balansowania |
| 0x1D | maxDischargePower | uint16 | W | ×1 | Maksymalna moc rozładowania |
| 0x1E | maxChargePower | uint16 | W | ×1 | Maksymalna moc ładowania |
| 0x1F | maxDischargeEnergy | uint16 | kWh | ×100 | Maksymalna energia rozładowania |
| 0x20 | maxChargeEnergy | uint16 | kWh | ×100 | Maksymalna energia ładowania |
| 0x21-0x2F | reserved | uint16 | - | - | Rezerwa dla przyszłych użyć |
| 0x30 | chargeEnergy0 | uint16 | kWh | ×100 | Całkowita energia ładowania (młodsze) |
| 0x31 | chargeEnergy1 | uint16 | kWh | ×100 | Całkowita energia ładowania (starsze) |
| 0x32 | dischargeEnergy0 | uint16 | kWh | ×100 | Całkowita energia rozładowania (młodsze) |
| 0x33 | dischargeEnergy1 | uint16 | kWh | ×100 | Całkowita energia rozładowania (starsze) |
| 0x34 | recuperativeEnergy0 | uint16 | kWh | ×100 | Energia rekuperacji (młodsze) |
| 0x35 | recuperativeEnergy1 | uint16 | kWh | ×100 | Energia rekuperacji (starsze) |

### 📊 Przykład Dekodowania Multipleksera

```cpp
// Odczyt danych multipleksera z Frame 0x490
uint8_t mux_type = frame490Data[0];        // Typ multipleksera
uint16_t mux_value = (frame490Data[2] << 8) | frame490Data[1];  // Wartość

switch(mux_type) {
    case 0x06:  // Factory Energy
        float factory_energy = mux_value / 100.0;  // kWh
        break;
        
    case 0x0F:  // Inlet/Outlet Temperature
        float temperature = (int16_t)mux_value / 10.0;  // °C
        break;
        
    case 0x17:  // Time to Full Charge
        uint16_t charge_time = mux_value;  // minutes
        break;
}
```

## 🚀 Instalacja i Kompilacja

### Wymagania
- **PlatformIO IDE** lub **Arduino IDE** z platformą ESP32
- **Framework**: Arduino dla ESP32

### Kroki instalacji:

1. **Klonowanie repozytorium**:
```bash
git clone <repository-url>
cd esp32s3-can-modbus-tcp
```

2. **Konfiguracja PlatformIO**:
```bash
pio init --project-dir . --board seeed_xiao_esp32s3
```

3. **Instalacja zależności**:
```bash
pio lib install
```

4. **Kompilacja**:
```bash
pio run
```

5. **Upload do ESP32S3**:
```bash
pio run --target upload
```

6. **Monitor Serial**:
```bash
pio device monitor --baud 115200
```

## 📡 Protokoły CAN

System obsługuje następujące typy ramek CAN:

### Ramki Podstawowe
- **0x190-0x19F**: Podstawowe dane BMS (SOC, napięcie, prąd)
- **0x290-0x29F**: Napięcia cell indywidualnych
- **0x310-0x31F**: SOH i dane temperaturowe

### Ramki Zaawansowane  
- **0x390-0x39F**: Limity maksymalne
- **0x410-0x41F**: Dane temperaturowe rozszerzone
- **0x510-0x51F**: Limity mocy
- **0x490-0x49F**: Dane multipleksowane (54 typy)
- **0x1B0-0x1BF**: Dane dodatkowe
- **0x710-0x71F**: Protokół CANopen (adres = 0x701 + Node_ID - 1)

### Ramki Specjalne
- **0xEF1**: 🔥 **CAN-Triggered AP Mode** - wyzwalacz trybu AP (dane: 0xFF 0xBB)

### Częstotliwości Transmisji
- **Wysoka (100ms)**: Ramki 0x190 (dane podstawowe)
- **Średnia (500ms)**: Ramki 0x290, 0x310, 0x390, 0x410, 0x510
- **Niska (2000ms)**: Ramki 0x490, 0x1B0, 0x710

## 🌐 Interfejs Sieciowy

### WiFi Manager
- **Automatyczne połączenie** z zapisanym SSID
- **Fallback do trybu AP** przy braku połączenia
- **Auto-reconnect** przy utracie sygnału
- **Diagnostyka sygnału** i qualności połączenia

### Tryb Access Point (AP)
- **SSID**: `ESP32S3-CAN-XXXXXX` (gdzie XXXXXX to ostatnie 6 cyfr MAC)
- **Hasło**: `esp32modbus`
- **IP**: `192.168.4.1`
- **Port Modbus**: `502`

### 🔥 CAN-Triggered AP Mode (Nowa Funkcja!)

System umożliwia zdalną aktywację trybu AP poprzez specjalne ramki CAN:

#### Parametry Wyzwalacza:
- **CAN ID**: `0xEF1` (3825 decimal)
- **Dane**: `0xFF 0xBB` (pierwsze 2 bajty ramki)
- **Wymagane wystąpienia**: 3 ramki w ciągu 1 sekundy
- **Czas aktywności AP**: 30 sekund od ostatniego wyzwalacza

#### Jak to działa:
1. **Detekcja**: System monitoruje magistralę CAN w poszukiwaniu ramek o ID `0xEF1`
2. **Walidacja**: Sprawdza czy pierwsze 2 bajty to `0xFF 0xBB`
3. **Liczenie**: Wymaga 3 prawidłowych ramek w oknie 1 sekundy
4. **Aktywacja**: Uruchamia tryb AP z SSID `ESP32S3-CAN-XXXXXX-TRIGGER`
5. **Przedłużanie**: Każda kolejna prawidłowa ramka resetuje timer na 30 sekund
6. **Wyłączenie**: AP wyłącza się automatycznie po 30 sekundach bez wyzwalaczy

#### Przykład wysłania wyzwalacza (SocketCAN Linux):
```bash
# Wyślij 3 ramki z odstępem 200ms
cansend can0 EF1#FFBB000000000000
sleep 0.2
cansend can0 EF1#FFBB000000000000  
sleep 0.2
cansend can0 EF1#FFBB000000000000
```

#### Przykład wysłania wyzwalacza (Python):
```python
import can
import time

bus = can.interface.Bus(bustype='socketcan', channel='can0', bitrate=125000)

# Ramka wyzwalacza
trigger_frame = can.Message(
    arbitration_id=0xEF1,
    data=[0xFF, 0xBB, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00],
    is_extended_id=False
)

# Wyślij 3 ramki
for i in range(3):
    bus.send(trigger_frame)
    print(f"Wyzwalacz {i+1}/3 wysłany")
    time.sleep(0.2)
    
print("AP mode powinien być aktywny przez 30 sekund")
```

#### Zalety CAN-Triggered AP:
- **Bezpieczeństwo**: Tryb AP tylko na żądanie, nie domyślnie aktywny
- **Zdalność**: Aktywacja z dowolnego urządzenia na magistrali CAN
- **Automatyzacja**: Możliwość integracji z systemami diagnostycznymi
- **Elastyczność**: Timer resetuje się z każdym wyzwalaczem
- **Kompatybilność**: Nie koliduje ze standardowymi ramkami BMS

### 🌐 Web Configuration Interface

Po aktywacji trybu AP dostępny jest kompletny interfejs konfiguracyjny:

#### Dostęp do Interface:
1. **Aktywuj AP mode**: Wyślij 3x ramkę `0xEF1` z danymi `0xFF 0xBB`
2. **Połącz się z WiFi**: `ESP32S3-CAN-XXXXXX-TRIGGER` (hasło: `esp32modbus`)
3. **Otwórz browser**: http://192.168.4.1/

#### Dostępne Strony:

**🏠 Strona Główna** (`/`)
- Przegląd statusu systemu
- Podstawowe informacje o konfiguracji
- Szybki dostęp do wszystkich sekcji

**📡 Konfiguracja WiFi** (`/wifi`)
- Ustawienie SSID i hasła sieci WiFi
- Informacje o aktualnym połączeniu
- Zapisanie konfiguracji do EEPROM

**🔧 Konfiguracja BMS** (`/bms`)
- **Liczba aktywnych baterii**: 1-16 modułów
- **Prędkość CAN**: 125 kbps lub 500 kbps (domyślnie)
- **Przypisanie ID**: Unikalne Node ID (1-16) dla każdej baterii
- **🔥 Podgląd adresów Frame 710**: Automatyczne wyliczanie adresów (0x701 + ID - 1)
- **Modbus Layout**: Podgląd mapowania rejestrów (200 rejestrów na BMS)

**📊 Monitor CAN** (`/can`)
- **Konfiguracja CAN**: Podgląd aktualnej prędkości i liczby węzłów
- **Mapowanie Adresów**: Tabela adresów ramek 190, 290, 710 dla każdego BMS
- **Status węzłów**: Podgląd stanu komunikacji z każdym BMS
- **Typy ramek**: Opis wszystkich monitorowanych ramek CAN

**📈 Status Systemu** (`/status`)
- Informacje o sprzęcie i firmware
- Status sieci WiFi i Modbus TCP
- Wykorzystanie pamięci i czas działania
- Akcje systemowe (restart, export konfiguracji)

#### Funkcje Web Interface:
- **💾 Automatyczny zapis**: Wszystkie zmiany zapisywane do EEPROM
- **📊 Real-time Preview**: Podgląd adresów ramek w czasie rzeczywistym
- **🔄 Restart Integration**: Bezpieczny restart z web interface
- **📁 Export konfiguracji**: Pobieranie konfiguracji jako JSON
- **📱 Responsive Design**: Optymalizacja dla urządzeń mobilnych

#### Przykład konfiguracji BMS przez Web Interface:
```
Konfiguracja przed:
- Liczba baterii: 4
- CAN Speed: 125 kbps
- Node IDs: [1, 2, 3, 4]
- Frame 710 adresy: [0x701, 0x702, 0x703, 0x704]

Po zmianie przez web:
- Liczba baterii: 8
- CAN Speed: 500 kbps  
- Node IDs: [1, 3, 5, 7, 9, 11, 13, 15]
- Frame 710 adresy: [0x701, 0x703, 0x705, 0x707, 0x709, 0x70B, 0x70D, 0x70F]
- Modbus registers: 0-199, 200-399, 400-599, 600-799, 800-999, 1000-1199, 1200-1399, 1400-1599
```

#### Bezpieczeństwo Web Interface:
- **Czasowe ograniczenie**: Interface dostępny tylko przez 30 sekund
- **Lokalna sieć**: AP dostępny tylko lokalnie
- **Autoryzacja**: Hasło wymagane do połączenia z AP
- **Automatyczne wyłączenie**: AP wyłącza się po timeout

## 📈 Monitoring i Diagnostyka

### System Heartbeat
System wysyła co minutę raport zawierający:
- Status połączenia WiFi i Modbus
- Liczbę aktywnych modułów BMS
- Wykorzystanie pamięci
- Czas działania systemu
- Parametry aktywnych baterii

### Diagnostyka Rozszerzona (co 5 minut)
- Szczegółowe statystyki pamięci
- Statystyki sieci i CAN Bus
- Stan zdrowia systemu
- Analiza błędów komunikacji

### LED Status
- **Pojedyncze mrugnięcie**: System OK, brak komunikacji BMS
- **Wielokrotne mrugnięcia**: Liczba aktywnych modułów BMS
- **Szybkie mrugnięcia**: Błąd systemu lub restart

## 🔧 API Modbus TCP

### Funkcje Obsługiwane
- **0x03**: Read Holding Registers
- **0x06**: Write Single Register (ograniczone)
- **0x10**: Write Multiple Registers (ograniczone)

### Przykłady Użycia API Modbus TCP

#### 🐍 Python - Podstawowy Odczyt
```python
from pymodbus.client.sync import ModbusTcpClient

# Połączenie z ESP32S3
client = ModbusTcpClient('192.168.1.100', port=502)
client.connect()

# Odczyt podstawowych parametrów BMS 1 (Node ID 1)
base_addr = 0  # BMS 1 zaczyna od rejestru 0

# Napięcie baterii (rejestr 0)
result = client.read_holding_registers(base_addr + 0, 1, unit=1)
voltage = result.registers[0] / 1000.0  # mV → V
print(f"Napięcie baterii: {voltage:.2f} V")

# Prąd baterii (rejestr 1) - ze znakiem
result = client.read_holding_registers(base_addr + 1, 1, unit=1)
current = (result.registers[0] if result.registers[0] < 32768 
          else result.registers[0] - 65536) / 1000.0  # mA → A
print(f"Prąd baterii: {current:.2f} A")

# SOC (rejestr 3)
result = client.read_holding_registers(base_addr + 3, 1, unit=1)
soc = result.registers[0] / 100.0  # %×100 → %
print(f"SOC: {soc:.1f} %")

# SOH (rejestr 20)
result = client.read_holding_registers(base_addr + 20, 1, unit=1)
soh = result.registers[0] / 100.0  # %×100 → %
print(f"SOH: {soh:.1f} %")

client.close()
```

#### 🐍 Python - Odczyt Wszystkich BMS
```python
def read_all_bms(client, max_bms=16):
    """Odczyt danych ze wszystkich aktywnych modułów BMS"""
    bms_data = []
    
    for bms_id in range(1, max_bms + 1):
        base_addr = (bms_id - 1) * 200  # Każdy BMS ma 200 rejestrów
        
        # Sprawdź status komunikacji (rejestr 111)
        result = client.read_holding_registers(base_addr + 111, 1, unit=1)
        if not result.isError() and result.registers[0] == 1:
            
            # Odczyt kluczowych parametrów
            result = client.read_holding_registers(base_addr + 0, 4, unit=1)
            if not result.isError():
                voltage = result.registers[0] / 1000.0
                current = (result.registers[1] if result.registers[1] < 32768 
                          else result.registers[1] - 65536) / 1000.0
                energy = result.registers[2] / 100.0
                soc = result.registers[3] / 100.0
                
                bms_data.append({
                    'id': bms_id,
                    'voltage': voltage,
                    'current': current,
                    'energy': energy,
                    'soc': soc
                })
                
    return bms_data

# Użycie
client = ModbusTcpClient('192.168.1.100', port=502)
client.connect()

all_bms = read_all_bms(client)
for bms in all_bms:
    print(f"BMS {bms['id']}: {bms['voltage']:.2f}V, {bms['current']:.2f}A, SOC: {bms['soc']:.1f}%")

client.close()
```

#### 🔧 C# - Przykład dla .NET
```csharp
using System;
using EasyModbus;

class BMSReader
{
    static void Main()
    {
        ModbusClient client = new ModbusClient("192.168.1.100", 502);
        client.Connect();
        
        // Odczyt danych BMS 1
        int baseAddr = 0;  // BMS 1
        
        // Odczyt 4 rejestrów na raz (voltage, current, energy, soc)
        int[] registers = client.ReadHoldingRegisters(baseAddr, 4);
        
        float voltage = registers[0] / 1000.0f;  // mV → V
        float current = (registers[1] > 32767 ? registers[1] - 65536 : registers[1]) / 1000.0f;  // mA → A
        float energy = registers[2] / 100.0f;   // Wh×100 → kWh
        float soc = registers[3] / 100.0f;      // %×100 → %
        
        Console.WriteLine($"BMS 1: {voltage:F2}V, {current:F2}A, {energy:F2}kWh, SOC: {soc:F1}%");
        
        client.Disconnect();
    }
}
```

#### 🌐 JavaScript/Node.js - Przykład Web
```javascript
const ModbusRTU = require("modbus-serial");

async function readBMSData() {
    const client = new ModbusRTU();
    
    try {
        await client.connectTCP("192.168.1.100", { port: 502 });
        client.setID(1);
        
        // Odczyt podstawowych danych BMS 1
        const baseAddr = 0;
        const data = await client.readHoldingRegisters(baseAddr, 4);
        
        const voltage = data.data[0] / 1000.0;  // mV → V
        const current = (data.data[1] > 32767 ? data.data[1] - 65536 : data.data[1]) / 1000.0;  // mA → A  
        const energy = data.data[2] / 100.0;   // Wh×100 → kWh
        const soc = data.data[3] / 100.0;      // %×100 → %
        
        console.log(`BMS 1: ${voltage.toFixed(2)}V, ${current.toFixed(2)}A, SOC: ${soc.toFixed(1)}%`);
        
        // Odczyt temperatur (rejestry 22-24)
        const tempData = await client.readHoldingRegisters(baseAddr + 22, 3);
        const cellTemp = tempData.data[0] / 10.0;     // °C×10 → °C
        const minTemp = tempData.data[1];             // °C (int8 w LSB)
        const meanTemp = tempData.data[2] >> 8;       // °C (int8 w MSB)
        
        console.log(`Temperatury: Cell: ${cellTemp.toFixed(1)}°C, Min: ${minTemp}°C, Mean: ${meanTemp}°C`);
        
    } catch (error) {
        console.error("Błąd komunikacji:", error);
    } finally {
        client.close();
    }
}

readBMSData();
```

#### 📊 SCADA/HMI - Mapowanie Tagów
```
// Przykład konfiguracji tagów dla systemu SCADA

BMS1_Voltage:           Address=40001,  Type=UINT16,  Scale=0.001,  Unit=V
BMS1_Current:           Address=40002,  Type=INT16,   Scale=0.001,  Unit=A  
BMS1_Energy:            Address=40003,  Type=UINT16,  Scale=0.01,   Unit=kWh
BMS1_SOC:               Address=40004,  Type=UINT16,  Scale=0.01,   Unit=%
BMS1_SOH:               Address=40021,  Type=UINT16,  Scale=0.01,   Unit=%
BMS1_CellTemp:          Address=40023,  Type=INT16,   Scale=0.1,    Unit=°C
BMS1_ReadyToCharge:     Address=40046,  Type=BOOL,    Bit=0
BMS1_ReadyToDischarge:  Address=40046,  Type=BOOL,    Bit=1
BMS1_CommOK:            Address=40112,  Type=BOOL

// BMS 2 - dodaj 200 do każdego adresu  
BMS2_Voltage:           Address=40201,  Type=UINT16,  Scale=0.001,  Unit=V
BMS2_Current:           Address=40202,  Type=INT16,   Scale=0.001,  Unit=A
// ... itd
```

## 🛠️ Rozwiązywanie Problemów

### Częste Problemy

**1. Brak komunikacji CAN**
- Sprawdź połączenia hardware MCP2515
- Zweryfikuj terminatory 120Ω na magistrali CAN
- Sprawdź czy prędkość CAN (125kbps) jest zgodna z urządzeniami BMS

**2. Problemy WiFi**
- System automatycznie przełączy się w tryb AP po 3 nieudanych próbach
- Sprawdź SSID i hasło w `config.h`
- Monitor Serial pokaże szczegóły procesu łączenia

**3. Brak odpowiedzi Modbus**
- Sprawdź czy urządzenie jest dostępne na porcie 502
- Zweryfikuj adres IP ESP32S3
- Sprawdź mapę rejestrów dla poprawnych adresów

**4. CAN-Triggered AP nie uruchamia się**
- Sprawdź czy wysyłasz dokładnie 3 ramki w ciągu 1 sekundy
- Zweryfikuj CAN ID: `0xEF1` (3825 decimal)
- Sprawdź dane ramki: pierwsze 2 bajty muszą być `0xFF 0xBB`
- Upewnij się że prędkość CAN to 125 kbps
- Monitor Serial pokaże logi wyzwalaczy: `🎯 AP Trigger frame received`

**5. Tryb AP wyłącza się za szybko**
- Każda prawidłowa ramka `0xEF1` resetuje timer na 30 sekund
- Wyślij kolejne ramki wyzwalacza aby przedłużyć czas aktywności
- Monitor Serial pokaże: `⏰ AP mode timeout` gdy AP się wyłącza

### Debug przez Serial Monitor
```bash
pio device monitor --baud 115200 --filter esp32_exception_decoder
```

System wyświetla szczegółowe logi zawierające:
- Status inicjalizacji modułów
- Odebrane ramki CAN z dekodowaniem
- Żądania Modbus TCP
- Diagnostykę błędów
- **🔥 Logi CAN-triggered AP**:
  - `📡 AP Trigger system initialized` - inicjalizacja systemu
  - `🎯 AP Trigger frame received: 0xEF1 [FF BB]` - odebrano wyzwalacz
  - `   Trigger count: X/3` - postęp liczenia
  - `🚀 Starting triggered AP mode` - uruchomienie AP
  - `✅ Triggered AP mode started: ESP32S3-CAN-XXXXXX-TRIGGER` - AP aktywny
  - `⏰ AP mode timeout: XXXXX ms since last trigger` - timeout AP
  - `🛑 Stopping triggered AP mode` - zatrzymanie AP

## 🔄 Development Workflow

This project uses Universal Workflow patterns for professional development with systematic session management.

### 📋 Session Management:
- **Progress Tracking**: See [DEVELOPMENT_PROGRESS_LOG.md](DEVELOPMENT_PROGRESS_LOG.md) for detailed session history
- **Session Templates**: See [SESSION_TEMPLATES.md](SESSION_TEMPLATES.md) for startup/work/completion patterns
- **TodoWrite Patterns**: Systematic task management throughout development sessions
- **Token Management**: Strategies for managing context and continuity across sessions

### 🔧 Development Standards:
- **Professional Headers**: All modules include comprehensive documentation headers
- **Git Conventions**: All commits include Claude Code signature for traceability
- **Documentation First**: Documentation maintained alongside code changes
- **Testing Integration**: Hardware testing with actual ESP32S3 and CAN Bus systems

### 👩‍💻 For Developers:

#### Starting New Development Session:
1. **Environment Setup**: Navigate to project directory and check git status
2. **Progress Review**: Read last session notes from DEVELOPMENT_PROGRESS_LOG.md
3. **TodoWrite Setup**: Use appropriate pattern from SESSION_TEMPLATES.md
4. **Session Planning**: Define specific objectives and priorities

#### During Development:
- **Regular Updates**: Update todos every major milestone
- **Documentation**: Add/update headers and documentation with code changes
- **Commits**: Regular commits every 30-45 minutes for logical units
- **Progress Logging**: Document important decisions and findings

#### Session Completion:
- **Logical Closure**: Complete current work unit cleanly  
- **Progress Update**: Update DEVELOPMENT_PROGRESS_LOG.md with session summary
- **Final Commit**: Use Claude Code convention with descriptive message
- **Next Session**: Set specific priorities for continuation

### 🎯 ESP32S3/PlatformIO Specific:
- **Hardware Considerations**: Memory usage and real-time constraints documented
- **Module Templates**: Professional headers adapted for Arduino Framework
- **Compilation Testing**: Regular verification with `pio run`
- **Embedded Patterns**: Session templates optimized for embedded development

## 📝 Licencja

Ten projekt jest udostępniony na licencji MIT. Zobacz plik `LICENSE` dla szczegółów.

## 🤝 Wkład w Projekt

1. Fork repozytorium
2. Stwórz branch dla nowej funkcji (`git checkout -b feature/NazwaFunkcji`)
3. Commit zmian (`git commit -am 'Dodaj nową funkcję'`)
4. Push do branch (`git push origin feature/NazwaFunkcji`)
5. Stwórz Pull Request

## 📞 Kontakt

Projekt: ESP32S3 CAN to Modbus TCP Bridge
Wersja: 4.0.2
Data: 2025-08-17

---

### 🔄 Historia Zmian

**v4.0.2** (2025-08-17)
- ✅ Naprawiono wszystkie błędy kompilacji
- ✅ Usunięto duplikaty funkcji
- ✅ Dodano brakujące implementacje WiFiManager
- ✅ Zoptymalizowano wykorzystanie pamięci

**v4.0.1** (2025-08-13)
- ✅ Usunięto moduł can_handler (duplikat)
- ✅ Skonsolidowano funkcje CAN w bms_protocol
- ✅ Naprawiono wszystkie błędy konfiguracji

**v4.0.0** (2025-08-13)
- 🚀 Pierwszy stabilny release
- 📦 Pełna implementacja modułowa
- 🔌 Obsługa 16 modułów BMS
- 🌐 WiFi Manager z fallback do AP
- 📊 3200 rejestrów Modbus TCP