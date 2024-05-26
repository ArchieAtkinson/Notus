#pragma once

#include <coroutine>
#include <functional>
#include <iostream>
#include <utility>

#include "active_object.hpp"
#include "zephyr/kernel.h"

template <typename Base, typename Derive>
concept IsDerivedFromBaseRef = std::is_base_of_v<Base, std::remove_reference_t<Derive>>;

template <typename ReturnType = void>
class Task;

template <typename ReturnType>
struct transform_task_awaiter;

struct Sleep;

struct task_promise_base
{
    explicit task_promise_base(ActiveObjectBase &aob) : _aob{&aob}
    {
    }

    template <typename Derive>
    explicit task_promise_base(Derive &aob)
        requires IsDerivedFromBaseRef<ActiveObjectBase, Derive>
        : _aob{&aob}
    {
    }

    static void unhandled_exception() noexcept
    {
    }

    auto initial_suspend() noexcept
    {
        struct awaiter
        {
            ActiveObjectBase *aob = nullptr;

            static bool await_ready() noexcept
            {
                return false;
            }

            void await_resume() const noexcept
            {
            }

            void await_suspend(std::coroutine_handle<> handle) const noexcept
            {
                aob->coroutines.push(handle);
            }
        };

        return awaiter{_aob};
    }

    [[nodiscard]] auto final_suspend() const noexcept
    {
        std::cout << "final_suspend on " << _aob->_name << "\n";
        std::cout << "Promise : " << this << "\n";
        
        final();

        return std::suspend_always{};
    }

    std::function<void()> final;
    ActiveObjectBase *_aob;
};

template <typename ReturnType>
struct task_promise : public task_promise_base
{
    Task<ReturnType> get_return_object();
    explicit task_promise(ActiveObjectBase &aob) : task_promise_base(aob)
    {
    }

    template <typename Derive, typename... Args>
    explicit task_promise(Derive &aob, Args... /*unused*/)
        requires IsDerivedFromBaseRef<ActiveObjectBase, Derive>
        : task_promise_base(aob)
    {
    }

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

template <typename RetType = void>
struct WaitingOnHandler;

template <typename RetType>
struct WaitingOnHandler
{
    WaitingOnHandler() = default;
    explicit WaitingOnHandler(RetType *return_val) : _return{return_val}
    {
        std::cout << "WaitingOnHandler: " << this << "\n";
    }
    struct Awaiter
    {
        static bool await_ready() noexcept
        {
            return false;
        }

        auto await_resume() const noexcept
        {
            if constexpr (std::is_void_v<RetType>)
            {
                return;
            }
            else
            {
                return *_return;
            }
        }

        void await_suspend(std::coroutine_handle<> handle) const noexcept
        {
            auto new_handle = std::coroutine_handle<task_promise_base>::from_address(handle.address());
            callee->waiting_on.push_back({handle, new_handle.promise()._aob});
            std::cout << "Adding " << handle.address() << " from " << new_handle.promise()._aob->_name
                      << " to:" << callee << "\n";
        }

        WaitingOnHandler<RetType> *callee{}; // NOLINT
        RetType* _return{};
    };

    auto operator co_await() & noexcept
    {
        if constexpr (std::is_void_v<RetType>)
        {
            return Awaiter{this};
        }
        else
        {
            return Awaiter{this, _return};
        }
    }

    auto operator co_await() && noexcept
    {
        if constexpr (std::is_void_v<RetType>)
        {
            return Awaiter{this};
        }
        else
        {
            return Awaiter{this, _return};
        }
    }

    void add_waiting_to_ao() noexcept
    {
        std::cout << "add_waiting_to_ao - WaitingOnHandler: " << this << "\n";
        for (const auto &waiting : waiting_on)
        {
            std::cout << "Pushing :" << waiting.handle.address() << "  to " << waiting._abo->_name << "\n";
            waiting._abo->coroutines.push(waiting.handle);
        }
        waiting_on.clear();
    }

    struct waiting_on_t
    {
        std::coroutine_handle<> handle{};
        ActiveObjectBase       *_abo{};
    };
    std::vector<waiting_on_t>    waiting_on;
    RetType* _return{};
};


template <typename ReturnType>
class Task : public WaitingOnHandler<ReturnType>
{
    friend transform_task_awaiter<ReturnType>;

  public:
    using task_type        = Task<ReturnType>;
    using promise_type     = task_promise<ReturnType>;
    using coroutine_handle = std::coroutine_handle<promise_type>;
    
    explicit Task(coroutine_handle handle) requires(!std::is_same_v<ReturnType, void>) : WaitingOnHandler<ReturnType>(&(_handle.promise()._value)), _handle{handle}
    {
        std::cout << "Creating Handler: " << handle.address() << "\n";
        std::cout << "Promise Handler: " << &handle.promise() << "\n";
        static auto out = [this](){this-> add_waiting_to_ao();};
        handle.promise().final = out;
    }

    explicit Task(coroutine_handle handle) requires(std::is_same_v<ReturnType, void>) : _handle{handle}
    {
        std::cout << "Creating Handler:  " << handle.address() << "\n";
        std::cout << "Promise Handler: " << &handle.promise() << "\n";
        static auto out = [this](){this-> add_waiting_to_ao();};
        handle.promise().final = out;
    }

    ~Task()
    {
        if (!_handle)
        {
            _handle.destroy();
        }
    }

    Task(const Task &)            = delete;
    Task &operator=(const Task &) = delete;
    Task(Task &&)                 = delete;
    Task &operator=(Task &&)      = delete;

    // Use the concept in the function signature
    ReturnType get_return() const
        requires(!std::is_same_v<ReturnType, void>)
    {
        return _handle.promise()._value;
    }

    void resume()
    {
        _handle.resume();
    }

  private:
    coroutine_handle _handle{};
};

struct Sleep : public WaitingOnHandler<void>
{
    Sleep() = delete;
    explicit Sleep(k_timeout_t timeout)
    {
        std::cout << "New Sleep: " << this << " " << k_uptime_get() << "\n";
        k_timer_init(&_timer, expiry_func, nullptr);
        k_timer_user_data_set(&_timer, static_cast<void *>(this));
        k_timer_start(&_timer, timeout, K_NO_WAIT);
    }

    friend void expiry_func(struct k_timer *timer);

    friend struct operator_sleep_awaiter;

    static void expiry_func(struct k_timer *timer)
    {
        std::cout << "expiry_func " << k_uptime_get() << "\n";
        auto *sleep = static_cast<Sleep *>(k_timer_user_data_get(timer));
        sleep->add_waiting_to_ao();
        k_timer_stop(timer);
    }

    Sleep(const Sleep &)            = delete;
    Sleep &operator=(const Sleep &) = delete;
    Sleep(Sleep &&)                 = delete;
    Sleep &operator=(Sleep &&)      = delete;

    ~Sleep() = default;

  private:
    k_timer _timer{};
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