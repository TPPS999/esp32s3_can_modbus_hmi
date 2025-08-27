# [API_NAME] API Documentation

> **Project:** ESP32S3 CAN to Modbus TCP Bridge  
> **API:** [API_DESCRIPTION]  
> **Version:** v[VERSION]  
> **Protocol:** [PROTOCOL_TYPE] ([HTTP/Modbus/CAN/Serial])  
> **Created:** [DATE] (Warsaw Time)  
> **Last Updated:** [DATE] (Warsaw Time)

## 📋 Overview

[DETAILED_API_OVERVIEW]

The [API_NAME] API provides [MAIN_FUNCTIONALITY] for the ESP32S3 CAN-Modbus TCP Bridge system. This API enables [USE_CASE] through [PROTOCOL_TYPE] protocol with [DATA_FORMAT] data format.

### Key Features

- ✅ **[FEATURE_1]**: [FEATURE_1_DESCRIPTION]
- ✅ **[FEATURE_2]**: [FEATURE_2_DESCRIPTION]
- ✅ **[FEATURE_3]**: [FEATURE_3_DESCRIPTION]
- ✅ **Error Handling**: Comprehensive error codes and responses
- ✅ **Authentication**: [AUTH_METHOD] authentication support
- ✅ **Rate Limiting**: Built-in request rate limiting

### Supported Operations

| Operation | Method/Function | Description |
|-----------|-----------------|-------------|
| [OPERATION_1] | `[METHOD_1]` | [DESCRIPTION_1] |
| [OPERATION_2] | `[METHOD_2]` | [DESCRIPTION_2] |
| [OPERATION_3] | `[METHOD_3]` | [DESCRIPTION_3] |

## 🔗 Connection Information

### Endpoint Base URL
```
[PROTOCOL]://[HOST]:[PORT]/[BASE_PATH]
```

**Example:**
```
http://192.168.1.100:80/api/v1
```

### Connection Parameters

| Parameter | Value | Description |
|-----------|-------|-------------|
| Host | `[DEFAULT_HOST]` | ESP32S3 device IP address |
| Port | `[DEFAULT_PORT]` | Service port number |
| Protocol | `[PROTOCOL]` | Communication protocol |
| Timeout | `[DEFAULT_TIMEOUT]ms` | Request timeout |

### Authentication

**Method:** [AUTH_METHOD]

**Headers Required:**
```http
[AUTH_HEADER]: [AUTH_VALUE]
Content-Type: [CONTENT_TYPE]
```

**Example:**
```http
X-API-Key: your-api-key-here
Content-Type: application/json
```

## 📚 API Reference

### System Operations

#### Get System Status
**Endpoint:** `GET /system/status`

**Description:** Retrieve current system status and health information.

**Request:**
```http
GET /api/v1/system/status HTTP/1.1
Host: 192.168.1.100
X-API-Key: your-api-key
```

**Response:**
```json
{
  "system_state": 2,
  "wifi_connected": true,
  "can_active": true,
  "modbus_active": true,
  "free_memory": 245760,
  "uptime": 3600000,
  "timestamp": 1692345678
}
```

**Response Fields:**

| Field | Type | Description |
|-------|------|-------------|
| `system_state` | `integer` | System state (0=Init, 1=Running, 2=Error) |
| `wifi_connected` | `boolean` | WiFi connection status |
| `can_active` | `boolean` | CAN bus status |
| `modbus_active` | `boolean` | Modbus server status |
| `free_memory` | `integer` | Available heap memory in bytes |
| `uptime` | `integer` | System uptime in milliseconds |
| `timestamp` | `integer` | Response timestamp |

#### Get System Information
**Endpoint:** `GET /system/info`

**Description:** Retrieve system hardware and firmware information.

**Request:**
```http
GET /api/v1/system/info HTTP/1.1
Host: 192.168.1.100
X-API-Key: your-api-key
```

**Response:**
```json
{
  "firmware_version": "4.0.1",
  "build_date": "Aug 27 2025 14:30:15",
  "device_name": "ESP32S3-CAN-MODBUS-TCP",
  "chip_model": "ESP32-S3",
  "chip_cores": 2,
  "cpu_frequency": 240,
  "flash_size": 8388608,
  "free_heap": 245760,
  "uptime": 3600000
}
```

#### Restart System
**Endpoint:** `POST /system/restart`

**Description:** Restart the ESP32S3 system.

**Request:**
```http
POST /api/v1/system/restart HTTP/1.1
Host: 192.168.1.100
X-API-Key: your-api-key
```

