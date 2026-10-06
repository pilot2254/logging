#pragma once

#include <chrono>
#include <format>
#include <string>

namespace logging
{
        inline std::string GetTime()
        {
                using namespace std::chrono;
                auto now = floor<milliseconds>(system_clock::now());
                return std::format("{:%Y-%m-%d %H:%M:%S}", zoned_time{current_zone(), now});
        }
}
