
#include <zephyr/drivers/gpio.h>
#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio/gpio_emul.h>

#include "logging.hpp"

#include "gpio.hpp"

LOG_MODULE_REGISTER(main);

int main()
{
    gpio_dt_spec spec = GPIO_DT_SPEC_GET(DT_INST(0, test_button), button_gpios);
    auto         gpio = Gpio<Unconfigured>(&spec);

    auto igpio = gpio.configure_as_input();
    if (!igpio)
    {
        LOG_ERR("Not Present %d", __LINE__);
        return 0;
    }

    auto gpio_state = igpio->get_state();
    if (!gpio_state)
    {
        LOG_ERR("Not Present %d", __LINE__);
        return 0;
    }

    auto ogpio = igpio->configure_as_output();
    if (!ogpio)
    {
        LOG_ERR("Not Present %d", __LINE__);
        return 0;
    }

    auto set_ret = ogpio->set_state(true);

    LOG_INF("%d %d", static_cast<int>(gpio_state.value()), set_ret);
}