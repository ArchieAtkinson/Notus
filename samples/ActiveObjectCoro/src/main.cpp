#include <cstdlib>
#include <iostream>


#include "ao1.hpp"
#include "zephyr/kernel.h"


// AO1 ao1("AO1"); // NOLINT
// AO1 ao2("AO2"); // NOLINT



int main()
{
    AO1 ao1("AO1");
    AO2 ao2("AO2");

    auto out = ao2.ret_value(ao1);

    while(true)
    {
        ao2.execute();
        ao1.execute();
        k_sleep(K_TICKS(1));
    }

    std::cout << out.get_return() << "\n";

    std::exit(0); // NOLINT
}