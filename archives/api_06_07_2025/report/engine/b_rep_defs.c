/**
 *  @header4iode
 *  
 *  IODE report $define implementation 
 *  ----------------------------------
 *  Implementation of the report $defines and of the mechanism to push/pop macros in PROCDEF 
 *  and FOREACH blocks.
 *  
 *  The $defines are stored in a special KDB (RP_MACRO) of type ASIS_CASE.
 *  
 *  List of functions
 *  -----------------
 *      int RP_macro_createdb()                     Creates the KDB RP_MACRO if it does not exist.
 *      int RP_macro_deletedb()                     Deletes the KDB RP_MACRO and its content.
 *      int RP_define_1(char *name, char *macro)    Adds or replaces a macro to RP_MACRO.
 *      int RP_define(char* arg)                    Report function to define a new macro.
 *      char* RP_get_macro_ptr(char* macro_name)    Returns the pointer to a macro (aka define) value.    
 *      int RP_undef_1(char *name)                  Deletes one macro.
 *      int RP_undef(char *arg)                     Report function to delete macros.
 *      int RP_define_calcdepth(char *name)         Returns the max depth of a saved (pushed) macro.
 *      int RP_define_save(char *name)              Saves (pushes) a macro under the name "name#<depth+1>".
 *      int RP_define_restore(char *name)           Deletes the macro "name" and restores (pops) the macro "name#<depth>" under the name "name".
 *      int RP_define_save_list(char **list)        Saves (pushes) a list of macros using RP_define_save() for each macro.
 *      int RP_define_restore_list(char **list)     Restores a list of macros using RP_define_restore() for each macro.
 */
#include "api/b_args.h"
#include "api/b_errors.h"
#include "api/objs/objs.h"

#include "api/report/engine/engine.h"


KDB     *RP_MACRO = NULL;   // KDB containing the macros during a report execution

/**
 *  Creates the KDB RP_MACRO if it does not exist.
 *  
 *  @return int     -3 if K_Create() fails
 *                  0 on success
 */
int RP_macro_createdb()
{
    if(RP_MACRO) return(0);
    RP_MACRO = K_create(K_OBJ, ASIS_CASE);
    if(RP_MACRO == NULL) {
        B_seterrn(250);
        return(-3);
    }
    return(0);
}


/**
 *  Deletes the KDB RP_MACRO and its content.
 *  
 *  @return int     0 always
 */
int RP_macro_deletedb()
{
    if(RP_MACRO) return(0);
    K_free(RP_MACRO);
    RP_MACRO = 0;
    return(0);
}


/**
 *  Adds or replaces a macro to RP_MACRO.
 *  
 *  @param [in] char*   name    macro name
 *  @param [in] char*   macro   macro content
 *  @return     int             -3 if RP_MACRO cannot be created
 *                              -1 if K_add() fails
 *                              0 on success
 */
int RP_define_1(char *name, char *macro)
{
    int     rc, lg;

    rc = RP_macro_createdb();
    if(rc) return(rc);

    if(macro == 0) macro = "";
    lg = (int)strlen(macro) + 1;
    if(K_add(RP_MACRO, name, macro, &lg) < 0) {
        B_seterrn(251, name, macro);
        return(-1);
    }
    return(0);
}


/**
 *  Report function to define a new macro.
 *  
 *  Syntax: $define macro_name [macro_value]
 *  
 *  @param [in] char* arg   $define parameters
 *  @return                 see RP_define_1()
 */
int RP_define(char* arg)
{
    int     lg;
    ONAME   name;
    char    *macro;

    lg = B_get_arg0(name, arg, K_MAX_NAME + 1);
    macro = arg + lg + 1;
    U_ljust_text(macro);
    SCR_strip(macro);

    return(RP_define_1(name, macro));
}

/**
 *  Returns the pointer to a macro (aka define) value.
 *  
 *  @param [in] char*   macro_name  name of the macro to find
 *  @return     char*               pointer to the macro value (not allocated here)
 *                                  NULL if macro_name does not exist
 */
char* RP_get_macro_ptr(char* macro_name)
{
    int     pos;
    
    pos = K_find(RP_MACRO, macro_name);
    if(pos < 0) return(NULL);
   
    return(KOVAL(RP_MACRO, pos));
}
    
/**
 *  Deletes one macro.
 *  
 *  @param [in] char*   name    macro to delete
 *  @return     int             always 0    
 */
int RP_undef_1(char *name)
{
    if(RP_MACRO == 0) return(0);
    K_del(RP_MACRO, K_find(RP_MACRO, name));
    return(0);
}


/**
 *  Report function to delete macros.
 *  
 *  Syntax: $undef macro_name1 macro_name2...
 *  
 *  @param [in] char* arg   list of macro names
 *  @return                 always 0
 */
int RP_undef(char *arg)
{
    //ONAME   name;

    return(B_ainit_loop(arg, RP_undef_1, (char *)0));
    //B_get_arg0(name, arg, K_MAX_NAME + 1);
    //RP_undef_1(name);

    //return(0);
}

