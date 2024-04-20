include(Util)
include(Sys)

create_notus_lib()

zephyr_include_directories(include)

add_subdirectory(subsys)

RunClangdTidy(notus_lib "${NOTUS_LIB_SAUCES}")