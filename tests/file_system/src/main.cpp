#include <zephyr/kernel.h>

#include "file_system.hpp"

#include "errors.hpp"
#include "testing.hpp"

#include <zephyr/device.h>
#include <zephyr/fs/fs_sys.h>
#include <zephyr/storage/flash_map.h>

#define PARTITION_NODE DT_NODELABEL(lfs1)

// NOLINTBEGIN(
// cppcoreguidelines-avoid-non-const-global-variables,
// cppcoreguidelines-interfaces-global-init,
// cppcoreguidelines-avoid-non-const-global-variables
// )
FS_FSTAB_DECLARE_ENTRY(PARTITION_NODE);
struct fs_mount_t mountpoint = FS_FSTAB_ENTRY(PARTITION_NODE);
// NOLINTEND

namespace
{

ZTEST(file_system, test_constructor_throw)
{
    auto constructor_test = [](fs_mount_t *mount, FileSystemError err)
    {
        try
        {
            FileSystem file_sys(mount);
            zassert_unreachable();
        }
        catch (const MajorError &e)
        {
            zassert_equal(e.code(), err);
        }
        catch (...)
        {
            zassert_unreachable();
        }
    };

    constructor_test(nullptr, FileSystemError::invalid_arg);

    struct fs_mount_t mount_no_mnt_point = mountpoint;
    mount_no_mnt_point.mnt_point              = "";
    constructor_test(&mount_no_mnt_point, FileSystemError::invalid_arg);

    struct fs_mount_t mount_not_type = mountpoint;
    mount_not_type.type              = 0;
    constructor_test(&mount_not_type, FileSystemError::fs_type_not_registered);

    struct fs_mount_t mount_null = mountpoint;
    mount_null.fs             = nullptr;
    constructor_test(&mount_not_type, FileSystemError::fs_type_not_registered);
}

ZTEST_SUITE(file_system, nullptr, nullptr, nullptr, nullptr, nullptr); // NOLINT

} // namespace