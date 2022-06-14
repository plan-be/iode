#pragma once
#include "kdb_equations.h"


// CRUD (Create - Read - Update - Delete) + Copy methods

// Note: argument set to "", NULL or 0 are unchanged
int KDBEquations::add_or_update(const std::string& name, const Equation& equation)
{
    char* c_name = const_cast<char*>(name.c_str());
    int pos = K_add(K_WS[I_EQUATIONS], c_name, equation.c_equation, c_name);
    if (pos == -1) throw std::runtime_error("Iode has not been initialized");
    if (pos < -1) throw std::runtime_error("Cannot create or update " + vIodeTypes[type] + " with name " + name);
    return pos;
}

Equation KDBEquations::copy_obj(const Equation& original) const
{
    return Equation(original);
}

Equation KDBEquations::get_unchecked(const int pos) const
{
    std::string name = get_name(pos);
    return Equation(name);
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

Equation KDBEquations::add(const std::string& name, const std::string& lec, const std::string& comment, const std::string& method,
    Sample& sample, const std::string& instruments, const std::string& block, const std::array<float, EQS_NBTESTS>& tests, const bool date)
{
    char* c_name = const_cast<char*>(name.c_str());

    if (lec.empty()) throw std::runtime_error("Cannot create new equation with name " + name + ". Passed lec expression is empty.");
    char* c_lec = const_cast<char*>(lec.c_str());
    CLEC* clec = L_solve(c_lec, c_name);
    if (clec == NULL) throw std::runtime_error("Cannot create new equation with name " + name + ". Passed lec expression is not valid:\n" + lec);
    SW_nfree(clec);

    char* c_comment = comment.empty() ? NULL : const_cast<char*>(comment.c_str());
    char* c_instruments = instruments.empty() ? NULL : const_cast<char*>(instruments.c_str());
    char* c_block = block.empty() ? NULL : const_cast<char*>(block.c_str());
    float* c_tests = const_cast<float*>(tests.data());
    int i_method = 0;
    for (int i = 0; i < I_NB_EQ_METHODS; i++) if (method == vEquationMethods[i]) i_method = i;
    int i_date = date ? 1 : 0;

    int res = B_DataUpdateEqs(c_name, c_lec, c_comment, i_method, sample.c_sample, c_instruments, c_block, c_tests, i_date);
    if (res < 0) throw std::runtime_error("something went wrong when trying to add or update equation " + std::string(name));

    return Equation(name);
}
