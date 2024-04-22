
#include "zephyr/arch/posix/arch.h"

#include "errors.hpp"

#include "file_system.hpp"


FileSystem::FileSystem(struct fs_mount_t *mount) : _mount{mount}
{
    int ret = fs_mount(mount);
    if (ret != 0)
    {
        throw MajorError(err_value_to_fs_err(ret), ret);
    }
}

FileSystem::~FileSystem()
{
    arch_nop();
}