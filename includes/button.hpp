#pragma once

#include <functional>
#include <system_error>

#include "zephyr/drivers/gpio.h"
#include "zephyr/kernel.h"
#include "zephyr/sys_clock.h"

#include "uptime_tp.hpp"

enum class ButtonError
{
    init = 1,
    pin_configuration,
    interrupt_configuration,
    add_callback,
};

namespace std
{
  template <>
    struct is_error_code_enum<ButtonError> : true_type {};
}

std::error_code make_error_code(ButtonError);

class IButton
{
  public:
    IButton() = default;
    virtual ~IButton() = default;
    virtual auto add_on_press_callback(std::function<void(void)> callback) -> void = 0;

    IButton(const IButton &other) = delete;
    IButton(IButton&& other) noexcept = delete;
    IButton& operator=(const IButton &other) = delete;
    IButton& operator=(IButton&& other) noexcept = delete;
    
};

class Button final : public IButton
{
  public:
    Button() = delete;
    explicit Button(gpio_dt_spec *spec, k_timeout_t debounce_time);
    auto add_on_press_callback(std::function<void(void)> callback) -> void final;

    Button(const Button &other) = delete;
    Button(Button&& other) noexcept = delete;
    Button& operator=(const Button &other) = delete;
    Button &operator=(Button &&other) noexcept = delete;

    ~Button() final;
        
    struct CallbackContainer
    {
        gpio_callback cb_data{};
        Button *self{};
    };

    friend void zephyr_callback(const struct device *port, struct gpio_callback *callback,
                                       gpio_port_pins_t pins);
    friend void timer_expiry_func(struct k_timer *timer);

  private:
    auto gpio_handler() -> void;
    gpio_dt_spec *_spec;
    CallbackContainer _cb_data;
    k_timeout_t       _debounce_time;

    std::chrono::time_point<UpTime> _start_tp;

    k_timer _debounce_timer{};
    std::function<void(void)> _on_press_callback;

    int _press_count = 0;
};

