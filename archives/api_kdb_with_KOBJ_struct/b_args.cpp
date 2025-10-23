/**
 * @header4iode
 *
 * Basic functions for managing function and report arguments.
 *
 *  Main functions
 *  --------------
 *      char **B_ainit_chk(char* arg, ADEF* adef, int nb)                   : expands an argument by replacing @filename and $listname by their contents
 *      char **B_vtom_chk(char* arg, int nb)                                : splits a string (generally a function argument) into a table of strings. 
 *      int B_loop(char *argv[], int (*fn)(char*, void*), char* client)     : executes the function fn(char*, char*) for each string in the table of strings argv.
 *      int B_ainit_loop(char* arg, int (*fn)(char*, void*), char* client)  : calls B_ainit_check() to expand arg, then calls B_loop() on the resulting table of strings.
 *      int B_get_arg0(char* arg0, char*arg, int lg)                        : computes arg0, the first arg ('word') of max lg bytes, in the string arg. 
 *      int B_argpos(char* str, int ch)                                     : returns the position of a char in a string. 
 *   
 */
#include "scr4/s_prost.h"
#include "scr4/s_strs.h"

#include "api/constants.h"
#include "b_args.h"
#include "b_errors.h"
#include "api/time/period.h"
#include "api/time/sample.h"
#include "api/report/engine/engine.h"       // SCR_vtomsq
#include <algorithm>    // for std::min, std::max


/**
 *  Expands an argument by replacing @filename and $listname by their contents : 
 *  
 *      @filename : replaces @filename by the content of the file "filename"
 *      $listname : replaces @listname by the content of the list "listname"
 *  
 *  Expansion can be recursive, i.e. if the file contains $LST1 or/and @otherfile, 
 *  $LST1 and @otherfile will be recursively replaced up to 10 levels of depth.
 *  
 *  The string resulting from the expansion is then split on the separators defined in 
 *      A_SEPS, by default " ,\n\t\r".
 *  
 *  
 *  Info : when A_NO_EXPANDSTAR_DFT is null, which is never the case in IODE, 
 *  filenames matching arg (ex "*.txt") will be included in the resulting table of strings.
 *  
 *  If nb is > 0, the function checks that, after expanding arg, the resulting number or arguments 
 *  equals nb (the expected value). 
 *  
 *  On error, IodeErrorManager::append_error() is called and the function returns NULL.
 *  
 *  Calls A_init() and A_check() functions from the s_args group (see http://xon.be/scr4/libs1/libs12.htm)
 *  
 *  @param [in] arg     char*   argument    
 *  @param [in] adef    ADEF*   list of ADEF structures (always NULL in the IODE functions)
 *  @param [in] nb      int     0 or expected number of arguments after expansion if adef is not NULL
 *  @return             char**  NULL on error 
 *                              list of arguments after expansion
 *  
 */
char **B_ainit_chk(char* arg, ADEF* adef, int nb)
{
    char    **args;
    
    A_NO_EXPANDSTAR_DFT = 1; // Suppress default filename expansion JMP 14/01/2022 
    
    args = A_init(arg);
    if(args == 0) return(args);
    if((adef && A_check(args, adef)) ||
            (nb > 0 && SCR_tbl_size((unsigned char**) args) != nb)) {
        error_manager.append_error("Illegal argument(s)");
        A_free((unsigned char**) args);
        args = 0;
    }

    return(args);
}


/**
 *  Splits a string (generally a function argument) into a table of strings. 
 *  The possible string separators are the chars in B_SEPS (by default " ,\n\t").
 *  
 *  If nb is not null, checks that the number of parameters is equal to n. 
 *  
 *  On error, IodeErrorManager::append_error() is called and the function returns NULL.
 *  
 *  @param [in] arg char*   argument
 *  @param [in] nb  int     if not null, expected nb of args after splitting arg
 *  @return         char**  NULL on error or if arg is NULL        
 *                          table of string (split arg)
 *  
 */
