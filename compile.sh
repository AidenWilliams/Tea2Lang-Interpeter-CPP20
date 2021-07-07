#!/bin/bash
# For windows
cmake -B "cmake-build-debug"/
make -C "cmake-build-debug"/

# For linux
cmake -B "cmake-build-wsl"/
make -C "cmake-build-wsl"/