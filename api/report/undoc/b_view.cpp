/**
 *  @header4iode
 *  
 *  Functions to display or print calculated tables and tables of variables. The same functions are used
 *  to print and to display tables as graphs or as text.
 *  
 *  The functions generate IODE tables in A2M format based on TBL structures and GSample definition.
 *  
 *  List of functions 
 *  -----------------
 *      int B_ViewVar(char* arg, int unused)                            | Display a list of variables in the form of tables of max 50 variables.
 *      int B_PrintVar(char* arg, int unused)                           | Print a list of variables in the form of tables of max 50 variables.
 *      int B_ViewPrintVar(char* arg, int mode)             | Print or display (according to the mode parameter) variables in the form of tables. 
 *      int B_ViewByTbl(char* arg, int unused)                          | $ViewTbl sample table [list of tables]
 *      int B_ViewTbl(char* arg, int unused)                            | Alias of B_ViewByTbl()
 *      int B_PrintTbl(char* arg, int unused)                           | $PrintTbl gsample table1 [table2...]
 *      int B_ViewGr(char* arg, int unused)                             | $ViewGr gsample tbl1[+tbl2] tbl3 ... 
 *      int B_PrintGr(char* arg, int unused)                            | $PrintGr gsample table1 [table2...]
 *      int B_ViewPrintTbl_1(char* name, char* smpl)        | Calculate and display (or print according to the value of B_viewmode) a table on a specified GSample.
 *      int B_ViewPrintGr_1(char* names, char* gsmpl)       | Calculate and display (or print according to the value of B_viewmode) a graph on a specified GSample, based on TBL definition(s).
 *      int B_ViewPrintTbl(char* arg, int type, int mode)   | Calculate, then print or display (according to the mode parameter) IODE TBLs either in the form of graphs or in the form of text (SCROLLs).
 *      int B_ViewTblFile(char* arg, int unused)                        | $PrintTblFile n varfilename    (n := 2, 3, 4, 5)
 *      int B_ViewTblEnd()                                  | Close a Print tables or Print variables session.
 */
#include "scr4/s_args.h"

#include "api/constants.h"
#include "api/b_args.h"
#include "api/b_errors.h"
#include "api/objs/objs.h"
#include "api/objs/pack.h"
#include "api/objs/grep.h"
#include "api/print/print.h"
#include "api/write/write.h"

#include "api/report/engine/engine.h"
#include "api/report/undoc/undoc.h"


int B_viewmode;         // 0: displays the graph/table on screen, 1: print graph/table

int     ODE_VIEW = 0;   // Used in the DOS GUI to differentiate between display & print
char    ODE_SMPL[81];   // Current GSample for printing / displaying tables and graphs


// Display a list of variables in the form of tables of max 50 variables.
int B_ViewVar(char* arg, int unused)
{
    return(B_ViewPrintVar(arg, 0));
}

// Print a list of variables in the form of tables of max 50 variables.
int B_PrintVar(char* arg, int unused)
{
    return(B_ViewPrintVar(arg, 1));
}


/**
 *  Print or display (according to the mode parameter) variables in the form of tables. 
 *  Temporary tables of maximum 50 variables are constructed then printed of displayed.
 *  
 *  @param [in] arg  char*  sample + list of variables to be printed
 *  @param [in] mode int    0 to display, 1 to print
 *  @return          int 
 */
