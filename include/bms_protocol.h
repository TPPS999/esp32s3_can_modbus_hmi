/*
 * bms_protocol.h - ESP32S3 CAN to Modbus TCP Bridge BMS Protocol Header
 * 
 * VERSION: v4.0.0 - MODULAR ARCHITECTURE
 * DATE: 2025-08-12
 * DESCRIPTION: Interface for IFS BMS protocol parsing
 */

#ifndef BMS_PROTOCOL_H
#define BMS_PROTOCOL_H

#include "config.h"
#include "bms_data.h"

// === PROTOCOL CONSTANTS ===
#define MAX_CAN_FRAME_LENGTH 8
#define BMS_PROTOCOL_TIMEOUT_MS 30000

// === MAIN FRAME PROCESSING ===
void parseCANFrame(unsigned long canId, unsigned char len, unsigned char* buf);
uint8_t extractNodeId(unsigned long canId, uint16_t baseId);

// === FRAME PARSERS ===
void parseBMSFrame190(uint8_t nodeId, unsigned char* data);  // Basic data
void parseBMSFrame290(uint8_t nodeId, unsigned char* data);  // Cell voltages
void parseBMSFrame310(uint8_t nodeId, unsigned char* data);  // SOH/Temperature
void parseBMSFrame390(uint8_t nodeId, unsigned char* data);  // Max voltages
void parseBMSFrame410(uint8_t nodeId, unsigned char* data);  // Temperatures
void parseBMSFrame510(uint8_t nodeId, unsigned char* data);  // Power limits
void parseBMSFrame490(uint8_t nodeId, unsigned char* data);  // Multiplexed data
void parseBMSFrame1B0(uint8_t nodeId, unsigned char* data);  // Additional data
void parseBMSFrame710(uint8_t nodeId, unsigned char* data);  // CANopen status

// === FRAME VALIDATION ===
bool isValidBMSFrame(unsigned long canId);
bool validateFrameData(unsigned long canId, unsigned char len, unsigned char* buf);

// === FRAME TYPE DETECTION ===
bool isFrame190(unsigned long canId);  // Basic data
bool isFrame290(unsigned long canId);  // Cell voltages
bool isFrame310(unsigned long canId);  // SOH/Temperature
bool isFrame390(unsigned long canId);  // Max voltages
bool isFrame410(unsigned long canId);  // Temperatures
bool isFrame510(unsigned long canId);  // Power limits
bool isFrame490(unsigned long canId);  // Multiplexed
bool isFrame1B0(unsigned long canId);  // Additional
bool isFrame710(unsigned long canId);  // CANopen

// === UTILITY FUNCTIONS ===
const char* getFrameTypeName(unsigned long canId);
BMSFrameType_t getFrameType(unsigned long canId);
const char* getMux490TypeName(uint8_t type);

// === DIAGNOSTICS ===
void enableProtocolLogging(bool enable);
void printBMSProtocolStatistics();
void printBMSFrameDetails(uint8_t nodeId);

#endif // BMS_PROTOCOL_H