#pragma once

#include "includes.hpp"

namespace logging
{
        inline void save_log(const std::string& log)
        {
                if(config::LOG_FILE_NAME == ""){ config::LOG_FILE_NAME = "my-log-file"; if(config::LOG_FILE_EXTENSION == ""){ config::LOG_FILE_EXTENSION = ".log"; }};
                std::ofstream log_file(config::LOG_FILE_NAME + config::LOG_FILE_EXTENSION, std::ios::app);
                log_file << log << '\n';
                log_file.close();
        }
}