#pragma once
#include "kdb_equations.h"


Equation* KDBEquations::copy_obj(Equation* const original) const
{
    return new Equation(*original);
}

Equation* KDBEquations::get_unchecked(const int pos) const
{
    KDB* kdb = get_database();

    // Note: KEVAL allocate a new pointer EQ*
    EQ* c_eq = KEVAL(kdb, pos);
    // re-compute CLEC
    c_eq->clec = L_solve((char*) c_eq->lec.c_str(), (char*) c_eq->endo.c_str());
    if (c_eq->clec == NULL)
        throw std::runtime_error("Failed to compute LEC expression '" + std::string(c_eq->lec) + 
                    "' of equation named '" + std::string(c_eq->endo) + "'");
	// Note: static_cast<Equation*>(EQ*) calls the copy constructor Equation(const EQ* c_eq)
	//       which calls copy_from_EQ_obj()
    return static_cast<Equation*>(c_eq);
}

std::string KDBEquations::get_lec(const int pos) const
{ 
    return KELEC(get_database(), pos); 
}

std::string KDBEquations::get_lec(const std::string& name) const
{
    int pos = get_position(name);
    return get_lec(pos);
}

int KDBEquations::add(const std::string& name, const Equation& obj)
{
    char* c_name = to_char_array(name);

    Equation eq(obj);
    int pos = KDBTemplate::add(name, static_cast<EQ*>(&eq), c_name);

    return pos;
}

int KDBEquations::add(const std::string& name, const std::string& lec, const std::string& method, const std::string& from, 
    const std::string& to, const std::string& comment, const std::string& instruments, const std::string& block, const bool date)
{
    char* c_name = to_char_array(name);

    Equation eq(name, lec, method, from, to, comment, instruments, block, date);
    int pos = KDBTemplate::add(name, static_cast<EQ*>(&eq), c_name);

    return pos;
}

void KDBEquations::update(const int pos, const Equation& obj)
{
    std::string name = get_name(pos);
    char* c_name = to_char_array(name);

    Equation eq(obj);
    KDBTemplate::update(name, static_cast<EQ*>(&eq), c_name);
}

void KDBEquations::update(const std::string& name, const Equation& obj)
{
    char* c_name = to_char_array(name);

    Equation eq(obj);
    KDBTemplate::update(name, static_cast<EQ*>(&eq), c_name);
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
