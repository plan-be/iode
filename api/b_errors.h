#pragma once

#include "api/constants.h"
#include "api/k_super.h"
#include "api/write/write.h"

#include <string>
#include <vector>


class IodeErrorManager
{
    inline static std::vector<std::string> B_ERROR_MSG;
    inline const static std::string B_ERROR_DFT_MSG = "Unknown error";

    // Private constructor to prevent instantiation
    IodeErrorManager() = default;
    // Delete copy constructor and assignment operator
    IodeErrorManager(const IodeErrorManager&) = delete;
    IodeErrorManager& operator=(const IodeErrorManager&) = delete;

public:
    static IodeErrorManager& instance()
    {
        static IodeErrorManager instance;
        return instance;
    }

    void append_error(const std::string& msg)
    {
        if(msg.empty()) 
            return;

        if(std::find(B_ERROR_MSG.begin(), B_ERROR_MSG.end(), msg) != B_ERROR_MSG.end())
            return; // error message already present

        B_ERROR_MSG.push_back(msg);
    }

    void display_last_error()
    {
        if(B_ERROR_MSG.empty()) 
            return;

        std::string errors = "errors:\n";
        for(const auto& msg : B_ERROR_MSG)
            errors += msg + "\n";
        kerror(0, errors.c_str());
        kpause();
        B_ERROR_MSG.clear();
    }

    std::string get_last_error()
    {
        if(B_ERROR_MSG.empty())
            return "";
        
        std::string errors = "errors:\n";
        for(const auto& msg : B_ERROR_MSG)
            errors += msg + "\n";
        B_ERROR_MSG.clear();
        return errors;
    }

    void print_last_error()
    {
        if(B_ERROR_MSG.empty()) 
            return;

        for(const std::string& msg : B_ERROR_MSG) 
            W_printf((char*) "%s\n", msg.c_str());

        B_ERROR_MSG.clear();
    }

    void clear()
    {
        B_ERROR_MSG.clear();
    }
};

// Global variable holding the singleton instance
inline IodeErrorManager& error_manager = IodeErrorManager::instance();
