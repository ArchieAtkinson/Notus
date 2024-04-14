include(Util)
include(Sys)

zephyr_library_named(gen_lib)
gen_lib_compile_options(${SYS_COMPILE_FLAGS})

zephyr_include_directories(include)

add_subdirectory(subsys)

RunClangdTidy(gen_lib "${GEN_LIB_SAUCES}")