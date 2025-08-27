# ESP32S3 CAN-Modbus TCP - Git Workflow Guide

**Project:** ESP32S3 CAN to Modbus TCP Bridge  
**Repository:** https://github.com/user/esp32s3-can-modbus-tcp  
**Version:** Git Workflow Guide v1.0  
**Created:** 27.08.2025 (Warsaw Time)  
**Last Updated:** 27.08.2025 (Warsaw Time)  
**Status:** Final

---

## Document Information

### Purpose:
Comprehensive git workflow documentation for professional development standards
with Universal Workflow compliance and embedded development optimizations.

### Scope:
- Single branch development strategy
- Commit message conventions and templates
- Regular push schedules and automation
- Git command helpers and scripts
- ESP32S3-specific workflow patterns

### Audience:
- Development team members
- Contributors and maintainers
- Future developers joining the project
- Embedded systems developers

### Related Documents:
- [Session Templates](../SESSION_TEMPLATES.md) - Development workflow patterns
- [Development Progress Log](../DEVELOPMENT_PROGRESS_LOG.md) - Project history
- [Architecture Documentation](ARCHITECTURE.md) - System design

---

## Single Branch Strategy

### Branch Model: Simplified Main Branch
```
main (primary development branch)
├── Feature development commits
├── Bug fixes and improvements  
├── Documentation updates
├── Configuration changes
├── Hardware testing milestones
└── Release milestones
```

### Why Single Branch Strategy for ESP32S3 Project:
- **Solo Development**: Optimized for single developer workflow
- **Embedded Projects**: Simplified for hardware-dependent development
- **Linear History**: Clear progression of changes for hardware iterations
- **Easy Rollback**: Simple revert to any previous stable state
- **Reduced Complexity**: No merge conflicts between branches
- **Hardware Testing**: Simplified state management for physical testing

### Benefits of Single Branch:
1. **Clear History**: Linear development progression
2. **Simple Rollback**: Easy to revert to any working state
3. **No Merge Conflicts**: Eliminates branch merging issues
4. **Hardware Focus**: Optimized for embedded development cycles
5. **Session Continuity**: Seamless session-to-session development

### When to Consider Multiple Branches:
- Multiple developers working simultaneously
- Production vs development versions needed
- Experimental features requiring isolation
- Long-term feature development parallel to maintenance

## Commit Message Conventions

### Standard Format Template:
```
type: brief description (50 characters max)

Optional longer description explaining the change in more detail.
Can include multiple paragraphs if needed for complex changes.

- Specific change 1
- Specific change 2  
- Specific change 3

🤖 Generated with Claude Code

Co-Authored-By: Claude <noreply@anthropic.com>
```

### Commit Types for ESP32S3 Project:

#### Core Development Types:
- **feat:** New feature or functionality
- **fix:** Bug fix or error correction
- **refactor:** Code refactoring (no functionality change)
- **perf:** Performance improvements
- **style:** Code formatting changes (no logic change)

#### Documentation and Maintenance:
- **docs:** Documentation updates
- **test:** Adding or updating tests
- **chore:** Maintenance tasks (dependencies, build, etc.)

#### ESP32S3-Specific Types:
- **hw:** Hardware configuration changes
- **config:** Configuration file changes  
- **can:** CAN bus protocol changes
- **modbus:** Modbus TCP implementation changes
- **wifi:** WiFi connectivity changes

### Commit Message Examples:

#### Feature Addition:
```bash
feat: add CAN-triggered AP mode functionality

Implemented remote AP activation via CAN frames:
- Added CAN frame monitoring for ID 0xEF1
- Created AP trigger state machine with 3-frame validation
- Added 30-second timeout with auto-disable
- Integrated with WiFi manager callbacks
- Updated web interface for triggered AP configuration

🤖 Generated with Claude Code

Co-Authored-By: Claude <noreply@anthropic.com>
```

#### Bug Fix:
```bash
fix: resolve Modbus TCP register mapping overflow

Fixed register address calculation for BMS modules 9-16:
- Corrected base address calculation formula
- Added bounds checking for register access
- Updated register mapping documentation
- Verified all 16 BMS modules access correctly

🤖 Generated with Claude Code

Co-Authored-By: Claude <noreply@anthropic.com>
```

