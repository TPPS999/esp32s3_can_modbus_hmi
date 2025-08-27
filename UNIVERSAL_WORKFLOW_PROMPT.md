# 🔧 UNIVERSAL PROJECT WORKFLOW REPRODUCTION PROMPT

**Purpose:** Reproduce professional development workflow in any new project  
**Based on:** Established patterns from industrial EMS project  
**Usage:** Copy sections and replace {PLACEHOLDERS} with your project details  
**Version:** v1.0.0  
**Created:** 27.08.2025 (Warsaw Time)

---

## QUICK START INSTRUCTIONS

### 🚀 How to Use This Prompt:

1. **Copy this entire prompt** to your Claude Code conversation
2. **Replace all placeholders** with your project-specific values:
   - `{PROJECT_NAME}` - Your project name (e.g., "PowerSystem_Control")
   - `{REPO_PATH}` - Full path to your repository (use quotes if spaces)
   - `{REPO_URL}` - Your GitHub/Git repository URL
   - `{TIMEZONE}` - Your timezone (default: Warsaw Time)
   - `{MAIN_BRANCH}` - Main branch name (default: main)

3. **Start with Phase 0** - Complete project inventory before development
4. **Follow phases sequentially** - Each phase builds on the previous
5. **Use TodoWrite consistently** - Track all tasks and progress

### 📋 Placeholder Replacement Example:
```
BEFORE: cd "{REPO_PATH}"
AFTER:  cd "C:\Projects\MyProject"

BEFORE: Repository: {REPO_URL}
AFTER:  Repository: https://github.com/user/myproject
```

---

# PHASE 0: PROJECT INVENTORY & ANALYSIS

## Step 1: Repository Structure Analysis

### 🔍 Initial Assessment Commands:
```bash
# Navigate to project root (ALWAYS use quotes for paths with spaces)
cd "{REPO_PATH}"

# Verify git repository status
git status
git branch -v

# Map main folder structure
LS path="{REPO_PATH}"

# Identify all subdirectories
Glob pattern="*/"

# Get overview of project scale
Glob pattern="**/*" | head 50
```

### 📊 Structure Documentation Template:
```markdown
# PROJECT STRUCTURE INVENTORY - {PROJECT_NAME}
Date: {DATE} ({TIMEZONE})

## Main Directories Found:
- [List all main folders]
- [Note any unusual structures]
- [Identify potential organization patterns]

## Initial Scale Assessment:
- Total directories: X
- Apparent project type: [embedded/web/data/etc.]
- Technology indicators: [file extensions found]
```

## Step 2: Code Base Inventory

### 🔧 Code File Analysis:
```bash
# Find all code files by type
Glob pattern="**/*.st"    # Structured Text (PLC)
Glob pattern="**/*.py"    # Python
Glob pattern="**/*.js"    # JavaScript
Glob pattern="**/*.ts"    # TypeScript
Glob pattern="**/*.c"     # C
Glob pattern="**/*.cpp"   # C++
Glob pattern="**/*.h"     # Headers
Glob pattern="**/*.cs"    # C#
Glob pattern="**/*.java"  # Java

# Find configuration files
Glob pattern="**/*.json"
Glob pattern="**/*.xml"
Glob pattern="**/*.yaml"
Glob pattern="**/*.yml"
Glob pattern="**/*.ini"
Glob pattern="**/*.conf"
```

### 🔍 Component Identification:
```bash
# Search for main component types
Grep pattern="FUNCTION_BLOCK|CLASS|PROGRAM" output_mode="files_with_matches"
Grep pattern="TYPE.*STRUCT|interface|typedef" output_mode="files_with_matches"
Grep pattern="FUNCTION|def |function |public " output_mode="count"

# Look for main entry points
Grep pattern="main|Main|MAIN|setup|init" output_mode="files_with_matches"

# Identify potential duplicates
Grep pattern="FB_|Class|function" output_mode="content" -A 2 | head 100
```

### 📋 Code Inventory Template:
```markdown
## CODE BASE INVENTORY

### File Count by Type:
- Structured Text (.st): X files
- Python (.py): X files  
- Configuration (.json/.xml): X files
- [Other types found]: X files

### Main Components Identified:
- Function Blocks/Classes: [list main ones]
- Data Types/Structures: [list main ones] 
- Main Programs/Entry Points: [list]
- Configuration Files: [list important ones]

### Potential Issues Spotted:
- Duplicate naming patterns: [list if any]
- Missing documentation: [note gaps]
- Unusual file locations: [note if any]
```

## Step 3: Documentation Audit

### 📚 Documentation Discovery:
```bash
# Find all documentation files
Glob pattern="**/*.md"
Glob pattern="**/*.txt"
Glob pattern="**/README*"
Glob pattern="**/CHANGELOG*"
Glob pattern="**/LICENSE*"

# Check for project documentation
Read file_path="{REPO_PATH}/README.md" # if exists
Read file_path="{REPO_PATH}/README.txt" # if exists

# Look for embedded documentation
Grep pattern="//.*TODO|#.*TODO|//.*FIXME" output_mode="content"
Grep pattern="//.*@|#.*@|/\*\*.*@" output_mode="files_with_matches" # JSDoc/similar
```

### 📋 Documentation Audit Template:
```markdown
## DOCUMENTATION AUDIT

### Existing Documentation:
- Main README: [YES/NO - quality rating]
- Project documentation: [list files found]
- Code comments: [GOOD/SPARSE/NONE]
- API documentation: [YES/NO]

### Documentation Gaps:
- Missing project overview: [YES/NO]
- Missing setup instructions: [YES/NO]
- Missing architecture docs: [YES/NO]
- Missing coding standards: [YES/NO]

### Documentation Quality:
- Consistency: [GOOD/FAIR/POOR]
- Completeness: [GOOD/FAIR/POOR]
- Up-to-date: [GOOD/FAIR/POOR]
```

