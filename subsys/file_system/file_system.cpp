#include "errors.hpp"

#include "file_system.hpp"

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

ZFileSystem::~ZFileSystem()
{
    fs_unmount(_mount);
}