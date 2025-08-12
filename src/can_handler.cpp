/*
 * can_handler.cpp - ESP32S3 CAN to Modbus TCP Bridge CAN Handler Implementation
 * 
 * VERSION: v3.1.0
 * DATE: 2025-08-12
 */

#include "can_handler.h"
#include "bms_data.h"
#include "utils.h"

// === GLOBAL VARIABLES ===
bool canStatus = false;
unsigned long lastCanFrame = 0;
MCP_CAN CAN(SPI_CS_PIN);
CanStats canStats = {0};
CanError_t lastCANError = CAN_ERROR_NONE;
CanState_t canState = CAN_STATE_UNINITIALIZED;

// === PRIVATE VARIABLES ===
static bool canDebugEnabled = ENABLE_CAN_FRAME_LOGGING;
static unsigned long lastCANDiagnostics = 0;

// === INITIALIZATION ===

bool setupCAN() {
  DEBUG_PRINTLN("🔧 Setting up CAN communication...");
  
  setCANState(CAN_STATE_INITIALIZING);
  
  configureSPIPins();
  delay(CAN_RESET_DELAY_MS);
  
  bool success = initializeMCP2515();
  
  if (success) {
    setupCANFilters();
    resetCANStatistics();
    setCANState(CAN_STATE_RUNNING);
    DEBUG_PRINTLN("✅ CAN setup completed successfully");
  } else {
    setCANState(CAN_STATE_ERROR);
    DEBUG_PRINTLN("❌ CAN setup failed");
  }
  
  return success;
}

void configureSPIPins() {
  DEBUG_PRINTLN("🔧 Configuring SPI pins for CAN Expansion Board...");
  DEBUG_PRINTF("   MOSI: GPIO%d\n", SPI_MOSI_PIN);
  DEBUG_PRINTF("   MISO: GPIO%d\n", SPI_MISO_PIN);
  DEBUG_PRINTF("   SCK:  GPIO%d\n", SPI_SCK_PIN);
  DEBUG_PRINTF("   CS:   GPIO%d\n", SPI_CS_PIN);
  
  SPI.begin(SPI_SCK_PIN, SPI_MISO_PIN, SPI_MOSI_PIN, SPI_CS_PIN);
  DEBUG_PRINTLN("✅ SPI pins configured");
}

bool initializeMCP2515() {
  DEBUG_PRINTLN("🔧 Initializing MCP2515...");
  
  canStats.initializationAttempts++;
  
  if (CAN.begin(MCP_ANY, systemConfig.canSpeed, MCP_8MHZ) != CAN_OK) {
    DEBUG_PRINTLN("❌ MCP2515 initialization failed!");
    lastCANError = CAN_ERROR_INIT_FAILED;
    canStats.lastInitSuccess = false;
    return false;
  }
  
  if (CAN.setMode(MCP_NORMAL) != CAN_OK) {
    DEBUG_PRINTLN("❌ MCP2515 mode setting failed!");
    lastCANError = CAN_ERROR_MODE_SET_FAILED;
    canStats.lastInitSuccess = false;
    return false;
  }
  
  DEBUG_PRINTF("✅ MCP2515 initialized successfully (speed: %s)\n", 
               getCANSpeedString(systemConfig.canSpeed));
  
  canStatus = true;
  lastCANError = CAN_ERROR_NONE;
  canStats.lastInitSuccess = true;
  
  return true;
}

bool resetCAN() {
  DEBUG_PRINTLN("🔄 Resetting CAN controller...");
  
  setCANState(CAN_STATE_RECOVERING);
  
  CAN.reset();
  delay(CAN_RESET_DELAY_MS);
  
  bool success = initializeMCP2515();
  
  if (success) {
    setCANState(CAN_STATE_RUNNING);
    DEBUG_PRINTLN("✅ CAN reset successful");
  } else {
    setCANState(CAN_STATE_ERROR);
    DEBUG_PRINTLN("❌ CAN reset failed");
  }
  
  return success;
}

// === MAIN PROCESSING ===

void processCAN() {
  updateCANStatus();
  
  if (canState != CAN_STATE_RUNNING) {
    return;
  }
  
  while (checkCANMessage()) {
    // Process all available messages
  }
  
  checkCANTimeout();
  
  // Periodic diagnostics
  if (millis() - lastCANDiagnostics > 30000) { // Every 30 seconds
    performCANDiagnostics();
    lastCANDiagnostics = millis();
  }
}

bool checkCANMessage() {
  if (CAN_MSGAVAIL != CAN.checkReceive()) {
    return false;
  }
  
  unsigned char len = 0;
  unsigned char buf[8];
  unsigned long canId;
  
  if (CAN_OK != CAN.readMsgBuf(&len, buf)) {
    DEBUG_PRINTLN("❌ CAN read error!");
    canStats.parseErrors++;
    lastCANError = CAN_ERROR_FRAME_RECEIVE_FAILED;
    return false;
  }
  
  canId = CAN.getCanId();
  
  handleCANFrame(canId, len, buf);
  return true;
}

