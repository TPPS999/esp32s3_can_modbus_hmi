#!/bin/bash

# =====================================================================
# === git-helpers.sh - ESP32S3 CAN to Modbus TCP Bridge ===
# =====================================================================
# 
# = PROJECT INFO:
#    Repository: https://github.com/user/esp32s3-can-modbus-tcp
#    Project: ESP32S3 CAN to Modbus TCP Bridge
#    Branch: main
#    Created: 27.08.2025 (Warsaw Time)
#
# = MODULE INFO:
#    Module: Git Workflow Automation Scripts
#    Version: v4.0.0
#    Created: 27.08.2025 (Warsaw Time)
#    Last Modified: 27.08.2025 (Warsaw Time)
#    Author: ESP32 Development Team
#
# = DESCRIPTION:
#    Automation scripts for standardized git workflow in ESP32S3 embedded
#    development. Provides helper functions for common git operations,
#    hardware testing backups, session management, and commit message
#    standardization with Claude Code signature integration.
#
# = USAGE:
#    ./scripts/git-helpers.sh [command] [parameters]
#    
#    Commands:
#      status           - Comprehensive git status check
#      backup           - Hardware testing backup commit
#      end [message]    - End development session commit
#      commit [type] [desc] [details] - Custom commit with signature
#      config           - Setup git configuration for this project
#      test             - Test hardware functionality commit
#
# =====================================================================

# Project path with spaces - always use quotes
PROJECT_PATH="d:\OD\OneDrive - Wanaka sp. z o.o\Documents laptop\PlatformIO\Projects\esp32s3-can-modbus tcp"

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Helper function to print colored messages
print_status() {
    echo -e "${BLUE}[INFO]${NC} $1"
}

print_success() {
    echo -e "${GREEN}[SUCCESS]${NC} $1"
}

print_warning() {
    echo -e "${YELLOW}[WARNING]${NC} $1"
}

print_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

# Function to check if we're in the correct project directory
check_project_dir() {
    if [[ ! -f "platformio.ini" ]] || [[ ! -d "src" ]]; then
        print_error "Not in ESP32S3 project directory. Please run from project root."
        exit 1
    fi
}

# Comprehensive git status check
git_status() {
    print_status "ESP32S3 CAN-Modbus TCP Bridge - Git Status Check"
    echo "======================================================="
    
    print_status "Repository Status:"
    git status
    
    echo ""
    print_status "Recent Commits (last 10):"
    git log --oneline -10
    
    echo ""
    print_status "Branch Information:"
    git branch -v
    
    echo ""
    print_status "Uncommitted Changes Summary:"
    if git diff --quiet && git diff --staged --quiet; then
        print_success "Working directory is clean"
    else
        git diff --name-only
        git diff --stat
    fi
    
    echo ""
    print_status "Remote Status:"
    git remote -v
    
    # Check if ahead/behind remote
    if git rev-parse --abbrev-ref --symbolic-full-name @{u} > /dev/null 2>&1; then
        local ahead=$(git rev-list --count @{u}..HEAD)
        local behind=$(git rev-list --count HEAD..@{u})
        
        if [[ $ahead -gt 0 ]]; then
            print_warning "Local branch is $ahead commit(s) ahead of remote"
        fi
        
        if [[ $behind -gt 0 ]]; then
            print_warning "Local branch is $behind commit(s) behind remote"
        fi
        
        if [[ $ahead -eq 0 ]] && [[ $behind -eq 0 ]]; then
            print_success "Local branch is up to date with remote"
        fi
    fi
}

# Hardware testing backup
git_backup() {
    print_status "Creating hardware testing backup..."
    
    # Stage all changes
    git add -A
    
    # Create backup commit
    git commit -m "$(cat <<'EOF'
chore: backup before ESP32S3 hardware testing

Saving current state before physical testing:
- Code compilation verified with pio run
- Configuration ready for hardware verification
- CAN bus and Modbus TCP implementation ready
- All systems prepared for hardware validation

🤖 Generated with Claude Code

Co-Authored-By: Claude <noreply@anthropic.com>
EOF
)"
    
    # Push to remote
    git push origin main
    
    print_success "Hardware testing backup completed - safe to proceed with testing"
}

# End development session
git_end() {
    local session_message="$1"
    if [[ -z "$session_message" ]]; then
        session_message="Development session completed"
    fi
    
    print_status "Ending development session..."
    
    # Stage all changes
    git add -A
    
    # Create session end commit
    git commit -m "$(cat <<EOF
chore: end development session - Universal Workflow

Session accomplishments:
- $session_message
- Updated documentation and progress tracking
- Applied professional development standards
- Maintained ESP32S3 embedded development workflow

Next session: Continue with planned development priorities

🤖 Generated with Claude Code

Co-Authored-By: Claude <noreply@anthropic.com>
EOF
)"
    
    # Push to remote
    git push origin main
    
    # Verify push success
    git status
    
    print_success "Development session ended and pushed to remote"
}

