#include <magic_enum.hpp>

#include "button.hpp"
   
namespace { // anonymous namespace
    
struct ButtonErrorCategory : std::error_category
{
  [[nodiscard]] const char* name() const noexcept override;
  [[nodiscard]] std::string message(int err_value) const override;
};
    
const char* ButtonErrorCategory::name() const noexcept
{
  return "Button";
}
    
std::string ButtonErrorCategory::message(int err_value) const {
    auto error = static_cast<ButtonError>(err_value);
    std::string_view errorName = magic_enum::enum_name(error);

    if (errorName.empty()) {
        return "(unrecognized error)";
    } else {
        return std::string(errorName);
    }
}
 
const ButtonErrorCategory theButtonErrorCategory {};
    
} // anonymous namespace
   
std::error_code make_error_code(ButtonError err)
{
  return {static_cast<int>(err), theButtonErrorCategory};
}
 