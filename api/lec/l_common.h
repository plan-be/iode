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
bool L_stack_is_nan(double* stack, const int pos_stack, int nargs);
double L_exec_sub(unsigned char* expr, int lg, int t, double* stack);

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
    std::string representation;     // string representation of the atomic lec (for debugging purposes)

protected:
    LEC_ABSTRACT(const int type) : type(type) {}

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

struct LEC_CONST_REAL: public LEC_ABSTRACT
{
    double value;

public:
    LEC_CONST_REAL(const double value) : LEC_ABSTRACT(L_DCONST), value(value) 
    {
        representation = std::to_string(value);
    }

    // extract from the buffer starting at pos_buffer and update pos_buffer
    LEC_CONST_REAL(const unsigned char* buffer, int& pos_buffer) : LEC_ABSTRACT(L_DCONST)
    {
        // the first byte is the type, we can skip it since we already know it's L_DCONST
        pos_buffer++;
        memcpy(&value, buffer + pos_buffer, sizeof(double));
        pos_buffer += sizeof(double);
        representation = std::to_string(value);
    }

    void add_to_stack(double* stack, int& pos_stack) const
    {
        pos_stack++;
        stack[pos_stack] = value;
    }

    void add_to_buffer(unsigned char* buffer, int& pos_buffer) const override
    {
        LEC_ABSTRACT::add_to_buffer(buffer, pos_buffer);
        memcpy(buffer + pos_buffer, &value, sizeof(double));
        pos_buffer += sizeof(double);
    }

    int get_length() const override 
    {
        return 1 + sizeof(double);
    }
};

struct LEC_CONST_LONG: public LEC_ABSTRACT
{ 
    long value;

public:
    LEC_CONST_LONG(const long value) : LEC_ABSTRACT(L_LCONST), value(value) 
    {
        representation = std::to_string(value);
    }

    // extract from the buffer starting at pos_buffer and update pos_buffer
    LEC_CONST_LONG(const unsigned char* buffer, int& pos_buffer) : LEC_ABSTRACT(L_LCONST)
    {
        // the first byte is the type, we can skip it since we already know it's L_LCONST
        pos_buffer++;
        memcpy(&value, buffer + pos_buffer, sizeof(long));
        pos_buffer += sizeof(long);
        representation = std::to_string(value);
    }

    void add_to_stack(double* stack, int& pos_stack) const
    {
        pos_stack++;
        stack[pos_stack] = (double) value;
    }

    void add_to_buffer(unsigned char* buffer, int& pos_buffer) const override
    {
        LEC_ABSTRACT::add_to_buffer(buffer, pos_buffer);
        memcpy(buffer + pos_buffer, &value, sizeof(long));
        pos_buffer += sizeof(long);
    }

    int get_length() const override 
    {
        return 1 + sizeof(long);
    }
};

struct LEC_COEF: public LEC_ABSTRACT
{
    int pos;          // position of the coefficient in V_EXEC_POS

public:
    LEC_COEF(const std::string& name, const int pos) : LEC_ABSTRACT(L_COEF), pos(pos) 
    {
        representation = name;
    }

    // extract from the buffer starting at pos_buffer and update pos_buffer
    LEC_COEF(const unsigned char* buffer, int& pos_buffer) : LEC_ABSTRACT(L_COEF)
    {
        // the first byte is the type, we can skip it since we already know it's L_COEF
        pos_buffer++;
        memcpy(&pos, buffer + pos_buffer, sizeof(int));
        pos_buffer += sizeof(int);
    }

    bool add_to_stack(double* stack, int& pos_stack) const
    {
        double value = L_getscl(L_EXEC_DBS, V_EXEC_POS[pos]);
        if(!IODE_IS_A_NUMBER(value)) 
            return false;

        pos_stack++;
        stack[pos_stack] = L_getscl(L_EXEC_DBS, V_EXEC_POS[pos]);
        return true;
    }

    void add_to_buffer(unsigned char* buffer, int& pos_buffer) const override
    {
        LEC_ABSTRACT::add_to_buffer(buffer, pos_buffer);
        memcpy(buffer + pos_buffer, &pos, sizeof(int));
        pos_buffer += sizeof(int);
    }

    int get_length() const override 
    {
        return 1 + sizeof(int);
    }
};

struct LEC_VAR: public LEC_ABSTRACT
{
    int pos;            // position of the variable in L_NAMES
    short lag;          // lag of the variable (0 if current value, 1 if t-1...)
    short ref;          
    Period per;         // period of the variable (if any)

public:
    // type = L_VAR or L_VART (variable with time) 
    LEC_VAR(const int type, const std::string& name, const int pos, const short lag, const Period& per)
        : LEC_ABSTRACT(type), pos(pos), lag(lag), ref(0), per(per) 
    {
        if(type != L_VAR && type != L_VART)
            throw std::invalid_argument("Invalid type for LEC_VAR: " + std::to_string(type));
        representation = name;
    }

