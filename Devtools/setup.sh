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
if [ ! -d "$UM_ROOT_PATH/.venv" ]; then
    echo "Creating virtual environment..."
    python3 -m venv "$UM_ROOT_PATH/.venv"
fi
source "$UM_ROOT_PATH/.venv/bin/activate"
pip install -r "$UM_ROOT_PATH/requirements.txt"
