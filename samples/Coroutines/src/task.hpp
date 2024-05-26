#pragma once

#include <coroutine>
#include <iostream>
#include <utility>

#include <uptime_tp.hpp>

class Scheduler;

struct SleepTime
{
    int time;
};

template <typename ReturnType = void>
class Task;

struct task_promise_base
{
    static void unhandled_exception() noexcept
    {
    }

    static std::suspend_always initial_suspend() noexcept
    {
        return {};
    }

    friend struct final_awaitable;
    struct final_awaitable
    {
        [[nodiscard]] static auto await_ready()  noexcept -> bool { return false; }

        template<typename promise_type>
        std::coroutine_handle<> await_suspend(std::coroutine_handle<promise_type> coroutine) noexcept
        {
            auto& promise = coroutine.promise();
            if (promise._continuation != nullptr)
            {
                std::cout << _promise->_continuation.address() << "\n";
                std::cout << promise._continuation.address() << "\n\n";
                return promise._continuation;
            }
            
            return std::noop_coroutine(); 
        }

        auto await_resume() noexcept -> void
        {
        }
        task_promise_base *_promise;
    };

    auto final_suspend() noexcept
    {
        return final_awaitable{this};
    }

    void sleep(int sleep_time_ms)
    {
        runable_time = UpTime::now() + std::chrono::milliseconds(sleep_time_ms);
    }
    
    // auto await_transform(SleepTime time)
    // {
    //     struct sleep_awaiter
    //     {
    //         explicit sleep_awaiter(task_promise_base *promise, int sleep_time_ms)
    //             : _promise{promise}, _sleep_time_ms{sleep_time_ms}
    //         {
    //         }

    //         static bool await_ready() noexcept
    //         {
    //             return false;
    //         }

    //         void await_resume() const noexcept
    //         {
    //         }

    //         void await_suspend(std::coroutine_handle<> /*unused*/) const noexcept
    //         {
    //             _promise->sleep(_sleep_time_ms);
    //         }

    //         task_promise_base *_promise;
    //         int           _sleep_time_ms{};
    //     };

    //     return sleep_awaiter{this, time.time};
    // }

    void continuation(std::coroutine_handle<> continuation) noexcept
    {
        _continuation = continuation;
    }

    std::chrono::time_point<UpTime> runable_time = UpTime::now();
protected:
    std::coroutine_handle<> _continuation{nullptr};
};


template <typename ReturnType>
struct task_promise : public task_promise_base
{
    Task<ReturnType> get_return_object();

    void return_value(ReturnType value) noexcept
    {
        _value = std::move(value);
    }

    ReturnType result()
    {
        return _value;
    }

    ReturnType _value;
};


template <>
struct task_promise<void> : public task_promise_base
{
    Task<void> get_return_object();
    
    void return_void() noexcept
    {
    }
};

template <typename ReturnType>
class Task
{
  public:
    using task_type        = Task<ReturnType>;
    using promise_type     = task_promise<ReturnType>;
    using coroutine_handle = std::coroutine_handle<promise_type>;
    explicit Task(coroutine_handle handle) : _handle{handle}
    {
        std::cout << "Created\n";
    }
    ~Task()
    {
        std::cout << "destroyed\n";
        if (_handle)
        {
            _handle.destroy();
        }
    }

    Task(const Task &)            = delete;
    Task &operator=(const Task &) = delete;
    Task(Task &&)                 = delete;
    Task &operator=(Task &&)      = delete;


    // Use the concept in the function signature
    ReturnType get_return() requires (!std::is_same_v<ReturnType, void>)
    {
        return _handle.promise()._value;
    }

    void resume()
    {
        _handle.resume();
    }

    struct awaitable_base
    {
        explicit awaitable_base(coroutine_handle coroutine) noexcept : _coroutine(coroutine) {}

        bool await_ready() noexcept
        {
            return !_coroutine || _coroutine.done();
        }

        std::coroutine_handle<> await_suspend(std::coroutine_handle<> awaiting_coroutine) noexcept
        {
            _coroutine.promise().continuation(awaiting_coroutine);
            return _coroutine;
        }

       coroutine_handle _coroutine{};
    };

    // lvalue
    auto operator co_await() const& noexcept
    {        
        struct awaitable : public awaitable_base
        {
            explicit awaitable(coroutine_handle coroutine) noexcept : awaitable_base(coroutine)
            {
            }
            
            auto await_resume() -> decltype(auto)
            {
                return this->_coroutine.promise().result();
            }
        };

        return awaitable{_handle};
    }

    // rvalue
    auto operator co_await() const&& noexcept
    {
        struct awaitable : public awaitable_base
        {
            explicit awaitable(coroutine_handle coroutine) noexcept : awaitable_base(coroutine)
            {
            }
            auto await_resume() -> decltype(auto)
            {
                return std::move(this->_coroutine.promise()).result();
            }
        };
        return awaitable{_handle};
    }

  private:
    friend class Scheduler;
    coroutine_handle _handle{};
};



template <typename ReturnType>
inline Task<ReturnType> task_promise<ReturnType>::get_return_object()
{
    return Task<ReturnType>{std::coroutine_handle<task_promise<ReturnType>>::from_promise(*this)};
}

inline Task<void> task_promise<void>::get_return_object()
{
    return Task<void>{std::coroutine_handle<task_promise<void>>::from_promise(*this)};
}