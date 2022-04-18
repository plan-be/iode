#pragma once

#include "common.h"

#include <string>
#include <stdexcept>
// requires C++17 
#include <filesystem>

#ifdef _WIN32 // valid for both 32 and 64 bits
    #include <Windows.h>
#else
    #define CP_OEMCP -1
#endif


/* ****************************** *
 *      UTILITY FUNCTIONS         *
 * ****************************** */


// see http://www.cplusplus.com/forum/general/245426/
inline std::string convert_between_codepages(const std::string str_in, const int codepage_in, const int codepage_out)
{
    if (str_in.empty()) return str_in;
    if (codepage_in <= 0) throw std::runtime_error("Parameter codepage_in must be positive.");
    if (codepage_out <= 0) throw std::runtime_error("Parameter codepage_out must be positive.");

    const char* c_in = str_in.c_str();
    // add 1 because strlen doesn't count the ending \0 character
    int c_in_length = static_cast<int>(strlen(c_in)) + 1;
    // MultiByteToWideChar: https://docs.microsoft.com/fr-fr/windows/win32/api/stringapiset/nf-stringapiset-multibytetowidechar
    const int wchar_length = MultiByteToWideChar(codepage_in, 0, c_in, c_in_length, NULL, 0);
    wchar_t* wchar_array = new wchar_t[wchar_length];
    MultiByteToWideChar(codepage_in, 0, c_in, c_in_length, wchar_array, wchar_length);

    int c_out_length = WideCharToMultiByte(codepage_out, 0, wchar_array, wchar_length, NULL, 0, NULL, FALSE);
    // Warning: some special characters take 2 chars to be stored.
    //          In other words, s_out_length may be greater to s_in_length.
    //          This is why a new char array must be allocated and returned.
    char* c_out = new char[c_out_length];
    // WideCharToMultiByte: https://docs.microsoft.com/fr-fr/windows/win32/api/stringapiset/nf-stringapiset-widechartomultibyte
    WideCharToMultiByte(codepage_out, 0, wchar_array, wchar_length, c_out, c_out_length, NULL, FALSE);
    std::string str_out(c_out);

    delete[] wchar_array;
    delete[] c_out;

    return str_out;
}

inline std::string oem_to_utf8(const std::string str_oem)
{
    return convert_between_codepages(str_oem, CP_OEMCP, CP_UTF8);
}

inline std::string utf8_to_oem(const std::string str_utf8)
{
    return convert_between_codepages(str_utf8, CP_UTF8, CP_OEMCP);
}


inline std::string check_filepath(std::string& filepath, const EnumIodeType type, const std::string& caller_name, const bool file_must_exist)
{
    std::filesystem::path p_filepath(filepath);
    std::string error_msg = "Call to " + caller_name + "() failed. ";

    // convert to absolute path
    if (p_filepath.is_relative()) p_filepath = std::filesystem::absolute(p_filepath);

    // check if parent directory exist
    std::filesystem::path p_directory = p_filepath.parent_path();
    if (!p_directory.empty())
    {
        if (!std::filesystem::exists(p_directory)) throw std::runtime_error(error_msg + "Directory " + p_directory.string()
            + " does not exist. Could not " + caller_name + " file " + p_filepath.filename().string());
    }

    // check or add extension
    std::filesystem::path p_filename = p_filepath.filename();
    IodeFileExtension expected_ext = vFileExtensions[type];
    if (p_filepath.has_extension())
    {
        // check extension
        std::string ext = p_filename.extension().string();
        if ((ext != expected_ext.ext) && (ext != expected_ext.ascii)) throw std::runtime_error(error_msg + "File " + p_filename.string()
            + " has wrong extension " + ext + ". Expected extension is " + expected_ext.ext + " or " + expected_ext.ascii);

        // check if file exist
        if (file_must_exist && !std::filesystem::exists(p_filepath)) throw std::runtime_error(error_msg + "File " + p_filepath.string() + " does not exist");
    }
    else
    {
        // set binary format extension
        p_filepath = p_filepath.replace_extension(expected_ext.ext);

        // check if file exist
        if (file_must_exist)
        {
            // check first with binary format extension
            bool binary_file_found = std::filesystem::exists(p_filepath);
            // switch to ascii format extension and check if file exist 
            if (!binary_file_found)
            {
                p_filepath = p_filepath.replace_extension(expected_ext.ascii);
                if (!std::filesystem::exists(p_filepath)) 
                {
                    std::string stem = p_filepath.stem().string();
                    throw std::runtime_error(error_msg + "Neither " + stem + expected_ext.ext + " nor " + stem + expected_ext.ascii
                        + " could be found in directory " + p_directory.string());
                }
            }
        }
    }

    return p_filepath.string();
}