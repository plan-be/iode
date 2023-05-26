/**
 * @header4iode
 *
 * Functions to print IODE object definitions.
 *
 *  List of functions 
 *  -----------------
 *    int B_PrintVal(IODE_REAL val)                                    | Print a double with the function T_print_val() and with the number of decimals set to -1
 *    IODE_REAL B_calc_ttest(SCL* scl)                                 | Return the t-test of a scalar or L_NAN if it cannot be determined.
 *    int B_replesc(unsigned char* out, unsigned char* in)             | Replace \ by / in a string
 *    int B_PrintDefGnl(char* name, char* text)                        | Print an object name and its title in an enum_1 paragraph.
 *    int B_isdef(char* txt)                                           | Checks if a string contains non space charaters.
 *    int B_dump_str(unsigned char*head, unsigned char*txt)            | Print a header and a modified text: spaces are added before and after specific characters in the text
 *    int B_get1int(char* arg)                                         | Return the integer value of the beginning of a string.
 *    int B_ScrollSet(char* arg, long *plong, int inf, int sup)        | Interprets the first part of a string as a integer and check that the value is between 2 boundaries.
 *    int B_PrintObjTblTitle(char* arg)                                | $PrintObjTitle 0 or 1
 *    int B_PrintObjLec(char* arg)                                     | $PrintObjLec {0|1|2}
 *    int B_PrintObjEqsInfos(char* arg)                                | $PrintObjInfos {0|1|2}
 *    int B_PrintObjDef_1(char* arg, int* type)                        | Print the definition of the object named arg of the given type
 *    int B_PrintObjDef(char* arg, int type)                           | $PrintObjDefXxx object_list
 *    int B_PrintObjDefArgs(char* arg, int type)                       | Print a list of objects of a given type.
 *    int B_PrintDefTbl(KDB* kdb, int pos)                             | Print the table in position pos in kdb.  
 *    int B_DumpTblDef(TBL* tbl)                                       | Print a table definition.
 *    int B_CellDef(TCELL* cell)                                       | Checks that a TCELL is not empty (for TEXT cells) and not "1" (for LEC cells).
 *    int B_PrintTblCell(TCELL* cell, int straddle)                    | Print a TABLE cell optionally on several columns.
 *    int B_PrintDefCmt(KDB* kdb, int pos)                             | Print a comment.
 *    int B_PrintDefLst(KDB* kdb, int pos)                             | Print a list.
 *    int B_PrintDefIdt(KDB* kdb, int pos)                             | Print a identity.
 *    int B_PrintDefEqs(KDB* kdb, int pos)                             | Print a equation.
 *    int B_PrintLec(char* name, char* eqlec, CLEC* eqclec, int coefs) | Print a LEC expression. Set the engogenous (name) in bold.
 *    int B_PrintEqs(char* name, EQ* eq)                               | Print an equation and optionally its statistical tests.
 *    int B_PrintDefSclPtr(SCL* scl, char*name, int enum_)             | Print a scalar in an enumeration list.
 *    int B_PrintDefScl(KDB* kdb, int pos)                             | Print the scalar kdb[pos].
 *    int B_PrintDefVar(KDB* kdb, int pos)                             | Print the variable kdb[pos] in a table. Sub-function of B_PrintObjDef_1().
 *
 */

#include "iode.h"

int     B_MULTIBAR = 0; // Graph parameter (see GB)
int     B_TBL_TITLE;    // Specify how to print a TABLE 
                        //      0 : print table full definitions
                        //      1 : print only table titles
int     B_EQS_INFOS;    // Information detail to print (for equations)
                        //    0: equation only 
                        //    1: equation + comment
                        //    2: equation + comment + estimation results
int     B_EQS_LEC;      // Specify how to print a LEC expression 
                        //    0 : print the LEC form as is
                        //    1 : replace all scalars by their values
                        //    2 : replaced all scalars by their values + t-tests

/*================================= UTILITIES ===============================*/

/**
 *  Print a double with the function T_print_val() and with the number of decimals set to -1.
 *  
 *  @param [in] val double  value to print
 *  @return         int     0
 */
int B_PrintVal(IODE_REAL val)
{
    // B_NBDEC replaced by K_NBDEC JMP 18/04/2022
    int     nbdec = K_NBDEC;

    K_NBDEC = -1;
    T_print_val(val);
    K_NBDEC = nbdec;
    return(0);
}


