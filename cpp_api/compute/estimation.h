#pragma once
#include <array>
#include <algorithm>
#include <stdexcept>
#include <boost/algorithm/string.hpp>

#include "utils/utils.h"
#include "time/sample.h"
#include "kdb/kdb_equations.h"
#include "kdb/kdb_scalars.h"
#include "kdb/kdb_variables.h"


constexpr static int    ESTIMATION_MAXIT = 100;
constexpr static double ESTIMATION_EPS   = 0.0000001;

enum EnumIodeAdjustmentMethod
{
    AM_PARTIAL_ADJUSTMENT,
    AM_ERROR_CORRECTION_METHOD
};

const static std::vector<std::string> v_adjstment_method =
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


/**
 * @warning Only for GUI. Implemented in the C++ API for testing.
 * 
 */
class EditAndEstimateEquations
{
    bool estimation_done;

    Sample* sample;
    std::string block;
    std::string instruments;
    int method;

    std::vector<std::string> v_equations;
    std::vector<std::string>::iterator current_eq;

    KDBEquations* kdb_eqs;
    KDBScalars* kdb_scl;

public:
    EditAndEstimateEquations(const std::string& from = "", const std::string& to = "");
    ~EditAndEstimateEquations();

    Sample get_sample() const
    {
        return Sample(*sample);
    }

    void set_sample(const Sample* sample = nullptr)
    {
        if(this->sample) 
            delete this->sample;

        // make a copy
        if(sample)
            this->sample = new Sample(*sample);
        else
            this->sample = new Sample(Variables.get_sample());
    }
    
    void set_sample(const Period* from = nullptr, const Period* to = nullptr)
    {
        if(this->sample) 
            delete this->sample;

        Sample vars_sample = Variables.get_sample();  
        Period from_ = from ? Period(*from) : vars_sample.start_period();
        Period to_ = to ? Period(*to) : vars_sample.end_period();

        try
        {
            sample = new Sample(*from, *to);
        }
        catch(IodeException)
        {
            throw IodeExceptionInvalidArguments("Cannot estimate (block of) equation(s) " + block, 
                "Cannot create sample with range from " + from->to_string() + " to " + to->to_string());
        }
    }
    
    void set_sample(const std::string& from = "", const std::string& to = "")
    {
        if(this->sample) 
            delete this->sample;

        Sample vars_sample = Variables.get_sample();  
        Period from_ = (!from.empty()) ? Period(from) : vars_sample.start_period();
        Period to_ = (!to.empty()) ? Period(to) : vars_sample.end_period();

        try
        {
            sample = new Sample(from, to);
        }
        catch(IodeException)
        {
            throw IodeExceptionInvalidArguments("Cannot estimate (block of) equation(s) " + block, 
                "Cannot create sample with range from " + from + " to " + to);
        }
    }

    std::string get_block() const
    {
        return block;
    }

    /**
     * @brief update the 'block', 'kdb_eqs', 'kdb_scl', 'v_equations' and 'current_eq' attributes 
     *        given the passed values for the 'block' and 'current_eq_name' arguments.
     * 
     *        Step 1:  reset attributes 'block', 'v_equations' and 'current_eq'
     *        Step 2:  generate a list of equations names from the passed argument 'block'
     *        Step 3:  add the current equation name to the block if not referenced in it 
     *        Step 4:  check each name if is valid.
     *                 If there is an invalid name, throw an error
     *        Step 5:  for each equation name from the block
     *                 a. check if the equation is already present in the local database 'kdb_eqs':
	 *                 - no  -> check if in the global database
     *                          - yes -> copy equation from the global database to 'kdb_eqs' 
	 *                          - no  -> add a new equation with LEC '<name> := 0' to 'kdb_eqs'
	 *                 - yes -> does nothing
     *                 b. add the equation name to the vector 'v_equations'
     *        Step 6: copy the list of equations names separated by ';' to the 'block' attribute
     *        Step 7: move the equations iterator to the current equation or the first equation of the block
     * 
     * @param block string representing the list of equations to estimate
     * @param current_eq_name name of the currently displayed (edited) equation (in the GUI)
     * 
     * @note equivalent to the ODE_blk_check() function from o_est.c from the old GUI
     */
    void set_block(const std::string& block, const std::string& current_eq_name = "");

    std::string get_method() const
    {
        return v_eq_methods.at(method);
    }

    int get_method_as_int() const
    {
        return method;
    }

    void set_method(const int method)
    {
        if(method >= I_NB_EQ_METHODS)
            throw std::invalid_argument("Invalid value " + std::to_string(method) + " for the method.\n" + 
                "The passed value must be in the range [0, " + std::to_string(I_NB_EQ_METHODS - 1) + "]");

        this->method = method;
    }

    void set_method(const std::string& method)
    {
        int m = -1;
        for(int i = 0; i < I_NB_EQ_METHODS; i++) 
            if(method == v_eq_methods[i]) m = i;

        if(m < 0)
            throw std::invalid_argument("The method '" + method + "' is not valid.\n" + 
                "Accepted methods are: " + boost::algorithm::join(v_eq_methods, ", "));

        this->method = m;
    }

    std::string get_instruments() const
    {
        return instruments;
    }

