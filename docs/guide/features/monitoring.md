# Environmental Monitoring

> **Note:** This page is under construction. Detailed documentation coming soon!

PlantNanny continuously monitors environmental conditions around your plants to ensure optimal growing conditions.

## Monitored Variables

### Temperature
- **Range:** -40°C to 80°C
- **Accuracy:** ±0.5°C
- **Update Frequency:** Every 30 seconds
- **Sensor:** DHT22 or similar

### Humidity
- **Range:** 0% to 100% RH
- **Accuracy:** ±2% RH
- **Update Frequency:** Every 30 seconds
- **Sensor:** DHT22 or similar

### Light Level
- **Range:** 0 to 100,000 lux
- **Update Frequency:** Every 60 seconds
- **Sensor:** LDR or BH1750 digital sensor

## Display

Environmental data is displayed in real-time on the TFT screen:
- Current temperature
- Current humidity
- Light level indicator
- Trend indicators (rising/falling)

## Data Storage

Sensor readings are:
1. Displayed locally on the controller
2. Buffered in memory
3. Sent to the cloud server
4. Stored in the database for historical analysis

## Alerts

The system can alert you when readings exceed configured thresholds:
- Temperature too high/low
- Humidity too high/low
- Insufficient light

## API Access

Access sensor data via the REST API:

```http
GET /api/v1/measurements/latest
GET /api/v1/measurements?startDate=2025-12-01
```

## Next Steps

- [Configure plant thresholds](./plant-config)
- [Set up automated watering](./watering)
- [View API documentation](/api/overview)

---

*Full documentation coming soon. Check back later or [contribute](https://github.com/DrHurel/IOT)!*
