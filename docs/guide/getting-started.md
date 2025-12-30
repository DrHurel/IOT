# Getting Started

This guide will help you set up your development environment and build the PlantNanny system.

## Prerequisites

### Hardware Requirements
- ESP32 development board (LilyGo T-Display recommended)
- Environmental sensors (temperature, humidity, light)
- Water pump and relay module
- USB cable for programming

### Software Requirements
- [PlatformIO](https://platformio.org/) (for embedded development)
- [Python 3.x](https://www.python.org/) (for build tools)
- [Node.js](https://nodejs.org/) (for documentation)
- [Git](https://git-scm.com/) (for version control)

## Installation

### 1. Clone the Repository

```bash
git clone https://github.com/DrHurel/IOT.git
cd IOT
```

### 2. Run Setup Script

The setup script will install dependencies and configure your environment:

```bash
./Devtools/setup.sh
```

This script will:
- Install PlatformIO dependencies
- Set up Python virtual environment
- Install required Python packages
- Configure the build environment

### 3. Install Node Dependencies

For documentation development:

```bash
npm install
```

## Project Structure

```
IOT/
├── src/                    # Source code for firmware
├── include/                # Header files
├── lib/                    # Local libraries
├── test/                   # Unit tests
├── docs/                   # VitePress documentation
├── Devtools/               # Build and development scripts
├── RestApi/                # OpenAPI specifications
└── platformio.ini          # PlatformIO configuration
```

## Quick Verification

### Build the Firmware

```bash
# Build for LilyGo T-Display
pio run -e lilygo-t-display-debug

# Or use the helper script
./Devtools/build.sh
```

### Run Tests

```bash
# Run all tests
./Devtools/test.sh

# Run UI snapshot tests
./Devtools/test_ui_snapshots.sh
```

### Start Documentation Server

```bash
npm run docs:dev
```

Visit `http://localhost:5173` to view the documentation.

## Next Steps

- [Quick Start Guide](/guide/quick-start) - Build your first controller
- [Architecture Overview](/development/architecture) - Understand the system design
- [Development Guide](/development/project-structure) - Learn about the codebase

## Troubleshooting

### PlatformIO Installation Issues

If you encounter issues with PlatformIO:

```bash
# Reinstall PlatformIO
pip install -U platformio

# Update platforms and packages
pio platform update
```

### Build Errors

If you encounter build errors:

1. Clean the build directory:
   ```bash
   pio run -t clean
   ```

2. Check your `platformio.ini` configuration
3. Verify all dependencies are installed
4. Check the [Issues](https://github.com/DrHurel/IOT/issues) page

### USB Connection Issues

On Linux, you may need to add your user to the `dialout` group:

```bash
sudo usermod -a -G dialout $USER
# Log out and back in for changes to take effect
```

## Getting Help

- Check the [documentation](/guide/what-is-plantnanny)
- Review [example code](/development/code-templates)
- Open an [issue on GitHub](https://github.com/DrHurel/IOT/issues)
