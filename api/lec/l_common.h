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

    std::size_t last;
    std::array<double, capacity> values;

public:
    LECStack() : last(-1)
    { 
        values.fill(IODE_NAN);
    }

    LECStack(const LECStack& other) : last(other.last)
    {
        if(last >= 0)
            std::copy_n(other.values.begin(), last + 1, values.begin());
        else
            values.fill(IODE_NAN);
    }

    LECStack& operator=(const LECStack& other)
    {
        if(this != &other)
        {
            last = other.last;
            if(last >= 0)
                std::copy_n(other.values.begin(), last + 1, values.begin());
            else
                values.fill(IODE_NAN);
        }
        return *this;
    }

    void push_back(double value)
    {
        last++;
        if(last == capacity)
        {
            last--;
            throw std::overflow_error("LEC execution stack exceeded 1000 values");
        }
        values[last] = value;
    }

    void pop_back()
    {
        if(last < 0)
            throw std::underflow_error("LEC execution stack is empty");
        values[last] = 0.0;
        last--;
    }

    double back() const
    {
        if(last < 0)
            throw std::underflow_error("LEC execution stack is empty");
        return values[last];
    }

    bool is_nan(int shift) const
    { 
        std::size_t index = last - shift;
        if(index < 0)
        {
            std::string msg = "Cannot check if LEC execution stack is NaN: ";
            msg += "negative index";
            throw std::underflow_error(msg);
        }
        return !IODE_IS_A_NUMBER(values[index]);
    }

    auto begin() const 
    { 
        return values.begin(); 
    }

    auto end() const 
    { 
        return values.begin() + last + 1; 
    }
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

    virtual int get_nb_obs() const = 0;
    virtual void link_sample(KDBVariablesPtr dbv, const int start, const int length) = 0;
    virtual double execute_sub_expression(const int start, const int length, const int t) const = 0;
};

struct LEC_ABSTRACT
{
    int type;                       // type of the atomic lec (L_VAR, L_COEF, L_PERIOD, etc.)    
    bool is_executable;
    std::string representation;     // string representation of the atomic lec (for debugging purposes)

    virtual ~LEC_ABSTRACT() = default;

protected:
    LEC_ABSTRACT(const int type, const bool is_executable) 
        : type(type), is_executable(is_executable) {}
    
    LEC_ABSTRACT(const LEC_ABSTRACT& other) = default;

    bool is_same_type(const LEC_ABSTRACT& other) const
    {
        return this->type == other.type;
    }
};

struct LEC_ABSTRACT_VALUE: public LEC_ABSTRACT
{
protected:
    LEC_ABSTRACT_VALUE(const int type) : LEC_ABSTRACT(type, false) {}

    LEC_ABSTRACT_VALUE(const LEC_ABSTRACT_VALUE& other) = default;

public:
    ~LEC_ABSTRACT_VALUE() override = default;
};

struct LEC_ABSTRACT_EXECUTABLE: public LEC_ABSTRACT
{
    int nb_args;            // number of arguments of the function
    int pos;                // position of the function in the corresponding table (L_FN, L_TFN or L_MTFN)
    std::string fn_name;    // name of the function (for debugging purposes)

protected:
    LEC_ABSTRACT_EXECUTABLE(const int type, const int nb_args) 
        : LEC_ABSTRACT(type, true), nb_args(nb_args), pos(-1) {}
    
    LEC_ABSTRACT_EXECUTABLE(const LEC_ABSTRACT_EXECUTABLE& other) = default;

public:
    ~LEC_ABSTRACT_EXECUTABLE() override = default;
};

struct LEC_OTHER: public LEC_ABSTRACT
{
public:
    LEC_OTHER(const int type) : LEC_ABSTRACT(type, false)
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

    bool operator==(const LEC_OTHER& other) const
    {
        return is_same_type(other);
    }
};
