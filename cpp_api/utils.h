#pragma once

#include "common.h"

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


// oem stands for oem 437 (see https://en.wikipedia.org/wiki/Code_page_437)
inline std::string convert_oem_to_utf8(const std::string str_oem)
{
    return convert_between_codepages(str_oem, CP_OEMCP, CP_UTF8);
}


inline std::string convert_utf8_to_oem(const std::string str_u8)
{
    return convert_between_codepages(str_u8, CP_UTF8, CP_OEMCP);
}