int B_ViewPrintVar(char* arg, int mode)
{
    int     rc = 0, nb, i;
    char    *smpl, *ptr;
    U_ch    **args;
    TBL     *tbl;
    char    *oldseps = A_SEPS, *lst;  /* JMP 14-07-96 */

    if(arg == 0 || arg[0] == 0) {
        error_manager.append_error("Invalid argument");
        return(-1);
    }
    args = SCR_vtom((unsigned char*) arg, (int) ' ');
    if(args == NULL) {
        error_manager.append_error("Invalid argument");
        return(-1);
    }

    smpl = (char*) SCR_stracpy(args[0]);
    SCR_free_tbl(args);

    A_SEPS = ";\t\n"; /* JMP 24-12-98 */
    lst = K_expand(VARIABLES, NULL, arg + strlen(smpl) + 1, '*');
    args = (unsigned char**) B_ainit_chk(lst, NULL, 0);
    SCR_free(lst);
    A_SEPS = oldseps;    /* JMP 14-07-96 */
    if(args == 0 || args[0] == 0) {
        error_manager.append_error("Invalid argument");
        rc = -1;
        goto fin;
    }
    nb = SCR_tbl_size(args);
    for(i = 0 ; i < nb ; i += 50) {
        if(i + 50 < nb) {
            ptr = (char*) args[i + 50];
            args[i + 50] = 0;
        }
        tbl = T_create(2);
        if(mode == 0) {
            T_default(tbl, 0L, ((char**) args) + i, ((char**) args) + i, 0, 0, 0);
            rc = T_view_tbl(tbl, smpl, "TABLE_OF_VARIABLES");
        }
        else {
            T_default(tbl, 0L, ((char**) args) + i, ((char**) args) + i, 1, 1, 1);
            rc = T_print_tbl(tbl, smpl);
        }
        T_free(tbl);
        if(i + 50 < nb) args[i + 50] = (unsigned char*) ptr;
        if(rc) break;
    }

fin:
    SCR_free(smpl);
    SCR_free_tbl(args);

    /*    W_close(); */
    W_flush();
    B_ViewTblEnd();
    return(rc);
}


// Display a list of tables
// $ViewTbl sample table [list of tables]
int B_ViewByTbl(char* arg, int unused)
{
    return(B_ViewPrintTbl(arg, 0, 0));
}

// Alias of B_ViewByTbl()
int B_ViewTbl(char* arg, int unused)
{
    return(B_ViewPrintTbl(arg, 0, 0));
}

// Print a list of tables
// $PrintTbl gsample table1 [table2...]
int B_PrintTbl(char* arg, int unused)
{
    return(B_ViewPrintTbl(arg, 0, 1));
}


// Display a list of tables as graphs
// $ViewGr gsample tbl1[+tbl2] tbl3 ... 
int B_ViewGr(char* arg, int unused)
{
    return(B_ViewPrintTbl(arg, 1, 0));
}

// Print a list of tables as graphs
// $PrintGr gsample table1 [table2...]
int B_PrintGr(char* arg, int unused)
{
    return(B_ViewPrintTbl(arg, 1, 1));
}


/**
 *  Calculate and display (or print, according to the value of B_viewmode) a table on a specified GSample.
 *  
 *  @param [in] name char*  table name
 *  @param [in] smpl char*  GSample
 *  @return          int    0 on success, -1 on error (table not found, illegal gsample...)
 */
int B_ViewPrintTbl_1(char* name, char* smpl)
{
    int rc, pos;
    TBL *tbl;

    pos = K_find(K_WS[TABLES], name);
    if(pos < 0) {
        error_manager.append_error("Table '" + std::string(name) + "' not found");
        return(-1);
    }

    tbl = KTVAL(K_WS[TABLES], pos);
    if(B_viewmode == 0)
        rc = T_view_tbl(tbl, smpl, name);
    else
        rc = T_print_tbl(tbl, smpl);

    if(rc < 0) 
        error_manager.append_error("Table '" + std::string(name) + "' not printed");

    T_free(tbl);
    return(rc);
}


/**
 *  Calculate and display (or print according to the value of B_viewmode) a graph on a specified GSample, 
 *  based on TBL definition(s).
 *  
 *  @param [in] names   char*  list of table names
 *  @param [in] smpl    char*  GSample
 *  @return             int    0 on success, -1 on error (table not found, illegal gsample...)
 */
int B_ViewPrintGr_1(char* names, char* gsmpl)
{
    int     rc = 0, pos, hg, ng, i, view = !B_viewmode;
    TBL     *tbl;
    char    **tbls;

    tbls = (char**) SCR_vtoms((unsigned char*) names, (unsigned char*) "+-");
    ng = SCR_tbl_size((unsigned char**) tbls);
    if(ng == 0) {
        error_manager.append_error("No tables defined");
        return(-1);
    }

    for(i = 0; i < ng; i ++) {
        if(view) W_InitDisplay();

        pos = K_find(K_WS[TABLES], tbls[i]);
        if(pos < 0) {
            error_manager.append_error("Table '" + std::string(tbls[i]) + "' not found");
            rc = -1;
            break;
        }
        tbl = KTVAL(K_WS[TABLES], pos);
        //KT_nb++;
        hg = T_graph_tbl_1(tbl, gsmpl, B_viewmode);

        if(view) W_EndDisplay((char*) T_get_title(tbl), -ng, -i, -1, -1);

        T_free(tbl);
        if(hg < 0) {
            rc = -1;
            break;
        }
    }

    SCR_free_tbl((unsigned char**) tbls);
    return(rc);
}


