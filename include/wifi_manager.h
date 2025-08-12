/*
 * wifi_manager.h - ESP32S3 CAN to Modbus TCP Bridge WiFi Manager
 * 
 * VERSION: v4.0.0
 * DATE: 2025-08-12
 * DESCRIPTION: Zarządzanie połączeniem WiFi dla ESP32S3
 */

#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include "config.h"
#include <WiFi.h>
#include <WiFiAP.h>
#include <DNSServer.h>

// === WIFI MANAGER CONSTANTS ===
#define WIFI_CONNECTION_TIMEOUT_MS 30000
#define WIFI_RECONNECT_INTERVAL_MS 60000
#define WIFI_AP_TIMEOUT_MS 300000        // 5 minutes
#define WIFI_MAX_RECONNECT_ATTEMPTS 5
#define WIFI_RSSI_UPDATE_INTERVAL_MS 5000
#define WIFI_STATUS_CHECK_INTERVAL_MS 1000

// === WIFI ACCESS POINT SETTINGS ===
#define WIFI_AP_SSID "ESP32S3-CAN-Bridge"
#define WIFI_AP_PASSWORD "modbus123"
#define WIFI_AP_CHANNEL 6
#define WIFI_AP_MAX_CONNECTIONS 4
#define WIFI_AP_IP_ADDRESS "192.168.4.1"
#define WIFI_AP_GATEWAY "192.168.4.1"
#define WIFI_AP_SUBNET "255.255.255.0"

// === DNS SERVER FOR CAPTIVE PORTAL ===
#define DNS_PORT 53
#define CAPTIVE_PORTAL_TIMEOUT_MS 180000  // 3 minutes

// === WIFI STATES ===
typedef enum {
  WIFI_STATE_UNINITIALIZED = 0,
  WIFI_STATE_INITIALIZING,
  WIFI_STATE_SCANNING,
  WIFI_STATE_CONNECTING,
  WIFI_STATE_CONNECTED,
  WIFI_STATE_DISCONNECTED,
  WIFI_STATE_RECONNECTING,
  WIFI_STATE_AP_MODE,
  WIFI_STATE_AP_CLIENT_CONNECTED,
  WIFI_STATE_ERROR,
  WIFI_STATE_DISABLED
} WiFiState_t;

// === WIFI CONNECTION MODES ===
typedef enum {
  WIFI_MODE_STATION = 0,
  WIFI_MODE_ACCESS_POINT,
  WIFI_MODE_STATION_AP,
  WIFI_MODE_DISABLED
} WiFiMode_t;

// === WIFI SECURITY TYPES ===
typedef enum {
  WIFI_SECURITY_OPEN = 0,
  WIFI_SECURITY_WEP,
  WIFI_SECURITY_WPA,
  WIFI_SECURITY_WPA2,
  WIFI_SECURITY_WPA3,
  WIFI_SECURITY_UNKNOWN
} WiFiSecurity_t;

// === WIFI NETWORK SCAN RESULT ===
struct WiFiNetwork {
  String ssid;
  int32_t rssi;
  uint8_t channel;
  WiFiSecurity_t security;
  bool isHidden;
  String bssid;
};

// === WIFI STATISTICS ===
struct WiFiStats {
  unsigned long connectionAttempts;
  unsigned long successfulConnections;
  unsigned long disconnections;
  unsigned long reconnectAttempts;
  unsigned long apModeActivations;
  unsigned long scanAttempts;
  unsigned long lastConnectionTime;
  unsigned long lastDisconnectionTime;
  unsigned long totalConnectedTime;
  unsigned long totalDisconnectedTime;
  int32_t currentRSSI;
  int32_t minRSSI;
  int32_t maxRSSI;
  int32_t avgRSSI;
  uint8_t currentChannel;
  String currentBSSID;
  WiFiSecurity_t currentSecurity;
  int rssiSamples;
  long rssiSum;
};

// === WIFI MANAGER CLASS ===
class WiFiManager {
private:
  // State management
  WiFiState_t currentState;
  WiFiMode_t currentMode;
  WiFiStats stats;
  
  // Connection management
  String targetSSID;
  String targetPassword;
  unsigned long lastConnectionAttempt;
  unsigned long lastStatusCheck;
  unsigned long lastRSSIUpdate;
  unsigned long apModeStartTime;
  int reconnectAttempts;
  bool autoReconnectEnabled;
  bool apFallbackEnabled;
  
  // Access Point mode
  DNSServer* dnsServer;
  bool apModeActive;
  unsigned long apClientConnectedTime;
  
  // Network scanning
  std::vector<WiFiNetwork> scannedNetworks;
  unsigned long lastScanTime;
  bool scanInProgress;
  
