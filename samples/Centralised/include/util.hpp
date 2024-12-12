#pragma once

#include <tl/expected.hpp>

#include "zephyr/kernel.h"
#include <zephyr/zbus/zbus.h>

constexpr std::size_t std_message_queue_size = 10;

template <typename TMessageVariant>
int send_message(const zbus_channel *channel, auto message, k_timeout_t timeout)
{
    TMessageVariant msg = message;
    return zbus_chan_pub(channel, &msg, timeout);
}

template <typename TMessageVariant>
int add_to_queue(const zbus_channel *channel, k_msgq *msg_q,
                 k_timeout_t timeout)
{
    const auto *msg =
        static_cast<const TMessageVariant *>(zbus_chan_const_msg(channel));
    return k_msgq_put(msg_q, msg, timeout);
}

template <typename TMessageVariant>
tl::expected<TMessageVariant, int> msg_get(k_msgq *msgq, k_timeout_t timeout)
{
    TMessageVariant msg{};
    int             ret = k_msgq_get(msgq, &msg, timeout);
    if (ret == 0)
    {
        return msg;
    }

    return tl::make_unexpected(ret);
}

template <typename TMessageVariant, std::size_t queue_size>
class MessageExchange
{
  public:
    explicit MessageExchange(const struct zbus_channel *channel)
        : channel_{channel}
    {
        k_msgq_init(&msgq_, buffer_.data(), sizeof(TMessageVariant),
                    queue_size);
    }

    int send_message(auto message, k_timeout_t timeout)
    {
        TMessageVariant msg = message;
        return zbus_chan_pub(channel_, &msg, timeout);
    }

    int add_to_queue(k_timeout_t timeout)
    {
        const auto *msg =
            static_cast<const TMessageVariant *>(zbus_chan_const_msg(channel_));
        return k_msgq_put(&msgq_, msg, timeout);
    }

    tl::expected<TMessageVariant, int> msg_get(k_timeout_t timeout)
    {
        TMessageVariant msg{};
        int             ret = k_msgq_get(msgq_, &msg, timeout);
        if (ret == 0)
        {
            return msg;
        }

        return tl::make_unexpected(ret);
    }

  private:
    const struct zbus_channel                             *channel_{};
    k_msgq                                                 msgq_{};
    std::array<char, queue_size * sizeof(TMessageVariant)> buffer_;
};