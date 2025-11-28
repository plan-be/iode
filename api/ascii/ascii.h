#pragma once

#include "scr4/s_yy.h"

#include "api/constants.h"
#include "api/time/period.h"
#include "api/time/sample.h"
#include "api/utils/yy.h"
#include "api/objs/kdb.h"
#include "api/objs/comments.h"
#include "api/objs/equations.h"
#include "api/objs/identities.h"
#include "api/objs/lists.h"
#include "api/objs/scalars.h"
#include "api/objs/tables.h"
#include "api/objs/variables.h"

#include <array>    // for std::array
#include <memory>   // for std::unique_ptr

/*---------------- FUNCS ------------------ */

inline int ascii_compare(const void* a, const void* b)
{
    return YY_strcmp((const char*) a, (const char*) b);
}

inline int ascii_eqs_compare(const void* a, const void* b)
{
    return K_compare((YYKEYS*) a, (YYKEYS*) b);
}

/*---------------- GLOBALS ------------------ */

#define SMPL 1

/**
 * Table of keywords recognized by YY in the context of a Tables ascii file (.at).
 * See s_yy function group for more informations (http://www.xon.be/scr4/libs1/libs157.htm).
 */
static YYKEYS TABLE[] = {
    // ---- Table ----
    (unsigned char*) "BOLD",         TABLE_CELL_BOLD,
    (unsigned char*) "UNDERLINE",    TABLE_CELL_UNDERLINE,
    (unsigned char*) "CENTER",       TABLE_CELL_CENTER,
    (unsigned char*) "DATE",         TABLE_ASCII_LINE_DATE,
    (unsigned char*) "DECIMAL",      TABLE_CELL_DECIMAL,
    (unsigned char*) "DIM",          TABLE_ASCII_DIM,
    (unsigned char*) "DIV",          TABLE_ASCII_DIVIDER,
    (unsigned char*) "DUTCH",        TABLE_ASCII_DUTCH,
    (unsigned char*) "ENGLISH",      TABLE_ASCII_ENGLISH,
    (unsigned char*) "FILES",        TABLE_ASCII_LINE_FILES,
    (unsigned char*) "FRENCH",       TABLE_ASCII_FRENCH,
    (unsigned char*) "ITALIC",       TABLE_CELL_ITALIC,
    (unsigned char*) "LEC",          TABLE_ASCII_CELL_LEC,
    (unsigned char*) "LEFT",         TABLE_CELL_LEFT,
    (unsigned char*) "LINE",         TABLE_ASCII_LINE_SEP,
    (unsigned char*) "LINE BOLD",    TABLE_ASCII_BOLD_LINE,
    (unsigned char*) "MODE",         TABLE_ASCII_LINE_MODE,
    (unsigned char*) "NORMAL",       TABLE_CELL_NORMAL,
    (unsigned char*) "RIGHT",        TABLE_CELL_RIGHT,
    (unsigned char*) "TITLE",        TABLE_ASCII_LINE_TITLE,
    (unsigned char*) "{",            TABLE_ASCII_OPEN,
    (unsigned char*) "}",            TABLE_ASCII_CLOSE,
    (unsigned char*) "-",            TABLE_ASCII_BREAK,
    (unsigned char*) "YMIN",         TABLE_ASCII_YMIN,
    (unsigned char*) "YMAX",         TABLE_ASCII_YMAX,
    (unsigned char*) "ZMIN",         TABLE_ASCII_ZMIN,
    (unsigned char*) "ZMAX",         TABLE_ASCII_ZMAX,
    (unsigned char*) "XGRID",        TABLE_ASCII_XGRID,
    (unsigned char*) "YGRID",        TABLE_ASCII_YGRID,
    (unsigned char*) "BOX",          TABLE_ASCII_BOX,
    (unsigned char*) "AXIS",         TABLE_ASCII_AXIS,
    (unsigned char*) "ALIGN",        TABLE_ASCII_ALIGN,
    (unsigned char*) "LAXIS",        TABLE_ASCII_LEFT_AXIS,
    (unsigned char*) "RAXIS",        TABLE_ASCII_RIGHT_AXIS,
    (unsigned char*) "GRLINE",       TABLE_ASCII_GRAPH_LINE,
    (unsigned char*) "GRBAR",        TABLE_ASCII_GRAPH_BAR,
    (unsigned char*) "GRSCATTER",    TABLE_ASCII_GRAPH_SCATTER,
    // ---- Variable ----
    (unsigned char *) "sample",      SMPL
};
