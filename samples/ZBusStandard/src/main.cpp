#include <zephyr/kernel.h>
#include <posix_board_if.h>

#include "events/data_a_event.hpp"

#include "logging.hpp"

LOG_MODULE_REGISTER(main);

static void main_lis_cb(const struct zbus_channel *chan) // NOLINT
{
	const auto *msg = static_cast<const data_a_event*>(zbus_chan_const_msg(chan)); 

	LOG_INF("Value %d", msg->value);
}

ZBUS_LISTENER_DEFINE(main_lis, main_lis_cb); // NOLINT
ZBUS_CHAN_ADD_OBS(data_a_event_chan, main_lis, 1); // NOLINT

int main()
{
    k_sleep(K_FOREVER);
    posix_exit(0);
}