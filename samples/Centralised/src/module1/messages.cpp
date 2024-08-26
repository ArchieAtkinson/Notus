#include "messages.hpp"

ZBUS_CHAN_DEFINE(module1_out_chan,          /* Name */
                 module1::module1_out_msgs, /* Message type */
                 nullptr,                   /* Validator */
                 nullptr,                   /* User data */
                 ZBUS_OBSERVERS(central),   /* observers */
                 ZBUS_MSG_INIT()            /* Initial value */
);



