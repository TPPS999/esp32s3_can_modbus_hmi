# Session Management Templates - ESP32S3 CAN-Modbus TCP

**Project:** ESP32S3 CAN to Modbus TCP Bridge  
**Repository:** https://github.com/user/esp32s3-can-modbus-tcp  
**Version:** Session Templates v1.0  
**Created:** 27.08.2025 (Warsaw Time)  

---

## 🎯 Template Purpose

This document provides systematic templates for managing development sessions using Universal Workflow patterns, specifically adapted for ESP32S3/PlatformIO embedded development.

## 📋 Session Startup Template

### 1. Environment Setup Commands
```bash
# Navigate to project (ALWAYS use quotes for paths with spaces)
cd "d:\OD\OneDrive - Wanaka sp. z o.o\Documents laptop\PlatformIO\Projects\esp32s3-can-modbus tcp"

# Check git repository status
git status
git branch -v

# Verify PlatformIO environment
pio run --list-targets
```

### 2. Progress Review Pattern
```bash
# Read last session notes (limit to avoid token overflow)
Read file_path="DEVELOPMENT_PROGRESS_LOG.md" limit=50

# Check recent commits
git log --oneline -5
```

### 3. TodoWrite Session Setup Pattern
```
TodoWrite todos=[
    {"content": "Review previous session progress from DEVELOPMENT_PROGRESS_LOG.md", "status": "in_progress"},
    {"content": "Plan current session objectives", "status": "pending"},
    {"content": "[Specific task from Next Session Priorities]", "status": "pending"},
    {"content": "[Second priority task if applicable]", "status": "pending"},
    {"content": "Update documentation with changes", "status": "pending"},
    {"content": "Commit and push changes with Claude Code convention", "status": "pending"}
]
```

---

## 🔄 Mid-Session Work Patterns

### Module Enhancement Work Pattern
```
# For working on specific ESP32S3 modules (config.h, wifi_manager.h, etc.)
TodoWrite todos=[
    {"content": "Read and analyze [module_name].h/.cpp files", "status": "in_progress"},
    {"content": "Add professional header to [module_name]", "status": "pending"},
    {"content": "Document public API functions and structures", "status": "pending"},
    {"content": "Test compilation with: pio run", "status": "pending"},
    {"content": "Update progress log with module changes", "status": "pending"},
    {"content": "Commit module enhancements", "status": "pending"}
]
```

### Documentation Creation Pattern
```
# For creating new documentation (ARCHITECTURE.md, SETUP.md, etc.)
TodoWrite todos=[
    {"content": "Create docs/[DOCUMENT_NAME].md with professional header", "status": "in_progress"},  
    {"content": "Research existing code for documentation content", "status": "pending"},
    {"content": "Write comprehensive sections with ESP32 specifics", "status": "pending"},
    {"content": "Add code examples and hardware connection details", "status": "pending"},
    {"content": "Update main README.md with new document references", "status": "pending"},
    {"content": "Commit documentation updates", "status": "pending"}
]
```

### Code Template Creation Pattern
```
# For creating reusable code templates
TodoWrite todos=[
    {"content": "Analyze existing code patterns in ESP32S3 modules", "status": "in_progress"},
    {"content": "Create function/class template for Arduino Framework", "status": "pending"},
    {"content": "Create module header template for ESP32 development", "status": "pending"},
    {"content": "Test templates with sample implementation", "status": "pending"},
    {"content": "Document template usage in project", "status": "pending"},
    {"content": "Commit template system", "status": "pending"}
]
```

### Troubleshooting/Testing Pattern
```
# For debugging and testing ESP32S3 functionality
TodoWrite todos=[
    {"content": "Identify and reproduce issue with [component]", "status": "in_progress"},
    {"content": "Analyze code and hardware connections", "status": "pending"},
    {"content": "Implement fix with proper error handling", "status": "pending"},
    {"content": "Test solution on hardware if available", "status": "pending"},
    {"content": "Document fix in troubleshooting guide", "status": "pending"},
    {"content": "Commit tested solution", "status": "pending"}
]
```

---

## 🏁 Session Completion Template

### Final Session Update Process

#### 1. Complete Current Logical Work Unit
- Finish the task currently marked as "in_progress"
- Ensure no code is left in broken state
- Test compilation if code changes were made