**Response:**
```json
{
  "success": true,
  "message": "System restart initiated",
  "timestamp": 1692345678
}
```

### Configuration Operations

#### Get Configuration
**Endpoint:** `GET /config`

**Description:** Retrieve current system configuration.

**Response:**
```json
{
  "config": {
    "can_speed": 125,
    "active_bms_nodes": 4,
    "modbus_port": 502,
    "modbus_slave_id": 1,
    "enable_can_filtering": true,
    "enable_modbus_write": true,
    "enable_wifi_ap": false,
    "heartbeat_interval": 60000,
    "communication_timeout": 30000,
    "bms_node_ids": [1, 2, 3, 4]
  }
}
```

#### Update Configuration
**Endpoint:** `PUT /config`

**Description:** Update system configuration parameters.

**Request:**
```http
PUT /api/v1/config HTTP/1.1
Host: 192.168.1.100
X-API-Key: your-api-key
Content-Type: application/json

{
  "can_speed": 500,
  "active_bms_nodes": 6,
  "bms_node_ids": [1, 2, 3, 4, 5, 6]
}
```

**Response:**
```json
{
  "success": true,
  "message": "Configuration updated successfully",
  "timestamp": 1692345678
}
```

### BMS Data Operations

#### List BMS Nodes
**Endpoint:** `GET /bms/nodes`

**Description:** Get list of active BMS nodes and their status.

**Response:**
```json
{
  "bms_nodes": [
    {
      "node_id": 1,
      "online": true,
      "status": {
        "voltage": 48.2,
        "current": 12.5,
        "soc": 85.5,
        "temperature": 23.4
      }
    },
    {
      "node_id": 2,
      "online": true,
      "status": {
        "voltage": 48.1,
        "current": 12.3,
        "soc": 84.2,
        "temperature": 24.1
      }
    }
  ],
  "total_nodes": 2,
  "timestamp": 1692345678
}
```

#### Get BMS Node Data
**Endpoint:** `GET /bms/nodes/{nodeId}/data`

**Description:** Get detailed data for a specific BMS node.

**Path Parameters:**

| Parameter | Type | Required | Description |
|-----------|------|----------|-------------|
| `nodeId` | `integer` | Yes | BMS node ID (1-16) |

**Request:**
```http
GET /api/v1/bms/nodes/1/data HTTP/1.1
Host: 192.168.1.100
X-API-Key: your-api-key
```

**Response:**
```json
{
  "node_id": 1,
  "online": true,
  "data": {
    "voltage": 48.25,
    "current": 12.48,
    "soc": 85.3,
    "soh": 98.2,
    "temperature": 23.4,
    "status": 0,
    "cell_voltages": [3.35, 3.36, 3.34, 3.35],
    "balancing_active": false,
    "charge_cycles": 127,
    "last_update": 1692345678
  },
  "timestamp": 1692345678
}
```

#### Send BMS Command
**Endpoint:** `POST /bms/nodes/{nodeId}/command`

**Description:** Send a command to a specific BMS node.

**Request:**
```http
POST /api/v1/bms/nodes/1/command HTTP/1.1
Host: 192.168.1.100
X-API-Key: your-api-key
Content-Type: application/json

{
  "command": "reset",
  "parameters": {
    "reason": "user_requested"
  }
}
```

**Supported Commands:**

| Command | Parameters | Description |
|---------|------------|-------------|
| `reset` | `reason` (string) | Reset BMS node |
| `calibrate` | `type` (string) | Calibrate sensors |
| `balance_start` | `cells` (array) | Start cell balancing |
| `balance_stop` | None | Stop cell balancing |

**Response:**
```json
{
  "success": true,
  "message": "Command executed successfully",
  "timestamp": 1692345678
}
```

### Statistics Operations

#### Get Statistics
**Endpoint:** `GET /statistics`

**Description:** Retrieve system statistics and counters.

**Response:**
```json
{
  "request_count": 1247,
  "error_count": 3,
  "auth_failures": 1,
  "last_request_time": 1692345678,
  "uptime": 3600000,
  "can_stats": {
    "frames_received": 15420,
    "frames_sent": 892,
    "error_frames": 2,
    "bus_load": 15.3
  },
  "modbus_stats": {
    "connections": 4,
    "register_reads": 2341,
    "register_writes": 156,
    "errors": 1
  },
  "timestamp": 1692345678
}
```

#### Reset Statistics
**Endpoint:** `DELETE /statistics`

**Description:** Reset all statistics counters to zero.

**Response:**
```json
{
  "success": true,
  "message": "Statistics reset successfully",
  "timestamp": 1692345678
}
```

