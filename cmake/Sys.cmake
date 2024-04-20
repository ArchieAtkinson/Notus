set(NOTUS_LIB_SAUCES "" CACHE INTERNAL "NOTUS_LIB_SAUCES")


function (notus_lib_add_sources)
    target_sources(notus_lib PRIVATE ${ARGN})
    get_filename_component(CURRENT_DIR "${CMAKE_CURRENT_LIST_FILE}" PATH)

    foreach(CURRENT_PATH ${ARGN}) 
        if(IS_ABSOLUTE ${CURRENT_PATH})
            list(APPEND NOTUS_LIB_SAUCES ${CURRENT_PATH})
        else()
            get_filename_component(ABSOLUTE_PATH "${CURRENT_DIR}/${CURRENT_PATH}" ABSOLUTE)
            list(APPEND NOTUS_LIB_SAUCES ${ABSOLUTE_PATH})
        endif()
    endforeach()

    set(NOTUS_LIB_SAUCES "${NOTUS_LIB_SAUCES}" CACHE INTERNAL "NOTUS_LIB_SAUCES")
endfunction()


function (notus_lib_sources_ifdef feature_toggle)
    if(${${feature_toggle}})
        notus_lib_add_sources(${ARGN})
    endif()
endfunction()


function (notus_lib_compile_options)
    # Ensure are flags are added at the end (handled by the zephyr func)
    set(ZEPHYR_CURRENT_LIBRARY notus_lib)
    zephyr_library_compile_options(${ARGN})
endfunction()


function (create_notus_lib)
    zephyr_library_named(notus_lib)

    file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/empty.cpp "")
    notus_lib_add_sources(${CMAKE_CURRENT_BINARY_DIR}/empty.cpp)

    notus_lib_compile_options(${SYS_COMPILE_FLAGS})
endfunction()
