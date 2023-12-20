#pragma once

#include <chrono>

#include "uptime_tp.hpp"

class IStopwatch
{
	public:
        virtual ~IStopwatch() = default;

        virtual auto start() -> void = 0;
        virtual auto pause() -> void = 0;
        virtual auto reset() -> void = 0;
        virtual auto get_time() -> std::chrono::milliseconds = 0;
};


class Stopwatch : public IStopwatch
{
	public:
        Stopwatch() = default;
        auto start() -> void final;
        auto pause() -> void final;
        auto reset() -> void final;
	    auto get_time() -> std::chrono::milliseconds final;

	  private:
	    std::chrono::time_point<UpTime> _start_tp;
	    std::chrono::milliseconds _time_before_pause{0};
	    bool _is_paused = false;
};