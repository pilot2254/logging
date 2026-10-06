#pragma once

#include <fstream>
#include <string>

namespace logging
{
	class file
	{
	public:
		bool open(const std::string& path)
		{
			if (m_stream.is_open()) m_stream.close();
			m_stream.open(path, std::ios::app);
			return m_stream.is_open();
		}

		bool is_open() const { return m_stream.is_open(); }

		void write(const std::string& line)
		{
			if (m_stream.is_open()) m_stream << line << '\n' << std::flush;
		}

	private:
		std::ofstream m_stream;
	};
}
