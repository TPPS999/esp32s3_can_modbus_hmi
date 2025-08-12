/*
 * ESP32S3 CAN to Modbus TCP Bridge - Zarządzanie WiFi
 * 
 * VERSION: v4.0.0 - MODULAR ARCHITECTURE
 * DATE: 2025-08-12
 * STATUS: ✅ READY FOR COMPILATION
 * 
 * OPIS: Moduł odpowiedzialny za zarządzanie połączeniem WiFi i Access Point
 */

#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include "config.h"
#include <WiFi.h>
#include <WiFiAP.h>

// ================================
// === WIFI CONNECTION STATES ===
// ================================

/**
 * @brief Enum z stanami połączenia WiFi
 */
enum WiFiConnectionState {
    WIFI_STATE_DISCONNECTED = 0,
    WIFI_STATE_CONNECTING,
    WIFI_STATE_CONNECTED,
    WIFI_STATE_RECONNECTING,
    WIFI_STATE_AP_MODE,
    WIFI_STATE_ERROR
};

/**
 * @brief Struktura statystyk WiFi
 */
struct WiFiStats {
    uint32_t connectionAttempts;        // Liczba prób połączenia
    uint32_t successfulConnections;     // Udane połączenia
    uint32_t failedConnections;         // Nieudane połączenia
    uint32_t reconnections;             // Liczba reconnect
    uint32_t apModeActivations;         // Aktywacje trybu AP
    unsigned long totalConnectedTime;   // Całkowity czas połączenia [ms]
    unsigned long lastConnectTime;      // Czas ostatniego połączenia [ms]
    unsigned long lastDisconnectTime;   // Czas ostatniego rozłączenia [ms]
    int32_t minRSSI;                    // Minimalny RSSI
    int32_t maxRSSI;                    // Maksymalny RSSI
    int32_t avgRSSI;                    // Średni RSSI
    uint32_t rssiSamples;               // Liczba próbek RSSI
    
    WiFiStats() {
        connectionAttempts = 0;
        successfulConnections = 0;
        failedConnections = 0;
        reconnections = 0;
        apModeActivations = 0;
        totalConnectedTime = 0;
        lastConnectTime = 0;
        lastDisconnectTime = 0;
        minRSSI = 0;
        maxRSSI = -100;
        avgRSSI = 0;
        rssiSamples = 0;
    }
};

// ================================
// === WIFI MANAGER CLASS ===
// ================================

/**
 * @brief Klasa zarządzająca połączeniem WiFi i trybem Access Point
 */
class WiFiManager {
private:
    WiFiConnectionState currentState;
    WiFiStats stats;
    bool apModeEnabled;
    bool autoReconnect;
    unsigned long lastConnectionAttempt;
    unsigned long lastRSSICheck;
    unsigned long stateChangeTime;
    String currentSSID;
    String currentPassword;
    String apSSID;
    String apPassword;
    IPAddress staticIP;
    IPAddress gateway;
    IPAddress subnet;
    bool useStaticIP;
    
    // Private methods
    void attemptConnection();
    void handleDisconnection();
    void startAccessPoint();
    void stopAccessPoint();
    void updateRSSIStats();
    void logStateChange(WiFiConnectionState newState);
    
public:
    // Constructor
    WiFiManager();
    
    // Public interface
    bool initialize();
    void process();
    void disconnect();
    void reconnect();
    
    // Configuration
    void setCredentials(const String& ssid, const String& password);
    void setAPCredentials(const String& ssid, const String& password);
    void setStaticIP(IPAddress ip, IPAddress gateway, IPAddress subnet);
    void enableAutoReconnect(bool enable) { autoReconnect = enable; }
    void enableAPMode(bool enable) { apModeEnabled = enable; }
    
    // Status
    WiFiConnectionState getState() const { return currentState; }
    bool isConnected() const { return currentState == WIFI_STATE_CONNECTED; }
    bool isAPMode() const { return currentState == WIFI_STATE_AP_MODE; }
    String getStateString() const;
    IPAddress getLocalIP() const;
    IPAddress getAPIP() const;
    String getSSID() const;
    int32_t getRSSI() const;
    String getMacAddress() const;
    
    // Statistics
    const WiFiStats& getStats() const { return stats; }
    void printStatistics();
    void resetStatistics();
    
    // Network information
    void printNetworkInfo();
    void scanNetworks();
    bool isSSIDAvailable(const String& ssid);
    
    // Events (można rozszerzyć o callback functions)
    virtual void onConnected() {}
    virtual void onDisconnected() {}
    virtual void onAPStarted() {}
    virtual void onAPStopped() {}
};

// ================================
// === NETWORK UTILITIES ===
// ================================

/**
 * @brief Klasa z narzędziami sieciowymi
 */
class NetworkUtils {
public:
    /**
     * @brief Sprawdza czy podany IP jest prawidłowy
     * @param ip String z adresem IP
     * @return true jeśli IP jest prawidłowy
     */
    static bool isValidIP(const String& ip);
    
    /**
     * @brief Konwertuje string IP na IPAddress
     * @param ip String z adresem IP
     * @return Obiekt IPAddress
     */
    static IPAddress stringToIP(const String& ip);
    
