#!/bin/bash
# Launch the MQTT broker Docker container

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$SCRIPT_DIR"

# Stop existing container if running
docker stop plantnanny-mqtt 2>/dev/null
docker rm plantnanny-mqtt 2>/dev/null

# Run the container
docker run -d \
    --name plantnanny-mqtt \
    -p 1883:1883 \
    -p 9001:9001 \
    -v plantnanny-mqtt-data:/mosquitto/data \
    plantnanny-mqtt-broker

echo "MQTT broker started on ports 1883 (MQTT) and 9001 (WebSocket)"
echo "Use 'docker logs -f plantnanny-mqtt' to view logs"
