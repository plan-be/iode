#pragma once

#include "iode.h"
#ifdef _WIN32 // valid for both 32 and 64 bits
    #include <Windows.h>
#elif
    #define CP_OEMCP -1
#endif

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
    COMMENTS_FILE,
    EQUATIONS_FILE,
    IDENTITIES_FILE,
    LISTS_FILE,
    SCALARS_FILE,
    TABLES_FILE,
    VARIABLES_FILE,
    SETTINGS_FILE,
    LOGS_FILE,
    ANY_FILE
};

const static int NB_IODE_FILES = 10;


/* ****************************** *
 *          FUNCTIONS             *
 * ****************************** */


// oem stands for oem 437 (see https://en.wikipedia.org/wiki/Code_page_437)
inline char* convert_oem_to_utf8(char* oem, int codepage = CP_OEMCP)
{
    // see http://www.cplusplus.com/forum/general/245426/
    if (codepage > 0 && oem != NULL)
    {
        // add 1 because strlen doesn't count the ending \0 character
        int oem_length = static_cast<int>(strlen(oem)) + 1;
        // MultiByteToWideChar: https://docs.microsoft.com/fr-fr/windows/win32/api/stringapiset/nf-stringapiset-multibytetowidechar
        const int u16_length = MultiByteToWideChar(codepage, 0, oem, oem_length, NULL, 0);
        wchar_t* u16 = new wchar_t[u16_length];
        MultiByteToWideChar(codepage, 0, oem, oem_length, u16, u16_length);

        int u8_length = WideCharToMultiByte(CP_UTF8, 0, u16, u16_length, NULL, 0, NULL, FALSE);
        // Warning: some special characters take 2 chars to be stored.
        //          In other words, u8_length may be greater to oem_length.
        //          This is why a new char array must be allocated and returned.
        char* u8 = new char[u8_length];
        // WideCharToMultiByte: https://docs.microsoft.com/fr-fr/windows/win32/api/stringapiset/nf-stringapiset-widechartomultibyte
        WideCharToMultiByte(CP_UTF8, 0, u16, u16_length, u8, u8_length, NULL, FALSE);
        delete[] u16;

        return u8;
    }
}


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
    std::string type_name;

protected:
    KDB* getKDB() const
    {
        KDB* kdb = K_WS[type];
        if (kdb == NULL) throw std::runtime_error("There is currently no " + type_name + " in memory.");
        return kdb;
    }

    KOBJ getObject(const int pos) const
    {
        KDB* kdb = getKDB();
        int nb_objs = count();
        if (pos < 0 || pos > nb_objs) throw std::runtime_error(type_name + " at position " + std::to_string(pos) + " does not exist.");
        return kdb->k_objs[pos];
    }

    int getPosition(const char* name) const
    {
        KDB* kdb = getKDB();
        int pos = K_find(kdb, name);
        if (pos < 0) throw std::runtime_error(type_name + " with name " + name + " does not exist.");
        return pos;
    }

