#pragma once
#include "super.h"


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

void cpp_warning(const char* msg)
{
	std::cout << "WARNING: " + std::string(msg) << std::endl;	
}

void cpp_assign_super_API()
{
    IODE_assign_super_API();
	kwarning_super = cpp_warning;
    kpanic_super = cpp_panic_super;
}
