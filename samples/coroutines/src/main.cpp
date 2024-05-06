#include <cstdlib>
#include <iostream>

#include "scheduler.hpp"
#include "task.hpp"

Task foo(Scheduler& scheduler)
{
    std::cout << "Hello" << '\n';

    co_await sleep_awaiter(1000, scheduler);
    std::cout << "Goodbye" << '\n';

    co_return;
}


Task foo2(Scheduler& scheduler)
{
    std::cout << "Hello 2" << '\n';

    co_await sleep_awaiter(1000, scheduler);

    std::cout << "Goodbye 2" << '\n';

    co_return;
}



int main()
{
    Scheduler scheduler;
    auto      task1 = foo(scheduler);
    auto      task2 = foo2(scheduler);
    scheduler.add_routine(&task1);
    scheduler.add_routine(&task2);

    scheduler.run();

    std::exit(0);
}