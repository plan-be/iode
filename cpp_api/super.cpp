#pragma once
#include "super.h"


void cpp_error_super(int level, char* msg)
{
    throw std::runtime_error(std::string(msg));
}

void cpp_panic_super()
{
	char* error_msg = "Swap Panic!";
	std::cout << error_msg << std::endl;
	std::cout << "Attempting to save Data saved in ws.*files... ";
	K_end_ws(1);
	std::cout << "OK" << std::endl;
	std::cout << "To restart iode with the last data, type :" << std::endl;
	std::cout << "    iode -ws" << std::endl;
	exit(2);
}

void cpp_assign_super_API()
{
    IODE_assign_super_API();
    kerror_super = cpp_error_super;
    kpanic_super = cpp_panic_super;
}
