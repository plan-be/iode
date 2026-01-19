#pragma once

#include "api/constants.h"
#include "api/time/period.h"
#include "api/time/sample.h"
#include "api/objs/kdb.h"
#include "api/objs/pack.h"

#include <string>

/*----------------------- TYPEDEF ----------------------------*/

// using is the C++11 version of typedef
using VAR = double*;
using Variable = std::vector<double>;

/*----------------------- DEFINE ----------------------------*/

#define LTOH_LIN    'L'
#define LTOH_CS     'C'
#define LTOH_STEP   'S'

const static std::vector<std::string> v_var_modes = 
    { "Level", "Differences", "Growth rates", "YoY Diffs", "YoY Grt" };

/*----------------------- ENUMS ----------------------------*/

enum IodeVarMode
{
    VAR_MODE_LEVEL,              //< no modification             x[t]
    VAR_MODE_DIFF,               //< difference on one period    (x[t]-x[t-1])
    VAR_MODE_GROWTH_RATE,        //< growth rate on one period   (x[t]/x[t-1] - 1)*100
    VAR_MODE_Y0Y_DIFF,           //< difference on one year      (x[t]-x[t-{nb sub periods}])
    VAR_MODE_Y0Y_GROWTH_RATE     //< growth rate on one year     (x[t]/x[t-{nb sub periods}] - 1) * 100
};

const static int IODE_NB_VAR_MODES = 5;

enum VariablesInitialization
{
    VAR_INIT_TM1,
    VAR_INIT_TM1_A,
    VAR_INIT_EXTRA,
    VAR_INIT_EXTRA_A,
    VAR_INIT_ASIS,
    VAR_INIT_TM1_NA,
    VAR_INIT_EXTRA_NA
};

enum IodeHighToLow
{
    HTOL_LAST,
    HTOL_MEAN,
    HTOL_SUM 
};

enum IodeLowToHigh
{
    LTOH_STOCK,
    LTOH_FLOW
};

/*----------------------- STRUCTS ----------------------------*/

struct KDBVariables : public KDBTemplate<double>
{
    /**
     *  Parameters specific to csv output files. 
     *  These parameters can be modified via report commands: 
     *    - $csvdigits
     *    - $csvsep
     *    - $csvdec
     *    - $csvnan
     *    - $csvaxes
     *  
     */
    static char* CSV_SEP;
    static char* CSV_DEC;
    static char* CSV_NAN;
    static char* CSV_AXES;
    static int   CSV_NBDEC;

private:
    void check_var_size(const std::string& action, const std::string& name, const Variable& variable)
    {
        int nb_periods = get_nb_periods();
	    if(variable.size() != nb_periods)
        {
            std::string error_msg = "Cannot " + action + " Variable '" + name + "'.\n";
            error_msg += "The size of the passed vector for values is ";
            error_msg += std::to_string(variable.size()) + " while the number of periods "; 
            error_msg += " is " + std::to_string(nb_periods);
            throw std::length_error(error_msg);
        }
    }

    Variable calculate_var_from_lec(const std::string& lec, const int t_first = 0, const int t_last = -1);
    Variable calculate_var_from_lec(const std::string& lec, const std::string& first_period = "", const std::string& last_period = "");

    bool load_asc_type_ask(const std::string& file_or_string, int type, int ask);

public:
    // global or standalone database
    KDBVariables(const bool is_global) : KDBTemplate(VARIABLES, is_global) {}

    // subset (shallow or deep copy) 
    KDBVariables(KDBVariables* db_parent, const std::string& pattern, const bool copy) 
        : KDBTemplate(db_parent, pattern, copy) {}

    // copy constructor
    KDBVariables(const KDBVariables& other): KDBTemplate(other) {}

    // NOTE: get_obj() and set_obj() methods to be replaced by operator[] when 
    //       k_objs will be changed to std::map<std::string, T>
    //       T& operator[](const std::string& name)

    double* get_obj(const SWHDL handle) const override;
    double* get_obj(const std::string& name) const override;
    double get_value(const std::string& name, const int t) const
    {
        double* var = this->get_obj(name);
        return *(var + t);
    }

    double* get_var_ptr(const std::string& name, const int t = 0)
    {
        double* var = this->get_obj(name);
        return var + t;
    }

    bool set_obj(const std::string& name, const double* value) override;
    bool set_obj(const std::string& name, const Variable& value);
    
    Variable get(const std::string& name) const;
    bool add(const std::string& name, const Variable& obj);
    void update(const std::string& name, const Variable& obj);

    double get_var(const std::string& name, const int t, const IodeVarMode mode = VAR_MODE_LEVEL) const;
    double get_var(const std::string& name, const std::string& period, const IodeVarMode mode = VAR_MODE_LEVEL) const;
    double get_var(const std::string& name, const Period& period, const IodeVarMode mode = VAR_MODE_LEVEL) const;
    
