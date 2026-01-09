#!/bin/bash
# Build the MQTT broker Docker image

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$SCRIPT_DIR"

docker build -t plantnanny-mqtt-broker .
