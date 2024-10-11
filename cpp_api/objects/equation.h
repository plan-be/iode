#pragma once
#include <stdexcept>
#include <algorithm>

#include "cpp_api/common.h"
#include "cpp_api/utils/utils.h"
#include "cpp_api/time/period.h"
#include "cpp_api/time/sample.h"
#include "cpp_api/lec/lec.h"

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
    Equation(const std::string& name, const std::string& lec, const IodeEquationMethod method = EQ_LSQ, const std::string& from = "", 
        const std::string& to = "", const std::string& comment = "", const std::string& instruments = "", const std::string& block = "", 
        const bool date = true);
    
    Equation(const std::string& name, const std::string& lec, const std::string& method = "LSQ", const std::string& from = "", 
        const std::string& to = "", const std::string& comment = "", const std::string& instruments = "", const std::string& block = "", 
        const bool date = true);

    Equation(const EQ* c_other);
    Equation(const Equation& other);

    ~Equation();

    // required to be used in std::map
    Equation& operator=(const Equation& other);

    // -- endo --

    std::string get_endo() const;

    // -- lec --

    std::string get_lec() const;

    void set_lec(const std::string& lec);

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


// Custom specialization of std::hash can be injected in namespace std.
template<>
struct std::hash<EQ>
{
    std::size_t operator()(const EQ& eq) const noexcept
    {
		std::size_t seed = 0;

        // need to wrap with std::string() because hash_value() and
        // hash_combine() only compare pointer addresses when applied 
        // on char* arrays
        hash_combine<std::string_view>(seed, std::string_view(eq.endo, strlen(eq.endo)));
        hash_combine<std::string_view>(seed, std::string_view(eq.lec, strlen(eq.lec)));
        hash_combine<char>(seed, eq.method);
        hash_combine<SAMPLE>(seed, eq.smpl);
        hash_combine<std::string_view>(seed, std::string_view(eq.cmt, strlen(eq.cmt)));
        hash_combine<std::string_view>(seed, std::string_view(eq.blk, strlen(eq.blk)));
        hash_combine<std::string_view>(seed, std::string_view(eq.instr, strlen(eq.instr)));

        return seed;
    }
};

std::size_t hash_value(const Equation& equation);
