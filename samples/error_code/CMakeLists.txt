cmake_minimum_required(VERSION 3.20.0)
list(APPEND CMAKE_MODULE_PATH "$ENV{APPLICATION_ROOT}/cmake")
include(Util)

ProjectSetup("standard")

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(button)

set(SOURCES 
    src/main.cpp
    src/flights.cpp
    src/flights2.cpp
    src/queries.cpp
    src/seats.cpp
)

target_sources(app PRIVATE ${SOURCES})
SetupTarget(app)