/**
 *  Return the t-test of a scalar or L_NAN if it cannot be determined.
 *      
 *  @param [in] scl SCL*    given scalar 
 *  @return         double  value / stderr 
 */
IODE_REAL B_calc_ttest(SCL* scl)
{
    if(L_ISAN(scl->val) && L_ISAN(scl->std) && !L_IS0(scl->std))
        return(scl->val / scl->std);
    return(L_NAN);
}

/**
 *  Replace \ by / in a string.
 *  
 *  @param [out] out    unsigned char*      output string
 *  @param [in]  in     unsigned char*      input string
 *  @return             int                 0
 *  
 *  TODO: replace by SCR_replace()
 */
int B_replesc(unsigned char* out, unsigned char* in)
{
    int     i;

    if(in == 0 || out == 0) return(0); /* JMP 29-09-2015 */
    for(i = 0 ; in[i] ; i++) {
        out[i] = in[i];
        if(out[i] == '\\') out[i] = '/';
    }
    out[i] = 0;
    return(0);
}


/**
 *  Print an object name and its title in an enum_1 paragraph.
 *          
 *  @param [in] name    char*   object name
 *  @param [in] text    char*   object title of definition    
 *  @return             int     0
 */
int B_PrintDefGnl(char* name, char* text)
{
    char    buf[80];

    //sprintf(buf, ".par1 enum_1\n\\b%s\\B : ", name);
    sprintf(buf, ".par1 enum_1\n%cb%s%cB : ", A2M_ESCCH, name, A2M_ESCCH);  // JMP 10/04/2023
    B_dump_str(buf, text);
    return(0);
}


/**
 *  Checks if a string contains non space charaters.
 *  
 *  @param [in] txt char*   string to check
 *  @return         int     0 if txt is NULL or contains only spaces, 1 otherwise        
 */
int B_isdef(char* txt)
{
    if(txt == NULL || txt[0] == 0) return(0);
    for(; *txt ; txt++) if(!isspace(*txt)) return(1);
    return(0);
}


/**
 *  Print a header and a modified text: spaces are added before and after specific characters in the text.
 *  If the text is NULL, print only a \n.
 *  
 *  A space is added before each '+' ':' and ']'.
 *  A space is added after  each '=', '+', ',', ')' '\n'.
 *  
 *  @param [in] head char *     header
 *  @param [in] txt  char *     text 
 *  @return          int        0
 */
int B_dump_str(unsigned char*head, unsigned char*txt)
{
    int     i;

    if(txt) {
        W_printf("\n%s", head);
        for(i = 0 ; txt[i] ; i++) {
            if(U_is_in(txt[i], "+:]")) W_putc(' ');
            W_putc(txt[i]);
            /*          if(txt[i] == '\\') W_putc('\\'); */
            if(U_is_in(txt[i], "=+,)\n")) W_putc(' ');
        }
    }
    W_printf("\n");
    return(0);
}

/* ============================ PRINT ==================================== */

/**
 *  Return the integer value of the beginning of a string.
 *  
 *  @param [in] arg char*   string beginning with a number
 *  @return         int     value of the first number in the string         
 *                          -100 if arg is empty
 *  
 *  TODO: rewrite this!
 *  
 */
int B_get1int(char* arg)
{
    char    **args;
    int     n;

    args = B_ainit_chk(arg, 0L, 1);
    if(args == 0) return(-100);
    n = atoi(args[0]);
    SCR_free_tbl(args);
    return(n);
}



/**
 *  Interprets the first part of a string as a integer and check that the value 
 *  is between 2 boundaries.
 *  
 *  @note Moved from sb_global.c
 *  
 *  @param [in]  arg   char*    string beginning with an integer 
 *  @param [out] plong long*    pointer to the calculated value
 *  @param [in]  inf   int      min value
 *  @param [in]  sup   int      max value
 *  @return            int      0 on success, -1 if arg is NULL
 */
int B_ScrollSet(char* arg, long *plong, int inf, int sup)
{
    int     n;

    n = B_get1int(arg);
    if(n <= -100) return(-1);
    n = max(inf, n);
    n = min(sup, n);
    *plong = n;
    return(0);
}


