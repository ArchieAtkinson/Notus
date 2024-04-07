function(ProjectSetup snippets)
    set(CMAKE_EXPORT_COMPILE_COMMANDS ON PARENT_SCOPE)
    set(CMAKE_CXX_STANDARD 20 PARENT_SCOPE)

    list(APPEND SNIPPET_ROOT $ENV{APPLICATION_ROOT})
    set(SNIPPET_ROOT ${SNIPPET_ROOT} PARENT_SCOPE)
    
    set(SNIPPET ${snippets} PARENT_SCOPE)
endfunction()


function(SetupTarget target_name)
    target_include_directories(${target_name} PUBLIC $ENV{APPLICATION_ROOT}/includes)

    target_compile_options(
        ${target_name}
        PUBLIC
        -Wall
        -Wextra
        -Werror
        -Wshadow
        -Wdouble-promotion 
        -Wno-invalid-offsetof
    )

    add_custom_command(TARGET ${target_name} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E rm -f $ENV{APPLICATION_ROOT}/compile_commands.json
        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_BINARY_DIR}/compile_commands.json $ENV{APPLICATION_ROOT}/compile_commands.json    COMMENT "Moving compile_commands.json to $ENV{APPPLICATION_ROOT}"
        VERBATIM
    )
endfunction()

function(RunClangdTidy target sources)
    add_custom_command(TARGET ${target} POST_BUILD
    COMMAND clangd-tidy
        -j 8
        -p ${CMAKE_BINARY_DIR}
        --clangd-executable clangd-$ENV{LLVM_VERSION}
        ${sources}
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMENT "Running clangd-tidy"
    )
endfunction()

function(RunClangTidy target)
    file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/clang-tidy-cc)

    add_custom_command(TARGET ${target} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E rm -f ${CMAKE_BINARY_DIR}/clang-tidy-cc/compile_commands.json
        COMMAND $ENV{APPLICATION_ROOT}/scripts/prepare_cc_for_clang_tidy.py 
            ${CMAKE_BINARY_DIR}/compile_commands.json
            ${CMAKE_BINARY_DIR}/clang-tidy-cc/compile_commands.json
        COMMAND run-clang-tidy-17
            -p ${CMAKE_BINARY_DIR}/clang-tidy-cc
            -quiet
        BYPRODUCTS ${CMAKE_BINARY_DIR}/clang-tidy-cc/compile_commands.json
        DEPENDS ${CMAKE_BINARY_DIR}/compile_commands.json
    )
endfunction()