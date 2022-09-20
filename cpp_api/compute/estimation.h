#pragma once
#include <algorithm>
#include "utils/utils.h"
#include "time/sample.h"
#include "kdb/kdb_equations.h"
#include "kdb/kdb_scalars.h"


class Estimation
{
    std::string str_equations;
    std::vector<std::string> v_equations;
    std::vector<std::string>::iterator current_eq;

    KDBEquations* kdb_eqs;
    KDBScalars* kdb_scl;
    Sample* sample;

private:
    void set_equations_list(const std::string& str_equations);

public:
    Estimation(const Sample& sample, const std::string& str_equations);
    Estimation(const Period& from, const Period& to, const std::string& str_equations);
    Estimation(const std::string& from, const std::string& to, const std::string& str_equations);

    ~Estimation();

    void set_sample(const Sample& sample)
    {
        if (this->sample) delete this->sample;
        this->sample = new Sample(sample);
    }
    
    void set_sample(const Period& from, const Period& to)
    {
        if (sample) delete sample;
        try
        {
            sample = new Sample(from, to);
        }
        catch(IodeException)
        {
            throw IodeExceptionInvalidArguments("Cannot estimate (block of) equation(s) " + str_equations, 
                "Cannot create sample with range from " + from.to_string() + " to " + to.to_string());
        }
    }
    
    void set_sample(const std::string& from, const std::string& to)
    {
        if (sample) delete sample;
        try
        {
            sample = new Sample(from, to);
        }
        catch(IodeException)
        {
            throw IodeExceptionInvalidArguments("Cannot estimate (block of) equation(s) " + str_equations, 
                "Cannot create sample with range from " + from + " to " + to);
        }
    }

    KDBScalars* get_coefficients() { return kdb_scl; }

    KDBEquations* get_equations() { return kdb_eqs; }

    Equation next_equation()
    {
        if(current_eq == v_equations.end()) current_eq = v_equations.begin();
        else current_eq++;
        return Equation(*current_eq);
    }

    /** 
     * Equivalent to B_EqsEstimateEqs
     */
    void equations_estimate();

    void save();
};
