#pragma once
#include <chrono>
#include <cstdint>

#include "zephyr/kernel.h"

struct UpTime
{
    using duration   = std::chrono::microseconds;
    using rep        = duration::rep;
    using period     = duration::period;
    using time_point = std::chrono::time_point<UpTime>;
    static constexpr bool is_steady = true;

    static time_point now() noexcept
    {
        using namespace std::chrono;
        auto microseconds_per_second = duration_cast<microseconds>(seconds(1));
        const auto microseconds_passed = std::chrono::microseconds(k_uptime_ticks() * microseconds_per_second.count() / CONFIG_SYS_CLOCK_TICKS_PER_SEC);
        return time_point{microseconds_passed};
    }
};