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
 *      int write_header(ExportToFile *expdef, KDB* dbv, KDB* dbc, char* outfile)       Opens and initialise a tsp export file.
 *      int close(ExportToFile* expdef, KDB* dbv, KDB* dbc)                     Saves the footer and closes the tsp export files.
 *      char *write_object_name(char* name, char** code)                             Variable name translation for tsp output.
 *      char *extract_comment(KDB* dbc, char* name, char**cmt)                      Creates the CMT text + separator for tsp output. 
 *      char *get_variable_value(KDB* dbv, int nb, int t, char** vec)                 Adds one element of a VAR (KDB[nb][t]) to the export vector in tsp format.
 *      int write_variable_and_comment(ExportToFile* expdef, char* code, char* cmt, char* vec)       Saves one VAR in the tsp export file.
 */
#include "api/objs/kdb.h"
#include "api/objs/objs.h"
#include "api/objs/comments.h"
#include "api/objs/scalars.h"
#include "api/objs/variables.h"
#include "api/io/export.h"


int ExportObjsTSP::write_header(ExportToFile* expdef, KDB* dbv, KDB* dbc, char* outfile)
{
    int     freq;

    expdef->file_descriptor = fopen(outfile, "w+");

    freq = PER_nb((KSMPL(dbv)->s_p1).p_p);
    switch(freq) {
        case 1:
            fprintf(expdef->file_descriptor,
                    "FREQ A;\nSMPL %ld %ld ;\n",
                    (KSMPL(dbv)->s_p1).p_y, (KSMPL(dbv)->s_p2).p_y);
            break;

        case 4:
            fprintf(expdef->file_descriptor,
                    "FREQ Q;\nSMPL %ld:%ld %ld:%ld ;\n",
                    (KSMPL(dbv)->s_p1).p_y, (KSMPL(dbv)->s_p1).p_s,
                    (KSMPL(dbv)->s_p2).p_y, (KSMPL(dbv)->s_p2).p_s);
            break;

        case 12:
            fprintf(expdef->file_descriptor,
                    "FREQ M;\nSMPL %ld:%ld %ld:%ld ;\n",
                    (KSMPL(dbv)->s_p1).p_y, (KSMPL(dbv)->s_p1).p_s,
                    (KSMPL(dbv)->s_p2).p_y, (KSMPL(dbv)->s_p2).p_s);
            break;
    }
    return(0);
}

int ExportObjsTSP::close(ExportToFile* expdef, KDB* dbv, KDB* dbc, char* outfile)
{
    fprintf(expdef->file_descriptor, " ;\n");
    fprintf(expdef->file_descriptor, "END\n");
    fclose(expdef->file_descriptor);
    return(0);
}

char* ExportObjsTSP::write_object_name(char* name, char** code)
{
    return(write_pre_post("LOAD ", " ;\n", name, code));
}

char* ExportObjsTSP::extract_comment(KDB* dbc, char* name, char** cmt)
{
    int pos;

    pos = K_find(dbc, name);
    if(pos >= 0)  
        *cmt = (char*) SCR_stracpy((unsigned char*) KCVAL(dbc, pos));
    else 
        *cmt = (char*) SCR_stracpy((unsigned char*) " ");
    
    return(*cmt);
}

char* ExportObjsTSP::get_variable_value(KDB* dbv, int nb, int t, char** vec)
{
    int     lg, olg;
    char    tmp[81], *buf = NULL;

    write_value(tmp, (double)(*KVVAL(dbv, nb, t)));
    write_pre_post("", " ", tmp, &buf);
    lg = (int)strlen(buf) + 1;

    if(*vec == NULL) olg = 0;
    else olg = (int)strlen(*vec);
    *vec = (char*) SW_nrealloc(*vec, olg, olg + lg);

    strcat(*vec, buf);
    SW_nfree(buf);
    return(*vec);
}

int ExportObjsTSP::write_variable_and_comment(ExportToFile* expdef, char* code, char* cmt, char* vec)
{
    int     i;
    char    **text;

    fprintf(expdef->file_descriptor, "%s \n", code);
    if(cmt) { /* JMP 04-03-99 */
        text = (char**) SCR_text((unsigned char*) cmt, (unsigned char*) " ", 75);
        for(i = 0; text[i]; i++)  fprintf(expdef->file_descriptor, "? %s\n", text[i]);
        SCR_free_tbl((unsigned char**) text);
    }

    text = (char**) SCR_text((unsigned char*) vec, (unsigned char*) " ", 80);
    for(i = 0; text[i]; i++)  fprintf(expdef->file_descriptor, "%s\n", text[i]);
    fprintf(expdef->file_descriptor, " ;\n");
    SCR_free_tbl((unsigned char**) text);
    return(0);
}
