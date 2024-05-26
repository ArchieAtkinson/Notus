cmake_minimum_required(VERSION 3.20.0)
list(APPEND CMAKE_MODULE_PATH "$ENV{NOTUS_ROOT}/cmake")
include(Util)

ProjectSetup("standard")

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(coroutine)

set(SOURCES 
    src/main.cpp
    src/scheduler.hpp
    src/scheduler.cpp
    src/task.hpp
    src/active_object.cpp
    src/active_object.hpp
    src/ao1.cpp
    src/ao1.hpp
)

set_property(SOURCE ${SOURCES} APPEND PROPERTY COMPILE_OPTIONS "-fcoroutines")

target_include_directories(app PRIVATE src)

target_sources(app PRIVATE ${SOURCES})
SetupTarget(app)