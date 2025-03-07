#pragma once

#include <compare>
#include <concepts>
#include <string_view>
#include <type_traits>
#include <iostream>

// Forward declarations
class error_code;

// Concept for valid error code enums
template <typename E>
concept ErrorEnum = std::is_enum_v<E>;
template <ErrorEnum E> class error_domain;

// Base error domain interface - this is our "domain" concept
class error_domain_base
{
  public:
    virtual ~error_domain_base()                                                                 = default;
    virtual constexpr std::string_view name() const noexcept                                     = 0;
    virtual constexpr std::string_view message(int code) const noexcept                          = 0;
    virtual constexpr bool             equivalent(const error_code &lhs, int rhs) const noexcept = 0;
    virtual constexpr bool             equivalent(int lhs, const error_code &rhs) const noexcept = 0;
};

// Main error_code class
class error_code
{
  private:
    int                      m_value{0};
    const error_domain_base *m_domain{nullptr};

  public:
    constexpr error_code() noexcept = default;

    template <ErrorEnum E>
    constexpr error_code(E e, const error_domain<E> &domain) noexcept : m_value(static_cast<int>(e)), m_domain(&domain)
    {
    }

    constexpr int value() const noexcept
    {
        return m_value;
    }
    constexpr const error_domain_base &domain() const noexcept
    {
        return *m_domain;
    }

    constexpr std::string_view message() const noexcept
    {
        return m_domain != nullptr ? m_domain->message(m_value) : "No error";
    }

    constexpr explicit operator bool() const noexcept
    {
        return m_value != 0;
    }

    constexpr friend bool operator==(const error_code &lhs, const error_code &rhs) noexcept
    {
        return lhs.m_value == rhs.m_value && lhs.m_domain == rhs.m_domain;
    }

    template <ErrorEnum E> constexpr friend bool operator==(const error_code &lhs, E rhs) noexcept
    {
        return lhs.m_domain && lhs.m_domain->equivalent(lhs, static_cast<int>(rhs));
    }

    constexpr auto operator<=>(const error_code &) const = default;
};

// Concrete error domain for specific enums
template <ErrorEnum E> class error_domain : public error_domain_base
{
  private:
    std::string_view m_name;

  public:
    constexpr explicit error_domain(const std::string_view name) noexcept : m_name(name)
    {

    }
    constexpr std::string_view name() const noexcept override
    {
        return m_name;
    }

    constexpr std::string_view message(int code) const noexcept override
    {
        return get_message(static_cast<E>(code));
    }

    constexpr bool equivalent(const error_code &lhs, int rhs) const noexcept override
    {
        return &lhs.domain() == this && lhs.value() == rhs;
    }

    constexpr bool equivalent(int lhs, const error_code &rhs) const noexcept override
    {
        return &rhs.domain() == this && rhs.value() == lhs;
    }

    // This is the method that domain implementers need to provide
    constexpr virtual std::string_view get_message(E code) const noexcept = 0;
};
