#pragma once

#include "../api/iode.h"
#include "../api/iodeapi.h"

#include <string>
#include <array>
#include <map>
#include <stdexcept>


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


// TODO: rename K_(...) as below in C api + group them in an enum
enum EnumIodeType
{
    COMMENTS = K_CMT,
    EQUATIONS = K_EQS,
    IDENTITIES = K_IDT,
    LISTS = K_LST,
    SCALARS = K_SCL,
    TABLES = K_TBL,
    VARIABLES = K_VAR
};

// TODO: rename K_NBR_OBJ as NB_IODE_TYPES in C api
const static int NB_IODE_TYPES = K_NBR_OBJ;

const static std::array<std::string, NB_IODE_TYPES> vIodeTypes = { "Comments", "Equations", "Identities", "Lists", "Scalars", "Tables", "Variables" };


// TODO: remove K as below in C api + group them in an enum
enum EnumLang
{
    T_ENGLISH = KT_ENGLISH,
    T_DUTCH = KT_DUTCH,
    T_FRENCH = KT_FRENCH,
};

const static int NB_LANGS = 3;

const static std::map<std::string, EnumLang> mLangs =
{ 
    {"English", T_ENGLISH},
    {"Dutch", T_DUTCH},
    {"French", T_FRENCH}
};


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

const static int NB_FILE_EXT = 10;


struct IodeFileExtension
{
    std::string name;
    std::string ext;
    std::string ascii;
};

const static std::array<IodeFileExtension, NB_FILE_EXT> vFileExtensions =
{ {
    {"Comments", "cmt", "ac"},
    {"Equations", "eqs", "ae"},
    {"Identities", "idt", "ai"},
    {"Lists", "lst", "al"},
    {"Scalars", "scl", "as"},
    {"Tables", "tbl", "at"},
    {"Variables", "var", "av"},
    {"Logs", "log", ""},
    {"Settings", "ini", ""},
    {"Any", "*", ""}
} };


// TODO: remove K as below in C api + group them in an enum
enum EnumIodeCase
{
    UPPER = K_UPPER,
    LOWER = K_LOWER,
    ASIS = K_ASIS
};


// TODO: remove K as below in C api + group them in an enum
enum EnumIodeEquationMethod
{
    E_LSQ = KE_LSQ,
    E_ZELLNER = KE_ZEL,
    E_INSTRUMENTAL = KE_INF,
    E_GLS = KE_GLS,
    E_MAX_LIKELIHOOD = KE_MAXLIK 
};

const static int NB_EQ_METHODS = 5;

const static std::array<std::string, NB_EQ_METHODS> vEquationMethods = { "LSQ", "ZELLNER", "INSTRUMENTAL", "GLS (3SLS)", "MAX_LIKELIHOOD" };
