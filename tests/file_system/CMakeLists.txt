cmake_minimum_required(VERSION 3.20.0)
list(APPEND CMAKE_MODULE_PATH "$ENV{APPLICATION_ROOT}/cmake")
include(Util)

ProjectSetup("standard;testing")

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(file_system)

set(SOURCES 
    src/main.cpp
    $ENV{APPLICATION_ROOT}/src/file_system.cpp
    $ENV{APPLICATION_ROOT}/src/file_system_error.cpp

)

target_sources(app PRIVATE ${SOURCES})
SetupTarget(app)

RunClangdTidy(app "${SOURCES}")