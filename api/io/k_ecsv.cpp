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
 *      int write_header(ExportToFile *expdef, KDB* dbv, KDB* dbc, char* outfile)       Opens and initialise a CSV export file.
 *      int close(ExportToFile* expdef, KDB* dbv, KDB* dbc)                     Saves the footer and closes the CSV export files.
 *      char *write_object_name(char* name, char** code)                             Variable name translation for CSV output.
 *      char *extract_comment(KDB* dbc, char* name, char**cmt)                      Creates the CMT text + separator for CSV output. 
 *      char *get_variable_value(KDB* dbv, int nb, int t, char** vec)                 Adds one element of a VAR (KDB[nb][t]) to the export vector in CSV format.
 *      int write_variable_and_comment(ExportToFile* expdef, char* code, char* cmt, char* vec)       Saves one VAR in the csv export file.
 *      int write_header(ExportToFile* expdef, KDB* dbv, KDB* dbc, char*outfile)       Opens and initialise a rotated CSV export file.
 *      char *get_variable_value(KDB* dbv, int nb, int t, char** vec)                Adds one element of a VAR (KDB[nb][t]) to the export vector in rotated CSV format.
 *      int write_variable_and_comment(ExportToFile* expdef, char* code, char* cmt, char* vec)      Saves one VAR in the rotated csv export file.
 *  
 */
#include "api/b_errors.h"
#include "api/objs/kdb.h"
#include "api/objs/objs.h"
#include "api/objs/comments.h"
#include "api/objs/scalars.h"
#include "api/objs/variables.h"
#include "api/io/export.h"


/**
 *  Opens and initialise a CSV file.
 *  
 *  @param [in] ExportToFile* expdef      struct that contains implementation for csv output
 *  @param [in] KDB*    dbv         VAR KDB
 *  @param [in] KDB*    dbc         CMT KDB
 *  @param [in] char*   outfile     output filename
 *  @return     int                 0 on success, -1 if outfile cannot be created  
 */
int ExportObjsCSV::write_header(ExportToFile *expdef, KDB* dbv, KDB* dbc, char* outfile)
{
    int dim, i;

    expdef->file_descriptor.open(outfile);
    if((expdef->file_descriptor.rdstate() & std::ofstream::failbit ) != 0) 
    {
        std::string error_msg = "Cannot create file '" + std::string(outfile) + "'";
        error_manager.append_error(error_msg);
        return(-1);
    }

    expdef->file_descriptor <<  "code" << EXP_SEP << "comment" << EXP_SEP;
    dim = dbv->sample->nb_periods;
    std::string str_period;
    for(i = 0; i < dim; i++) 
    {
        Period period = dbv->sample->start_period.shift(i);
        str_period = period.to_string();
        expdef->file_descriptor << str_period << EXP_SEP;
    }
    expdef->file_descriptor << "\n";
    return(0);
}

/**
 *  Saves the footer and closes the CSV export files.
 *  
 *  @param [in] ExportToFile* expdef      struct that contains implementation for csv output
 *  @param [in] KDB*    dbv         VAR KDB
 *  @param [in] KDB*    dbc         CMT KDB
 *  @return     int                 0 always
 */
int ExportObjsCSV::close(ExportToFile* expdef, KDB* dbv, KDB* dbc, char* outfile)
{
    // No footer needed for CSV output
    expdef->file_descriptor.close();
    return(0);
}

/**
 *  Variable name translation for CSV output.
 *  
 *  @param [in]  char*   name    VAR name
 *  @param [out] char**  code    allocated coded name used has identifier in the output file
 *  @return      char*           pointer to *code
 */
char* ExportObjsCSV::write_object_name(char* name, char** code)
{
    return(write_separator(name, code));
}

/**
 *  Creates the CMT text + separator for CSV output. If the comment does not exist, it is replaced by an empty string.
 *  
 *  @param [in]  char*   name    VAR name
 *  @param [out] char**  code    allocated string with the comment + sep
 *  @return      char*           pointer to *code
 */
char* ExportObjsCSV::extract_comment(KDB* dbc, char* name, char**cmt)
{
    int     pos;
    U_ch    *ccmt;                     /* JMP 19-09-96 */

    pos = dbc->index_of(name);
    if(pos >= 0)  
    {
        ccmt = (unsigned char*) KCVAL(dbc, pos);        /* JMP 19-09-96 */
        SCR_replace(ccmt, (unsigned char*) "\n", (unsigned char*) "");  /* JMP 19-09-96 */
        return(write_separator((char*) ccmt, cmt)); /* JMP 19-09-96 */
    }
    else 
        return(write_separator("", cmt));
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
char* ExportObjsCSV::get_variable_value(KDB* dbv, int nb, int t, char** vec)
{
    int     lg, olg;
    char    tmp[81], *buf = NULL;

    write_value(tmp, (double)(*KVVAL(dbv, nb, t)));
    write_separator(tmp, &buf);
    lg = (int)strlen(buf) + 1;

    if(*vec == NULL) 
        olg = 0;
    else 
        olg = (int)strlen(*vec);
    *vec = (char*) SW_nrealloc(*vec, olg, olg + lg);

    strcat(*vec, buf);
    SW_nfree(buf);
    return(*vec);
}

/**
 *  Saves one VAR in the csv export file.
 *  
 *  @param [in] ExportToFile* expdef  struct that contains implementation for csv output
 *  @param [in] char*   code    code of the VAR  in csv format
 *  @param [in] char*   cmt     comment in csv format
 *  @param [in] char*   vec     values in csv format
 *  @return 
 */
int ExportObjsCSV::write_variable_and_comment(ExportToFile* expdef, char* code, char* cmt, char* vec)
{
    expdef->file_descriptor << (code == NULL ? "" : code) << " " << (cmt == NULL  ? "" : cmt)
                            << " " << (vec == NULL  ? "" : vec) << "\n";
    return(0);        
}


// =================== ROTATED CSV ==========================

// Same functions as for normal CSV export. See above for details.

int ExportObjsRevertCSV::write_header(ExportToFile* expdef, KDB* dbv, KDB* dbc, char*outfile)
{
    expdef->file_descriptor.open(outfile);
    if((expdef->file_descriptor.rdstate() & std::ofstream::failbit ) != 0) 
    {
        std::string error_msg = "Cannot create file '" + std::string(outfile) + "'";
        error_manager.append_error(error_msg);
        return(-1);
    }
    return(0);
}

char* ExportObjsRevertCSV::write_object_name(char* name, char** code)
{
    return(write_separator(name, code));
}

char* ExportObjsRevertCSV::get_variable_value(KDB* dbv, int nb, int t, char** vec)
{
    char    tmp[81], *buf = NULL;

    write_value(tmp, (double)(*KVVAL(dbv, nb, t)));
    write_separator(tmp, &buf);
    if(vec) *vec = buf;
    return(buf);
}

int ExportObjsRevertCSV::write_variable_and_comment(ExportToFile* expdef, char* code, char* cmt, char* vec)
{
    if(code == 0) 
    {
        expdef->file_descriptor << "\n";
        return(0);
    }
    expdef->file_descriptor << code << " ";
    return(0);
}

int ExportObjsRevertCSV::close(ExportToFile* expdef, KDB* dbv, KDB* dbc, char* outfile)
{
    // No footer needed for CSV output
    expdef->file_descriptor.close();
    return(0);
}
