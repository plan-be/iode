// cpp_api.cpp : Source file for your target.
//

#pragma once
#include "iode_cpp_api.h"

void cpp_iode_init(const std::string& iode_msg_path)
{
    if(iode_msg_path.empty())
        IodeInit(NULL);
    else
        IodeInit(const_cast<char*>(iode_msg_path.c_str()));
    cpp_assign_super_API();
}
