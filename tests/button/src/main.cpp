#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/gpio/gpio_emul.h>
#include <zephyr/ztest.h>
#include <zephyr/kernel.h>

#include "button.hpp"

ZTEST(button, test_debounce) // NOLINT
{
    static struct gpio_dt_spec irq_pin = GPIO_DT_SPEC_GET(DT_INST(0, test_button), button_gpios);

    atomic_t cb_call_count = ATOMIC_INIT(0);
    auto cb_test2  = [&]()
    {
        atomic_inc(&cb_call_count);
    };

    Button button(&irq_pin);
    button.add_callback(cb_test2);
    
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
    constexpr int debounce_time_ms = 5;
    k_sleep(K_MSEC(debounce_time_ms + 1));

    printk("cb_call_count: %d\n", atomic_get(&cb_call_count)); // NOLINT
    zassert_true(atomic_get(&cb_call_count) == 1); // NOLINT
    
}
ZTEST_SUITE(button, NULL, NULL, NULL, NULL, NULL); // NOLINT
