#include <zephyr/kernel.h>

#include <zephyr/zbus/zbus.h>

#include "posix_board_if.h"

#include "logging.hpp"

#include "messages.hpp"

LOG_MODULE_DECLARE(zbus, CONFIG_ZBUS_LOG_LEVEL);

ZBUS_CHAN_DEFINE(acc_data_chan,  /* Name */
                struct acc_msg, /* Message type */
                NULL, /* Validator */
                NULL, /* User data */
                ZBUS_OBSERVERS_EMPTY,     /* observers */
		        ZBUS_MSG_INIT(.x = 0, .y = 0, .z = 0) /* Initial value */
);

static bool print_channel_data_iterator(const struct zbus_channel *chan, void *user_data)
{
	auto *count = static_cast<int*>(user_data);

	LOG_INF("%d - Channel %s:", *count, zbus_chan_name(chan));
	LOG_INF("      Message size: %d", zbus_chan_msg_size(chan));
	LOG_INF("      Observers:");

	++(*count);

	struct zbus_channel_observation *observation;

	for (int16_t i = chan->data->observers_start_idx, limit = chan->data->observers_end_idx;
	     i < limit; ++i) {
		STRUCT_SECTION_GET(zbus_channel_observation, i, &observation);

		__ASSERT(observation != NULL, "observation must be not NULL");

		LOG_INF("      - %s", observation->obs->name);
	}

	struct zbus_observer_node *obs_nd, *tmp;

	SYS_SLIST_FOR_EACH_CONTAINER_SAFE(&chan->data->observers, obs_nd, tmp, node) {
		LOG_INF("      - %s", obs_nd->obs->name);
	}

	return true;
}

static bool print_observer_data_iterator(const struct zbus_observer *obs, void *user_data)
{
	auto *count = static_cast<int*>(user_data);

	LOG_INF("%d - %s %s", *count,
		obs->type == ZBUS_OBSERVER_LISTENER_TYPE ? "Listener" : "Subscriber",
		zbus_obs_name(obs));

	++(*count);

	return true;
}

int main(void)
{
    struct acc_msg acc1 = {.x = 1, .y = 1, .z = 1};

	int count = 0;

	LOG_INF("Channel list:");
	zbus_iterate_over_channels_with_user_data(print_channel_data_iterator, &count);

	count = 0;

	LOG_INF("Observers list:");
	zbus_iterate_over_observers_with_user_data(print_observer_data_iterator, &count);
    int ret = zbus_chan_pub(&acc_data_chan, &acc1, K_SECONDS(1));
    if (ret != 0)
    {
        LOG_ERR("ZBus Pub Ret:%d", ret);
    }

	k_msleep(1000);

	acc1.x = 2;
	acc1.y = 2;
	acc1.z = 2;
    ret = zbus_chan_pub(&acc_data_chan, &(acc1), K_NO_WAIT);
    if (ret != 0)
    {
        LOG_ERR("ZBus Pub Ret:%d", ret);
    }

    acc1.x = 3;
	acc1.y = 3;
	acc1.z = 3;
    ret = zbus_chan_pub(&acc_data_chan, &(acc1), K_NO_WAIT);
    if (ret != 0)
    {
        LOG_ERR("ZBus Pub Ret:%d", ret);
    }

    acc1.x = 4;
	acc1.y = 4;
	acc1.z = 4;
    ret = zbus_chan_pub(&acc_data_chan, &(acc1), K_NO_WAIT);
    if (ret != 0)
    {
        LOG_ERR("ZBus Pub Ret:%d", ret);
    }

    k_msleep(100);

    posix_exit(0);
	return 0;
}