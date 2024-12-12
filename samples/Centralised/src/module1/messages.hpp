#pragma once

#include <cstdint>
#include <variant>


#include <zephyr/kernel.h>
#include <zephyr/zbus/zbus.h>

#include "util.hpp"

ZBUS_CHAN_DECLARE(module1_out_chan);
ZBUS_CHAN_DECLARE(module1_in_chan);

namespace module1
{


struct module1_in_msg_a
{
    int value;
};

struct module1_in_msg_b
{
    int value;
};

using module1_in_msgs = std::variant<module1_in_msg_a, module1_in_msg_b>;

// using MessageExchangeIn = MessageExchange<module1_in_msgs, std_message_queue_size>;
// MessageExchangeIn ToModule1(&module1_in_chan);

struct module1_out_msg_a
{
    std::array<char, 100> value; // NOLINT(cppcoreguidelines-avoid-magic-numbers)
};

using module1_out_msgs = std::variant<module1_out_msg_a>;

// using MessageExchangeOut = MessageExchange<module1_out_msgs, std_message_queue_size>;
// extern MessageExchangeOut ToCentral(&module1_out_chan);

} // namespace module1



