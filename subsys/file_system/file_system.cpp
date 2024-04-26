#include "etl/string.h"

#include "errors.hpp"
#include <etl/string_view.h>

#include "file_system.hpp"


ZFile::Flags operator&(const ZFile::Flags& lhs, const ZFile::Flags& rhs) {
    using T = std::underlying_type_t<ZFile::Flags>;
    return static_cast<ZFile::Flags>(static_cast<T>(lhs) & static_cast<T>(rhs));
}

ZFile::Flags operator|(const ZFile::Flags& lhs, const ZFile::Flags& rhs) {
    using T = std::underlying_type_t<ZFile::Flags>;
    return static_cast<ZFile::Flags>(static_cast<T>(lhs) | static_cast<T>(rhs));
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

ZFile ZFileSystem::open_file(const etl::string_view& file_name, ZFile::Flags flags)
{
    return {file_name, flags, _mount->mnt_point};
}


ZFileSystem::~ZFileSystem()
{
    fs_unmount(_mount);
}

ZFile::ZFile(const etl::string_view& file_name, Flags flags, const etl::string_view &mount)
{
    fs_file_t_init(&_file);

    constexpr std::size_t max_filename = 255;

    etl::string<max_filename> file_path;
    file_path.append(mount.data(), mount.size());
    file_path.append("/");
    file_path.append(file_name.data(), file_name.size());

    int ret = fs_open(&_file, file_path.c_str(), static_cast<int>(flags));
    if (ret != 0) {
        auto err_value_to_fs_err = [](int err)
        {
            switch (err)
            {
            case -ENOENT:
                return FileSystemError::file_not_at_path;
            case -EISDIR:
                return FileSystemError::path_is_directory;
            case -EBUSY:
                return FileSystemError::busy;
            case -EINVAL:
                return FileSystemError::invalid_filename;
            default:
                return FileSystemError::unknown;
            }
        };
        throw MajorError(err_value_to_fs_err(ret), ret);
    }
}

ZFile::~ZFile()
{
    fs_close(&_file);
}