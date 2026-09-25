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
 *
 */
int B_FileImportCmt(char* arg, int unused)
{
    int     rc = 0, format, lang;
    char    *trace, *rule, *infile, *oufile, empty_buf[1];

    empty_buf[0] = 0;
    std::vector<std::string> v_args = expand_arg(arg, 0);
    if(v_args.size() < 5) 
    {
        error_manager.append_error("DataEditGraph : Syntax error");
        rc = -1;
        goto fin;
    }

    format = B_argpos("ARDDNGPT", v_args[0][0]);
    rule   = v_args[1].data();
    infile = v_args[2].data();
    oufile = v_args[3].data();
    lang   = B_argpos("EFD", v_args[4].data()[0]);

    if(v_args.size() == 6) 
        trace = v_args[5].data();
    else             
        trace = empty_buf;

    rc = IMP_RuleImport(COMMENTS, trace, rule, oufile, infile,
                        empty_buf, empty_buf, format, lang);

fin:
    return rc;
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
 *
 */
int B_FileImportVar(char* arg, int unused)
{
    int     rc = 0, format;
    char    *trace, *rule, *from, *to, *infile, *oufile, empty_buf[1];

    empty_buf[0] = 0;
    std::vector<std::string> v_args = expand_arg(arg, 0);
    if(v_args.size() < 6) 
    {
        error_manager.append_error("DataEditGraph : Syntax error");
        rc = -1;
        goto fin;
    }

    format = B_argpos("ARDDNGPT", v_args[0][0]);
    rule   = v_args[1].data();
    infile = v_args[2].data();
    oufile = v_args[3].data();
    from   = v_args[4].data();
    to     = v_args[5].data();

    if(v_args.size() == 7) 
        trace = v_args[6].data();
    else             
        trace = empty_buf;

    rc = IMP_RuleImport(VARIABLES, trace, rule, oufile, infile,
                        from, to, format, 0);

fin:
    return rc;
}
