# Debugging Session Template

> **Template for:** Systematic Debugging and Troubleshooting  
> **Target:** ESP32S3 CAN to Modbus TCP Bridge Issues  
> **Version:** 1.0 - Universal Workflow Phase 5  
> **Usage:** Follow this workflow when encountering system issues  

## 🐛 Debug Session Initialization

### **Problem Definition**
- [ ] **Issue Description:** [DETAILED_PROBLEM_DESCRIPTION]
- [ ] **Symptom Category:** [ ] Build | [ ] Runtime | [ ] Communication | [ ] Performance
- [ ] **Reproducibility:** [ ] Always | [ ] Intermittent | [ ] Specific Conditions
- [ ] **Impact Level:** [ ] Critical | [ ] High | [ ] Medium | [ ] Low
- [ ] **First Occurrence:** [WHEN_ISSUE_FIRST_NOTICED]

### **Environment Snapshot**
- [ ] **Git Commit:** `git log --oneline -1`
- [ ] **Build Status:** `pio run -e esp32s3dev`
- [ ] **Hardware Setup:** [CAN_TRANSCEIVER/ETHERNET_MODULE_STATUS]
- [ ] **Serial Monitor:** `pio device monitor --baud 115200`
- [ ] **System Resources:** `pio run -t size`

## 🔍 Systematic Debug Workflow

### **Phase 1: Information Gathering (15-20 minutes)**

#### **Code Analysis**
```bash
# Recent changes analysis
git log --oneline -10
git diff HEAD~3..HEAD

# Affected file identification  
find src/ include/ -name "*.h" -o -name "*.cpp" | xargs grep -l "[ERROR_KEYWORD]"

# Build warnings check
pio run -e esp32s3dev 2>&1 | grep -i warning
```

#### **Runtime Data Collection**
- [ ] Enable verbose debugging: `#define DEBUG_ENABLED 1`
- [ ] Add strategic debug prints in suspected problem areas
- [ ] Capture serial output: `pio device monitor > debug_output.log`
- [ ] Monitor memory usage and task states
- [ ] Record timing of issue occurrence

### **Phase 2: Hypothesis Formation (10-15 minutes)**

#### **Common ESP32S3 CAN-Modbus Issues**
- [ ] **CAN Bus Communication:** Frame corruption, timing issues, bus-off state
- [ ] **Modbus TCP Processing:** Register mapping errors, connection drops, timeout issues  
- [ ] **Memory Management:** Stack overflow, heap fragmentation, memory leaks
- [ ] **Task Synchronization:** Race conditions, deadlocks, priority inversion
- [ ] **Ethernet Connectivity:** PHY initialization, DHCP issues, packet loss

#### **Hypothesis Prioritization**
1. **Primary Hypothesis:** [MOST_LIKELY_CAUSE_WITH_REASONING]
2. **Secondary Hypothesis:** [SECOND_MOST_LIKELY_CAUSE]
3. **Tertiary Hypothesis:** [LESS_LIKELY_BUT_POSSIBLE_CAUSE]

### **Phase 3: Systematic Testing (30-45 minutes)**

#### **Hardware Verification**
```bash
# CAN bus health check
pio device monitor --filter esp32_exception_decoder

# Test commands to add to code:
// CAN_controller_status_check()
// ModBus_TCP_connection_test()  
// Memory_stack_watermark_check()
```

#### **Software Testing Sequence**
1. **Minimal Configuration Test**
   - [ ] Disable all non-essential features
   - [ ] Test basic CAN receive/transmit
   - [ ] Verify Modbus TCP socket creation
   - [ ] Check core system stability

2. **Component Isolation Testing**
   - [ ] Test CAN subsystem independently: `#define ISOLATE_CAN_TESTING`
   - [ ] Test Modbus subsystem independently: `#define ISOLATE_MODBUS_TESTING`
   - [ ] Test Ethernet subsystem independently: `#define ISOLATE_ETHERNET_TESTING`

3. **Progressive Feature Re-enablement**
   - [ ] Add features back one at a time
   - [ ] Test stability after each addition
   - [ ] Monitor resource usage throughout process

### **Phase 4: Issue Resolution (20-30 minutes)**

#### **Fix Implementation Strategy**
- [ ] **Implement Minimal Fix:** Address root cause with smallest change
- [ ] **Add Defensive Measures:** Error handling, bounds checking, timeouts
- [ ] **Update Configuration:** Adjust buffer sizes, timing parameters, priorities
- [ ] **Document Solution:** Code comments explaining fix reasoning

#### **Resolution Verification**
```bash
# Comprehensive testing after fix
pio run -t clean && pio run -e esp32s3dev
pio device monitor --baud 115200

# Extended runtime test (let run for 10+ minutes)
# Monitor for issue recurrence
```

