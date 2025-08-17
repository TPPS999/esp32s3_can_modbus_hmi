/*
 * web_server.cpp - ESP32S3 CAN to Modbus TCP Bridge - Web Configuration Server Implementation
 * 
 * VERSION: v1.0.1 - Clean version without emojis
 * DATE: 2025-08-17
 */

#include "web_server.h"
#include "wifi_manager.h"
#include "utils.h"
#include <WiFi.h>
#include <mcp_can.h>

// === GLOBAL INSTANCE ===
ConfigWebServer configWebServer;

// === CONSTRUCTOR & DESTRUCTOR ===

ConfigWebServer::ConfigWebServer() 
  : server(nullptr)
  , serverRunning(false)
{
}

ConfigWebServer::~ConfigWebServer() {
  end();
}

// === LIFECYCLE MANAGEMENT ===

bool ConfigWebServer::begin() {
  if (serverRunning) {
    Serial.println("Web server already running");
    return true;
  }
  
  Serial.println("Starting configuration web server...");
  
  server = new AsyncWebServer(WEB_SERVER_PORT);
  
  // === ROUTE HANDLERS ===
  
  // Main page
  server->on("/", HTTP_GET, [this](AsyncWebServerRequest *request) {
    handleRoot(request);
  });
  
  // WiFi configuration
  server->on("/wifi", HTTP_GET, [this](AsyncWebServerRequest *request) {
    handleWiFiConfig(request);
  });
  
  server->on("/wifi/save", HTTP_POST, [this](AsyncWebServerRequest *request) {
    handleWiFiSave(request);
  });
  
  // Network configuration
  server->on("/network", HTTP_GET, [this](AsyncWebServerRequest *request) {
    handleNetworkConfig(request);
  });
  
  server->on("/network/save", HTTP_POST, [this](AsyncWebServerRequest *request) {
    handleNetworkSave(request);
  });
  
  // BMS configuration
  server->on("/bms", HTTP_GET, [this](AsyncWebServerRequest *request) {
    handleBMSConfig(request);
  });
  
  server->on("/bms/save", HTTP_POST, [this](AsyncWebServerRequest *request) {
    handleBMSSave(request);
  });
  
  // CAN monitoring
  server->on("/can", HTTP_GET, [this](AsyncWebServerRequest *request) {
    handleCANMonitor(request);
  });
  
  // System status
  server->on("/status", HTTP_GET, [this](AsyncWebServerRequest *request) {
    handleSystemStatus(request);
  });
  
  // Configuration export/import
  server->on("/config/export", HTTP_GET, [this](AsyncWebServerRequest *request) {
    handleConfigExport(request);
  });
  
  // System restart
  server->on("/restart", HTTP_POST, [this](AsyncWebServerRequest *request) {
    handleRestart(request);
  });
  
  // 404 handler
  server->onNotFound([this](AsyncWebServerRequest *request) {
    handleNotFound(request);
  });
  
  // Start server
  server->begin();
  serverRunning = true;
  
  Serial.printf("Web server started on port %d\n", WEB_SERVER_PORT);
  Serial.printf("Access at: http://%s/\n", WiFi.softAPIP().toString().c_str());
  
  return true;
}

void ConfigWebServer::end() {
  if (server && serverRunning) {
    Serial.println("Stopping web server...");
    server->end();
    delete server;
    server = nullptr;
    serverRunning = false;
    Serial.println("Web server stopped");
  }
}

// === PAGE GENERATORS ===

