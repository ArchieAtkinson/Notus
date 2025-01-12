cmake_minimum_required(VERSION 3.20.0)
list(APPEND CMAKE_MODULE_PATH "$ENV{NOTUS_ROOT}/cmake")
list(APPEND DTS_PTH "$ENV{NOTUS_ROOT}/cmake")
include(Util)

ProjectSetup("standard;testing")


find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(button)

set(SOURCES 
    src/main.cpp
    src/mock.c
)

target_sources(app PRIVATE ${SOURCES})
target_include_directories(app PRIVATE src)

SetupTarget(app)

RunClangdTidy(app "${SOURCES}")