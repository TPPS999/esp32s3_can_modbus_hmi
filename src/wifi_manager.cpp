/*
 * wifi_manager.cpp - ESP32S3 CAN to Modbus TCP Bridge - WiFi Management Implementation
 * 
 * VERSION: v4.0.2 - NAPRAWIONY
 * DATE: 2025-08-13 09:25
 * STATUS: ✅ WSZYSTKIE BŁĘDY NAPRAWIONE
 * 
 * Implementacja kompletnego WiFi managera z:
 * - Connection management
 * - AP mode fallback
 * - Network scanning
 * - Callback system
 * - Statistics tracking
 */

#include "wifi_manager.h"
#include "utils.h"

// === GLOBAL WIFI MANAGER INSTANCE ===
WiFiManager wifiManager;

// === CONSTRUCTOR & DESTRUCTOR ===

WiFiManager::WiFiManager() 
  : currentState(WIFI_STATE_DISCONNECTED)
  , previousState(WIFI_STATE_DISCONNECTED)
  , stateChangeTime(0)
  , lastConnectionAttempt(0)
  , lastScanTime(0)
  , connectionAttempts(0)
  , enableAPFallback(true)
  , enableAutoReconnect(true)
  , stateChangeCallback(nullptr)
  , connectedCallback(nullptr)
  , disconnectedCallback(nullptr)
  , scanInProgress(false)
{
  // Initialize statistics
  memset(&statistics, 0, sizeof(statistics));
  
  // Use config values if available
  configSSID = String(WIFI_SSID);
  configPassword = String(WIFI_PASSWORD);
  
  // Clear scanned networks
  scannedNetworks.clear();
}

WiFiManager::~WiFiManager() {
  end();
}

// === LIFECYCLE MANAGEMENT ===

bool WiFiManager::begin() {
  return begin(configSSID.c_str(), configPassword.c_str());
}

bool WiFiManager::begin(const char* ssid, const char* password) {
  Serial.println("📡 Initializing WiFi Manager...");
  
  // Store credentials
  setCredentials(ssid, password);
  
  // Set WiFi mode
  WiFi.mode(WIFI_STA);
  
  // Set hostname
  String hostname = String(DEVICE_NAME) + "_" + String(ESP.getEfuseMac() & 0xFFFF, HEX);
  WiFi.setHostname(hostname.c_str());
  
  // Setup event handlers
  WiFi.onEvent([this](WiFiEvent_t event) {
    switch (event) {
      case ARDUINO_EVENT_WIFI_STA_CONNECTED:
        Serial.println("📡 WiFi station connected to AP");
        break;
        
      case ARDUINO_EVENT_WIFI_STA_GOT_IP:
        handleWiFiConnected();
        break;
        
      case ARDUINO_EVENT_WIFI_STA_DISCONNECTED:
        handleWiFiDisconnected();
        break;
        
      case ARDUINO_EVENT_WIFI_AP_START:
        Serial.println("📡 WiFi AP started");
        setState(WIFI_STATE_AP_MODE);
        break;
        
      case ARDUINO_EVENT_WIFI_AP_STOP:
        Serial.println("📡 WiFi AP stopped");
        break;
        
      default:
        break;
    }
  });
  
  // Set initial state
  setState(WIFI_STATE_DISCONNECTED);
  
  // Attempt initial connection
  if (connect()) {
    Serial.println("✅ WiFi Manager initialized and connected");
    return true;
  } else {
    Serial.println("⚠️ WiFi Manager initialized but not connected");
    
    // Try AP mode fallback if enabled
    if (enableAPFallback) {
      Serial.println("🔄 Starting AP mode fallback...");
      delay(WIFI_AP_FALLBACK_DELAY_MS);
      return startAPMode();
    }
    
    return false;
  }
}

void WiFiManager::end() {
  Serial.println("📡 Shutting down WiFi Manager...");
  
  stopAPMode();
  disconnect();
  WiFi.mode(WIFI_OFF);
  
  setState(WIFI_STATE_DISCONNECTED);
  
  Serial.println("✅ WiFi Manager shut down");
}

bool WiFiManager::restart() {
  Serial.println("🔄 Restarting WiFi Manager...");
  
  String ssid = configSSID;
  String password = configPassword;
  
  end();
  delay(1000);
  
  return begin(ssid.c_str(), password.c_str());
}

