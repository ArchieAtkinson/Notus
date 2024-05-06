#pragma once

#include <vector>
#include <iostream>
#include <ranges>

#include <uptime_tp.hpp>

#include <task.hpp>

class Scheduler
{
  public:
    void add_routine(Task<void> *task)
    {
        routines.emplace_back(task);
    }

    void run()
    {
        while (true)
        {       
            for (auto *task : routines)
            {
                k_sleep(K_TICKS(1));
                if (task->_handle.promise().runable_time < UpTime::now())
                {
                    if (!task->_handle.done())
                    {
                        task->_handle.resume();
                    }
                }
            }
        }
    }

    friend struct sleep_awaiter;

  private:
    
    std::vector<Task<void> *> routines{};
};