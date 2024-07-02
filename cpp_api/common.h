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
 *         MISCELLANEOUS          *
 * ****************************** */

struct IodeRegexName
{
    std::string regex;
    std::string type;
};

struct FileType
{
    std::string name;
    std::vector<std::string> v_ext;

    FileType(const std::string& name = "", const std::vector<std::string>& v_ext = {}) 
        : name(name), v_ext(v_ext) {} 
};

const static std::vector<std::string> v_iode_types = { "Comment", "Equation", "Identity", "List", "Scalar", "Table", "Variable" };


/* ****************************** *
 *             ENUMS              *
 * ****************************** */

 /* NOTE FOR THE DEVELOPPERS:
  * enum documentation: https://en.cppreference.com/w/cpp/language/enum
  */

// TODO: replace K by I as below in C api + group them in an enum
enum EnumLang
{
    IT_ENGLISH,
    IT_DUTCH,
    IT_FRENCH
};

const static int I_NB_LANGS = 3;

const static std::vector<std::string> vLangs = { "English", "Dutch", "French" };

const static std::string report_ext = ".rep";

const static FileType file_comments("Comment", {".cmt", ".ac"});
const static FileType file_equations("Equation", {".eqs", ".ae"});
const static FileType file_identities("Identity", {".idt", ".ai"});
const static FileType file_lists("List", {".lst", ".al"});
const static FileType file_scalars("Scalar", {".scl", ".as"});
const static FileType file_tables("Table", {".tbl", ".at"});
const static FileType file_variables("Variable", {".var", ".av"});

// see IodeFileType in api/iode.h
const static std::vector<FileType> v_file_types = 
{
    file_comments,
    file_equations,
    file_identities,
    file_lists,
    file_scalars,
    file_tables,
    file_variables,
    FileType(),
    file_comments,
    file_equations,
    file_identities,
    file_lists,
    file_scalars,
    file_tables,
    file_variables,
    FileType(), 
    FileType("Report", {".rep"}), 
    FileType("A2m", {".a2m"}), 
    FileType("Agl", {".agl"}), 
    FileType("Profile", {".prf"}), 
    FileType("Dif", {".dif"}), 
    FileType("Mif", {".mif"}), 
    FileType("Rtf", {".rtf"}), 
    FileType("PostScript", {".ps"}), 
    FileType("Ascii", {".asc"}), 
    FileType("Text", {".txt"}), 
    FileType("Csv", {".csv"}), 
    FileType(),
    FileType("Html", {".html"}),
    FileType("Ref", {".ref"}), 
    FileType("Logs", {".log"}), 
    FileType("Settings", {".ini"}), 
    FileType("Any", {}), 
    FileType("Directory", {})
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
