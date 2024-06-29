#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/gpio/gpio_emul.h>

#include <zephyr/kernel.h>

#include "errors.hpp"
#include "testing.hpp"

#include "button.hpp"

namespace
{


ZTEST(button, test_constructor_throw)
{
    static struct gpio_dt_spec irq_pin = GPIO_DT_SPEC_GET(DT_INST(0, test_button), button_gpios);
    irq_pin.port->state->initialized   = false;
    
    bool exception_caught = false;
    try {
        Button button(&irq_pin, K_MSEC(5));
    }
    catch (const MajorError &e)
    {
        zassert_equal(e.code(), ButtonError::gpio_is_ready);
        exception_caught = true;
    }
    catch(...)
    {
        zassert_unreachable();   
    }

    zassert_true(exception_caught);

    irq_pin.port->state->initialized  = true;
}


ZTEST(button, test_on_press_and_debounce)
{
    static struct gpio_dt_spec irq_pin = GPIO_DT_SPEC_GET(DT_INST(0, test_button), button_gpios);

    atomic_t cb_call_count = ATOMIC_INIT(0);
    auto cb_test2  = [&]()
    {
        atomic_inc(&cb_call_count);
    };
    const k_timeout_t debounce_time_ms = K_MSEC(5);
    Button            button(&irq_pin, debounce_time_ms);

    button.add_on_press_callback(cb_test2);

    auto debounce_sim = [](int total_debounce_time_ms)
    {
        for (int i = 0; i < total_debounce_time_ms; i++)
        {
            gpio_emul_input_set(irq_pin.port, irq_pin.pin, 1);
            k_sleep(K_MSEC(1));
            gpio_emul_input_set(irq_pin.port, irq_pin.pin, 0);
        }
    };

    int i = 0;
    printf("i%d", i);

    constexpr int estimate_debounce_time_ms = 5;
    debounce_sim(estimate_debounce_time_ms);

    zassert_equal(atomic_get(&cb_call_count), 1);
}

void my_suite_before(void *fixture)
{
    ARG_UNUSED(fixture);
    static struct gpio_dt_spec irq_pin = GPIO_DT_SPEC_GET(DT_INST(0, test_button), button_gpios);
    gpio_pin_configure_dt(&irq_pin, GPIO_INPUT); 
    gpio_emul_input_set(irq_pin.port, irq_pin.pin, 0);
}

ZTEST_SUITE(button, nullptr, nullptr, my_suite_before, nullptr, nullptr); // NOLINT


} // namespace