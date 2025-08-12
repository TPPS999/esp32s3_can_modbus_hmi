/*
 * wifi_manager.cpp - ESP32S3 CAN to Modbus TCP Bridge WiFi Manager Implementation
 * 
 * VERSION: v4.0.0
 * DATE: 2025-08-12
 */

#include "wifi_manager.h"
#include "utils.h"

// === GLOBAL INSTANCES ===
WiFiManager wifiManager;

// === STATIC CALLBACK POINTERS ===
static WiFiStateChangeCallback stateChangeCallback = nullptr;
static WiFiRSSIChangeCallback rssiChangeCallback = nullptr;
static WiFiConnectedCallback connectedCallback = nullptr;
static WiFiDisconnectedCallback disconnectedCallback = nullptr;
static WiFiAPClientCallback apClientCallback = nullptr;

// === STATIC VARIABLES ===
static WiFiManager* staticInstance = nullptr;

// === CONSTRUCTOR/DESTRUCTOR ===

WiFiManager::WiFiManager() {
  currentState = WIFI_STATE_UNINITIALIZED;
  currentMode = WIFI_MODE_STATION;
  memset(&stats, 0, sizeof(WiFiStats));
  
  lastConnectionAttempt = 0;
  lastStatusCheck = 0;
  lastRSSIUpdate = 0;
  apModeStartTime = 0;
  reconnectAttempts = 0;
  autoReconnectEnabled = true;
  apFallbackEnabled = true;
  
  dnsServer = nullptr;
  apModeActive = false;
  apClientConnectedTime = 0;
  
  lastScanTime = 0;
  scanInProgress = false;
  
  stats.minRSSI = 0;
  stats.maxRSSI = -100;
  stats.currentRSSI = -100;
  
  staticInstance = this;
}

WiFiManager::~WiFiManager() {
  if (dnsServer) {
    delete dnsServer;
    dnsServer = nullptr;
  }
  staticInstance = nullptr;
}

// === MAIN FUNCTIONS ===

bool WiFiManager::initialize() {
  DEBUG_PRINTLN("🔧 Initializing WiFi Manager...");
  
  currentState = WIFI_STATE_INITIALIZING;
  
  // Set WiFi mode to station
  WiFi.mode(WIFI_STA);
  
  // Register event handler
  WiFi.onEvent(staticWiFiEventHandler);
  
  // Set hostname
  WiFi.setHostname(DEVICE_NAME);
  
  // Load credentials from configuration
  if (strlen(systemConfig.wifiSSID) > 0) {
    targetSSID = String(systemConfig.wifiSSID);
    targetPassword = String(systemConfig.wifiPassword);
    DEBUG_PRINTF("✅ Loaded WiFi credentials: %s\n", targetSSID.c_str());
  } else {
    DEBUG_PRINTLN("⚠️ No WiFi credentials found");
  }
  
  currentState = WIFI_STATE_DISCONNECTED;
  
  DEBUG_PRINTLN("✅ WiFi Manager initialized");
  return true;
}

void WiFiManager::process() {
  unsigned long now = millis();
  
  // Update status periodically
  if (now - lastStatusCheck >= WIFI_STATUS_CHECK_INTERVAL_MS) {
    lastStatusCheck = now;
    updateConnectionStats();
    updateRSSIStats();
  }
  
  // Handle DNS requests in AP mode
  if (apModeActive && dnsServer) {
    processDNSRequests();
  }
  
  // State machine
  switch (currentState) {
    case WIFI_STATE_DISCONNECTED:
      if (autoReconnectEnabled && hasCredentials()) {
        if (now - lastConnectionAttempt >= getReconnectDelay()) {
          currentState = WIFI_STATE_CONNECTING;
          attemptConnection();
        }
      }
      break;
      
    case WIFI_STATE_CONNECTING:
      if (now - lastConnectionAttempt >= WIFI_CONNECTION_TIMEOUT_MS) {
        DEBUG_PRINTLN("⏰ WiFi connection timeout");
        handleDisconnection();
      }
      break;
      
    case WIFI_STATE_CONNECTED:
      if (!WIFI_IS_CONNECTED()) {
        DEBUG_PRINTLN("📡 WiFi connection lost");
        handleDisconnection();
      }
      break;
      
    case WIFI_STATE_AP_MODE:
      handleAPClient();
      
      // Auto-exit AP mode after timeout if no clients
      if (getAPClientCount() == 0 && 
          now - apModeStartTime >= WIFI_AP_TIMEOUT_MS) {
        DEBUG_PRINTLN("⏰ AP mode timeout - returning to station mode");
        stopAPMode();
        currentState = WIFI_STATE_DISCONNECTED;
      }
      break;
      
    default:
      break;
  }
}

