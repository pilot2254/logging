#pragma once

#include <concepts>
#include <cstdlib>
#include <exception>
#include <filesystem>
#include <format>
#include <functional>
#include <iostream>
#include <memory>
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
		//the instance is never destroyed on purpose, so logging from a static destructor
		//(which runs after main) is always safe. whatever is still buffered gets flushed at exit
		static logger& get()
		{
			static logger* instance = new logger;
			return *instance;
		}

		bool set_file(const std::string& path)
		{
			std::lock_guard lock(m_mutex);
			m_file_tried = true;
			return m_file.open(path);
		}

		void set_log_to_file(bool enable)
		{
			std::lock_guard lock(m_mutex);
			m_log_to_file = enable;
		}

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

		void set_use_stderr(bool use)
		{
			std::lock_guard lock(m_mutex);
			m_use_stderr = use;
		}

		//a sink gets every line that passes the min severity (plain text, no colors)
		//it gets called while the logger is locked, so calling logging::info() etc inside a sink does nothing
		//changing settings or adding/removing sinks from inside a sink is fine
		using sink = std::function<void(severity, const std::string&)>;

		void add_sink(sink s)
		{
			std::lock_guard lock(m_mutex);
			//copy on write, so a sink can add/clear sinks while the list is being walked
			auto copy = std::make_shared<std::vector<sink>>(*m_sinks);
			copy->push_back(std::move(s));
			m_sinks = std::move(copy);
		}

		void clear_sinks()
		{
			std::lock_guard lock(m_mutex);
			m_sinks = std::make_shared<std::vector<sink>>();
		}

		//flushes everything and calls std::abort() right after a fatal message (off by default)
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

		void flush()
		{
			std::lock_guard lock(m_mutex);
			m_file.flush();
		}

		void set_show_pid(bool show)
		{
			std::lock_guard lock(m_mutex);
			m_show_pid = show;
		}

		void set_use_color(bool use)
		{
			std::lock_guard lock(m_mutex);
			m_use_color = use;
		}

		//never throws, a logger should not be the thing that crashes your program
		void log(severity s, const std::string& message, const std::source_location& loc = std::source_location::current()) noexcept
		{
			try { write(s, message, loc); }
			catch (...) {}
		}

	private:
		logger() : m_pid(LOGGING_GETPID())
		{
			std::atexit(&logger::at_exit);
		}

		//since the instance is never destroyed we flush by hand when the program exits
		//anything logged after this (static destructors) gets flushed right away
		static void at_exit() noexcept
		{
			try
			{
				logger& self = get();
				std::lock_guard lock(self.m_mutex);
				self.m_exiting = true;
				self.m_file.flush();
			}
			catch (...) {}
		}

		void write(severity s, const std::string& message, const std::source_location& loc)
		{
			if (in_sink()) return;

			std::unique_lock lock(m_mutex);
			if (s < m_min) return;

			std::string prefix;

			if (m_show_time) prefix += std::format(" [{}]", current_time());

			if (m_show_severity) prefix += std::format(" [{}]", severity_to_string(s));

			if (m_show_pid) prefix += std::format(" [pid:{}]", m_pid);

			if (m_show_location)
			{
				const std::string name = std::filesystem::path(loc.file_name()).filename().string();
				prefix += std::format(" [{}:{}]", name, loc.line());
			}

			std::string line = prefix.empty() ? message : prefix.substr(1) + ": " + message;

			std::ostream& out = (m_use_stderr && s >= severity::error) ? std::cerr : std::cout;

			if (m_use_color) out << color(s) << line << termcolor::reset << '\n';
			else out << line << '\n';

			if (m_log_to_file)
			{
				if (!m_file_tried)
				{
					m_file_tried = true;
					m_file.open("log.txt");
				}

				m_file.write(line, m_auto_flush || m_exiting || s >= severity::warning);
			}

			//walk a snapshot, so a sink changing the sink list cant break the loop
			const auto sinks = m_sinks;
			{
				sink_guard guard;
				for (const auto& fn : *sinks)
				{
					try { fn(s, line); }
					catch (...) {} //a broken sink shouldnt take the program down
				}
			}

			if (m_abort_on_fatal && s == severity::fatal)
			{
				//abort() doesnt flush anything, so do it ourselves or the last lines get lost when stdout is redirected
				m_file.flush();
				std::cout.flush();
				std::cerr.flush();
				lock.unlock();
				std::abort();
			}
		}

		static bool& in_sink()
		{
			static thread_local bool flag = false;
			return flag;
		}

		//sets the flag and always clears it again, even if something throws
		struct sink_guard
		{
			sink_guard() { in_sink() = true; }
			~sink_guard() { in_sink() = false; }
			sink_guard(const sink_guard&) = delete;
			sink_guard& operator=(const sink_guard&) = delete;
		};

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

		//recursive so a sink can call the set_* functions / add_sink / clear_sinks without deadlocking
		std::recursive_mutex m_mutex;
		file       m_file;
		int        m_pid;
		severity   m_min = severity::debug;
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
		bool       m_exiting = false;
		std::shared_ptr<const std::vector<sink>> m_sinks = std::make_shared<const std::vector<sink>>();
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

	namespace detail
	{
		//formats the message and logs it. std::format can throw (a broken formatter, out of memory)
		//so a bad format never escapes into the caller
		template <typename... Args>
		void emit(severity s, format_loc<std::type_identity_t<Args>...> f, Args&&... args) noexcept
		{
			try
			{
				std::string message;

				try { message = std::format(f.fmt, std::forward<Args>(args)...); }
				catch (const std::exception& e) { message = std::string("<log formatting failed: ") + e.what() + ">"; }

				logger::get().log(s, message, f.loc);
			}
			catch (...) {}
		}
	}

	template <typename... Args>
	void info(format_loc<std::type_identity_t<Args>...> f, Args&&... args)
	{
		detail::emit(severity::info, f, std::forward<Args>(args)...);
	}

	template <typename... Args>
	void success(format_loc<std::type_identity_t<Args>...> f, Args&&... args)
	{
		detail::emit(severity::success, f, std::forward<Args>(args)...);
	}

	template <typename... Args>
	void debug(format_loc<std::type_identity_t<Args>...> f, Args&&... args)
	{
		detail::emit(severity::debug, f, std::forward<Args>(args)...);
	}

	template <typename... Args>
	void warning(format_loc<std::type_identity_t<Args>...> f, Args&&... args)
	{
		detail::emit(severity::warning, f, std::forward<Args>(args)...);
	}

	template <typename... Args>
	void error(format_loc<std::type_identity_t<Args>...> f, Args&&... args)
	{
		detail::emit(severity::error, f, std::forward<Args>(args)...);
	}

	template <typename... Args>
	void fatal(format_loc<std::type_identity_t<Args>...> f, Args&&... args)
	{
		detail::emit(severity::fatal, f, std::forward<Args>(args)...);
	}
}
