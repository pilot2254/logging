#pragma once

#include "includes.hpp"

namespace logging
{
        inline void log(const std::string& message)
        {
                std::string content = "[ ] " + message;
                std::cout << content << '\n';
                save_log(content);
                return;
        }

        inline void warn(const std::string& message)
        {
                std::string content = "[!] " + message;
                std::cout << content << '\n';
                save_log(content);
                return;
        }

        inline void error(const std::string& message)
        {
                std::string content = "[-] " + message;
                std::cout << content << '\n';
                save_log(content);
                return;
        }

        inline void success(const std::string& message)
        {
                std::string content = "[+] " + message;
                std::cout << content << '\n';
                save_log(content);
                return;
        }
}