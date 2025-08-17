/*
 * web_server.h - ESP32S3 CAN to Modbus TCP Bridge - Web Configuration Server
 * 
 * VERSION: v1.0.0
 * DATE: 2025-08-17
 * 
 * Provides web interface for configuration in AP mode:
 * - WiFi credentials setup
 * - IP configuration (DHCP/Static)
 * - BMS battery count and ID assignment
 * - System configuration export/import
 */

#ifndef WEB_SERVER_H
#define WEB_SERVER_H

#include <Arduino.h>
#include <ESPAsyncWebServer.h>
#include <AsyncTCP.h>
#include "config.h"

// === WEB SERVER CONFIGURATION ===
#define WEB_SERVER_PORT 80
#define CONFIG_JSON_MAX_SIZE 2048

// === WEB SERVER CLASS ===
class ConfigWebServer {
private:
  AsyncWebServer* server;
  bool serverRunning;
  
  // HTML page generators
  String generateMainPage();
  String generateWiFiConfigPage();
  String generateNetworkConfigPage();
  String generateBMSConfigPage();
  String generateCANMonitorPage();
  String generateSystemStatusPage();
  
  // Request handlers
  void handleRoot(AsyncWebServerRequest *request);
  void handleWiFiConfig(AsyncWebServerRequest *request);
  void handleWiFiSave(AsyncWebServerRequest *request);
  void handleNetworkConfig(AsyncWebServerRequest *request);
  void handleNetworkSave(AsyncWebServerRequest *request);
  void handleBMSConfig(AsyncWebServerRequest *request);
  void handleBMSSave(AsyncWebServerRequest *request);
  void handleCANMonitor(AsyncWebServerRequest *request);
  void handleSystemStatus(AsyncWebServerRequest *request);
  void handleConfigExport(AsyncWebServerRequest *request);
  void handleConfigImport(AsyncWebServerRequest *request);
  void handleRestart(AsyncWebServerRequest *request);
  void handleNotFound(AsyncWebServerRequest *request);
  
  // Utility functions
  String getContentType(String filename);
  bool validateIPAddress(const String& ip);
  bool validateBMSConfiguration(int count, const String& ids);
  
public:
  ConfigWebServer();
  ~ConfigWebServer();
  
  // Lifecycle
  bool begin();
  void end();
  bool isRunning() const { return serverRunning; }
  
  // Status
  void printStatus() const;
  int getClientCount() const;
};

// === GLOBAL INSTANCE ===
extern ConfigWebServer configWebServer;

// === FUNCTION DECLARATIONS ===
bool startConfigWebServer();
void stopConfigWebServer();
bool isConfigWebServerRunning();
void processConfigWebServer();

// === HTML COMPONENTS ===

// CSS Styles
const char* const CSS_STYLE = R"(
<style>
body { font-family: Arial, sans-serif; margin: 0; padding: 20px; background: #f0f0f0; }
.container { max-width: 800px; margin: 0 auto; background: white; padding: 20px; border-radius: 8px; box-shadow: 0 2px 10px rgba(0,0,0,0.1); }
.header { background: #2c3e50; color: white; padding: 15px; margin: -20px -20px 20px -20px; border-radius: 8px 8px 0 0; }
.nav { margin: 20px 0; }
.nav a { display: inline-block; padding: 10px 15px; margin: 5px; background: #3498db; color: white; text-decoration: none; border-radius: 4px; }
.nav a:hover { background: #2980b9; }
.form-group { margin: 15px 0; }
.form-group label { display: block; margin-bottom: 5px; font-weight: bold; }
.form-group input, .form-group select { width: 100%; padding: 8px; border: 1px solid #ddd; border-radius: 4px; box-sizing: border-box; }
.btn { padding: 10px 20px; background: #27ae60; color: white; border: none; border-radius: 4px; cursor: pointer; margin: 5px; }
.btn:hover { background: #229954; }
.btn-danger { background: #e74c3c; }
.btn-danger:hover { background: #c0392b; }
.status { padding: 10px; margin: 10px 0; border-radius: 4px; }
.status.success { background: #d4edda; color: #155724; border: 1px solid #c3e6cb; }
.status.error { background: #f8d7da; color: #721c24; border: 1px solid #f5c6cb; }
.status.info { background: #d1ecf1; color: #0c5460; border: 1px solid #bee5eb; }
table { width: 100%; border-collapse: collapse; margin: 10px 0; }
table th, table td { padding: 8px; text-align: left; border-bottom: 1px solid #ddd; }
table th { background-color: #f8f9fa; }
.grid { display: grid; grid-template-columns: 1fr 1fr; gap: 20px; }
@media (max-width: 600px) { .grid { grid-template-columns: 1fr; } }
</style>
)";

// JavaScript - Simple version without complex syntax
#define JAVASCRIPT_CODE "<script>\n" \
"function toggleIPConfig() {\n" \
"  var dhcp = document.getElementById('use_dhcp').checked;\n" \
"  var staticConfig = document.getElementById('static_config');\n" \
"  staticConfig.style.display = dhcp ? 'none' : 'block';\n" \
"}\n" \
"function confirmRestart() {\n" \
"  return confirm('Are you sure you want to restart the device?');\n" \
"}\n" \
"function refreshStatus() {\n" \
"  location.reload();\n" \
"}\n" \
"</script>"

#endif // WEB_SERVER_H