String ConfigWebServer::generateMainPage() {
  String html = "<!DOCTYPE html><html><head>";
  html += "<title>ESP32S3 CAN-Modbus Configuration</title>";
  html += "<meta name='viewport' content='width=device-width, initial-scale=1'>";
  html += CSS_STYLE;
  html += "</head><body>";
  
  html += "<div class='container'>";
  html += "<div class='header'>";
  html += "<h1>ESP32S3 CAN to Modbus TCP Bridge</h1>";
  html += "<p>Configuration Panel - Firmware v" + String(FIRMWARE_VERSION) + "</p>";
  html += "</div>";
  
  html += "<div class='nav'>";
  html += "<a href='/wifi'>WiFi Settings</a>";
  html += "<a href='/network'>Network Config</a>";
  html += "<a href='/bms'>BMS Setup</a>";
  html += "<a href='/can'>CAN Monitor</a>";
  html += "<a href='/status'>System Status</a>";
  html += "</div>";
  
  html += "<h2>Quick Configuration</h2>";
  html += "<div class='grid'>";
  
  // Current WiFi Status
  html += "<div>";
  html += "<h3>WiFi Status</h3>";
  if (wifiManager.isConnected()) {
    html += "<div class='status success'>Connected to: " + wifiManager.getSSID() + "</div>";
    html += "<p><strong>IP:</strong> " + wifiManager.getLocalIP() + "</p>";
    html += "<p><strong>RSSI:</strong> " + String(wifiManager.getRSSI()) + " dBm</p>";
  } else {
    html += "<div class='status error'>Not connected to WiFi</div>";
    html += "<p>Configure WiFi credentials to connect</p>";
  }
  html += "</div>";
  
  // Current BMS Status
  html += "<div>";
  html += "<h3>BMS Status</h3>";
  html += "<p><strong>Active Batteries:</strong> " + String(systemConfig.activeBmsNodes) + "</p>";
  html += "<p><strong>CAN Speed:</strong> " + String(systemConfig.canSpeed == CAN_500KBPS ? "500" : "125") + " kbps</p>";
  html += "<p><strong>Node IDs:</strong> ";
  for (int i = 0; i < systemConfig.activeBmsNodes; i++) {
    html += String(systemConfig.bmsNodeIds[i]);
    if (i < systemConfig.activeBmsNodes - 1) html += ", ";
  }
  html += "</p>";
  html += "<p><strong>Modbus Registers:</strong> " + String(systemConfig.activeBmsNodes * 200) + "</p>";
  html += "</div>";
  
  html += "</div>";
  
  // Quick Actions
  html += "<h2>Quick Actions</h2>";
  html += "<div class='nav'>";
  html += "<a href='/config/export' class='btn'>Export Config</a>";
  html += "<a href='/restart' onclick='return confirmRestart()' class='btn btn-danger'>Restart Device</a>";
  html += "</div>";
  
  // System Information
  html += "<h2>System Information</h2>";
  html += "<table>";
  html += "<tr><td><strong>Device:</strong></td><td>" + String(DEVICE_NAME) + "</td></tr>";
  html += "<tr><td><strong>Firmware:</strong></td><td>" + String(FIRMWARE_VERSION) + "</td></tr>";
  html += "<tr><td><strong>Build Date:</strong></td><td>" + String(BUILD_DATE) + "</td></tr>";
  html += "<tr><td><strong>Free Memory:</strong></td><td>" + formatBytes(ESP.getFreeHeap()) + "</td></tr>";
  html += "<tr><td><strong>Uptime:</strong></td><td>" + formatUptime(millis()) + "</td></tr>";
  html += "<tr><td><strong>AP IP:</strong></td><td>" + WiFi.softAPIP().toString() + "</td></tr>";
  html += "</table>";
  
  html += "</div>";
  html += JAVASCRIPT_CODE;
  html += "</body></html>";
  
  return html;
}

