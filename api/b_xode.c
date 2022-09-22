/**
 *  @header4iode
 * 
 *  Report functions to import comments and variables from various non-IODE formats.
 *  
 *  See b_data.c for more details on the report function syntax and logic.
 *  
 *  List of functions 
 *  -----------------
 *      int B_FileImportCmt(char* arg)  $FileImportCmt format rule infile outfile language [trace]
 *      int B_FileImportVar(char* arg)  $FileImportVar format rule infile outfile from to  [trace]$FileImportVar format rule infile outfile from to  [trace]
 */

#include "iode.h"


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
int B_FileImportCmt(char* arg)
{
    int     rc = 0, nb_args, format, lang;
    char    **args = NULL, *trace, *rule, *infile, *oufile;

    args = B_ainit_chk(arg, NULL, 0);
    nb_args = SCR_tbl_size(args);
    if(nb_args < 5) {
        B_seterrn(67);
        rc = -1;
        goto fin;
    }

    format = B_argpos("ARDDNGPT", args[0][0]);
    rule   = args[1];
    infile = args[2];
    oufile = args[3];
    lang   = B_argpos("EFD", args[4][0]);

    if(nb_args == 6) trace = args[5];
    else             trace = "";

    rc = IMP_RuleImport(K_CMT, trace, rule, oufile, infile,
                      "", "", format, lang);

fin:
    A_free(args);
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
int B_FileImportVar(char* arg)
{
    int     rc = 0, nb_args, format, lang;
    char    **args = NULL, *trace, *rule, *from, *to,*infile, *oufile;

    args = B_ainit_chk(arg, NULL, 0);
    nb_args = SCR_tbl_size(args);    /* JMP 16-12-93 */
    if(nb_args < 6) {
        B_seterrn(67);
        rc = -1;
        goto fin;
    }

    format = B_argpos("ARDDNGPT", args[0][0]);
    rule   = args[1];
    infile = args[2];
    oufile = args[3];
    from   = args[4];
    to     = args[5];

    if(nb_args == 7) trace = args[6];
    else             trace = "";

    rc = IMP_RuleImport(K_VAR, trace, rule, oufile, infile,
                      from, to, format, 0);

fin:
    A_free(args);
    return(rc);
}


