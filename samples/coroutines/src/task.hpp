#pragma once

#include <coroutine>
#include <uptime_tp.hpp>

class Scheduler;

struct SleepTime
{
    int time;
};

template <typename ReturnType>
class Task;

template <typename TaskType = Task<void>>
struct task_promise
{
    static void unhandled_exception() noexcept
    {
    }

    TaskType get_return_object()
    {
        return TaskType{std::coroutine_handle<task_promise>::from_promise(*this)};
    }

    static std::suspend_always initial_suspend() noexcept
    {
        return {};
    }

    static std::suspend_always final_suspend() noexcept
    {
        return {};
    }

    void return_void() noexcept
    {
    }

    void sleep(int sleep_time_ms)
    {
        runable_time = UpTime::now() + std::chrono::milliseconds(sleep_time_ms);
    }

    // void return_value(std::string msg) noexcept
    // {
    //     std::cout << " ---- " << std::source_location::current().function_name() << "\n";
    //     _msgOut = std::move(msg);
    // }

    auto await_transform(SleepTime time)
    {
        struct sleep_awaiter
        {
            explicit sleep_awaiter(task_promise *promise, int sleep_time_ms)
                : _promise{promise}, _sleep_time_ms{sleep_time_ms}
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

            task_promise *_promise;
            int           _sleep_time_ms{};
        };

        return sleep_awaiter{this, time.time};
    }

    std::chrono::time_point<UpTime> runable_time = UpTime::now();
};

template <typename ReturnType = void>
class Task
{
  public:
    using task_type        = Task<ReturnType>;
    using promise_type     = task_promise<task_type>;
    using coroutine_handle = std::coroutine_handle<promise_type>;
    explicit Task(coroutine_handle handle) : _handle{handle}
    {
    }
    ~Task()
    {
        if (_handle)
        {
            _handle.destroy();
        }
    }

    Task(const Task &)            = delete;
    Task &operator=(const Task &) = delete;
    Task(Task &&)                 = delete;
    Task &operator=(Task &&)      = delete;

  private:
    friend class Scheduler;

    coroutine_handle _handle{};
};