String ConfigWebServer::generateBMSConfigPage() {
  String html = "<!DOCTYPE html><html><head>";
  html += "<title>BMS Configuration</title>";
  html += "<meta name='viewport' content='width=device-width, initial-scale=1'>";
  html += CSS_STYLE;
  html += "</head><body>";
  
  html += "<div class='container'>";
  html += "<div class='header'>";
  html += "<h1>BMS Configuration</h1>";
  html += "<a href='/' style='color: #ecf0f1; text-decoration: none;'>← Back to Main</a>";
  html += "</div>";
  
  // BMS Configuration Form
  html += "<form method='POST' action='/bms/save'>";
  html += "<h2>Battery Management System Setup</h2>";
  
  html += "<div class='form-group'>";
  html += "<label for='battery_count'>Number of Active Batteries:</label>";
  html += "<input type='number' id='battery_count' name='battery_count' value='" + String(systemConfig.activeBmsNodes) + "' min='1' max='16' required>";
  html += "<small>Each battery uses 200 Modbus registers</small>";
  html += "</div>";
  
  // CAN Speed Configuration
  html += "<div class='form-group'>";
  html += "<label for='can_speed'>CAN Bus Speed:</label>";
  html += "<select id='can_speed' name='can_speed' required>";
  html += "<option value='125'" + String(systemConfig.canSpeed == CAN_125KBPS ? " selected" : "") + ">125 kbps</option>";
  html += "<option value='500'" + String(systemConfig.canSpeed == CAN_500KBPS ? " selected" : "") + ">500 kbps</option>";
  html += "</select>";
  html += "</div>";
  
  html += "<h3>Battery ID Assignment</h3>";
  html += "<p>Assign unique CAN Node IDs (1-16) to each battery:</p>";
  
  html += "<table id='battery_table'>";
  html += "<thead>";
  html += "<tr><th>Index</th><th>CAN Node ID</th><th>Frame 710 Address</th><th>Name/Description</th></tr>";
  html += "</thead>";
  html += "<tbody>";
  
  // Generate rows for current batteries
  for (int i = 0; i < systemConfig.activeBmsNodes; i++) {
    uint8_t nodeId = systemConfig.bmsNodeIds[i];
    uint16_t frame710Address = 0x701 + nodeId - 1; // Calculate 710 frame address
    
    html += "<tr>";
    html += "<td>" + String(i + 1) + "</td>";
    html += "<td><input type='number' name='battery_id_" + String(i + 1) + "' min='1' max='16' value='" + String(nodeId) + "' required onchange='updateFrameAddress(this)'></td>";
    html += "<td><span id='frame_addr_" + String(i + 1) + "'>0x" + String(frame710Address, HEX) + "</span></td>";
    html += "<td><input type='text' name='battery_name_" + String(i + 1) + "' value='BMS " + String(nodeId) + "' maxlength='20'></td>";
    html += "</tr>";
  }
  
  html += "</tbody>";
  html += "</table>";
  
  html += "<h3>Modbus Register Layout</h3>";
  html += "<div class='status info'>";
  html += "Each battery uses 200 consecutive Modbus registers:<br>";
  html += "Battery 1: Registers 0-199<br>";
  html += "Battery 2: Registers 200-399<br>";
  html += "Battery 3: Registers 400-599<br>";
  html += "And so on...<br>";
  html += "Total registers used: <strong>" + String(systemConfig.activeBmsNodes * 200) + "</strong> of " + String(MODBUS_MAX_HOLDING_REGISTERS);
  html += "</div>";
  
  html += "<button type='submit' class='btn'>Save BMS Configuration</button>";
  html += "</form>";
  
  html += "</div>";
  
  // JavaScript for frame address calculation
  html += "<script>";
  html += "function updateFrameAddress(input) {";
  html += "  var row = input.closest('tr');";
  html += "  var index = row.rowIndex;";
  html += "  var nodeId = parseInt(input.value);";
  html += "  var frameAddr = 0x701 + nodeId - 1;";
  html += "  var spanId = 'frame_addr_' + index;";
  html += "  document.getElementById(spanId).textContent = '0x' + frameAddr.toString(16).toUpperCase();";
  html += "}";
  html += JAVASCRIPT_CODE;
  html += "</script>";
  
  html += "</body></html>";
  
  return html;
}

