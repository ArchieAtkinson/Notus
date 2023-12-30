#pragma once

#include <functional>

#include "zephyr/drivers/gpio.h"
#include "zephyr/kernel.h"
#include "zephyr/sys_clock.h"

#include "uptime_tp.hpp"

class IButton
{
  public:
    IButton() = default;
    virtual ~IButton() = default;
    virtual auto add_on_press_callback(std::function<void(void)> callback) -> void = 0;
    virtual auto add_on_release_callback(std::function<void(void)> callback, std::chrono::milliseconds press_duration) -> void = 0;

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
    auto add_on_press_callback(std::function<void(void)> callback) -> void final;
    auto add_on_release_callback(std::function<void(void)> callback, std::chrono::milliseconds press_duration) -> void final;

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

    friend void zephyr_callback_rising(const struct device *port, struct gpio_callback *callback,
                                       gpio_port_pins_t pins);
    friend void zephyr_callback_falling(const struct device *port, struct gpio_callback *callback, gpio_port_pins_t pins);
    friend void timer_expiry_func(struct k_timer *timer);
    
  private:
    gpio_dt_spec *_spec;
    CallbackContainer _rising_cb_data;
    CallbackContainer _falling_cb_data;
    k_timeout_t       _debounce_time;

    std::chrono::time_point<UpTime> _start_tp;
    std::chrono::milliseconds _on_release_duration{0};

    k_timer _debounce_timer{};
    std::function<void(void)> _on_press_callback;
    std::function<void(void)> _on_release_callback;
};