/** 
 *  $foreach and $procdef temporary macros
 *  -------------------------------------- 
 *  In $foreach and $procdef constructs, some macros must be temporary saved (pushed) 
 *  and later restored (popped). 
 *  
 *  The mechanism implemented to achieved that aim is to save a copy of the macro
 *  to push (save) under a special name constructed by appending "#<depth>"
 *  to the macro name. It's a way to create a stack of "local" macros.
 *  
 *  For example the macro "i" (that will be used as %i%) will be saved (pushed) under the name of
 *  "i#1" if it does not yet exist, "i#2" if "i#1" already exists, etc.
 */

/**
 *  Returns the max depth of a saved (pushed) macro.
 *  
 *  E.g.: mymacro
 *      if mymacro#2 exists and 2 is the max level    => 2
 *      if mymacro   exists but no mymacro#<n> exists => -1
 *  
 *  @param [in] char*   name    macro name (without #<n>)
 *  @return     int             max <n> in macros name#<n>
 */
int RP_define_calcdepth(char *name)
{
    int     i, lg, maxdepth = -1, depth, objpos = -1;
    char    buf[1024];

    // Try to find object name#*
    sprintf(buf, "%s%c", name, K_SECRETSEP);
    lg = (int)strlen(buf);

    for(i = 0; i < KNB(RP_MACRO); i++) {
        if(strncmp(buf, KONAME(RP_MACRO, i), lg) == 0) {
            objpos = i;
            depth = atoi(KONAME(RP_MACRO, i) + lg);
            if(depth > maxdepth) maxdepth = depth; // cas avec depth > 9
        }
    }

    return(maxdepth);
}


/**
 *  Saves (pushes) a macro under the name "name#<depth+1>".
 *  
 *  @param [in] char*   name    macro name (w/o suffix)
 *  @return     int             -3 if RP_MACRO cannot be created
 *                              -1 if RP_define_1() fails
 *                              0 on success
 */
int RP_define_save(char *name)
{
    int     pos, rc, maxdepth;
    char    buf[1024];

    // Create the macro KDB if needed
    rc = RP_macro_createdb();
    if(rc) return(rc);

    // if the macro "name" does not yet exist, no need to push its definition
    pos = K_find(RP_MACRO, name);
    if(pos < 0) return(0);

    // Try to find object name#*
    maxdepth = RP_define_calcdepth(name);

    // Create a copy of existing name in name#(maxdepth+1)
    sprintf(buf, "%s%c%d", name, K_SECRETSEP, maxdepth + 1);
    rc = RP_define_1(buf, KOVAL(RP_MACRO, pos));

    return(rc);
}


/**
 *  Deletes the macro "name" and restores (pops) the macro "name#<depth>" under the name "name". 
 *  If there is no macro "name#depth", the macro name is simply deleted.
 *  The macro "name#depth" is also deleted.
 *  
 *  @param [in] char*   name    macro name (w/o suffix)
 *  @return     int             -3 if RP_MACRO cannot be created
 *                              -1 if RP_define_1() fails
 *                              0 on success
 */
int RP_define_restore(char *name)
{
    int     pos, rc, maxdepth;
    char    buf[1024];

    // Create the macro KDB if needed
    rc = RP_macro_createdb();
    if(rc) return(rc);

    // Undefine the current
    RP_undef_1(name);

    // Try to find object name#* - Nothing to do if not found
    maxdepth = RP_define_calcdepth(name);
    if(maxdepth < 0) return(0);

    // Restore the copy of existing name in name#(maxdepth+1)
    sprintf(buf, "%s%c%d", name, K_SECRETSEP, maxdepth);
    pos = K_find(RP_MACRO, buf);
    rc = RP_define_1(name, KOVAL(RP_MACRO, pos));

    // Delete the copy
    RP_undef_1(buf);

    return(rc);
}


/**
 *  Saves (pushes) a list of macros using RP_define_save() for each macro.
 *  
 *  @param [in] char**  list    list of macro names
 *  @return     int             -3 if RP_MACRO cannot be created
 *                              -1 if RP_define_1() fails
 *                              0 on success
 */
int RP_define_save_list(char **list)
{
    int i, rc;

    if(list == 0) return(0);
    for(i = 0 ; list[i] ; i++) {
        rc = RP_define_save(list[i]);
        if(rc) return(rc);
    }
    return(0);
}


/**
 *  Restores a list of macros using RP_define_restore() for each macro.
 *  
 *  @param [in] char**  list    list of macro names
 *  @return     int             -3 if RP_MACRO cannot be created
 *                              -1 if RP_define_1() fails
 *                              0 on success
 */
int RP_define_restore_list(char **list)
{
    int i, rc;

    if(list == 0) return(0);
    for(i = 0 ; list[i] ; i++) {
        rc = RP_define_restore(list[i]);
        if(rc) return(rc);
    }
    return(0);
}