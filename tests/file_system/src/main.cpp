
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <ranges>
#include <vector>
#include <array>
#include <numeric>


#include <etl/string_view.h>
#include <magic_enum.hpp>

#include <zephyr/device.h>
#include <zephyr/kernel.h>
#include <zephyr/storage/flash_map.h>

#include "errors.hpp"
#include "file_system.hpp"
#include "testing.hpp"
#include "zephyr/fs/fs.h"

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

auto ignore_create = [](ZFile::Flags flag) { return flag != ZFile::Flags::Create; }; // NOLINT
auto enum_flags = magic_enum::enum_values<ZFile::Flags>();  // NOLINT
auto non_create_flags = enum_flags | std::views::filter(ignore_create); // NOLINT

[[maybe_unused]] void convert_to_zeros(const std::string &filename)
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

[[maybe_unused]] std::string get_flash_bin_path()
{
    std::string           current_file = std::source_location::current().file_name();
    std::filesystem::path path(current_file);

    path = path.parent_path().parent_path() / "flash.bin";

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

ZTEST(file_system, test_constructor)
{
    try
    {
        ZFileSystem file_sys(&mountpoint);
    }
    catch (...)
    {
        zassert_unreachable();
    }
}

ZTEST(file_system, test_file_open_create_throw)
{
    auto open_test_fail = [](const etl::string_view &file_name, FileSystemError err)
    {
        try
        {
            ZFileSystem           file_sys(&mountpoint);
            [[maybe_unused]] auto unused = file_sys.open_file(file_name, ZFile::Flags::Create);
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

    open_test_fail("", FileSystemError::path_is_directory);
    open_test_fail("dir/file", FileSystemError::file_not_at_path);
}

ZTEST(file_system, test_file_open_create)
{
    ZFileSystem file_sys(&mountpoint);

    [[maybe_unused]] auto unused = file_sys.open_file("test", ZFile::Flags::Create);
    struct fs_file_t      _file
    {
    };
    fs_file_t_init(&_file);
    zassert_not_ok(fs_open(&_file, "/lfs1/test", 0));
}

ZTEST(file_system, test_file_open_other_throw)
{
    auto open_test_fail = [](const etl::string_view &file_name, FileSystemError err, ZFile::Flags flag) 
    {
        try
        {
            ZFileSystem           file_sys(&mountpoint);
            [[maybe_unused]] auto unused = file_sys.open_file(file_name, flag);
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

    for (const auto& flag : non_create_flags)
    {
        open_test_fail("", FileSystemError::path_is_directory, flag);
        open_test_fail("dir/file", FileSystemError::file_not_at_path, flag);
        open_test_fail("test", FileSystemError::file_not_at_path, flag);
    }
}

ZTEST(file_system, test_file_open_other)
{
    ZFileSystem file_sys(&mountpoint);
    {
        [[maybe_unused]] auto unused = file_sys.open_file("test", ZFile::Flags::Create);
    }

    for (const auto& flag : non_create_flags)
    {
        [[maybe_unused]] auto unused2 = file_sys.open_file("test", flag);
    }
}

ZTEST(file_system, test_file_open_create_with_other_throw)
{
    auto open_test_fail = [](const etl::string_view &file_name, FileSystemError err, ZFile::Flags flag)
    {
        try
        {
            ZFileSystem           file_sys(&mountpoint);
            [[maybe_unused]] auto unused = file_sys.open_file(file_name, flag);
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

    for (const auto& flag : non_create_flags)
    {
        open_test_fail("", FileSystemError::path_is_directory, ZFile::Flags::Create | flag);
        open_test_fail("dir/file", FileSystemError::file_not_at_path, ZFile::Flags::Create | flag);
    }
}

ZTEST(file_system, test_file_open_create_with_other)
{
    ZFileSystem file_sys(&mountpoint);

    for (const auto& flag : non_create_flags)
    {
        auto                  filename = magic_enum::enum_name<ZFile::Flags>(flag);
        etl::string_view      filename_view(filename.begin(), filename.size());
        [[maybe_unused]] auto unused = file_sys.open_file(filename_view, ZFile::Flags::Create | flag);
    }
}


ZTEST(file_system, test_file_write_throw)
{
    try
    {
        ZFileSystem file_sys(&mountpoint);
        auto        file = file_sys.open_file("test", ZFile::Flags::Create);
        std::array<uint8_t, 1> value = {1};
        file.write(value);
        zassert_unreachable();
    }
    catch (const MajorError &e)
    {
        zassert_equal(e.code(), FileSystemError::file_not_open_for_write);
    }
    catch (...)
    {
        zassert_unreachable();
    }

    try
    {
        ZFileSystem file_sys(&mountpoint);
        auto        file = file_sys.open_file("test", ZFile::Flags::Create | ZFile::Flags::Write);
        uint8_t    *ptr  = nullptr;
        std::span<uint8_t> bad_pointer{ptr, 1};
        file.write(bad_pointer);
        zassert_unreachable();
    }
    catch (const MajorError &e)
    {
        zassert_equal(e.code(), FileSystemError::bad_data);
    }
    catch (...)
    {
        zassert_unreachable();
    }
}

ZTEST(file_system, test_file_write)
{
    constexpr std::size_t data_size = 10;
    std::array<uint8_t, data_size> data{};
    std::iota(data.begin(), data.end(), 0);

    ZFileSystem file_sys(&mountpoint);
    {
        auto file = file_sys.open_file("test", ZFile::Flags::Create | ZFile::Flags::Write);
        file.write(data);
    }

    struct fs_file_t file_handler{};
    fs_file_t_init(&file_handler);
    zassert_ok(fs_open(&file_handler, "/lfs1/test", FS_O_READ));

    std::array<uint8_t, data_size> out{};
    zassert_equal(fs_read(&file_handler, out.data(), out.size()),  static_cast<int>(out.size()));
    zassert_equal(out, data);

    zassert_ok(fs_close(&file_handler));
}

ZTEST(file_system, test_file_read_throw)
{
    try
    {
        ZFileSystem file_sys(&mountpoint);
        auto        file = file_sys.open_file("test", ZFile::Flags::Create);

        constexpr std::size_t data_size = 10;
        std::array<uint8_t, data_size> data{};
        file.read(data);

        zassert_unreachable();
    }
    catch (const MajorError &e)
    {
        zassert_equal(e.code(), FileSystemError::file_not_open_for_read);
    }
    catch (...)
    {
        zassert_unreachable();
    }
}

ZTEST(file_system, test_file_read)
{
    constexpr std::size_t data_size = 10;
    std::array<uint8_t, data_size> data{};
    std::iota(data.begin(), data.end(), 0);

    ZFileSystem file_sys(&mountpoint);
    {
        auto file = file_sys.open_file("test", ZFile::Flags::Create | ZFile::Flags::Write);
        file.write(data);
    }

    auto file = file_sys.open_file("test", ZFile::Flags::Read);
    std::array<uint8_t, data_size> out{};
    file.read(out);

    zassert_equal(out, data);
}


ZTEST(file_system, test_file_close)
{
    ZFileSystem file_sys(&mountpoint);
    {
        [[maybe_unused]] auto unused = file_sys.open_file("test", ZFile::Flags::Create);
    }

    struct fs_file_t _file
    {
    };
    fs_file_t_init(&_file);
    zassert_ok(fs_open(&_file, "/lfs1/test", 0));
    zassert_ok(fs_close(&_file));
}

ZTEST_SUITE(file_system, nullptr, nullptr, nullptr, teardown, nullptr); // NOLINT

} // namespace