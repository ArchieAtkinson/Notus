#pragma once

#include <variant>

#include "ActiveObject.hpp"

class Module1 
{
  public:
    Module1(std::string_view name, k_thread_stack_t *stack, int priority) : _ao(name, stack, priority)
    {
    }

    struct SubHandler : public BaseHandler
    {
        SubHandler() = default;
        SubHandler(Future<int> *fut, int a, int b);
        void handle() override;
      private:  
        int _a, _b;
    };

    struct PrintHandler : public BaseHandlerNoRet
    {
        PrintHandler() = default;
        PrintHandler(int a);
        void handle() override;
      private:
        int _a;
    };

    using msg_var = std::variant<SubHandler, PrintHandler>;

    FunctionCall<msg_var, int, SubHandler, int, int> Create_Sub()
    {
        return FunctionCall<msg_var, int, SubHandler, int, int>(_ao.get_msgq());
    }

    FunctionCall<msg_var, void, PrintHandler, int> Create_Print()
    {
        return FunctionCall<msg_var, void, PrintHandler, int>(_ao.get_msgq());
    }

  private:
    ActiveObject<msg_var> _ao;
};

extern Module1 mod_1;