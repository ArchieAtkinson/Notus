#include "stopwatch.hpp"

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
    if (_is_paused)
    {
        return _time_before_pause;
    }
    else
    {
        return UpTime::now() - _start_tp + _time_before_pause;
    }
}
