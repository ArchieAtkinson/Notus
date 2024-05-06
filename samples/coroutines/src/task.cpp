#include "task.hpp"


Task::Task(promise_type *promise) : _handle(Handle::from_promise(*promise))
{
}

void Task::resume() const
{
    _handle.resume();
}

Task::~Task()
{
    if (_handle)
    {
        _handle.destroy();
    }
}


void Task::promise_type::unhandled_exception() noexcept
{
}

Task Task::promise_type::get_return_object()
{
    return Task{this};
}

std::suspend_always Task::promise_type::initial_suspend() noexcept
{
    return {};
}

std::suspend_always Task::promise_type::final_suspend() noexcept
{
    return {};
}

void Task::promise_type::return_void() noexcept {};








