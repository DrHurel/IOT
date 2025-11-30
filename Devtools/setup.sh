#!/bin/bash
source "$(dirname -- "$(readlink -f "${BASH_SOURCE}")")"/common_variables.sh

if [ ! -d "$UM_ROOT_PATH/tmp" ]; then
    echo "Creating tmp directory..."
    mkdir  "$UM_ROOT_PATH/tmp"
fi
if [ ! -d "$UM_ROOT_PATH/Builds" ]; then
    echo "Creating Builds directory..."
    mkdir  "$UM_ROOT_PATH/Builds"
fi

sudo apt-get update
sudo apt-get install python3 python3-pip python3-venv -y

# Install PlatformIO if not already installed
if [ ! -f "$HOME/.platformio/penv/bin/platformio" ]; then
    echo "Installing PlatformIO..."
    python3 -c "$(curl -fsSL https://raw.githubusercontent.com/platformio/platformio/master/scripts/get-platformio.py)"
fi

if [ ! -d "$UM_ROOT_PATH/.venv" ]; then
    echo "Creating virtual environment..."
    python3 -m venv "$UM_ROOT_PATH/.venv"
fi
source "$UM_ROOT_PATH/.venv/bin/activate"
pip install -r "$UM_ROOT_PATH/requirements.txt"

# Create .env file for WiFi credentials if it doesn't exist
if [ ! -f "$UM_ROOT_PATH/.env" ]; then
    echo "Creating .env file for WiFi credentials..."
    echo "# WiFi Configuration" > "$UM_ROOT_PATH/.env"
    echo "# Edit this file to add your WiFi credentials" >> "$UM_ROOT_PATH/.env"
    echo "" >> "$UM_ROOT_PATH/.env"
    
    read -p "Enter WiFi SSID: " wifi_ssid
    read -sp "Enter WiFi Password: " wifi_password
    echo ""
    
    echo "WIFI_SSID=$wifi_ssid" >> "$UM_ROOT_PATH/.env"
    echo "WIFI_PASSWORD=$wifi_password" >> "$UM_ROOT_PATH/.env"
    
    echo ".env file created successfully!"
else
    echo ".env file already exists. Skipping WiFi credential setup."
    echo "To update credentials, edit $UM_ROOT_PATH/.env manually."
fi
