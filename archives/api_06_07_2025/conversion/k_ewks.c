/**
 *  @header4iode
 * 
 *  Implementation of the (obsolete) WKS export functions. 
 *  
 *  See k_emain.c for more information.
 *  See and k_ecsv.c for a detailed example.
 *  
 *  
 *  List of functions
 *  -----------------
 *      int EXP_hd_wks(EXPDEF *expdef, KDB* dbv, KDB* dbc, char* outfile)       Opens and initialise a wks export file.
 *      int EXP_end_wks(EXPDEF* expdef, KDB* dbv, KDB* dbc)                     Saves the footer and closes the wks export files.
 *      char *EXP_code_wks(char* name, char** code)                             Variable name translation for wks output.
 *      char *EXP_cmt_wks(KDB* dbc, char* name, char**cmt)                      Creates the CMT text + separator for wks output. 
 *      char *EXP_elem_wks(KDB* dbv, int nb, int t, char** vec)                 Adds one element of a VAR (KDB[nb][t]) to the export vector in wks format.
 *      int EXP_vec_wks(EXPDEF* expdef, char* code, char* cmt, char* vec)       Saves one VAR in the wks export file.
 */
#include "api/objs/kdb.h"
#include "api/objs/objs.h"
#include "api/objs/comments.h"
#include "api/objs/scalars.h"
#include "api/objs/variables.h"
#include "api/conversion/export.h"


int WKS_COL = 1, WKS_ROW = 1;

int EXP_hd_wks(EXPDEF* expdef, KDB* dbv, KDB* dbc, char* outfile)
{
    int dim, nb, i;

    WKS_COL = 1, WKS_ROW = 1;
    dim = KSMPL(dbv)->s_nb;
    nb = KNB(dbv);

    wks_init(outfile, dim + 2, nb + 1);
    wks_cwidth(2, 20);
    for(i = 0, WKS_COL = 3; i < dim; i++, WKS_COL++)
        wks_string(PER_pertoa(PER_addper(&(KSMPL(dbv)->s_p1), i), NULL),
                   WKS_COL, WKS_ROW);
    WKS_ROW ++;
    WKS_COL = 1;
    return(0);
}

int EXP_end_wks(EXPDEF* expdef, KDB* dbv, KDB* dbc)
{
    wks_end();
    return(0);
}


char    *EXP_code_wks(name, code)
char    *name, **code;
{
    wks_string(name, WKS_COL, WKS_ROW);
    WKS_COL ++;
    *code = SCR_stracpy(name);
    return(*code);
}

char    *EXP_cmt_wks(dbc, name, cmt)
KDB     *dbc;
char    *name, **cmt;
{
    int pos;

    pos = K_find(dbc, name);
    if(pos >= 0)  wks_string(KCVAL(dbc, pos), WKS_COL, WKS_ROW);
    else wks_string(" ", WKS_COL, WKS_ROW);
    WKS_COL ++;

    return(*cmt = NULL);
}

char    *EXP_elem_wks(dbv, nb, t, vec)
KDB     *dbv;
int     nb;
char    **vec;
{
    char    *buf = NULL;

    wks_value((double)(*KVVAL(dbv, nb, t)), WKS_COL, WKS_ROW);
    WKS_COL ++;

    return(*vec = NULL);
}

int EXP_vec_wks(expdef, code, cmt, vec)
EXPDEF  *expdef;
char    *code, *cmt, *vec;
{
    wks_name(code, 3, WKS_ROW, WKS_COL, WKS_ROW);
    WKS_ROW ++;
    WKS_COL = 1;
    return(0);
}

EXPDEF EXPWKS = {
    EXP_hd_wks,

    EXP_code_wks,
    EXP_cmt_wks,
    EXP_elem_wks,
    EXP_vec_wks,

    EXP_end_wks,
    NULL
};
