cmake_minimum_required(VERSION 3.20.0)
list(APPEND CMAKE_MODULE_PATH "$ENV{APPLICATION_ROOT}/cmake")
include(Util)

ProjectSetup("standard")


find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(ActiveObject)

set(SOURCES 
    src/main.cpp
)

target_sources_ifdef(CONFIG_MODULE1 app PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src/module1.cpp)

target_sources(app PRIVATE ${SOURCES})
SetupTarget(app)

target_compile_options(app PUBLIC -Wno-missing-field-initializers)
target_include_directories(app PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/includes)

