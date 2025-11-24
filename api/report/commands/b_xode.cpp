/**
 *  @header4iode
 * 
 *  Report functions to import comments and variables from various non-IODE formats.
 *  
 *  See b_data.c for more details on the report function syntax and logic.
 *  
 *  List of functions 
 *  -----------------
 *      int B_FileImportCmt(char* arg, int unused)  $FileImportCmt format rule infile outfile language [trace]
 *      int B_FileImportVar(char* arg, int unused)  $FileImportVar format rule infile outfile from to  [trace]
 */

 
#include "api/b_args.h"
#include "api/b_errors.h"
#include "api/io/import.h"

#include "api/report/commands/commands.h"


/**
 *  Syntax: $FileImportCmt format rule infile outfile language [trace]
 *  where:
 *      format = {Ascii, Rotated_Ascii, DIF, NIS, GEM, PRN, TXT_Belgostat}
 *      rule = rulefile to use to translate names
 *      infile = file to import
 *      outfile = IODE-file with imported series
 *      language =  {E,F,D}
 *      trace = debug file (optional)
 *  
 *  @see https://iode.plan.be/doku.php?id=fileimportcmt
 */
int B_FileImportCmt(char* arg, int unused)
{
    int     rc = 0, nb_args, format, lang;
    char    **args = NULL, *trace, *rule, *infile, *oufile, empty_buf[1];

    empty_buf[0] = 0;
    args = B_ainit_chk(arg, NULL, 0);
    nb_args = SCR_tbl_size((unsigned char**) args);
    if(nb_args < 5) 
    {
        error_manager.append_error("DataEditGraph : Syntax error");
        rc = -1;
        goto fin;
    }

    format = B_argpos("ARDDNGPT", args[0][0]);
    rule   = args[1];
    infile = args[2];
    oufile = args[3];
    lang   = B_argpos("EFD", args[4][0]);

    if(nb_args == 6) 
        trace = args[5];
    else             
        trace = empty_buf;

    rc = IMP_RuleImport(COMMENTS, trace, rule, oufile, infile,
                        empty_buf, empty_buf, format, lang);

fin:
    A_free((unsigned char**) args);
    return(rc);
}


/**
 *  Syntax:  $FileImportVar format rule infile outfile from to  [trace]
 *  where:
 *      format = {Ascii, Rotated_Ascii, DIF, NIS, GEM, PRN, TXT_Belgostat}
 *      rule = rulefile to use to translate names
 *      infile = file to import
 *      outfile = IODE-file with imported series
 *      from = begin of sample
 *      to   = end of sample
 *      language =  {E,F,D}
 *      trace = debug file (optional)
 *  
 *  @see https://iode.plan.be/doku.php?id=fileimportvar
 */
int B_FileImportVar(char* arg, int unused)
{
    int     rc = 0, nb_args, format;
    char    **args = NULL, *trace, *rule, *from, *to,*infile, *oufile, empty_buf[1];

    empty_buf[0] = 0;
    args = B_ainit_chk(arg, NULL, 0);
    nb_args = SCR_tbl_size((unsigned char**) args);    /* JMP 16-12-93 */
    if(nb_args < 6) 
    {
        error_manager.append_error("DataEditGraph : Syntax error");
        rc = -1;
        goto fin;
    }

    format = B_argpos("ARDDNGPT", args[0][0]);
    rule   = args[1];
    infile = args[2];
    oufile = args[3];
    from   = args[4];
    to     = args[5];

    if(nb_args == 7) 
        trace = args[6];
    else             
        trace = empty_buf;

    rc = IMP_RuleImport(VARIABLES, trace, rule, oufile, infile,
                        from, to, format, 0);

fin:
    A_free((unsigned char**) args);
    return(rc);
}