## Step 4: Convention Detection

### 🔍 Naming Pattern Analysis:
```bash
# Analyze file naming conventions
LS path="{REPO_PATH}" | head 20  # Sample file names
Grep pattern="^[A-Z_]+|^[a-z_]+|^[A-Z][a-z]" output_mode="files_with_matches"

# Check for prefixes/suffixes
Grep pattern="FB_|ST_|E_|GVL_" output_mode="files_with_matches" # PLC patterns
Grep pattern="Test|test|Mock|mock" output_mode="files_with_matches" # Test patterns
Grep pattern="Utils|utils|Helper|helper" output_mode="files_with_matches" # Utility patterns

# Examine code style in sample files
Read file_path=[select 2-3 representative files] limit=50
```

### 📊 Git Pattern Analysis:
```bash
# Check recent commit patterns
git log --oneline -20

# Look for commit message conventions
git log --pretty=format:"%s" -20

# Check branch patterns
git branch -a
```

### 📋 Convention Detection Template:
```markdown
## CONVENTIONS ANALYSIS

### File Naming Patterns:
- Main pattern: [CamelCase/snake_case/kebab-case]
- Prefixes used: [list any consistent prefixes]
- Suffixes used: [list any consistent suffixes]
- Special conventions: [note any unique patterns]

### Code Style Indicators:
- Indentation: [spaces/tabs - how many]
- Bracket style: [same line/new line]
- Variable naming: [camelCase/snake_case]
- Comment style: [// or # or /* */]

### Git Patterns:
- Commit message style: [conventional/informal/mixed]
- Branch naming: [feature/fix/custom pattern]
- Frequency: [regular/sparse/unknown]

### Architectural Patterns:
- Folder organization: [by type/by feature/mixed]
- Separation concerns: [GOOD/FAIR/POOR]
- Dependency management: [clear/unclear]
```

## Step 5: Component Assessment

### 🏗️ Architecture Understanding:
```bash
# Look for main system entry points
Grep pattern="main|Main|setup|init|start" output_mode="content" -B 2 -A 5

# Identify core vs utility components  
Grep pattern="core|Core|main|Main|primary" output_mode="files_with_matches"
Grep pattern="util|Utils|helper|Helper|common" output_mode="files_with_matches"

# Check for test/example code
Grep pattern="test|Test|example|Example|sample" output_mode="files_with_matches"

# Look for configuration/setup files
Grep pattern="config|Config|setup|Setup|install" output_mode="files_with_matches"
```

### 📋 Component Assessment Template:
```markdown
## COMPONENT ASSESSMENT

### Critical Components (Core System):
- Entry points: [list main programs/functions]
- Core modules: [list essential components]
- Data structures: [list main data types]

### Utility Components:
- Helper functions: [list utility modules]
- Common libraries: [list shared components]
- Configuration: [list config files/modules]

### Development/Test Components:
- Test files: [list test-related files]
- Examples: [list example/demo files]
- Development tools: [list dev utilities]

### Legacy/Archive Components:
- Unused files: [list potentially obsolete files]
- Old versions: [list files that might be old versions]
- Archive folders: [note any archive/backup folders]

### Dependency Assessment:
- External dependencies: [list if identifiable]
- Internal dependencies: [note complex interdependencies]
- Potential circular dependencies: [flag if suspected]
```

## Step 6: Inventory Report Generation

### 📊 Complete Inventory Report Template:
```markdown
# PROJECT INVENTORY REPORT - {PROJECT_NAME}
**Date:** {DATE} ({TIMEZONE})  
**Repository:** {REPO_URL}  
**Analyzed by:** Claude Code Assistant

---

## EXECUTIVE SUMMARY

### Project Scale:
- **Total Files:** X files
- **Main Technology:** [Primary language/framework]
- **Project Type:** [embedded/web/desktop/data/etc.]
- **Estimated Complexity:** [Low/Medium/High]

### Key Findings:
- **Documentation Status:** [Complete/Partial/Minimal/Missing]
- **Code Organization:** [Excellent/Good/Fair/Needs Improvement]
- **Convention Consistency:** [Consistent/Mixed/Inconsistent]
- **Potential Issues:** [Count of issues found]

---

## DETAILED FINDINGS

[Include all sections from Steps 1-5]

---

## RECOMMENDED ACTIONS

### Priority 1 (Critical):
- [ ] [Action items that must be addressed first]

### Priority 2 (Important):  
- [ ] [Action items that should be addressed soon]

### Priority 3 (Improvement):
- [ ] [Action items for long-term improvement]

### Documentation Tasks:
- [ ] Create/update main README.md
- [ ] Add project setup instructions  
- [ ] Document coding standards
- [ ] Create architecture overview

### Code Cleanup Tasks:
- [ ] Remove duplicate components
- [ ] Standardize naming conventions
- [ ] Add missing documentation
- [ ] Organize file structure

---

## NEXT STEPS
1. **Address Priority 1 items** before starting development
2. **Set up workflow** (Phase 1 of this prompt)
3. **Establish documentation standards** (Phase 3 of this prompt)
4. **Begin regular development** with established patterns
```

---

# PHASE 1: WORKFLOW SETUP

## Git Repository Configuration

### 🔧 Initial Git Setup:
```bash
# Navigate to project root
cd "{REPO_PATH}"

# Initialize git if not already done
git init # (only if needed)

# Set up main branch
git branch -M {MAIN_BRANCH}

# Configure user (if not set globally)
git config user.name "Your Name"
git config user.email "your.email@domain.com"

# Check current status
git status
git branch -v

# Create .gitignore if needed
# [Add common ignore patterns for your technology]
```

