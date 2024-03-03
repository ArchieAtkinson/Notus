#include <zephyr/sys/printk.h>
#include <zephyr/zbus/zbus.h>
#include <zephyr/logging/log.h>

#include "messages.hpp"

LOG_MODULE_DECLARE(zbus, CONFIG_ZBUS_LOG_LEVEL);

ZBUS_CHAN_DECLARE(acc_data_chan);

ZBUS_MSG_SUBSCRIBER_DEFINE(msg_sub_obj);

static void msg_sub_task(void)
{
    const struct zbus_channel *chan {};
    struct acc_msg acc {};


    while (true)
    {
        int ret = zbus_sub_wait_msg(&msg_sub_obj, &chan, &acc, K_FOREVER);
        if (ret != 0)
        {
            LOG_ERR("ZBus Sub Ret:%d", ret);
        }
        else if (&acc_data_chan == chan)
        {
			LOG_INF("From msg subscriber -> Acc x=%d, y=%d, z=%d", acc.x, acc.y, acc.z);
		}
    }
}

K_THREAD_DEFINE(msg_sub_id, CONFIG_MAIN_STACK_SIZE, msg_sub_task, NULL, NULL, NULL, 6, 0, 0);

ZBUS_CHAN_ADD_OBS(acc_data_chan, msg_sub_obj, 1);