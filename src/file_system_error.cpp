#include <magic_enum.hpp>

#include "file_system.hpp"
   
namespace { // anonymous namespace
    
struct FileSystemErrorCategory : std::error_category
{
  [[nodiscard]] const char* name() const noexcept override;
  [[nodiscard]] std::string message(int err_value) const override;
};
    
const char* FileSystemErrorCategory::name() const noexcept
{
  return "file system";
}
    
std::string FileSystemErrorCategory::message(int err_value) const {
    auto error = static_cast<FileSystemError>(err_value);
    std::string_view errorName = magic_enum::enum_name(error);

    if (errorName.empty()) {
        return "(unrecognized error)";
    }

    return std::string(errorName);
}
 
const FileSystemErrorCategory theFileSystemErrorCategory {};
    
} // anonymous namespace
   
std::error_code make_error_code(FileSystemError err)
{
  return {static_cast<int>(err), theFileSystemErrorCategory};
}
 