public:
    AbstractIODEObjects(EnumIodeType type) : type(type) 
    {
        type_name = qmapIodeTypes.keys(type)[0].toStdString();
    }

    int getIODEType() const { return type; }

    int count() const { return K_WS[type] != NULL ? K_WS[type]->k_nb : 0; }

    char* getObjectName(const int pos) const
    {
        KOBJ obj = getObject(pos);
        char* name = convert_oem_to_utf8(obj.o_name);
        return name;
    }

    // Do not set const char* new_name because of K_ren definition
    int setObjectName(const int pos, char* new_name)
    {
        // TODO : convert new_name to CP 437 ?
        //        OR check with regex if no special characters except _ + no space + does not start with a number
        char* old_name = getObjectName(pos);
        renameObject(old_name, new_name);
        return pos;
    }

    // Do not set const char* old/new_name because of K_ren definition
    int renameObject(char* old_name, char* new_name)
    {
        // TODO : convert new_name to CP 437 ?
        //        OR check with regex if no special characters except _ + no space + does not start with a number
        if (strlen(new_name) > 20) throw std::runtime_error("Iode names cannot exceed 20 characters." + std::string(new_name) + " : " + std::to_string(strlen(new_name)));
        KDB* kdb = getKDB();
        int pos = K_ren(kdb, old_name, new_name);
        // see K_ren documentation
        if (pos < 0) 
        {
            std::string msg = type_name + " cannot be renamed as " + new_name + ".";
            if (pos == -1) throw std::runtime_error(std::string("Name ") + old_name + " does not exist.\n" + msg);
            else if (pos == -2) throw std::runtime_error(type_name + " with name " + new_name + " already exist.\n" + msg);
            else throw std::runtime_error("Something went wrong.\n" + msg);
        }
        return pos;
    }

    virtual T* getObjectValue(const int pos) const = 0;

    T* getObjectValueByName(const char* name) const
    {
        int pos = getPosition(name);
        if (pos >= 0)
        {
            T* value = getObjectValue(pos);
            return value;
        }
        else
        {
            return NULL;
        }  
    }

    // TODO : implement this method
    // virtual void setObjectValue(const T* value, const int pos) const = 0;

    // TODO : overload subscript operator 
    //T& operator[](cosnt char* name) { ... }

    // TODO : overload subscript operator 
    //T& operator[](const int pos) { ... }
};


class Comments : public AbstractIODEObjects<Comment>
{
public:
    Comments() : AbstractIODEObjects(COMMENTS) {};

    Comment* getObjectValue(const int pos) const { 
        char* oem = K_oval0(getKDB(), pos);
        char* comment = convert_oem_to_utf8(oem);
        return comment;
    }
};


class Equations : public AbstractIODEObjects<Equation>
{
public:
    Equations() : AbstractIODEObjects(EQUATIONS) {};

    // TODO: create a Class Equation (with a destructor)
    Equation* getObjectValue(const int pos) const { return NULL; }

    char* getLec(const int pos) const { return KELEC(getKDB(), pos); }
};


class Identities : public AbstractIODEObjects<Identity>
{
public:
    Identities() : AbstractIODEObjects(IDENTITIES) {};

    Identity* getObjectValue(const int pos) const { return NULL; }

    char* getLec(const int pos) const { return KILEC(getKDB(), pos); }
};


class Lists : public AbstractIODEObjects<List>
{
public:
    Lists() : AbstractIODEObjects(LISTS) {}

    List* getObjectValue(const int pos) const { return K_oval0(getKDB(), pos); }
};


class Scalars : public AbstractIODEObjects<Scalar>
{
public:
    Scalars() : AbstractIODEObjects(SCALARS) {};

    Scalar* getObjectValue(const int pos) const { return KSVAL(getKDB(), pos); }
};


class Tables : public AbstractIODEObjects<Table>
{
public:
    Tables() : AbstractIODEObjects(TABLES) {};

    // TODO: create a Class Table (with a destructor)
    Table* getObjectValue(const int pos) const { return NULL; }

    char* getTitle(const int pos) const
    {
        Table* table = KTVAL(getKDB(), pos);
        char* title_oem = (char*)T_get_title(table);
        char* title = convert_oem_to_utf8(title_oem);
        T_free(table);
        return title;
    }
};


class Variables : public AbstractIODEObjects<Variable>
{
public:
    Variables() : AbstractIODEObjects(VARIABLES) {};

    // TODO: not tested yet
    Variable* getObjectValue(const int pos) const
    {
        KOBJ obj = getObject(pos);
        return (Variable*)SW_getptr(obj.o_val);
    }

    IODE_REAL getValue(const int pos, const int t, const int mode) const 
    { 
        return KV_get(getKDB(), pos, t, mode); 
    }

    int getNbPeriods() const { return KSMPL(getKDB())->s_nb; }

    void getPeriod(char* period, const int t) const
    {
        PER_pertoa(PER_addper(&(KSMPL(getKDB())->s_p1), t), period);
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

inline void CPP_WsClear(EnumIodeType type, std::string str_type)
{
    int res = B_WsClear("", type);
    if (res != EXIT_SUCCESS)
        throw std::runtime_error("Something went wrong when trying to clear objets of type " + str_type);
}
