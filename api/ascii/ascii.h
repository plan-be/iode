#pragma once

#include "scr4/s_yy.h"

#include "api/constants.h"
#include "api/time/period.h"
#include "api/time/sample.h"
#include "api/utils/yy.h"
#include "api/objs/kdb.h"
#include "api/objs/tables.h"

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

/*---------------- CLASSES ------------------ */

struct AbstractAscii
{
    virtual KDB* load_asc(char* filename, int db_global) = 0;
    virtual int save_asc(KDB* kdb, char* filename) = 0;
    virtual int save_csv(KDB* kdb, char* filename, Sample* sample, char** varlist) = 0;
};

struct AsciiComments : public AbstractAscii
{
    KDB* load_asc(char* filename, int db_global) override;
    int save_asc(KDB* kdb, char* filename) override;
    int save_csv(KDB* kdb, char* filename, Sample* sample, char** varlist) override;
};

struct AsciiEquations : public AbstractAscii
{
    KDB* load_asc(char* filename, int db_global) override;
    int save_asc(KDB* kdb, char* filename) override;
    int save_csv(KDB* kdb, char* filename, Sample* sample, char** varlist) override;
};

struct AsciiIdentities : public AbstractAscii
{
    KDB* load_asc(char* filename, int db_global) override;
    int save_asc(KDB* kdb, char* filename) override;
    int save_csv(KDB* kdb, char* filename, Sample* sample, char** varlist) override;
};

struct AsciiLists : public AbstractAscii
{
    KDB* load_asc(char* filename, int db_global) override;
    int save_asc(KDB* kdb, char* filename) override;
    int save_csv(KDB* kdb, char* filename, Sample* sample, char** varlist) override;
};

struct AsciiScalars : public AbstractAscii
{
    KDB* load_asc(char* filename, int db_global) override;
    int save_asc(KDB* kdb, char* filename) override;
    int save_csv(KDB* kdb, char* filename, Sample* sample, char** varlist) override;
};

struct AsciiTables : public AbstractAscii
{
    KDB* load_asc(char* filename, int db_global) override;
    int save_asc(KDB* kdb, char* filename) override;
    int save_csv(KDB* kdb, char* filename, Sample* sample, char** varlist) override;
};

struct AsciiVariables : public AbstractAscii
{
    /** 
     *  Parameters specific to csv output files. 
     *  These parameters can be modified via report commands: 
     *    - $csvdigits
     *    - $csvsep
     *    - $csvdec
     *    - $csvnan
     *    - $csvaxes
     *  
     */
    static char* CSV_SEP;
    static char* CSV_DEC;
    static char* CSV_NAN;
    static char* CSV_AXES;
    static int   CSV_NBDEC;

    KDB* load_asc(char* filename, int db_global) override;
    int save_asc(KDB* kdb, char* filename) override;
    int save_csv(KDB* kdb, char* filename, Sample* sample, char** varlist) override;

private:
    KDB* load_asc_type_ask(char *file_or_string, int type, int ask, int db_global);
};

/*---------------- GLOBALS ------------------ */

inline std::array<std::unique_ptr<AbstractAscii>, 7> ascii_handlers = {
    std::make_unique<AsciiComments>(),
    std::make_unique<AsciiEquations>(),
    std::make_unique<AsciiIdentities>(),
    std::make_unique<AsciiLists>(),
    std::make_unique<AsciiScalars>(),
    std::make_unique<AsciiTables>(),
    std::make_unique<AsciiVariables>()
};

#define SMPL     1

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