#### 2. Update DEVELOPMENT_PROGRESS_LOG.md
```
# Add new session entry with:
## Session YYYY-MM-DD HH:MM - [Session Title]

### 📊 Session Status:
- **Duration:** [X minutes]
- **Branch:** main  
- **Files Modified:** [list modified files]
- **Git Status:** [COMMITTED/PENDING]

### ✅ Completed This Session:
- HH:MM - [Specific task completed]
- HH:MM - [Another task completed]

### 📋 Next Session Priorities:
1. **High Priority:** [Specific next action with file paths]
2. **Medium Priority:** [Important follow-up task]
3. **Low Priority:** [Nice-to-have improvement]

### 💡 Session Notes:
- [Important observations]
- [Decisions made]
- [Issues encountered and solutions]
```

#### 3. Final TodoWrite Update
```
TodoWrite todos=[
    {"content": "Review previous session progress", "status": "completed"},
    {"content": "Plan current session objectives", "status": "completed"},
    {"content": "[Main task of session]", "status": "completed"},
    {"content": "Update documentation with changes", "status": "completed"},
    {"content": "Commit and push changes with Claude Code convention", "status": "completed"}
]
```

#### 4. Git Commit Pattern
```bash
# Stage all changes
git add [modified files]

# Commit with Claude Code convention
git commit -m "$(cat <<'EOF'
[type]: [brief description]

[Detailed description of changes]:
- [Specific change 1]
- [Specific change 2]
- [Specific change 3]

🤖 Generated with Claude Code

Co-Authored-By: Claude <noreply@anthropic.com>
EOF
)"
```

---

## 📊 Session Type Quick References

### Documentation Session (20-30 min)
1. **Setup**: Navigate + Review progress
2. **Create**: New .md file with professional header  
3. **Research**: Read existing code for content
4. **Write**: Comprehensive sections with examples
5. **Link**: Update README.md references
6. **Commit**: Documentation updates

### Code Enhancement Session (30-45 min)
1. **Setup**: Navigate + Review progress
2. **Analyze**: Read existing module code
3. **Enhance**: Add headers, improve code, document APIs
4. **Test**: Compile with `pio run`
5. **Document**: Update related documentation
6. **Commit**: Module improvements

### Architecture Session (45-60 min)
1. **Setup**: Navigate + Review progress
2. **Research**: Analyze entire project structure
3. **Document**: Create comprehensive architectural overview
4. **Diagram**: Add structure diagrams if needed
5. **Integrate**: Link with existing documentation
6. **Commit**: Architecture documentation

---

## 🔗 Git Workflow Integration Patterns

### Session Startup Git Workflow
```bash
# 1. Enhanced Git Status Check (replaces basic git status)
./scripts/git-helpers.sh status

# 2. Configure git if first time in session
./scripts/git-helpers.sh config

# 3. Check for uncommitted work from previous session
git status
```

### Git Integration TodoWrite Patterns

#### Session with Git Helper Scripts:
```
TodoWrite todos=[
    {"content": "Review git status with comprehensive helper script", "status": "in_progress"},
    {"content": "Plan current session objectives", "status": "pending"},
    {"content": "[Specific development task]", "status": "pending"},
    {"content": "Use git backup before hardware testing if applicable", "status": "pending"},
    {"content": "Commit with standardized message format", "status": "pending"}
]
```

#### Hardware Testing Session Pattern:
```
TodoWrite todos=[
    {"content": "Create hardware testing backup", "status": "in_progress"},
    {"content": "Perform ESP32S3 hardware testing", "status": "pending"},
    {"content": "Document test results", "status": "pending"},
    {"content": "Commit test results with git helper", "status": "pending"}
]
```

### Mid-Session Git Patterns

#### Regular Commit Pattern (every 30-45 minutes):
```bash
# Use git helper for standardized commits
./scripts/git-helpers.sh commit [type] "[brief description]" "[detailed changes]"

# Example:
./scripts/git-helpers.sh commit "feat" "add CAN filtering" "Enhanced CAN frame filtering with advanced pattern matching"
```

#### Hardware Testing Backup Pattern:
```bash
# Before any hardware testing or risky operations
./scripts/git-helpers.sh backup
```

### Session End Git Workflow

#### Standard Session Completion:
```bash
# Use git helper for session end commit
./scripts/git-helpers.sh end "Phase 4 git workflow implementation completed"
```

