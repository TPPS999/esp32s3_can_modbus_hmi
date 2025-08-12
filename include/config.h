/*
 * ESP32S3 CAN to Modbus TCP Bridge - Konfiguracja Systemu
 * 
 * VERSION: v4.0.0 - MODULAR ARCHITECTURE
 * DATE: 2025-08-12
 * STATUS: ✅ READY FOR COMPILATION - Architektura Modularna
 * 
 * OPIS: Centralna konfiguracja dla wszystkich modułów systemu
 */

#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>

// ================================
// === VERSION & BUILD INFO ===
// ================================
#define SYSTEM_VERSION         "v4.0.0"
#define BUILD_DATE             "2025-08-12"
#define SYSTEM_NAME            "ESP32S3 CAN to Modbus TCP Bridge"
#define PROTOCOL_DESCRIPTION   "IFS BMS → Modbus TCP"
#define MAX_BMS_NODES          16

// ================================
// === HARDWARE CONFIGURATION ===
// ================================

// SPI pins dla MCP2515 CAN Controller
#define SPI_CS_PIN             44    // Chip Select
#define SPI_MOSI_PIN          9     // Master Out Slave In
#define SPI_MISO_PIN          8     // Master In Slave Out  
#define SPI_SCK_PIN           7     // Serial Clock
#define CAN_INT_PIN           2     // Interrupt pin (nieużywany)

// GPIO pins
#define LED_PIN               21    // LED diagnostyki
#define STATUS_LED_PIN        LED_PIN

// CAN Bus Configuration
#define CAN_SPEED             CAN_125KBPS  // 125 kbps - stała prędkość dla IFS BMS
#define CAN_FRAME_LENGTH      8            // Stała długość ramki CAN
#define CAN_TIMEOUT_MS        5000         // Timeout komunikacji CAN [ms]

// ================================
// === NETWORK CONFIGURATION ===
// ================================

// WiFi Settings
#define WIFI_SSID             "WNK3"
#define WIFI_PASSWORD         "PiotrStrzyklaskiNieIstnieje"
#define WIFI_CONNECT_TIMEOUT  10000        // Timeout połączenia WiFi [ms]
#define WIFI_RETRY_INTERVAL   30000        // Interval prób reconnect [ms]

// AP Settings (fallback mode)
#define AP_SSID               "ESP32-CAN-Bridge"
#define AP_PASSWORD           "CAN2Modbus123"
#define AP_CHANNEL            1
#define AP_HIDDEN             false

// ================================
// === MODBUS TCP CONFIGURATION ===
// ================================

// Modbus TCP Settings
#define MODBUS_TCP_PORT                   502
#define MODBUS_SLAVE_ID                   1
#define MODBUS_MAX_HOLDING_REGISTERS      2000  // 16 baterii * 125 rejestrów
#define MODBUS_MAX_CLIENTS                4     // Maksymalna liczba klientów TCP
#define MODBUS_TCP_TIMEOUT                30000 // Timeout połączenia TCP [ms]

// Modbus Function Codes
#define MODBUS_FUNC_READ_HOLDING_REGISTERS     0x03
#define MODBUS_FUNC_WRITE_SINGLE_REGISTER      0x06
#define MODBUS_FUNC_WRITE_MULTIPLE_REGISTERS   0x10

// Modbus Register Layout (na baterię - 125 rejestrów)
#define MODBUS_REGISTERS_PER_BATTERY      125
#define MODBUS_BASE_FRAME_190             0    // Rejestry 0-9: Frame 190 data
#define MODBUS_BASE_FRAME_190_FLAGS       10   // Rejestry 10-19: Frame 190 error flags
#define MODBUS_BASE_FRAME_290             20   // Rejestry 20-29: Frame 290 data
#define MODBUS_BASE_FRAME_310             30   // Rejestry 30-39: Frame 310 data
#define MODBUS_BASE_FRAME_390             40   // Rejestry 40-49: Frame 390 data
#define MODBUS_BASE_FRAME_410             50   // Rejestry 50-59: Frame 410 data
#define MODBUS_BASE_FRAME_510             60   // Rejestry 60-69: Frame 510 data
#define MODBUS_BASE_FRAME_490             70   // Rejestry 70-89: Frame 490 multiplexed data
#define MODBUS_BASE_ERROR_MAPS            90   // Rejestry 90-109: Error maps & versions
#define MODBUS_BASE_FRAME_710             110  // Rejestry 110-119: Frame 710 & communication
#define MODBUS_BASE_RESERVED              120  // Rejestry 120-124: Reserved

// ================================
// === CAN PROTOCOL CONFIGURATION ===
// ================================

