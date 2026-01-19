#pragma once

#include <fstream>      // for std::ofstream
#include <filesystem>   // for std::filesystem::path


enum LogLevel
{
    LOG_NONE,
    LOG_INFO,
    LOG_WARNING,
    LOG_ERROR
};


class Logging 
{
    LogLevel level;
    std::ofstream log_file;
    std::string path_log_file_str;

public:
    Logging(LogLevel level = LOG_NONE) : level(level)
    {
        std::string current_file = __FILE__;
        std::filesystem::path current_path(current_file);
        std::filesystem::path utils_dir = current_path.parent_path();
        std::filesystem::path api_dir = utils_dir.parent_path();
        std::filesystem::path root_dir = api_dir.parent_path();
        std::filesystem::path path_log_file = root_dir / "debug.txt";
        
        // delete existing log file
        if(std::filesystem::exists(path_log_file))
            std::filesystem::remove(path_log_file);
        
        // open log file if level is not LOG_NONE 
        path_log_file_str = path_log_file.string();
        if(level != LOG_NONE)
            log_file = std::ofstream(path_log_file_str, std::ios::out);
    }

    ~Logging() 
    {
        if(log_file.is_open())
            log_file.close();
    }

    void set_level(LogLevel new_level) 
    {
        if(new_level == LOG_NONE && log_file.is_open())
            log_file.close();

        if(new_level != LOG_NONE && !log_file.is_open())
            log_file = std::ofstream(path_log_file_str, std::ios::app);

        level = new_level;
    }

    void info(const std::string& message) 
    {
        if(log_file.is_open() && level == LOG_INFO)
            log_file << "DEBUG - " <<  message << std::endl;
    }

    void warning(const std::string& message) 
    {
        if(log_file.is_open() && level != LOG_NONE && level <= LOG_WARNING)
            log_file << "WARNING - " <<  message << std::endl;
    }

    void error(const std::string& message) 
    {
        if(log_file.is_open() && level != LOG_NONE && level <= LOG_ERROR)
            log_file << "ERROR - " <<  message << std::endl;
    }
};

inline Logging logger;
