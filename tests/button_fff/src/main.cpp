#include <zephyr/drivers/gpio.h>

#include <zephyr/fff.h>
#include <zephyr/kernel.h>

#include "errors.hpp"
#include "testing.hpp"

#include "button.hpp"

#include "mock.h"

DEFINE_FFF_GLOBALS; // NOLINT

#define BUTTON_NODE DT_NODELABEL(mgpio)
#define BUTTON_CHILD_NODE DT_CHILD(BUTTON_NODE, button)

/* Alternative, more readable approach using DT macros */
#define GPIO_SPEC                                                                                                      \
    {                                                                                                                  \
        .port = DEVICE_DT_GET(BUTTON_NODE), .pin = DT_GPIO_HOG_PIN_BY_IDX(BUTTON_CHILD_NODE, 0),                       \
        .dt_flags = DT_GPIO_HOG_FLAGS_BY_IDX(BUTTON_CHILD_NODE, 0),                                                    \
    }

namespace
{

ZTEST(button, test_constructor_throw)
{
    static struct gpio_dt_spec irq_pin = GPIO_SPEC;
    irq_pin.port->state->initialized   = false;

    bool exception_caught = false;
    try
    {
        Button button(&irq_pin, K_MSEC(5));
    }
    catch (const MajorError &e)
    {
        zassert_equal(e.code(), ButtonError::gpio_is_ready);
        exception_caught = true;
    }
    catch (...)
    {
        zassert_unreachable();
    }

    zassert_true(exception_caught);

    irq_pin.port->state->initialized = true;
}


ZTEST(button, test_on_press_and_debounce)
{
    static struct gpio_dt_spec irq_pin = GPIO_SPEC;

    static struct gpio_callback * callback_data = nullptr; // NOLINT

    auto handle_callback = [](const struct device *port, struct gpio_callback *callback, bool set) -> int {
        ARG_UNUSED(port);
        if (set)
        {
            callback_data = callback;
        }
        else
        {
            callback_data = nullptr;
        }
        
        return 0;
    };

    manage_callback_mock_fake.custom_fake = handle_callback;

    atomic_t          cb_call_count    = ATOMIC_INIT(0);
    auto              cb_test2         = [&]() { atomic_inc(&cb_call_count); };
    const k_timeout_t debounce_time_ms = K_MSEC(5);
    Button            button(&irq_pin, debounce_time_ms);

    zassert_equal(pin_configure_mock_fake.call_count, 1);
    zassert_equal(pin_interrupt_configure_mock_fake.call_count, 1);
    zassert_equal(manage_callback_mock_fake.call_count, 1);

    button.add_on_press_callback(cb_test2);

    auto debounce_sim = [](int total_debounce_time_ms)
    {
        
        for (int i = 0; i < total_debounce_time_ms; i++)
        {
            if (callback_data != nullptr)
            {
                callback_data->handler(nullptr, callback_data, 0);
            }

            k_sleep(K_MSEC(1));

            if (callback_data != nullptr)
            {
                callback_data->handler(nullptr, callback_data, 0);
            }
        }
    };

    constexpr int estimate_debounce_time_ms = 5;
    debounce_sim(estimate_debounce_time_ms);

    zassert_equal(atomic_get(&cb_call_count), 1);
}

void my_suite_before(void *fixture)
{
    ARG_UNUSED(fixture);
    static struct gpio_dt_spec irq_pin = GPIO_SPEC;
    gpio_pin_configure_dt(&irq_pin, GPIO_INPUT);
    RESET_FAKE(pin_configure_mock);
    RESET_FAKE(port_get_raw_mock);
    RESET_FAKE(port_set_masked_raw_mock);
    RESET_FAKE(port_set_bits_raw_mock);
    RESET_FAKE(port_clear_bits_raw_mock);
    RESET_FAKE(port_toggle_bits_mock);
    RESET_FAKE(pin_interrupt_configure_mock);
    RESET_FAKE(manage_callback_mock);
    RESET_FAKE(get_pending_int_mock);
}

ZTEST_SUITE(button, nullptr, nullptr, my_suite_before, nullptr, nullptr); // NOLINT

} // namespace