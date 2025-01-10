#include <assert.h>
#include <variant>
#include <zephyr/kernel.h>
#include <posix_board_if.h>

#include "events/data_a_event.hpp"
#include "events/data_b_event.hpp"

#include "logging.hpp"

LOG_MODULE_REGISTER(main);

/*
Each new event requires 5 changes.
1. Header - Add event header to file
2. Variant - Add event type to the EventHolder
3. Listener - Add channel to listener with ZBUS_CHAN_ADD_OBS
4. Callback - Add boilerplate to put event on queue with `push_event_to_queue`
5. Handler - Add overload to process_event() to handle event as required

All the above actions, apart from the Listener macro, will error if not present.
*/

using EventHolder = std::variant<const data_a_event, const data_b_event>;

K_MSGQ_DEFINE(event_queue, sizeof(EventHolder), 10 , 4); // NOLINT

void process_event(const data_a_event &event)
{
    LOG_INF("A Value: %d", event.value);
}

void process_event(const data_b_event event)
{
    LOG_INF("B Value: %d", event.value);
}


int main()
{
    LOG_INF("Entering Main");
    while (true)
    {
        EventHolder event{};
        k_msgq_get(&event_queue, &event, K_FOREVER);
        std::visit([](const auto &holder) { process_event(holder); }, event);
        k_msleep(500);  // NOLINT
    }
    k_sleep(K_FOREVER);
    posix_exit(0);
}

static void main_lis_cb(const struct zbus_channel *channel) // NOLINT
{
    auto push_event_to_queue = []<class TEvent>(const struct zbus_channel *chan)
    {
        const auto *event_ptr = static_cast<const TEvent *>(zbus_chan_const_msg(chan));
        const EventHolder event = *event_ptr;
        k_msgq_put(&event_queue, &event, K_NO_WAIT);
    };
    
    if (channel == &data_a_event_chan)
    {
        push_event_to_queue.template operator()<data_a_event>(channel);
    }
    else if (channel == &data_b_event_chan)
    {
        push_event_to_queue.template operator()<data_b_event>(channel);
    }
    else 
    {   
        __ASSERT(false, "Channel %s is being listened to but not processed", channel->name);
    }

}

ZBUS_LISTENER_DEFINE(main_lis, main_lis_cb); // NOLINT
ZBUS_CHAN_ADD_OBS(data_a_event_chan, main_lis, 1); // NOLINT
ZBUS_CHAN_ADD_OBS(data_b_event_chan, main_lis, 1); // NOLINT