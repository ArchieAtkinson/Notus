#include <cassert>
#include <chrono>

#include <zephyr/drivers/gpio.h>
#include <zephyr/logging/log.h>

#include "button.hpp"
#include "uptime_tp.hpp"

LOG_MODULE_REGISTER(button, LOG_LEVEL_DBG); // NOLINT


void timer_expiry_func(struct k_timer *timer)
{
    LOG_DBG("Enter"); 

    auto *self = static_cast<Button *>(k_timer_user_data_get(timer));

    if (self->_on_press_callback)
    {
        LOG_DBG("Calling on_press_callback"); 
        self->_on_press_callback();
    }

    if (self->_on_release_callback)
    {
        gpio_pin_interrupt_configure_dt(self->_spec, GPIO_INT_EDGE_FALLING); 
        gpio_add_callback_dt(self->_spec, &self->_falling_cb_data.cb_data);
    }

    LOG_DBG("Exit"); 
}

void zephyr_callback_rising(const struct device *port, struct gpio_callback *callback, gpio_port_pins_t pins)
{
    ARG_UNUSED(port);
    ARG_UNUSED(pins);
    LOG_DBG("Enter"); 

    auto *data = CONTAINER_OF(callback, Button::CallbackContainer, cb_data); // NOLINT
    Button *self = data->self;

    gpio_remove_callback_dt(self->_spec, &data->cb_data);
    gpio_pin_interrupt_configure_dt(self->_spec, GPIO_INT_DISABLE); 
    gpio_pin_configure_dt(self->_spec, GPIO_INPUT); 
    k_timer_start(&self->_debounce_timer, self->_debounce_time, K_NO_WAIT);

    self->_start_tp = UpTime::now();

    LOG_DBG("Exit"); 
}


void zephyr_callback_falling(const struct device *port, struct gpio_callback *callback, gpio_port_pins_t pins)
{
    ARG_UNUSED(port);
    ARG_UNUSED(pins);
    LOG_DBG("Enter"); 

    auto *data = CONTAINER_OF(callback, Button::CallbackContainer, cb_data); // NOLINT
    Button *self = data->self;

    std::chrono::milliseconds time_since_pressed = UpTime::now() - self->_start_tp;

    if (self->_on_release_callback && time_since_pressed >= self->_on_release_duration)
    {
        LOG_DBG("Calling on_release_callback"); 
        self->_on_release_callback();
    }
    LOG_DBG("Exit"); 
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

    if (gpio_pin_configure_dt(_spec, GPIO_INPUT) != 0) 
    {
        throw ButtonError();
    }

    if (gpio_pin_interrupt_configure_dt(_spec, GPIO_INT_EDGE_RISING) != 0) 
    {
        throw ButtonError();
    }

    k_timer_init(&_debounce_timer, timer_expiry_func, nullptr);
    k_timer_user_data_set(&_debounce_timer, static_cast<void *>(this));

    _rising_cb_data.self   = this;
    _falling_cb_data.self = this;

    gpio_init_callback(&_rising_cb_data.cb_data, zephyr_callback_rising, BIT(_spec->pin));
    gpio_add_callback_dt(_spec, &_rising_cb_data.cb_data);

    gpio_init_callback(&_falling_cb_data.cb_data, zephyr_callback_falling, BIT(_spec->pin));

}

auto Button::add_on_press_callback(std::function<void(void)> callback) -> void
{
    _on_press_callback = callback;
}

auto Button::add_on_release_callback(std::function<void(void)> callback, std::chrono::milliseconds press_duration) -> void
{
    _on_release_callback = callback;
    _on_release_duration = press_duration;
}

Button::~Button()
{
    gpio_pin_interrupt_configure_dt(_spec, GPIO_INT_MODE_DISABLED); 
    gpio_remove_callback_dt(_spec, &_rising_cb_data.cb_data);
    gpio_remove_callback_dt(_spec,  &_falling_cb_data.cb_data);
}