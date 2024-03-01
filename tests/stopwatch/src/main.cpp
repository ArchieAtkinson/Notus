#include <chrono>
#include <iostream>

#include <zephyr/kernel.h>

#include "testing.hpp"

#include "stopwatch.hpp"

using millis = std::chrono::milliseconds;

constexpr auto long_wait = std::chrono::duration_cast<millis>(std::chrono::minutes(30));

constexpr auto short_wait = std::chrono::duration_cast<millis>(std::chrono::minutes(1));

/* Notes
zassert_within with 1ms timing due to clock not being perfect (limit tick rate)
Above times based on max typical usage
*/

ZTEST(stopwatch, test_start_and_reset)
{
    Stopwatch stop_watch{};
    stop_watch.start();

    k_msleep(long_wait.count());

    auto duration = stop_watch.get_time();
    zassert_within(duration, long_wait, millis(1));

    stop_watch.reset();

    duration = stop_watch.get_time();
    zassert_equal(duration, millis(0));

    k_msleep(long_wait.count());

    duration = stop_watch.get_time();
    zassert_within(duration, long_wait, millis(1));
}

ZTEST(stopwatch, test_double_pause)
{
    Stopwatch stop_watch{};

    stop_watch.start();

    k_msleep(short_wait.count());

    stop_watch.pause();

    k_msleep(short_wait.count());

    stop_watch.pause();

    k_msleep(short_wait.count());

    auto duration = stop_watch.get_time();

    zassert_within(duration, short_wait, millis(1));
}

ZTEST(stopwatch, test_pause_and_start)
{
    Stopwatch stop_watch{};

    stop_watch.start();

    k_msleep(long_wait.count()/2);

    stop_watch.pause();

    auto duration = stop_watch.get_time();

    zassert_within(duration, long_wait/2, millis(1));

    k_msleep(short_wait.count());

    stop_watch.start();

    k_msleep(long_wait.count()/2);

    stop_watch.pause();

    k_msleep(short_wait.count());

    duration = stop_watch.get_time();
    zassert_within(duration, long_wait, millis(1));
}


ZTEST(stopwatch, test_pause)
{
    Stopwatch stop_watch{};

    stop_watch.start();

    k_msleep(long_wait.count());

    stop_watch.pause();

    k_msleep(short_wait.count());

    auto duration = stop_watch.get_time();

    zassert_within(duration, long_wait, millis(1));
}

ZTEST(stopwatch, test_start_and_get)
{
    Stopwatch stop_watch{};

    stop_watch.start();

    k_msleep(long_wait.count());

    auto duration = stop_watch.get_time();

    std::cout << (duration - long_wait).count() << std::endl;
    
    zassert_within(duration, long_wait, millis(1)); 
}

ZTEST_SUITE(stopwatch, nullptr, nullptr, nullptr, nullptr, nullptr); // NOLINT
