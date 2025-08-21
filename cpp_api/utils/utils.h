#pragma once

#include "cpp_api/common.h"
#include "super.h"

#include <regex>
#include <string>
#include <vector>
#include <cctype>
#include <numeric>      // for std::accumulate
#include <sstream>
#include <iostream>
#include <stdexcept>
#include <functional>   // for std::hash
#include <filesystem>   // requires C++17 
#include <string_view>  // requires C++17


#ifdef _MSC_VER // valid for both 32 and 64 bits
    #include <Windows.h>
#else
    #include <iconv.h>
    #define CP_OEMCP -1
#endif


/* ****************************** *
 *      UTILITY FUNCTIONS         *
 * ****************************** */


// see http://www.cplusplus.com/forum/general/245426/
inline std::string convert_between_codepages(const std::string& str_in, const bool oem_to_utf8)
{
    if(str_in.empty()) 
        return str_in;

    char* c_in = const_cast<char*>(str_in.data());

#ifdef _MSC_VER
    const int codepage_in = oem_to_utf8 ? CP_OEMCP : CP_UTF8;
    const int codepage_out = oem_to_utf8 ? CP_UTF8 : CP_OEMCP;

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
    delete[] wchar_array;

    std::string str_out(c_out);
    delete[] c_out;

    return str_out;
#else
    const char* fromcode = oem_to_utf8 ? "CP850" : "UTF-8";
    const char* tocode = oem_to_utf8 ? "UTF-8" : "CP850";
    iconv_t conversion_descriptor = iconv_open(tocode, fromcode);
    
    size_t inbytesleft = strlen(c_in);
    // Allocate enough space for worst-case scenario
    size_t outbytesleft = inbytesleft * 4;
    std::string str_out(outbytesleft, '\0');
    char* c_out = &str_out[0];
    size_t result = iconv(conversion_descriptor, &c_in, &inbytesleft, &c_out, &outbytesleft);
    if (result == (size_t)-1) 
    {
        iconv_close(conversion_descriptor);
        throw std::runtime_error("Failed to convert string " + str_in);
    }

    // Resize the output string to the actual size of the converted data
    str_out.resize(str_out.size() - outbytesleft);
    
    iconv_close(conversion_descriptor);

    return str_out;
#endif
}

inline std::string oem_to_utf8(const std::string str_oem)
{
    return convert_between_codepages(str_oem, true);
}

inline std::string utf8_to_oem(const std::string str_utf8)
{
    return convert_between_codepages(str_utf8, false);
}

/**
 * @brief Joins the elements of a vector of strings into a single string using a specified separator.
 * 
 * @param vec The vector of strings to be joined.
 * @param separator The string used to separate each element in the final concatenated string.
 * @return The concatenated string formed by joining all elements of the input vector with the separator.
 */
inline std::string join(const std::vector<std::string>& vec, const std::string& separator) 
{
    if (vec.empty()) 
        return "";

    return std::accumulate(std::next(vec.begin()), vec.end(), vec[0],
        [&separator](const std::string& a, const std::string& b) { return a + separator + b; });
}

/**
 * @brief Splits a given string into substrings based on a specified delimiter.
 * 
 * @param str The string to be split.
 * @param delimiter The character used to identify where the string should be split.
 * @return A vector of strings containing the substrings after splitting the input string.
 */
inline std::vector<std::string> split(const std::string& str, char delimiter) 
{
    std::vector<std::string> result;
    std::string_view str_view(str);
    std::size_t start = 0;
    std::size_t end = 0;

    while ((end = str_view.find(delimiter, start)) != std::string_view::npos) 
    {
        result.emplace_back(str_view.substr(start, end - start));
        start = end + 1;
    }
    result.emplace_back(str_view.substr(start));

    return result;
}

/**
 * @brief trim leading and trailing whitespace of a string
 * 
 * @param str 
 * @return std::string 
 */
inline std::string trim(const std::string& str) 
{
    auto start = str.find_first_not_of(" \t\n\r\f\v");
    auto end = str.find_last_not_of(" \t\n\r\f\v");

    // String is all whitespace
    if (start == std::string::npos || end == std::string::npos)
        return "";

    return str.substr(start, end - start + 1);
}


