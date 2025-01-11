#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>

#include "mock.h"

#define DT_DRV_COMPAT zephyr_mock_gpio

// NOLINTBEGIN
DEFINE_FAKE_VALUE_FUNC(int, pin_configure_mock, const struct device *, gpio_pin_t, gpio_flags_t);
DEFINE_FAKE_VALUE_FUNC(int, port_get_raw_mock, const struct device *, gpio_port_value_t *);
DEFINE_FAKE_VALUE_FUNC(int, port_set_masked_raw_mock, const struct device *, gpio_port_pins_t, gpio_port_value_t);
DEFINE_FAKE_VALUE_FUNC(int, port_set_bits_raw_mock, const struct device *, gpio_port_pins_t);
DEFINE_FAKE_VALUE_FUNC(int, port_clear_bits_raw_mock, const struct device *, gpio_port_pins_t);
DEFINE_FAKE_VALUE_FUNC(int, port_toggle_bits_mock, const struct device *, gpio_port_pins_t);
DEFINE_FAKE_VALUE_FUNC(int,
                       pin_interrupt_configure_mock,
                       const struct device *,
                       gpio_pin_t,
                       enum gpio_int_mode,
                       enum gpio_int_trig);
DEFINE_FAKE_VALUE_FUNC(int, manage_callback_mock, const struct device *, struct gpio_callback *, bool);
DEFINE_FAKE_VALUE_FUNC(uint32_t, get_pending_int_mock, const struct device *);
// NOLINTEND

static const struct gpio_driver_api mock_gpio_api = {
    .pin_configure           = pin_configure_mock,
    .port_get_raw            = port_get_raw_mock,
    .port_set_masked_raw     = port_set_masked_raw_mock,
    .port_set_bits_raw       = port_set_bits_raw_mock,
    .port_clear_bits_raw     = port_clear_bits_raw_mock,
    .port_toggle_bits        = port_toggle_bits_mock,
    .pin_interrupt_configure = pin_interrupt_configure_mock,
    .manage_callback         = manage_callback_mock,
    .get_pending_int         = get_pending_int_mock,
};

// NOLINTBEGIN
struct gpio_driver_config config = {
    .port_pin_mask = 0xFF,

};

struct gpio_driver_data data = {
    .invert = 0x00,
};

DEVICE_DT_INST_DEFINE(0, NULL, NULL, &data, &config, POST_KERNEL, CONFIG_GPIO_INIT_PRIORITY, &mock_gpio_api);
// NOLINTEND