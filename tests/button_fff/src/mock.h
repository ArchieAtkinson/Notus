#pragma once

#include <zephyr/drivers/gpio.h>
#include <zephyr/fff.h>

#ifdef __cplusplus
extern "C"
{
#endif

    // NOLINTBEGIN(cppcoreguidelines-avoid-non-const-global-variables)
    DECLARE_FAKE_VALUE_FUNC(int, pin_configure_mock, const struct device *, gpio_pin_t, gpio_flags_t);
    DECLARE_FAKE_VALUE_FUNC(int, port_get_raw_mock, const struct device *, gpio_port_value_t *);
    DECLARE_FAKE_VALUE_FUNC(int, port_set_masked_raw_mock, const struct device *, gpio_port_pins_t, gpio_port_value_t);
    DECLARE_FAKE_VALUE_FUNC(int, port_set_bits_raw_mock, const struct device *, gpio_port_pins_t);
    DECLARE_FAKE_VALUE_FUNC(int, port_clear_bits_raw_mock, const struct device *, gpio_port_pins_t);
    DECLARE_FAKE_VALUE_FUNC(int, port_toggle_bits_mock, const struct device *, gpio_port_pins_t);
    DECLARE_FAKE_VALUE_FUNC(int,
                            pin_interrupt_configure_mock,
                            const struct device *,
                            gpio_pin_t,
                            enum gpio_int_mode,
                            enum gpio_int_trig);
    DECLARE_FAKE_VALUE_FUNC(int, manage_callback_mock, const struct device *, struct gpio_callback *, bool);
    DECLARE_FAKE_VALUE_FUNC(uint32_t, get_pending_int_mock, const struct device *);
    // NOLINTEND(cppcoreguidelines-avoid-non-const-global-variables)

#ifdef __cplusplus
}
#endif
