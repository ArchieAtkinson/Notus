#include <zephyr/sys/printk.h>
#include <zephyr/logging/log.h>
#include <zephyr/kernel.h>

#include "module1.hpp"

K_THREAD_STACK_DEFINE(mod_1_stack, CONFIG_MAIN_STACK_SIZE); // NOLINT

Module1 mod_1("Module 1", mod_1_stack, CONFIG_MAIN_STACK_SIZE ,5); // NOLINT

Module1::SubHandler::SubHandler(Future<int> *fut, int a, int b) : BaseHandler(fut), _a{a}, _b{b}
{
}
void Module1::SubHandler::handle()
{
    _fut->_ret = _a - _b;
     
}

Module1::PrintHandler::PrintHandler(int a) : _a{a}
{
}
void  Module1::PrintHandler::handle()
{
    printk("a is %d\n", _a);
}