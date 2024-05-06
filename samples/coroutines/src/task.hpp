#pragma once

#include <coroutine>
#include <uptime_tp.hpp>

class Scheduler;

class Task
{
  public:
    struct promise_type
    {
        static void                unhandled_exception() noexcept;
        Task                       get_return_object();
        static std::suspend_always initial_suspend() noexcept;
        static std::suspend_always final_suspend() noexcept;
        void return_void() noexcept;
    };
    explicit Task(promise_type *promise);
    
    ~Task();

    Task(const Task &)            = delete;
    Task &operator=(const Task &) = delete;
    Task(Task &&)                 = delete;
    Task &operator=(Task &&)      = delete;

  private:
    friend class Scheduler;
    void resume() const;

    using Handle = std::coroutine_handle<promise_type>;
    Handle _handle{};
    std::chrono::time_point<UpTime> runable_time;
};

