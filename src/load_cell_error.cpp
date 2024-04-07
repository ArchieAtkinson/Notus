#include <magic_enum.hpp>

#include "load_cell.hpp"
   
namespace { // anonymous namespace
    
struct LoadCellErrorCategory : std::error_category
{
  [[nodiscard]] const char* name() const noexcept override;
  [[nodiscard]] std::string message(int err_value) const override;
};
    
const char* LoadCellErrorCategory::name() const noexcept
{
  return "Load Cell";
}
    
std::string LoadCellErrorCategory::message(int err_value) const {
    auto error = static_cast<LoadCellError>(err_value);
    std::string_view errorName = magic_enum::enum_name(error);

    if (errorName.empty()) {
        return "(unrecognized error)";
    }
    return std::string(errorName);
}
 
const LoadCellErrorCategory theLoadCellErrorCategory {};
    
} // anonymous namespace
   
std::error_code make_error_code(LoadCellError err)
{
  return {static_cast<int>(err), theLoadCellErrorCategory};
}
 