#include <posix_board_if.h>
#include <zephyr/kernel.h>

#include "module1.hpp"

int main(void)
{

    auto sub1_func_call = mod_1.Create_Sub();
    sub1_func_call.run(5, 6);
    int ret = sub1_func_call.get_return(K_FOREVER);

    printk("%d \n", ret);

    auto print1_func_call = mod_1.Create_Print();
    print1_func_call.run(5);

    k_msleep(5);

    posix_exit(0);
    return 0;
}