String ConfigWebServer::generateCANMonitorPage() {
  String html = "<!DOCTYPE html><html><head>";
  html += "<title>CAN Monitor</title>";
  html += "<meta name='viewport' content='width=device-width, initial-scale=1'>";
  html += CSS_STYLE;
  html += "</head><body>";
  
  html += "<div class='container'>";
  html += "<div class='header'>";
  html += "<h1>CAN Bus Monitor</h1>";
  html += "<a href='/' style='color: #ecf0f1; text-decoration: none;'>← Back to Main</a>";
  html += "</div>";
  
  // CAN Configuration Info
  html += "<h2>CAN Configuration</h2>";
  html += "<table>";
  html += "<tr><td><strong>CAN Speed:</strong></td><td>" + String(systemConfig.canSpeed == CAN_500KBPS ? "500" : "125") + " kbps</td></tr>";
  html += "<tr><td><strong>Active BMS Nodes:</strong></td><td>" + String(systemConfig.activeBmsNodes) + "</td></tr>";
  html += "</table>";
  
  // Frame Address Mapping
  html += "<h2>Frame Address Mapping</h2>";
  html += "<table>";
  html += "<thead>";
  html += "<tr><th>BMS Node ID</th><th>Frame 190</th><th>Frame 290</th><th>Frame 710</th><th>Status</th></tr>";
  html += "</thead>";
  html += "<tbody>";
  
  for (int i = 0; i < systemConfig.activeBmsNodes; i++) {
    uint8_t nodeId = systemConfig.bmsNodeIds[i];
    uint16_t frame190 = 0x181 + nodeId - 1;
    uint16_t frame290 = 0x281 + nodeId - 1;
    uint16_t frame710 = 0x701 + nodeId - 1;
    
    html += "<tr>";
    html += "<td>" + String(nodeId) + "</td>";
    html += "<td>0x" + String(frame190, HEX) + "</td>";
    html += "<td>0x" + String(frame290, HEX) + "</td>";
    html += "<td>0x" + String(frame710, HEX) + "</td>";
    html += "<td><span class='status info'>Online</span></td>"; // TODO: Add real status
    html += "</tr>";
  }
  
  html += "</tbody>";
  html += "</table>";
  
  // CAN Traffic Monitor (placeholder)
  html += "<h2>CAN Traffic Monitor</h2>";
  html += "<div class='status info'>";
  html += "<p><strong>Note:</strong> Real-time CAN monitoring will be implemented in future version.</p>";
  html += "<p>For now, check the serial console for CAN frame debugging information.</p>";
  html += "</div>";
  
  html += "<h3>Frame Types Monitored</h3>";
  html += "<table>";
  html += "<tr><td><strong>Frame 190:</strong></td><td>Basic battery data (voltage, current, SOC)</td></tr>";
  html += "<tr><td><strong>Frame 290:</strong></td><td>Temperature and status data</td></tr>";
  html += "<tr><td><strong>Frame 310:</strong></td><td>Cell voltage data</td></tr>";
  html += "<tr><td><strong>Frame 390:</strong></td><td>Cell temperature data</td></tr>";
  html += "<tr><td><strong>Frame 410:</strong></td><td>Protection status</td></tr>";
  html += "<tr><td><strong>Frame 490:</strong></td><td>Multiplexed extended data</td></tr>";
  html += "<tr><td><strong>Frame 510:</strong></td><td>System configuration</td></tr>";
  html += "<tr><td><strong>Frame 1B0:</strong></td><td>Alarm and warning status</td></tr>";
  html += "<tr><td><strong>Frame 710:</strong></td><td>Device information and serial number</td></tr>";
  html += "</table>";
  
  html += "</div>";
  html += JAVASCRIPT_CODE;
  html += "</body></html>";
  
  return html;
}

