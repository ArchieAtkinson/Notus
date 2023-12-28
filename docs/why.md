# Why

## -Wno-invalid-offsetof
`-Wno-invalid-offsetof` is used in this codebase due to [Zephyr's `CONTAINER_OF`](https://docs.zephyrproject.org/latest/kernel/util/index.html#c.CONTAINER_OF) macro that makes use of the `offsetof()` of function. It's required to access user data in callbacks. It was chosen to disable the warning globally as `offsetof()` is unlikely to be used in application code.  It's possible to get around this issue with `#pramga` and `_Pragma()` however `clang-tidy` complains.

## clang-tidy not being part of the build process

clang-tidy is a very useful static analyses tools that picks up on coding errors and enforces best practices in ways the compiler is unable to. However, due to Zephyr being based on GCC clang-tidy when used within CMake with `CMAKE_CXX_CLANG_TIDY` it fails to due to our complication making use of compiler flags that are not available on clang. Therefore we are limited to only using it through clangd until a workaround has been found.

 

