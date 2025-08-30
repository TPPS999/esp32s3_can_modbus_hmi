# DEBUG SESSION LOG - Node 26 CAN Communication

## Session Date: 2025-08-30

### 🚨 PROBLEM DESCRIPTION
Node 26 BMS communication completely broken:
- **Symptom**: No CAN frames being received/processed
- **Working code**: Shows `🔍 CAN: ID=0x19A Len=8 Data=[92 0F FC FF...]` and proper BMS data parsing
- **Broken code**: Only shows `[TRIO HP LIMITS] WARNING: BMS node 26 data is stale` errors
- **Evidence**: User provided `dzialajacy can.md` with side-by-side comparison

### 🔍 ANALYSIS PROCESS

#### Step 1: CAN Configuration Comparison
- **Working**: Uses `limengdu/Arduino_CAN_BUS_MCP2515` library
- **Current**: Uses local `mcp_can.h/cpp` files
- **Assessment**: Library difference likely significant

#### Step 2: Frame Routing Logic Investigation
- **Discovery**: `parseCANFrame()` uses bitwise mask `(canId & 0xFF80)`
- **Problem**: For Node 26 frame 0x19A:
  - `0x19A & 0xFF80 = 0x180` 
  - `CAN_FRAME_190_BASE = 0x181`
  - `0x180 != 0x181` → NEVER MATCHES!
- **Impact**: ALL BMS frames classified as UNKNOWN

#### Step 3: Affected Functions Audit
Found identical broken mask pattern in:
- `parseCANFrame()` - main routing
- `processCANMessages()` - frame classification  
- `isValidBMSFrame()` - validation
- Debug output functions

### 🛠️ IMPLEMENTED FIXES

#### Fix 1: Library Dependency
```ini
# platformio.ini
lib_deps = 
    limengdu/Arduino_CAN_BUS_MCP2515@^1.0.1  # ADDED
```

#### Fix 2: Frame Routing Logic
```cpp
// BEFORE (BROKEN):
if ((canId & 0xFF80) == CAN_FRAME_190_BASE)

// AFTER (FIXED):  
if (canId >= CAN_FRAME_190_BASE && canId < CAN_FRAME_190_BASE + 32)
```

#### Fix 3: Enhanced Hardware Debug
- Added periodic CAN controller status logging
- Detailed `checkReceive()` and `readMsgBuf()` diagnostics
- Frame format matching working implementation
- Expected CAN ID calculation for Node 26

### 📊 MATHEMATICAL PROOF OF FIX

#### Node 26 Frame 190 Analysis:
- **Expected CAN ID**: `0x181 + 26 - 1 = 0x19A` ✅
- **Old mask check**: `0x19A & 0xFF80 = 0x180 != 0x181` ❌  
- **New range check**: `0x19A >= 0x181 && 0x19A < 0x1A1` ✅
- **Node ID extraction**: `0x19A - 0x181 + 1 = 26` ✅

### 🎯 EXPECTED RESOLUTION

After fixes, Node 26 should show:
1. **CAN Controller Init**: Debug messages with expected CAN IDs
2. **Frame Detection**: `📥 CAN RX: ID=0x19A Type=190 NodeID=26`
3. **Frame Processing**: `🔍 CAN: ID=0x19A Len=8 Data=[...] (Basic data)`
4. **BMS Data Parsing**: `📊 BMS26-190: V=249.12V I=4095.75A SOC=18.0%`
5. **Status Resolution**: No more "data is stale" warnings

### 📋 VERIFICATION CHECKLIST
- [ ] Compilation successful
- [ ] CAN library downloaded and linked
- [ ] Hardware initialization debug appears
- [ ] Frame routing debug shows correct classification
- [ ] BMS data appears in logs
- [ ] Modbus registers updated with BMS data
- [ ] System health status shows BMS communication active

---

## Root Cause Summary
**Primary**: Bitwise mask `0xFF80` mathematically incompatible with CAN frame base addresses
**Secondary**: Wrong CAN library (local vs. proven working library)
**Impact**: 100% frame loss - no BMS data processed despite valid CAN bus traffic
**Fix Confidence**: 95% - critical logic error corrected, proven library added