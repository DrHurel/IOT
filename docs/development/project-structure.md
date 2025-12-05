# Project Structure

Understanding the PlantNanny project organization and file structure.

## Root Directory

```
IOT/
├── docs/                   # VitePress documentation
├── Devtools/              # Development scripts
├── docker/                # Docker configurations
├── include/               # C++ header files
├── lib/                   # Local libraries
├── RestApi/               # OpenAPI specifications
├── src/                   # Source code
├── test/                  # Test files
├── archi.puml            # Architecture diagram (PlantUML)
├── package.json          # Node.js dependencies
├── platformio.ini        # PlatformIO configuration
├── README.md             # Project readme
├── requirements.txt      # Python dependencies
└── spec.md               # Project specification (French)
```

## Source Code (`src/`)

### Main Entry Point

```
src/
└── main.cpp              # Application entry point
```

The `main.cpp` file contains:
- Arduino `setup()` function - Initialization
- Arduino `loop()` function - Main event loop
- Service registration and initialization

### Libraries (`src/libs/`)

#### Common Libraries (`src/libs/common/`)

Reusable components shared across projects:

```
src/libs/common/
├── logger/               # Logging system
│   ├── Logger.cpp       # Base logger interface
│   ├── MQTTLogger.cpp   # MQTT-based remote logging
│   └── SerialLogger.cpp # Serial console logging
├── patterns/            # Design pattern implementations
│   ├── Result.cpp       # Result type for error handling
│   └── Singleton.cpp    # Singleton pattern template
├── service/             # Service framework
│   ├── Accessor.cpp     # Service accessor helper
│   ├── Context.cpp      # Execution context
│   ├── ContextSwitcher.cpp # Context management
│   ├── Registry.cpp     # Service registry (DI container)
│   └── Service.cpp      # Base service interface
├── ui/                  # UI framework
│   ├── core.cpp         # Core UI types and utilities
│   ├── UI.cpp           # Main UI manager
│   ├── Builders.cpp     # Fluent API builders
│   └── components/      # UI components
└── utils/               # Utility functions
    ├── EspError.cpp     # ESP32 error wrapper
    ├── Event.cpp        # Event system
    └── ...
```

#### Application Code (`src/libs/plant_nanny/`)

PlantNanny-specific implementation:

```
src/libs/plant_nanny/
├── App.cpp              # Main application class
└── services/            # Application services
    ├── SensorService.cpp      # Sensor data collection
    ├── WateringService.cpp    # Watering logic
    ├── DisplayService.cpp     # UI management
    ├── NetworkService.cpp     # WiFi/API communication
    └── ...
```

## Header Files (`include/`)

Headers mirror the source structure:

```
include/
├── README               # Include directory info
└── libs/
    ├── common/          # Common library headers
    │   ├── App.h
    │   ├── logger/
    │   ├── patterns/
    │   ├── service/
    │   ├── ui/
    │   └── utils/
    └── plant_nanny/     # Application headers
        ├── App.h
        └── services/
```

## Tests (`test/`)

Comprehensive test suite:

```
test/
├── README                      # Test documentation
├── test_common_init.cpp       # Common test initialization
├── snapshots/                 # Snapshot test data
│   └── ui/                    # UI component snapshots
├── test_basic/                # Basic functionality tests
├── test_bluetooth/            # Bluetooth tests
├── test_network/              # Network tests (mocked)
├── test_network_real/         # Real network tests
├── test_ota/                  # OTA update tests
├── test_real_wifi/            # Real WiFi tests
└── test_ui_snapshot/          # UI snapshot tests
    ├── test_ui_snapshot.cpp
    ├── test_ui_components.cpp
    ├── test_ui_layout_visual.cpp
    └── test_ui_text_visual.cpp
```

### Test Environments

Configured in `platformio.ini`:
- `native` - Run tests on local machine (for UI, utilities)
- `lilygo-t-display-test` - Run on actual hardware

## Development Tools (`Devtools/`)