// CAN Frame Types (Base IDs)
#define CAN_FRAME_190_BASE    0x180  // Podstawowe dane (0x181-0x190)
#define CAN_FRAME_290_BASE    0x280  // Napięcia ogniw (0x281-0x290)
#define CAN_FRAME_310_BASE    0x300  // SOH, temperatura, impedancja (0x301-0x310)
#define CAN_FRAME_390_BASE    0x380  // Maksymalne napięcia ogniw (0x381-0x390)
#define CAN_FRAME_410_BASE    0x400  // Temperatury, stany gotowości (0x401-0x410)
#define CAN_FRAME_510_BASE    0x500  // Limity mocy, stany I/O (0x501-0x510)
#define CAN_FRAME_490_BASE    0x480  // Dane multipleksowane (0x481-0x490)
#define CAN_FRAME_1B0_BASE    0x1A0  // Dane dodatkowe (0x1A1-0x1B0)
#define CAN_FRAME_710_BASE    0x700  // Stan CANopen (0x701-0x710)

// CAN Frame Frequencies (orientacyjne)
#define CAN_FREQ_HIGH         100    // Wysoka częstotliwość [ms] - Frame 190
#define CAN_FREQ_MEDIUM       500    // Średnia częstotliwość [ms] - Frames 290,310,390,410,510
#define CAN_FREQ_LOW          1000   // Niska częstotliwość [ms] - Frames 490,1B0,710

// ================================
// === SYSTEM TIMING CONFIGURATION ===
// ================================

// Task Intervals
#define HEARTBEAT_INTERVAL_MS         60000   // Heartbeat co 60 sekund
#define LED_HEARTBEAT_INTERVAL_MS     5000    // LED heartbeat co 5 sekund
#define COMMUNICATION_CHECK_MS        10000   // Sprawdzanie komunikacji co 10s
#define STATISTICS_UPDATE_MS          1000    // Aktualizacja statystyk co sekundę

// Timeouts
#define BMS_COMMUNICATION_TIMEOUT_MS  15000   // Timeout komunikacji z BMS [ms]
#define MODBUS_RESPONSE_TIMEOUT_MS    5000    // Timeout odpowiedzi Modbus [ms]
#define SYSTEM_WATCHDOG_MS            30000   // Watchdog systemowy [ms]

// ================================
// === BMS NODES CONFIGURATION ===
// ================================

// Domyślne Node ID dla modułów BMS (1-16)
static const uint8_t DEFAULT_BMS_NODE_IDS[MAX_BMS_NODES] = {
    1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16
};

// Aktywna liczba modułów BMS (można zmniejszyć jeśli nie wszystkie są używane)
#define ACTIVE_BMS_NODES              16

// ================================
// === DIAGNOSTICS & DEBUG ===
// ================================

// Debug levels
#define DEBUG_LEVEL_NONE              0
#define DEBUG_LEVEL_ERROR             1
#define DEBUG_LEVEL_WARNING           2
#define DEBUG_LEVEL_INFO              3
#define DEBUG_LEVEL_VERBOSE           4

#define DEBUG_LEVEL                   DEBUG_LEVEL_INFO  // Zmień dla różnych poziomów debug

// Serial Configuration
#define SERIAL_BAUDRATE               115200
#define SERIAL_INIT_TIMEOUT_MS        5000

// Debug output kontrola
#define DEBUG_CAN_FRAMES              true    // Wyświetlaj ramki CAN
#define DEBUG_MODBUS_REQUESTS         true    // Wyświetlaj zapytania Modbus
#define DEBUG_BMS_PARSING             true    // Wyświetlaj parsowanie BMS
#define DEBUG_STATISTICS              true    // Wyświetlaj statystyki
#define DEBUG_HEARTBEAT_EXTENDED      true    // Rozszerzony heartbeat

// ================================
// === MEMORY CONFIGURATION ===
// ================================

// Buffer sizes
#define CAN_RECEIVE_BUFFER_SIZE       64     // Bufor odbioru CAN
#define MODBUS_TCP_BUFFER_SIZE        256    // Bufor TCP Modbus
#define SERIAL_OUTPUT_BUFFER_SIZE     512    // Bufor wyjścia Serial

// Stack sizes (dla przyszłych tasków RTOS)
#define TASK_CAN_STACK_SIZE           4096
#define TASK_MODBUS_STACK_SIZE        4096
#define TASK_HEARTBEAT_STACK_SIZE     2048

// ================================
// === MULTIPLEXER CONFIGURATION ===
// ================================

// Frame 490 Multiplexer Types (najważniejsze)
#define MUX490_SERIAL_NUMBER_0        0x00
#define MUX490_SERIAL_NUMBER_1        0x01
#define MUX490_HW_VERSION_0           0x02
#define MUX490_HW_VERSION_1           0x03
#define MUX490_SW_VERSION_0           0x04
#define MUX490_SW_VERSION_1           0x05
#define MUX490_FACTORY_ENERGY         0x06
#define MUX490_DESIGN_CAPACITY        0x07
#define MUX490_INLET_TEMPERATURE      0x0F
#define MUX490_HUMIDITY               0x10
#define MUX490_TIME_TO_FULL_CHARGE    0x17
#define MUX490_TIME_TO_FULL_DISCHARGE 0x18
#define MUX490_BATTERY_CYCLES         0x1A

