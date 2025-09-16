rm -rf Build
mkdir Build
cmake -S . -B Build
cmake --build Build $@