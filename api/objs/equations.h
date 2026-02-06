#pragma once

#include "scr4/s_prodt.h"

#include "api/constants.h"
#include "api/k_super.h"
#include "api/time/period.h"
#include "api/time/sample.h"
#include "api/lec/lec.h"
#include "api/objs/kdb.h"
#include "api/objs/objs.h"
#include "api/objs/scalars.h"
#include "api/objs/variables.h"

#include <string>
#include <array>
#include <vector>
#include <utility>      // std::pair

/*----------------------- DEFINE ----------------------------*/

inline int B_EQS_INFOS;    // Information detail to print (for equations)
                           //    0: equation only 
                           //    1: equation + comment
                           //    2: equation + comment + estimation results
inline int B_EQS_LEC;      // Specify how to print a LEC expression 
                           //    0 : print the LEC form as is
                           //    1 : replace all scalars by their values
                           //    2 : replaced all scalars by their values + t-tests

/*----------------------- ENUMS ----------------------------*/

enum IodeEquationMethod
{
    EQ_LSQ,
    EQ_ZELLNER,
    EQ_INSTRUMENTAL,
    EQ_GLS,
    EQ_MAX_LIKELIHOOD
};

const static std::vector<std::string> v_eq_methods = 
{ "LSQ", "ZELLNER", "INSTRUMENTAL", "GLS (3SLS)", "MAX_LIKELIHOOD" };


const static int IODE_NB_EQ_METHODS = 5;

enum IodeEquationTest
{
    EQ_CORR,
    EQ_STDEV,
    EQ_MEANY,
    EQ_SSRES,
    EQ_STDERR,
    EQ_STDERRP,
    EQ_FSTAT,
    EQ_R2,
    EQ_R2ADJ,
    EQ_DW,
    EQ_LOGLIK,
};

const static std::vector<std::string> v_eq_tests_names = 
{ 
    "corr", "stdev", "meany", "ssres", "stderr", "stderrp",
    "fstat", "r2", "r2adj", "dw", "loglik"
};

const static std::vector<std::string> v_eq_tests_fullnames = 
{ 
    "Correlation",
    "St dev of residuals",
    "Mean of YOBS",
    "∑ Residuals²",
    "Standard error",
    "Standard error in %",
    "F-stat",
    "R²",
    "R² adjusted",
    "Durbin-Watson",
    "Log Likelihood"
};

const static int IODE_NB_EQ_TESTS = 11;

enum IodeEquationAscii
{
    EQ_ASCII_BLK,
    EQ_ASCII_CMT,
    EQ_ASCII_INSTR,
    EQ_ASCII_METH,
    EQ_ASCII_SMPL,
    EQ_ASCII_LSQ,
    EQ_ASCII_ZEL,
    EQ_ASCII_INF,
    EQ_ASCII_GLS,
    EQ_ASCII_DATE,
    EQ_ASCII_OPEN,
    EQ_ASCII_CLOSE,
    EQ_ASCII_STDEV,
    EQ_ASCII_MEANY,
    EQ_ASCII_SSRES,
    EQ_ASCII_STDERR,
    EQ_ASCII_FSTAT,
    EQ_ASCII_R2,
    EQ_ASCII_R2ADJ,
    EQ_ASCII_DW,
    EQ_ASCII_LOGLIK,
    EQ_ASCII_MAXLIK
};

/*----------------------- STRUCTS ----------------------------*/

// EQ = Equation (struct continaing a LEC equation, its compled form (CLEC), the estimation parameter and tests...)
//
// WARNING about the method property. 
// Before using it, check that the method property value is in the appropriate range: in the very first versions of iode,
// the allowed values for method were 'l', 'z', instead of 0, 1...
struct Equation 
{
    std::string   endo;                     // endogenous variable (= equation name)   
    std::string   lec;                      // LEC form of the equation (LHS := RHS)
    CLEC*         clec;                     // Compiled equation for the simulation
    char          solved;                   // Indicates if in clec, the equation is solved with respect to its endogenous (e.g.: "ln X := RHS" => "X := exp(RHS)")
    char          method;                   // Estimation method
    Sample        sample;                   // Estimation sample
    std::string   comment;                  // Free comment
    std::string   block;                    // List of equations estimated simultaneously
    std::string   instruments;              // List of instruments used to modify metric in the estimation process (INSTR method)
    long          date;                     // Estimation date
    std::array<float, EQS_NBTESTS> tests;   // Estimation tests

public:
    Equation(const std::string& name, const std::string& lec, const IodeEquationMethod method, const std::string& from, 
        const std::string& to, const std::string& comment, const std::string& instruments, const std::string& block, const bool date)
    {
        this->clec = NULL;
        this->date = 0L;
        this->solved = '\0';

        this->endo = name;
        // set this->lec and this->clec in the same time
        set_lec(lec);
        set_method(method);
        set_sample(from, to);
        set_comment(comment);
        this->instruments = instruments;
        this->block = block; 
        this->tests.fill(0.0f);
        if(date)
            update_date();
    }