#define MUX490_MAX_TYPE               0x35   // Maksymalny typ multipleksera

// ================================
// === ERROR HANDLING ===
// ================================

// Error codes
#define ERROR_NONE                    0
#define ERROR_CAN_INIT_FAILED         1
#define ERROR_WIFI_CONNECTION_FAILED  2
#define ERROR_MODBUS_SERVER_FAILED    3
#define ERROR_MEMORY_ALLOCATION       4
#define ERROR_BMS_COMMUNICATION       5
#define ERROR_INVALID_CONFIGURATION   6

// Error severity levels
#define ERROR_SEVERITY_INFO           0
#define ERROR_SEVERITY_WARNING        1
#define ERROR_SEVERITY_ERROR          2
#define ERROR_SEVERITY_CRITICAL       3

// ================================
// === SAFETY & LIMITS ===
// ================================

// Safety limits dla danych BMS
#define BMS_MAX_VOLTAGE               60.0f   // Maksymalne napięcie baterii [V]
#define BMS_MAX_CURRENT               200.0f  // Maksymalny prąd [A]
#define BMS_MAX_TEMPERATURE           60.0f   // Maksymalna temperatura [°C]
#define BMS_MIN_VOLTAGE               36.0f   // Minimalne napięcie baterii [V]
#define BMS_MAX_SOC                   100.0f  // Maksymalny SOC [%]
#define BMS_MAX_SOH                   100.0f  // Maksymalny SOH [%]

// Data validation limits
#define MAX_PARSE_ERRORS_PER_HOUR     100     // Maksymalne błędy parsowania na godzinę
#define MAX_COMMUNICATION_GAPS        5       // Maksymalne przerwy w komunikacji

// ================================
// === FEATURE FLAGS ===
// ================================

// Enable/disable features
#define FEATURE_WEB_SERVER            true    // Włącz serwer WWW (przyszłość)
#define FEATURE_OTA_UPDATES           true    // Włącz OTA updates (przyszłość)
#define FEATURE_HISTORICAL_DATA       false   // Włącz historyczne dane (przyszłość)
#define FEATURE_SNMP_MONITORING       false   // Włącz SNMP (przyszłość)
#define FEATURE_MODBUS_WRITE          false   // Włącz zapisy Modbus (przyszłość)
#define FEATURE_CAN_FILTERING         true    // Włącz filtrowanie CAN
#define FEATURE_ADVANCED_DIAGNOSTICS  true    // Włącz zaawansowaną diagnostykę

// ================================
// === COMPILER MACROS ===
// ================================

// Helper macros
#define ARRAY_SIZE(x)                 (sizeof(x) / sizeof(x[0]))
#define STRINGIFY(x)                  #x
#define TOSTRING(x)                   STRINGIFY(x)

// Min/Max macros (jeśli nie są zdefiniowane)
#ifndef MIN
#define MIN(a, b)                     ((a) < (b) ? (a) : (b))
#endif

#ifndef MAX
#define MAX(a, b)                     ((a) > (b) ? (a) : (b))
#endif

// Bit manipulation macros
#define SET_BIT(x, bit)               ((x) |= (1 << (bit)))
#define CLEAR_BIT(x, bit)             ((x) &= ~(1 << (bit)))
#define TOGGLE_BIT(x, bit)            ((x) ^= (1 << (bit)))
#define CHECK_BIT(x, bit)             ((x) & (1 << (bit)))

// ================================
// === VALIDATION MACROS ===
// ================================

// Sprawdzenie poprawności Node ID
#define IS_VALID_BMS_NODE_ID(id)      ((id) >= 1 && (id) <= MAX_BMS_NODES)

// Sprawdzenie poprawności CAN ID dla ramek BMS
#define IS_VALID_BMS_CAN_ID(id)       (((id) >= 0x181 && (id) <= 0x190) || \
                                       ((id) >= 0x281 && (id) <= 0x290) || \
                                       ((id) >= 0x301 && (id) <= 0x310) || \
                                       ((id) >= 0x381 && (id) <= 0x390) || \
                                       ((id) >= 0x401 && (id) <= 0x410) || \
                                       ((id) >= 0x501 && (id) <= 0x510) || \
                                       ((id) >= 0x481 && (id) <= 0x490) || \
                                       ((id) >= 0x1A1 && (id) <= 0x1B0) || \
                                       ((id) >= 0x701 && (id) <= 0x710))

// Sprawdzenie poprawności adresu Modbus
#define IS_VALID_MODBUS_ADDRESS(addr) ((addr) < MODBUS_MAX_HOLDING_REGISTERS)

#endif // CONFIG_H