    /**
     * @brief Set the instruments object
     * 
     * @param instruments
     * 
     * @note equivalent to the ODE_blk_instr() function of o_est.c from the old GUI 
     */
    void set_instruments(const std::string& instruments)
    {
        if(instruments.empty())
        {
            this->instruments = "";
            return;
        }

        std::vector<std::string> v_instrs;
        boost::split(v_instrs, instruments, boost::is_any_of(";"));

        // check the LEC expression for each instrument.
        // Return an error if one instrument is invalid.
        for(const std::string& instr: v_instrs) 
        {
            CLEC* clec = L_cc(to_char_array(instr));
            if(clec == NULL) 
            {
                this->instruments = "";
                B_seterrn(213, to_char_array(instr), L_error());
                throw std::runtime_error(get_last_error());
            }
        }

        this->instruments = instruments;
    }

    std::vector<std::string> get_list_equations() const
    {
        return v_equations;
    }

    KDBScalars* get_scalars() { return kdb_scl; }

    /**
     * @brief update the local Scalars workspace 'kdb_scl'.
     * 
     *        Step 1: for each equation in the local Equations workspace, get the list if corresponding scalars
     *        Step 2: remove duplicated scalar names
     *        Step 3: for each scalar name, check if it is already present in the local database 'kdb_scl':
	 *                - no  -> check if in the global Scalars database
     *                         - yes -> copy scalars from the global database to 'kdb_scl' 
	 *                         - no  -> add a new scalar with value = 0.0 and relax = 1.0 to 'kdb_scl'
	 *                - yes -> does nothing
     *        Step 4: remove the scalars associated with equations which are not in the present block to estimate
     * 
     * @note equivalent to the ODE_blk_coef() function from o_est.c from the old GUI
     */
    void update_scalars();

    /**
     * @brief extract and save the resulting values for the tests for each equations in the local Equations database.
     * 
     * @note equivalent to the ODE_blk_cur_atests() function from o_est.c from the old GUI
     */
    void copy_eq_tests_values();

    KDBEquations* get_equations() { return kdb_eqs; }

    /**
     * @brief update the LEC and comment of the current equation
     * 
     * @note equivalent to the ODE_blk_save_cur() function from o_est.c from the old GUI
     */
    void update_current_equation(const std::string& lec, const std::string& comment);

    NamedEquation current_equation() 
    {
        std::string name = *current_eq;
        Equation eq = kdb_eqs->get(name);
        return NamedEquation(name, eq); 
    }

    NamedEquation next_equation()
    {
        current_eq++;
        
        if(current_eq == v_equations.end()) 
            current_eq = v_equations.begin();

        std::string name = *current_eq;
        Equation eq = kdb_eqs->get(name);
        return NamedEquation(name, eq);
    }

    CorrelationMatrix get_correlation_matrix() 
    { 
        std::vector<std::string> v_coeffs = kdb_scl->get_names();
        return CorrelationMatrix(v_coeffs, E_MCORR); 
    }

    std::vector<IODE_REAL> get_observed_values(const std::string& name) const
    {
        std::vector<IODE_REAL> values;
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
        auto it = find(v_equations.begin(), v_equations.end(), name);
        if(it == v_equations.end())
            return values;
        
        int i = (int) std::distance(v_equations.begin(), it);
        for(int t = 0; t < M_NC(E_LHS); t++) values.push_back(MATE(E_LHS, i, t) - MATE(E_RHS, i, t));
        return values;
    }

    /**
     * @note equivalent to ODE_blk_est() from o_est.c from the old GUI 
     */
    void estimate();

    bool is_estimation_done() const { return estimation_done; }

    /**
     * @brief - copy the Equations referenced in the vector v_equations from the local database 
     *          to the global database
     *        - if estimation -> create the Scalars containing the results of an estimated equation
     *        - merge the local Scalars database into the global Scalars database
     *        - return the list of new equations 
     * 
     * @param from         starting period to copy if no estimation has been done
     * @param to           ending period to copy if no estimation has been done
     * @return             vector of the new equations names
     * 
     * @note equivalent to the ODE_blk_save() function from o_est.c from the old GUI
     */
    std::vector<std::string> save(const std::string& from = "", const std::string& to = "");
};


// TODO: add all estimation parameters
/**
 * @brief Estimate an equation of a given sample. 
 *        The estimation method and the instruments must be specified 
 *        in the equation before the estimation.
 *        Only defined to standardize the Python API functions.
 * 
 * @param eqs   comma separated list of equation names (=endo  names)
 * @param from  first period of the estimation sample
 * @param to    last period of the estimation sample
 * 
 * @note equivalent to function IodeEstimate() from b_api.c
 */
void eqs_estimate(const std::string& eqs, const std::string& from = "", const std::string& to = "");

/**
 * @brief Estimate an equation of a given sample. 
 *        The estimation method and the instruments must be specified 
 *        in the equation before the estimation.
 *        Only defined to standardize the Python API functions.
 * 
 * @param eqs   list of equation names (=endo  names)
 * @param from  first period of the estimation sample
 * @param to    last period of the estimation sample
 */
void eqs_estimate(const std::vector<std::string>& eqs, const std::string& from = "", const std::string& to = "");