    Equation(const std::string& name, const std::string& lec, const std::string& method, const std::string& from, 
        const std::string& to, const std::string& comment, const std::string& instruments, const std::string& block, const bool date)
    {
        this->clec = NULL;
        this->date = 0L;
        this->solved = '\0';

        this->endo = name;
        // set this->lec and this->clec in the same time
        set_lec(lec);
        set_method(method);
        set_sample(from, to);
        set_comment(comment);
        this->instruments = instruments;
        this->block = block; 
        this->tests.fill(0.0f);
        if(date)
            update_date();
    }

    Equation(const Equation& other)
    {
        this->endo = other.endo; 
        this->lec = other.lec;
        // NOTE : we do not use memcpy() because memcpy() actually makes  
        //        a shallow copy of a struct instead of a deep copy
        this->clec = clec_deep_copy(other.clec);
        this->solved = other.solved;
        this->method = other.method;
        if(this->method < 0 || this->method >= IODE_NB_EQ_METHODS)
            this->method = EQ_LSQ;  // Default method is LSQ
        this->sample = other.sample;
        this->comment = other.comment;
        this->block = other.block;
        this->instruments = other.instruments;
        this->date = other.date;
        this->tests = other.tests;
    }

    ~Equation()
    {
        if(this->clec != NULL)
            SW_nfree(this->clec);
    }

    // required to be used in std::map
    Equation& operator=(const Equation& other)
    {
        this->endo = other.endo; 
        this->lec = other.lec;
        this->compile();
        this->solved = other.solved;
        this->method = other.method;
        if(this->method < 0 || this->method >= IODE_NB_EQ_METHODS)
            this->method = EQ_LSQ;  // Default method is LSQ
        this->sample = other.sample;
        this->comment = other.comment;
        this->block = other.block;
        this->instruments = other.instruments;
        this->date = other.date;
        this->tests = other.tests;
        return *this;
    }

    // -- clec --

    void compile()
    {
        if(this->lec.empty())
            throw std::invalid_argument("LEC expression is empty");
        
        // check if LEC expression is valid
        CLEC* new_clec = L_solve((char*) this->lec.c_str(), (char*) this->endo.c_str());
        if(new_clec == NULL)
        {
            std::string error_msg = "Cannot compile the LEC expression '" + lec + "' "; 
            error_msg += "of the equation named '" + endo + "'";
            throw std::invalid_argument(error_msg);
        }

        if(this->clec != NULL)
            SW_nfree(this->clec);

        this->clec = new_clec;
    }

    // -- lec --

    void set_lec(const std::string& lec)
    {
        if(lec.empty())
            throw std::invalid_argument("Passed LEC expression is empty");
        
        this->lec = lec;

        // recompile the CLEC form of the equation each time the LEC form is updated
        this->compile();
    }

    // -- method --

    int get_method_as_int() const
    {
        int m = (int) this->method;
        return (m >= 0 && m < IODE_NB_EQ_METHODS) ? m : 0;
    }

    void set_method(const IodeEquationMethod method)
    {
        this->method = (char) method;
    }

    std::string get_method() const
    {
        int m = (int) this->method;

        if(m >= 0 && m < IODE_NB_EQ_METHODS)
            return v_eq_methods[m];
        else
            // invalid value
            return "";
    }

