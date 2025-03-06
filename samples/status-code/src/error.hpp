#pragma once

#include "system_error2.hpp"

// Define your enum
enum class Error1
{
  e1,
  e2,
  e3
};

// Specialization to make Error1 work with system_error2
SYSTEM_ERROR2_NAMESPACE_BEGIN
template <>
struct quick_status_code_from_enum<Error1>
  : quick_status_code_from_enum_defaults<Error1>
{
  static constexpr const auto domain_name = "Error1";

  static constexpr const auto domain_uuid = "{a4e72c9b-f853-40e1-b7d5-8c6e43c6f8d7}";

  static const std::initializer_list<mapping> &value_mappings()
  {
    static const std::initializer_list<mapping> v = {
      {Error1::e1, "Error type 1", {errc::invalid_argument}},
      {Error1::e2, "Error type 2", {errc::resource_unavailable_try_again}},
      {Error1::e3, "Error type 3", {errc::operation_not_permitted}},
    };
    return v;
  }
};
SYSTEM_ERROR2_NAMESPACE_END

// Helper function to create status codes from Error1 enum values
constexpr inline
SYSTEM_ERROR2_NAMESPACE::quick_status_code_from_enum_code<Error1>
status_code(Error1 err) { return SYSTEM_ERROR2_NAMESPACE::quick_status_code_from_enum_code<Error1>(err); }

