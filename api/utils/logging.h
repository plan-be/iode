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
    std::filesystem::path root_dir;
    std::string path_log_file_str;

public:
    Logging(LogLevel level = LOG_NONE) : level(level)
    {
        std::string current_file = __FILE__;
        std::filesystem::path current_path(current_file);
        std::filesystem::path utils_dir = current_path.parent_path();
        std::filesystem::path api_dir = utils_dir.parent_path();
        root_dir = api_dir.parent_path();
        set_log_filename();
    }

    ~Logging() 
    {
        if(log_file.is_open())
            log_file.close();
    }

    void set_log_filename(const std::string& filename="debug.txt", const bool overwrite=true)
    {
        std::filesystem::path new_path_log_file = root_dir / filename;
        if(new_path_log_file.string() == path_log_file_str)
            return;

        // delete log file if it already exists
        if(overwrite && std::filesystem::exists(new_path_log_file))
            std::filesystem::remove(new_path_log_file);

        // update log filepath
        path_log_file_str = new_path_log_file.string();

        // close current log file
        if(log_file.is_open())
            log_file.close();

        // open log file if level is not LOG_NONE 
        if(level != LOG_NONE)
            log_file = std::ofstream(path_log_file_str, std::ios::out);
    }

    void set_level(LogLevel new_level) 
    {
        if(new_level == LOG_NONE && log_file.is_open())
            log_file.close();

        if(new_level != LOG_NONE && !log_file.is_open())
            log_file = std::ofstream(path_log_file_str, std::ios::app);

        level = new_level;
    }

    bool is_enabled() const 
    {
        return level != LOG_NONE;
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