    void set_method(const std::string& method)
    {
        int m = -1;
        for(int i = 0; i < IODE_NB_EQ_METHODS; i++) 
            if(method == v_eq_methods[i]) m = i;

        if(m < 0)
            throw std::invalid_argument("The equation method '" + method + "' is not valid.\n" + 
                "Accepted methods are: " + join(v_eq_methods, ", "));

        this->method = (char) m;
    }

    // -- sample --

    void set_sample(std::string from = "", std::string to = "")
    {
        if(from.empty() && to.empty())
        {
            this->sample.nb_periods = 0;
            this->sample.start_period.year = 0;
            this->sample.start_period.periodicity = '\0';
            this->sample.start_period.step = 0;
            this->sample.end_period.year = 0;
            this->sample.end_period.periodicity = '\0';
            this->sample.end_period.step = 0;
            return;
        }
         
        if(from.empty() || to.empty())
        {
            Sample* c_vars_sample = global_ws_var->sample;
            if(c_vars_sample == NULL || c_vars_sample->nb_periods == 0)
            {
                this->sample.nb_periods = 0;
                this->sample.start_period.year = 0;
                this->sample.start_period.periodicity = '\0';
                this->sample.start_period.step = 0;
                this->sample.end_period.year = 0;
                this->sample.end_period.periodicity = '\0';
                this->sample.end_period.step = 0;
                kwarning("Variables sample not yet set. Set equation sample to 0.");
                return;
            }

            Sample vars_sample(*c_vars_sample);

            if(from.empty()) 
                from = vars_sample.start_period.to_string();

            if(to.empty())   
                to = vars_sample.end_period.to_string();
        }

        Sample new_sample(from, to);
        this->sample.start_period = new_sample.start_period;
        this->sample.end_period = new_sample.end_period;
        this->sample.nb_periods = new_sample.nb_periods;
    }

    // -- comment --

    std::string get_comment() const
    {
        std::string comment_oem = std::string(this->comment);
        std::string comment = oem_to_utf8(comment_oem);
        return comment;
    }

    // we assume that comment string is in UTF8 format
    void set_comment(const std::string& comment)
    {
        this->comment.clear();
        this->comment = comment.empty() ? "" : utf8_to_oem(comment);
    }

    // -- date --

    std::string get_date_as_string(const std::string& date_format = "dd-mm-yyyy") const
    {
        std::string date;
        // note: dates are stored as follow: yyyymmdd
        //       for example, "18-06-2024" will be stored as long value 20240618 
        long l_date = this->date;
        if(l_date > 0)
        {
            char c_date[12];
            char* c_date_format;
            c_date_format = to_char_array(date_format);
            date = SCR_long_to_fdate(l_date, c_date, c_date_format);
        }

        return date;
    }

    void update_date()
    {
        this->date = SCR_current_date();
    }

    void set_date(const std::string& date, const std::string& date_format = "dd-mm-yyyy")
    {
        // note: dates are stored as follow: yyyymmdd
        // for example, "18-06-2024" will be stored as long value 20240618 
        this->date = SCR_fdate_to_long(to_char_array(date), to_char_array(date_format));
    }

    void reset_date() { date = 0L; }

    // -- tests --

    std::map<std::string, float> get_tests_as_map() const
    {
        std::map<std::string, float> m_tests;
        m_tests["corr"] = this->tests[EQ_CORR];
        m_tests["stdev"] = this->tests[EQ_STDEV];
        m_tests["meany"] = this->tests[EQ_MEANY];
        m_tests["ssres"] = this->tests[EQ_SSRES];
        m_tests["stderr"] = this->tests[EQ_STDERR];
        m_tests["stderrp"] = this->tests[EQ_STDERRP];
        m_tests["fstat"] = this->tests[EQ_FSTAT];
        m_tests["r2"] = this->tests[EQ_R2];
        m_tests["r2adj"] = this->tests[EQ_R2ADJ];
        m_tests["dw"] = this->tests[EQ_DW];
        m_tests["loglik"] = this->tests[EQ_LOGLIK];
        return m_tests;
    }

    float get_test(const IodeEquationTest t) const
    {
        if(t < 0 || t >= EQS_NBTESTS)
            throw std::invalid_argument("get_test: the passed value must be in range [0, " + 
                std::to_string(EQS_NBTESTS-1) + "]");
        return this->tests[t];
    }