void WiFiManager::reset() {
  DEBUG_PRINTLN("🔄 Resetting WiFi Manager...");
  
  disconnect();
  stopAPMode();
  
  currentState = WIFI_STATE_UNINITIALIZED;
  reconnectAttempts = 0;
  
  memset(&stats, 0, sizeof(WiFiStats));
  stats.minRSSI = 0;
  stats.maxRSSI = -100;
  stats.currentRSSI = -100;
  
  initialize();
  
  DEBUG_PRINTLN("✅ WiFi Manager reset complete");
}

// === CONNECTION MANAGEMENT ===

bool WiFiManager::connect(const String& ssid, const String& password) {
  setCredentials(ssid, password);
  return connect();
}

bool WiFiManager::connect() {
  if (!hasCredentials()) {
    DEBUG_PRINTLN("❌ No WiFi credentials available");
    return false;
  }
  
  DEBUG_PRINTF("📡 Connecting to WiFi: %s\n", targetSSID.c_str());
  
  currentState = WIFI_STATE_CONNECTING;
  lastConnectionAttempt = millis();
  stats.connectionAttempts++;
  
  WiFi.begin(targetSSID.c_str(), targetPassword.c_str());
  
  return true;
}

void WiFiManager::disconnect() {
  if (currentState == WIFI_STATE_CONNECTED || currentState == WIFI_STATE_CONNECTING) {
    DEBUG_PRINTLN("📡 Disconnecting WiFi...");
    WiFi.disconnect(true);
    currentState = WIFI_STATE_DISCONNECTED;
    stats.disconnections++;
    stats.lastDisconnectionTime = millis();
  }
}

bool WiFiManager::reconnect() {
  DEBUG_PRINTLN("🔄 Reconnecting WiFi...");
  
  disconnect();
  delay(1000);
  
  stats.reconnectAttempts++;
  reconnectAttempts++;
  
  return connect();
}

// === PRIVATE METHODS ===

void WiFiManager::attemptConnection() {
  if (!hasCredentials()) return;
  
  DEBUG_PRINTF("🔗 Attempting WiFi connection to %s (attempt %d/%d)\n", 
               targetSSID.c_str(), reconnectAttempts + 1, WIFI_MAX_RECONNECT_ATTEMPTS);
  
  // Setup static IP if configured
  setupStaticIP();
  
  WiFi.begin(targetSSID.c_str(), targetPassword.c_str());
  lastConnectionAttempt = millis();
}

void WiFiManager::handleDisconnection() {
  if (currentState == WIFI_STATE_CONNECTED) {
    stats.disconnections++;
    stats.lastDisconnectionTime = millis();
    
    if (stats.lastConnectionTime > 0) {
      stats.totalConnectedTime += millis() - stats.lastConnectionTime;
    }
  }
  
  currentState = WIFI_STATE_DISCONNECTED;
  reconnectAttempts++;
  
  // Trigger disconnected callback
  if (disconnectedCallback) {
    disconnectedCallback();
  }
  
  // Start AP mode if max reconnect attempts reached and fallback enabled
  if (apFallbackEnabled && reconnectAttempts >= WIFI_MAX_RECONNECT_ATTEMPTS) {
    DEBUG_PRINTLN("🏗️ Max reconnect attempts reached - starting AP mode");
    startAccessPoint();
  }
}

