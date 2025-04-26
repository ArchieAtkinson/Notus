#include <cstring>
#include <string_view>
#include <zephyr/drivers/gpio/gpio_emul.h>
#include <zephyr/drivers/sensor.h>

#include "drivers/sensor/ezo_ec/emul_ezo_ec.h"
#include "drivers/sensor/ezo_ec/ezo_ec_util.h"
#include "drivers/sensors/ezo_ec.h"

#include "testing.hpp"

namespace {

const struct device *ezo = DEVICE_DT_GET(DT_NODELABEL(ezo_ec)); // NOLINT
const static struct emul *emul = nullptr; // NOLINT 

void set_output(const char *buf, int len)
{
    emul_ezo_ec_set_output(emul,buf, len);
}

// void get_input(char *buf, int len)
// {
//     emul_ezo_ec_get_input(emul,buf,len);
// }

void reset_emul()
{
    emul_ezo_ec_reset_buffers(emul);
}

ezo_ec_config get_emul_config()
{
    return emul_ezo_ec_get_config(emul);
}

void re_init(struct ezo_ec_config config)
{
    emul_ezo_ec_set_config(emul, config);

    std::string_view info = "?i,EC,2.16\0";
    set_output(info.data(), info.length()); // NOLINT

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
    emul = emul_get_binding("ezoec@64");

    struct ezo_ec_config config = get_emul_config();
    re_init(config);

    zassert_true(device_is_ready(ezo), "Device is not ready");
    return nullptr;
}

void teardown(void * /* f */)
{
    reset_emul();
}

ZTEST(t_ezo_ec, test_fetch_and_get)
{
    std::string_view output = "4000.0";
    set_output(output.data(), output.length()); // NOLINT

    zassert_ok(sensor_sample_fetch(ezo));

    sensor_value val {}; 
    zassert_ok(sensor_channel_get(ezo, (enum sensor_channel)SENSOR_CHAN_CONDUCTIVITY, &val));

    zassert_equal(sensor_value_to_double(&val),  4000.0);
        
}

ZTEST_SUITE(t_ezo_ec, NULL, setup, NULL, NULL, teardown); // NOLINT

}
