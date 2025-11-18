#!/bin/bash
source "$(dirname -- "$(readlink -f "${BASH_SOURCE}")")"/common_variables.sh

# Use local PlatformIO installation
PLATFORMIO_PATH="$HOME/.platformio/penv/bin/platformio"

# Check if local PlatformIO exists, fallback to system if not
if [ ! -f "$PLATFORMIO_PATH" ]; then
    echo "Local PlatformIO not found at $PLATFORMIO_PATH, using system platformio"
    PLATFORMIO_PATH="platformio"
fi

# Upload and monitor
cd "$UM_ROOT_PATH"
$PLATFORMIO_PATH run --target upload && $PLATFORMIO_PATH device monitor --baud 115200