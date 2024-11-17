#pragma once

#include <variant>
#include <string>


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
using MessageExchangeIn = MessageExchange<module1_in_msgs, 10>;
MessageExchangeIn EXToModule(&module1_in_chan);

struct module1_out_msg_a
{
    char value[100];
};

using module1_out_msgs = std::variant<module1_out_msg_a>;
// using MessageExchangeOut = MessageExchange<module1_out_msgs, 10>;
// extern MessageExchangeOut EXToCentral(&module1_out_chan);

}

