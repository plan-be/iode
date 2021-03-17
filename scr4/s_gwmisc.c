#include "scr4.h"

WapTransOptText(U_ch  *ansi, U_ch  *ascii, int key, int prfx)
{
    int     i, doneacc = 0, donebl = -1, bl = 0;

    key -= SCR_A_A - 'A';

    SCR_OemToAnsi(ansi, ascii);
    SCR_strip(ansi);
    for(i = 0 ; ansi[i] ; i++) {
	if(doneacc == 0 && donebl < 1 &&
		    (ansi[i] == key || ansi[i] == key + 'a' - 'A')) {
	    SCR_strrcpy(ansi + i + 1, ansi + i);
	    ansi[i] = prfx;  /* JMP 03-01-03 */
	    doneacc = 1;
	    i++;
	    }

	if(donebl == -1 && ansi[i] != ' ') donebl = 0;
	if(donebl == 0 && ansi[i] == ' ') {
	    if(bl) {
		bl = i - 1;
		while(ansi[i] == ' ') i++;
		strcpy(ansi + bl + 1, ansi + i);
		ansi[bl] = '\t';
		donebl = 1;
		i = bl;
		}
	    bl = 1;
	    }
	else bl = 0;
	}

    return(0);
}

MENU *WapFindActMenu(int act_nu)
{
    FILE_TYPE   *flt;
    A_ACTION    *aa;
    ACTION      *act;
    MENU        *mn = 0;
    int         mem, i;
    char        *buf;

    if(act_nu == 0) return(mn);
    flt = SCR_READ_FILES.tbl_fl[act_nu];
    mem = flt->fl_mem;
    if(mem == 0) act = (ACTION *)SCR_find_load_obj(act_nu);
    else         act = (ACTION *)flt->fl_ptr;

/***********    if(act) {
	buf = (char *)act->act_aa;
	for(i = 0 ; i < act->act_nb ; i++) {
	    aa = (A_ACTION *)buf;
	    if(aa->aa_fn == ACT_MN_EDIT) {
		mn = (MENU *)SCR_PTR(ACT_find_lg(aa, 0));
		break;
		}
	    buf += aa->aa_len;
	    }
	}
********* Correction: si MN_edit pas SEUL point, ne pas retenir
/* JMP 04-05-97 */

    if(act && act->act_nb == 1) {
	aa = (A_ACTION *)act->act_aa;
	if(aa->aa_fn == ACT_MN_EDIT)
	    mn = (MENU *)SCR_PTR(ACT_find_lg(aa, 0));
	}

    if(mem == 0) SCR_unload_obj(act_nu);
    return(mn);
}


int WscrGetLpgFld(PAGE *lpg, int col)
{
    int     i, j;

    PG_read_flds(lpg);
    for(i = j = 0 ; i < lpg->pg_nb_flds ; i++) {
	if(lpg->pg_flds[i].fld_io == SCR_HIDDEN - SCR_INPUT) continue;
	if(lpg->pg_flds[i].fld_line != 0) continue;
	if(col == j) return(i);
	j++;
	}
    return(0);
}

WscrGetNc(PAGE *lpg)
{
    int     i, j;

    PG_read_flds(lpg);
    for(i = j = 0 ; i < lpg->pg_nb_flds ; i++) {
	if(lpg->pg_flds[i].fld_io == SCR_HIDDEN - SCR_INPUT) continue;
	if(lpg->pg_flds[i].fld_line != 0) continue;
	j++;
	}
    return(j);
}




/* ========================================================
    Positionne un MENU juste en dessous d'un object (champs d'une page)
    mn : OUT : MENU *
    obj : IN : OBJ
 ======================================================== */

WscrPosMenu(MENU *mn, OBJ obj)      /* BP_M 13-02-2003 12:25 */
{
    PAGE        *pg;
    int     x, y, w, h;

    if(obj.obj_type != SCR_TYPE_PAGE) return;
    if(mn == 0) return;

    pg = (PAGE *)obj.obj_ptr;

    if(WscrGetFldPos(pg, obj.obj_fld, &x, &y, &w, &h) == 0) {
	mn->mn_w_x = x;
	mn->mn_w_y = y + h;
/*      if(WscrGetFldPos(pg, -1, &x, &y, &w, &h) == 0)
	    mn->mn_w_x = x+5;
*/
    }
    else {
	if(WscrGetFldPos(pg, -2, &x, &y, &w, &h) == 0) {
	    mn->mn_w_x = x;
	    mn->mn_w_y = y + 10;
	}
	else {
	    mn->mn_w_x = 20;
	    mn->mn_w_y = 20;
	}
    }
}


/* ========================================================
    Positionne une page par rapport … un objet
    pg : OUT : Page *
    obj : OBJ : Object de r‚f‚rence
    type : 0 Sur le champs
	   1 juste en dessous
	   2 … droite du champs
	   3 en dessous … droite
======================================================== */

WscrPosPg(PAGE *pg, OBJ obj, int type)
{
    int     x, y, w, h;
    PAGE    *pg_ref;

    if(obj.obj_type != SCR_TYPE_PAGE) return;
    if(pg == 0) return;

    pg_ref = (PAGE *)obj.obj_ptr;

    if(WscrGetFldPos(pg_ref, obj.obj_fld, &x, &y, &w, &h) == 0) {
	pg->pg_w_x = x;
	pg->pg_w_y = y;
	if(type == 1 || type == 3)
	    pg->pg_w_y += h;
	if(type == 2 || type == 3)
	    pg->pg_w_x += w;
    }
    else {
	if(WscrGetFldPos(pg_ref, -2, &x, &y, &w, &h) == 0) {
	    pg->pg_w_x = x;
	    pg->pg_w_y = y + 10;
	}
	else {
	    pg->pg_w_x = 20;
	    pg->pg_w_y = 20;
	}
    }
}

SCR_get_cmp_flds(pg, is, cmp_nb, cmp_flds)
PAGE    *pg;
ISAM    *is;
int     cmp_nb;
int     *cmp_flds;
{
    int     j;
    int     k;
    int     count = 0;
    COMPOSITE *cmp;
    FIELD   *fld;
    ISFLD   *isf;
    int     l;

	cmp = is->is_cmps + cmp_nb;
	for(j = 0  ; j < cmp->cmp_nb_flds ; j++)
	{
	    for(k = 0 ; k < pg->pg_nb_flds ; k++)
	    {
		fld = pg->pg_flds + k;
		isf  = fld->fld_isam;
		for(l = 0 ; l < fld->fld_nb_isam ; l++, isf++)
		    if( (SCR_PTR(isf->if_isam) == (char *)is) &&
			(fld->fld_io == 0 || SCR_COMP_RI) && /* JMP 15-06-96 */
			(isf->if_isam_nb == cmp->cmp_flds[j]) )
		    {
/*                      if(count >= SCR_MAX_CMP_FIELDS) SCR_yyerror(ERR_ERROR, ERR_LIMIT_CMP, "");
   JMP 27-08-91 */
			cmp_flds[count] = k;
			count++;
			break;
		    }
		if(l < fld->fld_nb_isam) break;
	    }
	    if(count == 0) return(count);
	}

    return(count);
}















