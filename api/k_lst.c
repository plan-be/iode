/**
 * @header4iode
 *
 *  Basic functions to manipulate lists and to extract lists of VARs and SCLs from IODE objects.
 *  
 *  List of functions 
 *  -----------------
 *    int K_scan(KDB* kdb, char* l_var, char* l_scal)       Analyses a KDB content and creates 2 lists with all VAR and all SCL found in the kdb objects (limited to IDT, EQ or TBL).
 *    void KE_scan(KDB* dbe, int i, KDB* exo, KDB* scal)    Analyses object i from a KDB of EQs and extracts all VARs and all SCLs from the CLEC struct.
 *    void KI_scan(KDB* dbi, int i, KDB* exo, KDB* scal)    Analyses object i from a KDB dbi of IDTs and extracts all VARs and all SCLs from the LEC expression.
 *    void KT_scan(KDB* dbt, int i, KDB* exo, KDB* scal)    Analyses object i from a KDB of TBLs and extracts all VARs and all SCLs from the LEC expressions found in the TCELLs.
 *    int KL_lst(char* name, char** lst, int chunck)        Creates a list from a table of strings. The elements in the new list are separated by semi-colons.
 *    unsigned char **KL_expand(char *str)                  Replaces recursively list names in a string. Returns a table containing all terms in the string after replacement.
 */
 
#include "iode.h"

/**
 *  Analyses a KDB content and creates 2 lists with all VAR and all SCL found in the kdb objects (limited to IDT, EQ or TBL).
 *  
 *  @param [in] KDB*  kdb    KDB to be analysed. Only possible for IDT, EQ or TBL KDB.
 *  @param [in] char* l_var  name of the list that will contain the resulting list of VAR
 *  @param [in] char* l_scal name of the list that will contain the resulting list of SCL
 *  @return     int        
 *  
 */
int K_scan(KDB* kdb, char* l_var, char* l_scal)
{
    int     rc = 0, i, j;
    char    **lst, **K_grep();
    KDB     *exo = NULL, *scal = NULL;

    if(kdb == NULL || KNB(kdb) == 0) {
        B_seterrn(110);
        return(-1);
    }

    exo = K_create(K_OBJ, K_UPPER);
    scal = K_create(K_OBJ, K_LOWER);

    if(exo == NULL || scal == NULL) {
        rc = -1;
        goto done;
    }

    for(i = 0; i < KNB(kdb); i++) {
        switch(KTYPE(kdb)) {
            case K_IDT :
                KI_scan(kdb, i, exo, scal);
                break;
            case K_EQS :
                KE_scan(kdb, i, exo, scal);
                break;
            case K_TBL :
                KT_scan(kdb, i, exo, scal);
                break;
        }

    }

    lst = K_grep(scal, "*", 1, 1, 0, 0, '*');
    rc =  KL_lst(l_scal, lst, 200);
    SCR_free_tbl(lst);

    lst = K_grep(exo, "*", 1, 1, 0, 0, '*');
    rc = KL_lst(l_var, lst, 200);
    SCR_free_tbl(lst);

done:
    K_free(scal);
    K_free(exo);
    return(rc);
}


/**
 *  Calculates exogenous variables and scalars found in a CLEC expression. 
 *  Adds the result to 2 KDBs: exo for the exogenous and scal for the scalars.
 *  
 *  The KDB of EQs is needed to determine if a VAR is an endogenous (found in dbe) or an exogenous (not present in dbe). 
 *  Only the exigenous variables are saved in exo.
 *  
 *  @param [in]      KDB*  dbe      KDB of equations used to check if a VAR is exo or endo
 *  @param [in]      CLEC* cl       compiled LEC expression or equation
 *  @param [in, out] KDB*  exo      KDB of exogenous (only the names, not the values)
 *  @param [in, out] KDB*  scal     KDB of scalars (only the names, not the values)
 *  @return          void 
 *  
 *  @details 
 */
static void K_clecscan(KDB* dbe, CLEC* cl, KDB* exo, KDB* scal)
{
    int j, rc = 0;

    if(cl == 0) return;
    for(j = 0 ; j < cl->nb_names ; j++) {
        if(L_ISCOEF(cl->lnames[j].name))
            K_add(scal, cl->lnames[j].name, NULL, &rc);
        else {
            if(dbe != NULL
                    && K_find(dbe, cl->lnames[j].name) >= 0) continue;
            K_add(exo, cl->lnames[j].name, NULL, &rc);
        }
    }
}