    /**
     * @brief Pobiera jakość sygnału na podstawie RSSI
     * @param rssi Wartość RSSI [dBm]
     * @return Jakość sygnału (0-100%)
     */
    static uint8_t getSignalQuality(int32_t rssi);
    
    /**
     * @brief Pobiera opis jakości sygnału
     * @param rssi Wartość RSSI [dBm]
     * @return Opis jakości ("Excellent", "Good", "Fair", "Weak", "Poor")
     */
    static const char* getSignalQualityDescription(int32_t rssi);
    
    /**
     * @brief Sprawdza połączenie z internetem (ping do Google DNS)
     * @param timeoutMs Timeout w milisekundach
     * @return true jeśli połączenie z internetem działa
     */
    static bool checkInternetConnection(unsigned long timeoutMs = 5000);
    
    /**
     * @brief Pobiera informacje o routerze/bramie
     * @return String z informacjami o routerze
     */
    static String getRouterInfo();
    
    /**
     * @brief Formatuje MAC address
     * @param mac Array z 6 bajtami MAC
     * @return Sformatowany MAC address (XX:XX:XX:XX:XX:XX)
     */
    static String formatMacAddress(uint8_t* mac);
    
    /**
     * @brief Pobiera hostname ESP32
     * @return Hostname urządzenia
     */
    static String getHostname();
    
    /**
     * @brief Ustawia hostname ESP32
     * @param hostname Nowa nazwa hosta
     * @return true jeśli ustawienie się powiodło
     */
    static bool setHostname(const String& hostname);
};

// ================================
// === WIFI EVENT HANDLER ===
// ================================

/**
 * @brief Klasa obsługująca eventy WiFi
 */
class WiFiEventHandler {
private:
    static WiFiManager* wifiManager;
    static void onWiFiEvent(WiFiEvent_t event);
    
public:
    static void setWiFiManager(WiFiManager* manager);
    static void initialize();
    static void cleanup();
};

// ================================
// === CONSTANTS & MACROS ===
// ================================

// Signal quality thresholds (RSSI in dBm)
#define RSSI_EXCELLENT      -30
#define RSSI_GOOD          -50
#define RSSI_FAIR          -60
#define RSSI_WEAK          -70
#define RSSI_POOR          -80

// Connection timeouts
#define WIFI_CONNECTION_TIMEOUT     20000    // 20 seconds
#define WIFI_RECONNECT_INTERVAL     30000    // 30 seconds
#define WIFI_RSSI_CHECK_INTERVAL    10000    // 10 seconds
#define WIFI_AP_TIMEOUT             300000   // 5 minutes in AP mode before retry

// Default network settings
#define DEFAULT_HOSTNAME            "ESP32-CAN-Bridge"
#define DEFAULT_AP_CHANNEL          1
#define DEFAULT_AP_MAX_CONNECTIONS  4

/**
 * @brief Makro do łatwego sprawdzenia stanu WiFi
 */
#define WIFI_IS_READY()    (WiFi.status() == WL_CONNECTED)

/**
 * @brief Makro do pobierania jakości sygnału w procentach
 */
#define WIFI_SIGNAL_PERCENT(rssi)  NetworkUtils::getSignalQuality(rssi)

/**
 * @brief Struktura konfiguracji WiFi
 */
struct WiFiConfig {
    String ssid;
    String password;
    String apSSID;
    String apPassword;
    IPAddress staticIP;
    IPAddress gateway;
    IPAddress subnet;
    IPAddress dns1;
    IPAddress dns2;
    bool useStaticIP;
    bool enableAP;
    bool autoReconnect;
    String hostname;
    uint8_t apChannel;
    uint8_t apMaxConnections;
    bool apHidden;
    
    WiFiConfig() {
        ssid = WIFI_SSID;
        password = WIFI_PASSWORD;
        apSSID = AP_SSID;
        apPassword = AP_PASSWORD;
        staticIP = IPAddress(0, 0, 0, 0);
        gateway = IPAddress(0, 0, 0, 0);
        subnet = IPAddress(255, 255, 255, 0);
        dns1 = IPAddress(8, 8, 8, 8);        // Google DNS
        dns2 = IPAddress(8, 8, 4, 4);        // Google DNS Secondary
        useStaticIP = false;
        enableAP = true;
        autoReconnect = true;
        hostname = DEFAULT_HOSTNAME;
        apChannel = DEFAULT_AP_CHANNEL;
        apMaxConnections = DEFAULT_AP_MAX_CONNECTIONS;
        apHidden = AP_HIDDEN;
    }
};

// ================================
// === GLOBAL FUNCTIONS ===
// ================================

/**
 * @brief Pobiera globalny status WiFi jako string
 * @return Status WiFi
 */
String getWiFiStatusString();

/**
 * @brief Sprawdza czy ESP32 ma dostęp do internetu
 * @return true jeśli internet jest dostępny
 */
bool hasInternetAccess();

/**
 * @brief Pobiera informacje o sieci jako string
 * @return Informacje o aktualnej sieci
 */
String getNetworkInfoString();

#endif // WIFI_MANAGER_H