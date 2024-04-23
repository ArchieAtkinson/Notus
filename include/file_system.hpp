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

class ZFileSystem
{
  public:
    explicit ZFileSystem(struct fs_mount_t *mount);
    ~ZFileSystem();

    ZFileSystem(const ZFileSystem &other) = delete;

    ZFileSystem(ZFileSystem &&other)                 = delete;
    ZFileSystem &operator=(const ZFileSystem &other) = delete;

    ZFileSystem &operator=(ZFileSystem &&other) = delete;

  private:
    struct fs_mount_t *_mount;

};