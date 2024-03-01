#include <cassert>
#include <chrono>

#include <zephyr/drivers/gpio.h>

#include "errors.hpp"
#include "logging.hpp"
#include "button.hpp"
#include "uptime_tp.hpp"

LOG_MODULE_REGISTER(button, LOG_LEVEL_DBG);

void timer_expiry_func(struct k_timer *timer)
{
    LOG_DBG("Enter");

    auto *self = static_cast<Button *>(k_timer_user_data_get(timer));

    gpio_pin_interrupt_configure_dt(self->_spec, GPIO_INT_EDGE_BOTH);
    gpio_add_callback_dt(self->_spec, &self->_cb_data.cb_data);

    if (self->_on_press_callback)
    {
        LOG_DBG("Calling on_press_callback"); 
        self->_on_press_callback();
    }

    LOG_DBG("Exit"); 
}

void zephyr_callback(const struct device *port, struct gpio_callback *callback, gpio_port_pins_t pins)
{
    ARG_UNUSED(port);
    ARG_UNUSED(pins);
    ARG_UNUSED(callback);

    LOG_DBG("Enter");


    auto *data = CONTAINER_OF(callback, Button::CallbackContainer, cb_data); // NOLINT
    Button *self = data->self;

    self->gpio_handler();

    LOG_DBG("Exit"); 
}

void Button::gpio_handler()
{
    LOG_DBG("Enter"); 
    if (k_timer_status_get(&_debounce_timer) == 0)
    {
        LOG_DBG("Handle Debounce"); 
        gpio_pin_interrupt_configure_dt(_spec, GPIO_INT_DISABLE);
        gpio_remove_callback_dt(_spec, &_cb_data.cb_data);
        gpio_pin_configure_dt(_spec, GPIO_INPUT);
        k_timer_start(&_debounce_timer, _debounce_time, K_NO_WAIT);
        _start_tp = UpTime::now();
    }
    LOG_DBG("Exit"); 
}


Button::Button(gpio_dt_spec *spec, k_timeout_t debounce_time)
    : _spec{spec},
      _debounce_time{debounce_time}
{
    int ret = 0;
    assert(spec != nullptr);

    if (!gpio_is_ready_dt(_spec))
    {
        throw MajorError(ButtonError::gpio_is_ready, 0);
    }

    ret = gpio_pin_configure_dt(_spec, GPIO_INPUT);
    if (ret != 0) 
    {
        throw MajorError(ButtonError::pin_configuration, ret);
    }

    ret = gpio_pin_interrupt_configure_dt(_spec, GPIO_INT_EDGE_BOTH);
    if (ret != 0) 
    {
        throw MajorError(ButtonError::interrupt_configuration, ret);
    }

    k_timer_init(&_debounce_timer, timer_expiry_func, nullptr);
    k_timer_user_data_set(&_debounce_timer, static_cast<void *>(this));

    _cb_data.self   = this;

    gpio_init_callback(&_cb_data.cb_data, zephyr_callback, BIT(_spec->pin));
    ret = gpio_add_callback_dt(_spec, &_cb_data.cb_data);
    if (ret != 0) 
    {
        throw MajorError(ButtonError::add_callback, ret);
    }

}

auto Button::add_on_press_callback(std::function<void(void)> callback) -> void
{
    _on_press_callback = callback;
}

Button::~Button()
{
    gpio_pin_interrupt_configure_dt(_spec, GPIO_INT_MODE_DISABLED); 
    gpio_remove_callback_dt(_spec, &_cb_data.cb_data);
}