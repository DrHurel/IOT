# Quick Start

Get up and running with PlantNanny in minutes!

## 1. Hardware Setup

### Required Components
- LilyGo T-Display ESP32 board
- DHT22 or similar temperature/humidity sensor
- Light sensor (LDR or BH1750)
- Water pump with relay module
- Power supply (5V for pump, USB for ESP32)
- Connecting wires

### Wiring Diagram

::: tip
Refer to your specific sensor datasheets for exact pin configurations.
:::

```
ESP32 Pin Connections:
- Display: Pre-configured on LilyGo T-Display
- DHT22: GPIO pin (configure in code)
- Light Sensor: Analog GPIO pin
- Relay: Digital GPIO pin
- Power: USB or external 5V supply
```

## 2. Configure the Project

### Update `platformio.ini`

If using a different board:

```ini
[env:your-board]
platform = espressif32
board = your-board-name
framework = arduino
```

### Configure WiFi (Optional for Development)

For initial testing, you can hardcode WiFi credentials in your development config.

::: warning
Never commit WiFi credentials to version control! Use Bluetooth pairing for production.
:::

## 3. Build and Upload

### Using PlatformIO CLI

```bash
# Build the firmware
pio run -e lilygo-t-display-debug

# Upload to device
pio run -e lilygo-t-display-debug -t upload

# Monitor serial output
pio device monitor
```

### Using Helper Scripts

```bash
# Build
./Devtools/build.sh

# Upload and monitor
./Devtools/run.sh
```

### Using VS Code

1. Open the project in VS Code
2. Click the PlatformIO icon in the sidebar
3. Select your environment (lilygo-t-display-debug)
4. Click "Build" and then "Upload"

## 4. First Boot

On first boot, the controller will:

1. **Display startup screen** showing PlantNanny logo
2. **Enter Bluetooth pairing mode** (if not configured)
3. **Wait for mobile app connection** to configure WiFi
4. **Connect to WiFi** and sync with server
5. **Start monitoring** environmental conditions

## 5. Configure via Mobile App

::: info
Mobile app is under development. For now, configure via serial console or hardcoded values.
:::

### Via Bluetooth Pairing

1. Open PlantNanny mobile app
2. Create account or log in
3. Tap "Add New Plant Controller"
4. Select your controller from the list
5. Enter WiFi credentials
6. Configure plant settings

## 6. Monitor Your Plant

### On the Display

The TFT display shows:
- Current temperature
- Current humidity  
- Light level
- Last watering time
- Next scheduled watering

### Via Serial Console

```bash
# View real-time logs
pio device monitor

# Or use the screen command
screen /dev/ttyUSB0 115200
```

### Via Mobile App

- View real-time sensor data
- Check historical graphs
- Trigger manual watering
- Adjust plant settings

## 7. Test Manual Watering

### From Serial Console

Send commands via serial (if implemented):
```
water
```

### From Mobile App

1. Select your plant
2. Tap "Water Now" button
3. Confirm watering action

## Example Plant Configuration

Here's a sample configuration for a common houseplant:

```json
{
  "name": "Spider Plant",
  "optimal_temperature": 21,
  "optimal_humidity": 50,
  "watering_cycle": 172800,  // 2 days in seconds
  "water_quantity": 200,      // ml
  "location": {
    "latitude": 48.8566,
    "longitude": 2.3522
  }
}
```

## Troubleshooting

### Display Not Working

1. Check TFT_eSPI configuration in `platformio.ini`
2. Verify display pins match your board
3. Test with a simple display example

### Sensors Reading 0 or Invalid Values

1. Check wiring connections
2. Verify sensor power supply
3. Test sensors individually with example code
4. Check pull-up resistors (especially for DHT22)

### WiFi Not Connecting

1. Verify WiFi credentials
2. Check signal strength
3. Ensure 2.4GHz network (ESP32 doesn't support 5GHz)
4. Check router security settings (WPA2 recommended)

### Pump Not Activating

1. Check relay wiring and power supply
2. Verify relay control pin
3. Test relay with manual HIGH/LOW signals
4. Ensure pump power supply is adequate

## Next Steps

- [Configure plant thresholds](/guide/features/plant-config)
- [Understand the monitoring system](/guide/features/monitoring)
- [Explore the codebase](/development/project-structure)
- [Contribute to the project](https://github.com/DrHurel/IOT)

## Resources

- [ESP32 Documentation](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/)
- [PlatformIO Documentation](https://docs.platformio.org/)
- [TFT_eSPI Library](https://github.com/Bodmer/TFT_eSPI)
