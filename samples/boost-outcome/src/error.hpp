#pragma once

#include <system_error>

enum class Error1
{
  e1,
  e2,
  e3
};

namespace std
{
  template <>
    struct is_error_code_enum<Error1> : true_type {};
}
   
std::error_code make_error_code(Error1);
