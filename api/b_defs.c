#include "iode.h"

/***********************************************************************
Defines
-------
RP_define(char *arg) : "$define name value" -> add or replace name in KDB RP_MACRO
RP_define_1(char *name, char *value)
RP_undef(arg) : "$undef name"
RP_undef_1(char *name)
RP_define_save(name)
RP_define_restore(name)
RP_define_save_list(char **list)
RP_define_restore_list(char **list)
RP_macro_createdb()
RP_macro_deletedb()

 **********************************************************************/

KDB     *RP_MACRO = NULL;

RP_macro_createdb()
{
    if(RP_MACRO) return(0);
    RP_MACRO = K_create(K_OBJ, K_ASIS);
    if(RP_MACRO == NULL) {
        B_seterrn(250);
        return(-3);
    }
    return(0);
}

RP_macro_deletedb()
{
    if(RP_MACRO) return(0);
    K_free(RP_MACRO);
    RP_MACRO = 0;
    return(0);
}


RP_define_1(char *name, char *macro)
{
    int     rc, lg;

    rc = RP_macro_createdb();
    if(rc) return(rc);

    if(macro == 0) macro = "";
    lg = strlen(macro) + 1;
    if(K_add(RP_MACRO, name, macro, &lg) < 0) {
        B_seterrn(251, name, macro);
        return(-1);
    }
    return(0);
}

RP_define(arg)
char *arg;
{
    int     lg, rc;
    ONAME   name;
    char    *macro;

    lg = B_get_arg0(name, arg, K_MAX_NAME + 1);
    macro = arg + lg + 1;
    U_ljust_text(macro);
    SCR_strip(macro);

    return(RP_define_1(name, macro));
}

// Delete a define
RP_undef_1(char *name)
{
    if(RP_MACRO == 0) return(0);
    K_del(RP_MACRO, K_find(RP_MACRO, name));
    return(0);
}

RP_undef(char *arg)
{
    ONAME   name;

    B_get_arg0(name, arg, K_MAX_NAME + 1);
    RP_undef_1(name);

    return(0);
}

// Try to find the depth of the definition of a macro
RP_define_calcdepth(char *name)
{
    int     i, lg, maxdepth = -1, depth, objpos = -1;
    char    buf[1024];

    // Try to find object name#*
    sprintf(buf, "%s%c", name, K_SECRETSEP);
    lg = strlen(buf);

    for(i = 0; i < KNB(RP_MACRO); i++) {
        if(strncmp(buf, KONAME(RP_MACRO, i), lg) == 0) {
            objpos = i;
            depth = atoi(KONAME(RP_MACRO, i) + lg);
            if(depth > maxdepth) maxdepth = depth; // cas avec depth > 9
        }
    }

    return(maxdepth);
}

// Save a macro (under a tmp name) if the same name is already in use
RP_define_save(char *name)
{
    int     pos, rc, maxdepth;
    char    buf[1024];

    // Create the macro KDB if needed
    rc = RP_macro_createdb();
    if(RP_macro_createdb()) return(rc);

    // if the macro "name" does not yet exist, nothing to do
    pos = K_find(RP_MACRO, name);
    if(pos < 0) return(0);

    // Try to find object name#*
    maxdepth = RP_define_calcdepth(name);

    // Create a copy of existing name in name#(maxdepth+1)
    sprintf(buf, "%s%c%d", name, K_SECRETSEP, maxdepth + 1);
    rc = RP_define_1(buf, KOVAL(RP_MACRO, pos));

    return(rc);
}

RP_define_restore(char *name)
{
    int     pos, rc, maxdepth;
    char    buf[1024];

    // Create the macro KDB if needed
    rc = RP_macro_createdb();
    if(RP_macro_createdb()) return(rc);

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

// Save a list of macros
RP_define_save_list(char **list)
{
    int i, rc;

    if(list == 0) return(0);
    for(i = 0 ; list[i] ; i++) {
        rc = RP_define_save(list[i]);
        if(rc) return(rc);
    }
    return(0);
}

// Restore a list of macros
RP_define_restore_list(char **list)
{
    int i, rc;

    if(list == 0) return(0);
    for(i = 0 ; list[i] ; i++) {
        rc = RP_define_restore(list[i]);
        if(rc) return(rc);
    }
    return(0);
}

/***************************** END DEFINES **********************************/
