#pragma once

#include <iostream>
#include <queue>
#include <coroutine>
#include <string>
#include <utility>

struct task_promise_base;

class ActiveObjectBase
{
    friend struct task_promise_base;
    friend struct normal_awaiter_base;

  public:
    explicit ActiveObjectBase(const std::string_view name) : _name{name}
    {
            std::cout << _name << " created \n";
    } 
    void execute()
    {
        while (!coroutines.empty())
        {
            std::coroutine_handle<> &handle = coroutines.front();

            // std::cout << "Front " << handle.address() << " in : " << _name << "\n";
            
            if (handle.done())
            { 
                handle.destroy();
            }

            if (handle)
            {
                // std::cout << "Running " << handle.address() << " in : " << _name << "\n";
                handle.resume();

                if (handle.done())
                { 
                    handle.destroy();
                }
            }

            coroutines.pop();
        }
    }
    std::queue<std::coroutine_handle<>> coroutines; // Convert to a "UniqueQueue"
    std::string _name{};
};