static IodeRegexName get_regex_name(const int type)
{
    IodeRegexName nre;
    if (type == COMMENTS)
    {
        nre.regex = "A-Za-z";
        nre.type = "capital or lowercase";
    }
    else if (type == SCALARS)
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

inline void check_name(const std::string name, const int type)
{
    std::string msg = "name \"" + name + "\" as " + v_iode_types[type];

    if(name.empty()) 
        throw std::invalid_argument("Cannot accept " + msg + ": Empty name");

    if(name.size() > K_MAX_NAME) 
        throw std::invalid_argument("Cannot accept " + msg + ": " + 
        "Iode names cannot exceed " + std::to_string(K_MAX_NAME) + " characters. " + name + 
        " = " + std::to_string(name.size()) + " characters");

    IodeRegexName nre = get_regex_name(type);
    if(!regex_match(name, std::regex(nre.regex))) 
        throw std::invalid_argument("Cannot accept " + msg + ": " + 
        v_iode_types[type] + " name must only contains " + nre.type + " letters, digits and underscore");
}

inline IodeFileType get_iode_file_type(const std::string& filepath)
{
    if (filepath.empty()) return FILE_ANY;

    std::filesystem::path p_filepath(filepath);
    if (std::filesystem::is_directory(p_filepath)) return DIRECTORY;
    if (!p_filepath.has_extension()) return FILE_ANY;

    std::string ext = p_filepath.extension().string();

    for(int i=0; i < v_file_types.size(); i++)
        for(const std::string& file_ext: v_file_types[i].v_ext)
            if(ext == file_ext)
                return (IodeFileType) i;

    return FILE_ANY;
}

inline int get_iode_type(const std::string& iode_type_as_string)
{
    for(int i=0; i < IODE_NB_TYPES; i++) 
        if(iode_type_as_string == v_iode_types[i]) 
            return i;
    return -1;
}

inline static std::filesystem::path check_file(const std::string& filepath, const std::string& caller_name, 
    const bool file_must_exist)
{
    std::filesystem::path p_filepath(filepath);

    // check if empty
    if (file_must_exist && filepath.empty()) 
        throw std::invalid_argument("Cannot run '" + caller_name + "'.\nEmpty filepath");

    // convert to absolute path
    if (p_filepath.is_relative()) p_filepath = std::filesystem::absolute(p_filepath);

    // check if parent directory exist
    std::filesystem::path p_directory = p_filepath.parent_path();
    if (!p_directory.empty())
    {
        if (!std::filesystem::exists(p_directory)) 
            throw std::invalid_argument("Cannot run '" + caller_name + "'.\n" + 
                    "Directory '" + p_directory.string() + "' in filepath '" + 
                    p_filepath.filename().string() + "' does not exist");
    }

    return p_filepath; 
}

inline std::string check_file_exists(const std::string& filepath, const std::string& caller_name)
{
    std::filesystem::path p_filepath = check_file(filepath, caller_name, true);
    // check if file exist
    if (!std::filesystem::exists(p_filepath)) 
        throw std::invalid_argument("Cannot run '" + caller_name + "'.\n" + 
                                    "The file '" + p_filepath.string() + "' does not exist");
    return p_filepath.string();
}

inline std::string check_filepath(const std::string& filepath, const IodeFileType expected_file_type, 
    const std::string& caller_name, const bool file_must_exist)
{
    std::filesystem::path p_filepath = check_file(filepath, caller_name, file_must_exist);

    // get list of valid extensions
    std::vector<std::string> v_expected_ext = v_file_types[expected_file_type].v_ext;

    // check or add extension
    std::filesystem::path p_filename = p_filepath.filename();
    if (p_filepath.has_extension())
    {
        // check extension
        std::string ext = p_filename.extension().string(); 
        bool match_ext = false;
        for(const std::string& expected_ext: v_expected_ext)
            if(expected_ext == ext) match_ext = true;
        if(!match_ext)
        {  
            std::string msg;
            if(v_expected_ext.size() == 1)
                msg = "Expected extension is " + v_expected_ext[0];
            else
            {
                msg = "Expected extensions are: " + v_expected_ext[0];
                for(int i=1; i < v_expected_ext.size(); i++) 
                    msg += ", " + v_expected_ext[i];
            }
            throw std::invalid_argument("Cannot run '" + caller_name + "'.\n" +
                    "The file '" + p_filename.string() + "' has wrong extension '" + ext + "'\n" + msg);
        }

        // check if file exist
        if (file_must_exist && !std::filesystem::exists(p_filepath)) 
            throw std::invalid_argument("Cannot run '" + caller_name + "'.\n" + 
                    "The file '" + p_filepath.string() + "' does not exist");
    }
    else
    {
        int database_type = (int) expected_file_type;
        if(database_type > VARIABLES) 
            throw std::invalid_argument("Cannot run '" + caller_name + "'.\n" + 
                    "You must provide an extension to the file " + p_filepath.string());

        // set binary format extension
        std::string binary_ext = v_file_types[database_type].v_ext[0];
        p_filepath = p_filepath.replace_extension(binary_ext);

        // check if file exist
        if (file_must_exist)
        {
            // check first with binary format extension
            bool binary_file_found = std::filesystem::exists(p_filepath);
            // switch to ascii format extension and check if file exist 
            if (!binary_file_found)
            {
                std::string ascii_ext = v_file_types[database_type].v_ext[1];
                p_filepath = p_filepath.replace_extension(ascii_ext);
                if (!std::filesystem::exists(p_filepath)) 
                {
                    std::filesystem::path p_directory = p_filepath.parent_path();
                    std::string stem = p_filepath.stem().string();
                    throw std::invalid_argument("Cannot run '" + caller_name + "'.\n" + 
                            "Neither '" + stem + binary_ext + "' nor '" + stem + ascii_ext + "' " +
                            "could be found in directory '" + p_directory.string() + "'");
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
    
    nb_items = (int) v_items.size();
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

// NOTE: generated by chatGPT + adapted to work with SCR (null terminated)
inline char** vector_to_double_char(const std::vector<std::string>& vec) 
{
    size_t nb_elements = vec.size();

    if(nb_elements == 0)
        return NULL;

    // Allocate memory for the C-style array (+ 1 for null terminated)
    char** c_table = new char*[nb_elements + 1];

    // Copy each string in the vector to the C-style array
    for (size_t i = 0; i < nb_elements; i++) 
    {
        // Get the length of the current string
        size_t len = vec[i].length();

        // Allocate memory for the current string in the C-style array
        c_table[i] = new char[len + 1];

        // Copy the string to the C-style array
        strcpy(c_table[i], vec[i].c_str());
    }

    // null terminated table
    c_table[nb_elements] = NULL;

    return c_table;
}
