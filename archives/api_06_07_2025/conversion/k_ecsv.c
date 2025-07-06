/**
 *  @header4iode
 * 
 *  Implementation of the CSV and rotated CSV export functions. See k_emain.c for more information.
 *  
 *  The normal CSV output file has the format below:
 *  
 *      code;comment;1990Y1;1991Y1;1992Y1;1993Y1;...
 *      ACAF; Ondernemingen: ontvangen kapitaaloverdrachten.; 23.771;26.240999;30.159;...
 *      ACAG; Totale overheid: netto ontvangen kapitaaloverdrachten.; -28.172186;-30.934;...

 *  
 *  The rotated CSV output file has the format below:
 *  
 *      ; ACAF; ACAG; AOUC; AOUC_; AQC; BENEF; BQY; ...
 *      1990Y1; 23.771; -28.172186; 1; 0.93735915; 1; ...
 *      1991Y1; 26.240999; -30.934; 1.0244334; 0.96466659; ...
 *      .....
 *  
 *  List of functions
 *  -----------------
 *      int EXP_hd_csv(EXPDEF *expdef, KDB* dbv, KDB* dbc, char* outfile)       Opens and initialise a CSV export file.
 *      int EXP_end_csv(EXPDEF* expdef, KDB* dbv, KDB* dbc)                     Saves the footer and closes the CSV export files.
 *      char *EXP_code_csv(char* name, char** code)                             Variable name translation for CSV output.
 *      char *EXP_cmt_csv(KDB* dbc, char* name, char**cmt)                      Creates the CMT text + separator for CSV output. 
 *      char *EXP_elem_csv(KDB* dbv, int nb, int t, char** vec)                 Adds one element of a VAR (KDB[nb][t]) to the export vector in CSV format.
 *      int EXP_vec_csv(EXPDEF* expdef, char* code, char* cmt, char* vec)       Saves one VAR in the csv export file.
 *      int EXP_hd_rcsv(EXPDEF* expdef, KDB* dbv, KDB* dbc, char*outfile)       Opens and initialise a rotated CSV export file.
 *      char *EXP_elem_rcsv(KDB* dbv, int nb, int t, char** vec)                Adds one element of a VAR (KDB[nb][t]) to the export vector in rotated CSV format.
 *      int EXP_vec_rcsv(EXPDEF* expdef, char* code, char* cmt, char* vec)      Saves one VAR in the rotated csv export file.
 *  
 */
#include "api/b_errors.h"
#include "api/objs/kdb.h"
#include "api/objs/objs.h"
#include "api/objs/comments.h"
#include "api/objs/scalars.h"
#include "api/objs/variables.h"
#include "api/conversion/export.h"

/**
 *  Opens and initialise a CSV file.
 *  
 *  @param [in] EXPDEF* expdef      struct that contains implementation for csv output
 *  @param [in] KDB*    dbv         VAR KDB
 *  @param [in] KDB*    dbc         CMT KDB
 *  @param [in] char*   outfile     output filename
 *  @return     int                 0 on success, -1 if outfile cannot be created  
 */
int EXP_hd_csv(EXPDEF *expdef, KDB* dbv, KDB* dbc, char* outfile)
{
    int dim, i;

    expdef->exp_fd = fopen(outfile, "w+");
    if(expdef->exp_fd == 0) {
        B_seterror("Cannot create %s", outfile);
        return(-1);
    }

    fprintf(expdef->exp_fd, "code%scomment%s", EXP_SEP, EXP_SEP);
    dim = KSMPL(dbv)->s_nb;
    for(i = 0; i < dim; i++) {
        fprintf(expdef->exp_fd, "%s%s",
                PER_pertoa(PER_addper(&(KSMPL(dbv)->s_p1), i), NULL), EXP_SEP);
    }
    fprintf(expdef->exp_fd, "\n");
    return(0);
}


/**
 *  Saves the footer and closes the CSV export files.
 *  
 *  @param [in] EXPDEF* expdef      struct that contains implementation for csv output
 *  @param [in] KDB*    dbv         VAR KDB
 *  @param [in] KDB*    dbc         CMT KDB
 *  @return     int                 0 always
 */
int EXP_end_csv(EXPDEF* expdef, KDB* dbv, KDB* dbc)
{
    fclose(expdef->exp_fd);
    return(0);
}


