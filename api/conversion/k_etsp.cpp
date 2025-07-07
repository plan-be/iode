/**
 *  @header4iode
 * 
 *  Implementation of the TSF export functions. 
 *  
 *  See k_emain.c for more information.
 *  See and k_ecsv.c for a detailed example.
 *  
 *  
 *  Example of a TSP output file
 *  ----------------------------
 *
 *        FREQ A;
 *        SMPL 1990 2000 ;
 *        LOAD ACAF ;
 *        
 *        ? Ondernemingen: ontvangen kapitaaloverdrachten.
 *        23.771 26.240999 30.159 34.661999 8.1610022 -13.130997 32.171001 39.935799 
 *        29.645657 13.530405 10.046611 
 *        ; 
 *        LOAD ACAG ;
 *        
 *        ? Totale overheid: netto ontvangen kapitaaloverdrachten.
 *        -28.172186 -30.934 -40.285999 -43.157997 -16.029003 -41.845993 -40.237 -32.93 
 *        -38.345695 -39.858174 -41.534787 
 *        ; 
 *        ...
 *  
 *  List of functions
 *  -----------------
 *      int EXP_hd_tsp(EXPDEF *expdef, KDB* dbv, KDB* dbc, char* outfile)       Opens and initialise a tsp export file.
 *      int EXP_end_tsp(EXPDEF* expdef, KDB* dbv, KDB* dbc)                     Saves the footer and closes the tsp export files.
 *      char *EXP_code_tsp(char* name, char** code)                             Variable name translation for tsp output.
 *      char *EXP_cmt_tsp(KDB* dbc, char* name, char**cmt)                      Creates the CMT text + separator for tsp output. 
 *      char *EXP_elem_tsp(KDB* dbv, int nb, int t, char** vec)                 Adds one element of a VAR (KDB[nb][t]) to the export vector in tsp format.
 *      int EXP_vec_tsp(EXPDEF* expdef, char* code, char* cmt, char* vec)       Saves one VAR in the tsp export file.
 */
#include "api/objs/kdb.h"
#include "api/objs/objs.h"
#include "api/objs/comments.h"
#include "api/objs/scalars.h"
#include "api/objs/variables.h"
#include "api/conversion/export.h"


int EXP_hd_tsp(EXPDEF* expdef, KDB* dbv, KDB* dbc, char* outfile)
{
    int     freq;

    expdef->exp_fd = fopen(outfile, "w+");

    freq = PER_nb((KSMPL(dbv)->s_p1).p_p);
    switch(freq) {
        case 1:
            fprintf(expdef->exp_fd,
                    "FREQ A;\nSMPL %ld %ld ;\n",
                    (KSMPL(dbv)->s_p1).p_y, (KSMPL(dbv)->s_p2).p_y);
            break;

        case 4:
            fprintf(expdef->exp_fd,
                    "FREQ Q;\nSMPL %ld:%ld %ld:%ld ;\n",
                    (KSMPL(dbv)->s_p1).p_y, (KSMPL(dbv)->s_p1).p_s,
                    (KSMPL(dbv)->s_p2).p_y, (KSMPL(dbv)->s_p2).p_s);
            break;

        case 12:
            fprintf(expdef->exp_fd,
                    "FREQ M;\nSMPL %ld:%ld %ld:%ld ;\n",
                    (KSMPL(dbv)->s_p1).p_y, (KSMPL(dbv)->s_p1).p_s,
                    (KSMPL(dbv)->s_p2).p_y, (KSMPL(dbv)->s_p2).p_s);
            break;
    }
    return(0);
}

int EXP_end_tsp(EXPDEF* expdef, KDB* dbv, KDB* dbc, char* outfile)
{
    fprintf(expdef->exp_fd, " ;\n");
    fprintf(expdef->exp_fd, "END\n");
    fclose(expdef->exp_fd);
    return(0);
}

char* EXP_code_tsp(char* name, char** code)
{
    return(EXP_addprepost("LOAD ", " ;\n", name, code));
}

char* EXP_cmt_tsp(KDB* dbc, char* name, char** cmt)
{
    int pos;

    pos = K_find(dbc, name);
    if(pos >= 0)  
        *cmt = (char*) SCR_stracpy((unsigned char*) KCVAL(dbc, pos));
    else 
        *cmt = (char*) SCR_stracpy((unsigned char*) " ");
    
    return(*cmt);
}

char* EXP_elem_tsp(KDB* dbv, int nb, int t, char** vec)
{
    int     lg, olg;
    char    tmp[81], *buf = NULL;

    EXP_val(tmp, (double)(*KVVAL(dbv, nb, t)));
    EXP_addprepost("", " ", tmp, &buf);
    lg = (int)strlen(buf) + 1;

    if(*vec == NULL) olg = 0;
    else olg = (int)strlen(*vec);
    *vec = (char*) SW_nrealloc(*vec, olg, olg + lg);

    strcat(*vec, buf);
    SW_nfree(buf);
    return(*vec);
}

int EXP_vec_tsp(EXPDEF* expdef, char* code, char* cmt, char* vec)
{
    int     i;
    char    **text;

    fprintf(expdef->exp_fd, "%s \n", code);
    if(cmt) { /* JMP 04-03-99 */
        text = (char**) SCR_text((unsigned char*) cmt, (unsigned char*) " ", 75);
        for(i = 0; text[i]; i++)  fprintf(expdef->exp_fd, "? %s\n", text[i]);
        SCR_free_tbl((unsigned char**) text);
    }

    text = (char**) SCR_text((unsigned char*) vec, (unsigned char*) " ", 80);
    for(i = 0; text[i]; i++)  fprintf(expdef->exp_fd, "%s\n", text[i]);
    fprintf(expdef->exp_fd, " ;\n");
    SCR_free_tbl((unsigned char**) text);
    return(0);
}

EXPDEF EXPTSP = {
    EXP_hd_tsp,     // exp_hd_fn
    EXP_code_tsp,   // exp_code_fn
    EXP_cmt_tsp,    // exp_cmt_fn
    EXP_elem_tsp,   // exp_elem_fn
    NULL,           // exp_vec_fn
    EXP_vec_tsp,    // exp_vec_var_fn
    EXP_end_tsp,    // exp_end_fn
    NULL            // exp_fd
};