// Simplified implementations for other pages
String ConfigWebServer::generateWiFiConfigPage() {
  String html = "<!DOCTYPE html><html><head>";
  html += "<title>WiFi Configuration</title>";
  html += "<meta name='viewport' content='width=device-width, initial-scale=1'>";
  html += CSS_STYLE;
  html += "</head><body>";
  
  html += "<div class='container'>";
  html += "<div class='header'>";
  html += "<h1>WiFi Configuration</h1>";
  html += "<a href='/' style='color: #ecf0f1; text-decoration: none;'>← Back to Main</a>";
  html += "</div>";
  
  // Current status
  if (wifiManager.isConnected()) {
    html += "<div class='status success'>";
    html += "Currently connected to: <strong>" + wifiManager.getSSID() + "</strong><br>";
    html += "IP Address: " + wifiManager.getLocalIP() + "<br>";
    html += "Signal Strength: " + String(wifiManager.getRSSI()) + " dBm";
    html += "</div>";
  } else {
    html += "<div class='status error'>Not connected to WiFi</div>";
  }
  
  // WiFi Configuration Form
  html += "<form method='POST' action='/wifi/save'>";
  html += "<h2>WiFi Credentials</h2>";
  
  html += "<div class='form-group'>";
  html += "<label for='ssid'>Network Name (SSID):</label>";
  html += "<input type='text' id='ssid' name='ssid' value='" + String(systemConfig.wifiSSID) + "' maxlength='63' required>";
  html += "</div>";
  
  html += "<div class='form-group'>";
  html += "<label for='password'>Password:</label>";
  html += "<input type='password' id='password' name='password' value='" + String(systemConfig.wifiPassword) + "' maxlength='63'>";
  html += "</div>";
  
  html += "<button type='submit' class='btn'>Save WiFi Settings</button>";
  html += "</form>";
  
  html += "</div>";
  html += "</body></html>";
  
  return html;
}

String ConfigWebServer::generateNetworkConfigPage() {
  return "Network config page - TODO";
}

String ConfigWebServer::generateSystemStatusPage() {
  return "System status page - TODO";
}

// === REQUEST HANDLERS ===

void ConfigWebServer::handleRoot(AsyncWebServerRequest *request) {
  request->send(200, "text/html", generateMainPage());
}

void ConfigWebServer::handleWiFiConfig(AsyncWebServerRequest *request) {
  request->send(200, "text/html", generateWiFiConfigPage());
}

void ConfigWebServer::handleWiFiSave(AsyncWebServerRequest *request) {
  String ssid = "";
  String password = "";
  
  if (request->hasParam("ssid", true)) {
    ssid = request->getParam("ssid", true)->value();
  }
  
  if (request->hasParam("password", true)) {
    password = request->getParam("password", true)->value();
  }
  
  // Validate and save
  if (ssid.length() > 0 && ssid.length() <= 63 && password.length() <= 63) {
    strcpy(systemConfig.wifiSSID, ssid.c_str());
    strcpy(systemConfig.wifiPassword, password.c_str());
    
    if (saveConfiguration()) {
      request->send(200, "text/html", "WiFi configuration saved! <a href='/'>Back to main</a>");
    } else {
      request->send(500, "text/plain", "Failed to save configuration");
    }
  } else {
    request->send(400, "text/plain", "Invalid input parameters");
  }
}

void ConfigWebServer::handleNetworkConfig(AsyncWebServerRequest *request) {
  request->send(200, "text/html", generateNetworkConfigPage());
}

void ConfigWebServer::handleNetworkSave(AsyncWebServerRequest *request) {
  request->send(200, "text/html", "Network settings saved! <a href='/'>Back to main</a>");
}

void ConfigWebServer::handleBMSConfig(AsyncWebServerRequest *request) {
  request->send(200, "text/html", generateBMSConfigPage());
}

