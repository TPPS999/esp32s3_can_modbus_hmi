/*
 * can_handler.h - ESP32S3 CAN to Modbus TCP Bridge CAN Handler
 * 
 * VERSION: v3.1.0
 * DATE: 2025-08-12
 * DESCRIPTION: Obsługa komunikacji CAN z modułami BMS
 */

#ifndef CAN_HANDLER_H
#define CAN_HANDLER_H

#include "config.h"
#include <mcp_can.h>
#include <SPI.h>

// === CAN STATUS ===
extern bool canStatus;
extern unsigned long lastCanFrame;
extern MCP_CAN CAN;

// === CAN STATISTICS ===
struct CanStats {
  unsigned long totalFramesReceived;
  unsigned long validBmsFrames;
  unsigned long invalidFrames;
  unsigned long parseErrors;
  unsigned long apTriggerFrames;
  unsigned long lastFrameTime;
  unsigned long communicationErrors;
  unsigned long initializationAttempts;
  bool lastInitSuccess;
};

extern CanStats canStats;

// === FUNCTION DECLARATIONS ===

// === Initialization ===
bool setupCAN();
bool initializeMCP2515();
bool resetCAN();
void configureSPIPins();

// === Main Processing ===
void processCAN();
bool checkCANMessage();
void handleCANFrame(unsigned long canId, unsigned char len, unsigned char* buf);

// === Frame Validation ===
bool isValidBMSFrame(unsigned long canId);
bool isValidFrameLength(unsigned char len);
bool isValidNodeId(uint8_t nodeId);

// === Frame Processing ===
void processAPTriggerFrame(unsigned long canId, unsigned char* buf);
void processBMSFrame(unsigned long canId, unsigned char len, unsigned char* buf);

// === Utility Functions ===
uint8_t extractNodeId(unsigned long canId, uint16_t baseId);
BMSFrameType_t getFrameType(unsigned long canId);
const char* getFrameTypeName(unsigned long canId);

// === Status and Diagnostics ===
void updateCANStatus();
void checkCANTimeout();
bool isCANHealthy();
void printCANStatus();
void printCANStatistics();
void resetCANStatistics();

// === Error Handling ===
void handleCANError();
void handleParseError(unsigned long canId, const char* reason);
bool recoverCAN();

// === Frame Type Detection ===
bool isFrame190(unsigned long canId);  // Basic data
bool isFrame290(unsigned long canId);  // Cell voltages
bool isFrame310(unsigned long canId);  // SOH/Temperature
bool isFrame390(unsigned long canId);  // Max limits
bool isFrame410(unsigned long canId);  // Temperatures
bool isFrame510(unsigned long canId);  // Power limits
bool isFrame490(unsigned long canId);  // Multiplexed
bool isFrame1B0(unsigned long canId);  // Additional
bool isFrame710(unsigned long canId);  // CANopen

// === CAN Speed Management ===
bool setCANSpeed(uint8_t speed);
uint8_t getCurrentCANSpeed();
const char* getCANSpeedString(uint8_t speed);

// === Frame Filtering ===
void setupCANFilters();
bool shouldProcessFrame(unsigned long canId);

// === Debug Functions ===
void enableCANDebug(bool enable);
void logCANFrame(unsigned long canId, unsigned char len, unsigned char* buf, bool incoming = true);
void dumpCANRegisters();

// === Constants ===
#define CAN_FRAME_MAX_AGE_MS 5000
#define CAN_ERROR_RECOVERY_ATTEMPTS 3
#define CAN_INIT_RETRY_DELAY_MS 1000
#define CAN_RESET_DELAY_MS 100

// === CAN Error Codes ===
typedef enum {
  CAN_ERROR_NONE = 0,
  CAN_ERROR_INIT_FAILED,
  CAN_ERROR_MODE_SET_FAILED,
  CAN_ERROR_COMMUNICATION_TIMEOUT,
  CAN_ERROR_FRAME_RECEIVE_FAILED,
  CAN_ERROR_INVALID_FRAME,
  CAN_ERROR_BUFFER_OVERFLOW
} CanError_t;

extern CanError_t lastCANError;

// === CAN State Machine ===
typedef enum {
  CAN_STATE_UNINITIALIZED,
  CAN_STATE_INITIALIZING,
  CAN_STATE_RUNNING,
  CAN_STATE_ERROR,
  CAN_STATE_RECOVERING
} CanState_t;

extern CanState_t canState;

// === Advanced Functions ===
void setCANState(CanState_t newState);
const char* getCANStateString(CanState_t state);
void performCANDiagnostics();
bool testCANLoopback();

#endif // CAN_HANDLER_H