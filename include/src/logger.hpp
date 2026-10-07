#pragma once

#include <concepts>
#include <cstdlib>
#include <filesystem>
#include <format>
#include <functional>
#include <iostream>
#include <mutex>
#include <source_location>
#include <string>
#include <string_view>
#include <type_traits>
#include <vector>

#ifdef _WIN32
#include <process.h>
#define LOGGING_GETPID() _getpid()
#else
#include <unistd.h>
#define LOGGING_GETPID() getpid()
#endif

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

		//change the log file (default is log.txt)
		bool set_file(const std::string& path)
		{
			std::lock_guard lock(m_mutex);
			m_file_tried = true;
			return m_file.open(path);
		}

		//turn writing to the log file on/off, when its off log.txt never gets created
		void set_log_to_file(bool enable)
		{
			std::lock_guard lock(m_mutex);
			m_log_to_file = enable;
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

		void set_show_time(bool show)
		{
			std::lock_guard lock(m_mutex);
			m_show_time = show;
		}

		void set_show_severity(bool show)
		{
			std::lock_guard lock(m_mutex);
			m_show_severity = show;
		}

		//error and fatal go to stderr instead of stdout
		void set_use_stderr(bool use)
		{
			std::lock_guard lock(m_mutex);
			m_use_stderr = use;
		}

		//a sink gets every line that passes the min severity (plain text, no colors)
		//it gets called while the logger is locked, so calling logging::info() etc inside a sink does nothing
		using sink = std::function<void(severity, const std::string&)>;

		void add_sink(sink s)
		{
			std::lock_guard lock(m_mutex);
			m_sinks.push_back(std::move(s));
		}

		void clear_sinks()
		{
			std::lock_guard lock(m_mutex);
			m_sinks.clear();
		}

		//flushes the file and calls std::abort() right after a fatal message (off by default)
		void set_abort_on_fatal(bool enable)
		{
			std::lock_guard lock(m_mutex);
			m_abort_on_fatal = enable;
		}

		//true = flush the file after every line (slow if you log a lot)
		//false = only flush on warning and above, so errors still make it to disk if the program crashes
		void set_auto_flush(bool enable)
		{
			std::lock_guard lock(m_mutex);
			m_auto_flush = enable;
		}

		//force everything to disk right now
		void flush()
		{
			std::lock_guard lock(m_mutex);
			m_file.flush();
		}

		//adds [pid:1234] to every line
		void set_show_pid(bool show)
		{
			std::lock_guard lock(m_mutex);
			m_show_pid = show;
		}

		//colors only affect the console, the log file is always plain text
		void set_use_color(bool use)
		{
			std::lock_guard lock(m_mutex);
			m_use_color = use;
		}

		void log(severity s, const std::string& message, const std::source_location& loc = std::source_location::current())
		{
			if (in_sink()) return;

			std::unique_lock lock(m_mutex);
			if (s < m_min) return;

			std::string prefix;

			if (m_show_time)
				prefix += std::format(" [{}]", current_time());

			if (m_show_severity)
				prefix += std::format(" [{}]", severity_to_string(s));

			if (m_show_pid)
				prefix += std::format(" [pid:{}]", m_pid);

			if (m_show_location)
			{
				const std::string name = std::filesystem::path(loc.file_name()).filename().string();
				prefix += std::format(" [{}:{}]", name, loc.line());
			}

			//everything above starts with a space, so cut that off
			std::string line = prefix.empty() ? message : prefix.substr(1) + ": " + message;

			std::ostream& out = (m_use_stderr && s >= severity::error) ? std::cerr : std::cout;

			if (m_use_color)
				out << color(s) << line << termcolor::reset << '\n';
			else
				out << line << '\n';

			if (m_log_to_file)
			{
				//only create the file the first time we actually need it
				if (!m_file_tried)
				{
					m_file_tried = true;
					m_file.open("log.txt");
				}

				m_file.write(line, m_auto_flush || s >= severity::warning);
			}

			in_sink() = true;
			for (auto& fn : m_sinks)
			{
				try { fn(s, line); }
				catch (...) {} //a broken sink shouldnt take the program down
			}
			in_sink() = false;

			if (m_abort_on_fatal && s == severity::fatal)
			{
				m_file.flush();
				lock.unlock(); //dont die while holding the mutex
				std::abort();
			}
		}

	private:
		logger() : m_pid(LOGGING_GETPID()) {}

		static bool& in_sink()
		{
			static thread_local bool flag = false;
			return flag;
		}

		//termcolor for each severity
		static std::ostream& (*color(severity s))(std::ostream&)
		{
			switch (s)
			{
				case severity::success: return termcolor::green;
				case severity::debug:   return termcolor::cyan;
				case severity::warning:	return termcolor::yellow;
				case severity::error:   return termcolor::red;
				case severity::fatal:   return termcolor::on_red;
				default:                return termcolor::white;
			}
		}

		std::mutex m_mutex;
		file       m_file;
		int        m_pid;
		severity   m_min = severity::debug; //show everything by default
		bool       m_show_time = true;
		bool       m_show_severity = true;
		bool       m_show_location = true;
		bool       m_show_pid = false;
		bool       m_use_color = true;
		bool       m_use_stderr = true;
		bool       m_log_to_file = true;
		bool       m_file_tried = false;
		bool       m_auto_flush = false;
		bool       m_abort_on_fatal = false;
		std::vector<sink> m_sinks;
	};



	// :)



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
	void success(format_loc<std::type_identity_t<Args>...> f, Args&&... args)
	{
		logger::get().log(severity::success, std::format(f.fmt, std::forward<Args>(args)...), f.loc);
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