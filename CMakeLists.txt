set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set (CMAKE_CXX_STANDARD 20)
set(APPLICATION_ROOT /home/user/zephyrproject/zephyr/application)

cmake_minimum_required(VERSION 3.20.0)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(hello_world)

target_sources(app PRIVATE src/main.cpp src/stopwatch.cpp)


add_custom_command(
    TARGET app PRE_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
            ${CMAKE_BINARY_DIR}/compile_commands.json
            ${APPLICATION_ROOT}/compile_commands.json
    COMMENT "Moving compile_commands.json to ${DESTINATION_DIR}"
)