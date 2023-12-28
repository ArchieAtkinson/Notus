#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/gpio/gpio_emul.h>
#include <zephyr/ztest.h>
#include <zephyr/kernel.h>

#include "button.hpp"
#include "zephyr/sys_clock.h"


ZTEST(button, test_constructor_throw) // NOLINT
{
    static struct gpio_dt_spec irq_pin = GPIO_DT_SPEC_GET(DT_INST(0, test_button), button_gpios);
    irq_pin.port->state->initialized  = false;
    
    bool exception_caught = false;
    try {
        Button button(&irq_pin, K_MSEC(5));
    }
    catch (const ButtonError &e)
    {
        exception_caught = true;
    }
    catch(...)
    {
        zassert_unreachable(); // NOLINT    
    }

    zassert_true(exception_caught); // NOLINT    
}


ZTEST(button, test_single_press_and_debounce) // NOLINT PP
{
    static struct gpio_dt_spec irq_pin = GPIO_DT_SPEC_GET(DT_INST(0, test_button), button_gpios);

    atomic_t cb_call_count = ATOMIC_INIT(0);
    auto cb_test2  = [&]()
    {
        atomic_inc(&cb_call_count);
    };
    const k_timeout_t debounce_time_ms = K_MSEC(5);
    Button            button(&irq_pin, debounce_time_ms);
    
    button.add_single_press_callback(cb_test2);
    
    gpio_emul_input_set(irq_pin.port, irq_pin.pin, 1);
    k_sleep(K_USEC(10));
    
    gpio_emul_input_set(irq_pin.port, irq_pin.pin, 0);
    k_sleep(K_USEC(10)); 
    gpio_emul_input_set(irq_pin.port, irq_pin.pin, 1);
    k_sleep(K_USEC(10));
    gpio_emul_input_set(irq_pin.port, irq_pin.pin, 0);
    k_sleep(K_USEC(10));
    gpio_emul_input_set(irq_pin.port, irq_pin.pin, 1);
    k_sleep(K_USEC(10));
    gpio_emul_input_set(irq_pin.port, irq_pin.pin, 0);
    k_sleep(K_USEC(10));
    gpio_emul_input_set(irq_pin.port, irq_pin.pin, 1);
    k_sleep(debounce_time_ms);
    gpio_emul_input_set(irq_pin.port, irq_pin.pin, 0);
    
    zassert_equal(atomic_get(&cb_call_count), 1); // NOLINT    
}
ZTEST_SUITE(button, NULL, NULL, NULL, NULL, NULL); // NOLINT
