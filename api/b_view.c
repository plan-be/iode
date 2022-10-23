/**
 *  @header4iode
 *  
 *  Functions to generate IODE tables in A2M format based on TBL structures and GSAMPLE definition.
 *  Includes some A2M helper functions. 
 *  
 *  List of functions 
 *  -----------------
 */

#include "iode.h"

int B_viewmode;     // 0: displays the graph/table on screen, 1: print graph/table

int     ODE_VIEW = 0;
char    ODE_SMPL[81];

/* Table */

int B_ViewVar(char* arg)
{
    return(B_ViewPrintVar(arg, 0));
}

int B_PrintVar(char* arg)
{
    return(B_ViewPrintVar(arg, 1));
}

int B_ViewPrintVar(char* arg, int mode)
{
    int     rc = 0, nb, i;
    char    *smpl, *ptr;
    U_ch    **args;
    TBL     *tbl;
    char    *oldseps = A_SEPS, *lst;  /* JMP 14-07-96 */

    if(arg == 0 || arg[0] == 0) {
        B_seterrn(74);
        return(-1);
    }
    args = SCR_vtom(arg, ' ');
    if(args == NULL) {
        B_seterrn(74);
        return(-1);
    }

    smpl = SCR_stracpy(args[0]);
    SCR_free_tbl(args);

    A_SEPS = ";\t\n"; /* JMP 24-12-98 */
    lst = K_expand(K_VAR, NULL, arg + strlen(smpl) + 1, '*');
    args = B_ainit_chk(lst, NULL, 0);
    SCR_free(lst);
    A_SEPS = oldseps;    /* JMP 14-07-96 */
    if(args == 0 || args[0] == 0) {
        B_seterrn(74);
        rc = -1;
        goto fin;
    }
    nb = SCR_tbl_size(args);
    for(i = 0 ; i < nb ; i += 50) {
        if(i + 50 < nb) {
            ptr = args[i + 50];
            args[i + 50] = 0;
        }
        tbl = T_create(2);
        if(mode == 0) {
            T_default(tbl, 0L, args + i, args + i, 0, 0, 0);
            rc = T_view_tbl(tbl, smpl, "of series");
        }
        else {
            T_default(tbl, 0L, args + i, args + i, 1, 1, 1);
            rc = T_print_tbl(tbl, smpl);
        }
        T_free(tbl);
        if(i + 50 < nb) args[i + 50] = ptr;
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

int B_ViewByTbl(char* arg)
{
    return(B_ViewPrintTbl(arg, 0, 0));
}

int B_ViewTbl(char* arg)
{
    return(B_ViewPrintTbl(arg, 0, 0));
}

int B_PrintTbl(char* arg)
{
    return(B_ViewPrintTbl(arg, 0, 1));
}

int B_ViewGr(char* arg)
{
    return(B_ViewPrintTbl(arg, 1, 0));
}

/*
B_PrintGrAll(arg)
char    *arg;
{
    B_what = 0;
    return(B_PrintGr(arg));
}

B_PrintGrData(arg)
char    *arg;
{
    B_what = 1;
    return(B_PrintGr(arg));
}

B_PrintGrWin(arg)
char    *arg;
{
    B_what = 2;
    return(B_PrintGr(arg));
}
/* JMP 02-12-97 */

int B_PrintGr(char* arg)
{
    return(B_ViewPrintTbl(arg, 1, 1));
}



int B_ViewPrintTbl_1(char* name, char* smpl)
{
    int rc, pos;
    TBL *tbl;

    pos = K_find(K_WS[K_TBL], name);
    if(pos < 0) {
        B_seterrn(80, name);
        return(-1);
    }

    tbl = KTVAL(K_WS[K_TBL], pos);
    if(B_viewmode == 0)
        rc = T_view_tbl(tbl, smpl, name);
    else
        rc = T_print_tbl(tbl, smpl);

    if(rc < 0) B_seterrn(81, name);

    T_free(tbl);
    return(rc);
}

int B_ViewPrintGr_1(char* names, char* gsmpl)
{
    //extern int KT_nb;
    int     rc = 0, pos, hg, ng, i, 
            //app = KT_nb, 
            view = !B_viewmode;
    TBL     *tbl;
    char    **tbls;

    tbls = SCR_vtoms(names, "+-");
    ng = SCR_tbl_size(tbls);
    if(ng == 0) {
        B_seterrn(82);
        return(-1);
    }

    for(i = 0; i < ng; i ++) {
        if(view) W_InitDisplay();

        pos = K_find(K_WS[K_TBL], tbls[i]);
        if(pos < 0) {
            B_seterrn(80, tbls[i]);
            rc = -1;
            break;
        }
        tbl = KTVAL(K_WS[K_TBL], pos);
        //KT_nb++;
        hg = T_graph_tbl_1(tbl, gsmpl, B_viewmode); // JMP 11-05-2022 to avoid extern in k_graph.c

        if(view) W_EndDisplay(T_get_title(tbl), -ng, -i, -1, -1);

        T_free(tbl);
        if(hg < 0) {
            rc = -1;
            break;
        }
    }

    SCR_free_tbl(tbls);
    return(rc);
}

/**
 *  
 *  
 *  @param [in] char* arg   
 *  @param [in] int   type  0: print or view in table format
 *                          1: print in graph format 
 *  @param [in] int   mode  0: view
 *                          1: print
 *  @return 
 *  
 *  @details 
 */
int B_ViewPrintTbl(char* arg, int type, int mode)
{
    int     rc = 0;
    char    *smpl;
    U_ch    **args;
    //extern int KT_nb;

    //KT_nb = 0;

    B_viewmode = mode;
    if(arg == 0 || arg[0] == 0) {
        B_seterrn(74);
        return(-1);
    }
    else {
        args = SCR_vtom(arg, ' ');
        if(args == NULL || args[0] == NULL) {
            B_seterrn(74);
            return(-1);
        }

        smpl = SCR_stracpy(args[0]);

        if(mode == 1 || SCR_tbl_size(args) < 3) {
            if(type == 0)
                rc = B_ainit_loop(arg + strlen(smpl) + 1, B_ViewPrintTbl_1, smpl);
            else
                rc = B_ainit_loop(arg + strlen(smpl) + 1, B_ViewPrintGr_1, smpl);
        }
        else {
            if(type == 0) ODE_VIEW = 1;
            else ODE_VIEW = 2;
            strcpy(ODE_SMPL, smpl);

            ODE_scroll(K_WS[K_TBL], args + 1);
        }

        SCR_free_tbl(args);
    }

    SCR_free(smpl);

    /*    W_close();*/
    W_flush();
    B_ViewTblEnd();
    return(rc);
}

int B_ViewTblFile(char* arg)
{
    int     ref, rc = 0;
    KDB     *kdb = NULL;
    U_ch    **args = NULL;

    if(arg == 0 || arg[0] == 0) {
        B_seterrn(74);
        return(-1);
    }
    else {
        args = SCR_vtom(arg, ' ');
        if(args == NULL)  {
            B_seterrn(74);
            rc = -1;
            goto err;
        }

        if(SCR_tbl_size(args) < 2) goto err;

        ref = atoi(args[0]);
        if(ref < 2 || ref > 5) {
            B_seterrn(73);
            rc = -1;
            goto err;
        }

        kdb = K_interpret(K_VAR, args[1]);
        if(kdb == NULL) {
            rc = -1;
            goto err;
        }

        K_free(K_RWS[K_VAR][ref - 1]);
        K_RWS[K_VAR][ref - 1] = kdb;
    }

err:
    SCR_free_tbl(args);
    return(rc);
}

int B_ViewTblEnd()
{
    int     i;

    for(i = 1; i < 5; i++) {
        K_free(K_RWS[K_VAR][i]);
        K_RWS[K_VAR][i] = NULL;
    }
    ODE_VIEW = 0;
    ODE_SMPL[0] = '\0';
    return(0);
}
