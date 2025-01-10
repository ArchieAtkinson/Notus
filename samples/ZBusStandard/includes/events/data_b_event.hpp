#include <zephyr/zbus/zbus.h>


struct data_b_event
{
    int value;
};

// NOLINTNEXTLINE
ZBUS_CHAN_DECLARE(data_b_event_chan);