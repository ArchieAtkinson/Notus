#pragma once

#include <zephyr/logging/log_core.h>
#include <zephyr/logging/log.h>

/*

This header file is to wrap all the logging macros with // NOLINT to get rid of clang-tidy warnings that we can't avoid.
As this is done at the preprocessor level this shouldn't have any effect on performance.
*/

#undef Z_LOG
#define Z_LOG(_level, ...)                                                                                             \
    Z_LOG2(_level, 0, __log_current_const_data, __log_current_dynamic_data, __VA_ARGS__) // NOLINT


#undef LOG_MODULE_REGISTER
#define LOG_MODULE_REGISTER(...)                                                                                       \
    COND_CODE_1(Z_DO_LOG_MODULE_REGISTER(__VA_ARGS__),                                                                 \
                (_LOG_MODULE_DATA_CREATE(GET_ARG_N(1, __VA_ARGS__), _LOG_LEVEL_RESOLVE(__VA_ARGS__))), ())             \
    LOG_MODULE_DECLARE(__VA_ARGS__) // NOLINT



#undef LOG_MODULE_DECLARE
#define LOG2_MODULE_DECLARE(...)                                                                                       \
    extern const struct log_source_const_data Z_LOG_ITEM_CONST_DATA(GET_ARG_N(1, __VA_ARGS__));                        \
    extern struct log_source_dynamic_data     LOG_ITEM_DYNAMIC_DATA(GET_ARG_N(1, __VA_ARGS__));                        \
                                                                                                                       \
    static const struct log_source_const_data *__log_current_const_data __unused =                                     \
        Z_DO_LOG_MODULE_REGISTER(__VA_ARGS__) ? &Z_LOG_ITEM_CONST_DATA(GET_ARG_N(1, __VA_ARGS__)) : NULL;              \
                                                                                                                       \
    static struct log_source_dynamic_data *__log_current_dynamic_data __unused =                                       \
        (Z_DO_LOG_MODULE_REGISTER(__VA_ARGS__) && IS_ENABLED(CONFIG_LOG_RUNTIME_FILTERING))                            \
            ? &LOG_ITEM_DYNAMIC_DATA(GET_ARG_N(1, __VA_ARGS__))                                                        \
            : NULL;                                                                                                    \
                                                                                                                       \
    static const uint32_t __log_level __unused = _LOG_LEVEL_RESOLVE(__VA_ARGS__)


#define LOG_MODULE_DECLARE(...) LOG2_MODULE_DECLARE(__VA_ARGS__) // NOLINT	