# Code Templates

PlantNanny includes code generation templates to quickly create boilerplate code following project conventions.

## Overview

The `Devtools/CodeTemplates/` directory contains templates for common code patterns:

```
Devtools/CodeTemplates/
├── Class/
│   ├── header.h
│   └── implementation.cpp
├── Enum class/
│   └── header.h
├── Interface/
│   └── header.h
├── Singleton/
│   ├── header.h
│   └── implementation.cpp
└── Templated class/
    └── header.h
```

## Using Templates

### Generate Code

Use the generation script:

```bash
# Interactive mode
./Devtools/generate.sh

# With parameters
./Devtools/generate.sh --type class --name MyService
```

### Template Types

1. **Class** - Standard C++ class
2. **Enum class** - Type-safe enumeration
3. **Interface** - Abstract base class
4. **Singleton** - Thread-safe singleton pattern
5. **Templated class** - Generic template class

## Class Template

### Header (`Class/header.h`)

```cpp
#ifndef LIBS_NAMESPACE_CLASSNAME_H
#define LIBS_NAMESPACE_CLASSNAME_H

namespace namespace_name {

/**
 * @brief Brief description of ClassName
 * 
 * Detailed description of what this class does.
 */
class ClassName {
public:
    /**
     * @brief Constructor
     */
    ClassName();
    
    /**
     * @brief Destructor
     */
    ~ClassName();
    
    // Delete copy constructor and assignment
    ClassName(const ClassName&) = delete;
    ClassName& operator=(const ClassName&) = delete;
    
    // Enable move semantics
    ClassName(ClassName&&) noexcept = default;
    ClassName& operator=(ClassName&&) noexcept = default;
    
private:
    // Private members
};

} // namespace namespace_name

#endif // LIBS_NAMESPACE_CLASSNAME_H
```

### Implementation (`Class/implementation.cpp`)

```cpp
#include "ClassName.h"

namespace namespace_name {

ClassName::ClassName() {
    // Constructor implementation
}

ClassName::~ClassName() {
    // Destructor implementation
}

} // namespace namespace_name
```

## Singleton Template

### Header (`Singleton/header.h`)

```cpp
#ifndef LIBS_NAMESPACE_SINGLETON_H
#define LIBS_NAMESPACE_SINGLETON_H

#include <memory>
#include <mutex>

namespace namespace_name {

/**
 * @brief Thread-safe singleton implementation
 */
class Singleton {
public:
    /**
     * @brief Get singleton instance
     * @return Reference to singleton instance
     */
    static Singleton& getInstance();
    
    // Delete copy and move
    Singleton(const Singleton&) = delete;
    Singleton& operator=(const Singleton&) = delete;
    Singleton(Singleton&&) = delete;
    Singleton& operator=(Singleton&&) = delete;
    
private:
    /**
     * @brief Private constructor
     */
    Singleton();
    
    /**
     * @brief Destructor
     */
    ~Singleton();
    
    static std::unique_ptr<Singleton> instance_;
    static std::mutex mutex_;
};

} // namespace namespace_name

#endif // LIBS_NAMESPACE_SINGLETON_H
```

### Implementation (`Singleton/implementation.cpp`)

```cpp
#include "Singleton.h"

namespace namespace_name {

std::unique_ptr<Singleton> Singleton::instance_ = nullptr;
std::mutex Singleton::mutex_;

Singleton& Singleton::getInstance() {
    std::lock_guard<std::mutex> lock(mutex_);
    if (!instance_) {
        instance_ = std::unique_ptr<Singleton>(new Singleton());
    }
    return *instance_;
}

Singleton::Singleton() {
    // Constructor implementation
}

Singleton::~Singleton() {
    // Destructor implementation
}

} // namespace namespace_name
```

## Interface Template

### Header (`Interface/header.h`)

```cpp
#ifndef LIBS_NAMESPACE_IINTERFACE_H
#define LIBS_NAMESPACE_IINTERFACE_H

namespace namespace_name {

/**
 * @brief Interface description
 * 
 * Define the contract for implementing classes.
 */
class IInterface {
public:
    /**
     * @brief Virtual destructor
     */
    virtual ~IInterface() = default;
    
    /**
     * @brief Pure virtual method
     */
    virtual void method() = 0;
    
protected:
    /**
     * @brief Protected constructor (interface cannot be instantiated)
     */
    IInterface() = default;
};

} // namespace namespace_name

#endif // LIBS_NAMESPACE_IINTERFACE_H
```

## Enum Class Template

### Header (`Enum class/header.h`)

```cpp
#ifndef LIBS_NAMESPACE_ENUMNAME_H
#define LIBS_NAMESPACE_ENUMNAME_H

#include <string>

namespace namespace_name {

/**
 * @brief Enumeration description
 */
enum class EnumName {
    VALUE1,
    VALUE2,
    VALUE3
};

/**
 * @brief Convert enum to string
 * @param value Enum value
 * @return String representation
 */
inline std::string toString(EnumName value) {
    switch (value) {
        case EnumName::VALUE1: return "Value1";
        case EnumName::VALUE2: return "Value2";
        case EnumName::VALUE3: return "Value3";
        default: return "Unknown";
    }
}

} // namespace namespace_name

#endif // LIBS_NAMESPACE_ENUMNAME_H
```

