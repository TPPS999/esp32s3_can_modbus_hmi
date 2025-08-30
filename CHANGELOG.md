# CHANGELOG - ESP32S3 CAN to Modbus TCP Bridge

## [2025-08-30] - CAN Communication Fix for Node 26

### 🚨 CRITICAL BUG FIXES
- **Fixed broken CAN frame routing mask (0xFF80)** that prevented ALL BMS frames from being processed
- **Replaced incorrect bitwise mask with proper range checks** in all CAN frame detection functions
- **Added proper CAN library dependency** (`limengdu/Arduino_CAN_BUS_MCP2515@^1.0.1`)

### 🔧 TECHNICAL CHANGES

#### platformio.ini
- Added `limengdu/Arduino_CAN_BUS_MCP2515@^1.0.1` library dependency
- This matches the working CAN implementation that successfully receives Node 26 frames

#### src/bms_protocol.cpp
- **CRITICAL FIX**: Replaced broken mask `(canId & 0xFF80) == CAN_FRAME_190_BASE` 
- **NEW LOGIC**: `canId >= CAN_FRAME_190_BASE && canId < CAN_FRAME_190_BASE + 32`
- **WHY BROKEN**: For Node 26 frame 0x19A: `0x19A & 0xFF80 = 0x180`, but `CAN_FRAME_190_BASE = 0x181` → NEVER MATCHED!

#### Functions Fixed:
- `parseCANFrame()` - Main frame routing logic 
- `processCANMessages()` - Frame type classification
- `isValidBMSFrame()` - Frame validation
- `initializeMCP2515()` - Added detailed hardware debug

#### Debug Enhancements:
- Added comprehensive hardware-level CAN debugging
- Periodic CAN controller status checks (every 5 seconds)
- Detailed `checkReceive()` and `readMsgBuf()` result logging
- Frame format identical to working implementation logs
- Expected CAN ID calculation debug for Node 26

### 🎯 EXPECTED RESULTS
With these fixes, Node 26 frames should now be properly:
- **Detected**: `🔍 CAN: ID=0x19A Len=8 Data=[92 0F FC FF...]`
- **Routed**: `190 NodeID=26` classification 
- **Parsed**: `📊 BMS26-190: V=249.12V I=4095.75A SOC=18.0%`
- **Processed**: Data updates in Modbus registers

### 🐛 ROOT CAUSE ANALYSIS
The previous implementation used a bitwise mask `0xFF80` that was mathematically incompatible with the CAN frame base addresses:
- Node 26, Frame 190: CAN ID `0x19A` (Expected: 0x181 + 26 - 1)
- Mask check: `0x19A & 0xFF80 = 0x180` 
- Base comparison: `0x180 == 0x181` → FALSE
- **Result**: ALL frames classified as UNKNOWN, no BMS data processed

The fix uses range checks that properly handle the Node ID calculation:
- `canId >= 0x181 && canId < 0x181 + 32` correctly matches 0x19A
- Node ID: `0x19A - 0x181 + 1 = 26` ✅

### 📊 TESTING CHECKLIST
- [ ] Build succeeds without errors
- [ ] CAN controller initialization debug appears
- [ ] `checkReceive()` results logged every 2 seconds  
- [ ] Frame detection shows `190 NodeID=26` for 0x19A
- [ ] BMS data parsing and Modbus register updates
- [ ] No more `TRIO HP LIMITS WARNING: BMS node 26 data is stale`

---

## Previous Releases
[Previous changelog entries would go here...]