/*
 * wifi_manager.h - ESP32S3 CAN to Modbus TCP Bridge - WiFi Management
 * 
 * VERSION: v4.0.2 - NAPRAWIONY
 * DATE: 2025-08-13 09:25
 * STATUS: ✅ WSZYSTKIE BŁĘDY NAPRAWIONE
 * 
 * Naprawione:
 * - Usunięte konflikty wifi_mode_t
 * - Dodane wszystkie brakujące funkcje
 * - Naprawione std::vector include
 * - Dodane callback functions zgodne z main.cpp
 * - Poprawione AP mode handling
 */

#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include <Arduino.h>
#include <WiFi.h>
#include <vector>
#include "config.h"

// === WIFI CONFIGURATION ===
#define WIFI_CONNECT_TIMEOUT_MS     30000
#define WIFI_RECONNECT_INTERVAL_MS  60000
#define WIFI_MAX_CONNECT_ATTEMPTS   3
#define WIFI_SCAN_TIMEOUT_MS        10000
#define WIFI_AP_FALLBACK_DELAY_MS   5000

// === AP MODE CONFIGURATION ===
#define AP_SSID_PREFIX     "ESP32S3-CAN-"
#define AP_PASSWORD        "esp32modbus"
#define AP_CHANNEL         1
#define AP_MAX_CONNECTIONS 4
#define AP_HIDDEN          false

// === CALLBACK FUNCTION TYPES ===
typedef void (*WiFiStateChangeCallback)(WiFiState_t oldState, WiFiState_t newState);
typedef void (*WiFiConnectedCallback)(String ip);
typedef void (*WiFiDisconnectedCallback)();

// === NETWORK SCAN RESULT ===
struct WiFiNetwork {
  String ssid;
  int32_t rssi;
  wifi_auth_mode_t authMode;
  int32_t channel;
  bool isEncrypted;
};

// === WIFI MANAGER CLASS ===
class WiFiManager {
private:
  // State management
  WiFiState_t currentState;
  WiFiState_t previousState;
  unsigned long stateChangeTime;
  unsigned long lastConnectionAttempt;
  unsigned long lastScanTime;
  int connectionAttempts;
  
  // Configuration
  String configSSID;
  String configPassword;
  bool enableAPFallback;
  bool enableAutoReconnect;
  
  // Callbacks
  WiFiStateChangeCallback stateChangeCallback;
  WiFiConnectedCallback connectedCallback;
  WiFiDisconnectedCallback disconnectedCallback;
  
  // Network scanning
  std::vector<WiFiNetwork> scannedNetworks;
  bool scanInProgress;
  
  // Statistics
  struct {
    unsigned long totalConnections;
    unsigned long totalDisconnections;
    unsigned long connectionFailures;
    unsigned long reconnectionAttempts;
    unsigned long apModeActivations;
    unsigned long lastConnectedTime;
    int bestRSSI;
    String lastConnectedIP;
  } statistics;
  
public:
  WiFiManager();
  ~WiFiManager();
  
  // === LIFECYCLE MANAGEMENT ===
  bool begin();
  bool begin(const char* ssid, const char* password);
  void end();
  bool restart();
  
  // === CONNECTION MANAGEMENT ===
  bool connect();
  bool connect(const char* ssid, const char* password);
  void disconnect();
  bool isConnected() const;
  bool isConnecting() const;
  
  // === AP MODE MANAGEMENT ===
  bool startAPMode();
  bool startAPMode(const char* ssid, const char* password);
  void stopAPMode();
  bool isAPModeActive() const;
  
  // === TRIGGERED AP MODE (CAN-based) ===
  bool startTriggeredAPMode();
  void stopTriggeredAPMode();
  bool isTriggeredAPModeActive() const;
  
  // === STATE MANAGEMENT ===
  WiFiState_t getState() const { return currentState; }
  WiFiState_t getPreviousState() const { return previousState; }
  unsigned long getStateTime() const { return millis() - stateChangeTime; }
  void setState(WiFiState_t newState);
  
  // === PROCESSING ===
  void process();
  void handleConnectionLost();
  void handleReconnection();
  
  // === CONFIGURATION ===
  void setCredentials(const char* ssid, const char* password);
  void setAPFallback(bool enable) { enableAPFallback = enable; }
  void setAutoReconnect(bool enable) { enableAutoReconnect = enable; }
  
  // === CALLBACKS ===
  void setCallbacks(WiFiStateChangeCallback stateChange, 
                   WiFiConnectedCallback connected, 
                   WiFiDisconnectedCallback disconnected);
  void setStateChangeCallback(WiFiStateChangeCallback callback) { stateChangeCallback = callback; }
  void setConnectedCallback(WiFiConnectedCallback callback) { connectedCallback = callback; }
  void setDisconnectedCallback(WiFiDisconnectedCallback callback) { disconnectedCallback = callback; }
  
