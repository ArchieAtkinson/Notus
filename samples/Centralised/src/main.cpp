#include <posix_board_if.h>
#include <zephyr/kernel.h>

#include "logging.hpp"
#include "util.hpp"

#include "module1/messages.hpp"

LOG_MODULE_REGISTER(APPLICATION, CONFIG_APPLICATION_LOG_LEVEL);

K_MSGQ_DEFINE(in, sizeof(module1::module1_out_msgs), 10, 1);

bool print_channel_data_iterator(const struct zbus_channel *chan,
                                 void                      *user_data)
{
    auto *count = static_cast<int *>(user_data);

    LOG_INF("%d - Channel %s:", *count, zbus_chan_name(chan));
    LOG_INF("      Message size: %d", zbus_chan_msg_size(chan));
    LOG_INF("      Observers:");

    ++(*count);

    struct zbus_channel_observation *observation = nullptr;

    for (int16_t i     = chan->data->observers_start_idx,
                 limit = chan->data->observers_end_idx;
         i < limit; ++i)
    {
        STRUCT_SECTION_GET(zbus_channel_observation, i, &observation);

        __ASSERT(observation != nullptr, "observation must be not NULL");

        LOG_INF("      - %s", observation->obs->name);
    }

    struct zbus_observer_node *obs_nd = nullptr;
    struct zbus_observer_node *tmp    = nullptr;

    SYS_SLIST_FOR_EACH_CONTAINER_SAFE(&chan->data->observers, obs_nd, tmp, node)
    {
        LOG_INF("      - %s", obs_nd->obs->name);
    }

    return true;
}

bool print_observer_data_iterator(const struct zbus_observer *obs,
                                  void                       *user_data)
{
    auto *count = static_cast<int *>(user_data);

    LOG_INF("%d - %s %s", *count,
            obs->type == ZBUS_OBSERVER_LISTENER_TYPE ? "Listener"
                                                     : "Subscriber",
            zbus_obs_name(obs));

    ++(*count);

    return true;
}

int main()
{
    using namespace module1;
    LOG_INF("Hello World");

    LOG_INF("Channel list:");
    int count = 0;
    zbus_iterate_over_channels_with_user_data(print_channel_data_iterator,
                                              &count);

    LOG_INF("Observers list:");
    count = 0;
    zbus_iterate_over_observers_with_user_data(print_observer_data_iterator,
                                               &count);

    while (true)
    {
        LOG_INF("Send to out");
        module1::module1_in_msgs msg_out = module1::module1_in_msg_a{1};
        send_message<module1_in_msgs>(&module1_in_chan, msg_out, K_NO_WAIT);
        k_msleep(500);

        LOG_INF("Getting in");
        module1::module1_out_msgs msg_in{};
        k_msgq_get(&in, &msg_in, K_FOREVER);
        LOG_INF("%s", std::get<0>(msg_in).value);
        k_msleep(500);
    }

    k_sleep(K_FOREVER);
}

static void listener_callback(const struct zbus_channel *chan)
{
    if (&module1_out_chan == chan)
    {
        LOG_INF("Adding to Queue");
        add_to_queue<module1::module1_out_msgs>(chan, &in, K_FOREVER);
    }
}

ZBUS_LISTENER_DEFINE(central, listener_callback); // NOLINT