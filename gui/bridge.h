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

const static char NAN_REP[3] = "--";


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

const static std::vector<std::string> iodeTypesAsString = {"Comments", "Equations", "Identities", "Lists", "Scalars", "Tables", "Variables"};


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


inline void init_iode_api()
{

    SW_MIN_MEM = 120 * 1024L;
    SW_MIN_SEGS = 2;
    // initializes SCR4
    if (SW_init(1)) throw std::runtime_error("An error occured when called the internal function SW_init()");

    // initializes Dynamic Data Exchange (DDE)
    //IodeStartDde();
    // initializes Workspace 
    K_init_ws(0);
}


inline void end_iode_api()
{
    // free Workspace
    K_end_ws(0);
    // free SCR4
    SW_end();
    // ???
    W_close();
    // stops Dynamic Data Exchange (DDE)
    //IodeEndDde();
}


// see http://www.cplusplus.com/forum/general/245426/
inline char* convert_between_codepages(const char* s_in, const int codepage_in, const int codepage_out)
{
    if (codepage_in > 0 && codepage_out > 0 && s_in != NULL)
    {
        // add 1 because strlen doesn't count the ending \0 character
        int s_in_length = static_cast<int>(strlen(s_in)) + 1;
        // MultiByteToWideChar: https://docs.microsoft.com/fr-fr/windows/win32/api/stringapiset/nf-stringapiset-multibytetowidechar
        const int wchar_length = MultiByteToWideChar(codepage_in, 0, s_in, s_in_length, NULL, 0);
        wchar_t* wchar_array = new wchar_t[wchar_length];
        MultiByteToWideChar(codepage_in, 0, s_in, s_in_length, wchar_array, wchar_length);

        int s_out_length = WideCharToMultiByte(codepage_out, 0, wchar_array, wchar_length, NULL, 0, NULL, FALSE);
        // Warning: some special characters take 2 chars to be stored.
        //          In other words, s_out_length may be greater to s_in_length.
        //          This is why a new char array must be allocated and returned.
        char* s_out = new char[s_out_length];
        // WideCharToMultiByte: https://docs.microsoft.com/fr-fr/windows/win32/api/stringapiset/nf-stringapiset-widechartomultibyte
        WideCharToMultiByte(codepage_out, 0, wchar_array, wchar_length, s_out, s_out_length, NULL, FALSE);
        delete[] wchar_array;

        return s_out;
    }
}


// oem stands for oem 437 (see https://en.wikipedia.org/wiki/Code_page_437)
inline char* convert_oem_to_utf8(const char* oem)
{
    return convert_between_codepages(oem, CP_OEMCP, CP_UTF8);
}


inline char* convert_utf8_to_oem(const char* u8)
{
    return convert_between_codepages(u8, CP_UTF8, CP_OEMCP);
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


template <class T> class AbstractKDB
{
protected:
    EnumIodeType type;
    std::string type_name;

protected:
    // Cannot define a KDB *kdb member set to K_WS[type] in the constructor because the pointer contained in 
    // K_WS[type] may change in the course of the course (when loading files for example)
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

    virtual int setOrAddObject(const char* name, const T* value) const = 0;

private:
    void setOrAddObjectWithException(const char* name, const T* value) const
    {
        int res = setOrAddObject(name, value);
        if (res == -1) throw std::runtime_error("Iode has not been initialized");
        if (res < -1) throw std::runtime_error("Something went wrong when trying to set or add " + type_name + " with name " + name);
    }

public:
    AbstractKDB(EnumIodeType type) : type(type), type_name(iodeTypesAsString[type]) { init_iode_api(); }

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
        char* old_name = getObjectName(pos);
        renameObject(old_name, new_name);
        return pos;
    }

    // Do not set const char* old/new_name because of K_ren definition
    int renameObject(char* old_name, char* new_name)
    {
        if (strlen(new_name) > K_MAX_NAME) throw std::runtime_error("Iode names cannot exceed " + std::to_string(K_MAX_NAME) + " characters." + std::string(new_name) + " : " + std::to_string(strlen(new_name)));
        KDB* kdb = getKDB();
        int pos = K_ren(kdb, old_name, new_name);
        // see K_ren documentation
        if (pos < 0) 
        {
            std::string msg = type_name + " cannot be renamed as " + new_name + ".";
            if (pos == -1) throw std::runtime_error(std::string("Name ") + old_name + " does not exist.\n" + msg);
            else if (pos == -2) throw std::runtime_error(type_name + " with name " + new_name + " already exists.\n" + msg);
            else throw std::runtime_error("Something went wrong.\n" + msg);
        }
        return pos;
    }

    virtual T* getObjectValue(const int pos) const = 0;

    T* getObjectValueByName(const char* name) const
    {
        int pos = getPosition(name);
        T* value = getObjectValue(pos);
        return value; 
    }

    void setObjectValue(const int pos, const T* value) const
    {
        char* name = getObjectName(pos);
        setOrAddObjectWithException(name, value);
    }

    void setObjectValueByName(const char* name, const T* value) const
    {
        // call getPosition() to check that object exists
        getPosition(name);
        setOrAddObjectWithException(name, value);
    }

    // TODO : overload subscript operator 
    //T& operator[](const char* name) { ... }

    // TODO : overload subscript operator 
    //T& operator[](const int pos) { ... }
};


