#pragma once

#include <etl/string_view.h>
#include <etl/vector.h>
#include <system_error>
#include <type_traits>

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
    invalid_filename,
    busy,
    path_is_directory,
    file_not_at_path,
    unknown,
};

namespace std
{
template <> struct is_error_code_enum<FileSystemError> : true_type
{
};
} // namespace std

std::error_code make_error_code(FileSystemError);

class ZFile
{
  public:
    enum class Flags
    {
        Check          = 0,
        Read           = FS_O_READ,
        Write          = FS_O_WRITE,
        Read_and_Write = FS_O_RDWR,
        Create         = FS_O_CREATE,
        Append         = FS_O_APPEND,
    };

    ZFile(const etl::string_view& file_name, Flags flags, const etl::string_view& mount);
    ~ZFile();

    ZFile(const ZFile &other)            = delete;
    ZFile(ZFile &&other)                 = delete;
    ZFile &operator=(const ZFile &other) = delete;
    ZFile &operator=(ZFile &&other)      = delete;

  private:
    struct fs_file_t _file
    {
    };

};

class ZFileSystem
{
  public:
    explicit ZFileSystem(struct fs_mount_t *mount);
    ZFile file_open(const etl::string_view& file_name, ZFile::Flags flags);

    ~ZFileSystem();

    ZFileSystem(const ZFileSystem &other)            = delete;
    ZFileSystem(ZFileSystem &&other)                 = delete;
    ZFileSystem &operator=(const ZFileSystem &other) = delete;
    ZFileSystem &operator=(ZFileSystem &&other)      = delete;

  private:
    struct fs_mount_t *_mount;
};