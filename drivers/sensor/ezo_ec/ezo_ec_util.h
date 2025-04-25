#pragma once

#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/i2c.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/sys/util.h>

#ifdef __cplusplus
extern "C"
{
#endif

#define EZO_EC_REG_AMOUNT 10

int ezo_ec_init(const struct device *dev);

typedef union {
    uint8_t full_reg;
    struct
    {
        uint8_t INT_S : 2;
        uint8_t DRDY : 1;
        uint8_t INT_F_OVR : 1;
        uint8_t INT_F_WTM: 1;
        uint8_t INT_F_FULL : 1;
        uint8_t : 2;
    };
} ezo_ec_ctrl_3_reg_t;

struct ezo_ec_config
{
    struct i2c_dt_spec i2c;
};

struct ezo_ec_data
{

};

#ifdef __cplusplus
}
#endif