int wrapper_B_ViewPrintGr_1(char* names, void* gsmpl)
{
    return B_ViewPrintTbl_1(names, (char*) gsmpl);
}

int wrapper_B_ViewPrintTbl_1(char* names, void* gsmpl)
{
    return B_ViewPrintTbl_1(names, (char*) gsmpl);
}


/**
 *  Calculate, then print or display (according to the mode parameter) IODE TBLs either 
 *  in the form of graphs or in the form of text (SCROLLs).
 *  
 *  @param [in] char* arg   sample + list of tables to print  
 *  @param [in] int   type  0: print or view in table format
 *                          1: print in graph format 
 *  @param [in] int   mode  0: view
 *                          1: print
 *  @return     int         0 on success-1 on error (no arg, error in execution of tables) 
 */
int B_ViewPrintTbl(char* arg, int type, int mode)
{
    int     rc = 0;
    char    *smpl;
    U_ch    **args;

    B_viewmode = mode;
    if(arg == 0 || arg[0] == 0) {
        error_manager.append_error("Invalid argument");
        return(-1);
    }
    else {
        args = SCR_vtom((unsigned char*) arg, ' ');
        if(args == NULL || args[0] == NULL) {
            error_manager.append_error("Invalid argument");
            return(-1);
        }

        smpl = (char*) SCR_stracpy(args[0]);

        if(mode == 1 || SCR_tbl_size(args) < 3) {
            if(type == 0)
                rc = B_ainit_loop(arg + strlen(smpl) + 1, wrapper_B_ViewPrintTbl_1, smpl);
            else
                rc = B_ainit_loop(arg + strlen(smpl) + 1, wrapper_B_ViewPrintGr_1, smpl);
        }
        else {
            if(type == 0) ODE_VIEW = 1;
            else ODE_VIEW = 2;
            //strcpy(ODE_SMPL, smpl);
            SCR_strlcpy((unsigned char*) ODE_SMPL, (unsigned char*) smpl, sizeof(ODE_SMPL) - 1); // JMP 10/04/2023
            ODE_scroll(K_WS[TABLES], ((char**) args) + 1);
        }

        SCR_free_tbl(args);
    }

    SCR_free(smpl);

    /*    W_close();*/
    W_flush();
    B_ViewTblEnd();
    return(rc);
}


// Report function:
//      $PrintTblFile n varfilename    (n := 2, 3, 4, 5)
int B_ViewTblFile(char* arg, int unused)
{
    int     ref, rc = 0;
    KDB     *kdb = NULL;
    U_ch    **args = NULL;

    if(arg == 0 || arg[0] == 0) {
        error_manager.append_error("Invalid argument");
        return(-1);
    }
    else {
        args = SCR_vtom((unsigned char*) arg, ' ');
        if(args == NULL)  {
            error_manager.append_error("Invalid argument");
            rc = -1;
            goto err;
        }

        if(SCR_tbl_size(args) < 2) goto err;

        ref = atoi((char*) args[0]);
        if(ref < 2 || ref > 5) {
            error_manager.append_error("Invalid Reference number");
            rc = -1;
            goto err;
        }

        kdb = K_interpret(VARIABLES, (char*) args[1]);
        if(kdb == NULL) {
            rc = -1;
            goto err;
        }

        K_free(K_RWS[VARIABLES][ref - 1]);
        K_RWS[VARIABLES][ref - 1] = kdb;
    }

err:
    SCR_free_tbl(args);
    return(rc);
}


// Close a Print tables or Print variables session.
int B_ViewTblEnd()
{
    int     i;

    for(i = 1; i < 5; i++) {
        K_free(K_RWS[VARIABLES][i]);
        K_RWS[VARIABLES][i] = NULL;
    }
    ODE_VIEW = 0;
    ODE_SMPL[0] = '\0';
    return(0);
}
