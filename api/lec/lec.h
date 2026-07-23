#pragma once

#include "api/lec/l_common.h"
#include "api/lec/l_value.h"
#include "api/lec/l_exec_fns.h"
#include "api/lec/l_exec_ops.h"
#include "api/lec/l_exec_tfn.h"
#include "api/lec/l_exec_val.h"
#include "api/lec/l_exec_mtfn.h"
#include "api/lec/l_err.h"
#include "api/lec/l_token.h"

#include <variant>              // for std::variant

/*----------------- GLOBALS ----------------------*/

using ATOMIC_LEC = std::variant<LEC_CONST_REAL, LEC_CONST_LONG, LEC_COEF, LEC_VAR, LEC_PERIOD, 
                                LEC_OTHER, LEC_OP, LEC_FN, LEC_TFN, LEC_VAL_FN, LEC_MTFN>;

inline std::vector<ATOMIC_LEC> L_EXPR;      // Table of pairs <type, atomic elements> 

/*---------------- STRUCTS ------------------------*/

struct CLEC
{
    // duplicate endogenous variable in the LEC expression
    char duplicated_endo = 0;

    // original LEC expression as a string (for debugging purposes)
    std::string lec;

    // 'executable' LEC expression
    int len_expr = 0;
    unsigned char* expression = NULL;
    std::vector<ATOMIC_LEC> v_expression;

    // list of pairs <scalar and variable name, positions in database>
    // NOTE: we don't use a std::map as we need to keep the order of the 
    //       names as they appear in the expression (for the link step) 
    std::vector<std::pair<std::string, int>> objs;

public:
    CLEC(const std::string& lec, std::vector<ATOMIC_LEC> v_expression, 
        unsigned char* expr, int len) 
    {
        this->lec = lec;
        this->v_expression = v_expression;

        len_expr = len;

        expression = new unsigned char[len];
        memset(expression, 0, len);
        if(expr)
            memcpy(expression, expr, len);

        // initialize all names with position -1 (not found)
        for(const std::string& name : L_NAMES)
            objs.push_back({name, -1});
    }

    CLEC(const CLEC& other) 
    {
        this->duplicated_endo = other.duplicated_endo;

        this->lec = other.lec;
        this->v_expression = other.v_expression;

        if(other.expression)
        {
            this->len_expr = other.len_expr;
            this->expression = new unsigned char[this->len_expr];
            memset(this->expression, 0, this->len_expr);
            memcpy(this->expression, other.expression, this->len_expr);
        }
        else
        {
            this->len_expr = 0;
            this->expression = nullptr;
        }

        this->objs = other.objs;
    }

    ~CLEC() 
    { 
        v_expression.clear();
        if(expression) 
            delete[] expression; 
    }

    // assignment operator (deep copy)
    CLEC& operator=(const CLEC& other) 
    {
        if(this == &other)
            return *this;

        this->duplicated_endo = other.duplicated_endo;

        this->lec = other.lec;
        this->v_expression = other.v_expression;

        if(other.expression)
        {
            this->len_expr = other.len_expr;
            this->expression = new unsigned char[this->len_expr];
            memset(this->expression, 0, this->len_expr);
            memcpy(this->expression, other.expression, this->len_expr);
        }
        else
        {
            this->len_expr = 0;
            if(this->expression)
                delete[] this->expression;
            this->expression = nullptr;
        }

        this->objs = other.objs;

        return *this;
    }

    // NOTE: we don't compare the lec attributes as they can be different but lead to the  
    // same executable expression (e.g. "D*2+ACAF" and "D * 2 + ACAF" are the same) 
    bool operator==(const CLEC& other) const
    {
        if(this->duplicated_endo != other.duplicated_endo)
            return false;

        if(this->v_expression.size() != other.v_expression.size())
            return false;

        for(size_t i = 0; i < this->v_expression.size(); ++i)
        {
            if(this->v_expression[i] != other.v_expression[i])
                return false;
        }

        if(this->len_expr != other.len_expr)
            return false;

        if(this->expression && other.expression)
        {
            if(memcmp(this->expression, other.expression, this->len_expr) != 0)
                return false;
        }
        else if(this->expression || other.expression) // only one of the two is null
            return false;

        return this->objs == other.objs;
    }

    std::vector<std::string> get_scalars() const
    {
        std::vector<std::string> list;
        for(const auto& [name, _] : this->objs)
        {
            if(is_coefficient(name))
                list.push_back(name);
        }
        return list;
    }

    std::vector<std::string> get_variables() const
    {
        std::vector<std::string> list;
        for(const auto& [name, _] : this->objs)
        {
            if(!is_coefficient(name))
                list.push_back(name);
        }
        return list;
    }
};

/* ---------------------- FUNCS ---------------------- */

/* l_cc1.c */
int L_cc1();
int L_sub_expr(const std::vector<ATOMIC_LEC>& v_alec, int close = -1);

/* l_cc2.c */
std::shared_ptr<CLEC> L_cc2(std::vector<ATOMIC_LEC>& expr, const std::string& lec);
std::shared_ptr<CLEC> L_cc_stream(const std::string& lec);
std::shared_ptr<CLEC> L_cc(const std::string& lec);

/* l_link.c */
int L_link(KDBVariablesPtr dbv, KDBScalarsPtr dbs, std::shared_ptr<CLEC>& cl);

/* l_exec.c */
#ifdef _MSC_VER
        int matherr(struct _exception *e);
#else
        // Define the exception structure
        struct exception {
        int type;       // Type of error
        char *name;     // Name of the function where the error occurred
        double arg1;    // First argument to the function
        double arg2;    // Second argument to the function (if applicable)
        double retval;  // Return value
        };
        
        int matherr(struct exception *e);
#endif
void L_fperror(int sig);
double L_exec(KDBVariablesPtr dbv, KDBScalarsPtr dbs, const std::shared_ptr<CLEC> clec, const int t);
double* L_cc_link_exec(char* lec, KDBVariablesPtr dbv, KDBScalarsPtr dbs);

/* l_hodrick.c */
int HP_calc(double *f_vec, double *t_vec, int nb, double lambda, int std);
void HP_test(double *f_vec, double *t_vec, int nb, int *beg, int *dim);

/* l_eqs.c */
std::shared_ptr<CLEC> L_solve(const std::string& eq, const std::string& endo);
int L_split_eq(const std::string& eq);

/* l_newton.c */
double L_zero(KDBVariablesPtr dbv, KDBScalarsPtr dbs, const std::shared_ptr<CLEC> clec, const int t, 
    const int varnb, const int eqvarnb);
double L_newton(KDBVariablesPtr dbv, KDBScalarsPtr dbs, const std::shared_ptr<CLEC> clec, const int t, 
    const int varnb, const int eqvarnb);

/* l_secant.c */
double L_secant(KDBVariablesPtr dbv, KDBScalarsPtr dbs, const std::shared_ptr<CLEC> clec, const int t, 
    const int varnb, const int eqvarnb);

/* k_lec.c */
bool print_lec_definition(const std::string& name, const std::string& eqlec, 
    const std::shared_ptr<CLEC> eqclec, const int coefs);
