#include <zephyr/kernel.h>

#include "events/data_a_event.hpp"
#include "events/data_b_event.hpp"

static void module_a_task() // NOLINT
{
    data_a_event msg_a{};
    data_b_event msg_b(10000000);
    while (true)
    {
        zbus_chan_pub(&data_a_event_chan, static_cast<void*>(&msg_a), K_MSEC(500));
        msg_a.value++;
        zbus_chan_pub(&data_b_event_chan, static_cast<void*>(&msg_b), K_MSEC(500));
        msg_b.value--;

        k_msleep(1000); // NOLINT
    }
}

// NOLINTNEXTLINE
K_THREAD_DEFINE(module_a_id, CONFIG_MAIN_STACK_SIZE, module_a_task, NULL, NULL, NULL, 3, 0, 0); 