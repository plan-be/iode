#pragma once
#include <string>
#include <iostream>
#include <Python.h>
#include "api/iode.h"
#include "cpp_api/iode_cpp_api.h"


int kerror_super_PYTHON(const int level, const char* msg)
{
    if(level > 0)
    {
        std::cerr << "Error: " << msg << std::endl;
        exit(level);
    }
    else
    {
        PyErr_SetString(PyExc_RuntimeError, msg);
        return -1;
    }
}

void kwarning_super_PYTHON(const char* msg)
{
    PyErr_WarnEx(PyExc_UserWarning, msg, 2);
}

void kmsg_super_PYTHON(const char* msg)
{
    PySys_WriteStdout("%s\n", msg);
}

int kconfirm_super_PYTHON(const char* msg)
{
    std::string s_yes = "OoYyJj1";
    std::cout << std::string(msg) << std::endl;

    std::string answer;
    std::getline(std::cin, answer);
    // Remove all spaces
    answer.erase(std::remove(answer.begin(), answer.end(), ' '), answer.end());
    
    return (s_yes.find(answer[0]) != std::string::npos) ? 0 : 1;
}

void kpanic_super_PYTHON()
{
	std::cout << "Swap Panic!" << std::endl;
	std::cout << "Attempting to save Data saved in ws.*files... ";
	K_end_ws(1);
	std::cout << "OK" << std::endl;
	std::cout << "To restart iode with the last data, type :" << std::endl;
	std::cout << "    iode -ws" << std::endl;
	exit(2);
}

void python_assign_super()
{
    kerror_super = kerror_super_PYTHON;
    kwarning_super = kwarning_super_PYTHON;
    kmsg_super = kmsg_super_PYTHON;
    kconfirm_super = kconfirm_super_PYTHON;
    kpanic_super = kpanic_super_PYTHON;
}
