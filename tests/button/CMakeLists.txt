set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set (CMAKE_CXX_STANDARD 20)
set(APPLICATION_ROOT /home/user/zephyrproject/zephyr/application)

cmake_minimum_required(VERSION 3.20.0)


find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(button)


target_include_directories(app PUBLIC ${APPLICATION_ROOT}/includes)
target_sources(app PRIVATE src/main.cpp ${APPLICATION_ROOT}/src/button.cpp)


add_custom_command(
    TARGET app POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
            ${CMAKE_BINARY_DIR}/compile_commands.json
            ${APPLICATION_ROOT}/compile_commands.json
    COMMENT "Moving compile_commands.json to ${APPLICATION_ROOT}"
)