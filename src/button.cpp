#include "button.hpp"
#include <cassert>

void zephyr_callback(const struct device *port, struct gpio_callback *callback, gpio_port_pins_t pins)
{
    ARG_UNUSED(port);
    ARG_UNUSED(pins);

    auto *data = CONTAINER_OF(callback, Button::ParentData, cb_data); // NOLINT
    Button *self = data->self;

    gpio_remove_callback_dt(self->_spec, &data->cb_data);
    gpio_pin_interrupt_configure_dt(self->_spec, GPIO_INT_DISABLE); // NOLINT(hicpp-signed-bitwise)
    gpio_pin_configure_dt(self->_spec, GPIO_INPUT); // NOLINT(hicpp-signed-bitwise)
    k_timer_start(&self->_timer, K_MSEC(5), K_NO_WAIT);
}


void timer_expiry_func(struct k_timer *timer)
{
    auto *self = static_cast<Button*>(k_timer_user_data_get(timer)); 

    if (gpio_pin_get_dt(self->_spec) == 1)
    {
        self->_callback();
    }
    gpio_pin_interrupt_configure_dt(self->_spec, GPIO_INT_EDGE_RISING); // NOLINT(hicpp-signed-bitwise)
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

}

auto Button::add_callback(std::function<void(void)> callback) -> void
{
    _callback = callback;
    _parent_data.self = this;

    gpio_init_callback(&_parent_data.cb_data, zephyr_callback, BIT(_spec->pin));
    gpio_add_callback_dt(_spec, &_parent_data.cb_data);
    k_timer_init(&_timer, timer_expiry_func, nullptr);
    k_timer_user_data_set(&_timer, static_cast<void*>(this));
}

Button::~Button()
{
    gpio_pin_interrupt_configure_dt(_spec, GPIO_INT_DISABLE); // NOLINT(hicpp-signed-bitwise)
}