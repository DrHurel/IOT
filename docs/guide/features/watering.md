# Automated Watering

> **Note:** This page is under construction. Detailed documentation coming soon!

PlantNanny's intelligent watering system automatically waters your plants based on configured thresholds and environmental conditions.

## How It Works

The watering system operates based on:
1. **Time-based scheduling** - Water at regular intervals
2. **Condition-based triggers** - Water when soil is dry or humidity is low
3. **Manual triggers** - Water on-demand via mobile app or button

## Configuration

For each plant, configure:

### Watering Cycle
- Time between waterings (e.g., every 2 days)
- Minimum/maximum intervals

### Water Quantity
- Amount of water per cycle (in ml)
- Pump duration (in seconds)

### Trigger Conditions
- Humidity threshold
- Temperature range
- Time of day preferences

## Safety Features

- **Overflow prevention** - Maximum water quantity limits
- **Pump protection** - Minimum off-time between waterings
- **Error detection** - Detect pump failures or sensor issues
- **Manual override** - Emergency stop capability

## Watering Modes

### Automatic Mode
System waters automatically based on configured thresholds.

### Manual Mode
Water only when triggered manually via app or controller.

### Scheduled Mode
Water at specific times of day (e.g., morning and evening).

## Hardware Requirements

- **Water pump** - 5V DC pump (or appropriate voltage)
- **Relay module** - To control pump (GPIO controlled)
- **Water reservoir** - Container for water supply
- **Tubing** - Connect pump to plant

## Wiring

```
ESP32 GPIO → Relay IN
Relay COM → Pump +
Relay NO → Pump -
Power Supply → Relay VCC/GND
```

## API Control

Trigger watering via REST API:

```http
POST /api/v1/plants/{plantId}/water
{
  "quantity": 200,  // ml
  "manual": true
}
```

## Monitoring

Track watering events:
- Last watering timestamp
- Water quantity delivered
- Success/failure status
- Historical watering log

## Next Steps

- [Configure monitoring](./monitoring)
- [Set plant thresholds](./plant-config)
- [Use the mobile app](./mobile-app)

---

*Full documentation coming soon. Check back later or [contribute](https://github.com/DrHurel/IOT)!*
