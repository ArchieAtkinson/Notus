#include <array>
#include <cassert>
#include <cstddef>

#include <zephyr/kernel.h>
#include <posix_board_if.h>

#include <logging.hpp>

#include "messages.hpp"

LOG_MODULE_REGISTER(MODULE1, CONFIG_MODULE1_LOG_LEVEL);




// ZBUS_CHAN_DEFINE(module1_in_chan,                        /* Name */
//                  module1_in_msgs,                      /* Message type */
//                  NULL,                                 /* Validator */
//                  NULL,                                 /* User data */
//                  ZBUS_OBSERVERS_EMPTY,                 /* observers */
//                  ZBUS_MSG_INIT({}) /* Initial value */
// );

// void listener_callback(const struct zbus_channel *chan)
// {

//     if (&module1_out_chan == chan)
//     {
//             const module1_out_msgs *msg;
//             msg = zbus_chan_const_msg(chan); // Direct message access
//             LOG_INF("%s", std::get<0>(&msg));
//     }
// }

// ZBUS_LISTENER_DEFINE(my_listener, listener_callback);

ZBUS_CHAN_DEFINE(module1_out_chan,          /* Name */
                 module1::module1_out_msgs, /* Message type */
                 nullptr,                   /* Validator */
                 nullptr,                   /* User data */
                 ZBUS_OBSERVERS(central),   /* observers */
                 ZBUS_MSG_INIT()            /* Initial value */
);

namespace
{

constexpr uint32_t stack_size = 1024;
constexpr uint32_t thread_priority = 5;
void entry(void * /* arg1 */, void * /* arg2 */, void * /* arg3 */);
K_THREAD_DEFINE(thread_id, stack_size, entry, NULL, NULL, NULL, thread_priority, 0, 0); // NOLINT

void entry(void * /* arg1 */, void * /* arg2 */, void * /* arg3 */)
{
    k_thread_name_set(thread_id, "Module1");
    LOG_INF("Entry to thread %s", thread_id->name);


    while(true)
    {
        module1::module1_out_msgs msg = module1::module1_out_msg_a{"string"};
        zbus_chan_pub(&module1_out_chan, &msg, K_NO_WAIT);
        k_msleep(1000);
    }
}


} // namespace



