#pragma once
#include <algorithm>
#include <stdexcept>
#include <boost/algorithm/string.hpp>

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


class CorrelationMatrix
{
    const std::vector<std::string> coeffs;
    const MAT* corr_matrix;

public:
    const int nb_coeffs;

public:
    CorrelationMatrix(const std::vector<std::string>& coeffs, const MAT* corr_matrix):
        nb_coeffs((int) coeffs.size()), coeffs(coeffs), corr_matrix(corr_matrix)
    {
        if(corr_matrix->m_nl != corr_matrix->m_nc)
            throw std::runtime_error("Cannot initialize the correlation matrix.\nNumber of lines " + 
                std::to_string(corr_matrix->m_nl) + " is different from the number of columns " + 
                std::to_string(corr_matrix->m_nc));
        
        if(corr_matrix->m_nl != nb_coeffs)
            throw std::runtime_error(std::string("Cannot initialize the correlation matrix.\n") + 
            "The list of coefficients contains " + std::to_string(nb_coeffs) + " names while the " + 
            "found correlation matrix is of shape " + std::to_string(corr_matrix->m_nl) + " x " + 
            std::to_string(corr_matrix->m_nc));
    }

    ~CorrelationMatrix() {} 

    std::string get_name(const int i) const
    {
        check_index(i, "Coefficient");
        return coeffs[i];
    }

    IODE_REAL get_value(const int row, const int col) const
    {
        check_index(row, "Row");
        check_index(col, "Column");
        return MATE(corr_matrix, row, col);
    }

private:
    void check_index(const int index, const std::string& index_name) const
    {
        if(index < 0 || index >= nb_coeffs)
            throw std::invalid_argument(index_name + " index " + std::to_string(index) + 
                " is out of the range [0, " + std::to_string(nb_coeffs) + "]");
    }
};


// NOTE: When exporting to Python, only get methods should be exposed (public).
//       NOT the constructor!
class EstimationResults
{
    std::string eqs_list;
    KDBEquations* kdb_eqs;
    KDBScalars* kdb_scl;
    Sample* sample;

public:
    // NOTE: Do not exposed a public constructor to the Python API
    EstimationResults(const std::string& eqs_list, const std::string& from, const std::string& to);

    ~EstimationResults();

    std::vector<std::string> get_list_equations() const
    {
        return kdb_eqs->get_names();
    }

    Sample& get_sample() const { return *sample; }

    KDBScalars& get_coefficients() const { return *kdb_scl; }

    KDBEquations& get_equations() const { return *kdb_eqs; }

    CorrelationMatrix get_correlation_matrix() 
    { 
        std::vector<std::string> v_coeffs = kdb_scl->get_names();
        return CorrelationMatrix(v_coeffs, E_MCORR); 
    }

    std::vector<IODE_REAL> get_observed_values(const std::string& name) const
    {
        std::vector<IODE_REAL> values;

        std::vector<std::string> v_equations = kdb_eqs->get_names();
        auto it = find(v_equations.begin(), v_equations.end(), name);
        if(it == v_equations.end())
            return values;
        
        int i = (int) std::distance(v_equations.begin(), it);
        for(int t = 0; t < M_NC(E_LHS); t++) values.push_back(MATE(E_LHS, i, t));
        return values;
    }

    std::vector<IODE_REAL> get_fitted_values(const std::string& name) const
    {
        std::vector<IODE_REAL> values;

        std::vector<std::string> v_equations = kdb_eqs->get_names();
        auto it = find(v_equations.begin(), v_equations.end(), name);
        if(it == v_equations.end())
            return values;
        
        int i = (int) std::distance(v_equations.begin(), it);
        for(int t = 0; t < M_NC(E_RHS); t++) values.push_back(MATE(E_RHS, i, t));
        return values;
    }

    std::vector<IODE_REAL> get_residual_values(const std::string& name) const
    {
        std::vector<IODE_REAL> values;

        std::vector<std::string> v_equations = kdb_eqs->get_names();
        auto it = find(v_equations.begin(), v_equations.end(), name);
        if(it == v_equations.end())
            return values;
        
        int i = (int) std::distance(v_equations.begin(), it);
        for(int t = 0; t < M_NC(E_LHS); t++) values.push_back(MATE(E_LHS, i, t) - MATE(E_RHS, i, t));
        return values;
    }

    void save();
};


// TODO: add all estimation parameters
/**
 * @brief Estimate an equation of a given sample. 
 *        The estimation method and the instruments must be specified 
 *        in the equation before the estimation.
 *        Only defined to standardize the Python API functions.
 * 
 * @param eqs_list   comma separated list of equation names (=endo  names)
 * @param from       first period of the estimation sample
 * @param to         last period of the estimation sample
 * @return EstimationResults 
 * 
 * @note equivalent to function B_EqsEstimateEqs() from b_est.c
 */
EstimationResults* equations_estimate(const std::string& eqs_list, const std::string& from = "", const std::string& to = "");

/**
 * @brief Estimate an equation of a given sample. 
 *        The estimation method and the instruments must be specified 
 *        in the equation before the estimation.
 *        Only defined to standardize the Python API functions.
 * 
 * @param eqs_vector   list of equation names (=endo  names)
 * @param from         first period of the estimation sample
 * @param to           last period of the estimation sample
 * @return EstimationResults
 */
EstimationResults* equations_estimate(const std::vector<std::string>& eqs_vector, const std::string& from = "", const std::string& to = "");
