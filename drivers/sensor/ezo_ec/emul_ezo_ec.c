#define DT_DRV_COMPAT atlas_ezo_ec
#define LOG_LEVEL CONFIG_I2C_LOG_LEVEL

#include <stdint.h>
#include <string.h>
#include <time.h>

#include <zephyr/device.h>
#include <zephyr/drivers/emul.h>
#include <zephyr/drivers/i2c.h>
#include <zephyr/drivers/i2c_emul.h>
#include <zephyr/logging/log.h>
#include <zephyr/sys/printk.h>
#include <zephyr/sys/timeutil.h>

#include "emul_ezo_ec.h"
#include "ezo_ec_util.h"

LOG_MODULE_REGISTER(EMUL_EZO_EC);

// Runtime Data
struct emul_ezo_ec_data
{
    struct i2c_emul emul;
    char input[EZO_EC_BUFFER_SIZE];
    char output[EZO_EC_BUFFER_SIZE];
};

void emul_ezo_ec_get_input(const struct emul *emul, char *buf, int len)
{
    struct emul_ezo_ec_data *data = emul->data;
    memcpy(buf, data->input, len);
}

void emul_ezo_ec_set_output(const struct emul *emul, const char *buf, int len)
{
    struct emul_ezo_ec_data *data = emul->data;
    memset(data->output, 0, EZO_EC_BUFFER_SIZE);
    data->output[0] = 1; // Hardcode response code for now;
    memcpy(data->output+1, buf, len);
}

void emul_ezo_ec_reset_buffers(const struct emul *emul)
{
    LOG_INF("Resetting Registers");
    struct emul_ezo_ec_data *data = emul->data;
    memset(data->input,0, EZO_EC_BUFFER_SIZE);
    memset(data->output, 0, EZO_EC_BUFFER_SIZE);
}

struct ezo_ec_config emul_ezo_ec_get_config(const struct emul *emul)
{
    // Here we are casting away const which is BAD but is required for unit testing
    // to allow us to inject custom configs that are normally created at compile time
    const struct ezo_ec_config *config = (struct ezo_ec_config *)emul->dev->config;
    return *config;
}

void emul_ezo_ec_set_config(const struct emul *emul, struct ezo_ec_config config)
{
    struct ezo_ec_config *current_config = (struct ezo_ec_config *)emul->dev->config;
    *current_config = config;
}

static int emul_ezo_ec_write(struct emul_ezo_ec_data *data, uint8_t len, const uint8_t *buf)
{
    if (len > EZO_EC_BUFFER_SIZE)
    {
        LOG_ERR("Write too large");
        return -ENXIO;
    }

    memcpy(data->input, buf, len);
    return 0;
}

static int emul_ezo_ec_read(struct emul_ezo_ec_data *data, uint8_t len, uint8_t *buf)
{
    if (len > EZO_EC_BUFFER_SIZE)
    {
        LOG_ERR("Read to large");
        return -ENXIO;
    }

    memcpy(buf, data->output,  len);

    return 0;
}

static int emul_ezo_ec_transfer(const struct emul *target, struct i2c_msg *msgs, int num_msgs, int addr)
{
    struct emul_ezo_ec_data *data = target->data;

    i2c_dump_msgs(target->dev, msgs, num_msgs, addr);

    if ((msgs[0].flags & I2C_MSG_RW_MASK) == I2C_MSG_READ)
    {
        emul_ezo_ec_read(data, msgs[0].len, msgs[0].buf);
    }
    else if ((msgs[0].flags & I2C_MSG_RW_MASK) == I2C_MSG_WRITE)
    {
        emul_ezo_ec_write(data, msgs[0].len, &msgs[0].buf[0]);
    }
    else
    {
        LOG_ERR("Unknown transfer");
        return -EIO;
    }
    
    // switch (num_msgs)
    // {
    // case 1: {
    //     if ((msgs[0].flags & I2C_MSG_RW_MASK) == I2C_MSG_READ)
    //     {
    //         emul_ezo_ec_read(data, msgs[0].len, msgs[0].buf);
    //     }
    //     else if ((msgs[0].flags & I2C_MSG_RW_MASK) == I2C_MSG_WRITE)
    //     {
    //         data->cur_reg = msgs[0].buf[0];
    //         if (msgs[0].len != 1)
    //         {
    //             emul_ezo_ec_write(data, msgs[0].len - 1, &msgs[0].buf[1]);
    //         }
    //     }
    //     else
    //     {
    //         LOG_ERR("Unknown transfer");
    //         return -EIO;
    //     }
    //     break;
    // }
    // case 2: {
    //     if ((msgs[0].flags & I2C_MSG_RW_MASK) == I2C_MSG_READ)
    //     {
    //         return -EIO;
    //     }
    //     if (msgs[0].len != 1U)
    //     {
    //         return -EIO;
    //     }

    //     data->cur_reg = msgs->buf[0];
    //     // Handle Second Message

    //     if ((msgs[1].flags & I2C_MSG_RW_MASK) == I2C_MSG_READ)
    //     {
    //         emul_ezo_ec_read(data, msgs[1].len, msgs[1].buf);
    //     }
    //     else if ((msgs[1].flags & I2C_MSG_RW_MASK) == I2C_MSG_WRITE)
    //     {
    //         emul_ezo_ec_write(data, msgs[1].len, msgs[1].buf);
    //     }
    //     break;
    // }
    // default: {
    //     LOG_ERR("Invalid number of messages");
    //     return -EIO;
    // }
    // }

    return 0;
}

const static struct i2c_emul_api emul_ezo_ec_api = {
    .transfer = emul_ezo_ec_transfer,
};

static int emul_ezo_ec_init(const struct emul *target, const struct device *parent) 
{
    LOG_INF("emul_ezo_ec_init");
    const struct emul_ezo_ec_config *config = target->cfg;
    struct emul_ezo_ec_data *data = target->data;

    ARG_UNUSED(config);
    ARG_UNUSED(data);
    ARG_UNUSED(parent);

    emul_ezo_ec_reset_buffers(target);

    char info[] = "?i,EC,2.16";
    emul_ezo_ec_set_output(target, info, strlen(info));

    return 0;
}

// clang-format off
#define ezo_ec_EMUL(inst)                                                            \
    static struct emul_ezo_ec_data emul_ezo_ec_data_##inst;                       \
    EMUL_DT_INST_DEFINE(inst,                                                            \
                &emul_ezo_ec_init,                                                  \
                &emul_ezo_ec_data_##inst,                                          \
                NULL,                                            \
                &emul_ezo_ec_api, \
                NULL)                                                        
                                              
DT_INST_FOREACH_STATUS_OKAY(ezo_ec_EMUL) // NOLINT
// clang-format on
