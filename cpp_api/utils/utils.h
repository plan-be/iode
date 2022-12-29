#pragma once

#include "common.h"
#include "iode_exceptions.h"
#include "super.h"

#include <string>
#include <vector>
#include <regex>
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
    IodeExceptionInvalidArguments error("Cannot convert string from codepage " + std::to_string(codepage_in) + 
                                        " to codepage " + std::to_string(codepage_out));
    if (codepage_in <= 0) error.add_argument("codepage_in", std::to_string(codepage_in) + " (must be positive)");
    if (codepage_out <= 0) error.add_argument("codepage_out", std::to_string(codepage_out) + " (must be positive)");
    if (error.invalid_args()) throw error;

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

static IodeRegexName get_regex_name(const EnumIodeType type)
{
    IodeRegexName nre;
    if (type == I_COMMENTS)
    {
        nre.regex = "A-Za-z";
        nre.type = "capital or lowercase";
    }
    else if (type == I_SCALARS)
    {
        nre.regex = "a-z";
        nre.type = "lowercase";
    }
    else
    {
        nre.regex = "A-Z";
        nre.type = "capital";
    }

    // see https://www.cplusplus.com/reference/regex/ECMAScript for regex syntax
    nre.regex = "[" + nre.regex + "_][" + nre.regex + "0-9_#]{0," + std::to_string(K_MAX_NAME - 1) + "}";

    return nre;
}

inline void check_name(const std::string name, const EnumIodeType type)
{
    std::string msg = "name " + name + " as " + vIodeTypes[type];

    if (name.empty()) throw IodeExceptionFunction("Cannot accept " + msg, "Empty name");

    if (name.size() > K_MAX_NAME) throw IodeExceptionFunction("Cannot accept " + msg,  
        "Iode names cannot exceed " + std::to_string(K_MAX_NAME) + " characters. " + name + 
        " = " + std::to_string(name.size()) + " characters.");

    IodeRegexName nre = get_regex_name(type);
    if (!regex_match(name, std::regex(nre.regex))) throw IodeExceptionFunction("Cannot accept " + msg, 
        vIodeTypes[type] + " name must only contains " + nre.type + " letters, digits and underscore. " + 
        name + " is invalid.");
}

inline static std::filesystem::path check_file(const std::string& filepath, const std::string& caller_name, const bool file_must_exist)
{
    std::filesystem::path p_filepath(filepath);

    // check if empty
    if (file_must_exist && filepath.empty()) throw IodeExceptionFunction("Cannot run " + caller_name, "Empty filepath");

    // convert to absolute path
    if (p_filepath.is_relative()) p_filepath = std::filesystem::absolute(p_filepath);

    // check if parent directory exist
    std::filesystem::path p_directory = p_filepath.parent_path();
    if (!p_directory.empty())
    {
        if (!std::filesystem::exists(p_directory)) throw IodeExceptionFunction("Cannot run " + caller_name, "Directory " + 
            p_directory.string() + " in filepath " + p_filepath.filename().string() + " does not exist");
    }

    return p_filepath; 
}

inline std::string check_file_exists(const std::string& filepath, const std::string& caller_name)
{
    std::filesystem::path p_filepath = check_file(filepath, caller_name, true);
    // check if file exist
    if (!std::filesystem::exists(p_filepath)) 
        throw IodeExceptionFunction("Cannot run " + caller_name, "File " + p_filepath.string() + " does not exist");

    return p_filepath.string();
}

inline std::string check_filepath(const std::string& filepath, const EnumIodeFile type, const std::string& caller_name, const bool file_must_exist)
{
    std::filesystem::path p_filepath = check_file(filepath, caller_name, file_must_exist);

    // check or add extension
    std::filesystem::path p_filename = p_filepath.filename();
    IodeFileExtension expected_ext = vFileExtensions[type];
    if (p_filepath.has_extension())
    {
        // check extension
        std::string ext = p_filename.extension().string();
        if ((ext != expected_ext.ext) && (ext != expected_ext.ascii)) throw IodeExceptionFunction("Cannot run " + caller_name, 
            "File " + p_filename.string() + " has wrong extension " + ext + ". Expected extension is " + 
            expected_ext.ext + " or " + expected_ext.ascii);

        // check if file exist
        if (file_must_exist && !std::filesystem::exists(p_filepath)) throw IodeExceptionFunction("Cannot run " + caller_name, 
            "File " + p_filepath.string() + " does not exist");
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
                    std::filesystem::path p_directory = p_filepath.parent_path();
                    std::string stem = p_filepath.stem().string();
                    throw IodeExceptionFunction("Cannot run " + caller_name, "Neither " + stem + expected_ext.ext + 
                        " nor " + stem + expected_ext.ascii + " could be found in directory " + p_directory.string());
                }
            }
        }
    }

    return p_filepath.string();
}

inline void remove_duplicates(std::vector<std::string>& v)
{
    // NOTE: std::unique only removes consecutive duplicated elements, 
    //       so the vector needst to be sorted first
    std::sort(v.begin(), v.end());
    std::vector<std::string>::iterator it = std::unique(v.begin(), v.end());  
    v.resize(std::distance(v.begin(), it));
}

// QUESTION FOR JMP: is there a function in the C Iode API to remove duplicates 
//                   from a char** ?
inline char** remove_duplicates(char** items)
{
    int nb_items = SCR_tbl_size((unsigned char**) items);
    std::vector<std::string> v_items;
    v_items.reserve(nb_items);
    for (int i=0; i < nb_items; i++) v_items.push_back(std::string(items[i]));
    remove_duplicates(v_items);
    SCR_free_tbl((unsigned char**) items);
    
    nb_items = v_items.size();
    items = new char*[nb_items+1];
    for (int i=0; i < nb_items; i++)
    {
        items[i] = new char[K_MAX_NAME+1];
        strcpy(items[i], v_items[i].c_str());
    }
    items[nb_items] = 0;
    return items;
}

inline char* copy_char_array(char* source_str)
{
    if(source_str == 0) return source_str;
    size_t size = strlen(source_str) + 1;
    char* dest_str = new char[size];
    strncpy(dest_str, source_str, size);
    return dest_str;
}

inline char* to_char_array(const std::string& str)
{
    return const_cast<char*>(str.c_str());
}

inline char* copy_string_to_char(const std::string& str)
{
    if(str.empty()) return NULL;
    size_t size = str.size() + 1;
    char* c_string = new char[size];
    strncpy(c_string, str.c_str(), size);
    return c_string;
}
