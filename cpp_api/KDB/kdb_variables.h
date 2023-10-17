#pragma once
#include "time/period.h"
#include "time/sample.h"
#include "kdb_template.h"
#include <stdexcept>


// TODO: replace KVAR_MODE section in iode.h by the enum below
enum EnumIodeVarMode
{
    I_VAR_MODE_LEVEL,              //< no modification             x[t]
    I_VAR_MODE_DIFF,               //< difference on one period    (x[t]-x[t-1])
    I_VAR_MODE_GROWTH_RATE,        //< growth rate on one period   (x[t]/x[t-1] - 1)*100
    I_VAR_MODE_Y0Y_DIFF,           //< difference on one year      (x[t]-x[t-{nb sub periods}])
    I_VAR_MODE_Y0Y_GROWTH_RATE     //< growth rate on one year     (x[t]/x[t-{nb sub periods}] - 1) * 100
};

const static int I_VAR_NB_MODES = 5;

const static std::array<std::string, I_VAR_NB_MODES> v_var_modes = 
    { "Level", "Differences", "Growth rates", "YoY Diffs", "YoY Grt" };


// TODO: wrapp functions from k_wsvar.c in KDBVariables
class KDBVariables : public KDBTemplate<Variable>
{
private:
    Variable new_var_from_lec(const std::string& lec);

protected:

    Variable copy_obj(const Variable& original) const override;

    Variable get_unchecked(const int pos) const override;

public:
    KDBVariables(const EnumIodeKDBType kdb_type = KDB_GLOBAL, const std::string& pattern = "") : 
        KDBTemplate(kdb_type, I_VARIABLES, pattern) {};

    KDBVariables(const KDBVariables& kdb_to_copy) : KDBTemplate(kdb_to_copy) {}

    IODE_REAL get_var(const int pos, const int t, const EnumIodeVarMode mode = I_VAR_MODE_LEVEL) const;

    IODE_REAL get_var(const int pos, const std::string& period, const EnumIodeVarMode mode = I_VAR_MODE_LEVEL) const;

    IODE_REAL get_var(const int pos, const Period& period, const EnumIodeVarMode mode = I_VAR_MODE_LEVEL) const;

    IODE_REAL get_var(const std::string& name, const int t, const EnumIodeVarMode mode = I_VAR_MODE_LEVEL) const;

    IODE_REAL get_var(const std::string& name, const std::string& period, const EnumIodeVarMode mode = I_VAR_MODE_LEVEL) const;

    IODE_REAL get_var(const std::string& name, const Period& period, const EnumIodeVarMode mode = I_VAR_MODE_LEVEL) const;

    void set_var(const int pos, const int t, const IODE_REAL value, const EnumIodeVarMode mode = I_VAR_MODE_LEVEL);

    void set_var(const int pos, const std::string& period, const IODE_REAL value, const EnumIodeVarMode mode = I_VAR_MODE_LEVEL);

    void set_var(const int pos, const Period& period, const IODE_REAL value, const EnumIodeVarMode mode = I_VAR_MODE_LEVEL);

    void set_var(const std::string& name, const int t, const IODE_REAL value, const EnumIodeVarMode mode = I_VAR_MODE_LEVEL);

    void set_var(const std::string& name, const std::string& period, const IODE_REAL value, const EnumIodeVarMode mode = I_VAR_MODE_LEVEL);

    void set_var(const std::string& name, const Period& period, const IODE_REAL value, const EnumIodeVarMode mode = I_VAR_MODE_LEVEL);

    int add(const std::string& name, const Variable& variable);

    int add(const std::string& name, const std::string& lec);

    void update(const int pos, const Variable& variable);

    void update(const int pos, const std::string& lec);

    void update(const std::string& name, const Variable& variable);

    void update(const std::string& name, const std::string& lec);

    Sample get_sample() const;

    void set_sample(const std::string& from, const std::string& to);

    void set_sample(const Period& from, const Period& to);

    int get_nb_periods() const;

    std::string get_period(const int t) const;

    float get_period_as_float(const int t) const;

