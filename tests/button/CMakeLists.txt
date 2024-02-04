set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 20)

list(APPEND SNIPPET_ROOT $ENV{APPLICATION_ROOT})
set(SNIPPET "standard" "testing")

cmake_minimum_required(VERSION 3.20.0)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(button)

set(SOURCES 
    src/main.cpp
    $ENV{APPLICATION_ROOT}/src/button.cpp
)

target_sources(app PRIVATE ${SOURCES})
target_include_directories(app PUBLIC $ENV{APPLICATION_ROOT}/includes)
target_compile_options(app PUBLIC -Wall -Wextra -Werror -Wshadow -Wdouble-promotion -Wno-invalid-offsetof)

add_custom_command(TARGET app POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E rm -f $ENV{APPLICATION_ROOT}/compile_commands.json
    COMMAND cp -rf ${CMAKE_BINARY_DIR}/compile_commands.json $ENV{APPLICATION_ROOT}/compile_commands.json
    COMMENT "Moving compile_commands.json to $ENV{APPPLICATION_ROOT}"
    VERBATIM
)