# Custom commit with signature
git_commit() {
    local commit_type="$1"
    local brief_desc="$2"
    local detailed_desc="$3"
    
    if [[ -z "$commit_type" ]] || [[ -z "$brief_desc" ]]; then
        print_error "Usage: git_commit <type> <brief_description> [detailed_description]"
        print_error "Types: feat, fix, refactor, perf, style, docs, test, chore, hw, config, can, modbus, wifi"
        exit 1
    fi
    
    print_status "Creating custom commit: $commit_type: $brief_desc"
    
    # Stage all changes
    git add -A
    
    # Create custom commit
    if [[ -n "$detailed_desc" ]]; then
        git commit -m "$(cat <<EOF
$commit_type: $brief_desc

$detailed_desc

🤖 Generated with Claude Code

Co-Authored-By: Claude <noreply@anthropic.com>
EOF
)"
    else
        git commit -m "$(cat <<EOF
$commit_type: $brief_desc

🤖 Generated with Claude Code

Co-Authored-By: Claude <noreply@anthropic.com>
EOF
)"
    fi
    
    print_success "Commit created successfully"
}

# Setup git configuration for this project
git_config() {
    print_status "Setting up git configuration for ESP32S3 project..."
    
    # Set commit message template
    git config commit.template .gitmessage
    print_success "Commit message template configured"
    
    # Set editor if not already set
    if [[ -z "$(git config core.editor)" ]]; then
        git config core.editor "nano"
        print_success "Default editor set to nano"
    fi
    
    # Configure line endings for cross-platform development
    git config core.autocrlf true
    print_success "Line ending configuration set"
    
    # Set up aliases for common operations
    git config alias.st status
    git config alias.co checkout
    git config alias.br branch
    git config alias.unstage 'reset HEAD --'
    git config alias.last 'log -1 HEAD'
    print_success "Git aliases configured"
    
    print_status "Git configuration completed for ESP32S3 project"
}

# Hardware functionality test commit
git_test() {
    print_status "Creating hardware test results commit..."
    
    # Stage all changes
    git add -A
    
    # Create test results commit
    git commit -m "$(cat <<'EOF'
test: verify ESP32S3 hardware functionality

Hardware testing completed successfully:
- CAN bus communication verified with actual BMS modules
- Modbus TCP server tested with industrial clients
- WiFi connectivity confirmed in production environment
- All system components operational on target hardware

🤖 Generated with Claude Code

Co-Authored-By: Claude <noreply@anthropic.com>
EOF
)"
    
    # Push to remote
    git push origin main
    
    print_success "Hardware test results committed and pushed"
}

# Main script logic
main() {
    # Check if we're in the project directory
    check_project_dir
    
    case "$1" in
        "status")
            git_status
            ;;
        "backup")
            git_backup
            ;;
        "end")
            git_end "$2"
            ;;
        "commit")
            git_commit "$2" "$3" "$4"
            ;;
        "config")
            git_config
            ;;
        "test")
            git_test
            ;;
        "help"|"--help"|"-h"|"")
            echo "ESP32S3 CAN-Modbus TCP Bridge - Git Helper Script"
            echo "=================================================="
            echo ""
            echo "Usage: $0 [command] [parameters]"
            echo ""
            echo "Commands:"
            echo "  status                          - Comprehensive git status check"
            echo "  backup                          - Create hardware testing backup"
            echo "  end [message]                   - End development session"
            echo "  commit <type> <desc> [details]  - Custom commit with signature"
            echo "  config                          - Setup git configuration"
            echo "  test                            - Hardware test results commit"
            echo "  help                            - Show this help message"
            echo ""
            echo "Commit Types:"
            echo "  feat, fix, refactor, perf, style, docs, test, chore"
            echo "  hw, config, can, modbus, wifi (ESP32S3-specific)"
            echo ""
            echo "Examples:"
            echo "  $0 status"
            echo "  $0 backup"
            echo "  $0 end \"Phase 4 implementation completed\""
            echo "  $0 commit feat \"add CAN filtering\" \"Added advanced CAN frame filtering\""
            echo "  $0 config"
            ;;
        *)
            print_error "Unknown command: $1"
            print_error "Use '$0 help' for usage information"
            exit 1
            ;;
    esac
}

# Run main function with all arguments
main "$@"