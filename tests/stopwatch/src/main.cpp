#include <chrono>

#include <zephyr/kernel.h>

#include "testing.hpp"

#include "stopwatch.hpp"

using millis = std::chrono::milliseconds;

constexpr auto long_timer_duration_min = std::chrono::minutes(30);
constexpr auto long_timer_duration_millis = std::chrono::duration_cast<millis>(long_timer_duration_min);
constexpr int long_wait_ms = long_timer_duration_millis.count();

constexpr auto short_timer_duration_min = std::chrono::minutes(1);
constexpr auto short_timer_duration_millis = std::chrono::duration_cast<millis>(short_timer_duration_min);
constexpr int short_wait_ms = short_timer_duration_millis.count();

/* Notes
zassert_within with 1ms timing due to clock not being perfect (limit tick rate)
Above times based on max typical usage
*/

ZTEST(stopwatch, test_start_and_reset)
{
    Stopwatch stop_watch{};
    stop_watch.start();

    k_msleep(long_wait_ms);

    auto duration = stop_watch.get_time();
    zassert_within(duration, millis(long_wait_ms), millis(1));

    stop_watch.reset();

    duration = stop_watch.get_time();
    zassert_equal(duration, millis(0));

    k_msleep(long_wait_ms);

    duration = stop_watch.get_time();
    zassert_within(duration, millis(long_wait_ms), millis(1));
}

ZTEST(stopwatch, test_double_pause)
{
    Stopwatch stop_watch{};

    stop_watch.start();

    k_msleep(short_wait_ms);

    stop_watch.pause();

    k_msleep(short_wait_ms);

    stop_watch.pause();

    k_msleep(short_wait_ms);

    auto duration = stop_watch.get_time();

    zassert_within(duration, millis(short_wait_ms), millis(1));
}

ZTEST(stopwatch, test_pause_and_start)
{
    Stopwatch stop_watch{};

    stop_watch.start();

    k_msleep(long_wait_ms/2);

    stop_watch.pause();

    auto duration = stop_watch.get_time();

    zassert_within(duration, millis(long_wait_ms/2), millis(1));

    k_msleep(short_wait_ms);

    stop_watch.start();

    k_msleep(long_wait_ms/2);

    stop_watch.pause();

    k_msleep(short_wait_ms);

    duration = stop_watch.get_time();
    zassert_within(duration, millis(long_wait_ms), millis(1));
}


ZTEST(stopwatch, test_pause)
{
    Stopwatch stop_watch{};

    stop_watch.start();

    k_msleep(long_wait_ms);

    stop_watch.pause();

    k_msleep(short_wait_ms);

    auto duration = stop_watch.get_time();

    zassert_within(duration, millis(long_wait_ms), millis(1));
}

ZTEST(stopwatch, test_start_and_get)
{
    Stopwatch stop_watch{};

    stop_watch.start();

    k_msleep(long_wait_ms);

    auto duration = stop_watch.get_time();
    
    zassert_within(duration, millis(long_wait_ms), millis(1)); 
}

ZTEST_SUITE(stopwatch, nullptr, nullptr, nullptr, nullptr, nullptr); // NOLINT
