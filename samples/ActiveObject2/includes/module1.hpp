#pragma once

#include <variant>

#include "ActiveObject.hpp"

class Module1
{
  private:
    static constexpr std::size_t message_queue_size = 10;

    struct SubArgs
    {
        int a;
        int b;
    };

    struct PrintArgs
    {
    };


  public:
    using msg_var = std::variant<std::monostate, Package<int, SubArgs>, Package<void, PrintArgs>>;

    FunctionCall<msg_var, Package<int, SubArgs>> Create_Sub()
    {
        return {&_msgq};
    }

    FunctionCall<msg_var, Package<void, PrintArgs>> Create_Print()
    {
        return {&_msgq};
    }


    Module1(std::string_view name, k_thread_stack_t *stack, std::size_t stack_size, int priority)
        : _name{name}
    {
        k_msgq_init(&_msgq, _msgq_buffer.data(), sizeof(msg_var), message_queue_size);

        // NOLINTNEXTLINE(cppcoreguidelines-prefer-member-initializer)
        _tid = k_thread_create(&_thread, stack, stack_size, thread_entry, this, NULL, NULL, priority, 0, K_NO_WAIT);
    };

  private:
    void execute_message(Package<int, SubArgs>);
    void execute_message(Package<void, PrintArgs>);
    
    static void thread_entry(void *p1, void *p2, void *p3)
    {
        ARG_UNUSED(p2);
        ARG_UNUSED(p3);
        static_cast<Module1 *>(p1)->run();
    }
    
    void run()
    {
        k_thread_name_set(_tid, _name.data());

        msg_var msg{};

        while (true)
        {
            k_msgq_get(&_msgq, &msg, K_FOREVER);

            auto execute = [this](auto &&arg) {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, std::monostate>)
                {
                    printk("Empty Msg\n");
                }
                else
                {
                    execute_message(arg);
                    if (arg.fut != nullptr)
                    {
                        k_sem_give(&arg.fut->_sem);
                    }
                }
            };
            std::visit(execute, msg);
        }
    }

    std::string_view                            _name;
    std::array<char, message_queue_size * sizeof(msg_var)> _msgq_buffer{};
    k_msgq                                      _msgq{};
    k_thread                                    _thread{};
    k_tid_t                                     _tid{};
};

extern Module1 mod_1;