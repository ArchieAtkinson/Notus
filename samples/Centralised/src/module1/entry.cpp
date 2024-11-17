#include <array>
#include <cassert>
#include <cstddef>
#include <variant>

#include <posix_board_if.h>
#include <zephyr/kernel.h>

#include "logging.hpp"
#include "util.hpp"

#include "messages.hpp"

LOG_MODULE_REGISTER(MODULE1, CONFIG_MODULE1_LOG_LEVEL);

K_MSGQ_DEFINE(module1_in_msgq, sizeof(module1::module1_in_msgs), 10, 1);

ZBUS_CHAN_DEFINE(module1_in_chan,          /* Name */
                 module1::module1_in_msgs, /* Message type */
                 NULL,                     /* Validator */
                 NULL,                     /* User data */
                 ZBUS_OBSERVERS(listen),   /* observers */
                 ZBUS_MSG_INIT()           /* Initial value */
);



namespace
{

constexpr uint32_t stack_size      = 1024;
constexpr uint32_t thread_priority = 5;
void entry(void * /* arg1 */, void * /* arg2 */, void * /* arg3 */);
K_THREAD_DEFINE(thread_id, stack_size, entry, NULL, NULL, NULL, thread_priority,
                0, 0); // NOLINT

void entry(void * /* arg1 */, void * /* arg2 */, void * /* arg3 */)
{
    using namespace module1;

    k_thread_name_set(thread_id, "Module1");
    LOG_INF("Entry to thread %Ps", thread_id->name);

    while (true)
        
    {
        LOG_INF("Sending Out");
        send_message<module1_out_msgs>(
            &module1_out_chan, module1_out_msg_a{.value = "Hello"}, K_NO_WAIT);
        k_msleep(500);

        LOG_INF("Getting in");
        auto ret =
            msg_get<module1::module1_in_msgs>(&module1_in_msgq, K_FOREVER);
        LOG_INF("%d", std::get<0>(ret.value()).value);
        k_msleep(500);
    }
}

} // namespace

static void listener_callback(const struct zbus_channel *chan)
{
    if (&module1_in_chan == chan)
    {
        LOG_INF("Adding to Queue");
        add_to_queue<module1::module1_in_msgs>(chan, &module1_in_msgq,
                                               K_FOREVER);
    }
}

ZBUS_LISTENER_DEFINE(listen, listener_callback);