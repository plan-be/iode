#pragma once

#include "api/all.h"
#include "api/utils/utils.h"
#include "api/time/sample.h"
#include "api/objs/kdb.h"
#include "api/k_super.h"

extern "C" 
{
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

// copied from api/objs/k_objfile.cpp
extern int  K_LZH;

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