### 📁 Recommended Project Structure:
```
{PROJECT_NAME}/
├── docs/                   # All documentation
│   ├── README.md          # Main project documentation
│   ├── ARCHITECTURE.md    # System architecture
│   ├── SETUP.md          # Setup instructions
│   └── API.md            # API documentation (if applicable)
├── src/                   # Source code (adjust for your tech)
│   ├── core/             # Core system components
│   ├── utils/            # Utility functions
│   ├── config/           # Configuration files
│   └── tests/            # Test files
├── scripts/              # Build/deployment scripts
├── examples/             # Usage examples
├── .gitignore           # Git ignore patterns
├── README.md            # Main project README
└── DEVELOPMENT_PROGRESS_LOG.md  # Development tracking
```

### 📋 Initial Documentation Files:

**Create Main README.md:**
```markdown
# {PROJECT_NAME}

**Repository:** {REPO_URL}  
**Status:** In Development  
**Technology:** [Your main technology stack]  
**Created:** {DATE} ({TIMEZONE})

## Quick Start
[Basic setup and usage instructions]

## Project Overview
[Brief description of what the project does]

## Documentation
- [ARCHITECTURE.md](docs/ARCHITECTURE.md) - System architecture
- [SETUP.md](docs/SETUP.md) - Detailed setup instructions
- [DEVELOPMENT_PROGRESS_LOG.md](DEVELOPMENT_PROGRESS_LOG.md) - Development progress

## Contributing
- Follow established coding conventions
- Update documentation with changes
- Use conventional commit messages
- Test thoroughly before committing

---
*Last Updated: {DATE} ({TIMEZONE})*
```

---

# PHASE 2: SESSION MANAGEMENT

## TodoWrite Usage Patterns

### 🎯 Todo Management Strategy:

**Session Startup Pattern:**
```
TodoWrite todos=[
    {"content": "Review previous session progress", "status": "in_progress"},
    {"content": "Plan current session objectives", "status": "pending"},
    {"content": "Implement [specific feature]", "status": "pending"},
    {"content": "Update documentation", "status": "pending"},
    {"content": "Commit and push changes", "status": "pending"}
]
```

**During Development Pattern:**
```
# When starting work on a task
TodoWrite todos=[
    {"content": "Review previous session progress", "status": "completed"},
    {"content": "Plan current session objectives", "status": "completed"},
    {"content": "Implement [specific feature]", "status": "in_progress"},
    {"content": "Update documentation", "status": "pending"},
    {"content": "Commit and push changes", "status": "pending"}
]
```

**Session Completion Pattern:**
```
TodoWrite todos=[
    {"content": "Review previous session progress", "status": "completed"},
    {"content": "Plan current session objectives", "status": "completed"},
    {"content": "Implement [specific feature]", "status": "completed"},
    {"content": "Update documentation", "status": "completed"},
    {"content": "Commit and push changes", "status": "completed"}
]
```

### 📊 Session Continuity Methods

**Development Progress Log System:**
Create `DEVELOPMENT_PROGRESS_LOG.md` with this structure:

```markdown
# {PROJECT_NAME} - Development Progress Log

> **Project:** {PROJECT_NAME}  
> **Repository:** {REPO_URL}  
> **Branch:** {MAIN_BRANCH}  
> **Created:** {DATE} ({TIMEZONE})

---

## Session YYYY-MM-DD HH:MM - [Session Title]

### 📊 Session Status:
- **Duration:** [X minutes]
- **Branch:** {MAIN_BRANCH}  
- **Files Modified:** [list]
- **Git Status:** [clean/changes pending]

### ✅ Completed This Session:
- HH:MM - [Task description]
- HH:MM - [Task description]
- HH:MM - [Task description]

### 🔄 Currently Working On:
- [Current task if session interrupted]

### 📋 Next Session Priorities:
1. **High Priority:** [Critical next steps]
2. **Medium Priority:** [Important tasks]
3. **Low Priority:** [Nice-to-have improvements]

### 💡 Session Notes:
- [Important observations]
- [Decisions made]
- [Issues encountered]

### 🔗 References:
- [Links to documentation]
- [External resources used]

---

[Previous sessions...]
```

## Token Management Strategies

### 🚨 Approaching Session Limits:

**Token Warning Actions (at ~80% usage):**
1. **Immediate Documentation Update:**
   ```
   # Update progress log with current status
   # Commit current work immediately
   # Create detailed notes for next session
   ```

2. **Strategic Session Closure:**
   ```
   # Finish current logical unit of work
   # Update todos to reflect true status
   # Commit with descriptive message
   # Add next session startup instructions
   ```

### 📋 Session Continuation Checklist:

**Starting New Session:**
- [ ] Navigate to project: `cd "{REPO_PATH}"`
- [ ] Check git status: `git status && git branch`
- [ ] Read last session notes in DEVELOPMENT_PROGRESS_LOG.md
- [ ] Review todos from previous session
- [ ] Update current session header in progress log

**During Session:**
- [ ] Update todos as work progresses
- [ ] Make regular commits (every 30-45 minutes)
- [ ] Update progress log with major milestones

**Ending Session:**
- [ ] Complete current logical work unit
- [ ] Update progress log with session summary
- [ ] Commit all changes with descriptive message
- [ ] Mark session as complete in progress log

---

# PHASE 3: DOCUMENTATION STANDARDS

## Professional File Headers

