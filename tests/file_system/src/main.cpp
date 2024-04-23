#include <filesystem>
#include <fstream>
#include <iostream>
#include <vector>

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

void convert_to_zeros(const std::string &filename)
{
    std::ifstream inFile(filename, std::ios::binary | std::ios::ate);
    zassert_true(inFile.is_open());

    std::streamsize size = static_cast<std::streamsize>(inFile.tellg());
    inFile.close();
    std::ofstream outFile(filename, std::ios::binary | std::ios::trunc);
    zassert_true(outFile.is_open());

    std::vector<char> buffer(size, 0);
    outFile.write(buffer.data(), size);
    zassert_true(outFile.is_open());

    outFile.close();
}


std::string get_flash_bin_path()
{
    std::string current_file = std::source_location::current().file_name();
    std::filesystem::path path(current_file);
    path.remove_filename();
    path += "flash.bin";
    return path.string();
} 

void teardown([[maybe_unused]] void *fixture)
{
    convert_to_zeros(get_flash_bin_path());

    int ret = fs_unmount(&mountpoint);
    zassert_true(ret == -EINVAL);
}

ZTEST(file_system, test_constructor_throw)
{
    auto constructor_test = [](fs_mount_t *mount, FileSystemError err)
    {
        try
        {
            ZFileSystem file_sys(mount);
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
    mount_no_mnt_point.mnt_point         = "";
    constructor_test(&mount_no_mnt_point, FileSystemError::invalid_arg);

    struct fs_mount_t mount_not_type = mountpoint;
    mount_not_type.type              = 0;
    constructor_test(&mount_not_type, FileSystemError::fs_type_not_registered);

    struct fs_mount_t mount_null = mountpoint;
    mount_null.fs                = nullptr;
    constructor_test(&mount_not_type, FileSystemError::fs_type_not_registered);
}

ZTEST_SUITE(file_system, nullptr, nullptr, nullptr, teardown, nullptr); // NOLINT

} // namespace