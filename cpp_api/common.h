#pragma once

#include "../api/iode.h"
#include "../api/iodeapi.h"

#include <string>
#include <array>
#include <vector>
#include <map>
#include <stdexcept>
#include <iostream>


/* ****************************** *
 *           CONSTANTS            *
 * ****************************** */

// should be moved to iode.h
const static char* ORGANIZATION_NAME = "Federal Planning Bureau";
const static char* DEFAULT_INSTALLATION_DIR = "c:/iode";               // see function HLP_filename() in dos/o_help.c
const static char* IODE_WEBSITE = "https://iode.plan.be/doku.php";
const static char NAN_REP[3] = "--";


/* ****************************** *
 *             ENUMS              *
 * ****************************** */

 /* NOTE FOR THE DEVELOPPERS:
  * enum documentation: https://en.cppreference.com/w/cpp/language/enum
  */


// TODO: replace K by I as below in C api + group them in an enum
enum EnumIodeType
{
    I_COMMENTS = K_CMT,
    I_EQUATIONS = K_EQS,
    I_IDENTITIES = K_IDT,
    I_LISTS = K_LST,
    I_SCALARS = K_SCL,
    I_TABLES = K_TBL,
    I_VARIABLES = K_VAR
};

// TODO: rename K_NBR_OBJ as I_NB_TYPES in C api
const static int I_NB_TYPES = K_NBR_OBJ;

const static std::array<std::string, I_NB_TYPES> vIodeTypes = { "Comments", "Equations", "Identities", "Lists", "Scalars", "Tables", "Variables" };

struct IodeRegexName
{
    std::string regex;
    std::string type;
};

inline IodeRegexName get_regex_name(const EnumIodeType type)
{
    IodeRegexName nre;
    if (type == I_COMMENTS)
    {
        nre.regex = "A-Za-z";
        nre.type = "capital or lowercase";
    }
    else if (type == I_SCALARS)
    {
        nre.regex = "a-z";
        nre.type = "lowercase";
    }
    else
    {
        nre.regex = "A-Z";
        nre.type = "capital";
    }

    // see https://www.cplusplus.com/reference/regex/ECMAScript for regex syntax
    nre.regex = "[" + nre.regex + "_][" + nre.regex + "0-9_]{0," + std::to_string(K_MAX_NAME - 1) + "}";

    return nre;
}

// TODO: replace K by I as below in C api + group them in an enum
enum EnumLang
{
    IT_ENGLISH = KT_ENGLISH,
    IT_DUTCH = KT_DUTCH,
    IT_FRENCH = KT_FRENCH,
};

const static int I_NB_LANGS = 3;

const static std::map<EnumLang, std::string> mLangs = 
{ 
    {IT_ENGLISH, "English"},
    {IT_DUTCH, "Dutch"},
    {IT_FRENCH, "French"}
};


enum EnumIodeFile
{
    I_COMMENTS_FILE,
    I_EQUATIONS_FILE,
    I_IDENTITIES_FILE,
    I_LISTS_FILE,
    I_SCALARS_FILE,
    I_TABLES_FILE,
    I_VARIABLES_FILE,
    I_SETTINGS_FILE,
    I_LOGS_FILE,
    I_ANY_FILE
};

const static int I_NB_FILE_EXT = 10;


struct IodeFileExtension
{
    std::string name;
    std::string ext;
    std::string ascii;
};

const static std::array<IodeFileExtension, I_NB_FILE_EXT> vFileExtensions =
{ {
    {"Comments", ".cmt", ".ac"},
    {"Equations", ".eqs", ".ae"},
    {"Identities", ".idt", ".ai"},
    {"Lists", ".lst", ".al"},
    {"Scalars", ".scl", ".as"},
    {"Tables", ".tbl", ".at"},
    {"Variables", ".var", ".av"},
    {"Logs", ".log", ""},
    {"Settings", ".ini", ""},
    {"Any", "*", ""}
} };


// TODO: replace K by I as below in C api + group them in an enum
enum EnumIodeCase
{
    I_UPPER = K_UPPER,
    I_LOWER = K_LOWER,
    I_ASIS = K_ASIS
};


// TODO: replace K by I as below in C api + group them in an enum
enum EnumIodeEquationMethod
{
    IE_LSQ = KE_LSQ,
    IE_ZELLNER = KE_ZEL,
    IE_INSTRUMENTAL = KE_INF,
    IE_GLS = KE_GLS,
    IE_MAX_LIKELIHOOD = KE_MAXLIK 
};

const static int I_NB_EQ_METHODS = 5;

const static std::array<std::string, I_NB_EQ_METHODS> vEquationMethods = { "LSQ", "ZELLNER", "INSTRUMENTAL", "GLS (3SLS)", "MAX_LIKELIHOOD" };
