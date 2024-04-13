#include <cstddef>
#include <zephyr/kernel.h>

#include <etl/vector.h>
#include <tl/expected.hpp>
#include "posix_board_if.h"
#include "zephyr/sys/printk.h"

#include <functional>
#include <string_view>
#include <source_location>
#include <utility>

enum class err_t
{
    error,
};

struct error_carry
{
    err_t error_code;
    etl::vector<std::size_t, 10> hash;
};

std::size_t hash_string_and_int(const std::string_view& str, uint num) {
    std::size_t hash1 = std::hash<std::string_view>{}(str);
    std::size_t hash2 = std::hash<uint>{}(num);

    // combine the hashes
    return hash1 ^ (hash2 << 1);
}

struct AddFunc
{
    std::source_location srcl = std::source_location::current();
    error_carry                operator()(error_carry ec) const
    {
        std::size_t hash = hash_string_and_int(srcl.function_name(), srcl.line());
        printk("%d\n", srcl.line());
        ec.hash.push_back(hash);
        return ec;
    }
};


tl::expected<int, error_carry> call3(int a)
{
    printk("%s - a:%d\n",__PRETTY_FUNCTION__, a);
    if (a > 90)
    {
        return tl::make_unexpected(error_carry{err_t::error, {}});
    }
    a++;
    return a;
}


tl::expected<int, error_carry> call2(int a)
{
    printk("%s - a:%d\n",__PRETTY_FUNCTION__, a);
    if (a > 95)
    {
        return tl::make_unexpected(error_carry{err_t::error, {}});
    }
    a++;
    return call3(a).transform_error(AddFunc{});
}



tl::expected<int, error_carry> call1(int a)
{
    printk("%s - a:%d\n",__PRETTY_FUNCTION__, a);
    if (a > 100)
    {
        return tl::make_unexpected(error_carry{err_t::error, {}});
    }
    a++;
    return call2(a).transform_error(AddFunc{});
}

int main ()
{

    auto ret = call1(96).transform_error(AddFunc{});

    if (!ret)
    {
        printk("Error\n");
        for (auto &&hash : ret.error().hash)
        {
            printk("%x \n", hash);
        }
    }
    else
    {
        printk("Not Error %d\n", ret.value());
    }

    printk("%s\n",std::source_location::current().function_name());

    posix_exit(0);
	return 0;
}