void WiFiManager::startAccessPoint() {
  DEBUG_PRINTLN("🏗️ Starting Access Point mode...");
  
  stopAPMode(); // Stop any existing AP
  
  WiFi.mode(WIFI_AP_STA);
  
  // Configure AP
  IPAddress apIP, apGateway, apSubnet;
  apIP.fromString(WIFI_AP_IP_ADDRESS);
  apGateway.fromString(WIFI_AP_GATEWAY);
  apSubnet.fromString(WIFI_AP_SUBNET);
  
  WiFi.softAPConfig(apIP, apGateway, apSubnet);
  
  bool success = WiFi.softAP(WIFI_AP_SSID, WIFI_AP_PASSWORD, WIFI_AP_CHANNEL, 0, WIFI_AP_MAX_CONNECTIONS);
  
  if (success) {
    currentState = WIFI_STATE_AP_MODE;
    currentMode = WIFI_MODE_ACCESS_POINT;
    apModeActive = true;
    apModeStartTime = millis();
    stats.apModeActivations++;
    
    // Start DNS server for captive portal
    if (!dnsServer) {
      dnsServer = new DNSServer();
    }
    dnsServer->start(DNS_PORT, "*", apIP);
    
    DEBUG_PRINTF("✅ Access Point started: %s\n", WIFI_AP_SSID);
    DEBUG_PRINTF("   IP Address: %s\n", WiFi.softAPIP().toString().c_str());
    DEBUG_PRINTF("   Password: %s\n", WIFI_AP_PASSWORD);
  } else {
    DEBUG_PRINTLN("❌ Failed to start Access Point");
    currentState = WIFI_STATE_ERROR;
  }
}

void WiFiManager::stopAccessPoint() {
  if (apModeActive) {
    DEBUG_PRINTLN("🛑 Stopping Access Point mode...");
    
    if (dnsServer) {
      dnsServer->stop();
      delete dnsServer;
      dnsServer = nullptr;
    }
    
    WiFi.softAPdisconnect(true);
    WiFi.mode(WIFI_STA);
    
    apModeActive = false;
    currentMode = WIFI_MODE_STATION;
    
    DEBUG_PRINTLN("✅ Access Point stopped");
  }
}

void WiFiManager::updateRSSIStats() {
  if (currentState != WIFI_STATE_CONNECTED) return;
  
  unsigned long now = millis();
  if (now - lastRSSIUpdate < WIFI_RSSI_UPDATE_INTERVAL_MS) return;
  
  lastRSSIUpdate = now;
  
  int32_t newRSSI = WiFi.RSSI();
  if (newRSSI != stats.currentRSSI) {
    stats.currentRSSI = newRSSI;
    
    // Update min/max
    if (newRSSI < stats.minRSSI) stats.minRSSI = newRSSI;
    if (newRSSI > stats.maxRSSI) stats.maxRSSI = newRSSI;
    
    // Update average
    stats.rssiSum += newRSSI;
    stats.rssiSamples++;
    stats.avgRSSI = stats.rssiSum / stats.rssiSamples;
    
    // Trigger RSSI callback
    if (rssiChangeCallback) {
      rssiChangeCallback(newRSSI);
    }
  }
  
  // Update other network info
  stats.currentChannel = WiFi.channel();
  stats.currentBSSID = WiFi.BSSIDstr();
}

void WiFiManager::updateConnectionStats() {
  // Update connection time statistics
  if (currentState == WIFI_STATE_CONNECTED && stats.lastConnectionTime > 0) {
    // Connection is ongoing
  } else if (currentState != WIFI_STATE_CONNECTED && stats.lastDisconnectionTime > 0) {
    stats.totalDisconnectedTime += millis() - stats.lastDisconnectionTime;
  }
}

void WiFiManager::setupStaticIP() {
  if (strlen(systemConfig.deviceIP) > 0 && strcmp(systemConfig.deviceIP, "DHCP") != 0) {
    IPAddress localIP, gateway, subnet, dns1;
    
    if (localIP.fromString(systemConfig.deviceIP)) {
      // Use default gateway and subnet if not specified
      gateway.fromString("192.168.1.1");  // Default gateway
      subnet.fromString("255.255.255.0"); // Default subnet
      dns1.fromString("8.8.8.8");         // Google DNS
      
      if (WiFi.config(localIP, gateway, subnet, dns1)) {
        DEBUG_PRINTF("🌐 Static IP configured: %s\n", systemConfig.deviceIP);
      } else {
        DEBUG_PRINTLN("❌ Static IP configuration failed");
      }
    }
  }
}

bool WiFiManager::isValidCredentials() {
  return targetSSID.length() > 0 && targetSSID.length() <= 32 &&
         targetPassword.length() >= 0 && targetPassword.length() <= 64;
}

