set(SYS_COMPILE_FLAGS 
    -Wall
    -Wextra
    -Werror
    -Wshadow
    -Wdouble-promotion
    -Wno-invalid-offsetof
    CACHE STRING "SYS_COMPILE_FLAGS"
)

function(ProjectSetup snippets)
    set(CMAKE_EXPORT_COMPILE_COMMANDS ON PARENT_SCOPE)
    set(CMAKE_CXX_STANDARD 20 PARENT_SCOPE)

    list(APPEND SNIPPET_ROOT $ENV{NOTUS_ROOT})
    set(SNIPPET_ROOT ${SNIPPET_ROOT} PARENT_SCOPE)

    list(APPEND DTS_ROOT $ENV{NOTUS_ROOT})
    set(DTS_ROOT ${DTS_ROOT} PARENT_SCOPE)
    
    set(SNIPPET ${snippets} PARENT_SCOPE)

    # Use CodeChecker during CI as its slower but covers more-
    if(DEFINED ENV{GITHUB_ACTIONS})
        set(ZEPHYR_SCA_VARIANT codechecker PARENT_SCOPE)
        set(CODECHECKER_PARSE_EXIT_STATUS y PARENT_SCOPE)
        set(CODECHECKER_ANALYZE_OPTS
            -i $ENV{NOTUS_ROOT}/codechecker_skip
            --analyzers=clang-tidy
            --analyzer-config=clang-tidy:take-config-from-directory=true
            --quiet
            --clean
            PARENT_SCOPE
        )
    endif()

endfunction()

function(SetupTarget target_name)
    target_include_directories(${target_name} PUBLIC $ENV{NOTUS_ROOT}/includes)
    target_include_directories(${target_name} PUBLIC $ENV{NOTUS_ROOT})

    target_compile_options(
        ${target_name}
        PUBLIC
        ${SYS_COMPILE_FLAGS}
    )
    
    add_custom_command(
        TARGET ${target_name} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E rm -f $ENV{NOTUS_ROOT}/compile_commands.json
        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_BINARY_DIR}/compile_commands.json $ENV{NOTUS_ROOT}/compile_commands.json    COMMENT "Moving compile_commands.json to $ENV{APPPLICATION_ROOT}"
        VERBATIM
    )

    if("${ZEPHYR_SCA_VARIANT}" STREQUAL "codechecker")
        # Required for CodeChecker/clang-tidy to not complain
        add_custom_target(ccClean ALL
            COMMAND sed -i 's/-fno-freestanding//g' ${CMAKE_BINARY_DIR}/compile_commands.json
            BYPRODUCTS ${CMAKE_BINARY_DIR}/compile_commands.json
            COMMENT "Removing -fno-freestanding from compile_commands.json"
        )
        add_dependencies(codechecker ccClean)
    endif()
    
endfunction()

function(RunClangdTidy target sources)
    # Use clangd-tidy during local dev as its faster
    if(NOT DEFINED ENV{GITHUB_ACTIONS})
        add_custom_command(TARGET ${target} POST_BUILD
        COMMAND clangd-tidy
            -j 8
            -p ${CMAKE_BINARY_DIR}
            --clangd-executable clangd-$ENV{LLVM_VERSION}
            ${sources}
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMENT "Running clangd-tidy"
        )
    endif()
endfunction()