// $PrintObjTitle 0 or 1
//      0 : print table full definitions
//      1 : print only table titles
int B_PrintObjTblTitle(char* arg)
{
    long    l;
    int     rc;

    rc  = B_ScrollSet(arg, &l, 0, 2);
    if(rc) return(rc);
    B_TBL_TITLE = l;
    return(0);
}


// $PrintObjLec {0|1|2}
//     0 : print the LEC equation
//     1 : replace all scalars by their values
//     2 : replaced all scalars by their values + t-tests
int B_PrintObjLec(char* arg)
{
    long    l;
    int     rc;

    rc  = B_ScrollSet(arg, &l, 0, 2);
    if(rc) return(rc);
    B_EQS_LEC = l;
    return(0);
}


// $PrintObjInfos {0|1|2}
//    0: equation only 
//    1: equation + comment
//    2: equation + comment + estimation results
int B_PrintObjEqsInfos(char* arg)
{
    long    l;
    int     rc;

    rc  = B_ScrollSet(arg, &l, 0, 2);
    if(rc) return(rc);
    B_EQS_INFOS = l;
    return(0);
}

static int     BEG = 0;     // Nb of the current printed variable (to limit each group of variable to 47)


/**
 *  Print the definition of the object named arg of the given type.
 *  
 *  If the object does not exist or type is llegal, a message is stored in the error table via B_seterror()
 *  
 *  @param [in] arg     char*   object name
 *  @param [in] type    int     object type
 *  @return             int     0 on success, -1 on error
 */
int B_PrintObjDef_1(char* arg, int* type)
{
    KDB     *kdb;
    int     pos, rc = 0;

    kdb = K_WS[*type];
    if((pos = K_find(kdb, arg)) == -1) goto err;

    kmsg("Printing %s ...", arg);
    //if(SCR_hit_key() != 0) SCR_get_key();
    if(khitkey() != 0) kgetkey();               // JMP 11/12/2021

    switch(*type) {
        case K_CMT :
            rc = B_PrintDefCmt(kdb, pos);
            W_flush();
            break;
        case K_EQS :
            rc = B_PrintDefEqs(kdb, pos);
            W_flush();
            break;
        case K_IDT :
            rc = B_PrintDefIdt(kdb, pos);
            W_flush();
            break;
        case K_LST :
            rc = B_PrintDefLst(kdb, pos);
            W_flush();
            break;
        case K_SCL :
            rc = B_PrintDefScl(kdb, pos);
            W_flush();
            break;
        case K_TBL :
            rc = B_PrintDefTbl(kdb, pos);
            W_flush();
            break;
        case K_VAR :
            if(BEG % 47 == 0) {
                SAMPLE  *smpl;
                PERIOD  *per;
                char txt[20];
                int j;

                if(BEG > 0) {
                    W_printf(".tl\n.te\n\n");  /* JMP 19-12-97 */
                    W_flush();
                }
                smpl = (SAMPLE *) KDATA(kdb);
                per = &(smpl->s_p1);
                W_printfRepl(".tb %d\n.sep &\n", smpl->s_nb + 1);
                PER_pertoa(&(smpl->s_p1), txt);
                /*                W_printfRepl("&%dLSample: %s - %s\n",smpl->s_nb + 1, txt, PER_pertoa(&(smpl->s_p2), 0L)); */
                /*                W_printfRepl("&1LSample&1C%s&1C%s\n", txt, PER_pertoa(&(smpl->s_p2), 0L)); */
                W_printfRepl(".tl\n&1LName");
                for(j = 0 ; j < smpl->s_nb; j++) {
                    PER_pertoa(PER_addper(per, j), txt);
                    W_printfRepl("&1C%s", txt);
                }
                W_printf("\n.tl\n");
            }
            rc = B_PrintDefVar(kdb, pos);
            BEG++;
            break;
        default    :
            rc = -1;
            B_seterror("Error printing object %.80s !", arg);
            break;
    }
    /*    W_flush(); /* JMP 19-12-97 */
    return(rc);
err:
    B_seterror("Object %.80s not found !", arg);
    return(-1);
}



// $PrintObjDefXxx object_list
//   where Xxx = {cmt | idt | lst | scl | tbl | var}
int B_PrintObjDef(char* arg, int type)
{
    return(B_PrintObjDefArgs(arg, type));
}


