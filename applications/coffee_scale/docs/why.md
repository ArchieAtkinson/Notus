# Why

## -Wno-invalid-offsetof
`-Wno-invalid-offsetof` is used in this codebase due to [Zephyr's `CONTAINER_OF`](https://docs.zephyrproject.org/latest/kernel/util/index.html#c.CONTAINER_OF) macro that makes use of the `offsetof()` of function. It's required to access user data in callbacks. It was chosen to disable the warning globally as `offsetof()` is unlikely to be used in application code.  It's possible to get around this issue with `#pramga` and `_Pragma()` however `clang-tidy` complains.

## Not using `required_snippets` in `testcases.yaml`
Currently `required_snippets` only checks Zephyr Root and each test directory, therefore doesn't work with the current setup. Therefore we are using `set(SNIPPET "standard")` in the `CMakeLists.txt`