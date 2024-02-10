#include <chrono>
#include <random>
#include <ranges>

#include <zephyr/kernel.h>

#include "testing.hpp"

#include "stopwatch.hpp"

constexpr int max_wait_ms = 10000;
constexpr int min_wait_ms = 1;

/* Notes
zassert_within with 1ms timing due to clock not being perfect (limit tick rate)
*/

int get_random_uniform(int min, int max)
{
	std::random_device rand_device;
    std::default_random_engine engine(rand_device());

    std::uniform_int_distribution<int> distribution(min, max);
    return distribution(engine);
}

ZTEST(stopwatch, test_start_and_reset)
{
	using millis = std::chrono::milliseconds;

    Stopwatch stop_watch{};
    stop_watch.start();

    int random_wait_ms = get_random_uniform(min_wait_ms, max_wait_ms);
    k_msleep(random_wait_ms);

    auto duration = stop_watch.get_time();
    zassert_within(duration, millis(random_wait_ms), millis(1));

    stop_watch.reset();

    duration = stop_watch.get_time();
    zassert_equal(duration, millis(0));

    random_wait_ms = get_random_uniform(min_wait_ms, max_wait_ms);
    k_msleep(random_wait_ms);

    duration = stop_watch.get_time();
    zassert_within(duration, millis(random_wait_ms), millis(1));
}

ZTEST(stopwatch, test_pause_and_start)
{
	using millis = std::chrono::milliseconds;

    Stopwatch stop_watch{};

    stop_watch.start();

    int first_random_wait = get_random_uniform(min_wait_ms, max_wait_ms);
    k_msleep(first_random_wait);

    stop_watch.pause();

    auto duration = stop_watch.get_time();

    zassert_within(duration, millis(first_random_wait), millis(1));

    k_msleep(get_random_uniform(min_wait_ms, max_wait_ms));

    stop_watch.start();

    int second_random_wait = get_random_uniform(min_wait_ms, max_wait_ms);
    k_msleep(second_random_wait);

    stop_watch.pause();

    k_msleep(get_random_uniform(min_wait_ms, max_wait_ms));

    duration = stop_watch.get_time();

    int combined_wait = first_random_wait + second_random_wait;
    zassert_within(duration, millis(combined_wait), millis(1));
}


ZTEST(stopwatch, test_pause)
{
	using millis = std::chrono::milliseconds;

    int random_wait = get_random_uniform(min_wait_ms, max_wait_ms);
    Stopwatch stop_watch{};

    stop_watch.start();

    k_msleep(random_wait);

    stop_watch.pause();

    k_msleep(get_random_uniform(min_wait_ms, max_wait_ms));

    auto duration = stop_watch.get_time();

    zassert_within(duration, millis(random_wait), millis(1));
}

ZTEST(stopwatch, test_start_and_get)
{
	using millis = std::chrono::milliseconds;

	for ([[maybe_unused]] auto unused : std::views::iota(0, 1000)) {
		int random_wait = get_random_uniform(min_wait_ms, max_wait_ms);

		Stopwatch stop_watch{};

		stop_watch.start();

		k_msleep(random_wait);

		auto duration = stop_watch.get_time();
		
		zassert_within(duration, millis(random_wait), millis(1)); 
	}
}

ZTEST_SUITE(stopwatch, nullptr, nullptr, nullptr, nullptr, nullptr); // NOLINT