/**
 *  Print a list of objects of a given type.
 *  
 *  @param [in] arg  char*      list of objects to print. If NULL or empty, prints all objects in the WS.
 *  @param [in] type int        object type
 *  @return          int        0 on success, -1 on error: return code of B_PrintObjDef_1()   
 */
int B_PrintObjDefArgs(char* arg, int type)
{
    int         i, rc = 0;

    if(arg == 0 || arg[0] == 0)
        for(i = 0 ; i < KNB(K_WS[type]) ; i++) {
            rc = B_PrintObjDef_1(KONAME(K_WS[type], i), &type);
            if(rc) break;
        }
    else
        rc = B_ainit_loop(arg, B_PrintObjDef_1, (char *)&type);

    if(BEG) { /* JMP 17-12-93 */
        BEG = 0;
        W_printf(".te\n\n");
    }
    /*    W_close();*/
    W_flush();
    return(rc);
}


/*============================= TBL ===============================*/

/**
 *  Print the table in position pos in kdb.  
 *  
 *  If B_TBL_TITLE != 0, print the table title.
 *  If B_TBL_TITLE == 1, print the table name and its title.
 *  If B_TBL_TITLE == 0, print the table definition.
 *  
 *  @param [in] kdb KDB*        KDB of tables
 *  @param [in] pos int         position of the table in kdb
 *  @return         int         -1 if the table cannot be found in kdb
 */
int B_PrintDefTbl(KDB* kdb, int pos)
{
    TBL                     *tbl = NULL;
    extern int              B_TBL_TITLE;
    extern unsigned char    *T_get_title();

    if((tbl = KTVAL(kdb, pos)) == NULL) return(-1);
    if(B_TBL_TITLE) {
        if(B_TBL_TITLE == 1) W_printfReplEsc("\n~b%s~B : %s\n", KONAME(kdb, pos), T_get_title(tbl));
        else W_printf("\n%s\n", T_get_title(tbl));
        T_free(tbl);
        return(0);
    }
    B_PrintRtfTopic(T_get_title(tbl));
    W_printf(".tb %d\n", T_NC(tbl));
    W_printfRepl(".sep &\n");
    W_printfRepl("&%dC%cb%s : definition%cB\n", T_NC(tbl), A2M_ESCCH, KONAME(kdb, pos), A2M_ESCCH);
    B_DumpTblDef(tbl);
    W_printf(".te\n");
    T_free(tbl);
    return(0);
}


// Print a table definition.
int B_DumpTblDef(TBL* tbl)
{
    TCELL   *cell;
    int     i, j;

    W_printf("\n.tl\n");

    /* lines */
    /*    W_printfRepl("&%dL\\bTable lines\\B\n", T_NC(tbl)); */
    /*    W_printfRepl("&%dL \n", T_NC(tbl)); */
    for(j = 0; j < T_NL(tbl); j++) {
        switch(tbl->t_line[j].tl_type) {
            case KT_CELL :
                cell = (TCELL *) tbl->t_line[j].tl_val;
                for(i = 0; i < T_NC(tbl); i++)
                    B_PrintTblCell(cell + i, 1);
                W_printf("\n");
                break;

            case KT_TITLE :
                B_PrintTblCell((TCELL *) tbl->t_line[j].tl_val, T_NC(tbl));
                W_printf("\n");
                break;

            case KT_LINE  :
            case KT_DLINE  :
                /* W_printfRepl("&%dL[LINE]\n", T_NC(tbl));*/
                W_printf(".tl\n");
                break;
            case KT_MODE  :
                W_printfRepl("&%dL[MODE]\n", T_NC(tbl));
                break;
            case KT_DATE  :
                W_printfRepl("&%dL[DATE]\n", T_NC(tbl));
                break;
            case KT_FILES :
                W_printfRepl("&%dL[FILES]\n", T_NC(tbl));
                break;

            default       :
                break;
        }
    }

    /* div */
    for(i = 0; i < T_NC(tbl); i++)
        if(B_CellDef((TCELL *)(tbl->t_div.tl_val) + i)) break;
    if(i < T_NC(tbl)) {
        W_printfRepl(".tl\n&%dC%cbColumn divisors%cB\n.tl\n", T_NC(tbl), A2M_ESCCH, A2M_ESCCH); /* JMP 14-06-96 */
        for(i = 0; i < T_NC(tbl); i++)
            B_PrintTblCell((TCELL *)(tbl->t_div.tl_val) + i, 1);
    }

    W_printf("\n.tl\n");
    return(0);
}


