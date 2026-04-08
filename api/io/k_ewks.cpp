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
 *      int write_header(KDB* dbv, KDB* dbc, char* outfile)       Opens and initialise a wks export file.
 *      int close(KDB* dbv, KDB* dbc)                     Saves the footer and closes the wks export files.
 *      char *write_object_name(char* name, char** code)                             Variable name translation for wks output.
 *      char *extract_comment(KDB* dbc, char* name, char**cmt)                      Creates the CMT text + separator for wks output. 
 *      char *get_variable_value(KDBVariablesPtr dbv, int nb, int t, char** vec)                 Adds one element of a VAR (KDB[nb][t]) to the export vector in wks format.
 *      int write_variable_and_comment(char* code, char* cmt, char* vec)       Saves one VAR in the wks export file.
 */
#include "api/objs/kdb.h"
#include "api/objs/objs.h"
#include "api/objs/comments.h"
#include "api/objs/scalars.h"
#include "api/objs/variables.h"
#include "api/io/export.h"


int ExportObjsWKS:: write_header(const KDBVariablesPtr dbv_ptr, const KDBCommentsPtr dbc_ptr, char* outfile)
{
    int dim, nb, i;

    WKS_COL = 1, WKS_ROW = 1;
    dim = dbv_ptr->sample->nb_periods;
    nb = dbv_ptr->size();

    wks_init(outfile, dim + 2, nb + 1);
    wks_cwidth(2, 20);

    std::string period_str;
    for(i = 0, WKS_COL = 3; i < dim; i++, WKS_COL++)
    {
        Period period = dbv_ptr->sample->start_period.shift(i);
        period_str = period.to_string();
        wks_string((char*) period_str.c_str(), WKS_COL, WKS_ROW);
    }
    WKS_ROW ++;
    WKS_COL = 1;
    return 0;
}

int ExportObjsWKS::close(const KDBVariablesPtr dbv_ptr, const KDBCommentsPtr dbc_ptr, char* outfile)
{
    wks_end();
    return 0;
}

char* ExportObjsWKS::write_object_name(char* name, char** code)
{
    wks_string(name, WKS_COL, WKS_ROW);
    WKS_COL ++;
    *code = (char*) SCR_stracpy((unsigned char*) name);
    return(*code);
}

char* ExportObjsWKS::extract_comment(const KDBCommentsPtr dbc_ptr, char* name, char** cmt)
{
    if(dbc_ptr->contains(name))
    {
        std::shared_ptr<Comment> cmt_ptr = dbc_ptr->get_obj_ptr(name);
        Comment cmt_utf8 = *cmt_ptr;
        Comment cmt_oem = utf8_to_oem(cmt_utf8);
        wks_string((char*) cmt_oem.c_str(), WKS_COL, WKS_ROW);
    }
    else 
        wks_string(" ", WKS_COL, WKS_ROW);
    WKS_COL ++;

    *cmt = NULL;
    return *cmt;
}

char* ExportObjsWKS::get_variable_value(const KDBVariablesPtr dbv_ptr, int nb, int t, char** vec)
{
    char* buf = NULL;

    std::string name = dbv_ptr->get_name(nb);
    double value = dbv_ptr->get_var(name, t);
    wks_value(value, WKS_COL, WKS_ROW);
    WKS_COL++;

    return(*vec = NULL);
}

int ExportObjsWKS::write_variable_and_comment(char* code, char* cmt, char* vec)
{
    wks_name(code, 3, WKS_ROW, WKS_COL, WKS_ROW);
    WKS_ROW ++;
    WKS_COL = 1;
    return 0;
}