    bool set_var(const std::string& name, const double* value);
    void set_var(const std::string& name, const int t, const double value, const IodeVarMode mode = VAR_MODE_LEVEL);
    void set_var(const std::string& name, const std::string& period, const double value, const IodeVarMode mode = VAR_MODE_LEVEL);
    void set_var(const std::string& name, const Period& period, const double value, const IodeVarMode mode = VAR_MODE_LEVEL);

    bool add(const std::string& name, const std::string& lec);

    void update(const std::string& name, const std::string& lec);
    void update(const std::string& name, const Variable& values, const int t_first, const int t_last);
    void update(const std::string& name, const Variable& values, const std::string& first_period, const std::string& last_period);
    void update(const std::string& name, const std::string& lec, const int t_first, const int t_last);
    void update(const std::string& name, const std::string& lec, const std::string& first_period, const std::string& last_period);

    Sample* get_sample() const;

    bool check_sample() const
    {
        Sample* sample = get_sample();
        if(sample == nullptr || sample->nb_periods == 0)
        {
            kwarning("The Variables sample has not been yet defined");
            return false;
        } 
        return true;
    }

    void set_sample(const std::string& from, const std::string& to);
    void set_sample(const Period& from, const Period& to);

    int get_nb_periods() const;
    std::string get_period(const int t) const;
    float get_period_as_float(const int t) const;

    std::vector<std::string> get_list_periods(const std::string& from = "", 
        const std::string& to = "") const;

    std::vector<float> get_list_periods_as_float(const std::string& from = "", 
        const std::string& to = "") const;

    void copy_from(const std::string& input_file, const std::string& from = "", 
        const std::string& to = "", const std::string objects_names = "");
    
    void copy_from(const std::string& input_file, const Period* from = nullptr, 
        const Period* to = nullptr, const std::string& objects_names = "");

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
    void extrapolate(const VariablesInitialization method, const std::string& from, 
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
    void extrapolate(const VariablesInitialization method, const Period& from, 
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

    bool load_asc(const std::string& filename) override;
    bool save_asc(const std::string& filename) override;
    bool save_csv(const std::string& filename, const std::vector<std::string>& 
        varlist=std::vector<std::string>(), Sample* sample=nullptr) override;

    char* dde_create_obj_by_name(const std::string& name, int* nc, int* nl) override;

    bool print_obj_def(const std::string& name) override;

private:
    bool grep_obj(const std::string& name, const SWHDL handle, 
        const std::string& pattern, const bool ecase, const bool forms, 
        const bool texts, const char all) const override;
    
    void update_reference_db() override;
};

/*----------------------- GLOBALS ----------------------------*/
// unique_ptr -> automatic memory management
//            -> no need to delete KDB workspaces manually
inline std::unique_ptr<KDBVariables> global_ws_var = std::make_unique<KDBVariables>(true);

/*----------------------- FUNCS ----------------------------*/

double *K_vval(KDB *, int, int);
double *K_vptr(KDB *, char*, int); 

inline std::size_t hash_value(KDBVariables const& cpp_kdb)
{
    if(cpp_kdb.size() == 0)
        return 0;

	Sample* smpl = cpp_kdb.sample;
    if(!smpl)
        return 0;
    
    int nb_periods = smpl->nb_periods;
    if(nb_periods == 0)
        return 0;

    double* var;
    std::size_t seed = 0;
    for(const auto& [name, handle] : cpp_kdb.k_objs)
    {
        hash_combine<std::string>(seed, name);
        var = cpp_kdb.get_obj(name);
		for(int t=0; t < nb_periods; t++)
        	hash_combine<double>(seed, var[t]);
    }
    
    return seed;
}

/* k_wsvar.c */
int KV_sample(KDBVariables*, Sample*);
int KV_merge(KDBVariables*, KDBVariables*, int);
void KV_merge_del(KDBVariables*, KDBVariables*, int);
int KV_add(KDBVariables* kdb, char* varname);
double KV_get(KDBVariables*, const std::string& ,int ,int );
void KV_set(KDBVariables*, const std::string& ,int ,int ,double );
int KV_extrapolate(KDBVariables*, int, Sample*, char*);
KDBVariables* KV_aggregate(KDBVariables*,int ,char *,char *);
void KV_init_values_1(double* val, int t, int method);
//int KV_GetSmpl(Sample *,char *);
double KV_get_at_t(char*varname, int t);
double KV_get_at_per(char*varname, Period* per);
double KV_get_at_aper(char*varname, char* aper);
int KV_set_at_t(char*varname, int t, double val);
int KV_set_at_per(char*varname, Period* per, double val);
int KV_set_at_aper(char*varname, char* aper, double val);
        