/**
 *  Analyses object i from a KDB of EQs and extracts all VARs and all SCLs from the CLEC struct.
 *  The result is added to 2 KDB of type K_OBJ (i.e.: no type), no type meaning that only the object names are relevant.
 *  
 *  @param [in]      KDB* dbe    KDB of equations
 *  @param [in]      int  i      position of the equation in the dbe
 *  @param [in, out] KDB* exo    KDB (of type K_OBJ == no type) containing all VAR names found in dbe[i]
 *  @param [in, out] KDB* scal   KDB (of type K_OBJ == no type) containing all SCL names found in dbe[i]
 *  @return          void
 *  
 */
void KE_scan(KDB* dbe, int i, KDB* exo, KDB* scal)
{
    EQ      *eq;
    CLEC    *cl;

    eq = KEVAL(dbe, i);
    cl = eq->clec;
    K_clecscan(dbe, cl, exo, scal);
    E_free(eq);
}


/**
 *  Analyses object i from a KDB dbi of IDTs and extracts all VARs and all SCLs from the LEC expression.
 *  
 *  The result is added to 2 KDB of type K_OBJ (i.e.: no type), no type meaning that only the object names are relevant.
 *  
 *  @param [in]      KDB* dbi    KDB of identities
 *  @param [in]      int  i      position of the identity in the dbi
 *  @param [in, out] KDB* exo    KDB (of type K_OBJ == no type) containing all VAR names found in dbi[i]
 *  @param [in, out] KDB* scal   KDB (of type K_OBJ == no type) containing all SCL names found in dbi[i]
 *  @return          void
 *  
 */
void KI_scan(KDB* dbi, int i, KDB* exo, KDB* scal)
{
    int     lg;
    CLEC    *cl = NULL;

    lg = KICLEC(dbi, i)->tot_lg;
    cl = (CLEC *) SW_nalloc(lg);
    memcpy(cl, KICLEC(dbi, i), lg);

    K_clecscan(dbi, cl, exo, scal);
    SW_nfree(cl);
}


/**
 *  Analyses object i from a KDB of TBLs and extracts all VARs and all SCLs from the LEC expressions found in the TCELLs.
 *  
 *  The result is added to 2 KDB of type K_OBJ (i.e.: no type), no type meaning that only the object names are relevant.
 *  
 *  @param [in]      KDB* dbt    KDB of tables
 *  @param [in]      int  i      position of the table in the dbt
 *  @param [in, out] KDB* exo    KDB (of type K_OBJ == no type) containing all VAR names found in dbt[i]
 *  @param [in, out] KDB* scal   KDB (of type K_OBJ == no type) containing all SCL names found in dbt[i]
 *  @return          void
 *  
 */
void KT_scan(KDB* dbt, int i, KDB* exo, KDB* scal)
{
    int     k, l;
    TBL     *tbl;
    TCELL   *cell;
    CLEC    *cl = NULL;

    tbl = KTVAL(dbt, i);
    for(k = 0; k < T_NL(tbl); k++)   {
        if(tbl->t_line[k].tl_type != KT_CELL) continue;

        cell = (TCELL *) tbl->t_line[k].tl_val;
        for(l = 0; l < T_NC(tbl); l++) {
            if(cell[l].tc_type != KT_LEC) continue;

            cl = P_get_ptr(cell[l].tc_val, 1);
            K_clecscan(NULL, cl, exo, scal);
        }
    }

    T_free(tbl);
}


/**
 *  Creates a list from a table of strings. The elements in the new list are separated by semi-colons.
 *  If the list exceeds chunck strings, it is split into sublists with each max chunck elements. The
 *  sublists have the same name with a numerical suffix.
 *  
 *  Example: 
 *      char** tbl;
 *      tbl = SCR_vtom("A,B,C,D,E,F,G,H", ',');
 *      KL_lst("MYLIST", tbl, 5); // Creates 3 lists : 
 *                                //     MYLIST  => "$MYLIST1;$MYLIST2"
 *                                //     MYLIST1 => "A,B,C,D,E"
 *                                //     MYLIST2 => "F,G,H"
 *    
 *  @param [in] char*   name    name or the resulting list
 *  @param [in] char**  lst     table of strings
 *  @param [in] int     chunck  max number of strings per (sub-)list
 *  @return     int             0 on success, < 0 on, error (rc from K_add())
 *  
 */
