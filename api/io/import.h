#pragma once

#include "scr4/s_yy.h"

#include "api/constants.h"
#include "api/utils/time.h"
#include "api/objs/kdb.h"
#include "api/io/dif.h"

#include <array>    // for std::array
#include <memory>   // for std::unique_ptr

/*---------------- DEFINE -------------------------*/

#define ASC_SMPL    1

/*---------------- ENUMS -------------------------*/

enum IodeImportFormat
{
    IMPORT_ASCII,
    IMPORT_ROT_ASCII,
    IMPORT_DIF,
    IMPORT_BISTEL,
    IMPORT_NIS,
    IMPORT_GEM,
    IMPORT_PRN,
    IMPORT_TXT
};

const static int IODE_NB_IMPORT_FORMATS = 8;

/*---------------- GLOBALS -------------------------*/

inline char** IMP_rule = NULL;  // Rules read in a rule file
inline char** IMP_pat = NULL;   // Patterns read in a rule file (// to IMP_rule)
inline int    IMP_trace = 0;    // If not null, saves a trace of the object name modifications

static YYKEYS ImportObjsASCII_KEYS[] = {
    (unsigned char*) "sample",   ASC_SMPL
};

static YYKEYS ImportObjsRevertASCII_KEYS[] = {
    (unsigned char*) "sample",   ASC_SMPL
};

static YYKEYS ImportObjsDIF_KEYS[] = {
    (unsigned char*) "EOD",           DIF_EOD,
    (unsigned char*) "TABLE",         DIF_TABLE,
    (unsigned char*) "VECTORS",       DIF_VECTORS,
    (unsigned char*) "TUPLES",        DIF_TUPLES,
    (unsigned char*) "DATA",          DIF_DATA,
    (unsigned char*) "BOT",           DIF_BOT,
    (unsigned char*) ",",             DIF_COMMA,
    (unsigned char*) "V",             DIF_V
};

/*---------------- CLASSES ------------------ */

// ---- Import Variables ----

struct ImportVarFromFile {
    YYKEYS  *imp_keys = NULL;                                                       // Table of keywords (see YY group of functions in scr4 libs)
    int imp_dim = 0;                                                                // Nb of keys in imp_keys
    bool read_variable_implemented;
    virtual int read_header(YYFILE*, SAMPLE*) { return 0; }                         // method to open the input file and to read its header
    virtual int read_variable(YYFILE*, char*, int, double*) { return 0; }           // method to read full variable (i.e. a name + a series of values)
    virtual int read_numerical_value(YYFILE*, char*, int*, double*) { return 0; }   // method to read a single numerical value (a double)
    virtual int close(void) { return 0; }                                           // method to close the input file
};


/* k_iasc.c */
struct ImportObjsASCII : public ImportVarFromFile 
{
    ImportObjsASCII() 
    {
        imp_keys = ImportObjsASCII_KEYS;    // Table of keywords (see YY group of functions in scr4 libs)
        imp_dim = 1;                        // Nb of keys in imp_keys
        read_variable_implemented = true;
    }

    int read_header(YYFILE*, SAMPLE*) override;
    int read_variable(YYFILE*, char*, int, double*) override;
};

/* k_irasc.c */
class ImportObjsRevertASCII : public ImportVarFromFile 
{
    char** RASC_toc = NULL;  // List of VARs in the rotated ASCII file (following the sample defn).
    int RASC_cv = 0;        // current variable
    int RASC_cy = 0;        // current period

public:
    ImportObjsRevertASCII()
    {
        imp_keys = ImportObjsRevertASCII_KEYS; // Table of keywords (see YY group of functions in scr4 libs)
        imp_dim = 1;                           // Nb of keys in imp_keys
        read_variable_implemented = false;
    }

    int read_header(YYFILE *,SAMPLE *) override;
    int read_numerical_value(YYFILE*, char*, int*, double*) override;
    int close(void) override;
};

/* k_idif.c */
class ImportObjsDIF : public ImportVarFromFile 
{
    SAMPLE* DIF_smpl;
    char    DIF_freq;
    int     DIF_nl; 
    int     DIF_ny;

public:
    ImportObjsDIF()
    {
        imp_keys = ImportObjsDIF_KEYS; // Table of keywords (see YY group of functions in scr4 libs)
        imp_dim = 8;                   // Nb of keys in imp_keys
        DIF_smpl = NULL;
        DIF_freq = 0;
        DIF_nl = 0; 
        DIF_ny = 0;
        read_variable_implemented = true;
    }

    int read_header(YYFILE*, SAMPLE*) override;
    int read_variable(YYFILE *,char *,int ,double *) override;
    int close(void) override;
};

/* k_ibst.c */
class ImportObjsBST : public ImportObjsDIF 
{
    SAMPLE  BST_smpl;
    char    BST_freq = 0;
    int     BST_nbper = 0;

public:
    bool read_variable_implemented = false;
    int read_header(YYFILE *,SAMPLE *) override;
    int read_numerical_value(YYFILE *,char *,int *,double *) override;
};

/* k_inis.c */
struct ImportObjsNIS : public ImportVarFromFile 
{
    bool read_variable_implemented = true;
    int read_header(YYFILE *,SAMPLE *) override;
    int read_variable(YYFILE *,char *,int ,double *) override;
};