    float get_test_stdev() const { return this->tests[EQ_STDEV]; }
    float get_test_meany() const { return this->tests[EQ_MEANY]; }
    float get_test_ssres() const { return this->tests[EQ_SSRES]; }
    float get_test_stderr() const { return this->tests[EQ_STDERR]; }
    float get_test_fstat() const { return this->tests[EQ_FSTAT]; }
    float get_test_r2() const { return this->tests[EQ_R2]; }
    float get_test_r2adj() const { return this->tests[EQ_R2ADJ]; }
    float get_test_dw() const { return this->tests[EQ_DW]; }
    float get_test_loglik() const { return this->tests[EQ_LOGLIK]; }

    void set_test(const IodeEquationTest t, const float value)
    {
        this->tests[t] = value;
    }

    void reset_tests() 
    { 
        this->tests.fill(0.0f); 
    }

    // -- misc --

    std::vector<std::string> get_coefficients_list(const bool create_if_not_exit=true)
    {
        std::vector<std::string> coeffs = get_scalars_from_clec(this->clec);

        // create scalars not yet present in the Scalars Database
        if(create_if_not_exit)
        {
            for(const std::string& coeff_name: coeffs)
            {
                // adds a new scalar with values { 0.9, 1.0, IODE_NAN } 
                // to the Scalars Database
                if (!global_ws_scl->contains(coeff_name)) 
                    global_ws_scl->set_obj_ptr(coeff_name, new Scalar(0.9, 1.0));
            }
        }

        return coeffs;
    }

    std::vector<std::string> get_variables_list(const bool create_if_not_exit=true)
    {
        std::vector<std::string> vars = get_variables_from_clec(this->clec);

        // create variables not yet present in the Variables Database
        if(create_if_not_exit)
        {
            Sample* sample = global_ws_var->sample;
            if(sample == NULL || sample->nb_periods == 0)
                throw std::runtime_error("Cannot return the list of variables.\nThe global sample is not yet defined.");

            int nb_obs = sample->nb_periods;
            for(const std::string& var_name: vars)
            {
                // adds a new variable with nb_obs IODE_NAN values to the 
                // Variables database
                Variable* var_ptr = new Variable(nb_obs, IODE_NAN);
                if (!global_ws_var->contains(var_name)) 
                    global_ws_var->set_obj_ptr(var_name, var_ptr);
            }
        }

        return vars;
    }

    /**
     * @brief split an equation into its left and right hand sides
     * 
     * @return left and right hand sides
     * 
     * @note It is the same as o_calc_lhs() in dos/o_gr.c but rewritten in C++
     */
    std::pair<std::string, std::string> split_equation()
    {
        std::pair<std::string, std::string> lrhs;

        // same as L_split_eq()
        size_t pos = this->lec.find(":=");
        // test if := not found -> return
        if(pos == std::string::npos) 
            return lrhs;
        // left hand side
        lrhs.first = this->lec.substr(0, pos);
        lrhs.first = trim(lrhs.first);
        // right hand side
        lrhs.second = this->lec.substr(pos+2);
        lrhs.second = trim(lrhs.second);

        return lrhs;
    }

    bool print_definition() const;

    bool to_binary(char** pack) const;

    // -- operators --

    bool operator==(const Equation& other) const
    {
        /* same unlinked CLEC, method, sample, cmt, instr, blk, --plus tests -- */
        char* name = (char*) this->endo.c_str();
        CLEC* cl1 = L_solve((char*) this->lec.c_str(), name);
        CLEC* cl2 = L_solve((char*) other.lec.c_str(), name);

        if(cl1 == NULL || cl2 == NULL
            || cl1->tot_lg != cl2->tot_lg
            || memcmp(cl1, cl2, cl1->tot_lg) != 0) 
                return false;

        if(this->method != other.method) 
            return false;

        if(this->sample != other.sample) 
            return false;

        if(this->block != other.block) 
            return false;

        if(this->instruments != other.instruments) 
            return false;

        return true;
    }
};

Equation* binary_to_eqs(char* pack, const std::string& name);

