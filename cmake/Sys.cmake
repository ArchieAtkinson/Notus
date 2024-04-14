set(GEN_LIB_SAUCES "" CACHE INTERNAL "GEN_LIB_SAUCES")

function (gen_lib_sources_ifdef feature_toggle)
    if(${${feature_toggle}})
        target_sources(gen_lib PRIVATE ${ARGN})

        get_filename_component(CURRENT_DIR "${CMAKE_CURRENT_LIST_FILE}" PATH)

        foreach(RELATIVE_PATH ${ARGN})
            get_filename_component(ABSOLUTE_PATH "${CURRENT_DIR}/${RELATIVE_PATH}" ABSOLUTE)
            list(APPEND GEN_LIB_SAUCES ${ABSOLUTE_PATH})
        endforeach()

        set(GEN_LIB_SAUCES "${GEN_LIB_SAUCES}" CACHE INTERNAL "GEN_LIB_SAUCES")
        message("GEN_LIB_SAUCE system include directories: ${GEN_LIB_SAUCES}")
    endif()
endfunction()

function (gen_lib_compile_options)
    # Ensure are flags are added at the end (handled by the zephyr func)
    set(ZEPHYR_CURRENT_LIBRARY gen_lib)
    zephyr_library_compile_options(${ARGN})
endfunction()
