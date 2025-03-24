#pragma once
#include "super.h"

// to test kerror from the cython wrapper
void cpp_kerror(const char* msg)
{
    kerror(0, msg);
}

void cpp_assign_super_API()
{
    IODE_assign_super_API();
}