// === CONNECTION MANAGEMENT ===

bool WiFiManager::connect() {
  return connect(configSSID.c_str(), configPassword.c_str());
}

bool WiFiManager::connect(const char* ssid, const char* password) {
  if (!ssid || strlen(ssid) == 0) {
    Serial.println("❌ WiFi connect: Empty SSID");
    return false;
  }
  
  Serial.printf("📡 Connecting to WiFi: %s\n", ssid);
  
  // Store credentials
  setCredentials(ssid, password);
  
  // Stop AP mode if active
  if (isAPModeActive()) {
    stopAPMode();
  }
  
  // Set state and start connection
  setState(WIFI_STATE_CONNECTING);
  lastConnectionAttempt = millis();
  connectionAttempts++;
  
  // Perform connection
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  
  // Wait for connection with timeout
  unsigned long startTime = millis();
  while (WiFi.status() != WL_CONNECTED && 
         (millis() - startTime) < WIFI_CONNECT_TIMEOUT_MS) {
    delay(100);
    
    // Update LED during connection
    if ((millis() / 250) % 2 == 0) {
      setLED(true);
    } else {
      setLED(false);
    }
  }
  
  if (WiFi.status() == WL_CONNECTED) {
    // Connection successful - handleWiFiConnected() will be called by event
    return true;
  } else {
    // Connection failed
    Serial.printf("❌ WiFi connection failed after %lu ms\n", 
                  millis() - startTime);
    statistics.connectionFailures++;
    setState(WIFI_STATE_ERROR);
    setLED(false);
    return false;
  }
}

void WiFiManager::disconnect() {
  if (isConnected()) {
    Serial.println("📡 Disconnecting WiFi...");
    WiFi.disconnect(true);
    setState(WIFI_STATE_DISCONNECTED);
  }
}

bool WiFiManager::isConnected() const {
  return WiFi.status() == WL_CONNECTED && currentState == WIFI_STATE_CONNECTED;
}

bool WiFiManager::isConnecting() const {
  return currentState == WIFI_STATE_CONNECTING;
}

// === AP MODE MANAGEMENT ===

bool WiFiManager::startAPMode() {
  String apSSID = generateAPSSID();
  return startAPMode(apSSID.c_str(), AP_PASSWORD);
}

bool WiFiManager::startAPMode(const char* ssid, const char* password) {
  Serial.printf("📡 Starting AP mode: %s\n", ssid);
  
  // Stop station mode
  WiFi.mode(WIFI_AP);
  
  // Configure and start AP
  bool success = WiFi.softAP(ssid, password, AP_CHANNEL, AP_HIDDEN, AP_MAX_CONNECTIONS);
  
  if (success) {
    Serial.printf("✅ AP mode started successfully\n");
    Serial.printf("   SSID: %s\n", ssid);
    Serial.printf("   Password: %s\n", password);
    Serial.printf("   IP: %s\n", WiFi.softAPIP().toString().c_str());
    
    statistics.apModeActivations++;
    setState(WIFI_STATE_AP_MODE);
    
    return true;
  } else {
    Serial.println("❌ Failed to start AP mode");
    setState(WIFI_STATE_ERROR);
    return false;
  }
}

void WiFiManager::stopAPMode() {
  if (isAPModeActive()) {
    Serial.println("📡 Stopping AP mode...");
    WiFi.softAPdisconnect(true);
  }
}

bool WiFiManager::isAPModeActive() const {
  return WiFi.getMode() == WIFI_AP || WiFi.getMode() == WIFI_AP_STA;
}

// === STATE MANAGEMENT ===

void WiFiManager::setState(WiFiState_t newState) {
  if (newState != currentState) {
    previousState = currentState;
    currentState = newState;
    stateChangeTime = millis();
    
    handleStateChange(newState);
  }
}

void WiFiManager::handleStateChange(WiFiState_t newState) {
  Serial.printf("📡 WiFi state changed: %s → %s\n", 
                stateToString(previousState).c_str(), 
                stateToString(newState).c_str());
  
  // Call callback if set
  if (stateChangeCallback) {
    stateChangeCallback(previousState, newState);
  }
}

// === PROCESSING ===

