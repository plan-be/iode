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
 *      char *get_variable_value(CKDBVariables* dbv, int nb, int t, char** vec)                 Adds one element of a VAR (KDB[nb][t]) to the export vector in tsp format.
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
    int freq = get_nb_periods_per_year(dbv->sample->start_period.periodicity);
    std::string start_period_year = std::to_string((dbv->sample->start_period).year);
    std::string end_period_year   = std::to_string((dbv->sample->end_period).year);
    std::string start_period_step = std::to_string((dbv->sample->start_period).step);
    std::string end_period_step   = std::to_string((dbv->sample->end_period).step);

    expdef->file_descriptor.open(outfile);

    std::string str_sample;
    std::string str_periodicity;
    switch(freq) {
        case 1:
            str_periodicity = "A";
            str_sample = start_period_year + " " + end_period_year;
            break;

        case 4:
            str_periodicity = "Q";
            str_sample = start_period_year + ":" + start_period_step + " "; 
            str_sample += end_period_year + ":" + end_period_step;
            break;

        case 12:
            str_periodicity = "M";
            str_sample = start_period_year + ":" + start_period_step + " "; 
            str_sample += end_period_year + ":" + end_period_step;
            break;
    }

    expdef->file_descriptor << "FREQ " + str_periodicity + ";\nSMPL " << str_sample << " ;\n";
    return(0);
}

int ExportObjsTSP::close(ExportToFile* expdef, KDB* dbv, KDB* dbc, char* outfile)
{
    expdef->file_descriptor.close();
    return(0);
}

char* ExportObjsTSP::write_object_name(char* name, char** code)
{
    return(write_pre_post("LOAD ", " ;\n", name, code));
}

char* ExportObjsTSP::extract_comment(CKDBComments* dbc, char* name, char** cmt)
{
    SWHDL handle = dbc->get_handle(name);
    if(handle > 0)  
        *cmt = (char*) SCR_stracpy((unsigned char*) dbc->get_obj(handle));
    else 
        *cmt = (char*) SCR_stracpy((unsigned char*) " ");
    
    return(*cmt);
}

char* ExportObjsTSP::get_variable_value(CKDBVariables* dbv, int nb, int t, char** vec)
{
    int     lg, olg;
    char    tmp[81], *buf = NULL;

    std::string name = dbv->get_name(nb);
    double* value_ptr = dbv->get_var_ptr(name, t);
    write_value(tmp, *value_ptr);
    write_pre_post("", " ", tmp, &buf);
    lg = (int) strlen(buf) + 1;

    if(*vec == NULL) 
        olg = 0;
    else 
        olg = (int) strlen(*vec);
    *vec = (char*) SW_nrealloc(*vec, olg, olg + lg);

    strcat(*vec, buf);
    SW_nfree(buf);
    return(*vec);
}

int ExportObjsTSP::write_variable_and_comment(ExportToFile* expdef, char* code, char* cmt, char* vec)
{
    int     i;
    char    **text;

    expdef->file_descriptor <<  code << " \n";
    if(cmt) 
    {
        text = (char**) SCR_text((unsigned char*) cmt, (unsigned char*) " ", 75);
        for(i = 0; text[i]; i++)  
            expdef->file_descriptor <<  "? " << text[i] << "\n";
        SCR_free_tbl((unsigned char**) text);
    }

    text = (char**) SCR_text((unsigned char*) vec, (unsigned char*) " ", 80);
    for(i = 0; text[i]; i++)  
        expdef->file_descriptor << text[i] << "\n";
    expdef->file_descriptor <<  " ;\n";
    SCR_free_tbl((unsigned char**) text);
    return(0);
}
