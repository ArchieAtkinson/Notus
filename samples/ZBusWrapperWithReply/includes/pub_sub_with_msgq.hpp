#pragma once

#include <array>
#include <cstddef>

#include <tl/expected.hpp>

#include "zephyr/sys_clock.h"
#include <zephyr/kernel.h>
#include <zephyr/zbus/zbus.h>

template<typename MessageType, typename RetType>
class SubscriberWithMsgQ
{
  public:
    SubscriberWithMsgQ() = default;
    SubscriberWithMsgQ(MessageType message, k_msgq * msqg) : _msqg{msqg}, _message{message} {}
    MessageType get_message() const
    {
        return _message;
    }
    int set_reply(RetType ret, k_timeout_t timeout=K_NO_WAIT)
    {
        return k_msgq_put(_msqg, &ret, timeout);
    }

  private:
    k_msgq * _msqg{};
    MessageType _message;
    
};

template<typename MessageType, typename RetType, std::size_t _msgq_size>
class PublisherWithMsgQ
{
  public:
    explicit PublisherWithMsgQ(const struct zbus_channel * chan) : _chan{chan}
    {
        k_msgq_init(&_msgq, _msgq_buffer.data(), sizeof(RetType), _msgq_size);
    }
    
    int send_message(MessageType message, k_timeout_t timeout)
    {
        _subscriber = SubscriberWithMsgQ<MessageType, RetType>(message, &_msgq);
        k_msgq_purge(&_msgq);
        return zbus_chan_pub(_chan, &_subscriber, timeout);
    }

    tl::expected<RetType, int> get_reply(k_timeout_t timeout)
    {
        RetType reply {};
        int ret = k_msgq_get(&_msgq, &reply,timeout);
        if (ret != 0)
        {
            return tl::make_unexpected(ret);
        }
        return reply;
    }

  private:
    const struct zbus_channel *_chan;
    std::array<char,_msgq_size * sizeof(RetType)>  _msgq_buffer{};
    k_msgq _msgq {};
    SubscriberWithMsgQ<MessageType,RetType> _subscriber{};
};