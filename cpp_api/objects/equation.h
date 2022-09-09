#pragma once
#include "common.h"
#include "utils/utils.h"
#include "time/period.h"
#include "time/sample.h"


EQ* create_equation_deep_copy(EQ* original_equation);
bool equation_equal(EQ* c_eq1, EQ* c_eq2);
EQ* prepare_equation(const std::string& name, const bool add_obj, const std::string& lec, const std::string& comment,
    const std::string& method, Sample* sample, const std::string& instruments, const std::string& block, const std::array<float, EQS_NBTESTS>* tests,
    const bool date);

struct Equation
{
    EQ* c_equation;

public:
    Equation();

    Equation(const int pos, KDB* kdb = nullptr);

    Equation(const std::string& name, KDB* kdb = nullptr);

    Equation(const Equation& eq);

    ~Equation();

    // required to be used in std::map
    Equation& operator=(const Equation& eq);

    // -- lec --

    std::string get_lec() const;

    void set_lec(const std::string& lec, const std::string& endo);

    // -- solved --

    char get_solved() const;

    // -- method --

    int get_method_as_int() const;

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

    // -- tests --

    std::array<float, EQS_NBTESTS> get_tests() const;

    void set_tests(const std::array<float, EQS_NBTESTS> tests);

    // -- misc --

    std::vector<std::string> get_coefficients_list(const std::string& enddo);

    // -- operators --

    bool operator==(const Equation& other) const;
};
