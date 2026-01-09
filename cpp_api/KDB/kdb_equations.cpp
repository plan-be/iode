#pragma once
#include "kdb_equations.h"


Equation* KDBEquations::copy_obj(Equation* const original) const
{
    return new Equation(*original);
}

Equation* KDBEquations::get_unchecked(const std::string& name) const
{
    CKDBEquations* kdb = get_database();

    // Note: KEVAL allocate a new pointer Equation*
    Equation* c_eq = kdb->get_obj(name);
    return c_eq;
}

std::string KDBEquations::get_lec(const std::string& name) const
{ 
    return get_database()->get_obj(name)->lec; 
}

bool KDBEquations::add(const std::string& name, const Equation& obj)
{
    Equation eq(obj);
    int pos = KDBTemplate::add(name, &eq);
    return pos;
}

bool KDBEquations::add(const std::string& name, const std::string& lec, const std::string& method, const std::string& from, 
    const std::string& to, const std::string& comment, const std::string& instruments, const std::string& block, const bool date)
{
    Equation eq(name, lec, method, from, to, comment, instruments, block, date);
    int pos = KDBTemplate::add(name, &eq);
    return pos;
}

void KDBEquations::update(const std::string& name, const Equation& obj)
{
    Equation eq(obj);
    KDBTemplate::update(name, &eq);
}

void KDBEquations::update(const std::string& name, const std::string& lec, const std::string& method, const std::string& from, 
    const std::string& to, const std::string& comment, const std::string& instruments, const std::string& block, const bool date)
{
    Equation eq(name, lec, method, from, to, comment, instruments, block, date);
    KDBTemplate::update(name, &eq);
}
