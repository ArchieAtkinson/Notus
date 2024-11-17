#include <zephyr/zbus/zbus.h>

#include "events/data_b_event.hpp"

// NOLINTNEXTLINE
ZBUS_CHAN_DEFINE(data_b_event_chan,	     /* Name */
		 data_b_event, /* Message type */
		 nullptr,		       /* Validator */
		 nullptr,		       /* User data */
		 ZBUS_OBSERVERS_EMPTY, /* observers */
		 ZBUS_MSG_INIT() /* Initial value */
);