/**
 *  Checks that a TCELL is not empty (for TEXT cells) and not "1" (for LEC cells).
 *  
 *  @param [in] cell    TCELL*  input TCELL
 *  @return             int     0 if the value is NULL or == "1" for LEC cells
 */
int B_CellDef(TCELL* cell)
{
    if((cell->tc_val) == NULL) return(0);
    if(cell->tc_type == KT_LEC &&
            strcmp("1", P_get_ptr(cell->tc_val, 0)) == 0) return(0);
    return(1);
}

/**
 *  Print a TABLE cell optionally on several columns.
 *  
 *  @param [in] cell     TCELL*     cell to print
 *  @param [in] straddle int        number of columns occupied by the cell
 *  @return              int        0
 */
int B_PrintTblCell(TCELL* cell, int straddle)
{
    if(B_CellDef(cell) == 0) {
        W_printfRepl("&%dL ", straddle);
        return(0);
    }

    switch(cell->tc_type) {
        case KT_STRING :
            T_open_cell(cell->tc_attr, straddle, KT_STRING); /* JMP 16-12-93 */
            /*      W_printf("&%d%c", straddle, ((straddle > 1) ? 'C' : 'L')); /* JMP 16-12-93 */
            T_open_attr(cell->tc_attr);
            W_printf("\"%s\"", cell->tc_val);
            break;

        case KT_LEC :
            W_printfRepl("&%dL", straddle);
            T_open_attr(cell->tc_attr);
            W_printf("%s", P_get_ptr(cell->tc_val, 0));
            break;
    }
    T_close_attr(cell->tc_attr);
    return(0);
}


/*================================= CMT ================================*/

// Print a comment.
int B_PrintDefCmt(KDB* kdb, int pos)
{
    B_PrintDefGnl(KONAME(kdb, pos), KCVAL(kdb, pos));
    return(0);
}

/*================================= LST ================================*/

// Print a list.
int B_PrintDefLst(KDB* kdb, int pos)
{
    B_PrintDefGnl(KONAME(kdb, pos), KLVAL(kdb, pos));
    return(0);
}

/*================================= IDT ================================*/

// Print an identity.
int B_PrintDefIdt(KDB* kdb, int pos)
{
    char    *name = KONAME(kdb, pos), *tmp;

    tmp = SCR_malloc(K_MAX_NAME + 10 + (int)strlen(KILEC(kdb, pos))); /* IODE64K */
    sprintf(tmp, "%s : %s", name, KILEC(kdb, pos));
    W_printf(".par1 enum_1\n");
    B_PrintLec(name, tmp, KICLEC(kdb, pos), B_EQS_LEC);
    SCR_free(tmp);
    return(0);
}

/*================================= EQS ================================*/

// Print an equation.
int B_PrintDefEqs(KDB* kdb, int pos)
{
    EQ      *eq = NULL;

    if((eq = KEVAL(kdb, pos)) == NULL) return (-1);
    B_PrintEqs(KONAME(kdb, pos), eq);
    E_free(eq);
    return(0);
}

/**
 *  Print a LEC expression. Set the engogenous (name) in bold.
 *  
 *  @param [in] name   char* name    endogenous name   
 *  @param [in] eqlec  char* eqlec   LEC expression 
 *  @param [in] eqclec CLEC* eqclec  CLEC (compiled LEC) equivalent to LEC
 *  @param [in] coefs  int   coefs   if 1: replace scalars by their value   
 *                                   if 2: replace scalars by their value and their t-test   
 *  @return            int           0
 *  
 */