void WiFiManager::handleAPClient() {
  int clientCount = WiFi.softAPgetStationNum();
  static int lastClientCount = -1;
  
  if (clientCount != lastClientCount) {
    lastClientCount = clientCount;
    
    if (clientCount > 0) {
      if (currentState != WIFI_STATE_AP_CLIENT_CONNECTED) {
        currentState = WIFI_STATE_AP_CLIENT_CONNECTED;
        apClientConnectedTime = millis();
        DEBUG_PRINTF("👤 AP client connected (total: %d)\n", clientCount);
      }
    } else {
      if (currentState == WIFI_STATE_AP_CLIENT_CONNECTED) {
        currentState = WIFI_STATE_AP_MODE;
        DEBUG_PRINTLN("👤 All AP clients disconnected");
      }
    }
    
    // Trigger AP client callback
    if (apClientCallback) {
      apClientCallback(clientCount);
    }
  }
}

void WiFiManager::processDNSRequests() {
  if (dnsServer) {
    dnsServer->processNextRequest();
  }
}

// === EVENT HANDLING ===

void WiFiManager::onWiFiEvent(WiFiEvent_t event) {
  WiFiState_t oldState = currentState;
  
  switch (event) {
    case ARDUINO_EVENT_WIFI_STA_START:
      DEBUG_PRINTLN("📡 WiFi STA started");
      break;
      
    case ARDUINO_EVENT_WIFI_STA_CONNECTED:
      DEBUG_PRINTF("🔗 WiFi connected to: %s\n", WiFi.SSID().c_str());
      break;
      
    case ARDUINO_EVENT_WIFI_STA_GOT_IP:
      currentState = WIFI_STATE_CONNECTED;
      stats.successfulConnections++;
      stats.lastConnectionTime = millis();
      reconnectAttempts = 0; // Reset reconnect counter
      
      DEBUG_PRINTF("✅ WiFi connected! IP: %s\n", WiFi.localIP().toString().c_str());
      DEBUG_PRINTF("   Gateway: %s\n", WiFi.gatewayIP().toString().c_str());
      DEBUG_PRINTF("   DNS: %s\n", WiFi.dnsIP().toString().c_str());
      DEBUG_PRINTF("   RSSI: %d dBm\n", WiFi.RSSI());
      
      // Stop AP mode if active
      if (apModeActive) {
        stopAccessPoint();
      }
      
      // Trigger connected callback
      if (connectedCallback) {
        connectedCallback(WiFi.localIP().toString());
      }
      break;
      
    case ARDUINO_EVENT_WIFI_STA_LOST_IP:
      DEBUG_PRINTLN("📡 WiFi lost IP address");
      break;
      
    case ARDUINO_EVENT_WIFI_STA_DISCONNECTED:
      if (currentState == WIFI_STATE_CONNECTED || currentState == WIFI_STATE_CONNECTING) {
        DEBUG_PRINTLN("📡 WiFi disconnected");
        handleDisconnection();
      }
      break;
      
    case ARDUINO_EVENT_WIFI_AP_START:
      DEBUG_PRINTLN("🏗️ WiFi AP started");
      break;
      
    case ARDUINO_EVENT_WIFI_AP_STOP:
      DEBUG_PRINTLN("🏗️ WiFi AP stopped");
      break;
      
    case ARDUINO_EVENT_WIFI_AP_STACONNECTED:
      DEBUG_PRINTLN("👤 Station connected to AP");
      break;
      
    case ARDUINO_EVENT_WIFI_AP_STADISCONNECTED:
      DEBUG_PRINTLN("👤 Station disconnected from AP");
      break;
      
    default:
      break;
  }
  
  // Trigger state change callback
  if (oldState != currentState && stateChangeCallback) {
    stateChangeCallback(oldState, currentState);
  }
}

void WiFiManager::staticWiFiEventHandler(WiFiEvent_t event) {
  if (staticInstance) {
    staticInstance->onWiFiEvent(event);
  }
}

unsigned long WiFiManager::getReconnectDelay() {
  // Exponential backoff: 1s, 2s, 4s, 8s, 16s, then cap at 60s
  unsigned long delay = 1000UL << min(reconnectAttempts, 6);
  return min(delay, WIFI_RECONNECT_INTERVAL_MS);
}

// === CREDENTIAL MANAGEMENT ===

void WiFiManager::setCredentials(const String& ssid, const String& password) {
  targetSSID = ssid;
  targetPassword = password;
  
  // Save to system configuration
  strncpy(systemConfig.wifiSSID, ssid.c_str(), MAX_WIFI_SSID_LENGTH - 1);
  strncpy(systemConfig.wifiPassword, password.c_str(), MAX_WIFI_PASSWORD_LENGTH - 1);
  systemConfig.wifiSSID[MAX_WIFI_SSID_LENGTH - 1] = '\0';
  systemConfig.wifiPassword[MAX_WIFI_PASSWORD_LENGTH - 1] = '\0';
  
  // Save configuration to EEPROM
  saveConfiguration();
  
  DEBUG_PRINTF("💾 WiFi credentials updated: %s\n", ssid.c_str());
}

