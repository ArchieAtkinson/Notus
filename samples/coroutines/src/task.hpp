#pragma once

#include <coroutine>
#include <uptime_tp.hpp>

class Scheduler;

struct SleepTime
{
    int time;
};

class Task
{
  public:
    
    struct promise_type
    {
        static void                unhandled_exception() noexcept;
        Task                       get_return_object();
        static std::suspend_always initial_suspend() noexcept;
        static std::suspend_always final_suspend() noexcept;
        void                       return_void() noexcept;
        void sleep(int sleep_time_ms)
        {
          runable_time = UpTime::now() + std::chrono::milliseconds(sleep_time_ms);
        }

        auto await_transform(SleepTime time)
        {
          struct sleep_awaiter
          {
              explicit sleep_awaiter(promise_type *promise, int sleep_time_ms) : _promise{promise}, _sleep_time_ms{sleep_time_ms}
              {
              }
              
              static bool await_ready() noexcept
              {
                  return false;
              }

              void await_resume() const noexcept
              {
              }
              
              void await_suspend(std::coroutine_handle<> /*unused*/) const noexcept
              {
                _promise->sleep(_sleep_time_ms);
              }

              promise_type *_promise;
              int _sleep_time_ms{};
          };
          
          return sleep_awaiter{this, time.time };
        }

        std::chrono::time_point<UpTime> runable_time = UpTime::now();
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
};

