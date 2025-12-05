# Architecture Overview

PlantNanny follows a modern IoT architecture with separation of concerns between embedded firmware, cloud services, and client applications.

## System Architecture

```
┌─────────────────┐
│  Mobile App     │
│  (Client)       │
└────────┬────────┘
         │ HTTPS/REST
         │
┌────────▼────────┐      ┌──────────────┐
│   REST API      │◄─────┤  Database    │
│   (Server)      │      │  (MongoDB)   │
└────────┬────────┘      └──────────────┘
         │ HTTPS/REST
         │ MQTT (optional)
         │
┌────────▼────────┐      ┌──────────────┐
│   Controller    │◄─────┤   Sensors    │
│   (ESP32)       │      │ (Temp/Hum/   │
└────────┬────────┘      │  Light)      │
         │               └──────────────┘
         │
┌────────▼────────┐
│   Actuator      │
│   (Water Pump)  │
└─────────────────┘
```

## Component Architecture

### 1. Embedded Controller (ESP32)

The firmware follows a service-oriented architecture with clear separation of concerns.

#### Core Components

```
App (Main Application)
├── Service Registry (Dependency Injection)
│   ├── Logger Service (MQTT + Serial)
│   ├── Network Service (WiFi Management)
│   ├── Sensor Service (Data Collection)
│   ├── Display Service (UI Rendering)
│   ├── Watering Service (Pump Control)
│   └── API Service (Server Communication)
├── UI Framework (Component-Based)
│   ├── Layout System
│   ├── Components (Text, Container, etc.)
│   └── Builders (Fluent API)
└── Utilities
    ├── Event System
    ├── Result Type (Error Handling)
    └── ESP Error Wrapper
```

#### Design Patterns

- **Service Registry**: Central dependency injection container
- **Singleton**: For global services (Logger, Registry)
- **Builder Pattern**: Fluent API for UI components
- **Observer Pattern**: Event-driven architecture
- **Result Type**: Functional error handling

### 2. Server (REST API)

The backend API follows RESTful principles with OpenAPI specification.

#### API Structure

```
/api/v1
├── /auth
│   ├── POST /register    (Create account)
│   ├── POST /login       (Authenticate)
│   └── POST /logout      (End session)
├── /plants
│   ├── GET /             (List user's plants)
│   ├── POST /            (Add new plant)
│   ├── GET /{id}         (Get plant details)
│   ├── PUT /{id}         (Update configuration)
│   ├── DELETE /{id}      (Remove plant)
│   └── POST /{id}/water  (Trigger watering)
├── /controllers
│   ├── GET /{id}         (Get controller info)
│   ├── GET /{id}/sensors (Get sensor list)
│   └── POST /{id}/data   (Upload measurements)
└── /measurements
    ├── GET /             (Get historical data)
    └── GET /{plantId}    (Get plant history)
```

#### Security

- JWT-based authentication
- Role-based access control (RBAC)
- API rate limiting
- HTTPS only in production

### 3. Mobile Application

Modern mobile app architecture (architecture details TBD based on platform choice).

#### Key Features

- User authentication
- Real-time data visualization
- Plant configuration management
- Manual watering control
- Historical data charts
- GPS-based plant location

## Data Flow

### 1. Sensor Data Collection

```
Sensors → Controller → Local Display
                    ↓
              Buffer/Queue
                    ↓
            REST API (POST)
                    ↓
              Server Storage
                    ↓
           Mobile App (GET)
```

### 2. Manual Watering Command

```
Mobile App → REST API (POST /plants/{id}/water)
                    ↓
              Authorization Check
                    ↓
            Controller Notification
                    ↓
              Watering Service
                    ↓
              Activate Pump
```

### 3. Automated Watering

```
Sensor Readings → Threshold Check
                        ↓
                  Watering Logic
                        ↓
                  Activate Pump
                        ↓
                  Log Event
                        ↓
              Sync to Server
```

## Technology Stack

### Embedded (ESP32)
- **Language**: C++23
- **Framework**: Arduino + ESP-IDF
- **Build System**: PlatformIO
- **UI Library**: Custom (TFT_eSPI based)
- **Networking**: WiFi, Bluetooth, HTTP/HTTPS
- **Logging**: MQTT + Serial

### Server
- **API Spec**: OpenAPI 3.0
- **Architecture**: RESTful
- **Authentication**: JWT
- **Database**: MongoDB (planned)
- **Protocols**: HTTP/HTTPS, MQTT (optional)

### Mobile
- **Platform**: TBD (React Native, Flutter, or native)
- **API Client**: REST
- **State Management**: TBD
- **Offline Support**: Planned

### Documentation
- **Framework**: VitePress
- **Language**: Markdown
- **Hosting**: GitHub Pages (planned)

## Code Organization

### Firmware Structure

```
src/
├── main.cpp                    # Entry point
└── libs/
    ├── common/                 # Shared libraries
    │   ├── logger/            # Logging system
    │   ├── patterns/          # Design patterns
    │   ├── service/           # Service framework
    │   ├── ui/                # UI framework
    │   └── utils/             # Utilities
    └── plant_nanny/           # Application code
        ├── App.cpp            # Main application
        └── services/          # App-specific services

include/
└── libs/                      # Header files mirror src/

test/
├── test_*/                    # Unit tests
└── snapshots/                 # UI snapshot tests
```

### API Structure

```
RestApi/
├── controller.yaml           # Controller API spec
├── server.yaml              # Server API spec
└── common/                  # Shared schemas
    ├── parameters.yaml
    ├── responses.yaml
    ├── schemas.yaml
    └── security.yaml
```

## Development Workflow

### 1. Local Development

```bash
# Build firmware
./Devtools/build.sh

# Run tests
./Devtools/test.sh

# Generate documentation
npm run docs:dev
```

### 2. Testing Strategy

- **Unit Tests**: PlatformIO Unity framework
- **UI Tests**: Snapshot testing on native platform
- **Integration Tests**: Network and Bluetooth tests
- **Hardware Tests**: Real WiFi and OTA tests

### 3. Deployment

- **Firmware**: OTA updates via ESP32 OTA mechanism
- **Server**: Docker containers (planned)
- **Documentation**: GitHub Pages (planned)

## Scalability Considerations

### Current Implementation
- Single controller per plant
- Direct REST API communication
- Synchronous operations

### Future Enhancements
- MQTT broker for pub/sub messaging
- Edge computing capabilities
- Multiple controllers per user
- Plant care recommendations via ML
- Community plant care database

## Security Considerations

### Controller
- Secure WiFi credential storage (ESP32 NVS)
- Encrypted HTTPS communication
- OTA update signature verification
- Rate limiting for API calls

### Server
- JWT token authentication
- Password hashing (bcrypt/argon2)
- Input validation and sanitization
- SQL injection prevention
- CORS configuration

### Mobile App
- Secure token storage
- Biometric authentication (optional)
- Certificate pinning
- Secure communication (TLS/SSL)

## Performance Considerations

### Memory Management
- Heap fragmentation prevention
- RAII pattern for resource management
- Smart pointers for dynamic allocation
- Watchdog timer for stability

### Power Consumption
- Deep sleep between readings
- Optimized sensor polling intervals
- WiFi power save mode
- Efficient display updates

## Next Steps

- [Project Structure](/development/project-structure) - Explore the codebase
- [UI Framework](/development/ui/framework) - Learn about the UI system
- [Service Registry](/development/libraries/service-registry) - Understand dependency injection
- [API Reference](/api/overview) - Browse the API documentation
