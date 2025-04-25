#define DT_DRV_COMPAT atlas_ezo_ec

#include <zephyr/device.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/init.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/sys/__assert.h>
#include <zephyr/sys/byteorder.h>

#include "ezo_ec_util.h"

LOG_MODULE_REGISTER(ezo_ec, CONFIG_SENSOR_LOG_LEVEL); // NOLINT

static int ezo_ec_sample_fetch(const struct device *dev, enum sensor_channel chan)
{
    // It is recommended to read the data registers starting from the lower address to the higher address.
    struct ezo_ec_data *data = dev->data;
    const int transfer_length = 1;
    const struct ezo_ec_config *config = dev->config;
    uint8_t out[transfer_length];

    __ASSERT_NO_MSG(chan == SENSOR_CHAN_ALL);

    // Burst read data
    if (i2c_burst_read_dt(&config->i2c, 0, out, transfer_length) < 0)
    {
        LOG_DBG("Failed to read sample");
        return -EIO;
    }

    return 0;
}

static int ezo_ec_channel_get(const struct device *dev, enum sensor_channel chan, struct sensor_value *val)
{
    const struct ezo_ec_data *data = dev->data;

    if (chan == SENSOR_CHAN_PRESS)
    {
    }
    else if (chan == SENSOR_CHAN_AMBIENT_TEMP)
    {
    }
    else
    {
        return -ENOTSUP;
    }

    return 0;
}

int ezo_ec_init(const struct device *dev)
{
    const struct ezo_ec_config *const config = dev->config;

    // device is ready
    if (!device_is_ready(config->i2c.bus))
    {
        LOG_ERR("I2C bus device not ready");
        return -ENODEV;
    }

    // uint8_t chip_id = 0;
    // if (i2c_reg_read_byte_dt(&config->i2c, ezo_ec_REG_DEVICE_ID, &chip_id) < 0)
    // {
    //     LOG_ERR("Failed reading chip id");
    //     return -EIO;
    // }

    return 0;
}

static const struct sensor_driver_api ezo_ec_api_funcs = {
    .sample_fetch = ezo_ec_sample_fetch,
    .channel_get = ezo_ec_channel_get,
};

#define ezo_ec_DEFINE(inst)                                \
    static struct ezo_ec_data ezo_ec_data_##inst;          \
                                                           \
    static struct ezo_ec_config ezo_ec_config_##inst = {   \
        .i2c = I2C_DT_SPEC_INST_GET(inst),                 \
    };                                                     \
                                                           \
    DEVICE_DT_INST_DEFINE(inst, ezo_ec_init, NULL, &ezo_ec_data_##inst, &ezo_ec_config_##inst, POST_KERNEL, CONFIG_SENSOR_INIT_PRIORITY, &ezo_ec_api_funcs);

DT_INST_FOREACH_STATUS_OKAY(ezo_ec_DEFINE) // NOLINT