// Custom specialization of std::hash can be injected in namespace std.
template<>
struct std::hash<Equation>
{
    std::size_t operator()(const Equation& eq) const noexcept
    {
		std::size_t seed = 0;

        hash_combine<std::string>(seed, eq.endo);
        hash_combine<std::string>(seed, eq.lec);
        hash_combine<char>(seed, eq.method);
        hash_combine<Sample>(seed, eq.sample);
        hash_combine<std::string>(seed, eq.comment);
        hash_combine<std::string>(seed, eq.block);
        hash_combine<std::string>(seed, eq.instruments);

        return seed;
    }
};

inline std::size_t hash_value(const Equation& equation)
{
    std::hash<Equation> eq_hash;
    return eq_hash(equation);
}


/*----------------------- STRUCTS ----------------------------*/

struct KDBEquations : public KDBTemplate<Equation>
{
    // global or standalone database
    KDBEquations(const bool is_global) : KDBTemplate(EQUATIONS, is_global) {}

    // subset (shallow or deep copy) 
    KDBEquations(KDBEquations* db_parent, const std::string& pattern, const bool copy) 
        : KDBTemplate(db_parent, pattern, copy) {}

    // copy constructor
    KDBEquations(const KDBEquations& other): KDBTemplate(other) {}

    std::string get_lec(const std::string& name) const;

    bool add(const std::string& name, const Equation& eq) override 
    { 
        return KDBTemplate::add(name, eq); 
    }

    void update(const std::string& name, const Equation& eq) override
    {
        KDBTemplate::update(name, eq);
    }

    bool add(const std::string& name, const std::string& lec);
    void update(const std::string& name, const std::string& lec);

    bool load_asc(const std::string& filename) override;
    bool save_asc(const std::string& filename) override;

    char* dde_create_obj_by_name(const std::string& name, int* nc, int* nl) override;

    bool print_obj_def(const std::string& name) override;

    void merge_from(const std::string& input_file) override
    {
        KDBEquations from(false);
        KDBTemplate::merge_from(from, input_file);
    }

    bool copy_from_file(const std::string& file, const std::string& objs_names, 
        std::set<std::string>& v_found)
    {
        KDBEquations from(false);
        return KDBTemplate::copy_from_file(from, file, objs_names, v_found);
    }

private:
    bool binary_to_obj(const std::string& name, char* pack) override;
    bool obj_to_binary(char** pack, const std::string& name) override;

    bool grep_obj(const std::string& name, const std::string& pattern, 
        const bool ecase, const bool forms, const bool texts, const char all) const override;
    
    void update_reference_db() override;
};

/*----------------------- GLOBALS ----------------------------*/
// unique_ptr -> automatic memory management
//            -> no need to delete KDB workspaces manually
inline std::unique_ptr<KDBEquations> global_ws_eqs = std::make_unique<KDBEquations>(true);
inline std::array<KDBEquations*, 5> global_ref_eqs = { nullptr };

/*----------------------- FUNCS ----------------------------*/

inline std::size_t hash_value(KDBEquations const& cpp_kdb)
{
    if(cpp_kdb.size() == 0)
        return 0;

    std::size_t seed = 0;
    for(const auto& [name, eq_ptr] : cpp_kdb.k_objs)
    {
        hash_combine<std::string>(seed, name);
        hash_combine<Equation>(seed, *eq_ptr);
    }

    return seed;
}

/* k_eqs.c */
int E_split_eq(char *,char **,char **);
int E_dynadj(int ,char *,char *,char *,char **);
int E_DynamicAdjustment(int ,char **,char *,char *);

// Estimation tests by equation name
double K_etest(KDBEquations* kdb, char*name, int test_nb);
double K_e_stdev (KDBEquations* kdb, char*name);
double K_e_meany (KDBEquations* kdb, char*name);
double K_e_ssres (KDBEquations* kdb, char*name);
double K_e_stderr(KDBEquations* kdb, char*name);
double K_e_fstat (KDBEquations* kdb, char*name);
double K_e_r2    (KDBEquations* kdb, char*name);
double K_e_r2adj (KDBEquations* kdb, char*name);
double K_e_dw    (KDBEquations* kdb, char*name);
double K_e_loglik(KDBEquations* kdb, char*name);

/*----------------------- FUNCS ----------------------------*/

int K_epack(char **,char *,char *);

/* lec/l_link.c */
void L_link_endos(KDBEquations* dbe, CLEC *cl);