char **B_vtom_chk(char* arg, int nb)
{
    unsigned char **args;
    char *tmp = (char*) SCR_stracpy((unsigned char*) arg);       // need to create a copy of arg to avoid segmentation fault 
                                        // when called from C++/cython: 

    args = SCR_vtomsq(tmp, B_SEPS, '"');
    if(args == 0) return((char**) args);
    if((nb > 0 && SCR_tbl_size(args) != nb)) {
        error_manager.append_error("Illegal argument(s)");
        SCR_free_tbl(args);
        args = 0;
    }

    return((char**) args);
}


/**
 *  Executes the function fn(char*, char*) for each string in the table of strings argv.
 *  Stops as soon as fn(arg) returns a non null value.
 *  
 *  Syntax of fn if client is not null:
 *      int fn(char* arg, char* client)
 *  
 *  Syntax of fn if client is null:
 *      int fn(char* arg)
 *   
 *  @param [in] argv    char**                  table of strings
 *  @param [in] fn      int (*fn)(char*, char*) fn pointer
 *  @param [in] client  char*                   2d param of fn() 
 *  @return             int                     result of the last call to fn()
 *  
 */
int B_loop(char *argv[], int (*fn)(char*, void*), char* client)
{
    int     i, rc;

    for(i = 0 ; argv[i] ; i++) {
        if(client == NULL) rc = (*fn)(argv[i], NULL);
        else rc = (*fn)(argv[i], client);
        if(rc) return(rc);
    }

    return(0);
}


/**
 *  Calls B_ainit_check() to expand arg, then calls B_loop() on the resulting table of strings.
 *  
 *  @see B_ainit_check() and B_loop().
 *  
 *  @param [in] arg     char*                   argument 
 *  @param [in] fn      int (*fn)(char*, char*) fn pointer
 *  @param [in] client  char*                   2d param of fn()
 *  @return             int                     result of the last call to fn()
 *  
 */
int B_ainit_loop(char* arg, int (*fn)(char*, void*), char* client)
{
    char    **argv;
    int     rc;

    if((argv = B_ainit_chk(arg, 0L, 0)) == 0) return(-1);
    rc = B_loop(argv, fn, client);
    A_free((unsigned char**) argv);
    return(rc);
}


/**
 *  Computes arg0, the first arg ('word') of max lg bytes, in the string arg. 
 *  
 *  Example: 
 *      char arg0[21];
 *  
 *      B_get_arg0(arg0 , " ACAF ACAG XYZ ", sizeof(arg0));  
 *      printf("'%s'", txt); // 'ACAF'
 *  
 *  @param [out] arg0    char*  first arg in the string arg 
 *  @param [in]  arg     char*  any string 
 *  @param [in]  lg      int    max length of arg0
 *  @return              int    length of arg0
 *  
 *  @details 
 */
int B_get_arg0(char* arg0, char*arg, int lg)
{
    int     i;

    SCR_replace((unsigned char*) arg, (unsigned char*) "\t", (unsigned char*) " ");
    U_ljust_text((unsigned char*) arg);
    for(i = 0; i < lg - 1 && arg[i] ; i++) {
        if(U_is_in(arg[i], B_SEPS)) break;
        arg0[i] = arg[i];
    }
    arg0[i] = 0;
    return(i);
}


/**
 *  Returns the position of a char in a string. 
 *  If not found, returns 0, which is considered as the default position.
 *  
 *  ch is translated in uppercase before searching. str must therefore be in uppercase.
 *  
 *  @param [in] str     char*   list of uppercase characters
 *  @param [in] ch      int     character to search in str
 *  @return             int     position of upper(ch) in str or 0 if not found
 *  
 */
int B_argpos(char* str, int ch)
{
    int     pos;

    ch = SCR_upper_char(ch);
    pos = get_pos_in_char_array(str, ch);
    pos = std::max(0, pos);
    return(pos);
}
