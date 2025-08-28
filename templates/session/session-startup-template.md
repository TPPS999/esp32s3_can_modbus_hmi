# Session Startup Template

> **Template for:** Development Session Initialization  
> **Target:** ESP32S3 CAN to Modbus TCP Bridge Project  
> **Version:** 1.0 - Universal Workflow Phase 5  
> **Usage:** Copy and customize for each new development session  

## 📋 Pre-Session Checklist

### **Environment Setup**
- [ ] Navigate to project directory: `cd "[PROJECT_PATH]"`
- [ ] Verify git status: `git status` (should be clean)
- [ ] Check current branch: `git branch -v`
- [ ] Review last 3 commits: `git log --oneline -3`
- [ ] Confirm PlatformIO environment ready: `pio --version`

### **Project Status Verification**
- [ ] Check build status: `pio run -e esp32s3dev`
- [ ] Verify serial monitor connection: `pio device list`
- [ ] Test hardware connections: CAN transceiver, Ethernet module
- [ ] Review system memory: `pio run -t size`
- [ ] Check dependencies: `pio pkg list`

### **Session Planning**
- [ ] Read `NEXT_SESSION_START.md` for priorities
- [ ] Review `DEVELOPMENT_PROGRESS_LOG.md` recent entries
- [ ] Check open issues in project tracking system
- [ ] Identify session goals and time allocation
- [ ] Set up TodoWrite task list for session tracking

## 🚀 Session Initialization Commands

### **Quick Start Commands**
```bash
# Core status check
cd "[PROJECT_PATH]"
git status && git log --oneline -3
git branch -v

# Project health check  
pio run -e esp32s3dev --target clean
pio run -e esp32s3dev
pio device list

# Development environment verification
ls -la include/ src/ lib/
find . -name "*.h" -o -name "*.cpp" | head -10
```

### **Advanced Diagnostics (If Issues Detected)**
```bash
# Memory and build analysis
pio run -t size
pio check --verbose

# Dependency verification
pio pkg list --only=libraries
pio pkg outdated

# Hardware connection test
pio device monitor --baud 115200 --filter esp32_exception_decoder
```

## 📊 Session Documentation Template

### **Session Header Format**
```markdown
## Development Session - [DATE] [START_TIME]-[END_TIME]
**Focus:** [MAIN_SESSION_OBJECTIVE]
**Priority:** [HIGH/MEDIUM/LOW]
**Estimated Duration:** [TIME_ESTIMATE]
**Prerequisites:** [DEPENDENCIES_OR_SETUP_REQUIREMENTS]
```

### **Progress Tracking Structure**
```markdown
### Session Tasks
1. **[TASK_NAME]** - [STATUS: PENDING/IN_PROGRESS/COMPLETED]
   - Description: [TASK_DESCRIPTION]
   - Files: [AFFECTED_FILES]
   - Estimated Time: [TIME_ESTIMATE]
   - Completion: [PERCENTAGE]

### Technical Decisions Made
- **[DECISION_TOPIC]:** [REASONING_AND_OUTCOME]
- **[ARCHITECTURE_CHOICE]:** [IMPLEMENTATION_APPROACH]

### Issues Encountered
- **Problem:** [ISSUE_DESCRIPTION]
- **Solution:** [RESOLUTION_METHOD]  
- **Prevention:** [HOW_TO_AVOID_IN_FUTURE]

### Session Results
- **Files Modified:** [LIST_OF_FILES_WITH_LINE_COUNTS]
- **Git Commits:** [COMMIT_HASHES_AND_MESSAGES]
- **Testing Status:** [PASS/FAIL_WITH_DETAILS]
- **Next Session Prep:** [HANDOFF_NOTES]
```

## 🔧 Common Debugging Workflows

### **Build Issues Resolution**
1. Clean build: `pio run -t clean`
2. Check compile errors in order of appearance
3. Verify include paths in `platformio.ini`
4. Check library dependencies and versions
5. Review memory constraints: `pio run -t size`

### **Hardware Communication Issues**
1. Test serial connection: `pio device monitor`
2. Verify CAN bus wiring and termination
3. Check Ethernet module power and connections
4. Use oscilloscope for signal verification
5. Review ESP32S3 pin assignments in `include/config.h`

### **Performance Analysis**
1. Enable debugging: `#define DEBUG_ENABLED 1`
2. Add timing measurements in critical paths
3. Monitor task stack usage
4. Check WiFi/Ethernet throughput
5. Profile CAN message handling latency

## 📝 Session End Procedures

### **Code Quality Checks**
- [ ] Run linter/formatter if available
- [ ] Check for TODO/FIXME comments
- [ ] Verify all debug prints are properly conditioned
- [ ] Confirm no hardcoded values remain
- [ ] Test critical functionality paths

### **Documentation Updates**
- [ ] Update relevant code comments
- [ ] Modify function headers if interfaces changed
- [ ] Update `DEVELOPMENT_PROGRESS_LOG.md`
- [ ] Refresh `NEXT_SESSION_START.md` priorities
- [ ] Document any new configuration options

### **Git Workflow**
- [ ] Stage changes: `git add [FILES]`
- [ ] Review diff: `git diff --staged`
- [ ] Commit with descriptive message following project conventions
- [ ] Update progress tracking files
- [ ] Consider push to remote if milestone reached

## 🎯 Session Types and Templates

### **Feature Development Session**
- Focus: Implementing new functionality
- Duration: 60-120 minutes typically
- Key Activities: Design → Code → Test → Document
- Success Criteria: Feature works, tests pass, documentation updated

### **Bug Fix Session**  
- Focus: Resolving specific issues
- Duration: 30-90 minutes typically
- Key Activities: Reproduce → Diagnose → Fix → Verify → Document
- Success Criteria: Bug eliminated, no regressions, preventive measures added

### **Refactoring Session**
- Focus: Code improvement without functionality changes
- Duration: 45-90 minutes typically
- Key Activities: Analyze → Plan → Refactor → Test → Document
- Success Criteria: Code cleaner, performance same/better, all tests pass

### **Integration Session**
- Focus: Combining components or external integration
- Duration: 90-180 minutes typically
- Key Activities: Plan → Configure → Integrate → Test → Debug → Document
- Success Criteria: Integration works, error handling robust, documentation complete

## 💡 Best Practices

### **Time Management**
- Set realistic session goals based on complexity
- Take breaks every 60-90 minutes to maintain focus
- Reserve 10-15 minutes at end for documentation and cleanup
- Track actual vs estimated time for better future planning

### **Quality Assurance**
- Test changes incrementally, not just at the end
- Keep commits focused and atomic
- Write descriptive commit messages following project standards
- Always leave code in working state at session end

### **Communication**
- Document decisions and reasoning in code comments
- Update progress logs immediately after major milestones
- Leave clear notes for next session or other developers
- Flag any blockers or dependencies for follow-up

---

**Usage Instructions:**
1. Copy this template at start of each development session
2. Fill in [PLACEHOLDER_VALUES] with actual information
3. Use checkboxes to track completion of setup items
4. Adapt command examples to match your specific project paths
5. Customize session type templates based on your workflow
6. Update and improve this template based on experience

**Integration with Universal Workflow:**
- This template supports all phases of development
- Works with TodoWrite patterns for task tracking  
- Integrates with git workflow standards
- Provides consistency across development sessions
- Enables effective session handoffs and continuity