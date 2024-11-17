#include <zephyr/zbus/zbus.h>

#include "events/data_a_event.hpp"

// NOLINTNEXTLINE
ZBUS_CHAN_DEFINE(data_a_event_chan,	     /* Name */
		 data_a_event, /* Message type */
		 nullptr,		       /* Validator */
		 nullptr,		       /* User data */
		 ZBUS_OBSERVERS_EMPTY, /* observers */
		 ZBUS_MSG_INIT(.value = 0) /* Initial value */
);
