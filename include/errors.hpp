#pragma once

#include <chrono>
#include <source_location>
#include <system_error>
#include <array>

#include "logging.hpp"
#include "uptime_tp.hpp"

class MajorError
{
  public:
    MajorError() = delete;
    explicit MajorError(std::error_code code, int err_no,
                        const std::source_location &loc = std::source_location::current())
        : _code{code}, _err_no{err_no}, _location{loc}
    {
        std::chrono::duration time_elapsed = UpTime::now().time_since_epoch();
        std::chrono::hh_mm_ss formatted_time{time_elapsed};
        uint32_t              microseconds = formatted_time.subseconds().count();
        constexpr int         microseconds_in_millisecond = 1000;
        constexpr int         milliseconds_in_seconds     = 1000;
        uint32_t milliseconds = microseconds / microseconds_in_millisecond;
        microseconds %= microseconds_in_millisecond;
        milliseconds %= milliseconds_in_seconds;

        std::array<char, 20> timestamp{};
        (void)std::snprintf(timestamp.data(), sizeof(timestamp), "[%02llu:%02llu:%02llu.%03u,%03u]", formatted_time.hours().count(), formatted_time.minutes().count(), formatted_time.seconds().count(), milliseconds, microseconds);

        LOG_PRINTK("\e[1;45m%s <exception>: %s:%d:%d - %s:%s:%d\e[0m\n", timestamp.data(), loc.file_name(), loc.line(), loc.column(), _code.category().name(), _code.message().c_str(), _err_no);
    }

    [[nodiscard]] std::error_code code() const {return _code;}
    [[nodiscard]] int err_no() const {return _err_no;}
    [[nodiscard]] std::source_location where()const {return _location;}

  private:
    std::error_code _code;
    int _err_no;
    std::source_location _location;
};