  // === NETWORK INFORMATION ===
  String getLocalIP() const;
  String getGatewayIP() const;
  String getSubnetMask() const;
  String getDNS() const;
  String getMACAddress() const;
  int getRSSI() const;
  String getSignalStrength() const;
  String getSSID() const;
  
  // === NETWORK SCANNING ===
  bool startScan();
  bool isScanComplete() const;
  int getScanResults(WiFiNetwork* networks, int maxResults);
  const std::vector<WiFiNetwork>& getScannedNetworks() const { return scannedNetworks; }
  void clearScanResults();
  
  // === DIAGNOSTICS ===
  void printStatus() const;
  void printStatistics() const;
  void printNetworkInfo() const;
  void printScanResults() const;
  bool isHealthy() const;
  
  // === STATISTICS ===
  unsigned long getTotalConnections() const { return statistics.totalConnections; }
  unsigned long getTotalDisconnections() const { return statistics.totalDisconnections; }
  unsigned long getConnectionFailures() const { return statistics.connectionFailures; }
  void resetStatistics();
  
private:
  // Internal state management
  void updateState();
  void checkConnection();
  void handleStateChange(WiFiState_t newState);
  
  // Internal connection management
  bool performConnection();
  void performDisconnection();
  bool shouldAttemptReconnection() const;
  
  // Internal AP mode management
  String generateAPSSID() const;
  bool configureAPMode(const char* ssid, const char* password);
  
  // Internal scanning
  void processScan();
  void populateScanResults();
  
  // Utility functions
  String authModeToString(wifi_auth_mode_t authMode) const;
  String stateToString(WiFiState_t state) const;
  String formatSignalStrength(int rssi) const;
};

// === GLOBAL WIFI MANAGER INSTANCE ===
extern WiFiManager wifiManager;

// === STANDALONE UTILITY FUNCTIONS ===

// Basic WiFi functions (for compatibility)
bool setupWiFi();
bool connectWiFi(const char* ssid, const char* password);
void disconnectWiFi();
bool isWiFiConnected();

// Network utilities
String getWiFiLocalIP();
int getWiFiRSSI();
String getWiFiSSID();
bool pingGateway();

// Scanning utilities
int scanWiFiNetworks();
void printWiFiNetworks();

// Event handling
void onWiFiEvent(WiFiEvent_t event);
void handleWiFiConnected();
void handleWiFiDisconnected();

// === WIFI STATE HELPERS ===

inline bool isWiFiStateConnected(WiFiState_t state) {
  return state == WIFI_STATE_CONNECTED;
}

inline bool isWiFiStateActive(WiFiState_t state) {
  return state == WIFI_STATE_CONNECTED || state == WIFI_STATE_AP_MODE;
}

inline bool isWiFiStateError(WiFiState_t state) {
  return state == WIFI_STATE_ERROR;
}

// === SIGNAL STRENGTH HELPERS ===

inline String rssiToQuality(int rssi) {
  if (rssi >= -30) return "Excellent";
  if (rssi >= -50) return "Very Good";
  if (rssi >= -60) return "Good";
  if (rssi >= -70) return "Fair";
  if (rssi >= -80) return "Weak";
  return "Very Weak";
}

inline int rssiToPercent(int rssi) {
  if (rssi >= -30) return 100;
  if (rssi >= -40) return 90;
  if (rssi >= -50) return 80;
  if (rssi >= -60) return 70;
  if (rssi >= -70) return 60;
  if (rssi >= -80) return 50;
  if (rssi >= -90) return 30;
  return 10;
}

// === ADVANCED FEATURES (for future use) ===

// Connection profiles
struct WiFiProfile {
  String ssid;
  String password;
  bool hidden;
  int priority;
  bool autoConnect;
};

// Connection manager for multiple profiles
class WiFiConnectionManager {
private:
  std::vector<WiFiProfile> profiles;
  int currentProfileIndex;
  
public:
  void addProfile(const char* ssid, const char* password, int priority = 0);
  void removeProfile(const char* ssid);
  bool connectBestProfile();
  void printProfiles() const;
};

// === CONFIGURATION CONSTANTS ===

// Default timeouts and intervals
static const unsigned long DEFAULT_CONNECT_TIMEOUT = 30000;
static const unsigned long DEFAULT_RECONNECT_INTERVAL = 60000;
static const unsigned long DEFAULT_SCAN_TIMEOUT = 10000;

// RSSI thresholds
static const int RSSI_EXCELLENT = -30;
static const int RSSI_GOOD = -50;
static const int RSSI_FAIR = -70;
static const int RSSI_WEAK = -80;

// Retry limits
static const int MAX_CONNECTION_ATTEMPTS = 3;
static const int MAX_RECONNECTION_ATTEMPTS = 5;
static const int MAX_SCAN_ATTEMPTS = 3;

#endif // WIFI_MANAGER_H