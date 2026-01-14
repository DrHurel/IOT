# PlantNanny MQTT Architecture

This document describes the MQTT communication architecture between ESP32 devices, the PlantNanny server, and the MQTT broker.

## High-Level Architecture

```
┌──────────────┐     ┌──────────────┐     ┌──────────────┐
│   ESP32 #1   │─┐   │              │     │              │
├──────────────┤ │   │              │     │              │
│   ESP32 #2   │─┼──▶│ MQTT Broker  │◀────│   Server     │
├──────────────┤ │   │ (Mosquitto)  │     │              │
│   ESP32 #N   │─┘   │              │     │              │
└──────────────┘     └──────────────┘     └──────────────┘
                            │
                            ▼
                     ┌──────────────┐
                     │   Frontend   │
                     │  (Flutter)   │
                     └──────────────┘
```

## Device Identification

Each ESP32 must have a **unique device ID**. Options include:

- **MAC Address** (recommended): `AA:BB:CC:DD:EE:FF` → `aabbccddeeff`
- **UUID**: Generated once and stored in NVS
- **Pairing Code**: User-provided during setup

Example: `device_id = "esp32-aabbccddeeff"`

## Topic Structure

### 📤 ESP32 → Server (Telemetry)

**Topic:** `devices/<device_id>/data`

**Example:** `devices/esp32-001/data`

**Payload (JSON):**
```json
{
  "temperatureC": 24.3,
  "humidityPct": 56.0,
  "luminosityPct": 75.5,
  "ts": 1704825600,
  "uptime": 123456
}
```

### 📥 Server → ESP32 (Commands)

**Topic:** `devices/<device_id>/command`

**Example:** `devices/esp32-001/command`

**Payload (JSON):**
```json
{
  "action": "send_now"
}
```

#### Supported Commands

| Action         | Description                    | Parameters               |
| -------------- | ------------------------------ | ------------------------ |
| `send_now`     | Force immediate sensor reading | None                     |
| `pump_water`   | Activate water pump            | `durationMs`, `amountMl` |
| `set_interval` | Change publish interval        | `intervalMs`             |
| `restart`      | Restart device                 | None                     |
| `ota_update`   | Trigger OTA update             | `url`                    |

### 📡 Device Status (LWT)

**Topic:** `devices/<device_id>/status`

**Example:** `devices/esp32-001/status`

**Payload (JSON):**
```json
{
  "status": "online"
}
```

Uses MQTT **Last Will and Testament (LWT)** to automatically set status to `offline` when connection is lost.

## Quality of Service (QoS)

All communications use **QoS 1** for reliable delivery:

- Telemetry: QoS 1
- Commands: QoS 1
- Status: QoS 1 with retain flag

## Data Flow

### 1. Device Registration Flow

```
┌────────┐    ┌────────┐    ┌────────┐    ┌────────┐
│Frontend│    │ Server │    │ Broker │    │ ESP32  │
└───┬────┘    └───┬────┘    └───┬────┘    └───┬────┘
    │             │             │             │
    │ Register    │             │             │
    │────────────▶│             │             │
    │             │             │             │
    │             │ send_now    │             │
    │             │────────────▶│────────────▶│
    │             │             │             │
    │             │             │ sensor data │
    │             │◀────────────│◀────────────│
    │             │             │             │
    │ SSE: data   │             │             │
    │◀────────────│             │             │
```

### 2. Periodic Data Flow

```
ESP32                    Broker                   Server
  │                        │                        │
  │ Publish sensor data    │                        │
  │ (every X seconds)      │                        │
  │───────────────────────▶│───────────────────────▶│
  │                        │                        │
  │                        │                        │ Store reading
  │                        │                        │ Notify SSE subscribers
```

### 3. On-Demand Data Request

```
Frontend                 Server                   Broker                   ESP32
    │                      │                        │                        │
    │ GET /stream          │                        │                        │
    │─────────────────────▶│                        │                        │
    │                      │                        │                        │
    │                      │ send_now command       │                        │
    │                      │───────────────────────▶│───────────────────────▶│
    │                      │                        │                        │
    │                      │                        │      sensor data       │
    │                      │◀───────────────────────│◀───────────────────────│
    │                      │                        │                        │
    │ SSE: reading event   │                        │                        │
    │◀─────────────────────│                        │                        │
```

## Security Configuration

### Authentication

The broker uses username/password authentication:

| User                | Purpose       | Access                                 |
| ------------------- | ------------- | -------------------------------------- |
| `plantnanny_server` | Server        | Read/write all `devices/#` topics      |
| `plantnanny_device` | Devices (dev) | Write `data`, `status`; read `command` |

### Access Control (ACL)

See [docker/mqtt_broker/acl](../docker/mqtt_broker/acl) for topic-level permissions.

### Production Recommendations

1. **TLS/SSL**: Enable MQTTS on port 8883
2. **Per-device credentials**: Generate unique credentials during pairing
3. **Client certificates**: For additional device authentication

## Configuration

### Environment Variables

#### ESP32 (.env)
```env
MQTT_BROKER_HOST=your-server-ip
MQTT_BROKER_PORT=1883
MQTT_USERNAME=plantnanny_device
MQTT_PASSWORD=device_secret_2024
MQTT_PUBLISH_INTERVAL_MS=60000
```

#### Server (.env)
```env
MQTT_BROKER_HOST=localhost
MQTT_BROKER_PORT=1883
MQTT_USERNAME=plantnanny_server
MQTT_PASSWORD=server_secret_2024
```

## Legacy Topic Support

For backward compatibility with existing firmware, the server also subscribes to:

- `plantnanny/+/sensors` → Sensor data
- `plantnanny/+/status` → Device status

ESP32 firmware can be configured to use either topic structure via the `use_new_topics` parameter.

## Implementation Files

### ESP32 (C++)

- [include/libs/plant_nanny/services/mqtt/MQTTService.h](../include/libs/plant_nanny/services/mqtt/MQTTService.h)
- [src/libs/plant_nanny/services/mqtt/MQTTService.cpp](../src/libs/plant_nanny/services/mqtt/MQTTService.cpp)

### Server (Python)

- [PlantNanny/server/mqtt_handler.py](../PlantNanny/server/mqtt_handler.py)
- [PlantNanny/server/handlers/v1/devices/stream.py](../PlantNanny/server/handlers/v1/devices/stream.py)

### Broker Configuration

- [docker/mqtt_broker/mosquitto.conf](../docker/mqtt_broker/mosquitto.conf)
- [docker/mqtt_broker/passwd](../docker/mqtt_broker/passwd)
- [docker/mqtt_broker/acl](../docker/mqtt_broker/acl)
