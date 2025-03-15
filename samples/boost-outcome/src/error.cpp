#include "error.hpp"
   
namespace { // anonymous namespace
    
struct Error1ErrCategory : std::error_category
{
  const char* name() const noexcept override;
  std::string message(int err_value) const override;
  // std::error_condition default_error_condition(int err_value) const noexcept override;
};
    
const char* Error1ErrCategory::name() const noexcept
{
  return "Error1";
}
    
std::string Error1ErrCategory::message(int err_value) const
{
  switch (static_cast<Error1>(err_value))
  {      
  case Error1::e1:
    return "e1";
   
  case Error1::e2:
    return "e2";
      
  case Error1::e3:
    return "e3";
        
  }

  return "Unknown";
}
    
const Error1ErrCategory theError1ErrCategory {};
    
} // anonymous namespace
   
std::error_code make_error_code(Error1 err)
{
  return {static_cast<int>(err), theError1ErrCategory};
}