int B_PrintLec(char* name, char* eqlec, CLEC* eqclec, int coefs)
{
    CLEC    *clec;
    SCL     *scl;
    char    *lec, *sname, buf[80], tcoef[128], ttest[128];
    int     j, pos, lg;

    lg = (int)strlen(eqlec);
    lg = max(512, 4 * lg);
    lec = SCR_malloc(lg);
    strcpy(lec, eqlec);
    clec = (CLEC *) SCR_malloc(eqclec->tot_lg + 1);
    memcpy(clec, eqclec, eqclec->tot_lg);

    sprintf(buf, "%cb%s%cB", A2M_ESCCH, name, A2M_ESCCH);
    SCR_replace_gnl(lec, name, buf, "_\\");
    for(j = 0 ; j < clec->nb_names ; j++) {
        sname = clec->lnames[j].name;
        buf[0] = 0;
        if(coefs && L_ISCOEF(sname)) {
            pos = K_find(KS_WS, sname);
            if(pos >= 0) {
                scl = KSVAL(KS_WS, pos);
                // T_fmt_val(tcoef, scl->val, 9, -1); /* JMP 27-10-08 */
                // T_fmt_val(ttest, B_calc_ttest(scl), 9, -1); /* JMP 27-10-08 */
                T_fmt_val(tcoef, scl->val, 15, K_NBDEC);           // JMP 18-04-2022
                T_fmt_val(ttest, B_calc_ttest(scl), 15, K_NBDEC);  // JMP 18-04-2022
                if(coefs == 1) sprintf(buf, "%ci%s%cI", A2M_ESCCH, tcoef, A2M_ESCCH);
                if(coefs == 2) sprintf(buf, "%ci%s(%s)%cI", A2M_ESCCH, tcoef, ttest, A2M_ESCCH);
            }
        }
        if(buf[0] == 0) sprintf(buf, "%ci%s%cI", A2M_ESCCH, sname, A2M_ESCCH);
        SCR_replace_gnl(lec, sname, buf, "_\\");
    }
    B_dump_str(" ", lec);

    SCR_free(lec);
    SCR_free(clec);
    return(0);
}


/**
 *  Print an equation and optionally its statistical tests.
 *  
 *  if B_EQS_INFOS == 0, print only the equation
 *  if B_EQS_INFOS == 1, print the equation and its (optional) comment
 *  if B_EQS_INFOS == 2, print the equation, its (optional) comment and the estimation 
 *                       tests (if the equation was estimated)
 *  
 *  @param [in] name  char*     equation name 
 *  @param [in] eq    EQ*       equation struct 
 *  @return           int       0    
 */
int B_PrintEqs(char* name, EQ* eq)
{
    CLEC    *clec;
    char    from[21], to[21], buf[256], *sname;
    int     j, pos;

    if(B_EQS_INFOS > 1) B_PrintRtfTopic(name);
    W_printf(".par1 enum_1\n");
    B_PrintLec(name, eq->lec, eq->clec, B_EQS_LEC);
    if(B_EQS_INFOS < 1) return(0);
    if(B_isdef(eq->cmt)) {
        sprintf(buf, ".par1 par_1\n%ci ", A2M_ESCCH);
        B_dump_str(buf, eq->cmt);
    }    

    if(B_EQS_INFOS < 2) return(0);
    if(eq->method >= 0 && eq->method < 4 &&
            (eq->smpl).s_nb != 0 && eq->tests[3]) {
        /*        W_printf(".par enum_2\nEndogenous : \\i%s\\I\n\n", name); */
        W_printf(".par enum_2\nEstimation : %ci%c%cI on %s-%s\n\n", A2M_ESCCH, "LZIG"[eq->method], A2M_ESCCH, 
                 PER_pertoa(&(eq->smpl.s_p1), from),
                 PER_pertoa(&(eq->smpl.s_p2), to));
        if(B_isdef(eq->blk))   B_dump_str("Block : ", eq->blk);
        if(B_isdef(eq->instr)) B_dump_str("Instruments : ", eq->instr);

        W_printf("\nTests :\n");
        W_printf(".par enum_3\n");
        /*        W_printf("Correlation                 : %f\n\n", eq->tests[0]); */
        W_printf("St dev of residuals         : %f\n\n", eq->tests[1]);
        W_printf("Mean of YOBS                : %f\n\n", eq->tests[2]);
        W_printf("Sum of Squares of Residuals : %f\n\n", eq->tests[3]);
        W_printf("Standard error (%%)          : %f (%ci%f%cI)\n\n", eq->tests[4], A2M_ESCCH, eq->tests[5], A2M_ESCCH);
        W_printf("F-stat                      : %f\n\n", eq->tests[6]);
        W_printf("R2 (R2 adjusted)            : %f (%ci%f%cI)\n\n", eq->tests[7], A2M_ESCCH, eq->tests[8], A2M_ESCCH);
        W_printf("Durbin-Watson               : %f\n\n", eq->tests[9]);
        W_printf("Log Likelihood              : %f\n\n", eq->tests[10]);
        W_printf(".par1 enum_2\nCoefficient values %ci(relax, stderr, t-stat)%cI :\n\n", A2M_ESCCH, A2M_ESCCH);
        clec = (CLEC *) SW_nalloc(eq->clec->tot_lg + 1);
        memcpy(clec, eq->clec, eq->clec->tot_lg);
        for(j = 0 ; j < clec->nb_names ; j++) {
            sname = clec->lnames[j].name;
            if(L_ISCOEF(sname)) {
                pos = K_find(KS_WS, sname);
                if(pos < 0)
                    B_PrintDefSclPtr(0L, sname, 3);
                else
                    B_PrintDefSclPtr(KSVAL(KS_WS, pos), sname,3);
            }
        }
        SW_nfree(clec);
    }

    W_printf("\n");
    return(0);
}


