#include <etl/string.h>
#include <etl/string_view.h>
#include <tl/expected.hpp>

#include "errors.hpp"
#include "file_system.hpp"


ZFile::Flags operator&(const ZFile::Flags& lhs, const ZFile::Flags& rhs) {
    using T = std::underlying_type_t<ZFile::Flags>;
    return static_cast<ZFile::Flags>(static_cast<T>(lhs) & static_cast<T>(rhs));
}

ZFile::Flags operator|(const ZFile::Flags& lhs, const ZFile::Flags& rhs) {
    using T = std::underlying_type_t<ZFile::Flags>;
    return static_cast<ZFile::Flags>(static_cast<T>(lhs) | static_cast<T>(rhs));
}

template <typename V, typename F>
constexpr bool has_flag(V value, F flag) noexcept
{
    static_assert(std::is_same_v<V, std::underlying_type_t<F>>);
    return ((value & static_cast<V>(flag)) == static_cast<V>(flag));
}

ZFileSystem::ZFileSystem(struct fs_mount_t *mount) : _mount{mount}
{
    int ret = fs_mount(mount);
    if (ret != 0)
    {
        auto err_value_to_fs_err = [](int err)
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
        };
        throw MajorError(err_value_to_fs_err(ret), ret);
    }
}

ZFile ZFileSystem::open_file(const etl::string_view& file_name, const ZFile::Flags flags)
{
    return {file_name, static_cast<int>(flags), _mount->mnt_point};
}

bool ZFileSystem::check_file_exists(const etl::string_view& file_name)
{
    tl::expected<fs_file_t, FileSystemError> output = ZFile::open_file(file_name, 0, _mount->mnt_point);
    if (!output)
    {
        if (output.error() == FileSystemError::file_not_at_path)
        {
            return false;
        }

        throw MajorError(output.error(), 0);
    }

    auto file = output.value();
    fs_close(&file);
    return true;
}

ZFileSystem::~ZFileSystem()
{
    fs_unmount(_mount);
}

ZFile::ZFile(const etl::string_view& file_name, const int flags, const etl::string_view &mount) 
{
    tl::expected<fs_file_t, FileSystemError> output = open_file(file_name, flags, mount);
    if (!output)
    {
        throw MajorError(output.error(), 0);
    }
    _file = output.value();
}

void ZFile::write(std::span<uint8_t> data)
{
    if (!has_flag(_file.flags, ZFile::Flags::Write))
    {
        throw MajorError(FileSystemError::file_not_open_for_write, 0);
    }

    if (data.data() == nullptr)
    {
        throw MajorError(FileSystemError::bad_data, 0);
    }
    
    int ret = fs_write(&_file, data.data(), data.size_bytes());
    if (ret < 0) {
        auto err_value_to_fs_err = [](int err)
        {

            switch (err)
            {
            case -EBADF:
                return FileSystemError::fs_not_available;
            case -ENOTSUP:
                return FileSystemError::operation_not_supported;
            default:
                return FileSystemError::unknown;
            }
        };
        throw MajorError(err_value_to_fs_err(ret), ret);
    }

    if (ret != static_cast<int>(data.size_bytes())) {
        throw MajorError(FileSystemError::uncompleted_write, ret);
    }
}

void ZFile::read(std::span<uint8_t> data)
{
    if (!has_flag(_file.flags, ZFile::Flags::Read))
    {
        throw MajorError(FileSystemError::file_not_open_for_read, 0);
    }

    int ret = fs_read(&_file, data.data(), data.size_bytes());
    if (ret < 0) {
        auto err_value_to_fs_err = [](int err)
        {
            switch (err)
            {
            case -EBADF:
                return FileSystemError::fs_not_available;
            case -ENOTSUP:
                return FileSystemError::operation_not_supported;
            default:
                return FileSystemError::unknown;
            }
        };
        throw MajorError(err_value_to_fs_err(ret), ret);
    }

    if (ret != static_cast<int>(data.size_bytes())) {
        throw MajorError(FileSystemError::uncompleted_read, ret);
    }
}

tl::expected<fs_file_t, FileSystemError> ZFile::open_file(const etl::string_view& file_name, const fs_mode_t flags, const etl::string_view &mount)
{
    struct fs_file_t file{}; 
    fs_file_t_init(&file);

    constexpr std::size_t max_filename = 255;

    etl::string<max_filename> file_path;
    file_path.append(mount.data(), mount.size());
    file_path.append("/");
    file_path.append(file_name.data(), file_name.size());

    int ret = fs_open(&file, file_path.c_str(), flags);
    if (ret != 0)
    {
        FileSystemError err = FileSystemError::unknown;
        switch (ret)
        {
            case -ENOENT:
                err = FileSystemError::file_not_at_path;
                break;
            case -EISDIR:
                err = FileSystemError::path_is_directory;
                break;
            case -EBUSY:
                err = FileSystemError::busy;
                break;
            case -EINVAL:
                err = FileSystemError::invalid_filename;
                break;
            default:
                err = FileSystemError::unknown;
                break;
        }
        return tl::make_unexpected(err);
    };

    return file;
}

ZFile::~ZFile()
{
    fs_close(&_file);
}