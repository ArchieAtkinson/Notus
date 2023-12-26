#include <stdexcept>

#include <zephyr/kernel.h>

#include <button.hpp>

namespace {

void zephyr_callback(const struct device *port, struct gpio_callback *callback, gpio_port_pins_t pins)
{
    ARG_UNUSED(port);
    ARG_UNUSED(pins);

    auto *data = CONTAINER_OF(callback, ParentData, cb_data); // NOLINT
    
    gpio_remove_callback_dt(data->spec, &data->cb_data);
    gpio_pin_interrupt_configure_dt(data->spec, GPIO_INT_DISABLE); // NOLINT(hicpp-signed-bitwise)
    gpio_pin_configure_dt(data->spec, GPIO_INPUT); // NOLINT(hicpp-signed-bitwise)
    k_timer_start(&data->timer, K_MSEC(5), K_NO_WAIT);
}

void timer_expiry_func(struct k_timer *timer)
{
    auto *data = CONTAINER_OF(timer, ParentData, timer); //NOLINT

    if (gpio_pin_get_dt(data->spec) == 1)
    {
        data->callback();
    }
    gpio_pin_interrupt_configure_dt(data->spec, GPIO_INT_EDGE_RISING); // NOLINT(hicpp-signed-bitwise)
}

}

Button::Button(gpio_dt_spec *spec) : _spec{spec}
{
    if (!gpio_is_ready_dt(_spec))
    {
        throw std::runtime_error("Here");
    }

    if (gpio_pin_configure_dt(_spec, GPIO_INPUT) != 0) // NOLINT(hicpp-signed-bitwise)
    {
        throw std::runtime_error("Here");
    }

    if (gpio_pin_interrupt_configure_dt(_spec, GPIO_INT_EDGE_RISING) != 0) // NOLINT(hicpp-signed-bitwise)
    {
        throw std::runtime_error("Here");
    }
}

auto Button::add_callback(std::function<void(void)> callback) -> void
{
    ParentData &data = _parent_data.at(_index_free);
    data.callback = callback;
    data.spec = _spec;
    gpio_init_callback(&data.cb_data, zephyr_callback, BIT(_spec->pin));
    gpio_add_callback_dt(_spec, &data.cb_data);
    k_timer_init(&data.timer, timer_expiry_func, nullptr);
    _index_free++;   
}

