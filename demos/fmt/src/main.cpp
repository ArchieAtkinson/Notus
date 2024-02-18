#include <array>

#include <zephyr/kernel.h>

#include <fmt/core.h>
#include <fmt/color.h>
#include <fmt/ranges.h>

#if defined(CONFIG_SOC_POSIX)
#include "posix_board_if.h"
#endif

int main(void)
{
    std::array<char, 100> buffer = {'0'};
    auto end = fmt::format_to(buffer.begin(), "Hello, {}!", "world");
    *end                      = '\0';

    printk("%s\n", buffer.data());

    fmt::print("Hello, {}!", "world");

    fmt::print(fg(fmt::color::crimson) | fmt::emphasis::bold,
             "Hello, {}!\n", "world");
    fmt::print(fg(fmt::color::floral_white) | bg(fmt::color::slate_gray) |
             fmt::emphasis::underline, "Olá, {}!\n", "Mundo");
    fmt::print(fg(fmt::color::steel_blue) | fmt::emphasis::italic,
             "你好{}！\n", "世界");

#if defined(CONFIG_SOC_POSIX)
    posix_exit(0);
#endif
    return 0;
}