### 🏷️ Code File Header Template:
```
// =====================================================================
// === {FILENAME} - {PROJECT_NAME} ===
// =====================================================================
// 
// 📋 PROJECT INFO:
//    Repository: {REPO_URL}
//    Project: {PROJECT_NAME}
//    Branch: {MAIN_BRANCH}
//    Created: {DATE} ({TIMEZONE})
//
// 📋 MODULE INFO:
//    Module: [ComponentName]
//    Version: v1.0.0
//    Created: {DATE} ({TIMEZONE})
//    Last Modified: {DATE} ({TIMEZONE})
//    Author: [Your Name]
//
// 📊 VERSION HISTORY:
//    v1.0.0 - {DATE} - Initial implementation
//    v1.0.1 - {DATE} - [description of changes]
//    v1.1.0 - {DATE} - [description of major changes]
//
// 🎯 DEPENDENCIES:
//    Internal: [list internal dependencies]
//    External: [list external dependencies]
//
// 📝 DESCRIPTION:
//    [Brief description of module functionality and purpose]
//
// 🔧 CONFIGURATION:
//    [Key configuration parameters or setup requirements]
//
// ⚠️  KNOWN ISSUES:
//    [Any known limitations, bugs, or issues]
//
// 🧪 TESTING STATUS:
//    Unit Tests: [PASS/FAIL/NOT_TESTED]
//    Integration Tests: [PASS/FAIL/NOT_TESTED]  
//    Manual Testing: [PASS/FAIL/NOT_TESTED]
//
// 📈 PERFORMANCE NOTES:
//    [Any performance considerations, benchmarks, or optimizations]
//
// =====================================================================
```

### 📚 Documentation File Header Template:
```markdown
# {DOCUMENT_TITLE}

**Project:** {PROJECT_NAME}  
**Repository:** {REPO_URL}  
**Version:** v1.0.0  
**Created:** {DATE} ({TIMEZONE})  
**Last Updated:** {DATE} ({TIMEZONE})  
**Status:** [Draft/Review/Final]

---

## Document Information

### Purpose:
[What this document is for and who should read it]

### Scope:
[What is covered and what is not covered]

### Audience:
[Target audience - developers/users/administrators/etc.]

### Related Documents:
- [Link to related documentation]
- [Link to related documentation]

---

[Document content here]

---

*Document maintained by: [Your Name/Team]*  
*Next review: [Date or schedule]*
```

## README Structure Template

### 📋 Main Project README.md:
```markdown
# {PROJECT_NAME}

**Repository:** {REPO_URL}  
**Technology:** [Primary tech stack]  
**Status:** [Development/Production/Maintenance]  
**Version:** [Current version]  
**Created:** {DATE} ({TIMEZONE})

## 🎯 Project Overview

### What It Does:
[Clear, concise description of project purpose and functionality]

### Key Features:
- [Feature 1 with brief description]
- [Feature 2 with brief description]
- [Feature 3 with brief description]

### Technology Stack:
- **Primary Language:** [Language/Framework]
- **Dependencies:** [Key dependencies]
- **Platform:** [Target platform/OS]

## 🚀 Quick Start

### Prerequisites:
- [Requirement 1]
- [Requirement 2]
- [Requirement 3]

### Installation:
```bash
# Clone repository
git clone {REPO_URL}
cd {PROJECT_NAME}

# Setup steps (customize for your technology)
[setup command 1]
[setup command 2]
[setup command 3]
```

### Basic Usage:
```bash
# Basic usage examples
[usage example 1]
[usage example 2]
```

## 📚 Documentation

### For Developers:
- [ARCHITECTURE.md](docs/ARCHITECTURE.md) - System architecture and design
- [DEVELOPMENT_PROGRESS_LOG.md](DEVELOPMENT_PROGRESS_LOG.md) - Development history
- [API.md](docs/API.md) - API documentation (if applicable)

### For Users:
- [SETUP.md](docs/SETUP.md) - Detailed setup instructions
- [USER_GUIDE.md](docs/USER_GUIDE.md) - User guide (if applicable)
- [TROUBLESHOOTING.md](docs/TROUBLESHOOTING.md) - Common issues and solutions

## 🏗️ Project Structure

```
{PROJECT_NAME}/
├── src/                    # Source code
├── docs/                   # Documentation
├── tests/                  # Test files
├── examples/               # Usage examples
├── scripts/                # Utility scripts
└── README.md              # This file
```

## 🤝 Contributing

### Development Workflow:
1. Create feature branch from main
2. Implement changes with proper documentation
3. Test thoroughly
4. Commit with conventional commit messages
5. Push and create pull request

### Coding Standards:
- Follow established naming conventions
- Add comprehensive comments
- Update documentation with changes
- Include tests for new functionality
- Maintain consistent code style

### Commit Message Format:
```
type: brief description

Optional longer description explaining the change in more detail.

- Specific change 1
- Specific change 2
```

## 📊 Project Status

### Current Version: [X.Y.Z]
- [Recent major changes]
- [Current development focus]

### Upcoming Features:
- [Planned feature 1]
- [Planned feature 2]

### Known Issues:
- [Known issue 1 with workaround if available]
- [Known issue 2 with workaround if available]

## 📞 Support

### Getting Help:
- **Documentation:** Check docs/ folder for detailed guides
- **Issues:** Report bugs via GitHub issues
- **Questions:** [Contact method or discussion forum]

### Maintainers:
- [Name] - [Role] - [Contact]
- [Name] - [Role] - [Contact]

---

**Last Updated:** {DATE} ({TIMEZONE})  
**Next Review:** [Date]

*Thank you for your interest in {PROJECT_NAME}!*
```

---

# PHASE 4: GIT WORKFLOW

## Single Branch Strategy

### 🌿 Simplified Branch Model:
```
main (primary development branch)
├── Regular development commits
├── Feature implementations
├── Bug fixes
└── Documentation updates
```

