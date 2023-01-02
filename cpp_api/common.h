#pragma once

#include "api/iode.h"
#include "api/iodeapi.h"

#include <string>
#include <array>
#include <vector>
#include <map>
#include <bitset>
#include <stdexcept>
#include <iostream>


using int_bitset = std::bitset<sizeof(int)>;


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

const static std::array<std::string, I_NB_TYPES> vIodeTypes = { "Comment", "Equation", "Identity", "List", "Scalar", "Table", "Variable" };

struct IodeRegexName
{
    std::string regex;
    std::string type;
};

// TODO: replace K by I as below in C api + group them in an enum
enum EnumLang
{
    IT_ENGLISH = KT_ENGLISH,
    IT_DUTCH = KT_DUTCH,
    IT_FRENCH = KT_FRENCH,
};

const static int I_NB_LANGS = 3;

const static std::array<std::string, I_NB_LANGS> vLangs = { "English", "Dutch", "French" };


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


enum EnumIodeExportFormat
{
    I_EXP_FMT_CSV,
    I_EXP_FMT_DIF,
    I_EXP_FMT_WKS,
    I_EXP_FMT_TSP,
    I_EXP_FMT_RCSV
};

const static int I_NB_EXPORT_FORMATS = 5;

const static std::array<std::string, I_NB_EXPORT_FORMATS> vExportFormats = 
	{ "CSV", "DIF", "WKS", "TSP", "Reverse CSV" };