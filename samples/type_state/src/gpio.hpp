#pragma once

#include <tl/optional.hpp>

#include <zephyr/drivers/gpio.h>

struct Input
{
};
struct Ouput
{
};
struct Unconfigured
{
};

template <typename TState = Unconfigured>
class Gpio;

template <>
class Gpio<Input>
{
  public:
    friend class Gpio<Unconfigured>;
    friend class Gpio<Ouput>;

    Gpio() = delete;

    [[nodiscard]] tl::optional<bool> get_state();
    [[nodiscard]] tl::optional<Gpio<Ouput>> configure_as_output();
    
  private:
    explicit Gpio(const gpio_dt_spec *spec);
    bool _has_been_configured = false;
    const gpio_dt_spec *_spec;
};

template <> class Gpio<Ouput>
{
  public:
    friend class Gpio<Unconfigured>;
    friend class Gpio<Input>;

    Gpio() = delete;

    [[nodiscard]] tl::optional<int> set_state(bool state);
    [[nodiscard]] tl::optional<Gpio<Input>> configure_as_input();

  private:
    explicit Gpio(const gpio_dt_spec *spec);
    bool _has_been_configured = false;
    const gpio_dt_spec *_spec;
};

template <> class Gpio<Unconfigured>
{
  public:
    Gpio() = delete;

    explicit Gpio(const gpio_dt_spec *spec);
    [[nodiscard]] tl::optional<Gpio<Input>> configure_as_input();
    [[nodiscard]] tl::optional<Gpio<Ouput>> configure_as_output();

  private:
    bool                _has_been_configured = false;
    const gpio_dt_spec *_spec;
};