String WiFiManager::getSSID() const {
  return targetSSID;
}

bool WiFiManager::hasCredentials() const {
  return targetSSID.length() > 0;
}

// === NETWORK SCANNING ===

int WiFiManager::scanNetworks() {
  DEBUG_PRINTLN("🔍 Scanning for WiFi networks...");
  
  scanInProgress = true;
  stats.scanAttempts++;
  
  int networkCount = WiFi.scanNetworks();
  
  scannedNetworks.clear();
  
  if (networkCount > 0) {
    DEBUG_PRINTF("✅ Found %d networks:\n", networkCount);
    
    for (int i = 0; i < networkCount; i++) {
      WiFiNetwork network;
      network.ssid = WiFi.SSID(i);
      network.rssi = WiFi.RSSI(i);
      network.channel = WiFi.channel(i);
      network.security = getSecurityType(WiFi.encryptionType(i));
      network.isHidden = network.ssid.length() == 0;
      network.bssid = WiFi.BSSIDstr(i);
      
      scannedNetworks.push_back(network);
      
      DEBUG_PRINTF("   %d: %s (%d dBm) Ch:%d %s\n", 
                   i + 1, 
                   network.ssid.c_str(),
                   network.rssi,
                   network.channel,
                   wifiSecurityToString(network.security).c_str());
    }
  } else {
    DEBUG_PRINTLN("❌ No networks found");
  }
  
  scanInProgress = false;
  lastScanTime = millis();
  
  return networkCount;
}

int WiFiManager::getNetworkCount() const {
  return scannedNetworks.size();
}

WiFiNetwork WiFiManager::getNetwork(int index) const {
  if (index >= 0 && index < scannedNetworks.size()) {
    return scannedNetworks[index];
  }
  return WiFiNetwork(); // Return empty network
}

String WiFiManager::getNetworkSSID(int index) const {
  if (index >= 0 && index < scannedNetworks.size()) {
    return scannedNetworks[index].ssid;
  }
  return "";
}

int32_t WiFiManager::getNetworkRSSI(int index) const {
  if (index >= 0 && index < scannedNetworks.size()) {
    return scannedNetworks[index].rssi;
  }
  return -100;
}

WiFiSecurity_t WiFiManager::getNetworkSecurity(int index) const {
  if (index >= 0 && index < scannedNetworks.size()) {
    return scannedNetworks[index].security;
  }
  return WIFI_SECURITY_UNKNOWN;
}

// === ACCESS POINT MODE ===

bool WiFiManager::startAPMode() {
  startAccessPoint();
  return apModeActive;
}

bool WiFiManager::stopAPMode() {
  stopAccessPoint();
  return !apModeActive;
}

bool WiFiManager::isAPModeActive() const {
  return apModeActive;
}

int WiFiManager::getAPClientCount() const {
  if (apModeActive) {
    return WiFi.softAPgetStationNum();
  }
  return 0;
}

// === STATUS AND DIAGNOSTICS ===

WiFiState_t WiFiManager::getState() const {
  return currentState;
}

WiFiMode_t WiFiManager::getMode() const {
  return currentMode;
}

bool WiFiManager::isConnected() const {
  return currentState == WIFI_STATE_CONNECTED;
}

bool WiFiManager::isConnecting() const {
  return currentState == WIFI_STATE_CONNECTING;
}

