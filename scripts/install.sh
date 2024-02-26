#!/bin/bash

# Install magic_enum
PROJECT_DIR=$WEST_WORKSPACE/magic_enum
cd $PROJECT_DIR
mkdir -p build
cmake . -B build
cd build
sudo make install