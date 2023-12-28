#pragma once

#include <functional>
#include <stdexcept>


#include "zephyr/drivers/gpio.h"
#include "zephyr/kernel.h"

template<typename UserData>
class IButton
{
  public:
    virtual ~IButton() = default;
    virtual auto add_callback(std::function<void(UserData*)> callback) -> void = 0;
};


template<typename UserData>
class Button final : public IButton<UserData>
{
  public:
    Button() = delete;
    explicit Button(gpio_dt_spec *spec, UserData * user_data);
    auto add_callback(std::function<void(UserData*)> callback) -> void final;
    ~Button() final;

  struct ParentData
  {
      std::function<void(UserData*)> callback;
      gpio_callback cb_data{};
      gpio_dt_spec *spec{};
      k_timer timer{};
      UserData * user_data;
  };
    
  private:
    gpio_dt_spec *_spec;
    ParentData    _parent_data;
};

template<typename UserData>
void zephyr_callback(const struct device *port, struct gpio_callback *callback, gpio_port_pins_t pins)
{
    ARG_UNUSED(port);
    ARG_UNUSED(pins);

    auto *data = CONTAINER_OF(callback, typename Button<UserData>::ParentData, cb_data); // NOLINT
    
    gpio_remove_callback_dt(data->spec, &data->cb_data);
    gpio_pin_interrupt_configure_dt(data->spec, GPIO_INT_DISABLE); // NOLINT(hicpp-signed-bitwise)
    gpio_pin_configure_dt(data->spec, GPIO_INPUT); // NOLINT(hicpp-signed-bitwise)
    k_timer_start(&data->timer, K_MSEC(5), K_NO_WAIT);
}

template<typename UserData>
void timer_expiry_func(struct k_timer *timer)
{
    auto *data = CONTAINER_OF(timer, typename Button<UserData>::ParentData, timer); //NOLINT

    if (gpio_pin_get_dt(data->spec) == 1)
    {
        data->callback(data->user_data);
    }
    gpio_pin_interrupt_configure_dt(data->spec, GPIO_INT_EDGE_RISING); // NOLINT(hicpp-signed-bitwise)
}

template<typename UserData>
Button<UserData>::Button(gpio_dt_spec *spec, UserData * user_data) : _spec{spec}
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

    _parent_data.user_data = user_data;
}

template<typename UserData>
auto Button<UserData>::add_callback(std::function<void(UserData*)> callback) -> void
{
    _parent_data.callback = callback;
    _parent_data.spec = _spec;
    gpio_init_callback(&_parent_data.cb_data, zephyr_callback<UserData>, BIT(_spec->pin));
    gpio_add_callback_dt(_spec, &_parent_data.cb_data);
    k_timer_init(&_parent_data.timer, timer_expiry_func<UserData>, nullptr);
}

template<typename UserData>
Button<UserData>::~Button()
{
    gpio_pin_interrupt_configure_dt(_spec, GPIO_INT_DISABLE); // NOLINT(hicpp-signed-bitwise)
}