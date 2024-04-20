#include <zephyr/kernel.h>

#include "errors.hpp"
#include "file_system.hpp"
#include "testing.hpp"
#include "zephyr/fs/fs.h"

namespace
{

ZTEST(file_system, test_constructor_throw)
{
    bool exception_caught = false;
    try
    {
        FileSystem file_sys(nullptr);
    }
    catch (const MajorError &e)
    {
        zassert_equal(e.code(), FileSystemError::invalid_arg);
        exception_caught = true;
    }
    catch (...)
    {
        zassert_unreachable();
    }

    exception_caught = false;
    struct fs_mount_t mount
    {
    };

    try
    {
        FileSystem file_sys(&mount);
    }
    catch (const MajorError &e)
    {
        zassert_equal(e.code(), FileSystemError::invalid_arg);
        exception_caught = true;
    }
    catch (...)
    {
        zassert_unreachable();
    }

    // std::pair foo{1, 5};

    // etl::unordered_map mounting_error_conversion3({-EINVAL, FileSystemError::invalid_arg},
    //                                               {-EBUSY, FileSystemError::already_mounted},
    //                                               {-ENOTSUP, FileSystemError::fs_type_not_supported});

    using Pair = std::pair<int, std::string>;
    Pair pair1 = {1, "one"};
    Pair pair2 = {2, "two"};
    Pair pair3 = {3, "three"};
    // etl::unordered_map my_map2{ {1, "one"}, {2, "two"}, {3, "three"}};

}

ZTEST_SUITE(file_system, nullptr, nullptr, nullptr, nullptr, nullptr); // NOLINT

} // namespace