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
    uint8_t cur_reg;
    uint8_t registers[EZO_EC_REG_AMOUNT];
};

uint8_t emul_ezo_ec_get_reg(const struct emul *emul, uint8_t address)
{
    struct emul_ezo_ec_data *data = emul->data;

    return data->registers[address];
}

void emul_ezo_ec_set_reg(const struct emul *emul, uint8_t address, uint8_t reg_value)
{
    struct emul_ezo_ec_data *data = emul->data;

    data->registers[address] = reg_value;
}

void emul_ezo_ec_reset_registers(const struct emul *emul)
{
    LOG_INF("Resetting Registers");
    struct emul_ezo_ec_data *data = emul->data;
    memset(data->registers, 0, EZO_EC_REG_AMOUNT * sizeof(uint8_t));
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
    if (data->cur_reg + len > EZO_EC_REG_AMOUNT)
    {
        LOG_ERR("Writing to nonexistant address");
        return -ENXIO;
    }
    for (int i = 0; i < len; i++)
    {
        data->registers[data->cur_reg] = buf[i];
        data->cur_reg++;
    }
    return 0;
}

static int emul_ezo_ec_read(struct emul_ezo_ec_data *data, uint8_t len, uint8_t *buf)
{
    if (data->cur_reg + len > EZO_EC_REG_AMOUNT)
    {
        LOG_ERR("Reading from nonexistant address");
        return -ENXIO;
    }

    for (int i = 0; i < len; i++)
    {
        buf[i] = data->registers[data->cur_reg];
        data->cur_reg++;
    }

    return 0;
}

static int emul_ezo_ec_transfer(const struct emul *target, struct i2c_msg *msgs, int num_msgs, int addr)
{
    struct emul_ezo_ec_data *data = target->data;

    i2c_dump_msgs(target->dev, msgs, num_msgs, addr);

    switch (num_msgs)
    {
    case 1: {
        if ((msgs[0].flags & I2C_MSG_RW_MASK) == I2C_MSG_READ)
        {
            emul_ezo_ec_read(data, msgs[0].len, msgs[0].buf);
        }
        else if ((msgs[0].flags & I2C_MSG_RW_MASK) == I2C_MSG_WRITE)
        {
            data->cur_reg = msgs[0].buf[0];
            if (msgs[0].len != 1)
            {
                emul_ezo_ec_write(data, msgs[0].len - 1, &msgs[0].buf[1]);
            }
        }
        else
        {
            LOG_ERR("Unknown transfer");
            return -EIO;
        }
        break;
    }
    case 2: {
        if ((msgs[0].flags & I2C_MSG_RW_MASK) == I2C_MSG_READ)
        {
            return -EIO;
        }
        if (msgs[0].len != 1U)
        {
            return -EIO;
        }

        data->cur_reg = msgs->buf[0];
        // Handle Second Message

        if ((msgs[1].flags & I2C_MSG_RW_MASK) == I2C_MSG_READ)
        {
            emul_ezo_ec_read(data, msgs[1].len, msgs[1].buf);
        }
        else if ((msgs[1].flags & I2C_MSG_RW_MASK) == I2C_MSG_WRITE)
        {
            emul_ezo_ec_write(data, msgs[1].len, msgs[1].buf);
        }
        break;
    }
    default: {
        LOG_ERR("Invalid number of messages");
        return -EIO;
    }
    }

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

    emul_ezo_ec_reset_registers(target);

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
