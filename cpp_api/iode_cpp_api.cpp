// cpp_api.cpp : Source file for your target.
//

#pragma once
#include "iode_cpp_api.h"

void cpp_iode_init()
{
    IodeInit();
    IODE_assign_super_API();
}
