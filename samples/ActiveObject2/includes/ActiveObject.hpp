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

template <>
struct Future<void>
{
    Future()
    {
        k_sem_init(&_sem, 0, 1);
    }
    k_sem   _sem{};
};

template <typename RetType, typename ArgsType>
struct Package
{
    Future<RetType> *fut;
    ArgsType args;
};

template <typename Package>
struct ExtractTypes;

template <typename RetType, typename ArgsType>
struct ExtractTypes<Package<RetType, ArgsType>> {
    using UsingRetType = RetType;
    using UsingArgsType = ArgsType;
};

template <typename Package>
using ExtractRetType_t = typename ExtractTypes<Package>::UsingRetType;

template <typename Package>
using ExtractArgsType_t = typename ExtractTypes<Package>::UsingArgsType;

template <typename Msg, typename PackageType>
class FunctionCall
{
public:
    using RetType = ExtractRetType_t<PackageType>;
    using ArgsType = ExtractArgsType_t<PackageType>;
    using FutuRetType = Future<RetType>;

    FunctionCall(k_msgq *msgq) : _msgq{msgq} {} // NOLINT(hicpp-explicit-conversions)

    void run(ArgsType args)
    {
        Package<RetType, ArgsType> handler(std::is_void_v<RetType> ? nullptr : &_fut, args);
        Msg msg = handler;
        k_msgq_put(_msgq, &msg, K_NO_WAIT);
    }

    RetType get_return(k_timeout_t timeout)
    {
        static_assert(!std::is_void_v<RetType>, "Cannot get return value of a void function");
        k_sem_take(&_fut._sem, timeout);
        return _fut._ret;
    }

private:
    FutuRetType _fut;
    k_msgq *_msgq;
};