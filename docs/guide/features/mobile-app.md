# Mobile Application

> **Note:** The mobile application is currently under development. This page will be updated as features are implemented.

The PlantNanny mobile app allows you to monitor and control your plants remotely from your smartphone.

## Planned Features

### Account Management
- ✅ Create new account
- ✅ Login/logout
- ✅ Password reset
- 🔄 Profile management
- 🔄 Multi-device support

### Plant Management
- 🔄 View list of configured plants
- 🔄 Add new plant with configuration
- 🔄 Edit plant settings
- 🔄 Delete plants
- 🔄 GPS location tracking for each plant

### Monitoring
- 🔄 Real-time sensor data (temperature, humidity, light)
- 🔄 Historical data charts
- 🔄 Watering history
- 🔄 System status indicators
- 🔄 Push notifications for alerts

### Control
- 🔄 Manual watering trigger
- 🔄 Adjust watering schedule
- 🔄 Enable/disable automation
- 🔄 Emergency stop

### Controller Setup
- 🔄 Bluetooth pairing for initial setup
- 🔄 WiFi configuration
- 🔄 Controller discovery
- 🔄 Firmware updates

## Platform Support

The mobile app will be available for:
- **iOS** (iPhone/iPad) - Planned
- **Android** - Planned

## Technology Stack

The technology stack is still being decided. Options include:
- React Native (cross-platform)
- Flutter (cross-platform)
- Native iOS (Swift) and Android (Kotlin)

## Screenshots

Coming soon! The UI is currently being designed.

## Initial Setup Flow

1. **Download App** - Install from App Store or Play Store
2. **Create Account** - Sign up with email and password
3. **Add Controller** - Pair with ESP32 via Bluetooth
4. **Configure WiFi** - Enter WiFi credentials for controller
5. **Add Plant** - Configure your first plant
6. **Start Monitoring** - View real-time data!

## API Integration

The mobile app communicates with:
- **Controller** via Bluetooth (for initial setup)
- **Server API** via HTTPS (for monitoring and control)

See [API Documentation](/api/overview) for details.

## Beta Testing

Interested in beta testing the mobile app?
- Watch the [GitHub repository](https://github.com/DrHurel/IOT) for announcements
- Join our [community discussions](https://github.com/DrHurel/IOT/discussions)

## Development Status

| Feature | Status |
|---------|--------|
| UI Design | 🔄 In Progress |
| Authentication | 🔄 In Progress |
| Plant Management | 📅 Planned |
| Real-time Monitoring | 📅 Planned |
| Bluetooth Setup | 📅 Planned |
| Push Notifications | 📅 Planned |

**Legend:**
- ✅ Complete
- 🔄 In Progress
- 📅 Planned

## Contributing

Want to help build the mobile app? Check out:
- [Contributing Guide](https://github.com/DrHurel/IOT/blob/main/CONTRIBUTING.md)
- [GitHub Issues](https://github.com/DrHurel/IOT/issues)

## Next Steps

- [Configure your plants](./plant-config)
- [Learn about monitoring](./monitoring)
- [Explore the API](/api/overview)

---

*This page will be updated as the mobile app development progresses. Stay tuned!*