/*================================= SCL ================================*/

/**
 *  Print a scalar.
 *  
 *  @param [in] scl   SCL*      scalar struct  
 *  @param [in] name  char*     scalar name
 *  @param [in] enum_ int       enum level 
 *  @return           int       -1 is scl is NULL, 0 otherwise
 */
int B_PrintDefSclPtr(SCL* scl, char*name, int enum_)
{
    double  ttest;
    char    tcoef[128], trelax[128], tstd[128], tttest[128];

    W_printfReplEsc(".par1 enum_%d\n~b%s~B : ", enum_, name);
    if(scl == NULL) {
        W_printf("?\n");
        return(-1);
    }

    T_fmt_val(tcoef, (double)scl->val, 15, K_NBDEC); // JMP 18-04-2022
    T_fmt_val(trelax, (double)scl->relax, 15, -1);
    T_fmt_val(tstd, (double)scl->std, 15, K_NBDEC);  // JMP 18-04-2022
    ttest = B_calc_ttest(scl);
    T_fmt_val(tttest, ttest, 15, K_NBDEC);           // JMP 18-04-2022   
    W_printfReplEsc("%s ~i(%s, %s, %s)\n",  tcoef, trelax, tstd, tttest);

    return(0);
}

// Print the scalar kdb[pos].
int B_PrintDefScl(KDB* kdb, int pos)
{
    return(B_PrintDefSclPtr(KSVAL(kdb, pos), KONAME(kdb, pos),1));
}

/*================================= VAR ================================*/

// Print the variable kdb[pos] in a table. Sub-function of B_PrintObjDef_1().
int B_PrintDefVar(KDB* kdb, int pos)
{
    IODE_REAL    *val;
    SAMPLE  *smpl;
    int     j;

    smpl = (SAMPLE *) KDATA(kdb);

    if((val = KVVAL(kdb, pos, 0)) == NULL) return (-1);
    W_printfRepl("&1L%s ", KONAME(kdb, pos));
    for(j = 0 ; j < smpl->s_nb; j++, val++) {
        W_printfRepl("&1D");
        B_PrintVal(*val);
    }
    W_printf("\n");
    return(0);
}


/*================================= TBL ================================*/

/* Table
B_PrintTbl_1(name, smpl)
char    *name, *smpl;
{
    int rc, pos;
    TBL *tbl;

    pos = K_find(K_WS[K_TBL], name);
    if (pos < 0) {
	B_seterror("Table %.80s not found", name);
	return(-1);
    }

    tbl = KTVAL(K_WS[K_TBL], pos);
    rc = T_print_tbl(tbl, smpl);
    if(rc < 0) B_seterror("Table %.80s not printed", name);

    T_free(tbl);
    return(rc);
}

B_PrintTbl(arg)
char    *arg;
{
    int     rc;
    char    *smpl;
    U_ch    **args;

    if(arg == 0 || arg[0] == 0) {
	B_seterror("Invalid Argument");
	return(-1);
    }
    else {
	args = SCR_vtom(arg, ' ');
	if(args == NULL) {
	    B_seterror("Invalid Argument");
	    return(-1);
	}

	smpl = SCR_stracpy(args[0]);
	SCR_free_tbl(args);

	rc = B_ainit_loop(arg + strlen(smpl) + 1, B_PrintTbl_1, smpl);
    }

    SCR_free(smpl);
    B_ViewTblEnd();
    W_close();
    return(rc);
}
*/