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
 *      int RP_define_1(char *name, char *macro)    Adds or replaces a macro to RP_MACRO.
 *      int RP_define(char* arg, int unused)        Report function to define a new macro.
 *      std::string RP_get_macro(const std::string& name)    Returns the pointer to a macro (aka define) value.    
 *      int RP_undef_1(char *name)                  Deletes one macro.
 *      int RP_undef(char *arg, int unused)         Report function to delete macros.
 *      int RP_define_calcdepth(char *name)         Returns the max depth of a saved (pushed) macro.
 *      int RP_define_save(char *name)              Saves (pushes) a macro under the name "name#<depth+1>".
 *      int RP_define_restore(char *name)           Deletes the macro "name" and restores (pops) the macro "name#<depth>" under the name "name".
 *      int RP_define_save_list(char **list)        Saves (pushes) a list of macros using RP_define_save() for each macro.
 *      int RP_define_restore_list(char **list)     Restores a list of macros using RP_define_restore() for each macro.
 */
#include "api/pch.h"
#include "api/b_args.h"
#include "api/b_errors.h"
#include "api/objs/objs.h"
#include "api/report/engine/engine.h"


/**
 *  Adds or replaces a macro to RP_MACRO.
 *  
 *  @param [in] char*   name    macro name
 *  @param [in] char*   macro   macro content
 *  @return     int             -3 if RP_MACRO cannot be created
 *                              -1 if add() fails
 *                              0 on success
 */
int RP_define_1(char* c_name, char* c_macro)
{
    if(c_macro == 0) 
        c_macro = "";
    
    std::string macro(c_macro);
    bool success = RP_MACRO->set_macro(c_name, macro);
    if(!success) 
    {
        std::string error_msg = "Report: Define of " + std::string(c_name);
        error_msg += " (" + macro + ") not possible";
        error_manager.append_error(error_msg);
        return -1;
    }

    return 0;
}


/**
 *  Report function to define a new macro.
 *  
 *  Syntax: $define macro_name [macro_value]
 *  
 *  @param [in] char* arg   $define parameters
 *  @return                 see RP_define_1()
 */
int RP_define(char* arg, int unused)
{
    int     lg;
    ONAME   name;
    char    *macro;

    lg = B_get_arg0(name, arg, K_MAX_NAME + 1);
    macro = arg + lg + 1;
    U_ljust_text((unsigned char*) macro);
    SCR_strip((unsigned char*) macro);

    return(RP_define_1(name, macro));
}

/**
 * @brief Checks if a macro with the given name exists.
 * 
 * @param name 
 * @return true 
 * @return false 
 */
bool RP_macro_exists(const std::string& name)
{
    return RP_MACRO->contains(name);
}

/**
 *  Returns the pointer to a macro (aka define) value.
 *  
 *  @param [in] char*   macro_name  name of the macro to find
 *  @return     char*               pointer to the macro value (not allocated here)
 *                                  NULL if macro_name does not exist
 */
std::string RP_get_macro(const std::string& name)
{
    if(!RP_MACRO->contains(name))
    {
        std::string warning_msg = "Report: Macro '" + name + "' is not defined";
        kwarning(warning_msg.c_str());
        return "";
    }
   
    return RP_MACRO->get_macro(name);
}

/**
 *  Deletes one macro.
 *  
 *  @param [in] char*   name    macro to delete
 *  @return     int             always 0    
 */
int RP_undef_1(char *name)
{
    if(!RP_MACRO->contains(name)) 
        return 0;
    
    bool success = RP_MACRO->remove(name);
    return (int) success;
}

int wrapper_RP_undef_1(char *name, void* unused)
{
    return RP_undef_1(name);
}

/**
 *  Report function to delete macros.
 *  
 *  Syntax: $undef macro_name1 macro_name2...
 *  
 *  @param [in] char* arg   list of macro names
 *  @return                 always 0
 */
int RP_undef(char *arg, int unused)
{
    return B_ainit_loop(arg, wrapper_RP_undef_1, (char*) 0);
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
int RP_define_calcdepth(char* c_name)
{    
    // add the secret separator to the name to find all macros with this prefix
    std::string prefix = std::string(c_name) + K_SECRETSEP;
    int lg = (int) prefix.length();

    int maxdepth = -1, depth;
    std::string name_without_depth;
    for(const auto& [macro_name, _] : RP_MACRO->k_objs) 
    {
        if(macro_name.length() <= lg) 
            continue;

        if(macro_name.substr(0, lg) != prefix) 
            continue;
        
        // convert the rest of the macro name to an integer
        depth = std::stoi(macro_name.substr(lg));
        if(depth > maxdepth) 
            maxdepth = depth;
    }

    return maxdepth;
}


/**
 *  Saves (pushes) a macro under the name "name#<depth+1>".
 *  
 *  @param [in] char*   name    macro name (w/o suffix)
 *  @return     int             -3 if RP_MACRO cannot be created
 *                              -1 if RP_define_1() fails
 *                              0 on success
 */
int RP_define_save(char* c_name)
{
    std::string name(c_name);

    // if the macro "name" does not yet exist, no need to push its definition
    if(!RP_MACRO->contains(name)) 
        return 0;

    // Try to find object name#*
    int maxdepth = RP_define_calcdepth(c_name);

    // Create a copy of existing name in name#(maxdepth+1)
    std::string macro = RP_MACRO->get_macro(name);
    std::string fullname = name + K_SECRETSEP + std::to_string(maxdepth + 1);
    int rc = RP_define_1((char*) fullname.c_str(), (char*) macro.c_str());
    return rc;
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
int RP_define_restore(char* c_name)
{
    // Undefine the current
    RP_undef_1(c_name);

    // Try to find object name#* - Nothing to do if not found
    int maxdepth = RP_define_calcdepth(c_name);
    if(maxdepth < 0) 
        return 0;

    // Restore 'name' from the copy of 'name#maxdepth' 
    std::string fullname = std::string(c_name) + K_SECRETSEP + std::to_string(maxdepth);
    std::string macro = RP_MACRO->get_macro(fullname);
    int rc = RP_define_1(c_name, (char*) macro.c_str());

    // Delete the copy 'name#maxdepth' 
    RP_undef_1((char*) fullname.c_str());

    return rc;
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

    if(list == 0) return 0;
    for(i = 0 ; list[i] ; i++) {
        rc = RP_define_save(list[i]);
        if(rc) return rc;
    }
    return 0;
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

    if(list == 0) return 0;
    for(i = 0 ; list[i] ; i++) {
        rc = RP_define_restore(list[i]);
        if(rc) return rc;
    }
    return 0;
}