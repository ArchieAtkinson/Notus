#include <zephyr/zbus/zbus.h>

struct data_a_event
{
    int value;
};

// NOLINTNEXTLINE
ZBUS_CHAN_DECLARE(data_a_event_chan);