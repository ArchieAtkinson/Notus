#pragma once
#include <system_error>
#include <type_traits>
#include <span>

#include <etl/string_view.h>
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
    bad_data,
    uncompleted_write,
    uncompleted_read,
    fs_not_available,
    operation_not_supported,
    file_not_open_for_write,
    file_not_open_for_read,
    unknown,
};

namespace std
{
template <> struct is_error_code_enum<FileSystemError> : true_type
{
};
} // namespace std

std::error_code make_error_code(FileSystemError);

class ZFileSystem;

class ZFile
{
  public:
    friend class ZFileSystem;
    enum class Flags
    {
        Check          = 0,
        Read           = FS_O_READ,
        Write          = FS_O_WRITE,
        Read_and_Write = FS_O_RDWR,
        Create         = FS_O_CREATE,
        Append         = FS_O_APPEND,
    };
    friend Flags operator&(const Flags &lhs, const Flags &rhs);
    friend Flags operator|(const Flags& lhs, const Flags& rhs);

    
    void write(std::span<uint8_t> data);
    void read(std::span<uint8_t> data);
    
    ~ZFile();
    ZFile()                              = delete;
    ZFile(const ZFile &other)            = delete;
    ZFile(ZFile &&other)                 = delete;
    ZFile &operator=(const ZFile &other) = delete;
    ZFile &operator=(ZFile &&other)      = delete;

  private:
    ZFile(const etl::string_view &file_name, const Flags flags, const etl::string_view &mount);
    struct fs_file_t _file
    {
    };
    const Flags _flags;
};

class ZFileSystem
{
  public:
    explicit ZFileSystem(struct fs_mount_t *mount);
    [[nodiscard]] ZFile open_file(const etl::string_view &file_name, const ZFile::Flags flags);
    bool check_file_exists();

    ~ZFileSystem();

    ZFileSystem(const ZFileSystem &other)            = delete;
    ZFileSystem(ZFileSystem &&other)                 = delete;
    ZFileSystem &operator=(const ZFileSystem &other) = delete;
    ZFileSystem &operator=(ZFileSystem &&other)      = delete;

  private:
    struct fs_mount_t *_mount;
};