### Diagnostic Operations

#### CAN Bus Diagnostics
**Endpoint:** `GET /diagnostics/can`

**Description:** Get detailed CAN bus diagnostic information.

**Response:**
```json
{
  "can_diagnostics": {
    "bus_state": "active",
    "error_count": 2,
    "frames_received": 15420,
    "frames_sent": 892,
    "bus_load": 15.3,
    "last_frame_time": 1692345675,
    "controller_status": "normal"
  },
  "timestamp": 1692345678
}
```

#### Modbus Diagnostics
**Endpoint:** `GET /diagnostics/modbus`

**Description:** Get detailed Modbus server diagnostic information.

**Response:**
```json
{
  "modbus_diagnostics": {
    "server_state": "running",
    "client_connections": 2,
    "register_reads": 2341,
    "register_writes": 156,
    "error_count": 1,
    "last_request_time": 1692345677
  },
  "timestamp": 1692345678
}
```

## 🔧 Data Types and Formats

### Common Data Types

| Type | Format | Example | Description |
|------|--------|---------|-------------|
| Timestamp | Unix timestamp (ms) | `1692345678000` | Milliseconds since epoch |
| Node ID | Integer (1-16) | `1` | BMS node identifier |
| Voltage | Float (V) | `48.25` | Voltage in volts |
| Current | Float (A) | `12.48` | Current in amperes |
| Temperature | Float (°C) | `23.4` | Temperature in Celsius |
| SOC | Float (%) | `85.3` | State of charge percentage |
| SOH | Float (%) | `98.2` | State of health percentage |

### Status Codes

| Code | Description | Recovery Action |
|------|-------------|-----------------|
| `0` | Normal | None |
| `1` | Warning | Monitor closely |
| `2` | Error | Check system |
| `3` | Critical | Immediate action required |

### Error Response Format

All API endpoints return errors in a consistent format:

```json
{
  "error": "ERROR_CODE",
  "message": "Human readable error description",
  "timestamp": 1692345678,
  "details": {
    "parameter": "invalid_value",
    "expected": "valid_range"
  }
}
```

## 📝 Usage Examples

### Basic Authentication and Status Check

```python
import requests
import json

# Configuration
API_BASE = "http://192.168.1.100/api/v1"
API_KEY = "your-api-key-here"

headers = {
    "X-API-Key": API_KEY,
    "Content-Type": "application/json"
}

# Get system status
response = requests.get(f"{API_BASE}/system/status", headers=headers)
if response.status_code == 200:
    status = response.json()
    print(f"System uptime: {status['uptime']} ms")
    print(f"Free memory: {status['free_memory']} bytes")
else:
    print(f"Error: {response.status_code} - {response.text}")
```

### BMS Data Collection

```python
# Get all BMS nodes
response = requests.get(f"{API_BASE}/bms/nodes", headers=headers)
nodes = response.json()["bms_nodes"]

# Collect data from each node
bms_data = {}
for node in nodes:
    node_id = node["node_id"]
    response = requests.get(f"{API_BASE}/bms/nodes/{node_id}/data", headers=headers)
    if response.status_code == 200:
        bms_data[node_id] = response.json()["data"]
        print(f"Node {node_id}: SOC={bms_data[node_id]['soc']:.1f}%")
```

### Configuration Update

```python
# Update CAN speed and BMS nodes
new_config = {
    "can_speed": 500,
    "active_bms_nodes": 6,
    "bms_node_ids": [1, 2, 3, 4, 5, 6]
}

response = requests.put(f"{API_BASE}/config", 
                       headers=headers, 
                       json=new_config)

if response.status_code == 200:
    print("Configuration updated successfully")
    print("System will apply changes on next restart")
else:
    error = response.json()
    print(f"Configuration error: {error['message']}")
```

### WebSocket Connection (if supported)

```javascript
// Real-time data streaming
const ws = new WebSocket('ws://192.168.1.100/ws');

ws.onopen = function() {
    console.log('WebSocket connected');
    // Subscribe to BMS data updates
    ws.send(JSON.stringify({
        action: 'subscribe',
        topic: 'bms_data',
        api_key: 'your-api-key-here'
    }));
};

ws.onmessage = function(event) {
    const data = JSON.parse(event.data);
    console.log('BMS Update:', data);
};
```

## ⚠️ Error Handling

### HTTP Status Codes

| Code | Description | Common Causes |
|------|-------------|---------------|
| `200` | OK | Request successful |
| `400` | Bad Request | Invalid parameters or JSON |
| `401` | Unauthorized | Missing or invalid API key |
| `404` | Not Found | Invalid endpoint or resource |
| `429` | Too Many Requests | Rate limit exceeded |
| `500` | Internal Server Error | System error or exception |

