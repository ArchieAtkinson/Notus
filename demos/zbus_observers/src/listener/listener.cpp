#include <zephyr/sys/printk.h>
#include <zephyr/zbus/zbus.h>
#include <zephyr/logging/log.h>

#include "messages.hpp"
#include "zephyr/kernel.h"

LOG_MODULE_DECLARE(zbus, CONFIG_ZBUS_LOG_LEVEL);

ZBUS_CHAN_DECLARE(acc_data_chan);

K_MSGQ_DEFINE(acc_msgq, sizeof(struct acc_msg), 10, 1);

static void listener_task()
{
	struct acc_msg acc {};

    while (true)
    {
        k_msgq_get(&acc_msgq, &acc, K_FOREVER);

        LOG_INF("From queue -> Acc x=%d, y=%d, z=%d", acc.x, acc.y, acc.z);
	}
}


static void listener_callback_example(const struct zbus_channel *chan)
{
    const auto *acc = static_cast<const struct acc_msg *>(zbus_chan_const_msg(chan));

    k_msgq_put(&acc_msgq, acc, K_NO_WAIT);

	LOG_INF("From listener -> Acc x=%d, y=%d, z=%d - Putting on msgq", acc->x, acc->y, acc->z);
}

ZBUS_LISTENER_DEFINE(listener_obj, listener_callback_example);

ZBUS_CHAN_ADD_OBS(acc_data_chan, listener_obj, 1);

K_THREAD_DEFINE(listener_task_id, CONFIG_MAIN_STACK_SIZE, listener_task, NULL, NULL, NULL, 7, 0, 0);