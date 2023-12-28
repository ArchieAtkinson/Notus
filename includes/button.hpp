#pragma once

#include <functional>

#include "zephyr/drivers/gpio.h"
#include "zephyr/kernel.h"
#include "zephyr/sys_clock.h"

class IButton
{
  public:
    IButton() = default;
    virtual ~IButton() = default;
    virtual auto add_callback(std::function<void(void)> callback) -> void = 0;

    IButton(const IButton &other) = delete;
    IButton(IButton&& other) noexcept = delete;
    IButton& operator=(const IButton &other) = delete;
    IButton& operator=(IButton&& other) noexcept = delete;
    
};

struct ButtonError : public std::exception
{
	[[nodiscard]] const char * what () const noexcept final
    {
    	return "Button Error";
    }
};


class Button final : public IButton
{
  public:
    Button() = delete;
    explicit Button(gpio_dt_spec *spec, k_timeout_t debounce_time);
    auto add_callback(std::function<void(void)> callback) -> void final;

    Button(const Button &other) = delete;
    Button(Button&& other) noexcept = delete;
    Button& operator=(const Button &other) = delete;
    Button &operator=(Button &&other) noexcept = delete;

    
    ~Button() final;
        
    struct ParentData
    {
        gpio_callback cb_data{};
        Button *self{};
    };

    friend void zephyr_callback(const struct device *port, struct gpio_callback *callback, gpio_port_pins_t pins);
    friend void timer_expiry_func(struct k_timer *timer);
    
  private:
    gpio_dt_spec *_spec;
    ParentData    _parent_data;
    k_timeout_t   _debounce_time;
    std::function<void(void)> _callback;
    gpio_callback _cb_data{};
    k_timer _timer{};
};

