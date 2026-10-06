#pragma once

#include <format>
#include <iostream>
#include <mutex>
#include <string>

#include "../vendor/termcolor.hpp"
#include "file.hpp"
#include "severity.hpp"
#include "time.hpp"

namespace logging
{
	class logger
	{
	public:
		static logger& get()
		{
			static logger instance;
			return instance;
		}

		bool set_file(const std::string& path)
		{
			std::lock_guard lock(m_mutex);
			return m_file.open(path);
		}

		//messages below this severity are ignored
		void set_min_severity(severity s)
		{
			std::lock_guard lock(m_mutex);
			m_min = s;
		}

		void log(severity s, const std::string& message)
		{
			std::lock_guard lock(m_mutex);
			if (s < m_min) return;

			const std::string line = std::format("[{}] [{}]: {}", GetTime(), SeverityToString(s), message);

			std::cout << color(s) << line << termcolor::reset << '\n';

			m_file.write(line);
		}

	private:
		logger() { m_file.open("logger.txt"); }

		//termcolor for each severity
		static std::ostream& (*color(severity s))(std::ostream&)
		{
			switch (s)
			{
			case severity::info:    return termcolor::green;
			case severity::debug:   return termcolor::cyan;
			case severity::warning: return termcolor::yellow;
			case severity::error:   return termcolor::red;
			case severity::fatal:   return termcolor::on_red;
			default:                return termcolor::white;
			}
		}

		std::mutex m_mutex;
		file       m_file;
		severity   m_min = severity::none;
	};

	//easy helpers: logging::info("x = {}", 5);
	template <typename... Args>
	void info(std::format_string<Args...> fmt, Args&&... args)
	{
		logger::get().log(severity::info, std::format(fmt, std::forward<Args>(args)...));
	}

	template <typename... Args>
	void debug(std::format_string<Args...> fmt, Args&&... args)
	{
		logger::get().log(severity::debug, std::format(fmt, std::forward<Args>(args)...));
	}

	template <typename... Args>
	void warning(std::format_string<Args...> fmt, Args&&... args)
	{
		logger::get().log(severity::warning, std::format(fmt, std::forward<Args>(args)...));
	}

	template <typename... Args>
	void error(std::format_string<Args...> fmt, Args&&... args)
	{
		logger::get().log(severity::error, std::format(fmt, std::forward<Args>(args)...));
	}

	template <typename... Args>
	void fatal(std::format_string<Args...> fmt, Args&&... args)
	{
		logger::get().log(severity::fatal, std::format(fmt, std::forward<Args>(args)...));
	}
}
