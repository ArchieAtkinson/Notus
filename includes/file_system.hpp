#pragma once

#include <system_error>

#include <etl/unordered_map.h>

#include <unordered_map>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmissing-field-initializers"
#pragma GCC diagnostic ignored "-Wshadow"
#include <zephyr/fs/fs.h>
#pragma GCC diagnostic pop

enum class FileSystemError
{
    invalid_arg = 1,
    already_mounted,
    fs_type_not_supported,
};

struct FileSystemError2
{
    enum
    {
        invalid_arg,
        already_mounted,
        fs_type_not_supported
    };

    explicit FileSystemError2(int error_) : error(error_)
    {
    }

    int error;
};

namespace std
{
template <> struct is_error_code_enum<FileSystemError> : true_type
{
};
} // namespace std

std::error_code make_error_code(FileSystemError);

class FileSystem
{
  public:
    explicit FileSystem(struct fs_mount_t *mount);
    ~FileSystem();

    FileSystem(const FileSystem &other) = delete;

    FileSystem(FileSystem &&other)                 = delete;
    FileSystem &operator=(const FileSystem &other) = delete;

    FileSystem &operator=(FileSystem &&other) = delete;

  private:
    struct fs_mount_t *_mount;

    
    using error_pair = std::pair<int, FileSystemError>;

    // NOLINTNEXTLINE(cert-err58-cpp)
    inline const static etl::unordered_map mounting_error_conversion{
        error_pair{-EINVAL, FileSystemError::invalid_arg},
        error_pair{-EBUSY, FileSystemError::already_mounted},
        error_pair{-ENOTSUP, FileSystemError::fs_type_not_supported},
    }; 
};
