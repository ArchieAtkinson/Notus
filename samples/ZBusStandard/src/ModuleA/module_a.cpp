#include <zephyr/kernel.h>

#include "events/data_a_event.hpp"


static void module_a_task()
{
    data_a_event msg
    {
        .value = 0
    };

    while (true)
    {
        zbus_chan_pub(&data_a_event_chan, static_cast<void*>(&msg), K_MSEC(500));
        msg.value++;

        k_msleep(1000);
    }
}

K_THREAD_DEFINE(module_a_id, CONFIG_MAIN_STACK_SIZE, module_a_task, NULL, NULL, NULL, 3, 0, 0); 