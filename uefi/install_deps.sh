#!/bin/bash

# When Linux is detected, assume Ubuntu
# TODO add more specific checks...
if [[ "$OSTYPE" == "linux-gnu"* ]]; then
    sudo apt install nasm g++ build-essential acpica-tools uuid-dev
elif [[ "$OSTYPE" == "darwin"* ]]; then
    brew install nasm qemu acpica
else
    printf "[ERROR] Unknown system. Quitting!\n"
    exit 1
fi

git submodule update --init --recursive