**Benefits of Single Branch:**
- Simple workflow for solo development
- No merge conflicts between branches
- Clear linear history
- Easy rollback to any point

**When to Consider Multiple Branches:**
- Multiple developers working simultaneously
- Need to maintain separate production version
- Working on experimental features

## Commit Message Conventions

### 📝 Commit Message Format:
```
type: brief description (50 characters max)

Optional longer description explaining the change in more detail.
Can include multiple paragraphs if needed.

- Specific change 1
- Specific change 2  
- Specific change 3

🤖 Generated with Claude Code

Co-Authored-By: Claude <noreply@anthropic.com>
```

### 🏷️ Commit Types:
- **feat:** New feature or functionality
- **fix:** Bug fix or error correction
- **docs:** Documentation updates
- **refactor:** Code refactoring (no functionality change)
- **test:** Adding or updating tests
- **chore:** Maintenance tasks (dependencies, build, etc.)
- **style:** Code style/formatting changes
- **perf:** Performance improvements

### 📋 Commit Examples:
```bash
# Feature addition
git commit -m "feat: Add user authentication system

Implemented complete user authentication with:
- Login/logout functionality
- Password hashing with bcrypt
- Session management
- Input validation

🤖 Generated with Claude Code

Co-Authored-By: Claude <noreply@anthropic.com>"

# Bug fix
git commit -m "fix: Resolve database connection timeout issue

- Increased connection timeout from 5s to 30s
- Added connection retry logic
- Improved error handling and logging

🤖 Generated with Claude Code

Co-Authored-By: Claude <noreply@anthropic.com>"

# Documentation update
git commit -m "docs: Update API documentation and examples

- Added new endpoint documentation
- Updated usage examples
- Fixed typos in README

🤖 Generated with Claude Code

Co-Authored-By: Claude <noreply@anthropic.com>"
```

## Regular Push Schedule

### ⏰ Push Strategy:
**Frequent Commits:** Every 30-45 minutes during active development
**Logical Units:** After completing each distinct feature/fix
**Session Boundaries:** Always before ending development session
**Documentation:** Immediately after significant doc updates

### 🔄 Git Command Patterns:
```bash
# Navigate to project (always use quotes for spaces)
cd "{REPO_PATH}"

# Check current status
git status

# Add all changes
git add .

# Or add specific files
git add src/component.py docs/README.md

# Commit with descriptive message
git commit -m "type: description

Detailed explanation if needed

🤖 Generated with Claude Code

Co-Authored-By: Claude <noreply@anthropic.com>"

# Push to remote
git push origin {MAIN_BRANCH}

# Check final status
git status
```

### 📊 Pre-Commit Checklist:
- [ ] All changes are tested and working
- [ ] Documentation is updated if needed
- [ ] Commit message follows convention
- [ ] No sensitive data is included
- [ ] File formatting is consistent

---

# PHASE 5: TEMPLATES & EXAMPLES

## Ready-to-Use Code Templates

### 🏷️ Function/Method Header Template:
```
/**
 * [Function Name] - Brief description
 * 
 * Purpose: [What this function does and why]
 * 
 * @param {type} paramName - Description of parameter
 * @param {type} paramName - Description of parameter
 * @returns {type} Description of return value
 * 
 * @example
 * // Usage example
 * const result = functionName(param1, param2);
 * 
 * @author [Your Name]
 * @created {DATE}
 * @modified {DATE}
 * 
 * @throws {ErrorType} Description of when this error occurs
 * 
 * @see [Related functions or documentation]
 * 
 * @performance O(n) time complexity, O(1) space complexity
 * 
 * @todo [Any known improvements or limitations]
 */
```

### 🏗️ Class/Module Template:
```
/**
 * =====================================================================
 * CLASS: [ClassName] 
 * =====================================================================
 * 
 * Purpose: [What this class represents and its role]
 * 
 * Responsibilities:
 * - [Responsibility 1]
 * - [Responsibility 2]
 * - [Responsibility 3]
 * 
 * Dependencies:
 * - [External dependency 1]
 * - [Internal dependency 2]
 * 
 * Usage:
 * ```
 * const instance = new ClassName(params);
 * instance.method();
 * ```
 * 
 * @author [Your Name]
 * @version 1.0.0
 * @created {DATE}
 * @modified {DATE}
 * 
 * @see [Related classes or documentation]
 * 
 * =====================================================================
 */
```

### ⚙️ Configuration File Template:
```json
{
  "project": {
    "name": "{PROJECT_NAME}",
    "version": "1.0.0",
    "description": "[Project description]",
    "repository": "{REPO_URL}",
    "created": "{DATE}"
  },
  "development": {
    "timezone": "{TIMEZONE}",
    "main_branch": "{MAIN_BRANCH}",
    "commit_convention": "conventional",
    "auto_formatting": true
  },
  "documentation": {
    "auto_headers": true,
    "progress_tracking": true,
    "api_docs": false
  },
  "workflow": {
    "todo_management": true,
    "session_tracking": true,
    "regular_commits": true
  }
}
```

## Documentation Templates