#### Alternative Manual Commit (when git-helpers not available):
```bash
git add -A
git commit -m "$(cat <<'EOF'
[type]: [brief description]

[Detailed description of session work]:
- [Specific accomplishment 1]
- [Specific accomplishment 2]  
- [Specific accomplishment 3]

Next session: [Specific next priorities]

🤖 Generated with Claude Code

Co-Authored-By: Claude <noreply@anthropic.com>
EOF
)"

git push origin main
```

### Git Helper Script Usage Quick Reference

#### Available Commands:
```bash
./scripts/git-helpers.sh status      # Comprehensive git status
./scripts/git-helpers.sh backup     # Hardware testing backup
./scripts/git-helpers.sh end "msg"  # End session with message
./scripts/git-helpers.sh commit type "desc" "details"  # Custom commit
./scripts/git-helpers.sh config     # Setup git configuration
./scripts/git-helpers.sh test       # Hardware test results commit
./scripts/git-helpers.sh help       # Show all commands
```

#### ESP32S3-Specific Commit Types:
- **Core Development**: feat, fix, refactor, perf, style
- **Maintenance**: docs, test, chore  
- **ESP32S3-Specific**: hw (hardware config), config (settings), can (CAN bus), modbus (Modbus TCP), wifi (WiFi)

### Git Workflow Session Integration

#### Enhanced Session Startup Template:
```bash
# 1. Navigate to project
cd "d:\OD\OneDrive - Wanaka sp. z o.o\Documents laptop\PlatformIO\Projects\esp32s3-can-modbus tcp"

# 2. Comprehensive git status check
./scripts/git-helpers.sh status

# 3. Review progress and setup session
Read file_path="DEVELOPMENT_PROGRESS_LOG.md" limit=50

# 4. Setup TodoWrite with git integration
TodoWrite todos=[
    {"content": "Review git status and previous session progress", "status": "in_progress"},
    {"content": "[Primary session task]", "status": "pending"},
    {"content": "Regular commits every 30-45 minutes", "status": "pending"},
    {"content": "End session with git helper script", "status": "pending"}
]
```

#### Enhanced Session End Template:
```bash
# 1. Complete current work
# [Complete logical work unit]

# 2. Update progress log
Edit file_path="DEVELOPMENT_PROGRESS_LOG.md"
# [Add session summary]

# 3. Final TodoWrite update
TodoWrite todos=[all completed]

# 4. Session end with git helper
./scripts/git-helpers.sh end "[Session accomplishments summary]"

# 5. Verify clean state
git status
```

### Hardware Development Git Patterns

#### Before Hardware Testing:
```
TodoWrite todos=[
    {"content": "Prepare code for hardware testing", "status": "in_progress"},
    {"content": "Create hardware testing backup", "status": "pending"},
    {"content": "Test on ESP32S3 hardware", "status": "pending"},
    {"content": "Document hardware test results", "status": "pending"},
    {"content": "Commit verified functionality", "status": "pending"}
]

# Backup before testing
./scripts/git-helpers.sh backup
```

#### After Hardware Testing:
```bash
# If testing successful
./scripts/git-helpers.sh test

# If testing revealed issues
./scripts/git-helpers.sh commit "fix" "resolve hardware testing issues" "Fixed CAN bus timing issues discovered during hardware testing"
```

---

## ⚠️ Emergency Session Procedures

### Token Limit Approaching (>80% usage)
1. **Immediate Action**:
   ```
   # Update progress log immediately
   Edit file_path="DEVELOPMENT_PROGRESS_LOG.md" 
   # Add current status to session log
   ```

2. **Quick Commit**:
   ```bash
   git add .
   git commit -m "work in progress: [current task status]

   🤖 Generated with Claude Code
   
   Co-Authored-By: Claude <noreply@anthropic.com>"
   ```

3. **Context Preservation**:
   - List exact next steps in progress log
   - Include specific file paths and line numbers
   - Note current state of implementation

### Hardware Testing Session
When ESP32S3 hardware is available:
1. **Prepare**: Ensure stable power and connections
2. **Upload**: Use `pio run --target upload`
3. **Monitor**: Use `pio device monitor --baud 115200`
4. **Test**: Verify CAN Bus, WiFi, and Modbus functionality
5. **Document**: Record test results and any issues
6. **Commit**: Hardware-verified changes

---

*Session Templates maintained for ESP32S3 CAN-Modbus TCP Bridge project*  
*Last Updated: 27.08.2025 (Warsaw Time)*