/* k_igem.c */
class ImportObjsGEM : public ImportVarFromFile 
{
    SAMPLE  GEM_smpl;
    char    GEM_freq = 0;
    int     GEM_nbper = 0;
    char    GEM_rubr[81];
    int     GEM_nser = 0; 
    int     GEM_cser = 0; 
    int     GEM_nobs = 0;
    double* GEM_mat = NULL;

public:
    bool read_variable_implemented = true;
    int read_header(YYFILE *,SAMPLE *) override;
    int read_variable(YYFILE *,char *,int ,double *) override;
    int close(void) override;
    
private:
    int GEM_name(char *);
    double GEM_read_real(YYFILE* yy);
    int GEM_readsrubr(YYFILE *);
    int GEM_readbegin(YYFILE *);
    int GEM_readrubr(YYFILE *);
};

/* k_iprn.c */
struct ImportObjsPRN : public ImportVarFromFile 
{
    bool read_variable_implemented = true;
    int read_variable(YYFILE *,char *,int ,double *) override;

private:
    double read_real(YYFILE *);
};

class ImportObjsTXT : public ImportVarFromFile 
{
    FILE*   TXT_fd = NULL;
    SAMPLE  TXT_smpl;
    char    TXT_freq = 0;
    int     TXT_nbper = 0;
    int     TXT_lang = 0;

public:
    bool read_variable_implemented = false;
    int read_header(YYFILE*, SAMPLE*) override;
    int read_numerical_value(YYFILE*, char*, int*, double*) override;

private:
    int read_value(char* date, int* shift, char* tval, double* dval);
};

inline std::array<std::unique_ptr<ImportVarFromFile>, IODE_NB_IMPORT_FORMATS> import_variables = 
{
    std::make_unique<ImportObjsASCII>(),
    std::make_unique<ImportObjsRevertASCII>(),
    std::make_unique<ImportObjsDIF>(),
    std::make_unique<ImportObjsBST>(),
    std::make_unique<ImportObjsNIS>(),
    std::make_unique<ImportObjsGEM>(),
    std::make_unique<ImportObjsPRN>(),
    std::make_unique<ImportObjsTXT>()
};

// ---- Import Comments ----

struct ImportCmtFromFile
{
    YYKEYS  *imp_keys = NULL;                                                       // Table of keywords (see YY group of functions in scr4 libs)
    int imp_dim = 0;                                                                // Nb of keys in imp_keys
    virtual int read_header(ImportCmtFromFile*, char*, int) { return 0; }       // method to open the input file and to read its header
    virtual int read_comment(char*, char**) { return 0; }                           // method to read full comment
    virtual int close(void) { return 0; }                                           // method to close the input file
};

class ImportCommentsASCII : public ImportCmtFromFile 
{
    YYFILE* AYY = NULL;

public:
    int read_header(ImportCmtFromFile*, char*, int) override;
    int read_comment(char* name, char** cmt) override;
};

class ImportCommentsBST : public ImportCmtFromFile 
{
    YYFILE* FYY = NULL;
    YYFILE* RYY = NULL;
    YYFILE* SYY = NULL;
    KDB*    C_kdb = NULL;

public:
    ImportCommentsBST() 
    {
        imp_keys = ImportObjsDIF_KEYS; // Table of keywords (see YY group of functions in scr4 libs)
        imp_dim = 8;                   // Nb of keys in imp_keys
    }

    int read_header(ImportCmtFromFile*, char*, int) override;
    int read_comment(char*, char**) override;
    int close() override;

private:
    YYFILE* open_file(YYKEYS* keys, int dim, char* name, char* suffix);
    char* add_ftr(char* cmt, long rubr, int lang);
    int sub_read_header(int lang);
    int DIF_long(YYFILE* yy, long* l_val);
    int get_niv(char* name);
};

class ImportCommentsPRN : public ImportCmtFromFile 
{
    YYFILE* PYY = NULL;

public:
    int read_header(ImportCmtFromFile*, char*, int) override;
    int read_comment(char* name, char** cmt) override;
};

class ImportCommentsTXT : public ImportCmtFromFile 
{
    FILE*   TXT_fd = NULL;
    SAMPLE  TXT_smpl;
    char    TXT_freq = 0;
    int     TXT_nbper = 0;
    int     TXT_lang = 0;

public:
    int read_header(ImportCmtFromFile*, char*, int) override;
    int read_comment(char* name, char** cmt) override;
    int close() override;
};

inline std::array<std::unique_ptr<ImportCmtFromFile>, IODE_NB_IMPORT_FORMATS> import_comments = 
{
    std::make_unique<ImportCommentsASCII>(),
    nullptr,                                    // No comments for ROT_ASCII
    nullptr,                                    // No comments for DIF
    std::make_unique<ImportCommentsBST>(),
    nullptr,                                    // No comments for NIS
    nullptr,                                    // No comments for GEM
    std::make_unique<ImportCommentsPRN>(),
    std::make_unique<ImportCommentsTXT>()
};

/*---------------- FUNCS -------------------------*/

/* k_imain.c */
KDB *IMP_InterpretVar(ImportVarFromFile *,char *,char *,SAMPLE *);
KDB *IMP_InterpretCmt(ImportCmtFromFile *,char *,char *,int );
int IMP_RuleImport(int ,char *,char *,char *,char *,char *,char *,int ,int );

/* k_rules.c */
int IMP_readrule(char *);
int IMP_change(char **,char **,char *,char *);

/* k_idif.c */
int dif_read_cell(YYFILE *,char **,double *);
int dif_skip_to(YYFILE *,int );