### 📋 Feature Documentation Template:
```markdown
# Feature: [Feature Name]

**Status:** [Planning/Development/Testing/Complete]  
**Priority:** [High/Medium/Low]  
**Estimated Effort:** [Small/Medium/Large]  
**Assigned To:** [Name]  
**Created:** {DATE} ({TIMEZONE})

## Overview

### Purpose:
[What this feature does and why it's needed]

### User Story:
As a [user type], I want [functionality] so that [benefit].

### Acceptance Criteria:
- [ ] [Specific requirement 1]
- [ ] [Specific requirement 2]
- [ ] [Specific requirement 3]

## Technical Details

### Components Affected:
- [Component 1] - [What changes]
- [Component 2] - [What changes]

### Implementation Notes:
- [Technical consideration 1]
- [Technical consideration 2]

### Dependencies:
- [Internal dependency]
- [External dependency]

## Testing

### Test Cases:
- [ ] [Test case 1]
- [ ] [Test case 2]
- [ ] [Test case 3]

### Performance Considerations:
[Any performance impacts or requirements]

## Documentation Updates Needed:
- [ ] Update API documentation
- [ ] Update user guide
- [ ] Update README if needed

## Completion Checklist:
- [ ] Implementation complete
- [ ] Tests written and passing
- [ ] Code reviewed
- [ ] Documentation updated
- [ ] Feature tested manually

---
*Feature documented by: [Your Name]*  
*Last updated: {DATE} ({TIMEZONE})*
```

### 🐛 Bug Report Template:
```markdown
# Bug Report: [Brief Description]

**Priority:** [Critical/High/Medium/Low]  
**Status:** [New/In Progress/Resolved/Closed]  
**Reporter:** [Name]  
**Assigned To:** [Name]  
**Created:** {DATE} ({TIMEZONE})

## Summary
[Clear, concise description of the bug]

## Environment
- **Version:** [Software version]
- **Platform:** [OS/Browser/etc.]
- **Configuration:** [Relevant config details]

## Steps to Reproduce
1. [First step]
2. [Second step]
3. [Third step]

## Expected Behavior
[What should happen]

## Actual Behavior
[What actually happens]

## Screenshots/Logs
[Include any relevant screenshots or log entries]

## Workaround
[Any temporary workaround if available]

## Root Cause Analysis
[Once investigated - what caused the issue]

## Solution
[How the bug was fixed]

## Prevention
[How to prevent similar bugs in the future]

## Testing
- [ ] Fix implemented
- [ ] Unit tests added/updated
- [ ] Manual testing completed
- [ ] Regression testing completed

---
*Bug reported by: [Your Name]*  
*Last updated: {DATE} ({TIMEZONE})*
```

## Git Command Examples

### 📝 Common Git Operations:
```bash
# ========================================
# REPOSITORY SETUP
# ========================================

# Clone repository (use quotes for paths with spaces)
git clone {REPO_URL} "{LOCAL_PATH}"

# Navigate to project
cd "{REPO_PATH}"

# Check repository status
git status
git branch -v
git remote -v

# ========================================
# DAILY WORKFLOW
# ========================================

# Start of session - check status
git status
git pull origin {MAIN_BRANCH}  # if working with others

# Stage changes
git add .                      # Add all changes
git add "src/specific file.py" # Add specific file (quotes for spaces)
git add src/                   # Add specific directory

# Commit changes
git commit -m "feat: add new feature

Detailed description of changes made:
- Added new functionality
- Updated documentation  
- Fixed related issues

🤖 Generated with Claude Code

Co-Authored-By: Claude <noreply@anthropic.com>"

# Push changes
git push origin {MAIN_BRANCH}

# ========================================
# FILE OPERATIONS
# ========================================

# Check what changed
git diff                       # See unstaged changes
git diff --staged             # See staged changes
git diff HEAD~1               # Compare with previous commit

# File history
git log --oneline -10         # Recent commits
git log --oneline --follow "specific file.py"  # File history

# Undo changes
git checkout -- "file.py"    # Undo unstaged changes to file
git reset HEAD "file.py"      # Unstage file
git reset --soft HEAD~1       # Undo last commit (keep changes)

# ========================================
# REPOSITORY MAINTENANCE
# ========================================

# Clean up
git clean -n                  # Preview what would be deleted
git clean -f                  # Remove untracked files

# Repository statistics
git log --oneline | wc -l     # Count commits
git ls-files | wc -l          # Count tracked files

# ========================================
# PATHS WITH SPACES - CRITICAL
# ========================================

# ALWAYS use quotes for paths with spaces
cd "C:\Projects\My Project Name"     # ✅ Correct
cd C:\Projects\My Project Name       # ❌ Will fail

git add "src/file with spaces.py"   # ✅ Correct  
git add src/file with spaces.py     # ❌ Will fail

# ========================================
# TROUBLESHOOTING
# ========================================

# If push fails
git pull origin {MAIN_BRANCH}       # Pull remote changes first
git push origin {MAIN_BRANCH}       # Then push

# If conflicts occur
git status                          # See conflicted files
# Edit files to resolve conflicts
git add .                          # Stage resolved files
git commit -m "fix: resolve merge conflicts"

# Emergency reset (USE CAREFULLY)
git reset --hard HEAD              # Lose all uncommitted changes
git reset --hard origin/{MAIN_BRANCH}  # Reset to match remote
```

---

# APPENDIX A: TOOL USAGE PATTERNS

## TodoWrite Patterns

### 🎯 Effective Todo Management:

**Session Planning Pattern:**
```
# At start of session - plan the work
TodoWrite todos=[
    {"content": "Review project status and last session", "status": "in_progress"},
    {"content": "Plan current session objectives", "status": "pending"},
    {"content": "[Specific task 1]", "status": "pending"},
    {"content": "[Specific task 2]", "status": "pending"},
    {"content": "Update documentation", "status": "pending"},
    {"content": "Commit and push changes", "status": "pending"}
]
```

**Progressive Updates Pattern:**
```
# As work progresses - update regularly
TodoWrite todos=[
    {"content": "Review project status and last session", "status": "completed"},
    {"content": "Plan current session objectives", "status": "completed"},  
    {"content": "[Specific task 1]", "status": "in_progress"},
    {"content": "[Specific task 2]", "status": "pending"},
    {"content": "Update documentation", "status": "pending"},
    {"content": "Commit and push changes", "status": "pending"}
]
```

