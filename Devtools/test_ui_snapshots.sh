#!/bin/bash

# UI Snapshot Testing Helper Script
# Usage: ./test_ui_snapshots.sh [options]

set -e

# Load common variables and functions
source "$(dirname -- "$(readlink -f "${BASH_SOURCE}")")"/common_variables.sh

# Use local PlatformIO installation
PLATFORMIO_PATH="$HOME/.platformio/penv/bin/platformio"

# Check if local PlatformIO exists, fallback to system if not
if [ ! -f "$PLATFORMIO_PATH" ]; then
    PLATFORMIO_PATH="platformio"
fi

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

print_usage() {
    echo "UI Snapshot Testing Helper"
    echo ""
    echo "Usage: $0 [options]"
    echo ""
    echo "Options:"
    echo "  -u, --update     Update snapshots (sets UPDATE_SNAPSHOTS=1)"
    echo "  -v, --verbose    Run tests with verbose output"
    echo "  -c, --clean      Clean test build artifacts"
    echo "  -h, --help       Show this help message"
    echo ""
    echo "Examples:"
    echo "  $0                  # Run snapshot tests"
    echo "  $0 --update         # Update all snapshots"
    echo "  $0 --verbose        # Run with verbose output"
    echo "  $0 --clean --update # Clean and update snapshots"
}

UPDATE_SNAPSHOTS=0
VERBOSE=""
CLEAN=0

# Parse command line arguments
while [[ $# -gt 0 ]]; do
    case $1 in
        -u|--update)
            UPDATE_SNAPSHOTS=1
            shift
            ;;
        -v|--verbose)
            VERBOSE="-v"
            shift
            ;;
        -c|--clean)
            CLEAN=1
            shift
            ;;
        -h|--help)
            print_usage
            exit 0
            ;;
        *)
            echo -e "${RED}Error: Unknown option $1${NC}"
            print_usage
            exit 1
            ;;
    esac
done

cd "$UM_ROOT_PATH"

# Clean if requested
if [ $CLEAN -eq 1 ]; then
    echo -e "${YELLOW}Cleaning test build artifacts...${NC}"
    $PLATFORMIO_PATH run --target clean -e native
fi

# Run tests
echo -e "${GREEN}Running UI snapshot tests...${NC}"
if [ $UPDATE_SNAPSHOTS -eq 1 ]; then
    echo -e "${YELLOW}UPDATE_SNAPSHOTS mode enabled${NC}"
    UPDATE_SNAPSHOTS=1 $PLATFORMIO_PATH test -e native -f test_ui_snapshot $VERBOSE
else
    $PLATFORMIO_PATH test -e native -f test_ui_snapshot $VERBOSE
fi

# Check exit code
if [ $? -eq 0 ]; then
    echo -e "${GREEN}✓ All tests passed!${NC}"
    
    # Show snapshot status
    if [ $UPDATE_SNAPSHOTS -eq 1 ]; then
        echo ""
        echo "Snapshots updated. Review changes with:"
        echo "  git diff test/snapshots/ui/"
    fi
else
    echo -e "${RED}✗ Tests failed${NC}"
    echo ""
    echo "Check .actual files in test/snapshots/ui/ for differences"
    echo "If changes are intentional, update snapshots with:"
    echo "  $0 --update"
    exit 1
fi
