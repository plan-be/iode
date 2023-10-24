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

int KDBEquations::add(const std::string& name, const Equation& obj)
{
    check_name(name, iode_type);

    char* c_name = to_char_array(name);

    // throw exception if object with passed name already exist
    if(K_find(get_KDB(), c_name) >= 0)
        throw std::invalid_argument("Cannot add " + iode_type_name + " with name '" + name + "'.\n" +  
            iode_type_name + " with name '" + name + "' already exists. Use the update() method instead.");

    Equation eq(obj);
    int pos = KDBTemplate::add(name, static_cast<EQ*>(&eq), c_name);

    return pos;
}

int KDBEquations::add(const std::string& name, const std::string& lec, const std::string& method, const std::string& from, 
    const std::string& to, const std::string& comment, const std::string& instruments, const std::string& block, const bool date)
{
    check_name(name, iode_type);

    char* c_name = to_char_array(name);

    // throw exception if object with passed name already exist
    if (K_find(get_KDB(), c_name) >= 0)
        throw std::invalid_argument("Cannot add " + iode_type_name + " with name '" + name + "'.\n" +  
            iode_type_name + " with name '" + name + "' already exists. Use the update() method instead.");

    Equation eq(name, lec, method, from, to, comment, instruments, block, date);

    int pos = KDBTemplate::add(name, static_cast<EQ*>(&eq), c_name);
    return pos;
}

void KDBEquations::update(const std::string& name, const std::string& lec, const std::string& method, const std::string& from, 
    const std::string& to, const std::string& comment, const std::string& instruments, const std::string& block, const bool date)
{
    char* c_name = to_char_array(name);

    Equation eq(name, lec, method, from, to, comment, instruments, block, date);

    KDBTemplate::update(name, static_cast<EQ*>(&eq), c_name);
}

void KDBEquations::update(const int pos, const std::string& lec, const std::string& method, const std::string& from, 
    const std::string& to, const std::string& comment, const std::string& instruments, const std::string& block, const bool date)
{
    std::string name = get_name(pos);
    char* c_name = to_char_array(name);

    Equation eq(name, lec, method, from, to, comment, instruments, block, date);

    KDBTemplate::update(name, static_cast<EQ*>(&eq), c_name);
}