void WiFiManager::process() {
  // Check connection status
  if (currentState == WIFI_STATE_CONNECTED && WiFi.status() != WL_CONNECTED) {
    handleConnectionLost();
  }
  
  // Handle automatic reconnection
  if (enableAutoReconnect && shouldAttemptReconnection()) {
    handleReconnection();
  }
  
  // Process network scanning
  if (scanInProgress) {
    processScan();
  }
  
  // Update state based on actual WiFi status
  updateState();
}

void WiFiManager::handleConnectionLost() {
  Serial.println("⚠️ WiFi connection lost");
  statistics.totalDisconnections++;
  setState(WIFI_STATE_DISCONNECTED);
  
  if (disconnectedCallback) {
    disconnectedCallback();
  }
}

void WiFiManager::handleReconnection() {
  if (millis() - lastConnectionAttempt >= WIFI_RECONNECT_INTERVAL_MS) {
    Serial.println("🔄 Attempting WiFi reconnection...");
    statistics.reconnectionAttempts++;
    
    if (connect()) {
      Serial.println("✅ WiFi reconnection successful");
    } else {
      Serial.println("❌ WiFi reconnection failed");
      
      // Try AP mode fallback if too many failures
      if (enableAPFallback && connectionAttempts >= MAX_CONNECTION_ATTEMPTS) {
        Serial.println("🔄 Starting AP mode fallback after reconnection failures");
        startAPMode();
      }
    }
  }
}

// === CONFIGURATION ===

void WiFiManager::setCredentials(const char* ssid, const char* password) {
  configSSID = String(ssid);
  configPassword = String(password ? password : "");
  
  Serial.printf("📡 WiFi credentials updated: SSID=%s\n", ssid);
}

void WiFiManager::setCallbacks(WiFiStateChangeCallback stateChange, 
                              WiFiConnectedCallback connected, 
                              WiFiDisconnectedCallback disconnected) {
  stateChangeCallback = stateChange;
  connectedCallback = connected;
  disconnectedCallback = disconnected;
  
  Serial.println("📡 WiFi callbacks configured");
}

// === NETWORK INFORMATION ===

String WiFiManager::getLocalIP() const {
  if (isConnected()) {
    return WiFi.localIP().toString();
  } else if (isAPModeActive()) {
    return WiFi.softAPIP().toString();
  }
  return "0.0.0.0";
}

String WiFiManager::getGatewayIP() const {
  if (isConnected()) {
    return WiFi.gatewayIP().toString();
  }
  return "0.0.0.0";
}

String WiFiManager::getSubnetMask() const {
  if (isConnected()) {
    return WiFi.subnetMask().toString();
  }
  return "0.0.0.0";
}

String WiFiManager::getDNS() const {
  if (isConnected()) {
    return WiFi.dnsIP().toString();
  }
  return "0.0.0.0";
}

String WiFiManager::getMACAddress() const {
  return WiFi.macAddress();
}

int WiFiManager::getRSSI() const {
  if (isConnected()) {
    return WiFi.RSSI();
  }
  return -100;  // Very weak signal as default
}

String WiFiManager::getSignalStrength() const {
  return rssiToQuality(getRSSI());
}

String WiFiManager::getSSID() const {
  if (isConnected()) {
    return WiFi.SSID();
  } else if (isAPModeActive()) {
    return WiFi.softAPSSID();
  }
  return "";
}

// === NETWORK SCANNING ===

bool WiFiManager::startScan() {
  if (scanInProgress) {
    Serial.println("⚠️ WiFi scan already in progress");
    return false;
  }
  
  Serial.println("🔍 Starting WiFi network scan...");
  
  clearScanResults();
  scanInProgress = true;
  lastScanTime = millis();
  
  int result = WiFi.scanNetworks(true);  // Async scan
  
  if (result == WIFI_SCAN_RUNNING) {
    return true;
  } else {
    scanInProgress = false;
    Serial.printf("❌ WiFi scan failed to start (error %d)\n", result);
    return false;
  }
}

bool WiFiManager::isScanComplete() const {
  return !scanInProgress;
}

int WiFiManager::getScanResults(WiFiNetwork* networks, int maxResults) {
  if (!networks || maxResults <= 0) {
    return 0;
  }
  
  int count = min(static_cast<int>(scannedNetworks.size()), maxResults);
  
  for (int i = 0; i < count; i++) {
    networks[i] = scannedNetworks[i];
  }
  
  return count;
}

