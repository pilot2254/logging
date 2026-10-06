#pragma once

#include <concepts>
#include <filesystem>
#include <format>
#include <iostream>
#include <mutex>
#include <source_location>
#include <string>
#include <string_view>
#include <type_traits>

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

		void set_show_location(bool show)
		{
			std::lock_guard lock(m_mutex);
			m_show_location = show;
		}

		void log(severity s, const std::string& message, const std::source_location& loc = std::source_location::current())
		{
			std::lock_guard lock(m_mutex);
			if (s < m_min) return;

			std::string line = std::format("[{}] [{}]", GetTime(), SeverityToString(s));

			if (m_show_location)
			{
				const std::string name = std::filesystem::path(loc.file_name()).filename().string();
				line += std::format(" [{}:{}]", name, loc.line());
			}

			line += ": " + message;

			std::cout << color(s) << line << termcolor::reset << '\n';

			m_file.write(line);
		}

	private:
		logger() { m_file.open("log.txt"); }

		//termcolor for each severity
		static std::ostream& (*color(severity s))(std::ostream&)
		{
			switch (s)
			{
				case severity::info:    return termcolor::green;
				case severity::debug:   return termcolor::cyan;
				case severity::warning:	return termcolor::yellow;
				case severity::error:   return termcolor::red;
				case severity::fatal:   return termcolor::on_red;
				default:                return termcolor::white;
			}
		}

		std::mutex m_mutex;
		file       m_file;
		severity   m_min = severity::none;
		bool       m_show_location = true;
	};

	//bundles the format string with the place it was called from
	//(a default argument cant come after "Args..." so i hide it in here)
	template <typename... Args>
	struct format_loc
	{
		std::format_string<Args...> fmt;
		std::source_location        loc;

		template <typename T> requires std::convertible_to<const T&, std::string_view>
		consteval format_loc(const T& f, std::source_location l = std::source_location::current()) : fmt(f), loc(l) {}
	};

	//easy helpers: logging::info("x = {}", 5);
	template <typename... Args>
	void info(format_loc<std::type_identity_t<Args>...> f, Args&&... args)
	{
		logger::get().log(severity::info, std::format(f.fmt, std::forward<Args>(args)...), f.loc);
	}

	template <typename... Args>
	void debug(format_loc<std::type_identity_t<Args>...> f, Args&&... args)
	{
		logger::get().log(severity::debug, std::format(f.fmt, std::forward<Args>(args)...), f.loc);
	}

	template <typename... Args>
	void warning(format_loc<std::type_identity_t<Args>...> f, Args&&... args)
	{
		logger::get().log(severity::warning, std::format(f.fmt, std::forward<Args>(args)...), f.loc);
	}

	template <typename... Args>
	void error(format_loc<std::type_identity_t<Args>...> f, Args&&... args)
	{
		logger::get().log(severity::error, std::format(f.fmt, std::forward<Args>(args)...), f.loc);
	}

	template <typename... Args>
	void fatal(format_loc<std::type_identity_t<Args>...> f, Args&&... args)
	{
		logger::get().log(severity::fatal, std::format(f.fmt, std::forward<Args>(args)...), f.loc);
	}
}
