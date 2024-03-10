#include <string>

#include <zephyr/kernel.h>
#include <zephyr/zbus/zbus.h>
#include <posix_board_if.h>

#include "logging.hpp"
#include "messages.hpp"
#include "pub_sub_with_msgq.hpp"

LOG_MODULE_DECLARE(zbus, CONFIG_ZBUS_LOG_LEVEL);

using SubType = SubscriberWithMsgQ<simple_msg, int>;

ZBUS_CHAN_DEFINE(simple_chan,                                /* Name */
                 SubType,                                    /* Message type */
                 NULL,                                       /* Validator */
                 NULL,                                       /* User data */
                 ZBUS_OBSERVERS_EMPTY,                       /* observers */
                 (SubType()) /* Initial value */
);


int main(void)
{
    PublisherWithMsgQ<simple_msg, std::pair<int, std::string>, 10> producer(&simple_chan);
    producer.send_message({1}, K_FOREVER);
    auto reply1 = producer.get_reply(K_FOREVER).value();
    LOG_INF("Reply: %d, %s", reply1.first, reply1.second.c_str());
    
    auto reply2 = producer.get_reply(K_FOREVER).value();
    LOG_INF("Reply: %d, %s", reply2.first, reply2.second.c_str());

    posix_exit(0);
	return 0;
}