#include <utility>
#include <string>

#include <zephyr/sys/printk.h>
#include <zephyr/zbus/zbus.h>
#include <zephyr/logging/log.h>
#include <zephyr/kernel.h>

#include "messages.hpp"
#include "pub_sub_with_msgq.hpp"

LOG_MODULE_DECLARE(zbus, CONFIG_ZBUS_LOG_LEVEL);

ZBUS_CHAN_DECLARE(simple_chan);

namespace {

K_MSGQ_DEFINE(simple_msgq, sizeof(SubscriberWithMsgQ<simple_msg, std::pair<int, std::string>>), 10, 1);

void module2_task();

K_THREAD_DEFINE(module2_task_id, CONFIG_MAIN_STACK_SIZE, module2_task, NULL, NULL, NULL, 7, 0, 0);

void module2_task()
{
    k_thread_name_set(module2_task_id, "Module 2");
	SubscriberWithMsgQ<simple_msg, std::pair<int, std::string>> msg({0}, nullptr);

    while (true)
    {
        k_msgq_get(&simple_msgq, &msg, K_FOREVER);

        LOG_INF("From queue -> %d", msg.get_message().a);

        k_msleep(100);

        msg.set_reply(std::make_pair(7, std::string(k_thread_name_get(module2_task_id))));
	}
}

void module2_callback(const struct zbus_channel *chan)
{
    const auto *msg = static_cast<const SubscriberWithMsgQ<simple_msg, std::pair<int, std::string>> *>(zbus_chan_const_msg(chan));

    k_msgq_put(&simple_msgq, msg, K_NO_WAIT);

    LOG_INF("From listener -> %d", msg->get_message().a);
}

}
ZBUS_LISTENER_DEFINE(module2_obj, module2_callback);

ZBUS_CHAN_ADD_OBS(simple_chan, module2_obj, 1);