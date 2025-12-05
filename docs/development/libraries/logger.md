# Logger System

> **Note:** This page is under construction. Check back later for detailed documentation.

PlantNanny includes a flexible logging system with multiple output targets.

## Logger Types

- **SerialLogger**: Output to serial console
- **MQTTLogger**: Remote logging via MQTT broker

## Usage Example

```cpp
#include "libs/common/logger/Logger.h"

Logger::info("Temperature: %.1f°C", temperature);
Logger::error("Sensor read failed");
```

## Next Steps

- [Service Registry](/development/libraries/service-registry)
- [Design Patterns](/development/libraries/patterns)

---

*Full documentation coming soon!*