void handleCANFrame(unsigned long canId, unsigned char len, unsigned char* buf) {
  // Update statistics
  canStats.totalFramesReceived++;
  canStats.lastFrameTime = millis();
  lastCanFrame = millis();
  canStatus = true;
  
  // Flash LED to indicate activity
  setLED(true);
  
  // Log frame if debugging enabled
  if (canDebugEnabled) {
    logCANFrame(canId, len, buf, true);
  }
  
  // Check for AP trigger frame
  if (canId == AP_TRIGGER_CAN_ID) {
    processAPTriggerFrame(canId, buf);
    canStats.apTriggerFrames++;
  }
  // Process BMS frames
  else if (isValidBMSFrame(canId)) {
    if (isValidFrameLength(len)) {
      processBMSFrame(canId, len, buf);
      canStats.validBmsFrames++;
    } else {
      handleParseError(canId, "Invalid frame length");
    }
  } else {
    canStats.invalidFrames++;
  }
  
  setLED(false);
}

// === FRAME VALIDATION ===

bool isValidBMSFrame(unsigned long canId) {
  return isFrame190(canId) || isFrame290(canId) || isFrame310(canId) ||
         isFrame390(canId) || isFrame410(canId) || isFrame510(canId) ||
         isFrame490(canId) || isFrame1B0(canId) || isFrame710(canId);
}

bool isValidFrameLength(unsigned char len) {
  return len == MAX_CAN_FRAME_LENGTH;
}

bool isValidNodeId(uint8_t nodeId) {
  return getBatteryIndexFromNodeId(nodeId) != -1;
}

// === FRAME PROCESSING ===

void processAPTriggerFrame(unsigned long canId, unsigned char* buf) {
  if (buf[0] == AP_TRIGGER_DATA_0 && buf[1] == AP_TRIGGER_DATA_1) {
    DEBUG_PRINTLN("🎯 AP trigger frame detected!");
    // Trigger handled by wifi_manager module
  }
}

void processBMSFrame(unsigned long canId, unsigned char len, unsigned char* buf) {
  uint8_t nodeId = 0;
  BMSFrameType_t frameType;
  
  // Extract node ID based on frame type
  if (isFrame190(canId)) {
    nodeId = extractNodeId(canId, BMS_FRAME_190_BASE);
    frameType = BMS_FRAME_TYPE_190;
  } else if (isFrame290(canId)) {
    nodeId = extractNodeId(canId, BMS_FRAME_290_BASE);
    frameType = BMS_FRAME_TYPE_290;
  } else if (isFrame310(canId)) {
    nodeId = extractNodeId(canId, BMS_FRAME_310_BASE);
    frameType = BMS_FRAME_TYPE_310;
  } else if (isFrame390(canId)) {
    nodeId = extractNodeId(canId, BMS_FRAME_390_BASE);
    frameType = BMS_FRAME_TYPE_390;
  } else if (isFrame410(canId)) {
    nodeId = extractNodeId(canId, BMS_FRAME_410_BASE);
    frameType = BMS_FRAME_TYPE_410;
  } else if (isFrame510(canId)) {
    nodeId = extractNodeId(canId, BMS_FRAME_510_BASE);
    frameType = BMS_FRAME_TYPE_510;
  } else if (isFrame490(canId)) {
    nodeId = extractNodeId(canId, BMS_FRAME_490_BASE);
    frameType = BMS_FRAME_TYPE_490;
  } else if (isFrame1B0(canId)) {
    nodeId = extractNodeId(canId, BMS_FRAME_1B0_BASE);
    frameType = BMS_FRAME_TYPE_1B0;
  } else if (isFrame710(canId)) {
    nodeId = extractNodeId(canId, BMS_FRAME_710_BASE);
    frameType = BMS_FRAME_TYPE_710;
  } else {
    handleParseError(canId, "Unknown frame type");
    return;
  }
  
  if (!isValidNodeId(nodeId)) {
    handleParseError(canId, "Invalid node ID");
    return;
  }
  
  // Update frame timestamp
  updateFrameTimestamp(nodeId, frameType);
  
  // Frame will be parsed by bms_protocol module
  DEBUG_PRINTF("📦 Processing %s frame from BMS%d\n", 
               frameInfo[frameType].name, nodeId);
}

// === UTILITY FUNCTIONS ===

uint8_t extractNodeId(unsigned long canId, uint16_t baseId) {
  return (uint8_t)(canId - baseId);
}

BMSFrameType_t getFrameType(unsigned long canId) {
  if (isFrame190(canId)) return BMS_FRAME_TYPE_190;
  if (isFrame290(canId)) return BMS_FRAME_TYPE_290;
  if (isFrame310(canId)) return BMS_FRAME_TYPE_310;
  if (isFrame390(canId)) return BMS_FRAME_TYPE_390;
  if (isFrame410(canId)) return BMS_FRAME_TYPE_410;