Helper scripts for common tasks:

```
Devtools/
├── build.sh                   # Build the firmware
├── run.sh                     # Upload and monitor
├── test.sh                    # Run tests
├── test_ui_snapshots.sh      # Run UI snapshot tests
├── setup.sh                   # Initial setup
├── make.sh                    # Build with options
├── generate.sh                # Code generation
├── help.sh                    # Display help
├── common_variables.sh        # Shared variables
├── CodeTemplates/            # Code templates
│   ├── Class/
│   ├── Enum class/
│   ├── Interface/
│   ├── Singleton/
│   └── Templated class/
└── python/
    └── class_creator.py       # Class generator script
```

### Using Dev Scripts

```bash
# Build firmware
./Devtools/build.sh

# Upload and monitor
./Devtools/run.sh

# Run all tests
./Devtools/test.sh

# Run UI snapshot tests
./Devtools/test_ui_snapshots.sh

# Generate new class from template
./Devtools/generate.sh
```

## API Specifications (`RestApi/`)

OpenAPI 3.0 specifications for RESTful APIs:

```
RestApi/
├── controller.yaml           # Controller API
├── server.yaml              # Server API
└── common/                  # Shared definitions
    ├── parameters.yaml
    ├── responses.yaml
    ├── schemas.yaml        # Data models
    └── security.yaml       # Auth schemes
```

## Documentation (`docs/`)

VitePress-based documentation:

```
docs/
├── .vitepress/             # VitePress configuration
│   ├── config.mts         # Site configuration
│   └── theme/             # Custom theme
├── index.md               # Home page
├── guide/                 # User guides
├── api/                   # API documentation
├── development/           # Developer docs
└── public/                # Static assets
```

## Docker (`docker/`)

Containerization for services:

```
docker/
├── build.sh               # Build Docker images
├── launch.sh              # Start containers
├── export.sh              # Export images
├── reset.sh               # Clean Docker environment
└── logger_broker/         # MQTT logger broker
    ├── DockerFile
    └── version.txt
```

## Configuration Files

### `platformio.ini`

PlatformIO project configuration:
- Board definitions
- Build flags
- Library dependencies
- Test configurations
- Upload settings

Key environments:
```ini
[env:lilygo-t-display-debug]  # Main development board
[env:native]                   # Native testing
```

### `package.json`

Node.js configuration for documentation:
```json
{
  "scripts": {
    "docs:dev": "vitepress dev docs",
    "docs:build": "vitepress build docs",
    "docs:preview": "vitepress preview docs"
  }
}
```

### `requirements.txt`

Python dependencies for build tools:
- Code generators
- Test utilities
- Helper scripts

## File Naming Conventions

### C++ Files
- **Headers**: `.h` extension
- **Implementation**: `.cpp` extension
- **PascalCase** for classes: `SensorService.cpp`
- **camelCase** for utilities: `espError.cpp`

### Tests
- Prefix with `test_`: `test_network.cpp`
- Group by feature: `test_ui_snapshot/`
- Keep test helpers nearby: `test_network_helpers.h`

### Documentation
- **kebab-case**: `getting-started.md`
- Organize by category in folders

## Build Artifacts

Generated files (not in version control):
```
.pio/                     # PlatformIO build directory
.vitepress/dist/          # Built documentation
node_modules/             # Node.js dependencies
__pycache__/              # Python cache
*.actual                  # Snapshot test output
```

## Important Files

### Project Documentation
- `README.md` - Project overview and setup
- `spec.md` - Original specification (French)
- `archi.puml` - Architecture diagram

### Configuration
- `platformio.ini` - Build and upload configuration
- `package.json` - Documentation build
- `.gitignore` - Version control exclusions

## Next Steps

- [Architecture](/development/architecture) - Understand the system design
- [Build System](/development/build-system) - Learn about compilation
- [Code Templates](/development/code-templates) - Generate boilerplate code
- [UI Framework](/development/ui/framework) - Build user interfaces
