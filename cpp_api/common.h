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


const static std::vector<std::string> v_ext_names = 
{ 
    "Comment", "Equation", "Identity", "List", "Scalar", "Table", "Variable", "", 
    "Comment", "Equation", "Identity", "List", "Scalar", "Table", "Variable", "",
    "Report", "A2m", "Agl", "Profile", "Dif", "Mif", "Rtf", "PostScript", "Ascii", "Text", "Csv", "",
    "Html",  "Ref", "Logs", "Settings", "Any", "Directory"
};

// same as k_ext defined in k_objfile.c + "html", ... "ini"
const static std::vector<std::string> v_ext = 
{
    "cmt", "eqs", "idt", "lst", "scl", "tbl", "var", "ooo",
    "ac",  "ae",  "ai",  "al",  "as",  "at",  "av", "",
    "rep", "a2m", "agl", "prf", "dif", "mif", "rtf", "ps", "asc", "txt", "csv", "xxx", 
    "html", "ref", "log", "ini", "", ""
};

const static std::string report_ext = ".rep";

const static std::map<std::string, IodeFileType> mFileExtensions =
{
    {".cmt", FILE_COMMENTS},
    {".eqs", FILE_EQUATIONS},
    {".idt", FILE_IDENTITIES},
    {".lst", FILE_LISTS},
    {".scl", FILE_SCALARS},
    {".tbl", FILE_TABLES},
    {".var", FILE_VARIABLES},

    {".ac",  FILE_COMMENTS},
    {".ae",  FILE_EQUATIONS},
    {".ai",  FILE_IDENTITIES},
    {".al",  FILE_LISTS},
    {".as",  FILE_SCALARS},
    {".at",  FILE_TABLES},
    {".av",  FILE_VARIABLES},

    {".rep", FILE_REP},
    {".a2m", FILE_A2M},
    {".agl", FILE_AGL},
    {".prf", FILE_PRF},
    {".dif", FILE_DIF},
    {".mif", FILE_MIF},
    {".rtf", FILE_RTF},
    {".ps", FILE_PS},
    {".asc", FILE_AAS},
    {".txt", FILE_TXT},
    {".csv", FILE_CSV},

    {".htm", FILE_HTML},
    {".ref", FILE_REF},
    {".log", FILE_LOG},
    {".ini", FILE_SETTINGS},
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