/**
 *  Variable name translation for CSV output.
 *  
 *  @param [in]  char*   name    VAR name
 *  @param [out] char**  code    allocated coded name used has identifier in the output file
 *  @return      char*           pointer to *code
 */
char *EXP_code_csv(char* name, char** code)
{
    return(EXP_addsep(name, code));
}


/**
 *  Creates the CMT text + separator for CSV output. If the comment does not exist, it is replaced by an empty string.
 *  
 *  @param [in]  char*   name    VAR name
 *  @param [out] char**  code    allocated string with the comment + sep
 *  @return      char*           pointer to *code
 */
char *EXP_cmt_csv(KDB* dbc, char* name, char**cmt)
{
    int     pos;
    U_ch    *ccmt;                     /* JMP 19-09-96 */

    pos = K_find(dbc, name);
    if(pos >= 0)  {
        ccmt = KCVAL(dbc, pos);        /* JMP 19-09-96 */
        SCR_replace(ccmt, "\n", " ");  /* JMP 19-09-96 */
        return(EXP_addsep(ccmt, cmt)); /* JMP 19-09-96 */
    }
    else return(EXP_addsep("", cmt));
}


/**
 *  Adds one element of a VAR (KDB[nb][t]) to the vector *vec of the series in CSV format. 
 *  
 *  @param [in]      KDB*   dbv     input KDB
 *  @param [in]      int    nb      position in dbv of the var
 *  @param [in]      int    t       position in VAR of the value
 *  @param [in, out] char** vec     (re-)allocated vector of the VAR values in CSV format
 *  @return          char*          *vec
 */
char *EXP_elem_csv(KDB* dbv, int nb, int t, char** vec)
{
    int     lg, olg;
    char    tmp[81], *buf = NULL;

    EXP_val(tmp, (double)(*KVVAL(dbv, nb, t)));
    EXP_addsep(tmp, &buf);
    lg = (int)strlen(buf) + 1;

    if(*vec == NULL) olg = 0;
    else olg = (int)strlen(*vec);
    *vec = SW_nrealloc(*vec, olg, olg + lg);

    strcat(*vec, buf);
    SW_nfree(buf);
    return(*vec);
}

/**
 *  Saves one VAR in the csv export file.
 *  
 *  @param [in] EXPDEF* expdef  struct that contains implementation for csv output
 *  @param [in] char*   code    code of the VAR  in csv format
 *  @param [in] char*   cmt     comment in csv format
 *  @param [in] char*   vec     values in csv format
 *  @return 
 */
int EXP_vec_csv(EXPDEF* expdef, char* code, char* cmt, char* vec)
{
    fprintf(expdef->exp_fd, "%s %s %s\n",
            (code == NULL ? "" : code),
            (cmt == NULL  ? "" : cmt),
            (vec == NULL  ? "" : vec));
    return(0);        
}

// Exportation functions for CSV output
EXPDEF EXPCSV = {
    EXP_hd_csv,

    EXP_code_csv,
    EXP_cmt_csv,
    EXP_elem_csv,

    EXP_vec_csv,
    EXP_end_csv,
    NULL
};


// =================== ROTATED CSV ==========================

// Same functions as for normal CSV export. See above for details.

int EXP_hd_rcsv(EXPDEF* expdef, KDB* dbv, KDB* dbc, char*outfile)
{
    expdef->exp_fd = fopen(outfile, "w+");
    if(expdef->exp_fd == 0) {
        B_seterror("Cannot create %s", outfile);
        return(-1);
    }
    return(0);
}

char *EXP_elem_rcsv(KDB* dbv, int nb, int t, char** vec)
{
    char    tmp[81], *buf = NULL;

    EXP_val(tmp, (double)(*KVVAL(dbv, nb, t)));
    EXP_addsep(tmp, &buf);
    if(vec) *vec = buf;
    return(buf);
}

int EXP_vec_rcsv(EXPDEF* expdef, char* code, char* cmt, char* vec)
{
    if(code == 0) {
        fprintf(expdef->exp_fd, "\n");
        return(0);
    }
    fprintf(expdef->exp_fd, "%s ", code);
    return(0);
}

EXPDEF EXPRCSV = {
    EXP_hd_rcsv,

    EXP_code_csv,
    EXP_cmt_csv,
    EXP_elem_rcsv,
    EXP_vec_rcsv,

    EXP_end_csv,
    NULL
};


