#!/bin/bash
cd $NOTUS_ROOT
west init -l .
west update

# Install magic_enum
PROJECT_DIR=$WEST_WORKSPACE/magic_enum
cd $PROJECT_DIR
mkdir -p build
cmake -DMAGIC_ENUM_OPT_BUILD_EXAMPLES=OFF -DMAGIC_ENUM_OPT_BUILD_TESTS=OFF . -B build
cd build
sudo make install

# Install expected
PROJECT_DIR=$WEST_WORKSPACE/expected
cd $PROJECT_DIR
mkdir -p build
cmake -DEXPECTED_BUILD_TESTS=OFF -DMAGIC_ENUM_OPT_BUILD_TESTS=OFF . -B build
cd build
sudo make install

# Install optional
PROJECT_DIR=$WEST_WORKSPACE/optional
cd $PROJECT_DIR
mkdir -p build
cmake -DOPTIONAL_BUILD_TESTS=OFF . -B build
cd build
sudo make install

# Install etl
PROJECT_DIR=$WEST_WORKSPACE/etl
cd $PROJECT_DIR
mkdir -p build
cmake . -B build
cd build
sudo make install