void ConfigWebServer::handleBMSSave(AsyncWebServerRequest *request) {
  int batteryCount = 0;
  uint8_t canSpeed = CAN_500KBPS; // Default to 500 kbps
  
  if (request->hasParam("battery_count", true)) {
    batteryCount = request->getParam("battery_count", true)->value().toInt();
  }
  
  if (request->hasParam("can_speed", true)) {
    String speedStr = request->getParam("can_speed", true)->value();
    canSpeed = (speedStr == "125") ? CAN_125KBPS : CAN_500KBPS;
  }
  
  // Validate battery count
  if (batteryCount < 1 || batteryCount > MAX_BMS_NODES) {
    request->send(400, "text/plain", "Invalid battery count");
    return;
  }
  
  // Extract battery IDs
  uint8_t newBmsIds[MAX_BMS_NODES];
  bool idsValid = true;
  
  for (int i = 0; i < batteryCount; i++) {
    String paramName = "battery_id_" + String(i + 1);
    if (request->hasParam(paramName, true)) {
      int id = request->getParam(paramName, true)->value().toInt();
      if (id < 1 || id > 16) {
        idsValid = false;
        break;
      }
      newBmsIds[i] = (uint8_t)id;
    } else {
      idsValid = false;
      break;
    }
  }
  
  if (!idsValid) {
    request->send(400, "text/plain", "Invalid battery IDs");
    return;
  }
  
  // Save configuration
  systemConfig.activeBmsNodes = batteryCount;
  systemConfig.canSpeed = canSpeed;
  for (int i = 0; i < batteryCount; i++) {
    systemConfig.bmsNodeIds[i] = newBmsIds[i];
  }
  
  if (saveConfiguration()) {
    String response = "BMS configuration saved!<br>";
    response += "Batteries: " + String(batteryCount) + "<br>";
    response += "CAN Speed: " + String(canSpeed == CAN_500KBPS ? "500" : "125") + " kbps<br>";
    response += "<a href='/bms'>Back to BMS config</a> | <a href='/'>Home</a>";
    request->send(200, "text/html", response);
  } else {
    request->send(500, "text/plain", "Failed to save configuration");
  }
}

void ConfigWebServer::handleCANMonitor(AsyncWebServerRequest *request) {
  request->send(200, "text/html", generateCANMonitorPage());
}

void ConfigWebServer::handleSystemStatus(AsyncWebServerRequest *request) {
  request->send(200, "text/html", generateSystemStatusPage());
}

void ConfigWebServer::handleConfigExport(AsyncWebServerRequest *request) {
  // Simple JSON export
  String json = "{";
  json += "\"device\":\"" + String(DEVICE_NAME) + "\",";
  json += "\"wifi_ssid\":\"" + String(systemConfig.wifiSSID) + "\",";
  json += "\"battery_count\":" + String(systemConfig.activeBmsNodes) + ",";
  json += "\"can_speed\":" + String(systemConfig.canSpeed) + "";
  json += "}";
  
  AsyncWebServerResponse *response = request->beginResponse(200, "application/json", json);
  response->addHeader("Content-Disposition", "attachment; filename=\"config.json\"");
  request->send(response);
}

void ConfigWebServer::handleRestart(AsyncWebServerRequest *request) {
  request->send(200, "text/html", "Device restarting... <script>setTimeout(function(){window.close();}, 3000);</script>");
  delay(1000);
  ESP.restart();
}

void ConfigWebServer::handleNotFound(AsyncWebServerRequest *request) {
  request->send(404, "text/html", "404 - Page Not Found<br><a href='/'>Home</a>");
}

// === UTILITY FUNCTIONS ===

bool ConfigWebServer::validateIPAddress(const String& ip) {
  return ip.length() > 6 && ip.length() < 16;
}

void ConfigWebServer::printStatus() const {
  Serial.println("=== WEB SERVER STATUS ===");
  Serial.printf("Running: %s\n", serverRunning ? "Yes" : "No");
  if (serverRunning) {
    Serial.printf("URL: http://%s:%d/\n", WiFi.softAPIP().toString().c_str(), WEB_SERVER_PORT);
  }
}

int ConfigWebServer::getClientCount() const {
  if (serverRunning && WiFi.getMode() == WIFI_AP) {
    return WiFi.softAPgetStationNum();
  }
  return 0;
}

// === GLOBAL FUNCTIONS ===

bool startConfigWebServer() {
  return configWebServer.begin();
}

void stopConfigWebServer() {
  configWebServer.end();
}

bool isConfigWebServerRunning() {
  return configWebServer.isRunning();
}

void processConfigWebServer() {
  // AsyncWebServer handles requests automatically
}