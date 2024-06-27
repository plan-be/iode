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


const static std::vector<std::string> vIodeTypes = { "Comment", "Equation", "Identity", "List", "Scalar", "Table", "Variable" };
const static std::vector<std::string> v_binary_ext = { "cmt", "eqs", "idt", "lst", "scl", "tbl", "var" };
const static std::vector<std::string> v_ascii_ext = { "ac", "ae", "ai", "al", "as", "at", "av" };

struct IodeRegexName
{
    std::string regex;
    std::string type;
};

// TODO: replace K by I as below in C api + group them in an enum
enum EnumLang
{
    IT_ENGLISH,
    IT_DUTCH,
    IT_FRENCH
};

const static int I_NB_LANGS = 3;

const static std::vector<std::string> vLangs = { "English", "Dutch", "French" };


enum EnumIodeFile
{
    COMMENTS_FILE,
    EQUATIONS_FILE,
    IDENTITIES_FILE,
    LISTS_FILE,
    SCALARS_FILE,
    TABLES_FILE,
    VARIABLES_FILE,
    I_REPORTS_FILE,
    I_TEXT_FILE,
    I_ASCII_FILE,
    I_A2M_FILE,
    I_RTF_FILE,
    I_HTML_FILE,
    I_MIF_FILE,
    I_CSV_FILE,
    I_REF_FILE,
    I_AGL_FILE,
    I_DIF_FILE,
    I_LOGS_FILE,
    I_SETTINGS_FILE,
    I_ANY_FILE,
    I_DIRECTORY
};

const static int I_NB_ENUM_FILE_EXT = 22;

const static std::vector<std::string> v_ext_names = 
{ 
    "Comment", "Equation", "Identity", "List", "Scalar", "Table", "Variable", 
    "Report", "Text", "Ascii", "A2m", "Rtf", "Html", "Mif", "Csv", "Ref", "Agl", "Dif",
    "Logs", "Settings", "Any", "Directory"
};

// same as k_ext defined in k_objfile.c
const static std::vector<std::string> v_ext = 
{
    "cmt", "eqs", "idt", "lst", "scl", "tbl", "var",
    "ac",  "ae",  "ai",  "al",  "as",  "at",  "av",
    "rep", "a2m", "agl", "prf", "dif", "mif", "rtf",
    "ref", "ps",  "asc", "txt", "csv"
};

const static std::string report_ext = ".rep";

// (iode objs) 14 + report (1) + any (1) + directory (1) = 17 
// logs (1) + settings (1) + text (9) = 11
const static int I_NB_FILE_EXT = 28;

const static std::map<std::string, EnumIodeFile> mFileExtensions =
{
    {".cmt", COMMENTS_FILE},
    {".ac",  COMMENTS_FILE},
    {".eqs", EQUATIONS_FILE},
    {".ae",  EQUATIONS_FILE},
    {".idt", IDENTITIES_FILE},
    {".ai",  IDENTITIES_FILE},
    {".lst", LISTS_FILE},
    {".al",  LISTS_FILE},
    {".scl", SCALARS_FILE},
    {".as",  SCALARS_FILE},
    {".tbl", TABLES_FILE},
    {".at",  TABLES_FILE},
    {".var", VARIABLES_FILE},
    {".av",  VARIABLES_FILE},
    {".rep", I_REPORTS_FILE},
    {".txt", I_TEXT_FILE},
    {".asc", I_ASCII_FILE},
    {".a2m", I_A2M_FILE},
    {".rtf", I_RTF_FILE},
    {".htm", I_HTML_FILE},
    {".mif", I_MIF_FILE},
    {".csv", I_CSV_FILE},
    {".ref", I_REF_FILE},
    {".agl", I_AGL_FILE},
    {".dif", I_DIF_FILE},
    {".log", I_LOGS_FILE},
    {".ini", I_SETTINGS_FILE},
};


const static std::vector<std::string> v_simulation_initialization
{
    "Y := Y[-1], if Y null or NA",                  // TM1
    "Y := Y[-1], always",                           // TM1_A
    "Y := extrapolation, if Y null or NA",          // EXTRA    -> Y := fn(Y[-1], Y[-2]), 
    "Y := extrapolation, always",                   // EXTRA_A  -> Y := fn(Y[-1], Y[-3])
    "Y unchanged",                                  // ASIS
    "Y := Y[-1], if Y = NA",                        // TM1_NA
    "Y := extrapolation, if Y = NA"                 // EXTRA_NA -> Y := fn(Y[-1], Y[-3])
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

const static std::vector<std::string> vExportFormats = 
	{ "CSV", "DIF", "WKS", "TSP", "Reverse CSV" };


// ====== Graphs ======

enum EnumIodeGraphAxisThicks
{
    I_G_MAJOR_THICKS,
    I_G_NO_THICKS,
    I_G_MINOR_THICKS
};

const static int I_NB_AXIS_THICKS = 3;

const static std::vector<std::string> vGraphsAxisThicks = 
    { "Major thicks", "No grids", "Minor thicks" };


enum EnumIodeGraphChart
{
    I_G_CHART_LINE,
    I_G_CHART_SCATTER,
    I_G_CHART_BAR
};

const static int I_NB_CHART_TYPES = 3;

const static std::vector<std::string> vGraphsChartTypes = 
    { "Line chart", "Scatter chart", "Bar chart" };


const static int I_NB_COLORS = 8;

const static std::vector<std::string> vGraphsColors = 
    {"Blue", "Yellow", "Black", "White", "Red", "Green", "Cyan", "Magenta"};

// ====== Fonts ======

const static int I_NB_FONTS = 6;

const static std::vector<std::string> vIodeFonts =
    {"Helvetica", "Times", "Courier", "Bookman", "Palatino", "Trebuchet MS"};

// ====== Print ======

const static int I_NB_PRINT_DEST = 2;

const static std::vector<std::string> vPrintDest = {"Windows printer", "File"};

const static int I_NB_PRINT_FORMATS = 5;

const static std::vector<std::string> vPrintFormats =
    {"A2M file (IODE ...)", "RTF file (Word, ...)", "HTML file (Internet)",
     "MIF file (Frame Maker)", "CSV file (Excel, ....)"};
