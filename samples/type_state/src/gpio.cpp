#include "gpio.hpp"

tl::optional<bool> Gpio<Input>::get_state()
{
    if (!_has_been_configured)
    {
        return gpio_pin_get_dt(_spec) == 1;
    }
    return tl::nullopt;
}

tl::optional<Gpio<Ouput>> Gpio<Input>::configure_as_output()
{
    if (!_has_been_configured)
    {
        _has_been_configured = true;
        return Gpio<Ouput>(_spec);
    }
    return tl::nullopt;
}

Gpio<Input>::Gpio(const gpio_dt_spec *spec) : _spec{spec}
{
    gpio_pin_configure_dt(_spec, GPIO_INPUT);
}

/* ------------------------------------------------------ */

Gpio<Ouput>::Gpio(const gpio_dt_spec *spec) : _spec{spec}
{
    gpio_pin_configure_dt(_spec, GPIO_OUTPUT);
}

tl::optional<int> Gpio<Ouput>::set_state(bool state)
{
    if (!_has_been_configured)
    {
        return gpio_pin_set_dt(_spec, static_cast<int>(state));
    }
    return tl::nullopt;
}

tl::optional<Gpio<Input>> Gpio<Ouput>::configure_as_input()
{
    if (!_has_been_configured)
    {
        _has_been_configured = true;
        return Gpio<Input>(_spec);
    }
    return tl::nullopt;
}

/* ----------------------------------------------------------- */


Gpio<Unconfigured>::Gpio(const gpio_dt_spec *spec) : _spec{spec}
{
    gpio_is_ready_dt(_spec);
    gpio_pin_configure_dt(_spec, GPIO_DISCONNECTED);
}

tl::optional<Gpio<Input>> Gpio<Unconfigured>::configure_as_input()
{
    if (!_has_been_configured)
    {
        _has_been_configured = true;
        return Gpio<Input>(_spec);
    }
    return tl::nullopt;
}

tl::optional<Gpio<Ouput>> Gpio<Unconfigured>::configure_as_output()
{
    if (!_has_been_configured)
    {
        _has_been_configured = true;
        return Gpio<Ouput>(_spec);
    }
    return tl::nullopt;
}
