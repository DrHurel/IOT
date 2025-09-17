#!/bin/bash
source "$(dirname -- "$(readlink -f "${BASH_SOURCE}")")"/common_variables.sh
cmake --build $UM_ROOT_PATH/Builds $@