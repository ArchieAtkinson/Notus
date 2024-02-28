#pragma once

#include <source_location>
#include <system_error>

#include "logging.hpp"
#include "zephyr/kernel.h"

class MajorError
{
  public:
    MajorError() = delete;
    explicit MajorError(std::error_code code, int err_no,
                        const std::source_location &loc = std::source_location::current())
        : _code{code}, _err_no{err_no}, _location{loc}
    {
        uint32_t cycles = k_cycle_get_32();
        uint32_t us_per_cycle = 1000000 / CONFIG_SYS_CLOCK_HW_CYCLES_PER_SEC;
        uint32_t us = cycles * us_per_cycle;
        uint32_t ms = us / 1000;
        uint32_t seconds = ms / 1000;
        uint32_t mins = seconds / 60;
        uint32_t hours = mins / 60;

        us %= 1000;
        ms %= 1000;
        seconds %= 60;
        mins %= 60;
        hours %= 24; // Assuming a 24-hour format

        char timestamp[20];
        (void)std::snprintf(timestamp, sizeof(timestamp), "[%02u:%02u:%02u.%03u,%03u]", hours, mins, seconds, ms, us);

        LOG_PRINTK("\e[1;45m%s <exception>: %s:%d:%d\e[0m\n", timestamp, loc.file_name(), loc.line(), loc.column());
    }

    [[nodiscard]] std::error_code code() const {return _code;}
    [[nodiscard]] int err_no() const {return _err_no;}
    [[nodiscard]] std::source_location where()const {return _location;}

  private:
    const std::error_code _code;
    const int _err_no;
    const std::source_location _location;
};