#pragma once

#include "api/iode.h"
#include "api/iodeapi.h"

extern "C" 
{
    // copied from api/objs/k_objfile.c
    extern int  K_LZH;

    // copied from scr4/s_prosw.h
    extern char *SW_nalloc_doc(unsigned int size, char *, int);
    extern void *SW_nrealloc_doc(void *old_ptr, unsigned int old_count, unsigned int new_count, char *file, int line);
    extern int SW_free(SWHDL item);
    extern int SW_nfree(void *ptr);

    // copied from scr4/s_prost.h
    extern int SCR_free(void *ptr);
    extern int SCR_free_tbl(unsigned char **tbl);
    extern int SCR_tbl_size(unsigned char **tbl);
}

#include <string>
#include <array>
#include <vector>
#include <map>
#include <bitset>
#include <stdexcept>
#include <iostream>
#include <iomanip>
#include <sstream>


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

const static std::vector<std::string> v_table_langs = { "English", "Dutch", "French" };

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
