#pragma once

#include <chrono>
#include <format>
#include <string>

namespace logging
{
	//local time with milliseconds, never throws
	//if the tz database isnt available (minimal containers, some windows setups) it falls back to utc
	inline std::string current_time()
	{
		using namespace std::chrono;
		const auto now = floor<milliseconds>(system_clock::now());

		//looked up once instead of on every line
		static const time_zone* const zone = []() -> const time_zone*
		{
			try { return current_zone(); }
			catch (...) { return nullptr; }
		}();

		try
		{
			if (zone) return std::format("{:%Y-%m-%d %H:%M:%S}", zoned_time{ zone, now });
		}
		catch (...) {}

		return std::format("{:%Y-%m-%d %H:%M:%S}", now); //utc
	}
}
