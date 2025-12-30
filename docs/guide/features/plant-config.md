# Plant Configuration

> **Note:** This page is under construction. Detailed documentation coming soon!

Configure individual plant settings to optimize care for each specific plant type.

## Configuration Overview

Each plant in the PlantNanny system can have custom settings tailored to its specific needs.

## Configurable Parameters

### Optimal Temperature
- **Description:** Target temperature range for the plant
- **Format:** Minimum and maximum values in Celsius
- **Example:** 18°C to 24°C for most houseplants

```json
{
  "temperature": {
    "optimal": 21,
    "min": 18,
    "max": 24
  }
}
```

### Optimal Humidity
- **Description:** Target humidity range
- **Format:** Percentage (0-100%)
- **Example:** 40-60% for most houseplants

```json
{
  "humidity": {
    "optimal": 50,
    "min": 40,
    "max": 60
  }
}
```

### Watering Cycle
- **Description:** Time between waterings
- **Format:** Seconds (or converted to days/hours)
- **Example:** 172800 seconds (2 days)

```json
{
  "watering": {
    "cycle": 172800,  // 2 days in seconds
    "flexible": true   // Allow adjustment based on conditions
  }
}
```

### Water Quantity
- **Description:** Amount of water per watering cycle
- **Format:** Milliliters (ml)
- **Example:** 200ml per watering

```json
{
  "watering": {
    "quantity": 200,  // ml
    "duration": 20    // pump duration in seconds
  }
}
```

### Light Requirements
- **Description:** Optimal light levels
- **Format:** Lux or percentage
- **Example:** Bright indirect light (5000-10000 lux)

```json
{
  "light": {
    "min": 5000,   // lux
    "max": 10000,
    "optimal": 7500
  }
}
```

### Location
- **Description:** GPS coordinates of plant location
- **Format:** Latitude/longitude
- **Purpose:** Track which room/location, outdoor vs indoor

```json
{
  "location": {
    "latitude": 48.8566,
    "longitude": 2.3522,
    "name": "Living Room"
  }
}
```

## Plant Profiles

Pre-configured profiles for common plants:

### Spider Plant
```json
{
  "name": "Spider Plant",
  "temperature": { "min": 18, "max": 24 },
  "humidity": { "min": 40, "max": 60 },
  "watering_cycle": 172800,
  "water_quantity": 200,
  "light": "medium"
}
```

### Succulent
```json
{
  "name": "Succulent",
  "temperature": { "min": 15, "max": 30 },
  "humidity": { "min": 20, "max": 40 },
  "watering_cycle": 604800,  // 7 days
  "water_quantity": 100,
  "light": "bright"
}
```

### Fern
```json
{
  "name": "Fern",
  "temperature": { "min": 16, "max": 22 },
  "humidity": { "min": 50, "max": 80 },
  "watering_cycle": 86400,  // 1 day
  "water_quantity": 250,
  "light": "low to medium"
}
```

## Configuration Methods

### Via Mobile App
1. Select plant from list
2. Tap "Edit Configuration"
3. Adjust sliders or input values
4. Save changes
5. Settings sync to controller

### Via REST API

```http
PUT /api/v1/plants/{plantId}
Content-Type: application/json
Authorization: Bearer <token>

{
  "name": "My Spider Plant",
  "temperature_optimal": 21,
  "humidity_optimal": 50,
  "watering_cycle": 172800,
  "water_quantity": 200,
  "location": {
    "latitude": 48.8566,
    "longitude": 2.3522
  }
}
```

### Via Controller (Planned)
- Configuration menu on TFT display
- Adjust settings using buttons

## Threshold Alerts

Configure alerts when conditions exceed thresholds:

```json
{
  "alerts": {
    "temperature_high": 28,
    "temperature_low": 15,
    "humidity_low": 35,
    "missed_watering": true
  }
}
```

## Advanced Settings

### Adaptive Watering (Planned)
Automatically adjust watering based on:
- Season (more water in summer)
- Temperature (more water when hot)
- Humidity (less water when humid)

### Growth Tracking (Planned)
- Track plant growth over time
- Adjust care based on growth stage
- Photo timeline

## Validation

The system validates configuration to prevent:
- Temperature out of sensor range
- Humidity values > 100%
- Watering intervals too short (pump protection)
- Water quantity too large (overflow prevention)

## Export/Import

Export configuration for backup or sharing:

```bash
# Export
GET /api/v1/plants/{plantId}/config

# Import
POST /api/v1/plants/{plantId}/config
```

## Next Steps

- [Set up monitoring](./monitoring)
- [Configure watering](./watering)
- [Use the mobile app](./mobile-app)
- [API Reference](/api/overview)

---

*Full documentation coming soon. Check back later or [contribute](https://github.com/DrHurel/IOT)!*
