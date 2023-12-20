#pragma once
#include <chrono>

#include "zephyr/kernel.h"

struct UpTime
{
    using duration   = std::chrono::milliseconds;
    using rep        = duration::rep;
    using period     = duration::period;
    using time_point = std::chrono::time_point<UpTime>;
    static constexpr bool is_steady = true;

    static time_point now() noexcept
    {
        using namespace std::chrono;
        return time_point{std::chrono::milliseconds(k_uptime_get())};
    }
};