### API-Specific Error Codes

| Error Code | HTTP Status | Description | Solution |
|------------|-------------|-------------|----------|
| `INVALID_NODE_ID` | 400 | Node ID out of range (1-16) | Use valid node ID |
| `NODE_OFFLINE` | 404 | BMS node not responding | Check CAN connection |
| `CONFIG_VALIDATION_FAILED` | 400 | Invalid configuration values | Check parameter ranges |
| `COMMAND_NOT_SUPPORTED` | 400 | BMS command not supported | Check supported commands |
| `SYSTEM_BUSY` | 503 | System temporarily unavailable | Retry after delay |

### Rate Limiting

The API implements rate limiting to prevent system overload:

- **Default Limit**: 100 requests per minute per API key
- **Burst Limit**: 10 requests per second
- **Headers**: Response includes `X-RateLimit-*` headers

**Rate Limit Headers:**
```http
X-RateLimit-Limit: 100
X-RateLimit-Remaining: 95
X-RateLimit-Reset: 1692345738
```

## 🧪 Testing

### API Testing Tools

**cURL Examples:**
```bash
# Get system status
curl -X GET "http://192.168.1.100/api/v1/system/status" \
     -H "X-API-Key: your-api-key"

# Update configuration
curl -X PUT "http://192.168.1.100/api/v1/config" \
     -H "X-API-Key: your-api-key" \
     -H "Content-Type: application/json" \
     -d '{"can_speed": 500}'

# Get BMS data
curl -X GET "http://192.168.1.100/api/v1/bms/nodes/1/data" \
     -H "X-API-Key: your-api-key"
```

**Postman Collection:**
A Postman collection with all API endpoints is available at:
`docs/postman/[API_NAME]-collection.json`

### Test Scripts

**Python Test Script:**
```python
#!/usr/bin/env python3
import requests
import sys

def test_api():
    base_url = "http://192.168.1.100/api/v1"
    headers = {"X-API-Key": "test-key"}
    
    # Test system status
    response = requests.get(f"{base_url}/system/status", headers=headers)
    assert response.status_code == 200
    
    # Test BMS nodes
    response = requests.get(f"{base_url}/bms/nodes", headers=headers)
    assert response.status_code == 200
    
    print("All tests passed!")

if __name__ == "__main__":
    test_api()
```

## 📊 Performance Guidelines

### Request Optimization

- **Batch Requests**: Group related operations when possible
- **Caching**: Cache configuration data locally
- **Polling Frequency**: Limit BMS data polling to necessary intervals
- **Compression**: Enable gzip compression for large responses

### Best Practices

1. **Connection Pooling**: Reuse HTTP connections
2. **Timeout Handling**: Set appropriate timeouts (5-10 seconds)
3. **Retry Logic**: Implement exponential backoff for retries
4. **Error Logging**: Log all API errors for troubleshooting
5. **Rate Limiting**: Respect rate limits to avoid blocking

### Performance Benchmarks

| Operation | Average Response Time | Max Response Time |
|-----------|----------------------|-------------------|
| System Status | 15ms | 50ms |
| BMS Data (single node) | 25ms | 100ms |
| Configuration Update | 100ms | 500ms |
| Statistics Query | 20ms | 80ms |

## 📝 Change Log

### v[VERSION] - [DATE]
- 🆕 Initial API implementation
- ✅ All basic endpoints functional
- ✅ Authentication system implemented
- ✅ Error handling complete

### Future Versions
- 🔄 WebSocket support for real-time updates
- 🆕 GraphQL endpoint for flexible queries
- 🔧 Enhanced filtering and pagination
- 🛡️ OAuth2 authentication support

## 🔗 Related Documentation

- [System Architecture](../ARCHITECTURE.md)
- [Setup Guide](../SETUP.md)
- [Module Documentation](../modules/)
- [Troubleshooting Guide](troubleshooting-template.md)

---

**Template Usage Instructions:**

To use this API documentation template:
1. Replace all [PLACEHOLDER] tokens with actual values
2. Update endpoint URLs and methods
3. Add real request/response examples
4. Document all error codes and responses
5. Include actual performance measurements
6. Add authentication and security details
7. Provide working code examples
8. Remove this instruction section

Key areas to customize:
- API endpoints and methods
- Request/response schemas
- Authentication mechanisms
- Error codes and handling
- Rate limiting rules
- Performance characteristics
- Testing procedures and examples