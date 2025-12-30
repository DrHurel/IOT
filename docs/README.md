# PlantNanny Documentation

Welcome to the PlantNanny documentation! This comprehensive guide covers everything you need to know about the smart plant care system.

## 📚 Documentation Structure

### 🚀 Getting Started

Perfect for new users and developers:

- **[What is PlantNanny?](/guide/what-is-plantnanny)** - Overview of the system and its capabilities
- **[Getting Started](/guide/getting-started)** - Set up your development environment
- **[Quick Start](/guide/quick-start)** - Build and deploy your first controller

### 🌱 Features

Learn about PlantNanny's core functionality:

- **[Environmental Monitoring](/guide/features/monitoring)** - Track temperature, humidity, and light
- **[Automated Watering](/guide/features/watering)** - Smart irrigation based on plant needs
- **[Mobile App](/guide/features/mobile-app)** - Remote monitoring and control
- **[Plant Configuration](/guide/features/plant-config)** - Configure thresholds and settings

### 🔌 API Reference

Technical API documentation:

- **[API Overview](/api/overview)** - RESTful API architecture and usage
- **[Authentication](/api/authentication)** - User authentication and authorization
- **[Controller Endpoints](/api/controller)** - ESP32 controller API
- **[Server Endpoints](/api/server)** - Cloud backend API

### 💻 Development Guide

For contributors and advanced users:

#### Architecture & Structure
- **[Architecture](/development/architecture)** - System design and components
- **[Project Structure](/development/project-structure)** - Codebase organization
- **[Build System](/development/build-system)** - Compilation and deployment

#### UI Development
- **[UI Framework](/development/ui/framework)** - Component-based UI system
- **[UI Components](/development/ui/components)** - Available components reference
- **[Snapshot Testing](/development/ui/snapshot-testing)** - UI testing guide
- **[Quick Reference](/development/ui/snapshot-testing-quick-ref)** - Testing cheat sheet

#### Core Libraries
- **[Logger System](/development/libraries/logger)** - Logging infrastructure
- **[Service Registry](/development/libraries/service-registry)** - Dependency injection
- **[Design Patterns](/development/libraries/patterns)** - Common patterns used

#### Testing
- **[Testing Overview](/development/testing/overview)** - Testing strategy
- **[Unit Tests](/development/testing/unit-tests)** - Writing unit tests
- **[Network Tests](/development/testing/network-tests)** - Network testing

#### Tools
- **[Code Templates](/development/code-templates)** - Generate boilerplate code
- **[Development Scripts](/development/dev-scripts)** - Helper scripts reference

## 🎯 Quick Links

### For Users
- [What is PlantNanny?](/guide/what-is-plantnanny) - Start here!
- [Quick Start Guide](/guide/quick-start) - Get running in minutes
- [Mobile App Guide](/guide/features/mobile-app) - Use the app

### For Developers
- [Getting Started](/guide/getting-started) - Set up your dev environment
- [Architecture Overview](/development/architecture) - Understand the system
- [Project Structure](/development/project-structure) - Navigate the code
- [Contributing Guide](https://github.com/DrHurel/IOT/blob/main/CONTRIBUTING.md) - Contribute

### For API Users
- [API Overview](/api/overview) - API introduction
- [OpenAPI Specs](https://github.com/DrHurel/IOT/tree/main/RestApi) - Full API definitions
- [Authentication](/api/authentication) - Get authenticated

## 🛠 Technology Stack

### Embedded (ESP32)
- **Language**: C++23
- **Framework**: Arduino + ESP-IDF
- **Build**: PlatformIO
- **Display**: TFT_eSPI (ST7789)
- **Communication**: WiFi, Bluetooth, HTTPS

### Server
- **API**: RESTful (OpenAPI 3.0)
- **Auth**: JWT tokens
- **Database**: MongoDB
- **Protocols**: HTTP/HTTPS, MQTT

### Documentation
- **Framework**: VitePress
- **Hosting**: GitHub Pages
- **Language**: Markdown

## 📦 Project Highlights

### Custom UI Framework
- Component-based architecture
- Fluent builder API
- Snapshot testing support
- Optimized for embedded displays

### Service Architecture
- Dependency injection via Registry
- Singleton pattern for global services
- Event-driven design
- Modular and testable

### Comprehensive Testing
- Unit tests with Unity framework
- UI snapshot tests on native platform
- Network and Bluetooth integration tests
- Real hardware testing support

### Developer Tools
- Code generation templates
- Build and deployment scripts
- Docker support
- Extensive documentation

## 🔗 External Resources

- **GitHub Repository**: [DrHurel/IOT](https://github.com/DrHurel/IOT)
- **Issues & Bugs**: [Issue Tracker](https://github.com/DrHurel/IOT/issues)
- **PlatformIO**: [docs.platformio.org](https://docs.platformio.org/)
- **ESP32 Docs**: [docs.espressif.com](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/)
- **VitePress**: [vitepress.dev](https://vitepress.dev/)

## 📝 License

PlantNanny is released under the [ISC License](https://github.com/DrHurel/IOT/blob/main/LICENSE).

## 🤝 Contributing

We welcome contributions! Please read our [Contributing Guide](https://github.com/DrHurel/IOT/blob/main/CONTRIBUTING.md) to get started.

### Ways to Contribute
- 🐛 Report bugs and issues
- 💡 Suggest new features
- 📝 Improve documentation
- 🔧 Submit pull requests
- ⭐ Star the repository

## 💬 Community & Support

- **Issues**: [GitHub Issues](https://github.com/DrHurel/IOT/issues)
- **Discussions**: [GitHub Discussions](https://github.com/DrHurel/IOT/discussions)
- **Email**: [Contact the team](mailto:contact@plantnanny.io)

## 🗺 Roadmap

### Current Status (v1.0)
- ✅ ESP32 firmware with sensor support
- ✅ TFT display UI framework
- ✅ RESTful API specifications
- ✅ Comprehensive documentation
- ✅ Testing infrastructure

### Planned Features
- 🔄 Mobile application
- 🔄 Server implementation
- 🔄 MQTT broker integration
- 🔄 OTA firmware updates
- 🔄 Machine learning plant recommendations
- 🔄 Multi-language support

## 📊 Project Stats

- **Lines of Code**: ~10,000+ (C++)
- **Test Coverage**: Growing
- **Documentation Pages**: 20+
- **Supported Boards**: ESP32 family
- **Active Development**: ✅

## 🎓 Learning Resources

### For Beginners
1. Start with [What is PlantNanny?](/guide/what-is-plantnanny)
2. Follow [Getting Started](/guide/getting-started) guide
3. Try the [Quick Start](/guide/quick-start) tutorial
4. Explore [Features](/guide/features/monitoring)

### For Experienced Developers
1. Review [Architecture](/development/architecture)
2. Understand [Project Structure](/development/project-structure)
3. Explore [UI Framework](/development/ui/framework)
4. Check [API Reference](/api/overview)

### For Contributors
1. Read [Contributing Guidelines](https://github.com/DrHurel/IOT/blob/main/CONTRIBUTING.md)
2. Set up [Development Environment](/guide/getting-started)
3. Learn about [Code Templates](/development/code-templates)
4. Write [Tests](/development/testing/overview)

---

**Ready to get started?** Head to [Getting Started](/guide/getting-started) or explore the documentation using the navigation menu! 🌱
