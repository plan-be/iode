#pragma once
#include "scr4/s_yy.h"      // YYKEYS

#include "api/constants.h"
#include "api/time/period.h"
#include "api/time/sample.h"
#include "api/objs/kdb.h"
#include "api/objs/scalars.h"
#include "api/objs/variables.h"
#include "api/utils/utils.h"
#include "api/lec/l_err.h"

#include <deque>            // for the execution stack
#include <algorithm>        // for std::min, std::max


enum GenericLecType
{
    GEN_LEC_OTHER,
    GEN_LEC_VALUE,
    GEN_LEC_EXECUTABLE
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

inline bool is_executable(const int type) 
{
    return type >= L_FN;
}

/*----------------- GLOBALS ----------------------*/

inline int L_curt = 0;              // current value of t
inline YYFILE* L_YY = nullptr;      // LEC stream the compiler is reading from

inline std::vector<std::string> L_NAMES;    // Table of names encountered in the current LEC expression (vars and scalars)

inline KDBScalarsPtr   L_EXEC_DBS = nullptr; 
inline KDBVariablesPtr L_EXEC_DBV = nullptr;   

inline char**  KEXEC_VFILES = NULL;
inline char**  KEXEC_SFILES = NULL;
inline int     KEXEC_TRACE = 0;

inline std::vector<int> V_EXEC_POS;


/* ---------------------- FUNCS ---------------------- */

/* l_exec.cpp */
int L_intlag(double lag);
bool L_stack_is_nan(const std::deque<double>& stack);
double L_exec_sub(unsigned char* expr, int lg, int t);

/* l_debug.cpp */
void L_debug(char*, ...);

/* k_lec.cpp */
char* L_expand(char* list_name);
inline char *(*L_expand_super)(char* list_name) = nullptr;

double *L_getvar(KDBVariablesPtr kdb, int pos);
double L_getscl(KDBScalarsPtr kdb, int pos);
Sample *L_getsmpl(KDBVariablesPtr kdb);
int L_findscl(KDBScalarsPtr kdb, const std::string& name);
int L_findvar(KDBVariablesPtr kdb, const std::string& name);

/* l_link.cpp */
void L_link_sample_expr(KDBVariablesPtr dbv, unsigned char* expr, short lg);


/*----------------- STRUCTS ----------------------*/

struct LEC_ABSTRACT
{
    int type;                       // type of the atomic lec (L_VAR, L_COEF, L_PERIOD, etc.)
    GenericLecType generic_type;      
    std::string representation;     // string representation of the atomic lec (for debugging purposes)

protected:
    LEC_ABSTRACT(const int type, const GenericLecType generic_type) 
        : type(type), generic_type(generic_type) {}
    LEC_ABSTRACT(const LEC_ABSTRACT& other) = default;

    bool is_same_type(const LEC_ABSTRACT& other) const
    {
        return this->type == other.type;
    }

public:
    // length in bytes of the executable representation of the atomic lec
    virtual int get_length() const = 0;

    // copies the executable representation of the atomic lec to the buffer at the given position and 
    // updates the position in the buffer
    virtual void add_to_buffer(unsigned char* buffer, int& pos_buffer) const 
    {
        // save the type of the expression element (1 byte)
        buffer[pos_buffer] = type;
        pos_buffer++;
    }
};

struct LEC_VALUE: public LEC_ABSTRACT
{
    LEC_VALUE(const int type) : LEC_ABSTRACT(type, GEN_LEC_VALUE) {}
    LEC_VALUE(const LEC_VALUE& other) = default;

    virtual bool add_to_stack(std::deque<double>& stack, const int t) const = 0;
};

struct LEC_EXECUTABLE: public LEC_ABSTRACT
{
    int nb_args;        // number of arguments of the function
    int pos;            // position of the function in the corresponding table (L_FN, L_TFN or L_MTFN)
    std::string fn_name;    // name of the function (for debugging purposes)

protected:
    LEC_EXECUTABLE(const int type, const int nb_args) 
    : LEC_ABSTRACT(type, GEN_LEC_EXECUTABLE), nb_args(nb_args), pos(-1) {}

    virtual void execute(unsigned char* expr, int j, int t, std::deque<double>& stack) = 0;
};

struct LEC_OTHER: public LEC_ABSTRACT
{
public:
    LEC_OTHER(const int type) : LEC_ABSTRACT(type, GEN_LEC_OTHER)
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

    // extract from the buffer starting at pos_buffer and update pos_buffer
    LEC_OTHER(const unsigned char* buffer, int& pos_buffer) 
        : LEC_ABSTRACT(L_SPECIAL, GEN_LEC_OTHER)
    {
        type = (int) buffer[pos_buffer];
        pos_buffer++;

        switch(type)
        {
            case L_EOE: 
                representation = "EOE"; 
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

    bool operator==(const LEC_OTHER& other) const
    {
        return is_same_type(other);
    }

    void add_to_buffer(unsigned char* buffer, int& pos_buffer) const override
    {
        // we don't save open and close parentheses in the executable expression
        if(type != L_OPENP && type != L_CLOSEP)
            LEC_ABSTRACT::add_to_buffer(buffer, pos_buffer);
    }

    int get_length() const override 
    {
        // we don't save open and close parentheses in the executable expression
        return (type == L_OPENP || type == L_CLOSEP) ? 0 : 1;
    }
};
