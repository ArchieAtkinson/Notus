#pragma once

#include <system_error>

#include <etl/unordered_map.h>

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
    fs_type_not_registered,
    unknown,
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

    static FileSystemError err_value_to_fs_err(int err)
    {
        switch (err)
        {
        case -ENOENT:
            return FileSystemError::fs_type_not_registered;
        case -EINVAL:
            return FileSystemError::invalid_arg;
        case -EBUSY:
            return FileSystemError::already_mounted;
        case -ENOTSUP:
            return FileSystemError::fs_type_not_supported;
        default:
            return FileSystemError::unknown;
        }
    }
};
