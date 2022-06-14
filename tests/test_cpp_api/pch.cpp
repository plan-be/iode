//
// pch.cpp (pch = precompiled header)
//

#include "pch.h"

std::vector<std::string> char_array_to_std_string_vector(char** char_array)
{
    std::vector<std::string> vec;
    
    int nb_elements = SCR_tbl_size((unsigned char**) char_array);
    vec.reserve(nb_elements);
    for (int i = 0; i < nb_elements; i++) vec.push_back(std::string(char_array[i]));

    return vec;
}
