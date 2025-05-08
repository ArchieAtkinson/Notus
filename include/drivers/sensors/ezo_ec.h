#ifndef INCLUDE_DRIVERS_SENSOR_EZO_EC_H_
#define INCLUDE_DRIVERS_SENSOR_EZO_EC_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <zephyr/drivers/sensor.h>
#include <zephyr/kernel.h>

enum sensor_channel_ezo_ec
{
    SENSOR_CHAN_CONDUCTIVITY = SENSOR_CHAN_PRIV_START,
};


#ifdef __cplusplus
}
#endif

#endif
