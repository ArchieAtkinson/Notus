cmake_minimum_required(VERSION 3.20.0)
list(APPEND CMAKE_MODULE_PATH "$ENV{NOTUS_ROOT}/cmake")
include(Util)

ProjectSetup("standard")

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(boost-leaf)

set(SOURCES 
    src/main.cpp
)

target_compile_definitions(app PUBLIC BOOST_LEAF_EMBEDDED BOOST_LEAF_NO_EXCEPTIONS)

target_sources(app PRIVATE ${SOURCES})
SetupTarget(app)
