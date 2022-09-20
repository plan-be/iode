#pragma once
#include "kdb_equations.h"


Equation KDBEquations::copy_obj(const Equation& original) const
{
    return Equation(original);
}

Equation KDBEquations::get_unchecked(const int pos) const
{
    return Equation(pos, get_KDB());
}

std::string KDBEquations::get_lec(const int pos) const
{ 
    return KELEC(get_KDB(), pos); 
}

std::string KDBEquations::get_lec(const std::string& name) const
{
    int pos = get_position(name);
    return get_lec(pos);
}

int KDBEquations::add(const std::string& name, const std::string& lec, const std::string& comment, const std::string& method,
    Sample* sample, const std::string& instruments, const std::string& block, const std::array<float, EQS_NBTESTS>& tests, const bool date)
{
    check_name(name, iode_type);

    char* c_name = to_char_array(name);

    // throw exception if object with passed name already exist
    if (K_find(get_KDB(), c_name) >= 0)
        throw IodeExceptionFunction("Cannot add " + iode_type_name + " with name " + name,  
            iode_type_name + " with name " + name + " already exists. Use update() method instead.");

    bool new_sample = sample == nullptr;
    if (new_sample) sample = new Sample(KSMPL(K_WS[I_VARIABLES]));
    EQ* eq = prepare_equation(name, "create new", lec, comment, method, sample, instruments, block, &tests, date);

    int pos = KDBTemplate::add(name, eq, c_name);

    if(new_sample) delete sample;
    return pos;
}

void KDBEquations::update(const std::string& name, const std::string& lec, const std::string& comment, const std::string& method,
    Sample* sample, const std::string& instruments, const std::string& block, const std::array<float, EQS_NBTESTS>* tests, const bool date)
{
    char* c_name = to_char_array(name);

    EQ* eq = prepare_equation(name, "update", lec, comment, method, sample, instruments, block, tests, date);

    KDBTemplate::update(name, eq, c_name);
}

void KDBEquations::update(const int pos, const std::string& lec, const std::string& comment, const std::string& method,
    Sample* sample, const std::string& instruments, const std::string& block, const std::array<float, EQS_NBTESTS>* tests, const bool date)
{
    std::string name = get_name(pos);
    char* c_name = to_char_array(name);

    EQ* eq = prepare_equation(name, "update", lec, comment, method, sample, instruments, block, tests, date);

    KDBTemplate::update(name, eq, c_name);
}
