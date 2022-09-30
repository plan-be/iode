#pragma once
#include <algorithm>
#include "utils/utils.h"
#include "time/sample.h"
#include "kdb/kdb_equations.h"
#include "kdb/kdb_scalars.h"
#include "kdb/kdb_variables.h"


enum EnumIodeAdjustmentMethod
{
    AM_PARTIAL_ADJUSTMENT,
    AM_ERROR_CORRECTION_METHOD
};

const static std::array<std::string, 2> v_adjstment_method =
{
    "Partial Adjustment",
    "Error Correction Model"
};

std::string dynamic_adjustment(const EnumIodeAdjustmentMethod method, 
    const std::string& eqs, const std::string& c1, const std::string& c2);

/**
 * @brief a.k.a Unit Root.
 * 
 * @return IODE_REAL 
 */
KDBScalars* dickey_fuller_test(const std::string& lec, bool drift, bool trend, int order);


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

    NamedEquation current_equation() { return NamedEquation(*current_eq); }

    NamedEquation next_equation()
    {
        if(current_eq == v_equations.end()) current_eq = v_equations.begin();
        else current_eq++;
        return NamedEquation(*current_eq);
    }

    MAT* get_correlation_matrix() { return E_MCORR; }

    /** 
     * Equivalent to B_EqsEstimateEqs
     */
    void equations_estimate();

    void save();

};