String WiFiManager::getLocalIP() const {
  if (isConnected()) {
    return WiFi.localIP().toString();
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

int32_t WiFiManager::getRSSI() const {
  if (isConnected()) {
    return WiFi.RSSI();
  }
  return stats.currentRSSI;
}

uint8_t WiFiManager::getChannel() const {
  if (isConnected()) {
    return WiFi.channel();
  }
  return stats.currentChannel;
}

String WiFiManager::getBSSID() const {
  if (isConnected()) {
    return WiFi.BSSIDstr();
  }
  return stats.currentBSSID;
}

// === CONFIGURATION ===

void WiFiManager::enableAutoReconnect(bool enabled) {
  autoReconnectEnabled = enabled;
  DEBUG_PRINTF("🔄 Auto-reconnect %s\n", enabled ? "enabled" : "disabled");
}

void WiFiManager::enableAPFallback(bool enabled) {
  apFallbackEnabled = enabled;
  DEBUG_PRINTF("🏗️ AP fallback %s\n", enabled ? "enabled" : "disabled");
}

void WiFiManager::setReconnectInterval(unsigned long intervalMs) {
  // Implementation would store this value and use it in getReconnectDelay()
  DEBUG_PRINTF("⏰ Reconnect interval set to %lu ms\n", intervalMs);
}

void WiFiManager::setConnectionTimeout(unsigned long timeoutMs) {
  // Implementation would store this value and use it in connection timeout logic
  DEBUG_PRINTF("⏰ Connection timeout set to %lu ms\n", timeoutMs);
}

// === STATISTICS AND MONITORING ===

const WiFiStats& WiFiManager::getStatistics() const {
  return stats;
}

void WiFiManager::resetStatistics() {
  memset(&stats, 0, sizeof(WiFiStats));
  stats.minRSSI = 0;
  stats.maxRSSI = -100;
  stats.currentRSSI = -100;
  DEBUG_PRINTLN("📊 WiFi statistics reset");
}

void WiFiManager::printStatus() const {
  DEBUG_PRINTLN("\n📡 === WIFI STATUS ===");
  DEBUG_PRINTF("   State: %s\n", wifiStateToString(currentState).c_str());
  DEBUG_PRINTF("   Mode: %s\n", wifiModeToString(currentMode).c_str());
  
  if (isConnected()) {
    DEBUG_PRINTF("   SSID: %s\n", WiFi.SSID().c_str());
    DEBUG_PRINTF("   IP Address: %s\n", getLocalIP().c_str());
    DEBUG_PRINTF("   Gateway: %s\n", getGatewayIP().c_str());
    DEBUG_PRINTF("   DNS: %s\n", getDNS().c_str());
    DEBUG_PRINTF("   RSSI: %d dBm (%s)\n", getRSSI(), getSignalStrength().c_str());
    DEBUG_PRINTF("   Channel: %d\n", getChannel());
    DEBUG_PRINTF("   BSSID: %s\n", getBSSID().c_str());
  } else if (hasCredentials()) {
    DEBUG_PRINTF("   Target SSID: %s\n", targetSSID.c_str());
    DEBUG_PRINTF("   Reconnect attempts: %d/%d\n", reconnectAttempts, WIFI_MAX_RECONNECT_ATTEMPTS);
  }
  
  if (apModeActive) {
    DEBUG_PRINTF("   AP SSID: %s\n", WIFI_AP_SSID);
    DEBUG_PRINTF("   AP IP: %s\n", WiFi.softAPIP().toString().c_str());
    DEBUG_PRINTF("   AP Clients: %d\n", getAPClientCount());
  }
  
  DEBUG_PRINTF("   MAC Address: %s\n", getMACAddress().c_str());
  DEBUG_PRINTLN();
}

void WiFiManager::printStatistics() const {
  DEBUG_PRINTLN("\n📊 === WIFI STATISTICS ===");
  DEBUG_PRINTF("   Connection Attempts: %lu\n", stats.connectionAttempts);
  DEBUG_PRINTF("   Successful Connections: %lu\n", stats.successfulConnections);
  DEBUG_PRINTF("   Disconnections: %lu\n", stats.disconnections);
  DEBUG_PRINTF("   Reconnect Attempts: %lu\n", stats.reconnectAttempts);
  DEBUG_PRINTF("   AP Mode Activations: %lu\n", stats.apModeActivations);
  DEBUG_PRINTF("   Network Scans: %lu\n", stats.scanAttempts);
  
  if (stats.lastConnectionTime > 0) {
    DEBUG_PRINTF("   Last Connected: %lu ms ago\n", millis() - stats.lastConnectionTime);
  }
  
  if (stats.rssiSamples > 0) {
    DEBUG_PRINTF("   RSSI - Current: %d dBm, Min: %d dBm, Max: %d dBm, Avg: %d dBm\n",
                 stats.currentRSSI, stats.minRSSI, stats.maxRSSI, stats.avgRSSI);
  }
  
  DEBUG_PRINTF("   Total Connected Time: %lu ms\n", stats.totalConnectedTime);
  DEBUG_PRINTF("   Total Disconnected Time: %lu ms\n", stats.totalDisconnectedTime);
  DEBUG_PRINTLN();
}

void WiFiManager::printNetworkInfo() const {
  if (scannedNetworks.size() > 0) {
    DEBUG_PRINTLN("\n🔍 === SCANNED NETWORKS ===");
    for (size_t i = 0; i < scannedNetworks.size(); i++) {
      const WiFiNetwork& network = scannedNetworks[i];
      DEBUG_PRINTF("   %d: %s (%d dBm) Ch:%d %s %s\n",
                   (int)i + 1,
                   network.ssid.c_str(),
                   network.rssi,
                   network.channel,
                   wifiSecurityToString(network.security).c_str(),
                   network.isHidden ? "[HIDDEN]" : "");
    }
    DEBUG_PRINTLN();
  }
}

// === QUALITY METRICS ===

int WiFiManager::getSignalQuality() const {
  return rssiToPercentage(getRSSI());
}

String WiFiManager::getSignalStrength() const {
  return rssiToQuality(getRSSI());
}

bool WiFiManager::isSignalStable() const {
  if (stats.rssiSamples < 10) return true; // Not enough samples
  
  int32_t rssiVariance = stats.maxRSSI - stats.minRSSI;
  return rssiVariance < 20; // Consider stable if variance < 20 dBm
}

// === ADVANCED FEATURES ===

bool WiFiManager::setStaticIP(const String& ip, const String& gateway, const String& subnet, const String& dns1, const String& dns2) {
  IPAddress localIP, gatewayIP, subnetMask, primaryDNS, secondaryDNS;
  
  if (!localIP.fromString(ip) || !gatewayIP.fromString(gateway) || !subnetMask.fromString(subnet)) {
    DEBUG_PRINTLN("❌ Invalid IP configuration");
    return false;
  }
  
  if (dns1.length() > 0) {
    primaryDNS.fromString(dns1);
  }
  
  if (dns2.length() > 0) {
    secondaryDNS.fromString(dns2);
  }
  
  bool success = WiFi.config(localIP, gatewayIP, subnetMask, primaryDNS, secondaryDNS);
  
  if (success) {
    // Save to system configuration
    strncpy(systemConfig.deviceIP, ip.c_str(), MAX_IP_ADDRESS_LENGTH - 1);
    systemConfig.deviceIP[MAX_IP_ADDRESS_LENGTH - 1] = '\0';
    saveConfiguration();
    
    DEBUG_PRINTF("🌐 Static IP configured: %s\n", ip.c_str());
  } else {
    DEBUG_PRINTLN("❌ Static IP configuration failed");
  }
  
  return success;
}

bool WiFiManager::enableDHCP() {
  // Clear static IP configuration
  strcpy(systemConfig.deviceIP, "DHCP");
  saveConfiguration();
  
  DEBUG_PRINTLN("🌐 DHCP enabled");
  return true;
}

bool WiFiManager::isStaticIP() const {
  return strlen(systemConfig.deviceIP) > 0 && strcmp(systemConfig.deviceIP, "DHCP") != 0;
}

// === POWER MANAGEMENT ===

bool WiFiManager::enablePowerSave(bool enabled) {
  wifi_power_t powerMode = enabled ? WIFI_POWER_11dBm : WIFI_POWER_19_5dBm;
  return setPowerMode(powerMode);
}

bool WiFiManager::setPowerMode(wifi_power_t power) {
  bool success = WiFi.setTxPower(power);
  if (success) {
    DEBUG_PRINTF("⚡ WiFi power mode set to %d\n", power);
  } else {
    DEBUG_PRINTLN("❌ Failed to set WiFi power mode");
  }
  return success;
}

wifi_power_t WiFiManager::getPowerMode() const {
  return WiFi.getTxPower();
}

// === ERROR HANDLING ===

String WiFiManager::getLastError() const {
  // Implementation would return the last error message
  return "";
}

bool WiFiManager::hasErrors() const {
  return currentState == WIFI_STATE_ERROR;
}

void WiFiManager::clearErrors() {
  if (currentState == WIFI_STATE_ERROR) {
    currentState = WIFI_STATE_DISCONNECTED;
    DEBUG_PRINTLN("🧹 WiFi errors cleared");
  }
}

// === HELPER FUNCTIONS ===

String wifiStateToString(WiFiState_t state) {
  switch (state) {
    case WIFI_STATE_UNINITIALIZED: return "Uninitialized";
    case WIFI_STATE_INITIALIZING: return "Initializing";
    case WIFI_STATE_SCANNING: return "Scanning";
    case WIFI_STATE_CONNECTING: return "Connecting";
    case WIFI_STATE_CONNECTED: return "Connected";
    case WIFI_STATE_DISCONNECTED: return "Disconnected";
    case WIFI_STATE_RECONNECTING: return "Reconnecting";
    case WIFI_STATE_AP_MODE: return "AP Mode";
    case WIFI_STATE_AP_CLIENT_CONNECTED: return "AP Client Connected";
    case WIFI_STATE_ERROR: return "Error";
    case WIFI_STATE_DISABLED: return "Disabled";
    default: return "Unknown";
  }
}

String wifiModeToString(WiFiMode_t mode) {
  switch (mode) {
    case WIFI_MODE_STATION: return "Station";
    case WIFI_MODE_ACCESS_POINT: return "Access Point";
    case WIFI_MODE_STATION_AP: return "Station + AP";
    case WIFI_MODE_DISABLED: return "Disabled";
    default: return "Unknown";
  }
}

String wifiSecurityToString(WiFiSecurity_t security) {
  switch (security) {
    case WIFI_SECURITY_OPEN: return "Open";
    case WIFI_SECURITY_WEP: return "WEP";
    case WIFI_SECURITY_WPA: return "WPA";
    case WIFI_SECURITY_WPA2: return "WPA2";
    case WIFI_SECURITY_WPA3: return "WPA3";
    case WIFI_SECURITY_UNKNOWN: return "Unknown";
    default: return "Unknown";
  }
}

String wifiEventToString(WiFiEvent_t event) {
  switch (event) {
    case ARDUINO_EVENT_WIFI_STA_START: return "STA Started";
    case ARDUINO_EVENT_WIFI_STA_CONNECTED: return "STA Connected";
    case ARDUINO_EVENT_WIFI_STA_GOT_IP: return "STA Got IP";
    case ARDUINO_EVENT_WIFI_STA_LOST_IP: return "STA Lost IP";
    case ARDUINO_EVENT_WIFI_STA_DISCONNECTED: return "STA Disconnected";
    case ARDUINO_EVENT_WIFI_AP_START: return "AP Started";
    case ARDUINO_EVENT_WIFI_AP_STOP: return "AP Stopped";
    case ARDUINO_EVENT_WIFI_AP_STACONNECTED: return "AP Station Connected";
    case ARDUINO_EVENT_WIFI_AP_STADISCONNECTED: return "AP Station Disconnected";
    default: return "Unknown Event";
  }
}

WiFiSecurity_t getSecurityType(wifi_auth_mode_t authMode) {
  switch (authMode) {
    case WIFI_AUTH_OPEN: return WIFI_SECURITY_OPEN;
    case WIFI_AUTH_WEP: return WIFI_SECURITY_WEP;
    case WIFI_AUTH_WPA_PSK: return WIFI_SECURITY_WPA;
    case WIFI_AUTH_WPA2_PSK: return WIFI_SECURITY_WPA2;
    case WIFI_AUTH_WPA_WPA2_PSK: return WIFI_SECURITY_WPA2;
    case WIFI_AUTH_WPA3_PSK: return WIFI_SECURITY_WPA3;
    default: return WIFI_SECURITY_UNKNOWN;
  }
}

int rssiToPercentage(int32_t rssi) {
  if (rssi >= -30) return 100;
  if (rssi <= -90) return 0;
  return 2 * (rssi + 100);
}

String rssiToQuality(int32_t rssi) {
  if (rssi >= -30) return "Excellent";
  if (rssi >= -50) return "Very Good";
  if (rssi >= -60) return "Good";
  if (rssi >= -70) return "Fair";
  if (rssi >= -80) return "Poor";
  return "Very Poor";
}

// === CALLBACK REGISTRATION ===

void setWiFiStateChangeCallback(WiFiStateChangeCallback callback) {
  stateChangeCallback = callback;
}

void setWiFiRSSIChangeCallback(WiFiRSSIChangeCallback callback) {
  rssiChangeCallback = callback;
}

void setWiFiConnectedCallback(WiFiConnectedCallback callback) {
  connectedCallback = callback;
}

void setWiFiDisconnectedCallback(WiFiDisconnectedCallback callback) {
  disconnectedCallback = callback;
}

void setWiFiAPClientCallback(WiFiAPClientCallback callback) {
  apClientCallback = callback;
}