**Task Breakdown Pattern:**
```
# When encountering complex tasks - break them down
TodoWrite todos=[
    {"content": "Implement authentication system", "status": "in_progress"},
    {"content": "- Create user model", "status": "pending"},
    {"content": "- Add password hashing", "status": "pending"},
    {"content": "- Implement login/logout", "status": "pending"},
    {"content": "- Add session management", "status": "pending"},
    {"content": "Test authentication system", "status": "pending"}
]
```

## File Operations Best Practices

### 📁 Working with Files:

**Read Operations:**
```bash
# Read entire files for small files
Read file_path="path/to/file.py"

# Read partial files for large files  
Read file_path="path/to/large_file.py" limit=50
Read file_path="path/to/large_file.py" offset=100 limit=50

# Read multiple files efficiently
Read file_path="file1.py"
Read file_path="file2.py"  
Read file_path="file3.py"
```

**Search Operations:**
```bash
# Find files by pattern
Glob pattern="**/*.py"                    # All Python files
Glob pattern="src/**/*.js"                # JS files in src
Glob pattern="**/test_*.py"               # Test files

# Search content
Grep pattern="function.*login" output_mode="files_with_matches"
Grep pattern="TODO|FIXME" output_mode="content" -n -A 2
Grep pattern="class.*Controller" output_mode="content" -B 2 -A 5
```

**File Modifications:**
```bash
# Single file edits
Edit file_path="path/to/file.py" old_string="old code" new_string="new code"

# Multiple edits in one file
MultiEdit file_path="path/to/file.py" edits=[
    {"old_string": "old1", "new_string": "new1"},
    {"old_string": "old2", "new_string": "new2"}
]

# Create new files
Write file_path="path/to/new_file.py" content="file content here"
```

## Development Session Patterns

### ⏰ Effective Session Management:

**Session Startup Routine:**
1. **Environment Check:**
   ```bash
   cd "{REPO_PATH}"
   git status
   git branch -v
   ```

2. **Review Previous Work:**
   ```bash
   Read file_path="DEVELOPMENT_PROGRESS_LOG.md" limit=30
   ```

3. **Plan Current Session:**
   ```
   TodoWrite todos=[...]  # Plan the session
   ```

**During Development:**
- Update todos every 30-45 minutes
- Make commits at logical breakpoints
- Update progress log with major milestones
- Take notes of important decisions

**Session Closure:**
- Complete current logical work unit
- Update progress log with session summary
- Commit all changes
- Plan next session priorities

---

# APPENDIX B: TROUBLESHOOTING

## Common Issues and Solutions

### 🐛 Git Issues:

**Path with Spaces Problems:**
```bash
# Problem: Commands fail with paths containing spaces
# Solution: Always use quotes

# ❌ Wrong:
cd C:\My Projects\Project Name
git add src/file name.py

# ✅ Correct:
cd "C:\My Projects\Project Name"  
git add "src/file name.py"
```

**Push Rejected:**
```bash
# Problem: Git push is rejected
# Solution: Pull first, then push

git pull origin {MAIN_BRANCH}
# Resolve any conflicts if they occur
git push origin {MAIN_BRANCH}
```

**Uncommitted Changes:**
```bash
# Problem: Switching branches with uncommitted changes
# Solution: Commit or stash changes

# Option 1: Commit changes
git add .
git commit -m "temp: work in progress"

# Option 2: Stash changes  
git stash
# Later: git stash pop
```

### 📁 File Operation Issues:

**File Not Found:**
```bash
# Problem: File path not found
# Solution: Verify path exists

# Check if file exists
LS path="parent/directory"
Glob pattern="**/target_file.*"

# Use absolute paths when possible
Read file_path="/absolute/path/to/file.py"
```

**Large File Handling:**
```bash
# Problem: File too large to read completely
# Solution: Read in chunks

Read file_path="large_file.py" limit=100
Read file_path="large_file.py" offset=100 limit=100
```

**Edit Failures:**
```bash
# Problem: Edit tool can't find text to replace
# Solution: Verify exact text match

# First read the file to see exact content
Read file_path="file.py" limit=20

# Then edit with exact match (including whitespace)
Edit file_path="file.py" old_string="    def function():" new_string="    def new_function():"
```

### 🔧 Development Workflow Issues:

**Lost Context Between Sessions:**
```bash
# Problem: Can't remember what was being worked on
# Solution: Maintain detailed progress logs

# Always update before ending session:
# 1. Update DEVELOPMENT_PROGRESS_LOG.md
# 2. Update todos with current status
# 3. Add specific notes about next steps
```

**Todo Management Chaos:**
```bash
# Problem: Too many todos, unclear priorities
# Solution: Use structured todo management

# Keep todos focused and specific
# Maximum 5-7 todos at once
# Use hierarchical structure for complex tasks
# Update status frequently
```

**Inconsistent Documentation:**
```bash
# Problem: Documentation gets out of sync
# Solution: Document as you code

# Update docs in same commit as code changes
# Use templates for consistency
# Review documentation regularly
```

## Emergency Procedures

### 🚨 Critical Issues:

**Accidental File Deletion:**
```bash
# If file deleted but not committed:
git checkout HEAD -- "deleted_file.py"

# If deletion was committed:
git log --oneline --follow "deleted_file.py"  # Find last commit with file
git checkout [commit_hash] -- "deleted_file.py"
```

**Repository Corruption:**
```bash
# Nuclear option - re-clone repository:
cd ..
mv "old_repo" "old_repo_backup"
git clone {REPO_URL} "new_repo"
# Copy any uncommitted work from backup
```

**Lost Work Recovery:**
```bash
# Check git reflog for lost commits:
git reflog
git checkout [lost_commit_hash]
git branch recovery_branch  # Save recovered work
```

