# What is PlantNanny?

PlantNanny is an intelligent IoT plant care system designed to automate watering based on environmental conditions and plant-specific needs.

## Overview

PlantNanny combines embedded hardware, cloud services, and mobile applications to provide a complete solution for plant care automation and monitoring.

### Key Capabilities

- **Environmental Monitoring**: Continuously tracks temperature, humidity, and light levels
- **Smart Watering**: Automatically waters plants based on configurable thresholds
- **Remote Access**: Monitor and control your plants from anywhere via mobile app
- **Historical Data**: Track environmental conditions over time with cloud storage
- **Multi-Plant Management**: Configure and manage multiple plants with individual settings

## System Components

### 1. Controller (ESP32)
The embedded controller is the heart of the system, running on an ESP32 microcontroller:
- Collects sensor data (temperature, humidity, light)
- Controls watering mechanism
- Displays real-time data on TFT screen
- Communicates with mobile app via Bluetooth (initial setup)
- Connects to WiFi for cloud integration

### 2. Server (Cloud API)
RESTful API server providing:
- User authentication and authorization
- Plant configuration storage
- Historical measurement data
- Remote control capabilities
- Multi-user support

### 3. Mobile Application
Native mobile app for:
- User account management
- Plant configuration and monitoring
- Manual watering triggers
- GPS-based plant location tracking
- Real-time data visualization

## How It Works

1. **Setup**: Configure WiFi via Bluetooth pairing with mobile app
2. **Monitor**: Sensors continuously collect environmental data
3. **Analyze**: System compares readings against configured thresholds
4. **Act**: Automatically waters when conditions require it
5. **Report**: Data synced to cloud for historical analysis and remote access

## Monitored Variables

- **Temperature**: Ambient temperature around the plant
- **Humidity**: Room humidity levels
- **Light**: Light intensity/luminosity

## Configurable Thresholds

For each plant, you can configure:
- **Optimal Temperature**: Target temperature range
- **Optimal Humidity**: Target humidity range
- **Watering Cycle**: Time interval between waterings
- **Water Quantity**: Amount of water per watering cycle

## Use Cases

- **Home Gardening**: Keep indoor plants healthy while traveling
- **Smart Agriculture**: Monitor and automate irrigation for small farms
- **Research**: Collect environmental data for plant growth studies
- **Education**: Learn about IoT, embedded systems, and automation

## Technology Stack

- **Hardware**: ESP32 (LilyGo T-Display), environmental sensors
- **Firmware**: PlatformIO, Arduino Framework, C++23
- **Server**: RESTful API (OpenAPI specification)
- **Mobile**: (Platform TBD)
- **Documentation**: VitePress
- **Version Control**: Git

## Next Steps

- [Getting Started](/guide/getting-started) - Set up your development environment
- [Quick Start](/guide/quick-start) - Build and deploy your first PlantNanny controller
- [Architecture](/development/architecture) - Dive deep into the system design
