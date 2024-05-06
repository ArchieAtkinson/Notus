#include <cstdlib>
#include <iostream>

#include "scheduler.hpp"
#include "task.hpp"

Task<void> foo()
{
    std::cout << "Hello" << '\n';

    co_await SleepTime(1000);

    std::cout << "Goodbye" << '\n';

    co_return;
}


Task<void> foo2()
{
    std::cout << "Hello 2" << '\n';

    co_await SleepTime(1000);

    std::cout << "Goodbye 2" << '\n';

    co_return;
}

int main()
{
    Scheduler scheduler;
    auto      task1 = foo();
    auto      task2 = foo2();
    scheduler.add_routine(&task1);
    scheduler.add_routine(&task2);

    scheduler.run();

    std::exit(0);
}