  // Private methods
  void attemptConnection();
  void handleDisconnection();
  void startAccessPoint();
  void stopAccessPoint();
  void updateRSSIStats();
  void updateConnectionStats();
  void setupStaticIP();
  bool isValidCredentials();
  void handleAPClient();
  void processDNSRequests();
  
  // Callback functions
  void onWiFiEvent(WiFiEvent_t event);
  static void staticWiFiEventHandler(WiFiEvent_t event);
  
  // Exponential backoff for reconnection
  unsigned long getReconnectDelay();
  
public:
  // Constructor/Destructor
  WiFiManager();
  ~WiFiManager();
  
  // Main functions
  bool initialize();
  void process();
  void reset();
  
  // Connection management
  bool connect(const String& ssid, const String& password);
  bool connect(); // Use stored credentials
  void disconnect();
  bool reconnect();
  
  // Credential management
  void setCredentials(const String& ssid, const String& password);
  String getSSID() const;
  bool hasCredentials() const;
  
  // Network scanning
  int scanNetworks();
  int getNetworkCount() const;
  WiFiNetwork getNetwork(int index) const;
  String getNetworkSSID(int index) const;
  int32_t getNetworkRSSI(int index) const;
  WiFiSecurity_t getNetworkSecurity(int index) const;
  
  // Access Point mode
  bool startAPMode();
  bool stopAPMode();
  bool isAPModeActive() const;
  int getAPClientCount() const;
  
  // Status and diagnostics
  WiFiState_t getState() const;
  WiFiMode_t getMode() const;
  bool isConnected() const;
  bool isConnecting() const;
  String getLocalIP() const;
  String getGatewayIP() const;
  String getSubnetMask() const;
  String getDNS() const;
  String getMACAddress() const;
  int32_t getRSSI() const;
  uint8_t getChannel() const;
  String getBSSID() const;
  
  // Configuration
  void enableAutoReconnect(bool enabled = true);
  void enableAPFallback(bool enabled = true);
  void setReconnectInterval(unsigned long intervalMs);
  void setConnectionTimeout(unsigned long timeoutMs);
  
  // Statistics and monitoring
  const WiFiStats& getStatistics() const;
  void resetStatistics();
  void printStatus() const;
  void printStatistics() const;
  void printNetworkInfo() const;
  
  // Quality metrics
  int getSignalQuality() const;      // 0-100%
  String getSignalStrength() const;  // "Excellent", "Good", etc.
  bool isSignalStable() const;
  
  // Advanced features
  bool setStaticIP(const String& ip, const String& gateway, const String& subnet, const String& dns1 = "", const String& dns2 = "");
  bool enableDHCP();
  bool isStaticIP() const;
  
  // Power management
  bool enablePowerSave(bool enabled = true);
  bool setPowerMode(wifi_power_t power);
  wifi_power_t getPowerMode() const;
  
  // Error handling
  String getLastError() const;
  bool hasErrors() const;
  void clearErrors();
};

// === GLOBAL INSTANCES ===
extern WiFiManager wifiManager;

// === HELPER FUNCTIONS ===
String wifiStateToString(WiFiState_t state);
String wifiModeToString(WiFiMode_t mode);
String wifiSecurityToString(WiFiSecurity_t security);
String wifiEventToString(WiFiEvent_t event);
WiFiSecurity_t getSecurityType(wifi_auth_mode_t authMode);
int rssiToPercentage(int32_t rssi);
String rssiToQuality(int32_t rssi);

// === WIFI EVENT CALLBACKS ===
typedef std::function<void(WiFiState_t oldState, WiFiState_t newState)> WiFiStateChangeCallback;
typedef std::function<void(int32_t rssi)> WiFiRSSIChangeCallback;
typedef std::function<void(String ip)> WiFiConnectedCallback;
typedef std::function<void()> WiFiDisconnectedCallback;
typedef std::function<void(int clientCount)> WiFiAPClientCallback;

// === CALLBACK REGISTRATION ===
void setWiFiStateChangeCallback(WiFiStateChangeCallback callback);
void setWiFiRSSIChangeCallback(WiFiRSSIChangeCallback callback);
void setWiFiConnectedCallback(WiFiConnectedCallback callback);
void setWiFiDisconnectedCallback(WiFiDisconnectedCallback callback);
void setWiFiAPClientCallback(WiFiAPClientCallback callback);

// === UTILITY MACROS ===
#define WIFI_IS_CONNECTED() (WiFi.status() == WL_CONNECTED)
#define WIFI_HAS_IP() (WiFi.localIP() != IPAddress(0, 0, 0, 0))
#define WIFI_SIGNAL_STRENGTH() rssiToPercentage(WiFi.RSSI())

#endif // WIFI_MANAGER_H