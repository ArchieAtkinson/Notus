#include <zephyr/sys/printk.h>
#include <zephyr/logging/log.h>
#include <zephyr/kernel.h>

#include "module1.hpp"

K_THREAD_STACK_DEFINE(mod_1_stack, CONFIG_MAIN_STACK_SIZE); // NOLINT

Module1 mod_1("Module 1", mod_1_stack, CONFIG_MAIN_STACK_SIZE ,5); // NOLINT

void Module1::execute_message(Package<int, SubArgs> package)
{
    package.fut->_ret = package.args.a -  package.args.b;
}


void Module1::execute_message(Package<void, PrintArgs>)
{
    printk("HERE \n");
}