---

# APPENDIX C: PROJECT ADAPTATION GUIDE

## Adapting for Different Project Types

### 🌐 Web Development Projects:

**Additional File Patterns:**
```bash
# Frontend files
Glob pattern="**/*.html"
Glob pattern="**/*.css"  
Glob pattern="**/*.scss"
Glob pattern="**/*.js"
Glob pattern="**/*.ts"
Glob pattern="**/*.jsx"
Glob pattern="**/*.tsx"
Glob pattern="**/*.vue"

# Configuration files
Glob pattern="**/package.json"
Glob pattern="**/webpack.config.js"
Glob pattern="**/.env*"
```

**Web-Specific Documentation Sections:**
- API endpoints documentation
- Component library documentation
- Deployment instructions
- Browser compatibility notes
- Performance optimization guide

### 🖥️ Desktop Application Projects:

**Additional File Patterns:**
```bash
# Application files
Glob pattern="**/*.exe"
Glob pattern="**/*.dll"
Glob pattern="**/*.so"
Glob pattern="**/*.app"

# Resource files
Glob pattern="**/*.ico"
Glob pattern="**/*.png"
Glob pattern="**/*.jpg"
Glob pattern="**/*.xaml"
```

**Desktop-Specific Documentation Sections:**
- Installation instructions  
- System requirements
- User interface guide
- Configuration options
- Troubleshooting guide

### 📱 Mobile Development Projects:

**Additional File Patterns:**
```bash
# Mobile files
Glob pattern="**/*.apk"
Glob pattern="**/*.ipa"
Glob pattern="**/*.swift"
Glob pattern="**/*.kt"
Glob pattern="**/*.dart"

# Resource files  
Glob pattern="**/*.storyboard"
Glob pattern="**/*.xib"
Glob pattern="**/AndroidManifest.xml"
```

### 🔧 Embedded/PLC Projects:

**Additional File Patterns:**
```bash
# PLC files
Glob pattern="**/*.st"         # Structured Text
Glob pattern="**/*.ld"         # Ladder Diagram  
Glob pattern="**/*.fbd"        # Function Block Diagram
Glob pattern="**/*.sfc"        # Sequential Function Chart

# Configuration files
Glob pattern="**/*.xml"
Glob pattern="**/*.eds"        # Electronic Data Sheets
Glob pattern="**/*.dcf"        # Device Configuration Files
```

**Embedded-Specific Documentation Sections:**
- Hardware requirements
- Communication protocols
- Safety considerations
- Real-time constraints
- Debugging procedures

### 🔬 Data Science Projects:

**Additional File Patterns:**
```bash
# Data science files
Glob pattern="**/*.ipynb"      # Jupyter notebooks
Glob pattern="**/*.csv"        # Data files
Glob pattern="**/*.json"       # Data files
Glob pattern="**/*.pkl"        # Pickle files
Glob pattern="**/*.h5"         # HDF5 files

# Analysis files
Glob pattern="**/*.R"          # R scripts
Glob pattern="**/*.sql"        # SQL queries
```

**Data Science-Specific Documentation Sections:**
- Data sources and descriptions
- Analysis methodology
- Model performance metrics
- Reproducibility instructions
- Ethical considerations

## Technology-Specific Customizations

### 🐍 Python Projects:
```bash
# Python-specific inventory
Glob pattern="**/*.py"
Glob pattern="**/requirements.txt"
Glob pattern="**/setup.py"
Glob pattern="**/pyproject.toml"
Grep pattern="def |class |import " output_mode="count"
```

### ☕ Java Projects:
```bash
# Java-specific inventory  
Glob pattern="**/*.java"
Glob pattern="**/pom.xml"
Glob pattern="**/build.gradle"
Grep pattern="public class |interface |package " output_mode="count"
```

### 🌐 JavaScript/Node.js Projects:
```bash
# JavaScript-specific inventory
Glob pattern="**/*.js"
Glob pattern="**/*.ts"  
Glob pattern="**/package.json"
Glob pattern="**/yarn.lock"
Grep pattern="function |class |const |let " output_mode="count"
```

---

# 🎯 FINAL CHECKLIST

## Before Starting Development:

### Phase 0 Completion:
- [ ] Repository structure mapped
- [ ] Code inventory completed  
- [ ] Documentation audit finished
- [ ] Conventions identified
- [ ] Components assessed
- [ ] Inventory report generated

### Phase 1 Setup:
- [ ] Git configured properly
- [ ] Project structure organized
- [ ] Initial documentation created
- [ ] README.md written
- [ ] DEVELOPMENT_PROGRESS_LOG.md started

### Phase 2-4 Preparation:
- [ ] TodoWrite patterns understood
- [ ] Session management plan ready
- [ ] Documentation standards defined
- [ ] Git workflow established
- [ ] Professional headers template ready

## During Development:

### Every Session:
- [ ] Update todos regularly
- [ ] Maintain progress log
- [ ] Follow commit conventions
- [ ] Update documentation
- [ ] Push changes regularly

### Weekly Reviews:
- [ ] Review project progress
- [ ] Update documentation
- [ ] Clean up todos
- [ ] Assess code quality
- [ ] Plan next priorities

---

**🎉 CONGRATULATIONS!**

You now have a complete workflow system for professional project development with Claude Code. This prompt provides everything needed to maintain high standards, consistent documentation, and effective progress tracking throughout your project lifecycle.

**Remember:** Adapt this template to your specific project needs, maintain consistency in your chosen patterns, and always prioritize clear documentation and regular progress tracking.

**Version:** v1.0.0  
**Created:** 27.08.2025 (Warsaw Time)  
**Usage:** Universal - adapt placeholders for any project  

*Happy coding! 🚀*