## 🛠️ ESP32S3-Specific Debug Tools

### **Built-in Diagnostics**
```cpp
// Memory diagnostics
Serial.printf("Free heap: %u bytes\n", esp_get_free_heap_size());
Serial.printf("Min free heap: %u bytes\n", esp_get_minimum_free_heap_size());

// Task monitoring
vTaskList(taskList);
Serial.print("Task Name\tState\tPrio\tStack\tNum\n");
Serial.print(taskList);

// Reset reason analysis
esp_reset_reason_t reset_reason = esp_reset_reason();
Serial.printf("Reset reason: %d\n", reset_reason);
```

### **CAN Bus Debug Commands**
```cpp
// CAN controller status
can_status_info_t can_status;
can_get_status_info(&can_status);
Serial.printf("CAN State: %d, RX Errors: %d, TX Errors: %d\n", 
              can_status.state, can_status.rx_error_counter, can_status.tx_error_counter);

// Message queue status  
Serial.printf("CAN RX Queue: %d messages pending\n", uxQueueMessagesWaiting(can_rx_queue));
```

### **Modbus Debug Information**
```cpp
// Connection status
Serial.printf("TCP Clients Connected: %d\n", active_tcp_clients);
Serial.printf("Register Access Errors: %d\n", modbus_register_errors);

// Traffic analysis
Serial.printf("Modbus Requests: %lu, Responses: %lu\n", 
              modbus_request_count, modbus_response_count);
```

## 📊 Debug Session Documentation

### **Issue Tracking Format**
```markdown
## Debug Session - [DATE] [TIME]
**Issue:** [PROBLEM_SUMMARY]
**Duration:** [START_TIME] - [END_TIME] ([TOTAL_MINUTES] minutes)

### Problem Analysis
- **Root Cause:** [IDENTIFIED_CAUSE]
- **Contributing Factors:** [ADDITIONAL_FACTORS]
- **Impact Assessment:** [WHAT_WAS_AFFECTED]

### Solution Implemented
- **Fix Description:** [WHAT_WAS_CHANGED]
- **Files Modified:** [LIST_WITH_LINE_COUNTS]
- **Configuration Changes:** [CONFIG_UPDATES]

### Prevention Measures
- **Code Improvements:** [DEFENSIVE_PROGRAMMING_ADDED]
- **Testing Enhancements:** [NEW_TEST_CASES]
- **Monitoring Added:** [DEBUG_INSTRUMENTATION]

### Verification Results  
- **Fix Confirmed:** [ ] Yes [ ] Partial [ ] No
- **Side Effects:** [ANY_UNINTENDED_CONSEQUENCES]
- **Long-term Testing:** [EXTENDED_RUNTIME_RESULTS]
```

## 🚨 Emergency Debug Procedures

### **System Crash Recovery**
1. **Boot Loop Analysis:** Check reset reason, disable watchdog temporarily
2. **Stack Overflow:** Increase task stack sizes, check recursion
3. **Memory Corruption:** Enable heap poisoning, check buffer overruns
4. **Hardware Failure:** Test on different board, check power supply

### **Communication Failures**  
1. **CAN Bus Silent:** Check transceiver power, verify wiring, test termination
2. **Modbus No Response:** Verify IP configuration, check firewall, test with tools
3. **Ethernet Down:** Check PHY registers, test cable, verify switch configuration

### **Performance Degradation**
1. **Slow Response:** Profile task execution times, check CPU utilization
2. **Memory Leaks:** Monitor heap over time, check for unreleased resources
3. **Timing Issues:** Verify interrupt priorities, check task scheduling

## 💡 Debug Session Best Practices

### **Efficient Debugging**
- Start with most likely causes based on symptoms
- Use binary search approach: disable half of functionality, then narrow down
- Keep detailed notes of what was tried and results
- Take screenshots/photos of oscilloscope traces if using hardware debugging

### **Avoid Common Pitfalls**
- Don't change multiple things at once
- Always test the simplest case first
- Don't assume hardware is working correctly  
- Verify assumptions with actual measurements

### **Knowledge Building**
- Document recurring issues and their solutions
- Build a database of symptoms → root cause mappings
- Share debugging knowledge with team
- Update this template based on new debugging experiences

---

**Usage Instructions:**
1. Use this template when systematic debugging is needed
2. Follow phases in order - don't skip information gathering
3. Document findings even if issue isn't resolved in current session
4. Update project-specific debug tools section based on your system
5. Integrate findings into preventive measures for future development

**Integration Notes:**
- Works with session-startup-template.md for complete session workflow
- Supports TodoWrite task tracking during debugging sessions
- Compatible with Universal Workflow git commit standards
- Enables systematic knowledge capture and sharing