int KL_lst(char* name, char** lst, int chunck)
{
    int     rc = 0, i, j, nb;
    char    *str, *ptr, buf[30];

    nb = SCR_tbl_size(lst);
    if(nb == 0) {
        if(K_add(K_WS[K_LST], name, "") < 0)  rc = -1;
        goto done;
    }

    if(nb < chunck || chunck < 0) { /* GB 16/10/2007 */
        str = SCR_mtov(lst, ';'); /* JMP 09-03-95 */
        if(K_add(K_WS[K_LST], name, str) < 0)  rc = -1;
        SCR_free(str);
        return(rc);
    }

    for(i = 0, j = 0; i < nb && !rc; i+= chunck, j++) {
        if(i + chunck < nb) {
            ptr = lst[i + chunck];
            lst[i + chunck] = NULL;
        }

        str = SCR_mtov(lst + i, ';');
        sprintf(buf, "%s%d", name, j);
        buf[K_MAX_NAME] = 0;
        if(K_add(K_WS[K_LST], buf, str) < 0)  rc = -1;
        SCR_free(str);

        if(i + chunck < nb)  lst[i + chunck] = ptr;
    }
    if(rc < 0) goto done;

    str = (char *) SW_nalloc(j * (K_MAX_NAME + 2));
    str[0] = 0;
    for(i = 0; i < j; i++) {
        sprintf(buf, "$%s%d;", name, i); /* GB 23/01/98 */
        buf[K_MAX_NAME] = 0;
        strcat(str, buf);
    }
    if(K_add(K_WS[K_LST], name, str) < 0) rc = -1;
    SW_nfree(str);

done:
    return(rc);
}


/**
 *  Replaces recursively list names in a string. Returns a table containing all terms in the string after replacement.
 *  The lists are searched in the current LST KDB.
 *  
 *  Example:
 *      Suppose we have the lists below in the current KDB of lists:
 *          A = "$A1;$A2"
 *          A1 = "X;Y"
 *          A2 = "Z"
 *  
 *      KL_expand("$A B C") => "X", "Y", "Z", "B", "C"
 *  
 *  @param [in] char*   str     list of terms separated by one of the chars in " ,;\t\n\r\f" and 
 *                              containing IODE lists under the form of $LISTNAME.
 *  @return     char**          table containing all terms in str after replacement.
 *  
 */
unsigned char **KL_expand(char *str)
{
    unsigned char 	**tbl, **tbl2, *seps = " ,;\t\n\r\f";
    int				i, nb, nb2, pos;

    tbl = SCR_vtoms(str, seps);
    nb = SCR_tbl_size(tbl);
    if(SCR_tbl_size(tbl) == 0) return(tbl);
    for(i = 0 ; tbl[i] ; i++) {
        if(tbl[i][0] == '$') {
            pos = K_find(K_WS[K_LST], tbl[i] + 1);
            if(pos >= 0) {
                SCR_free(tbl[i]); // plus besoin car remplac� par sa valeur
                tbl2 = KL_expand(KLVAL(K_WS[K_LST], pos));
                nb2 = SCR_tbl_size(tbl2);
                // Insertion dans tbl de la liste � la place de tbl[i]
                tbl = (unsigned char **) SCR_realloc(tbl, sizeof(char *), nb + 1, (nb + 1 - 1) + nb2);
                // d�caler ceux qui suivent i (nb -i - 1 �ls) de nb2 places vers la droite
                memcpy(tbl + i + nb2, tbl + i + 1, sizeof(char *) * (nb - i - 1));
                // recopier tbl2 � la place de i et suivants
                memcpy(tbl + i, tbl2, sizeof(char *) * nb2);
                nb += nb2 - 1; // -1 car on remplace le courant
                SCR_free(tbl2); // pas tbl_free car ptrs copi�s dans tbl
            }
        }
    }
    return(tbl);
}
