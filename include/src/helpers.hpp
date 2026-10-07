#pragma once

#include <concepts>
#include <exception>
#include <format>
#include <source_location>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>

#include "logger.hpp"
#include "severity.hpp"

namespace logging
{
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
