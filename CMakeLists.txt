list(APPEND CMAKE_MODULE_PATH "$ENV{NOTUS_ROOT}/cmake")

include(Util)
include(Sys)


if(NOT CONFIG_RUST) 
    create_notus_lib()
    zephyr_include_directories(include)
    RunClangdTidy(notus_lib "${NOTUS_LIB_SAUCES}")
endif()

add_subdirectory(subsys)
