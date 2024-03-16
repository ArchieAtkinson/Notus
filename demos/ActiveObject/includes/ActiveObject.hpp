#pragma once

#include <string_view>

#include <variant>
#include <zephyr/kernel.h>

template <typename RetType>
struct Future
{
    Future()
    {
        k_sem_init(&_sem, 0, 1);
    }
    RetType _ret;
    k_sem _sem{};
};

template<typename Msg, typename RetType, typename Handler, typename... Args>
class FunctionCall
{
  public:
    explicit FunctionCall(k_msgq *msgq) : _msgq{msgq}
    {
    }
    void run(Args... args)
    {
        Handler handler(&_fut, args...);
        Msg msg = handler;
        k_msgq_put(_msgq, &msg, K_NO_WAIT);
    }
    RetType get_return(k_timeout_t timeout)
    {
        k_sem_take(&_fut._sem, timeout);
        return _fut._ret;
    }

  private:
    Future<RetType> _fut;
    k_msgq *_msgq;
};

template <typename Msg, typename Handler, typename... Args>
class FunctionCall<Msg, void, Handler, Args...>
{
  public:
    explicit FunctionCall(k_msgq *msgq) : _msgq{msgq}
    {
    }
    void run(Args... args)
    {
        Handler handler(args...);
        Msg msg = handler;
        k_msgq_put(_msgq, &msg, K_NO_WAIT);
    }
  private:
    k_msgq *_msgq;
};


struct BaseHandler
{
    BaseHandler() = default;
    explicit BaseHandler(Future<int> *fut) : _fut{fut}
    {
    }
    void runner()
    {
        handle();
        k_sem_give(&_fut->_sem);
    }
    virtual void handle() = 0;
    Future<int>* _fut{};
};

struct BaseHandlerNoRet
{
    void runner()
    {
        handle();
    }
    virtual void handle() = 0;
};

template<typename MsgType>
class ActiveObject
{

  public:
    explicit ActiveObject(std::string_view name, k_thread_stack_t *stack, int priority) : _name{name}
    {
        k_msgq_init(&_msgq, _msgq_buffer, sizeof(MsgType), 10);

        _tid = k_thread_create(&_thread, stack, K_THREAD_STACK_SIZEOF(stack), thread_entry, this, NULL, NULL, priority, 0, K_NO_WAIT);
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

            auto execute = [](auto &&arg)
            {
                arg.runner();
            };
            std::visit(execute, msg);
        }
    }

    std::string_view _name;
    char        _msgq_buffer[10 * sizeof(MsgType)];
    k_msgq      _msgq;
    k_thread    _thread;
    k_tid_t _tid{};
};
