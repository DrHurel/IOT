# Controller API

> **Note:** This page is under construction. See [API Overview](/api/overview) for general API information.

The Controller API is exposed by the ESP32 controller device, allowing direct communication with the hardware.

## Base URL

```
http://<controller-ip-address>/api/v1
```

Find your controller's IP address:
- Displayed on the TFT screen
- Via router/DHCP server
- Via mDNS: `plantnanny-<id>.local`

## Endpoints

### Status

#### Get Controller Status

```http
GET /status
```

Response:
```json
{
  "status": "success",
  "data": {
    "controllerId": "esp32-abc123",
    "version": "1.0.0",
    "uptime": 3600,
    "wifiConnected": true,
    "wifiSignal": -45,
    "freeHeap": 123456,
    "plantConfigured": true
  }
}
```

### Sensors

#### List Available Sensors

```http
GET /sensors
```

Response:
```json
{
  "status": "success",
  "data": {
    "sensors": [
      {
        "type": "temperature",
        "unit": "celsius",
        "min": -40,
        "max": 80
      },
      {
        "type": "humidity",
        "unit": "percent",
        "min": 0,
        "max": 100
      },
      {
        "type": "light",
        "unit": "lux",
        "min": 0,
        "max": 100000
      }
    ]
  }
}
```

### Measurements

#### Get Latest Measurement

```http
GET /measurements/latest
```

Response:
```json
{
  "status": "success",
  "data": {
    "timestamp": "2025-12-05T10:30:00Z",
    "temperature": 22.5,
    "humidity": 55.2,
    "light": 7500
  }
}
```

#### Get Recent Measurements

```http
GET /measurements?limit=10
```

Response:
```json
{
  "status": "success",
  "data": {
    "measurements": [
      {
        "timestamp": "2025-12-05T10:30:00Z",
        "temperature": 22.5,
        "humidity": 55.2,
        "light": 7500
      },
      // ... more measurements
    ]
  }
}
```

### Watering

#### Trigger Manual Watering

```http
POST /water
Content-Type: application/json

{
  "quantity": 200,  // ml
  "duration": 20    // seconds (optional)
}
```

Response:
```json
{
  "status": "success",
  "data": {
    "watered": true,
    "quantity": 200,
    "duration": 20,
    "timestamp": "2025-12-05T10:30:00Z"
  }
}
```

#### Get Watering Status

```http
GET /water/status
```

Response:
```json
{
  "status": "success",
  "data": {
    "isWatering": false,
    "lastWatering": "2025-12-05T08:00:00Z",
    "nextScheduled": "2025-12-07T08:00:00Z",
    "totalWaterings": 42
  }
}
```

### Configuration

#### Get Current Configuration

```http
GET /config
```

Response:
```json
{
  "status": "success",
  "data": {
    "plantName": "Spider Plant",
    "temperatureOptimal": 21,
    "humidityOptimal": 50,
    "wateringCycle": 172800,
    "waterQuantity": 200,
    "autoWatering": true
  }
}
```

#### Update Configuration

```http
PUT /config
Content-Type: application/json

{
  "plantName": "Spider Plant",
  "temperatureOptimal": 22,
  "humidityOptimal": 55,
  "wateringCycle": 172800,
  "waterQuantity": 250,
  "autoWatering": true
}
```

### WiFi Configuration

#### Configure WiFi (via Bluetooth)

This endpoint is typically accessed via Bluetooth during initial setup:

```http
POST /wifi
Content-Type: application/json

{
  "ssid": "YourWiFiNetwork",
  "password": "wifi_password"
}
```

### System

#### Restart Controller

```http
POST /system/restart
```

#### Get System Info

```http
GET /system/info
```

Response:
```json
{
  "status": "success",
  "data": {
    "chipId": "esp32-abc123",
    "flashSize": 4194304,
    "freeHeap": 123456,
    "cpuFreq": 240,
    "sdkVersion": "v4.4.2"
  }
}
```

## Error Responses

### Sensor Error
```json
{
  "status": "error",
  "error": {
    "code": "SENSOR_ERROR",
    "message": "Failed to read temperature sensor"
  }
}
```

### Watering in Progress
```json
{
  "status": "error",
  "error": {
    "code": "WATERING_IN_PROGRESS",
    "message": "Cannot start watering while another cycle is in progress"
  }
}
```

## Authentication

Direct controller API typically doesn't require authentication when on the same local network. However, if enabled:

```http
Authorization: Bearer <local_token>
```

## Rate Limiting

- Maximum 60 requests per minute
- Watering commands limited to 1 per minute

## WebSocket (Planned)

Real-time sensor data via WebSocket:

```
ws://<controller-ip>/ws
```

## Next Steps

- [Server API](/api/server)
- [Authentication](/api/authentication)
- [API Overview](/api/overview)

---

*See [OpenAPI Specification](https://github.com/DrHurel/IOT/blob/main/PlantNanny/api/plant_nanny_api.yaml) for complete details.*
