#include "iode.h"

K_scan(kdb, l_var, l_scal)
KDB     *kdb;
char    *l_var, *l_scal;
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

void K_clecscan(dbe, cl, exo, scal)
KDB     *dbe, *exo, *scal;
CLEC    *cl;
{
    int j, rc = 0;

    if(cl == 0) return(0);
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

void KE_scan(dbe, i, exo, scal)
KDB     *dbe, *exo, *scal;
int     i;
{
    EQ      *eq;
    CLEC    *cl;

    eq = KEVAL(dbe, i);
    cl = eq->clec;
    K_clecscan(dbe, cl, exo, scal);
    E_free(eq);
}

void KI_scan(dbi, i, exo, scal)
KDB     *dbi, *exo, *scal;
int     i;
{
    int     lg;
    CLEC    *cl = NULL;

    lg = KICLEC(dbi, i)->tot_lg;
    cl = SW_nalloc(lg);
    memcpy(cl, KICLEC(dbi, i), lg);

    K_clecscan(dbi, cl, exo, scal);
    SW_nfree(cl);
}

void KT_scan(dbt, i, exo, scal)
KDB     *dbt, *exo, *scal;
int     i;
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

KL_lst(name, lst, chunck)
char    *name, **lst;
int     chunck;
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
                SCR_free(tbl[i]); // plus besoin car remplacé par sa valeur
                tbl2 = KL_expand(KLVAL(K_WS[K_LST], pos));
                nb2 = SCR_tbl_size(tbl2);
                // Insertion dans tbl de la liste à la place de tbl[i]
                tbl = SCR_realloc(tbl, sizeof(char *), nb + 1, (nb + 1 - 1) + nb2);
                // décaler ceux qui suivent i (nb -i - 1 éls) de nb2 places vers la droite
                memcpy(tbl + i + nb2, tbl + i + 1, sizeof(char *) * (nb - i - 1));
                // recopier tbl2 à la place de i et suivants
                memcpy(tbl + i, tbl2, sizeof(char *) * nb2);
                nb += nb2 - 1; // -1 car on remplace le courant
                SCR_free(tbl2); // pas tbl_free car ptrs copiés dans tbl
            }
        }
    }
    return(tbl);
}






