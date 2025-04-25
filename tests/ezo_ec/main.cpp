#include <zephyr/drivers/gpio/gpio_emul.h>
#include <zephyr/drivers/sensor.h>

#include "drivers/sensor/ezo_ec/emul_ezo_ec.h"
#include "drivers/sensor/ezo_ec/ezo_ec_util.h"

#include "testing.hpp"

namespace {

const struct device *ezo = DEVICE_DT_GET(DT_NODELABEL(ezo_ec)); // NOLINT
const static struct emul *emul = nullptr; // NOLINT 

// static uint8_t get_emul_reg(uint8_t address)
// {
//     return emul_ezo_ec_get_reg(emul, address);
// }

// [[maybe_unused]] static void set_emul_reg(uint8_t address, uint8_t reg_value)
// {
//     emul_ezo_ec_set_reg(emul, address, reg_value);
// }

void reset_emul_registers()
{
    emul_ezo_ec_reset_registers(emul);
}

ezo_ec_config get_emul_config()
{
    return emul_ezo_ec_get_config(emul);
}

void re_init_pcf85063a(struct ezo_ec_config config)
{
    emul_ezo_ec_set_config(emul, config);

    // This is a bit hacky... but it works!
    // Calls the init functions and updates the state used
    // by `device_is_ready()`. Code taken from Zephyr's init.c
    int ret = ezo_ec_init(emul->dev);
    if (ret != 0)
    {
        if (ret < 0)
        {
            ret = -ret;
        }
        if (ret > UINT8_MAX)
        {
            ret = UINT8_MAX;
        }
        emul->dev->state->init_res = ret;
    }
    else
    {
        emul->dev->state->init_res = 0;
    }
}

void* setup()
{
    emul = emul_get_binding("ezoec@0");

    struct ezo_ec_config config = get_emul_config();
    re_init_pcf85063a(config);

    zassert_true(device_is_ready(ezo), "Device is not ready");
    return nullptr;
}

void teardown(void * /* f */)
{
    reset_emul_registers();
}


ZTEST(t_ezo_ec, test_fetch_and_get)
{

    zassert_true(true);
}

ZTEST_SUITE(t_ezo_ec, NULL, setup, NULL, NULL, teardown); // NOLINT

}
