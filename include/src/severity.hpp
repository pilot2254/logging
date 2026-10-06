#pragma once
#include <string>

namespace logging
{
	enum class severity
	{
		none = 0,
		info = 1,
		debug = 2,
		warning = 3,
		error = 4,
		fatal = 5
	};

	inline std::string SeverityToString(severity s)
	{
		switch (s)
		{
		case severity::info:    return "INFO";
		case severity::debug:   return "DEBUG";
		case severity::warning: return "WARNING";
		case severity::error:   return "ERROR";
		case severity::fatal:   return "FATAL";
		default:                return "NONE";
		}
	}
}
