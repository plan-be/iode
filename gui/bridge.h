#pragma once

#include "../api/iode.h"
#include "utils.h"

#include <QString>

#include <string>
#include <vector>
#include <stdexcept>
#include <stdlib.h>

/* I hope to one day fully rewrite the IODE API into full pure C++ code instead of C but,
   in the meantime, we need bridge functions here
*/

// using is the C++11 version of typedef
using Comment = CMT;
using Equation = EQ;
using Identity = IDT;
using List = LIS;
using Scalar = SCL;
using Table = TBL;
using Variable = VAR;

using Sample = SAMPLE;


template <class T> class AbstractIODEObjects
{
protected:
    EnumIodeType type;

public:
    AbstractIODEObjects(EnumIodeType type) : type(type) {};

    KDB* kdb() const { return K_WS[type]; };

    int getIODEType() const { return type; };

    const char* getIODETypeName() { return qPrintable(qmapIodeTypes.keys(type)[0]); };
    
    const char* file() { return K_WS[type]->k_oname; };

    int count() const { return K_WS[type]->k_nb; };

    char* getObjectName(int pos) const { return const_cast<char*>(K_WS[type]->k_objs[pos].o_name); };
    
    // TODO : implement this method 
    //void setObjectName(char* name, int pos) { ... };
    
    virtual T getObjectValue(int pos) const = 0;
    
    // TODO : implement this method
    //virtual void setObjectValue(T value, int pos) const = 0;
};


class Comments : public AbstractIODEObjects<Comment>
{
public:
    Comments() : AbstractIODEObjects(COMMENTS) {};

    Comment getObjectValue(int pos) const { return K_oval0(kdb(), pos); };
};


class Equations : public AbstractIODEObjects<Equation*>
{
public:
    Equations() : AbstractIODEObjects(EQUATIONS) {};

    Equation* getObjectValue(int pos) const { return KEVAL(kdb(), pos); };

    char* getLec(int pos) const { return KELEC(kdb(), pos); }
};


class Identities : public AbstractIODEObjects<Identity>
{
public:
    Identities() : AbstractIODEObjects(IDENTITIES) {};

    Identity getObjectValue(int pos) const
    {
        Identity identity;
        identity.lec = KILEC(kdb(), pos);
        identity.clec = KICLEC(kdb(), pos);
        return identity;
    };

    char* getLec(int pos) const { return KILEC(kdb(), pos); };
};


class Lists : public AbstractIODEObjects<List>
{
public:
    Lists() : AbstractIODEObjects(LISTS) {};

    List getObjectValue(int pos) const { return K_oval0(kdb(), pos); };
};


class Scalars : public AbstractIODEObjects<Scalar*>
{
public:
    Scalars() : AbstractIODEObjects(SCALARS) {};

    Scalar* getObjectValue(int pos) const { return KSVAL(kdb(), pos); };
};


class Tables : public AbstractIODEObjects<Table*>
{
public:
    Tables() : AbstractIODEObjects(TABLES) {};

    // TODO: not tested yet
    Table* getObjectValue(int pos) const { return KTVAL(kdb(), pos); };

    char* getTitle(int pos) const 
    {
        Table* table = KTVAL(kdb(), pos);
        char* title = (char*) T_get_title(table);
        T_free(table);
        return title;
    }
};


class Variables : public AbstractIODEObjects<Variable>
{
public:
    Variables() : AbstractIODEObjects(VARIABLES) {};

    // TODO: not tested yet
    Variable getObjectValue(int pos) const 
    { 
        return (Variable) SW_getptr(kdb()->k_objs[pos].o_val);
    };

    IODE_REAL getValue(int pos, int t, int mode) const { return KV_get(kdb(), pos, t, mode); };

    int getNbPeriods() const { return KSMPL(kdb())->s_nb; }

    char* getPeriod(int t) const
    {
        char period[10];
        PER_pertoa(PER_addper(&(KSMPL(kdb())->s_p1), t), period);
        return period;
    }
};


inline void CPP_WsLoad(std::string arg, EnumIodeType type, std::string str_type)
{
    char* c_arg = const_cast<char*>(arg.c_str());

    int res = B_WsLoad(c_arg, type);
    if (res != EXIT_SUCCESS)
        throw std::runtime_error("Something went wrong when trying to import " + str_type + " from file " + arg);
}


inline void CPP_WsSave(std::string arg, EnumIodeType type, std::string str_type)
{
    char* c_arg = const_cast<char*>(arg.c_str());

    int res = B_WsSave(c_arg, type);
    if (res != EXIT_SUCCESS)
        throw std::runtime_error("Something went wrong when trying to save " + str_type + " to file " + arg);
}


inline void CPP_XodeRuleImport(EnumIodeType type, std::string trace, std::string rule, std::string ode, std::string asc, std::string from, std::string to, int fmt, int lang)
{
    char* c_trace = const_cast<char*>(trace.c_str());
    char* c_rule = const_cast<char*>(rule.c_str());
    char* c_ode = const_cast<char*>(ode.c_str());
    char* c_asc = const_cast<char*>(asc.c_str());
    char* c_from = const_cast<char*>(from.c_str());
    char* c_to = const_cast<char*>(to.c_str());

    int res = X_RuleImport(type, c_trace, c_rule, c_ode, c_asc, c_from, c_to, fmt, lang);
    if (res != EXIT_SUCCESS)
        throw std::runtime_error("Something went wrong when trying to import file " + asc);
}


inline void CPP_X_RuleExport(std::string trace, std::string rule, std::string out, std::string vfile, std::string cfile, std::string from, std::string to,
    std::string na, std::string sep, int fmt)
{
    char* c_trace = const_cast<char*>(trace.c_str());
    char* c_rule = const_cast<char*>(rule.c_str());
    char* c_out = const_cast<char*>(out.c_str());
    char* c_vfile = const_cast<char*>(vfile.c_str());
    char* c_cfile = const_cast<char*>(cfile.c_str());
    char* c_from = const_cast<char*>(from.c_str());
    char* c_to = const_cast<char*>(to.c_str());
    char* c_na = const_cast<char*>(na.c_str());
    char* c_sep = const_cast<char*>(sep.c_str());

    int res = X_RuleExport(c_trace, c_rule, c_out, c_vfile, c_cfile, c_from, c_to, c_na, c_sep, fmt);
    if (res != EXIT_SUCCESS)
        throw std::runtime_error("Something went wrong when trying to export file " + vfile);
}



