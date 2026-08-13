#pragma once
#include "scr4/s_yy.h"      // YYKEYS

#include "api/pch.h"
#include "api/time/period.h"
#include "api/time/sample.h"
#include "api/objs/kdb.h"
#include "api/objs/scalars.h"
#include "api/objs/variables.h"
#include "api/utils/utils.h"
#include "api/lec/l_err.h"


/*----------------- GLOBALS ----------------------*/

inline int L_curt = 0;              // current value of t

inline KDBScalarsPtr   L_EXEC_DBS = nullptr; 
inline KDBVariablesPtr L_EXEC_DBV = nullptr;   

inline char**  KEXEC_VFILES = NULL;
inline char**  KEXEC_SFILES = NULL;
inline int     KEXEC_TRACE = 0;

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

inline bool is_executable(const int type) 
{
    return type >= L_FN;
}

/* ---------------------- FUNCS ---------------------- */

/* l_exec.cpp */
int L_intlag(double lag);
bool L_stack_is_nan(const std::deque<double>& stack);

/* l_debug.cpp */
void L_debug(char*, ...);

/* k_lec.cpp */
char* L_expand(char* list_name);
inline char *(*L_expand_super)(char* list_name) = nullptr;


/*----------------- STRUCTS ----------------------*/

// Abstract class to be passed ot the execute() method of the sub-classes of TP_LEC_EXECUTABLE
struct AbstractCLEC
{
public:
    AbstractCLEC() {}

    virtual void link_sample(KDBVariablesPtr dbv, const int start, const int length) = 0;
    virtual double execute_sub_expression(const int start, const int length, const int t) = 0;
};

struct LEC_ABSTRACT
{
    int type;                       // type of the atomic lec (L_VAR, L_COEF, L_PERIOD, etc.)    
    std::string representation;     // string representation of the atomic lec (for debugging purposes)

protected:
    LEC_ABSTRACT(const int type) : type(type) {}
    LEC_ABSTRACT(const LEC_ABSTRACT& other) = default;

    bool is_same_type(const LEC_ABSTRACT& other) const
    {
        return this->type == other.type;
    }
};

template<typename... Args>
struct TP_LEC_VALUE: public LEC_ABSTRACT
{
    TP_LEC_VALUE(const int type) : LEC_ABSTRACT(type) {}
    TP_LEC_VALUE(const TP_LEC_VALUE& other) = default;

    virtual bool add_to_stack(std::deque<double>& stack, Args... args) const = 0;
};

struct LEC_EXECUTABLE: public LEC_ABSTRACT
{
    int nb_args;            // number of arguments of the function
    int pos;                // position of the function in the corresponding table (L_FN, L_TFN or L_MTFN)
    std::string fn_name;    // name of the function (for debugging purposes)

protected:
    LEC_EXECUTABLE(const int type, const int nb_args) 
        : LEC_ABSTRACT(type), nb_args(nb_args), pos(-1) {}
};

template<typename... Args>
struct TP_LEC_EXECUTABLE: public LEC_EXECUTABLE
{
protected:
    TP_LEC_EXECUTABLE(const int type, const int nb_args) : LEC_EXECUTABLE(type, nb_args) {}

    virtual void execute(std::deque<double>& stack, Args... args) = 0;
};

struct LEC_OTHER: public LEC_ABSTRACT
{
public:
    LEC_OTHER(const int type) : LEC_ABSTRACT(type)
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

    bool operator==(const LEC_OTHER& other) const
    {
        return is_same_type(other);
    }
};
