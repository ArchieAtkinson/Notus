#include "button.hpp"
#include "uptime_tp.hpp"
#include "zephyr/drivers/gpio.h"
#include <cassert>
#include <chrono>


void timer_expiry_func(struct k_timer *timer)
{
    auto *self = static_cast<Button *>(k_timer_user_data_get(timer));
    gpio_pin_interrupt_configure_dt(self->_spec, GPIO_INT_EDGE_FALLING); // NOLINT(hicpp-signed-bitwise)
    gpio_add_callback_dt(self->_spec, &self->_falling_cb_data.cb_data);
}

void zephyr_callback_rising(const struct device *port, struct gpio_callback *callback, gpio_port_pins_t pins)
{
    ARG_UNUSED(port);
    ARG_UNUSED(pins);

    auto *data = CONTAINER_OF(callback, Button::CallbackContainer, cb_data); // NOLINT
    Button *self = data->self;

    gpio_remove_callback_dt(self->_spec, &data->cb_data);
    gpio_pin_interrupt_configure_dt(self->_spec, GPIO_INT_DISABLE); // NOLINT(hicpp-signed-bitwise)
    gpio_pin_configure_dt(self->_spec, GPIO_INPUT); // NOLINT(hicpp-signed-bitwise)
    k_timer_start(&self->_debounce_timer, self->_debounce_time, K_NO_WAIT);
    self->_start_tp = UpTime::now();
}


void zephyr_callback_falling(const struct device *port, struct gpio_callback *callback, gpio_port_pins_t pins)
{
    ARG_UNUSED(port);
    ARG_UNUSED(pins);

    auto *data = CONTAINER_OF(callback, Button::CallbackContainer, cb_data); // NOLINT
    Button *self = data->self;

    if (!self->_long_press_callback || UpTime::now() - self->_start_tp < self->_long_press_duration)
    {
        self->_single_press_callback();
    }
    else
    {
        self->_long_press_callback();
    }
}

Button::Button(gpio_dt_spec *spec, k_timeout_t debounce_time)
    : _spec{spec},
      _debounce_time{debounce_time}
{

    assert(spec != nullptr);

    if (!gpio_is_ready_dt(_spec))
    {
        throw ButtonError();
    }

    if (gpio_pin_configure_dt(_spec, GPIO_INPUT) != 0) // NOLINT(hicpp-signed-bitwise)
    {
        throw ButtonError();
    }

    if (gpio_pin_interrupt_configure_dt(_spec, GPIO_INT_EDGE_RISING) != 0) // NOLINT(hicpp-signed-bitwise)
    {
        throw ButtonError();
    }

    k_timer_init(&_debounce_timer, timer_expiry_func, nullptr);
    k_timer_user_data_set(&_debounce_timer, static_cast<void *>(this));

    k_timer_init(&_press_timer, nullptr, nullptr);
}

auto Button::add_single_press_callback(std::function<void(void)> callback) -> void
{
    _single_press_callback = callback;
    _rising_cb_data.self   = this;
    
    _falling_cb_data.self = this;

    gpio_init_callback(&_rising_cb_data.cb_data, zephyr_callback_rising, BIT(_spec->pin));
    gpio_add_callback_dt(_spec, &_rising_cb_data.cb_data);

    gpio_init_callback(&_falling_cb_data.cb_data, zephyr_callback_falling, BIT(_spec->pin));
}

Button::~Button()
{
    gpio_pin_interrupt_configure_dt(_spec, GPIO_INT_DISABLE); // NOLINT(hicpp-signed-bitwise)
}