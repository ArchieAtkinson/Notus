#pragma once

#include <functional>

#include "zephyr/drivers/gpio.h"
#include "zephyr/kernel.h"


class IButton
{
  public:
    virtual ~IButton() = default;
    virtual auto add_callback(std::function<void(void)> callback) -> void = 0;
};

struct ParentData
{
    std::function<void(void)> callback;
    gpio_callback cb_data;
    gpio_dt_spec *spec;
    k_timer timer;
};

class Button : public IButton
{
  public:
    Button(gpio_dt_spec *spec);
    auto add_callback(std::function<void(void)> callback) -> void final;

  private:
    gpio_dt_spec *_spec;
    std::array<ParentData, 5> _parent_data;
    size_t _index_free = 0;
};
