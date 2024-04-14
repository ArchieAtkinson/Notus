#include <zephyr/kernel.h>

#include "testing.hpp"

#include "display.hpp"

using namespace std::literals::chrono_literals;

ZTEST(display, test_all)
{

    Display display{};
    constexpr double test_weight = 15.5;
    display.set_weight_grams(test_weight);
    auto test_time = 65s;
    display.set_time(test_time);
    
}

ZTEST_SUITE(display, nullptr, nullptr, nullptr, nullptr, nullptr); // NOLINT