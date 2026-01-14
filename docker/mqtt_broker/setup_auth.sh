#!/bin/bash
# Setup MQTT authentication for PlantNanny
# ========================================
# This script generates proper password hashes for the Mosquitto broker

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PASSWD_FILE="${SCRIPT_DIR}/passwd"

echo "PlantNanny MQTT Password Setup"
echo "=============================="
echo ""

# Check if mosquitto_passwd is available
if ! command -v mosquitto_passwd &> /dev/null; then
    echo "mosquitto_passwd not found. Attempting to use Docker..."
    USE_DOCKER=true
else
    USE_DOCKER=false
fi

# Default passwords (can be overridden via environment)
SERVER_USER="${MQTT_SERVER_USER:-plantnanny_server}"
SERVER_PASS="${MQTT_SERVER_PASSWORD:-server_secret_2024}"
DEVICE_USER="${MQTT_DEVICE_USER:-plantnanny_device}"
DEVICE_PASS="${MQTT_DEVICE_PASSWORD:-device_secret_2024}"

# Create password file
if [ "$USE_DOCKER" = true ]; then
    echo "Using Docker to generate passwords..."
    
    # Create temp passwd file
    docker run --rm -v "${SCRIPT_DIR}:/mosquitto/config" eclipse-mosquitto:2 \
        mosquitto_passwd -c -b /mosquitto/config/passwd "$SERVER_USER" "$SERVER_PASS"
    
    docker run --rm -v "${SCRIPT_DIR}:/mosquitto/config" eclipse-mosquitto:2 \
        mosquitto_passwd -b /mosquitto/config/passwd "$DEVICE_USER" "$DEVICE_PASS"
else
    echo "Using local mosquitto_passwd..."
    
    # Create password file with server user
    mosquitto_passwd -c -b "$PASSWD_FILE" "$SERVER_USER" "$SERVER_PASS"
    
    # Add device user
    mosquitto_passwd -b "$PASSWD_FILE" "$DEVICE_USER" "$DEVICE_PASS"
fi

echo ""
echo "✓ Password file created: $PASSWD_FILE"
echo ""
echo "Credentials (save these securely):"
echo "  Server: $SERVER_USER / $SERVER_PASS"
echo "  Device: $DEVICE_USER / $DEVICE_PASS"
echo ""
echo "Add these to your .env file:"
echo "  MQTT_USERNAME=$SERVER_USER"
echo "  MQTT_PASSWORD=$SERVER_PASS"
echo ""
echo "For ESP32 devices, use:"
echo "  MQTT_USERNAME=$DEVICE_USER"
echo "  MQTT_PASSWORD=$DEVICE_PASS"
echo ""
