cmake_minimum_required(VERSION 3.20.0)
list(APPEND CMAKE_MODULE_PATH "$ENV{APPLICATION_ROOT}/cmake")
include(Util)

ProjectSetup("standard")


find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(zbus_observers)

set(SOURCES 
    src/main.cpp
)

target_sources(app PRIVATE ${SOURCES})
SetupTarget(app)
target_compile_options(app PUBLIC -Wno-missing-field-initializers)
target_include_directories(app PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/includes)

add_subdirectory(src/listener)
add_subdirectory(src/sub)
add_subdirectory(src/msg_sub)