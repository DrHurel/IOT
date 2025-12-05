# Build System

Understanding PlantNanny's build system, compilation process, and development tools.

## PlatformIO

PlantNanny uses [PlatformIO](https://platformio.org/) as its build system, providing:
- Multi-platform support
- Dependency management
- Testing framework
- Serial monitoring
- OTA updates

## Configuration File

The `platformio.ini` file defines all build configurations:

```ini
[platformio]
default_envs = lilygo-t-display-debug

[env]
test_framework = unity
test_build_src = yes
```

## Build Environments

### Development Environment

**`lilygo-t-display-debug`** - Main development configuration:

```ini
[env:lilygo-t-display-debug]
platform = espressif32
board = lilygo-t-display
framework = arduino
build_flags = 
    -std=gnu++23        # C++23 standard
    -frtti              # Runtime type information
    -I src/libs         # Include path
    -DDEBUG             # Debug mode
    # Display configuration
    -DUSER_SETUP_LOADED=1
    -DST7789_DRIVER=1
    -DTFT_WIDTH=135
    -DTFT_HEIGHT=240
    # ... more flags
```

### Testing Environment

**`native`** - Run tests on your development machine:

```ini
[env:native]
platform = native
test_framework = unity
test_build_src = yes
build_flags = 
    -std=gnu++23
    -I src/libs
    -I test/mocks
```

## Build Flags

### Compiler Flags

- `-std=gnu++23` - Use C++23 with GNU extensions
- `-frtti` - Enable runtime type information
- `-fexceptions` - Enable C++ exceptions
- `-Os` - Optimize for size (production)
- `-O0` - No optimization (debug)

### Debug Flags

```ini
build_flags = 
    -DDEBUG                    # Enable debug mode
    -DCORE_DEBUG_LEVEL=5       # Verbose ESP32 logs
    -DLOG_LOCAL_LEVEL=ESP_LOG_VERBOSE
```

### Display Configuration

TFT_eSPI configuration via build flags:

```ini
-DUSER_SETUP_LOADED=1    # Skip default config
-DST7789_DRIVER=1        # Display driver
-DTFT_WIDTH=135          # Display width
-DTFT_HEIGHT=240         # Display height
-DTFT_MOSI=19           # SPI pins
-DTFT_SCLK=18
-DTFT_CS=5
-DTFT_DC=16
-DTFT_RST=23
-DTFT_BL=4
```

## Building

### Using PlatformIO CLI

```bash
# Build firmware
pio run -e lilygo-t-display-debug

# Build for specific environment
pio run -e native

# Clean build
pio run -t clean

# Build all environments
pio run
```

### Using Helper Scripts

```bash
# Build using Devtools
./Devtools/build.sh

# Build with custom options
./Devtools/make.sh --release

# Show build help
./Devtools/help.sh build
```

### Using VS Code

1. Install PlatformIO IDE extension
2. Open project
3. Click PlatformIO icon in sidebar
4. Select "Build" under your environment

## Uploading

### Via USB

```bash
# Upload firmware
pio run -e lilygo-t-display-debug -t upload

# Upload and monitor
pio run -e lilygo-t-display-debug -t upload -t monitor
```

### Via OTA (Over-The-Air)

```bash
# Upload via OTA
pio run -e lilygo-t-display-debug -t upload --upload-port <IP_ADDRESS>
```

### Using Helper Script

```bash
# Upload and start monitoring
./Devtools/run.sh

# Upload only
./Devtools/run.sh --upload-only
```

## Dependencies

### Library Dependencies

Defined in `platformio.ini`:

```ini
lib_deps = 
    bodmer/TFT_eSPI@^2.5.0
    knolleary/PubSubClient@^2.8
    bblanchon/ArduinoJson@^6.21.0
    # Add more libraries
```

### Automatic Installation

PlatformIO automatically downloads and installs dependencies on first build.

### Manual Installation

```bash
# Install specific library
pio lib install "TFT_eSPI@^2.5.0"

# Search for libraries
pio lib search "mqtt"

# List installed libraries
pio lib list
```

## Testing

### Run Tests

```bash
# Run all tests
pio test

# Run specific test environment
pio test -e native

# Run specific test file
pio test -e native -f test_ui_snapshot

# Verbose output
pio test -e native -v
```

### Test Filters

```ini
# Ignore certain tests
test_ignore = 
    test_embedded_only/*
    test_real_wifi/*
```

### Using Test Scripts

```bash
# Run all tests
./Devtools/test.sh

# Run UI snapshot tests
./Devtools/test_ui_snapshots.sh

# Update snapshots
./Devtools/test_ui_snapshots.sh --update
```

## Build Targets

### Standard Targets

```bash
# Build project
pio run -t build

# Clean build files
pio run -t clean

# Upload firmware
pio run -t upload

# Upload filesystem (SPIFFS/LittleFS)
pio run -t uploadfs

# Monitor serial output
pio run -t monitor
```

### Combined Targets

```bash
# Build and upload
pio run -t upload

# Upload and monitor
pio run -t upload -t monitor

# Clean, build, and upload
pio run -t clean -t upload
```

## Serial Monitor

### Start Monitor

```bash
# Default baudrate (115200)
pio device monitor

# Custom baudrate
pio device monitor -b 9600

# Specify port
pio device monitor -p /dev/ttyUSB0
```

### Monitor Filters

```bash
# Filter output
pio device monitor --filter colorize --filter time

# Available filters
pio device monitor --list-filters
```

### Exit Monitor

Press `Ctrl+C` to exit the monitor.

## Advanced Configuration

### Custom Build Scripts

Add Python scripts for build automation:

```ini
[env:custom]
extra_scripts = 
    pre:scripts/pre_build.py
    post:scripts/post_build.py
```

### Build Environments for Different Boards

```ini
[env:esp32-dev]
board = esp32dev
build_flags = ${env.build_flags}
    -DBOARD_ESP32_DEV

[env:lilygo]
board = lilygo-t-display
build_flags = ${env.build_flags}
    -DBOARD_LILYGO
```

### Production Build

```ini
[env:production]
extends = env:lilygo-t-display-debug
build_type = release
build_flags = 
    ${env:lilygo-t-display-debug.build_flags}
    -Os                    # Optimize for size
    -DNDEBUG              # Disable assertions
    -DCORE_DEBUG_LEVEL=0  # Disable logs
```

## Compilation Process

### 1. Preprocessing

- Process `#include` directives
- Expand macros
- Apply build flags

### 2. Compilation

- Compile `.cpp` files to object files (`.o`)
- Check syntax and types
- Apply optimizations

### 3. Linking

- Link object files together
- Link with libraries
- Create firmware binary (`.bin`)

### 4. Post-processing

- Calculate checksums
- Generate OTA-compatible format
- Create filesystem image

## Build Output

### Build Directory

```
.pio/build/lilygo-t-display-debug/
├── firmware.bin         # Main firmware binary
├── firmware.elf         # ELF file with debug symbols
├── partitions.bin       # Partition table
├── bootloader.bin       # ESP32 bootloader
└── src/                 # Compiled object files
```

### Firmware Size

```bash
# Show firmware size
pio run -t size

# Detailed size analysis
pio run -v -t size
```

Example output:
```
RAM:   [==        ]  15.2% (used 49816 bytes from 327680 bytes)
Flash: [====      ]  42.3% (used 554981 bytes from 1310720 bytes)
```

## Troubleshooting

### Build Errors

#### Missing Dependencies

```bash
# Update platforms
pio platform update

# Reinstall libraries
pio lib install --force
```

#### Cache Issues

```bash
# Clean build directory
pio run -t clean

# Remove entire build cache
rm -rf .pio
```

#### Compiler Errors

Check:
1. C++ standard (`-std=gnu++23`)
2. Include paths (`-I src/libs`)
3. Build flags in `platformio.ini`

### Upload Issues

#### Port Not Found

```bash
# List available ports
pio device list

# Specify port manually
pio run -t upload --upload-port /dev/ttyUSB0
```

#### Permission Denied (Linux)

```bash
# Add user to dialout group
sudo usermod -a -G dialout $USER

# Apply immediately (alternative to logout)
newgrp dialout
```

### Memory Issues

#### Stack Overflow

Increase stack size:
```cpp
// In main.cpp
void setup() {
    // Increase stack for main task
    xTaskCreate(mainTask, "main", 8192, NULL, 1, NULL);
}
```

#### Heap Fragmentation

Use:
- RAII patterns
- Smart pointers
- Fixed-size allocations

## Optimization

### Size Optimization

```ini
build_flags = 
    -Os                    # Optimize for size
    -flto                  # Link-time optimization
    -ffunction-sections    # Place functions in separate sections
    -fdata-sections        # Place data in separate sections
    -Wl,--gc-sections      # Remove unused sections
```

### Performance Optimization

```ini
build_flags = 
    -O2                    # Optimize for performance
    -finline-functions     # Aggressive inlining
```

### Debug Optimization

```ini
build_flags = 
    -O0                    # No optimization
    -g                     # Debug symbols
    -ggdb                  # GDB debug info
```

## Next Steps

- [Project Structure](/development/project-structure) - Understand the codebase
- [Code Templates](/development/code-templates) - Generate boilerplate
- [Testing Guide](/development/testing/overview) - Write and run tests
