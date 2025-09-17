#!/bin/bash
source "$(dirname -- "$(readlink -f "${BASH_SOURCE}")")"/common_variables.sh
rm -rf $UM_ROOT_PATH/Builds
mkdir $UM_ROOT_PATH/Builds
cmake -S . -B $UM_ROOT_PATH/Builds
cmake --build $UM_ROOT_PATH/Builds $@