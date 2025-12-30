# Service Registry

> **Note:** This page is under construction. Check back later for detailed documentation.

The Service Registry provides dependency injection for PlantNanny services.

## Overview

A central registry for managing service lifecycles and dependencies.

## Usage Example

```cpp
#include "libs/common/service/Registry.h"

// Register service
Registry::getInstance().register<SensorService>();

// Access service
auto& sensor = Registry::getInstance().get<SensorService>();
```

## Next Steps

- [Logger System](/development/libraries/logger)
- [Design Patterns](/development/libraries/patterns)
- [Architecture](/development/architecture)

---

*Full documentation coming soon!*