#### Documentation Update:
```bash
docs: add comprehensive API documentation for all modules

Enhanced project documentation with complete API reference:
- Created docs/API.md with 350+ lines of API documentation
- Added function signatures and usage examples
- Documented all 8 system modules with detailed descriptions
- Included error handling and best practices

🤖 Generated with Claude Code

Co-Authored-By: Claude <noreply@anthropic.com>
```

#### Hardware Configuration:
```bash
hw: update ESP32S3 CAN bus pin assignments

Modified CAN interface configuration for production hardware:
- Changed CS pin from GPIO5 to GPIO10 for PCB compatibility
- Updated SPI configuration for MCP2515 controller
- Verified signal integrity with new pin assignments
- Updated hardware documentation and connection diagrams

🤖 Generated with Claude Code

Co-Authored-By: Claude <noreply@anthropic.com>
```

#### Configuration Changes:
```bash
config: increase BMS communication timeout to 45 seconds

Extended BMS timeout for improved reliability:
- Changed timeout from 30s to 45s for slower BMS responses
- Updated configuration validation ranges
- Modified timeout handling in BMS protocol parser
- Tested with various BMS response times

🤖 Generated with Claude Code

Co-Authored-By: Claude <noreply@anthropic.com>
```

## Regular Push Schedule

### Push Strategy for Embedded Development:

#### High Priority (Immediate Push):
- **Feature Completion**: After implementing complete feature with testing
- **Critical Bug Fixes**: Immediately after verification
- **Hardware Configuration**: After successful hardware testing
- **Documentation Updates**: Major documentation changes

#### Medium Priority (End of Session):
- **Daily Progress**: End of each development session
- **Work-in-Progress**: Logical stopping points during development
- **Configuration Adjustments**: Minor config changes

#### Low Priority (Periodic):
- **Code Cleanup**: Style and formatting improvements
- **Comment Updates**: Code documentation improvements

### Automated Git Operations Integration:

#### Pre-commit Considerations:
- Compilation verification (if tools available)
- Basic code formatting validation
- Documentation consistency checks

#### Post-commit Actions:
- Automatic progress log updates
- Session state documentation
- Development milestone tracking

## Git Command Patterns for ESP32S3 Development

### Daily Workflow Commands:

#### Project Navigation (Critical for Paths with Spaces):
```bash
# ALWAYS use quotes for paths with spaces
cd "d:\OD\OneDrive - Wanaka sp. z o.o\Documents laptop\PlatformIO\Projects\esp32s3-can-modbus tcp"

# Verify current location
pwd
```

#### Status and History Check:
```bash
# Comprehensive status check
git status
git log --oneline -10
git branch -v

# Check for uncommitted changes
git diff --name-only
git diff --stat
```

#### Standard Commit Process:
```bash
# Stage all changes
git add .

# Or stage specific files
git add "src/specific file.cpp" "include/header.h"

# Commit with Claude Code signature
git commit -m "$(cat <<'EOF'
type: brief description

Detailed explanation of changes:
- Specific change 1
- Specific change 2
- Specific change 3

🤖 Generated with Claude Code

Co-Authored-By: Claude <noreply@anthropic.com>
EOF
)"

# Push to remote
git push origin main

# Verify push success
git status
```

### ESP32S3 Specific Git Patterns:

#### Before Hardware Testing:
```bash
# Create backup before physical testing
git add -A
git commit -m "chore: backup before ESP32S3 hardware testing

Saving current state before physical testing:
- Code compilation verified with pio run
- Configuration ready for hardware verification
- CAN bus and Modbus TCP implementation ready
- All systems prepared for hardware validation

🤖 Generated with Claude Code

Co-Authored-By: Claude <noreply@anthropic.com>"

git push origin main
echo "Hardware testing backup completed - safe to proceed with testing"
```

#### After Successful Hardware Test:
```bash
git add -A  
git commit -m "test: verify ESP32S3 hardware functionality

Hardware testing completed successfully:
- CAN bus communication verified with actual BMS modules
- Modbus TCP server tested with industrial clients
- WiFi connectivity confirmed in production environment
- All system components operational on target hardware

🤖 Generated with Claude Code

Co-Authored-By: Claude <noreply@anthropic.com>"

git push origin main
```

#### Configuration Changes:
```bash
# Stage configuration files specifically
git add include/config.h src/config.cpp

git commit -m "config: update BMS node configuration for deployment

Modified BMS configuration for production setup:
- Updated active BMS count from 4 to 8 modules
- Modified node ID assignments for field installation
- Adjusted communication timeouts for network conditions
- Updated CAN bus speed configuration for site requirements

🤖 Generated with Claude Code

Co-Authored-By: Claude <noreply@anthropic.com>"

git push origin main
```

