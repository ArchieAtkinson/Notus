#pragma once

#include <array>
#include <string_view>

#include <variant>
#include <zephyr/kernel.h>

template <typename RetType> struct Future
{
    Future()
    {
        k_sem_init(&_sem, 0, 1);
    }
    RetType _ret;
    k_sem   _sem{};
};


template <typename Msg, typename RetType, typename Handler, typename... Args>
class FunctionCall
{
public:
    using FutureType = std::conditional_t<std::is_void_v<RetType>, std::monostate, Future<RetType>>;

    explicit FunctionCall(k_msgq *msgq) : _msgq{msgq} {}

    void run(Args... args)
    {
        if constexpr (!std::is_void_v<RetType>)
        {
            Handler handler(&_fut, args...);
            Msg msg = handler;
            k_msgq_put(_msgq, &msg, K_NO_WAIT);
        }
        else
        {
            Handler handler(args...);
            Msg msg = handler;
            k_msgq_put(_msgq, &msg, K_NO_WAIT);
        }
    }

    RetType get_return(k_timeout_t timeout)
    {
        static_assert(!std::is_void_v<RetType>, "Cannot get return value of a void function");
        k_sem_take(&_fut._sem, timeout);
        return _fut._ret;
    }

private:
    FutureType _fut;
    k_msgq *_msgq;
};

struct BaseHandler
{
    explicit BaseHandler(Future<int> *fut) : _fut{fut}
    {
    }
    BaseHandler()                               = default;
    virtual ~BaseHandler()                      = default;
    BaseHandler(const BaseHandler &)            = default;
    BaseHandler &operator=(BaseHandler const &) = default;
    BaseHandler(BaseHandler &&)                 = default;
    BaseHandler &operator=(BaseHandler &&)      = default;

    void runner()
    {
        handle();
        k_sem_give(&_fut->_sem);
    }
    virtual void handle() = 0;
    Future<int> *_fut{};
};

struct BaseHandlerNoRet
{
    BaseHandlerNoRet()                                    = default;
    virtual ~BaseHandlerNoRet()                           = default;
    BaseHandlerNoRet(const BaseHandlerNoRet &)            = default;
    BaseHandlerNoRet &operator=(BaseHandlerNoRet const &) = default;
    BaseHandlerNoRet(BaseHandlerNoRet &&)                 = default;
    BaseHandlerNoRet &operator=(BaseHandlerNoRet &&)      = default;

    void runner()
    {
        handle();
    }
    virtual void handle() = 0;
};

template <typename MsgType, uint32_t MaxMsgs> class ActiveObject
{

  public:
    explicit ActiveObject(std::string_view name, k_thread_stack_t *stack, std::size_t stack_size, int priority)
        : _name{name}
    {
        k_msgq_init(&_msgq, _msgq_buffer.data(), sizeof(MsgType), MaxMsgs);

        // NOLINTNEXTLINE(cppcoreguidelines-prefer-member-initializer)
        _tid = k_thread_create(&_thread, stack, stack_size, thread_entry, this, NULL, NULL, priority, 0, K_NO_WAIT); 
    };

    k_msgq *get_msgq()
    {
        return &_msgq;
    }

  private:
    static void thread_entry(void *p1, void *p2, void *p3)
    {
        ARG_UNUSED(p2);
        ARG_UNUSED(p3);
        static_cast<ActiveObject *>(p1)->run();
    }
    
    void run()
    {
        k_thread_name_set(_tid, _name.data());

        MsgType msg{};

        while (true)
        {
            k_msgq_get(&_msgq, &msg, K_FOREVER);

            auto execute = [](auto &&arg) {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, std::monostate>)
                {
                    printk("Empty Msg\n");
                } else {
                    arg.runner();
                }
            };
            std::visit(execute, msg);
        }
    }

    std::string_view                            _name;
    std::array<char, MaxMsgs * sizeof(MsgType)> _msgq_buffer{};
    k_msgq                                      _msgq{};
    k_thread                                    _thread{};
    k_tid_t                                     _tid{};
};
