#pragma once

#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/i2c.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/sys/util.h>

#ifdef __cplusplus
extern "C"
{
#endif

#define EZO_EC_BUFFER_SIZE 30
#define OK_RESPONSE_CODE 1

#define INFO_COMMAND "i"
#define INFO_RESPONSE_TIME_MS 300

#define READ_COMMAND "R"
#define READ_RESPONSE_TIME_MS 600

#define CALIBRATE_DRY_COMMAND "Cal,dry"
#define CALIBRATE_RESPONSE_TIME_MS 600

#define PROBE_COMMAND "K,"
#define PROBE_RESPONSE_TIME_MS 300


int ezo_ec_init(const struct device *dev);

struct ezo_ec_config
{
    struct i2c_dt_spec i2c;
};

struct ezo_ec_data
{
    double conductiviy; 
    char output[EZO_EC_BUFFER_SIZE];
};

#ifdef __cplusplus
}
#endif
