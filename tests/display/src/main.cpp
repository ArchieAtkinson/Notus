#include <zephyr/kernel.h>

#include "testing.hpp"

#include "display.hpp"

using namespace std::literals::chrono_literals;

ZTEST(display, test_all)
{

    Display display{};

    display.set_weight_grams(15.5);
    display.set_time(65s);
    
}

ZTEST_SUITE(display, nullptr, nullptr, nullptr, nullptr, nullptr); // NOLINT