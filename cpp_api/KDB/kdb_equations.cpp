#pragma once
#include "kdb_equations.h"


Equation* KDBEquations::copy_obj(Equation* const original) const
{
    return new Equation(*original);
}

Equation* KDBEquations::get_unchecked(const int pos) const
{
    KDB* kdb = get_database();

    // Note: KEVAL allocate a new pointer Equation*
    Equation* c_eq = KEVAL(kdb, pos);
    return c_eq;
}

std::string KDBEquations::get_lec(const int pos) const
{ 
    return KELEC(get_database(), pos); 
}

std::string KDBEquations::get_lec(const std::string& name) const
{
    int pos = index_of(name);
    return get_lec(pos);
}

bool KDBEquations::add(const std::string& name, const Equation& obj)
{
    char* c_name = to_char_array(name);

    Equation eq(obj);
    int pos = KDBTemplate::add(name, eq, c_name);

    return pos;
}

bool KDBEquations::add(const std::string& name, const std::string& lec, const std::string& method, const std::string& from, 
    const std::string& to, const std::string& comment, const std::string& instruments, const std::string& block, const bool date)
{
    char* c_name = to_char_array(name);

    Equation eq(name, lec, method, from, to, comment, instruments, block, date);
    int pos = KDBTemplate::add(name, eq, c_name);

    return pos;
}

void KDBEquations::update(const int pos, const Equation& obj)
{
    std::string name = get_name(pos);
    char* c_name = to_char_array(name);

    Equation eq(obj);
    KDBTemplate::update(name, eq, c_name);
}

void KDBEquations::update(const std::string& name, const Equation& obj)
{
    char* c_name = to_char_array(name);

    Equation eq(obj);
    KDBTemplate::update(name, eq, c_name);
}

void KDBEquations::update(const std::string& name, const std::string& lec, const std::string& method, const std::string& from, 
    const std::string& to, const std::string& comment, const std::string& instruments, const std::string& block, const bool date)
{
    char* c_name = to_char_array(name);

    Equation eq(name, lec, method, from, to, comment, instruments, block, date);
    KDBTemplate::update(name, eq, c_name);
}

void KDBEquations::update(const int pos, const std::string& lec, const std::string& method, const std::string& from, 
    const std::string& to, const std::string& comment, const std::string& instruments, const std::string& block, const bool date)
{
    std::string name = get_name(pos);
    char* c_name = to_char_array(name);

    Equation eq(name, lec, method, from, to, comment, instruments, block, date);
    KDBTemplate::update(name, eq, c_name);
}
