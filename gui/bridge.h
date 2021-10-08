#pragma once

#include "iode.h"

#include <QObject>
#include <QVariant>
#include <QString>
#include <QVector>
#include <QList>
#include <QMap>
#include <QUrl>
#include <QDir>
#include <QDialog>
#include <QSettings>
#include <QCloseEvent>
#include <QMessageBox>
#include <QCoreApplication>
#include <QDesktopServices>

#include <string>
#include <typeinfo>      // typeid
#include <unordered_map>


/* ****************************** *
 *           CONSTANTS            *
 * ****************************** */

const static char* ORGANIZATION_NAME = "Federal Planning Bureau";
const static char* APPLICATION_NAME = "IODE";
const static char* DEFAULT_INSTALLATION_DIR = "c:/iode";               // see function HLP_filename() in dos/o_help.c
const static char* IODE_WEBSITE = "https://iode.plan.be/doku.php";


/* ****************************** *
 *             ENUMS              *
 * ****************************** */

 /* NOTE FOR THE DEVELOPPERS:
  * enum documentation: https://en.cppreference.com/w/cpp/language/enum
  */

enum EnumIodeType
{
    COMMENTS,
    EQUATIONS,
    IDENTITIES,
    LISTS,
    SCALARS,
    TABLES,
    VARIABLES
};

const static int NB_IODE_TYPES = 7;


enum EnumLang
{
    ENGLISH,
    FRENCH,
    DUTCH
};

const static int NB_LANGS = 3;


enum EnumIodeFile
{
    COMMENTSFILE,
    EQUATIONSFILE,
    IDENTITIESFILE,
    LISTSFILE,
    SCALARSFILE,
    TABLESFILE,
    VARIABLESFILE,
    SETTINGSFILE,
    LOGSFILE,
    ANYFILE
};

const static int NB_IODE_FILES = 10;


/* ****************************** *
 *          IODE TYPES            *
 * ****************************** */

 // using is the C++11 version of typedef
using Comment = char;
using Equation = EQ;
using Identity = IDT;
using List = char;
using Scalar = SCL;
using Table = TBL;
using Variable = IODE_REAL;

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

    int count() const
    {
        if (K_WS[type] != NULL) {
            return K_WS[type]->k_nb;
        }
        else {
            return 0;
        }
    };

    char* getObjectName(int pos) const
    {
        if (K_WS[type] != NULL) {
            return const_cast<char*>(K_WS[type]->k_objs[pos].o_name);
        }
        else {
            return NULL;
        }
    };

    virtual T* getObjectValue(int pos) const = 0;

    // TODO: implement this method
    //virtual T& getObjectValueByName(char* name) const = 0;

    // TODO : implement this method
    //virtual void setObjectValue(T& value, int pos) const = 0;

    // TODO : overload subscript operator 
    //T& operator[](char* name) { ... }

    // TODO : overload subscript operator 
    //T& operator[](int pos) { ... }
};


class Comments : public AbstractIODEObjects<Comment>
{
public:
    Comments() : AbstractIODEObjects(COMMENTS) {};

    Comment* getObjectValue(int pos) const { return K_oval0(kdb(), pos); };
};


class Equations : public AbstractIODEObjects<Equation>
{
public:
    Equations() : AbstractIODEObjects(EQUATIONS) {};

    // TODO: create a Class Equation (with a destructor)
    Equation* getObjectValue(int pos) const { return NULL; };

    char* getLec(int pos) const { return KELEC(kdb(), pos); }
};


class Identities : public AbstractIODEObjects<Identity>
{
public:
    Identities() : AbstractIODEObjects(IDENTITIES) {};

    Identity* getObjectValue(int pos) const { return NULL; };

    char* getLec(int pos) const { return KILEC(kdb(), pos); };
};


class Lists : public AbstractIODEObjects<List>
{
public:
    Lists() : AbstractIODEObjects(LISTS) {};

    List* getObjectValue(int pos) const { return K_oval0(kdb(), pos); };
};


class Scalars : public AbstractIODEObjects<Scalar>
{
public:
    Scalars() : AbstractIODEObjects(SCALARS) {};

    Scalar* getObjectValue(int pos) const { return KSVAL(kdb(), pos); };
};


class Tables : public AbstractIODEObjects<Table>
{
public:
    Tables() : AbstractIODEObjects(TABLES) {};

    // TODO: create a Class Table (with a destructor)
    Table* getObjectValue(int pos) const { return NULL; };

    char* getTitle(int pos) const
    {
        Table* table = KTVAL(kdb(), pos);
        char* title = (char*)T_get_title(table);
        T_free(table);
        return title;
    }
};


class Variables : public AbstractIODEObjects<Variable>
{
public:
    Variables() : AbstractIODEObjects(VARIABLES) {};

    // TODO: not tested yet
    Variable* getObjectValue(int pos) const
    {
        return (Variable*)SW_getptr(kdb()->k_objs[pos].o_val);
    };

    IODE_REAL getValue(int pos, int t, int mode) const { return KV_get(kdb(), pos, t, mode); };

    int getNbPeriods() const { return KSMPL(kdb())->s_nb; }

    void getPeriod(char* period, int t) const
    {
        PER_pertoa(PER_addper(&(KSMPL(kdb())->s_p1), t), period);
    }
};


/* ****************************** *
 *          FUNCTIONS             *
 * ****************************** */

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
