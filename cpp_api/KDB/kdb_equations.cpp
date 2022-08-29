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

    char* c_name = const_cast<char*>(name.c_str());

    // throw exception if object with passed name already exist
    if (K_find(get_KDB(), c_name) >= 0)
        throw std::runtime_error(iode_type_name + " with name " + name + " already exists. Use update() method instead.");

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
    char* c_name = const_cast<char*>(name.c_str());

    EQ* eq = prepare_equation(name, "update", lec, comment, method, sample, instruments, block, tests, date);

    KDBTemplate::update(name, eq, c_name);
}

void KDBEquations::update(const int pos, const std::string& lec, const std::string& comment, const std::string& method,
    Sample* sample, const std::string& instruments, const std::string& block, const std::array<float, EQS_NBTESTS>* tests, const bool date)
{
    std::string name = get_name(pos);
    char* c_name = const_cast<char*>(name.c_str());

    EQ* eq = prepare_equation(name, "update", lec, comment, method, sample, instruments, block, tests, date);

    KDBTemplate::update(name, eq, c_name);
}

/** 
 * Equivalent to B_EqsEstimateEqs
 */
void KDBEquations::equations_estimate(const Sample& sample, const std::string& equations_list)
{
    IodeExceptionInvalidArguments invalid_error("(block of) equation(s)", "estimate");
    if (equations_list.empty()) 
    {
        invalid_error.add_argument("(list of) equation(s) is empty! ", equations_list);
        throw invalid_error;
    }

    char* c_equations_list = const_cast<char*>(equations_list.c_str());
    char** eqs = B_ainit_chk(c_equations_list, NULL, 0);
    if(eqs == NULL || SCR_tbl_size((unsigned char**) eqs) == 0) 
    {
        B_seterrn(40);
        B_display_last_error();
    }
    else 
    {
        KDB* tdbe = K_refer(KE_WS, SCR_tbl_size((unsigned char**) eqs), eqs);
        if(tdbe == 0 || KNB(tdbe) == 0) 
        {
            invalid_error.add_argument("(list of) equation(s) invalid! ", equations_list);
            throw invalid_error;  
        }                                                       
        else
        {
            int rc;
            rc = KE_est_s(tdbe, KV_WS, KS_WS, sample.c_sample, eqs);    // JMP 29/04/2022
            K_free_kdb(tdbe);
            if (rc != 0)
            {
                IodeExceptionFunction func_error("(block of) equation(s)", "estimate");
                func_error.add_argument("From: ", sample.start_period().to_string());
                func_error.add_argument("To:   ", sample.end_period().to_string());
                func_error.add_argument("(block of) equation(s): ", equations_list);
                throw func_error;
            }
        }                                                                  
    }
}

void KDBEquations::equations_estimate(const Period& from, const Period& to, const std::string& equations_list = "")
{
    Sample sample(from, to);
    equations_estimate(sample, equations_list);
}

void KDBEquations::equations_estimate(const std::string& from, const std::string& to, const std::string& equations_list = "")
{
    Sample sample(from, to);
    equations_estimate(sample, equations_list);
}
