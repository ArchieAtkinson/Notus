#include <iostream>

#include <zephyr/kernel.h>

#if defined(CONFIG_SOC_POSIX)
#include "posix_board_if.h"
#endif


int main(void)
{
    std::cout << "Hello, C++ world! " << CONFIG_BOARD << '\n';

#if defined(CONFIG_SOC_POSIX)
    posix_exit(0);
#endif
    return 0;
}
