#pragma once
#include "time/period.h"
#include "time/sample.h"
#include "kdb_template.h"


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

    IODE_REAL get_var(const int pos, const int t, const int mode = K_LEVEL) const;

    IODE_REAL get_var(const int pos, const std::string& period, const int mode = K_LEVEL) const;

    IODE_REAL get_var(const int pos, const Period& period, const int mode = K_LEVEL) const;

    IODE_REAL get_var(const std::string& name, const int t, const int mode = K_LEVEL) const;

    IODE_REAL get_var(const std::string& name, const std::string& period, const int mode = K_LEVEL) const;

    IODE_REAL get_var(const std::string& name, const Period& period, const int mode = K_LEVEL) const;

    void set_var(const int pos, const int t, const IODE_REAL value, const int mode = K_LEVEL);

    void set_var(const int pos, const std::string& period, const IODE_REAL value, const int mode = K_LEVEL);

    void set_var(const int pos, const Period& period, const IODE_REAL value, const int mode = K_LEVEL);

    void set_var(const std::string& name, const int t, const IODE_REAL value, const int mode = K_LEVEL);

    void set_var(const std::string& name, const std::string& period, const IODE_REAL value, const int mode = K_LEVEL);

    void set_var(const std::string& name, const Period& period, const IODE_REAL value, const int mode = K_LEVEL);

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

    Period get_period(const int t) const;

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
		for(int t=0; t < smpl->s_nb; t++)
        	boost::hash_combine(seed, KVVAL(kdb, pos, t));
    }
    return seed;
}
