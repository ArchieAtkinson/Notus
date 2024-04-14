#include "stopwatch.hpp"
#include <chrono>

auto Stopwatch::start() -> void
{
    _start_tp = UpTime::now();
    _is_paused = false;
}

auto Stopwatch::pause() -> void
{
    if (!_is_paused)
    {
        _time_before_pause = get_time();
        _is_paused = true;
    }
}

auto Stopwatch::reset() -> void
{
    _time_before_pause = std::chrono::milliseconds{0};
    start();
}

auto Stopwatch::get_time() -> std::chrono::milliseconds
{
    std::chrono::microseconds time_passed {};
    if (_is_paused)
    {
        time_passed = _time_before_pause;
    }
    else
    {
        time_passed = UpTime::now() - _start_tp + _time_before_pause;
    }
    return std::chrono::duration_cast<std::chrono::milliseconds>(time_passed);
}