## Templated Class Template

### Header (`Templated class/header.h`)

```cpp
#ifndef LIBS_NAMESPACE_TEMPLATECLASS_H
#define LIBS_NAMESPACE_TEMPLATECLASS_H

namespace namespace_name {

/**
 * @brief Template class description
 * @tparam T Type parameter
 */
template<typename T>
class TemplateClass {
public:
    /**
     * @brief Constructor
     * @param value Initial value
     */
    explicit TemplateClass(T value) : value_(value) {}
    
    /**
     * @brief Get value
     * @return Current value
     */
    T getValue() const { return value_; }
    
    /**
     * @brief Set value
     * @param value New value
     */
    void setValue(T value) { value_ = value; }
    
private:
    T value_;
};

} // namespace namespace_name

#endif // LIBS_NAMESPACE_TEMPLATECLASS_H
```

## Generation Script

The `class_creator.py` script automates code generation:

```bash
# Usage
python Devtools/python/class_creator.py \
    --type class \
    --name MyService \
    --namespace services \
    --output src/libs/plant_nanny/services/
```

### Script Features

- Template substitution
- Proper include guards
- Namespace handling
- File naming conventions
- Header/implementation splitting

## Naming Conventions

### Files
- **Headers**: `ClassName.h`
- **Implementation**: `ClassName.cpp`
- **CamelCase** for classes
- **Include guards**: `LIBS_NAMESPACE_CLASSNAME_H`

### Classes
- **PascalCase**: `MyService`
- **Interfaces**: Prefix with `I` (e.g., `ILogger`)
- **Abstracts**: Prefix with `Abstract` (e.g., `AbstractService`)

### Namespaces
- **snake_case**: `plant_nanny`, `common`
- Reflect directory structure

### Members
- **camelCase**: `memberVariable`
- **Private members**: Suffix with `_` (e.g., `value_`)

## Best Practices

### 1. Include Guards

Always use include guards:
```cpp
#ifndef LIBS_NAMESPACE_CLASSNAME_H
#define LIBS_NAMESPACE_CLASSNAME_H
// ...
#endif
```

### 2. Forward Declarations

Prefer forward declarations in headers:
```cpp
// Good: Forward declaration
class OtherClass;

// Avoid: Including header
#include "OtherClass.h"
```

### 3. Documentation

Document all public APIs:
```cpp
/**
 * @brief Brief description
 * @param param Parameter description
 * @return Return value description
 */
void method(int param);
```

### 4. Rule of Five

Explicitly handle special members:
```cpp
class MyClass {
public:
    MyClass();                                    // Constructor
    ~MyClass();                                   // Destructor
    MyClass(const MyClass&) = delete;            // Copy constructor
    MyClass& operator=(const MyClass&) = delete; // Copy assignment
    MyClass(MyClass&&) noexcept;                 // Move constructor
    MyClass& operator=(MyClass&&) noexcept;      // Move assignment
};
```

### 5. Const Correctness

Mark methods const when appropriate:
```cpp
int getValue() const { return value_; }  // Doesn't modify state
```

## Example: Creating a Service

### 1. Generate Template

```bash
./Devtools/generate.sh --type class --name SensorService
```

### 2. Customize Header

```cpp
#ifndef LIBS_PLANT_NANNY_SERVICES_SENSORSERVICE_H
#define LIBS_PLANT_NANNY_SERVICES_SENSORSERVICE_H

#include "libs/common/service/Service.h"

namespace plant_nanny::services {

class SensorService : public common::Service {
public:
    SensorService();
    ~SensorService() override;
    
    // Service interface
    void init() override;
    void update() override;
    
    // Sensor operations
    float getTemperature() const;
    float getHumidity() const;
    int getLightLevel() const;
    
private:
    float temperature_;
    float humidity_;
    int lightLevel_;
};

} // namespace plant_nanny::services

#endif
```

### 3. Implement Methods

```cpp
#include "SensorService.h"

namespace plant_nanny::services {

SensorService::SensorService()
    : temperature_(0.0f)
    , humidity_(0.0f)
    , lightLevel_(0) {
}

SensorService::~SensorService() = default;

void SensorService::init() {
    // Initialize sensors
}

void SensorService::update() {
    // Read sensor values
    temperature_ = readTemperature();
    humidity_ = readHumidity();
    lightLevel_ = readLightLevel();
}

float SensorService::getTemperature() const {
    return temperature_;
}

// ... more implementations

} // namespace plant_nanny::services
```

## Next Steps

- [Project Structure](/development/project-structure) - Understand the codebase
- [Service Registry](/development/libraries/service-registry) - Implement services
- [Design Patterns](/development/libraries/patterns) - Learn patterns
- [Testing](/development/testing/overview) - Test your code
