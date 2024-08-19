#pragma once

#include <variant>
#include <string>


#include <zephyr/kernel.h>
#include <zephyr/zbus/zbus.h>


ZBUS_CHAN_DECLARE(module1_out_chan);

namespace module1
{

// struct module1_in_msg_a
// {
//     int value;
// };

// struct module1_in_msg_b
// {
//     int value;
// };

// using module1_in_msgs = std::variant<std::monostate, module1_in_msg_a, module1_in_msg_b>;

// ZBUS_CHAN_DECLARE(module1_in_chan);


struct module1_out_msg_a
{
    std::string value;
};

using module1_out_msgs = std::variant<module1_out_msg_a>;

}