#### Development Session End:
```bash
git add -A
git commit -m "chore: end development session - Phase 4 git workflow

Session accomplishments:
- Implemented git workflow documentation and standards
- Created automated git helper scripts
- Enhanced commit message templates and conventions
- Integrated git workflow with session management

Next session: Phase 5 implementation (templates and examples)

🤖 Generated with Claude Code

Co-Authored-By: Claude <noreply@anthropic.com>"

git push origin main
```

## Git Helpers and Automation

### Using .gitmessage Template:
```bash
# Configure git to use commit message template
git config commit.template .gitmessage

# Commit with template (opens editor)
git commit

# Or use direct message for simple commits
git commit -m "type: description"
```

### Scripts and Automation:

#### Git Helper Script Usage:
```bash
# Quick status check
./scripts/git-helpers.sh status

# Hardware testing backup
./scripts/git-helpers.sh backup

# End development session
./scripts/git-helpers.sh end "Phase 4 implementation completed"

# Custom commit with signature
./scripts/git-helpers.sh commit "feat" "new feature" "detailed description"
```

## Troubleshooting Git Issues

### Common Path Issues (Windows):
```bash
# Problem: Paths with spaces causing issues
# Solution: Always use quotes

# ❌ Wrong:
cd D:\OD\OneDrive - Wanaka sp. z o.o\Documents laptop\...

# ✅ Correct:
cd "D:\OD\OneDrive - Wanaka sp. z o.o\Documents laptop\..."

# For git commands:
git add "src/file with spaces.cpp"
```

### Repository Issues:
```bash
# Check repository status
git status
git remote -v

# Sync with remote if behind
git pull origin main

# Force sync if necessary (use carefully)
git reset --hard origin/main
```

### Commit Message Issues:
```bash
# Fix last commit message
git commit --amend -m "corrected message"

# Interactive rebase for multiple commits (use carefully)
git rebase -i HEAD~3
```

## Best Practices for ESP32S3 Development

### Development Workflow Integration:
1. **Start Session**: Check git status and recent commits
2. **During Development**: Regular commits every 30-45 minutes
3. **Before Hardware Testing**: Create backup commit
4. **After Testing**: Commit test results and findings
5. **End Session**: Final commit with session summary

### Commit Frequency Guidelines:
- **Feature Development**: After each logical component completion
- **Bug Fixes**: Immediately after fix verification
- **Documentation**: After significant documentation updates
- **Configuration**: After each config change that affects system behavior

### Message Quality Standards:
- **Subject Line**: Imperative mood, under 50 characters
- **Body**: Explain what and why, not how
- **Lists**: Use bullet points for multiple changes
- **Context**: Include relevant background information
- **Signature**: Always include Claude Code signature

## Integration with Development Sessions

### Session Startup Git Checklist:
- [ ] Navigate to project directory
- [ ] Check git status for uncommitted work
- [ ] Review recent commit history
- [ ] Pull latest changes (if collaborating)
- [ ] Verify clean working state

### Session Progress Git Pattern:
- [ ] Regular commits every 30-45 minutes
- [ ] Descriptive commit messages with context
- [ ] Backup before risky operations (hardware testing)
- [ ] Documentation commits alongside code changes

### Session End Git Checklist:
- [ ] Commit all completed work
- [ ] Update progress documentation
- [ ] Push all commits to remote
- [ ] Verify clean repository state
- [ ] Document next session starting point

---

## Summary

This git workflow guide provides comprehensive standards for professional development of the ESP32S3 CAN-Modbus TCP Bridge project. The single branch strategy, standardized commit messages, and embedded development optimizations ensure consistent, traceable, and maintainable code development.

### Key Benefits:
- **Consistency**: Standardized commit messages and workflow patterns
- **Traceability**: Clear development history with detailed context
- **Reliability**: Regular backups and systematic state management
- **Professional Standards**: Enterprise-grade development practices
- **Embedded Focus**: Optimized for hardware development cycles

---

*Git Workflow Guide maintained as part of Universal Workflow standards*  
*For session management, see [Session Templates](../SESSION_TEMPLATES.md)*  
*For project history, see [Development Progress Log](../DEVELOPMENT_PROGRESS_LOG.md)*