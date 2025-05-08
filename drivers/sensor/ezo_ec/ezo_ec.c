#define DT_DRV_COMPAT atlas_ezo_ec

#include <stdio.h>

#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/init.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/sys/__assert.h>
#include <zephyr/sys/byteorder.h>

#include "ezo_ec_util.h"
#include "drivers/sensors/ezo_ec.h"

LOG_MODULE_REGISTER(ezo_ec, CONFIG_SENSOR_LOG_LEVEL); // NOLINT

// TODO:
// Add while loop (with max timeout) for checking reponse codes
// Add all channels
// Add all attr
// Add support for DTS based configuration

static int ezo_ec_sample_fetch(const struct device *dev, enum sensor_channel chan)
{
    struct ezo_ec_data *data = dev->data;
    const struct ezo_ec_config *config = dev->config;

    __ASSERT_NO_MSG(chan == SENSOR_CHAN_ALL);

    if (i2c_write_dt(&config->i2c, READ_COMMAND, 1) < 0)
    {
        LOG_ERR("Failed to request sample");
        return -EIO;
    }    

    k_msleep(READ_RESPONSE_TIME_MS);

    if (i2c_read_dt(&config->i2c, data->output, EZO_EC_BUFFER_SIZE) < 0)
    {
        LOG_ERR("Failed to read sample");
        return -EIO;
    }

    if(data->output[0] != OK_RESPONSE_CODE)
    {
        LOG_ERR("Reponse Code not 1");
        return -EIO;
    }

    return 0;
}

static int str_double_to_sensor_val(const struct ezo_ec_data *data, struct sensor_value *val)
{
    char *endptr = NULL;

    // 1 byte offset for response code
    double value = strtod(data->output + 1, &endptr);

    if(data->output + 1 == endptr)
    {
        return -EIO;
    }

    sensor_value_from_double(val, value);

    return 0;        
}

static int ezo_ec_channel_get(const struct device *dev, enum sensor_channel chan, struct sensor_value *val)
{
    const struct ezo_ec_data *data = dev->data;

    enum sensor_channel_ezo_ec our_chan = (enum sensor_channel_ezo_ec)chan;

    if (our_chan == SENSOR_CHAN_CONDUCTIVITY)
    {
        return str_double_to_sensor_val(data, val);
    }
    
    return -ENOTSUP;

}

static int calibrate(const struct device *dev)
{
    const struct ezo_ec_config *config = dev->config;

    if (i2c_write_dt(&config->i2c, CALIBRATE_DRY_COMMAND, sizeof(CALIBRATE_DRY_COMMAND)) < 0)
    {
        LOG_ERR("Failed to request sample");
        return -EIO;
    }    

    k_msleep(CALIBRATE_RESPONSE_TIME_MS);

    uint8_t response = 0;

    if (i2c_read_dt(&config->i2c, &response, 1) < 0)
    {
        LOG_ERR("Failed to read calibration response");
        return -EIO;
    }

    if(response != OK_RESPONSE_CODE)
    {
        LOG_ERR("Reponse Code not 1");
        return -EIO;
    }

    return 0;

}

static int ezo_ec_attr_set(const struct device *dev, enum sensor_channel chan,
                              enum sensor_attribute attr, const struct sensor_value *val)
{
    ARG_UNUSED(chan);
    ARG_UNUSED(val);
   
    if(attr == SENSOR_ATTR_CALIBRATION)
    {
        return calibrate(dev);
    }    

    LOG_ERR("Attribute not recognised. Err: %i", attr);
    return -ENOTSUP;
}


int ezo_ec_init(const struct device *dev)
{
    const struct ezo_ec_config *const config = dev->config;

    if (!device_is_ready(config->i2c.bus))
    {
        LOG_ERR("I2C bus device not ready");
        return -ENODEV;
    }
    
    if (i2c_write_dt(&config->i2c, INFO_COMMAND, 1) < 0)
    {
        LOG_ERR("Failed sending info command");
        return -EIO;
    }

    k_msleep(INFO_RESPONSE_TIME_MS);

    char buf[EZO_EC_BUFFER_SIZE];
    if(i2c_read_dt(&config->i2c, (uint8_t*)&buf, EZO_EC_BUFFER_SIZE))
    {
        LOG_ERR("Failed to get info command");
        return -EIO;
    }
    
    char expected[] = "?i,EC,2.16";
    if (buf[0] != OK_RESPONSE_CODE && strcmp(expected, buf+1) != 0)
    {
        LOG_ERR("Incorrect Device Info %s", buf);
        return -EIO; 
    }

    return 0;
}

static const struct sensor_driver_api ezo_ec_api_funcs = {
    .sample_fetch = ezo_ec_sample_fetch,
    .channel_get = ezo_ec_channel_get,
    .attr_set = ezo_ec_attr_set,
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