class Comments : public AbstractKDB<Comment>
{
public:
    Comments() : AbstractKDB(COMMENTS) {};

    Comment* getObjectValue(const int pos) const { 
        char* oem = K_oval0(getKDB(), pos);
        char* comment = convert_oem_to_utf8(oem);
        return comment;
    }

protected:
    int setOrAddObject(const char* name, const Comment* comment) const
    {
        char* comment_oem = convert_utf8_to_oem(comment);
        return K_add(getKDB(), name, comment_oem);
    }
};


class Equations : public AbstractKDB<Equation>
{
public:
    Equations() : AbstractKDB(EQUATIONS) {};

    // TODO: create a Class Equation (with a destructor)
    Equation* getObjectValue(const int pos) const { return NULL; }

    char* getLec(const int pos) const { return KELEC(getKDB(), pos); }

protected:
    int setOrAddObject(const char* name, const Equation* equation) const
    {
        return K_add(getKDB(), name, equation, name);
    }
};


class Identities : public AbstractKDB<Identity>
{
public:
    Identities() : AbstractKDB(IDENTITIES) {};

    Identity* getObjectValue(const int pos) const { return NULL; }

    char* getLec(const int pos) const { return KILEC(getKDB(), pos); }

protected:
    int setOrAddObject(const char* name, const Identity* identity) const
    {
        return K_add(getKDB(), name, identity->lec);
    }
};


class Lists : public AbstractKDB<List>
{
public:
    Lists() : AbstractKDB(LISTS) {}

    List* getObjectValue(const int pos) const { return K_oval0(getKDB(), pos); }

protected:
    int setOrAddObject(const char* name, const List* list) const
    {
        return K_add(getKDB(), name, list);
    }
};


class Scalars : public AbstractKDB<Scalar>
{
public:
    Scalars() : AbstractKDB(SCALARS) {};

    Scalar* getObjectValue(const int pos) const { return KSVAL(getKDB(), pos); }

protected:
    int setOrAddObject(const char* name, const Scalar* scalar) const
    {
        return K_add(getKDB(), name, scalar);
    }
};


class Tables : public AbstractKDB<Table>
{
public:
    Tables() : AbstractKDB(TABLES) {};

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

protected:
    int setOrAddObject(const char* name, const Table* table) const
    {
        return K_add(getKDB(), name, table);
    }
};


class Variables : public AbstractKDB<Variable>
{
public:
    Variables() : AbstractKDB(VARIABLES) {};

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

protected:
    int setOrAddObject(const char* name, const Variable* variable) const
    {
        int nb_obs = getNbPeriods();
        return K_add(getKDB(), name, variable, nb_obs);
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
