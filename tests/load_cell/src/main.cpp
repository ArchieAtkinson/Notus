#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/gpio/gpio_emul.h>

#include <zephyr/kernel.h>

#include "testing.hpp"

#include "load_cell.hpp"

namespace {

ZTEST(load_cell, test_constructor)
{
    const struct device *const adc_dev = (DEVICE_DT_GET(DT_NODELABEL(adc0)));

    LoadCell load_cell(adc_dev);
}

ZTEST_SUITE(load_cell, nullptr, nullptr, nullptr, nullptr, nullptr); // NOLINT

} // namespace