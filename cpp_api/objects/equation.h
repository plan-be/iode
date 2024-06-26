#pragma once
#include <stdexcept>
#include <algorithm>
#include <boost/functional/hash.hpp>
#include <boost/algorithm/string.hpp>

#include "common.h"
#include "utils/utils.h"
#include "time/period.h"
#include "time/sample.h"
#include "lec/lec.h"


const static std::vector<std::string> v_eq_methods = { "LSQ", "ZELLNER", "INSTRUMENTAL", "GLS (3SLS)", "MAX_LIKELIHOOD" };


const static std::vector<std::string> vEquationTests = { 
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


struct Equation : public EQ
{
private:
    void copy_from_EQ_obj(const EQ* obj);

public:
    Equation(const int pos, KDB* kdb = nullptr);

    Equation(const std::string& name, KDB* kdb = nullptr);

    Equation(const std::string& name, const std::string& lec, const IodeEquationMethod method = EQ_LSQ, const std::string& from = "", 
        const std::string& to = "", const std::string& comment = "", const std::string& instruments = "", const std::string& block = "", 
        const bool date = true);
    
    Equation(const std::string& name, const std::string& lec, const std::string& method = "LSQ", const std::string& from = "", 
        const std::string& to = "", const std::string& comment = "", const std::string& instruments = "", const std::string& block = "", 
        const bool date = true);

    Equation(const Equation& other);

    ~Equation();

    // required to be used in std::map
    Equation& operator=(const Equation& other);

    // -- lec --

    std::string get_lec() const;

    void set_lec(const std::string& lec, const std::string& endo);

    // -- solved --

    char get_solved() const;

    // -- method --

    int get_method_as_int() const;

    void set_method(const IodeEquationMethod method);
    
    std::string get_method() const;

    void set_method(const std::string& method);

    // -- block --

    std::string get_block() const;

    void set_block(const std::string& block);

    // -- sample --

    Sample get_sample() const;

    void set_sample(std::string from = "", std::string to = "");

    // -- comment --

    std::string get_comment() const;

    void set_comment(const std::string& comment);

    // -- instruments --

    std::string get_instruments() const;

    void set_instruments(const std::string& instruments);

    // -- date --

    long get_date() const;

    std::string get_date_as_string(const std::string& date_format = "dd-mm-yyyy") const;

    void update_date();

    void set_date(const long date) { this->date = date; }

    void set_date(const std::string& date, const std::string& date_format = "dd-mm-yyyy");

    void reset_date() { date = 0L; }

    // -- tests --

    std::array<float, EQS_NBTESTS> get_tests() const;

    std::map<std::string, float> get_tests_as_map() const;
    
    float get_test(const IodeEquationTest t) const;

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

    void set_tests(const std::array<float, EQS_NBTESTS> tests);

    // -- misc --

    std::vector<std::string> get_coefficients_list(const bool create_if_not_exit=true);

    std::vector<std::string> get_variables_list(const bool create_if_not_exit=true);

    /**
     * @brief split an equation into its left and right hand sides
     * 
     * @return left and right hand sides
     * 
     * @note It is the same as o_calc_lhs() in dos/o_gr.c but rewritten in C++
     */
    std::pair<std::string, std::string> split_equation();

    // -- operators --

    bool operator==(const Equation& other) const;
};


struct NamedEquation
{
    std::string name;
    Equation eq;

    NamedEquation(const std::string& name);
    NamedEquation(const std::string& name, const Equation& eq);
};


/**
 * @brief compute a hash value for an object of type EQ (C API).
 * 
 * @note see https://www.boost.org/doc/libs/1_55_0/doc/html/hash/custom.html
 *       and https://www.boost.org/doc/libs/1_55_0/doc/html/hash/combine.html
 * 
 * @return std::size_t 
 */
std::size_t hash_value(EQ const& c_eq);

/**
 * @brief compute a hash value for an equation.
 * 
 * @note see https://www.boost.org/doc/libs/1_55_0/doc/html/hash/custom.html
 *       and https://www.boost.org/doc/libs/1_55_0/doc/html/hash/combine.html
 * 
 * @return std::size_t 
 */
std::size_t hash_value(Equation const& eq);