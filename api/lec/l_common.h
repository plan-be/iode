#pragma once
#include "api/iode_scr4.h"
#include <algorithm>
#include <array>
#include <memory>
#include <stdexcept>
#include "scr4/yy/s_yy.h"      // YYKEYS

#include "api/pch.h"
#include "api/time/period.h"
#include "api/time/sample.h"
#include "api/objs/kdb.h"
#include "api/objs/scalars.h"
#include "api/objs/variables.h"
#include "api/utils/utils.h"
#include "api/lec/l_err.h"


/*----------------- GLOBALS ----------------------*/

inline int KEXEC_TRACE = 0;

enum LECGenericType
{
    L_VALUE,
    L_EXECUTABLE,
    L_OTHER
};

constexpr int L_SPECIAL = 10;

/* LEC:SPECIAL */
enum LecSpecial
{
    L_EOE = L_SPECIAL,  // end of expression
    L_OPENP,            // open parenthesis
    L_CLOSEP,           // close parenthesis
    L_VAR,              // IODE Variable
    L_COEF,             // coefficient (IODE Scalar)
    L_OPENB,            // open bracket
    L_CLOSEB,           // close bracket
    L_COMMA,            // comma
    L_OCPAR,            // open-close parentheses
    L_PERIOD,           // period
    L_VART,             // IODE Variable with time expression (ex. A[1960Y1])
    L_COLON,            // colon (for equations)
    L_LCONST,           // long constant
    L_DCONST,           // double constant
    L_SQUARE            // - not used -
};

constexpr int L_SPECIAL_LAST = 29;

inline bool is_special_lec_elem(const int type) 
{
    return type >= L_SPECIAL && type < L_SPECIAL_LAST;
}

constexpr int L_FN = L_SPECIAL_LAST + 1;

class LECStack
{
    static constexpr std::size_t capacity = 1000;

    std::array<double, capacity> values;
    std::size_t top = 0;

public:
    LECStack() = default;

    LECStack(const LECStack& other) : top(other.top)
    {
        std::copy_n(other.values.begin(), top, values.begin());
    }

    LECStack& operator=(const LECStack& other)
    {
        if(this != &other)
        {
            top = other.top;
            std::copy_n(other.values.begin(), top, values.begin());
        }
        return *this;
    }

    void push_back(double value)
    {
        if(top == capacity)
            throw std::overflow_error("LEC execution stack exceeded 1000 values");
        values[top++] = value;
    }

    void pop_back()
    {
        if(top == 0)
            throw std::underflow_error("LEC execution stack is empty");
        values[top] = 0.0;
        --top;
    }

    double back() const
    {
        if(top == 0)
            throw std::underflow_error("LEC execution stack is empty");
        return values[top - 1];
    }

    bool is_nan(int shift) const
    {
        std::size_t index = top - shift;
        if(index < 0)
        {
            std::string msg = "Cannot check if LEC execution stack is NaN: ";
            msg += "negative index";
            throw std::underflow_error(msg);
        }
        return !IODE_IS_A_NUMBER(values[index]);
    }

    auto begin() const { return values.begin(); }
    auto end() const { return values.begin() + top; }
};

/* ---------------------- FUNCS ---------------------- */

/* l_exec.cpp */
int L_intlag(double lag);

/* l_debug.cpp */
void L_debug(char*, ...);

/* k_lec.cpp */
char* L_expand(char* list_name);
inline char *(*L_expand_super)(char* list_name) = nullptr;


/*----------------- STRUCTS ----------------------*/

// Abstract class to be passed ot the execute() method of the sub-classes of LEC_ABSTRACT_EXECUTABLE
struct AbstractCLEC
{
    // current time of execution (t) of the LEC
    int exec_t = 0;

public:
    AbstractCLEC() {}

    virtual int get_nb_obs() = 0;
    virtual void link_sample(KDBVariablesPtr dbv, const int start, const int length) = 0;
    virtual double execute_sub_expression(const int start, const int length, const int t) = 0;
};

struct LEC_ABSTRACT
{
    int type;                       // type of the atomic lec (L_VAR, L_COEF, L_PERIOD, etc.)    
    std::string representation;     // string representation of the atomic lec (for debugging purposes)
    LECGenericType generic_type;    // whether the current sub-class represents a value, an executable 
                                    // or neither a value or an executable 

    virtual ~LEC_ABSTRACT() = default;

    virtual std::unique_ptr<LEC_ABSTRACT> clone() const = 0;

    virtual bool equals(const LEC_ABSTRACT& other) const = 0;

protected:
    LEC_ABSTRACT(const int type, const LECGenericType generic_type) 
        : type(type), generic_type(generic_type) {}
    
    LEC_ABSTRACT(const LEC_ABSTRACT& other) = default;

    bool is_same_type(const LEC_ABSTRACT& other) const
    {
        return this->type == other.type;
    }
};

struct LEC_ABSTRACT_VALUE: public LEC_ABSTRACT
{
protected:
    LEC_ABSTRACT_VALUE(const int type) : LEC_ABSTRACT(type, L_VALUE) {}

    LEC_ABSTRACT_VALUE(const LEC_ABSTRACT_VALUE& other) = default;

public:
    ~LEC_ABSTRACT_VALUE() override = default;

    virtual double get_value(const int t) const = 0;
};

struct LEC_ABSTRACT_EXECUTABLE: public LEC_ABSTRACT
{
    int nb_args;            // number of arguments of the function
    int pos;                // position of the function in the corresponding table (L_FN, L_TFN or L_MTFN)
    std::string fn_name;    // name of the function (for debugging purposes)

protected:
    LEC_ABSTRACT_EXECUTABLE(const int type, const int nb_args) 
        : LEC_ABSTRACT(type, L_EXECUTABLE), nb_args(nb_args), pos(-1) {}
    
    LEC_ABSTRACT_EXECUTABLE(const LEC_ABSTRACT_EXECUTABLE& other) = default;

public:
    ~LEC_ABSTRACT_EXECUTABLE() override = default;

    virtual double execute(LECStack& stack, int t, int& expr_pos,
        AbstractCLEC& clec) const = 0;
};

struct LEC_OTHER: public LEC_ABSTRACT
{
public:
    LEC_OTHER(const int type) : LEC_ABSTRACT(type, L_OTHER)
    {
        switch(type)
        {
            case L_EOE: 
                representation = "EOE"; 
                break;
            case L_OPENP: 
                representation = "("; 
                break;
            case L_CLOSEP: 
                representation = ")"; 
                break;
            case L_COMMA: 
                representation = ","; 
                break;
            case L_OPENB: 
                representation = "["; 
                break;
            case L_CLOSEB: 
                representation = "]"; 
                break;
            case L_OCPAR: 
                representation = "()"; 
                break;
            case L_COLON: 
                representation = ":"; 
                break;
            default: 
                throw std::invalid_argument("Invalid type for LEC_OTHER: " + std::to_string(type));
                break;
        }
    }

    LEC_OTHER(const LEC_OTHER& other) = default;

    ~LEC_OTHER() override = default;

    std::unique_ptr<LEC_ABSTRACT> clone() const override 
    { 
        return std::make_unique<LEC_OTHER>(*this); 
    }

    bool equals(const LEC_ABSTRACT& other) const override
    {
        const auto* rhs = dynamic_cast<const LEC_OTHER*>(&other);
        return rhs && *this == *rhs;
    }

    bool operator==(const LEC_OTHER& other) const
    {
        return is_same_type(other);
    }
};
