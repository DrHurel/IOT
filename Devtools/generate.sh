#!/bin/bash
source "$(dirname -- "$(readlink -f "${BASH_SOURCE}")")"/common_variables.sh

$UM_ROOT_PATH/.venv/bin/python3 $UM_ROOT_PATH/Devtools/python/class_creator.py