    // extract from the buffer starting at pos_buffer and update pos_buffer
    LEC_VAR(const unsigned char* buffer, int& pos_buffer) : LEC_ABSTRACT(L_VAR)
    {
        type = (int) buffer[pos_buffer];
        pos_buffer++;
        memcpy(&pos, buffer + pos_buffer, sizeof(int));
        pos_buffer += sizeof(int);
        memcpy(&lag, buffer + pos_buffer, sizeof(short));
        pos_buffer += sizeof(short);
        memcpy(&ref, buffer + pos_buffer, sizeof(short));
        pos_buffer += sizeof(short);
        memcpy(&per, buffer + pos_buffer, sizeof(Period));
        pos_buffer += sizeof(Period);
    }

    void calculate_ref(const Sample& sample)
    {
        ref = lag;
        if(per.step != 0)
            ref += per.difference(sample.start_period);
    }

    bool add_to_stack(double* stack, int& pos_stack, const int t) const
    {
        double* d_ptr = L_getvar(L_EXEC_DBV, V_EXEC_POS[pos]);
        if(!d_ptr) 
            return false;

        int len = (int) ref;
        if(per.year == 0)  
            len += t;
        
        pos_stack++;
        if(len < 0 || len >= (L_getsmpl(L_EXEC_DBV))->nb_periods) 
            stack[pos_stack] = IODE_NAN;
        else 
            stack[pos_stack] = d_ptr[len];

        return true;
    }

    void add_to_buffer(unsigned char* buffer, int& pos_buffer) const override
    {
        LEC_ABSTRACT::add_to_buffer(buffer, pos_buffer);
        memcpy(buffer + pos_buffer, &pos, sizeof(int));
        pos_buffer += sizeof(int);
        memcpy(buffer + pos_buffer, &lag, sizeof(short));
        pos_buffer += sizeof(short);
        memcpy(buffer + pos_buffer, &ref, sizeof(short));
        pos_buffer += sizeof(short);
        memcpy(buffer + pos_buffer, &per, sizeof(Period));
        pos_buffer += sizeof(Period);
    }

    int get_length() const override 
    {
        int len = 1;                // for type
        len += sizeof(int);         // for pos
        len += sizeof(short) * 2;   // for lag and ref
        len += sizeof(Period);      // for period
        return len;
    }
};

// NOTE: pos is set in function L_link_sample_expr()
struct LEC_PERIOD: public LEC_ABSTRACT
{
    Period period;
    short pos;

public:
    LEC_PERIOD(const Period& period, const short pos) : LEC_ABSTRACT(L_PERIOD), pos(pos), period(period) 
    {
        representation = period.to_string();
    }
    // extract from the buffer starting at pos_buffer and update pos_buffer
    LEC_PERIOD(const unsigned char* buffer, int& pos_buffer) : LEC_ABSTRACT(L_PERIOD)
    {
        // the first byte is the type, we can skip it since we already know it's L_PERIOD
        pos_buffer++;
        memcpy(&period, buffer + pos_buffer, sizeof(Period));
        pos_buffer += sizeof(Period);
        memcpy(&pos, buffer + pos_buffer, sizeof(short));
        pos_buffer += sizeof(short);
        representation = period.to_string();
    }

    void calculate_pos(const Sample& sample)
    {
        pos = period.difference(sample.start_period);
    }

    void add_to_stack(double* stack, int& pos_stack) const
    {
        pos_stack++;
        stack[pos_stack] = (double) pos;
    }

    void add_to_buffer(unsigned char* buffer, int& pos_buffer) const override
    {
        LEC_ABSTRACT::add_to_buffer(buffer, pos_buffer);
        memcpy(buffer + pos_buffer, &period, sizeof(Period));
        pos_buffer += sizeof(Period);
        memcpy(buffer + pos_buffer, &pos, sizeof(short));
        pos_buffer += sizeof(short);
    }

    int get_length() const override 
    {
        return 1 + sizeof(Period) + sizeof(short);
    }
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

    // extract from the buffer starting at pos_buffer and update pos_buffer
    LEC_OTHER(const unsigned char* buffer, int& pos_buffer) : LEC_ABSTRACT(L_SPECIAL)
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

struct LEC_EXECUTABLE: public LEC_ABSTRACT
{
    int nb_args;        // number of arguments of the function
    int pos;            // position of the function in the corresponding table (L_FN, L_TFN or L_MTFN)

protected:
    LEC_EXECUTABLE(const int type, const int nb_args) : LEC_ABSTRACT(type), nb_args(nb_args), pos(-1) {}
};


constexpr int L_FN = L_SPECIAL_LAST + 1;

inline bool is_executable(const int type) 
{
    return type >= L_FN;
}