void WiFiManager::clearScanResults() {
  scannedNetworks.clear();
}

// === DIAGNOSTICS ===

void WiFiManager::printStatus() const {
  Serial.println("📡 === WIFI MANAGER STATUS ===");
  Serial.printf("🔄 State: %s\n", stateToString(currentState).c_str());
  Serial.printf("⏰ State Duration: %lu ms\n", getStateTime());
  
  if (isConnected()) {
    Serial.printf("📶 SSID: %s\n", getSSID().c_str());
    Serial.printf("🌐 IP: %s\n", getLocalIP().c_str());
    Serial.printf("🚪 Gateway: %s\n", getGatewayIP().c_str());
    Serial.printf("📡 RSSI: %d dBm (%s)\n", getRSSI(), getSignalStrength().c_str());
    Serial.printf("🔧 MAC: %s\n", getMACAddress().c_str());
  } else if (isAPModeActive()) {
    Serial.printf("📡 AP SSID: %s\n", WiFi.softAPSSID().c_str());
    Serial.printf("🌐 AP IP: %s\n", WiFi.softAPIP().toString().c_str());
    Serial.printf("👥 Connected Clients: %d\n", WiFi.softAPgetStationNum());
  } else {
    Serial.println("❌ Not connected");
  }
  
  Serial.println("==============================");
}

void WiFiManager::printStatistics() const {
  Serial.println("📊 === WIFI STATISTICS ===");
  Serial.printf("🔗 Total Connections: %lu\n", statistics.totalConnections);
  Serial.printf("💔 Total Disconnections: %lu\n", statistics.totalDisconnections);
  Serial.printf("❌ Connection Failures: %lu\n", statistics.connectionFailures);
  Serial.printf("🔄 Reconnection Attempts: %lu\n", statistics.reconnectionAttempts);
  Serial.printf("📡 AP Mode Activations: %lu\n", statistics.apModeActivations);
  
  if (statistics.lastConnectedTime > 0) {
    Serial.printf("⏰ Last Connected: %lu ms ago\n", 
                  millis() - statistics.lastConnectedTime);
  }
  
  if (statistics.bestRSSI != 0) {
    Serial.printf("📶 Best RSSI: %d dBm\n", statistics.bestRSSI);
  }
  
  if (statistics.lastConnectedIP.length() > 0) {
    Serial.printf("🌐 Last IP: %s\n", statistics.lastConnectedIP.c_str());
  }
  
  Serial.println("==========================");
}

void WiFiManager::printNetworkInfo() const {
  if (!isConnected()) {
    Serial.println("❌ Not connected to network");
    return;
  }
  
  Serial.println("🌐 === NETWORK INFORMATION ===");
  Serial.printf("📶 SSID: %s\n", WiFi.SSID().c_str());
  Serial.printf("🔐 Security: %s\n", 
                WiFi.encryptionType(0) == WIFI_AUTH_OPEN ? "Open" : "Encrypted");
  Serial.printf("📡 RSSI: %d dBm (%s)\n", WiFi.RSSI(), getSignalStrength().c_str());
  Serial.printf("📻 Channel: %d\n", WiFi.channel());
  Serial.printf("🔧 MAC: %s\n", WiFi.macAddress().c_str());
  Serial.printf("🌐 IP: %s\n", WiFi.localIP().toString().c_str());
  Serial.printf("🎭 Subnet: %s\n", WiFi.subnetMask().toString().c_str());
  Serial.printf("🚪 Gateway: %s\n", WiFi.gatewayIP().toString().c_str());
  Serial.printf("🗂️ DNS: %s\n", WiFi.dnsIP().toString().c_str());
  Serial.println("===============================");
}

void WiFiManager::printScanResults() const {
  Serial.printf("🔍 === WIFI SCAN RESULTS (%d networks) ===\n", 
                static_cast<int>(scannedNetworks.size()));
  
  for (size_t i = 0; i < scannedNetworks.size(); i++) {
    const WiFiNetwork& network = scannedNetworks[i];
    Serial.printf("  %2d. %-20s %4d dBm  Ch:%2d  %s %s\n",
                  static_cast<int>(i + 1),
                  network.ssid.c_str(),
                  network.rssi,
                  network.channel,
                  network.isEncrypted ? "🔐" : "📖",
                  authModeToString(network.authMode).c_str());