#include <chrono>
#include <random>
#include <ranges>


#include <zephyr/ztest.h>
#include <zephyr/kernel.h>

#include "stopwatch.hpp"

int get_random(int min, int max)
{
	std::random_device rd;
    std::default_random_engine engine(rd());

    std::uniform_int_distribution<int> distribution(min, max);
    return distribution(engine);
}

ZTEST(stopwatch, test_start_and_reset)
{
	using millis = std::chrono::milliseconds;

    Stopwatch stop_watch{};
    stop_watch.start();

    int random_wait = get_random(1, 10000);
    k_msleep(random_wait);

    auto duration = stop_watch.get_time();
    zassert_within(duration, millis(random_wait), millis(1));

    stop_watch.reset();

    duration = stop_watch.get_time();
    zassert_equal(duration, millis(0));

    random_wait = get_random(1, 10000);
    k_msleep(random_wait);

    duration = stop_watch.get_time();
    zassert_within(duration, millis(random_wait), millis(1));
}

ZTEST(stopwatch, test_pause_and_start)
{
	using millis = std::chrono::milliseconds;

    
    Stopwatch stop_watch{};

    stop_watch.start();

    int random_wait = get_random(1, 10000);
    k_msleep(random_wait);

    stop_watch.pause();

    auto duration = stop_watch.get_time();

    zassert_within(duration, millis(random_wait), millis(1));

    k_msleep(get_random(10, 10000));

    stop_watch.start();

    int random_wait_2 = get_random(1, 10000);
    k_msleep(random_wait_2);

    stop_watch.pause();

    k_msleep(get_random(10, 10000));

    duration = stop_watch.get_time();

    int combinded_wait = random_wait + random_wait_2;
    zassert_within(duration, millis(combinded_wait), millis(1));
}


ZTEST(stopwatch, test_pause)
{
	using millis = std::chrono::milliseconds;

    int random_wait = get_random(1, 10000);
    Stopwatch stop_watch{};

    stop_watch.start();

    k_msleep(random_wait);

    stop_watch.pause();

    k_msleep(get_random(10, 10000));

    auto duration = stop_watch.get_time();

    // within 1ms due to clock not being perfect (limit tick rate)
    zassert_within(duration, millis(random_wait), millis(1));
}

ZTEST(stopwatch, test_start_and_get)
{
	using millis = std::chrono::milliseconds;

	for ([[maybe_unused]] auto _ : std::views::iota(0, 1000)) {
		int random_wait = get_random(0, 10000);

		Stopwatch stop_watch{};

		stop_watch.start();

		k_msleep(random_wait);

		auto duration = stop_watch.get_time();
		
		// within 1ms due to clock not being perfect (limit tick rate)
		zassert_within(duration, millis(random_wait), millis(1)); 
	}
}

ZTEST_SUITE(stopwatch, NULL, NULL, NULL, NULL, NULL);