    std::vector<std::string> get_list_periods(const std::string& from = "", const std::string& to = "") const;

    std::vector<float> get_list_periods_as_float(const std::string& from = "", const std::string& to = "") const;

    void copy_into(const std::string& input_file, const std::string& from = "", const std::string& to = "", 
        const std::string objects_names = "");
    
    void copy_into(const std::string& input_file, const Period& from = nullptr, const Period& to = nullptr, 
        const std::string objects_names = "");

    /**
     *  Syntax: $WsExtrapolate [method] from to [variable list]
     *          where 
     *              method : 0 ... 6
     *                  0 = Y := Y[-1], if Y null or NA
     *                  1 = Y := Y[-1], always
     *                  2 = Y := extrapolation, if Y null or NA
     *                  3 = Y := extrapolation, always
     *                  4 = Y := unchanged, always
     *                  5 = Y := Y[-1], if Y is NA
     *                  6 = Y := extrapolation, if Y is NA
     *              from, to := periods
     *
     *  
     *  @see https://iode.plan.be/doku.php?id=wsextrapolate
     */
    void extrapolate(const EnumSimulationInitialization method, const std::string& from, 
        const std::string& to, const std::string& variables_list="");

    /**
     *  Syntax: $WsExtrapolate [method] from to [variable list]
     *          where 
     *              method : 0 ... 6
     *                  0 = Y := Y[-1], if Y null or NA
     *                  1 = Y := Y[-1], always
     *                  2 = Y := extrapolation, if Y null or NA
     *                  3 = Y := extrapolation, always
     *                  4 = Y := unchanged, always
     *                  5 = Y := Y[-1], if Y is NA
     *                  6 = Y := extrapolation, if Y is NA
     *              from, to := periods
     *
     *  
     *  @see https://iode.plan.be/doku.php?id=wsextrapolate
     */
    void extrapolate(const EnumSimulationInitialization method, const Period& from, 
        const Period& to, const std::string& variables_list="");

    // TODO JMP: please provide input for unit test
    /**
     * @brief $WsSeasAdj Filename VarList Eps
     * 
     * @see https://iode.plan.be/doku.php?id=wsseasonadj and
     *      https://iode.plan.be/doku.php?id=wsseasadj 
     */
    void seasonal_adjustment(std::string& input_file, const std::string& series, const double eps_test);

    // TODO JMP: please provide input for unit test
    // TODO JMP: argument Lambda is not explained in the documentation ? 
    /**
     * @brief Compute trend series using the Hodrick-Prescott method.
     *        
     *        $WsTrend VarFilename Lambda series1 series2 ...
     *        $WsTrendStd VarFilename Lambda series1 series2 ...
     *        
     * 
     * @see https://iode.plan.be/doku.php?id=wstrend and
     *      https://iode.plan.be/doku.php?id=wstrendstd 
     */
    void trend_correction(std::string& input_file, const double lambda, const std::string& series, const bool log);
};

/**
 * @brief compute a hash value for the database.
 * 
 * @note see https://www.boost.org/doc/libs/1_55_0/doc/html/hash/custom.html
 *       and https://www.boost.org/doc/libs/1_55_0/doc/html/hash/combine.html
 * 
 * @return std::size_t 
 */
inline std::size_t hash_value(KDBVariables const& cpp_kdb)
{
    KDB* kdb = cpp_kdb.get_KDB();
    if(kdb == NULL) return 0;

	SAMPLE* smpl = KSMPL(kdb);

    std::size_t seed = 0;
    for(int pos=0; pos < kdb->k_nb; pos++)
    {
        boost::hash_combine(seed, kdb->k_objs[pos].o_name);
        // KVVAL(kdb, pos, t) return a pointer to pointer to kdb[pos][t]. 
        // We need to compute the hash with the values of kdb[pos], not the pointers. 
        // Otherwise, hash_value() and hash_combine() will only compare pointer 
        // addresses and not the values.
		for(int t=0; t < smpl->s_nb; t++)
        	boost::hash_combine(seed, *KVVAL(kdb, pos, t));
    }
    return seed;
}

inline KDBVariables Variables;
