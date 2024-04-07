#include <zephyr/drivers/display.h>
#include <zephyr/kernel.h>
#include <zephyr/device.h>

#include "logging.hpp"

#include "display.hpp"

LOG_MODULE_REGISTER(display, LOG_LEVEL_DBG);

// Very placeholder implementation

void Display::set_weight_grams(double weight_grams)
{
    LOG_DBG("Weight: %.2fg", weight_grams);
}


void Display::set_time(std::chrono::milliseconds milliseconds)
{
    auto total_seconds = std::chrono::duration_cast<std::chrono::seconds>(milliseconds);
    int total_seconds_count = static_cast<int>(total_seconds.count());

    constexpr int seconds_in_min = std::chrono::seconds(std::chrono::minutes(1)).count();
    int minutes = total_seconds_count / seconds_in_min;
    int seconds = total_seconds_count % seconds_in_min;

    // Output in "MM:SS" format
    LOG_DBG("%02d:%02d\n", minutes, seconds);
}