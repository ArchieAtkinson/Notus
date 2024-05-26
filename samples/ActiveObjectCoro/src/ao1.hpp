#pragma once

#include <string>
#include <string_view>

#include "task.hpp"
#include "active_object.hpp"
    

class AO1 : public ActiveObjectBase
{
  public:
    explicit AO1(const std::string_view name) : ActiveObjectBase(name)
    {
        
    }
    Task<int> ret_value()
    {
        std::cout << _name << " : " << __PRETTY_FUNCTION__ << "\n"; // NOLINT;
        co_return 10;
    }
};

class AO2 : public ActiveObjectBase
{
  public:
    explicit AO2(const std::string_view name): ActiveObjectBase(name)
    {
    }
    
    Task<int> ret_value(AO1 &ao1)
    {
        std::cout << "Started AO2 , Starting Ao1"
                  << "\n";
        ARG_UNUSED(ao1);
        
        // auto out = ao1.ret_value();

        // std::cout << "Waiting on AO1" << "\n";

        // auto value = co_await out;

        int value = 10;

        // std::cout << "AO1 Returns:" << value << "\n";

        std::cout << "Sleeping \n";

        auto sleep = Sleep(K_MSEC(10));
        co_await sleep;

        // std::cout << "Done AO2:" << value << "\n";

        co_return ++value;
    }

};

