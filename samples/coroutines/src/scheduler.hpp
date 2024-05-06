#pragma once

#include <vector>
#include <iostream>
#include <ranges>

#include <uptime_tp.hpp>

#include <task.hpp>

class Scheduler
{
  public:
    void add_routine(Task *task)
    {
        routines.emplace_back(HandleHolder(UpTime::now(), task->_handle));
    }

    void run()
    {
        while (true)
        {       
            for (auto holder : routines)
            {
                k_sleep(K_TICKS(1));
                if (holder.runable_time < UpTime::now())
                {
                    holder.handle.resume();
                }
            }
        }
    }

    friend struct sleep_awaiter;

    void sleep(int sleep_time_ms, std::coroutine_handle<> handle)
    {
        auto itr = std::ranges::find(routines, handle, &Scheduler::HandleHolder::handle);
        
        if (itr != routines.end())
        {
            itr->runable_time = UpTime::now() + std::chrono::milliseconds(sleep_time_ms);
        }
    }

    struct HandleHolder
    {
        std::chrono::time_point<UpTime> runable_time;
        std::coroutine_handle<> handle{};
    };

  private:
    
    std::vector<HandleHolder> routines{};
};

struct sleep_awaiter
{
    explicit sleep_awaiter(int sleep_time_ms , Scheduler& scheduler) : _sleep_time_ms{sleep_time_ms}, _scheduler{scheduler}
    {
    }
    
    static bool await_ready() noexcept
    {
        return false;
    }

    void await_resume() const noexcept
    {
    }
    
    void await_suspend(std::coroutine_handle<>  handle) noexcept
    {
        _scheduler.sleep(_sleep_time_ms, handle);
    }

    int _sleep_time_ms{};
    Scheduler& _scheduler;
};