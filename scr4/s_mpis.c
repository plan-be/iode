#include "scr.h"

int SC_CONDS[] = {SCR_GE, SCR_LE, SCR_EQ,
		      SCR_EQ1, SCR_EQ2, SCR_EQ3, SCR_EQ4, SCR_EQ5};

/*NH*/
MP_start1(mp)
MPAGE   *mp;
{
    /* ALLOC BUFS */
    MP_alloc_bufs(mp);
    HLP_stack_add(mp->mp_help);

    /* OPEN ISAM */
    if(0 != MP_open(mp)) {
	PG_display_error(IS_error_text());
	return(-1);
    }
    return(0);
}

/*NH*/
MP_start2(mp, cmp_nb)
MPAGE   *mp;
int     cmp_nb;
{
    char    *is_rec;
    ISAM    *is  = (ISAM *)SCR_PTR(mp->mp_isam);
    PAGE    *apg = (PAGE *)SCR_PTR(mp->mp_apg);
    extern int SCR_PG2IS_INPUT;  /* JMP 15-12-98 */

    if(cmp_nb >= 0) mp->mp_index = cmp_nb;
    PG_read_flds(apg);
    if(apg->pg_fld_idx != MAX_BFIELD12)
	mp->mp_index = SCR_get_field_short(apg, apg->pg_fld_idx) - 1;
    if(apg->pg_fld_cond != MAX_BFIELD12)
	mp->mp_cond = SC_CONDS[SCR_get_field_short(apg, apg->pg_fld_cond)];

    is_rec = is->is_rec;
    is->is_rec = mp->mp_ref_rec;
    SCR_PG2IS_INPUT = 1;
    SCR_page_to_isam(apg, is);
    SCR_PG2IS_INPUT = 0;
    is->is_rec = is_rec;

    return(0);
}


/*NH*/
/* =+=== MP_open ==================================================
    Open the isam file associated with the MPAGE
    If already open, does not reopen
    mp : IO : pointer to the multi-page
   =================================================================== */

MP_open(mp)
MPAGE   *mp;
{

    IS_open(ISAM_PTR(mp->mp_isam), SCR_UPDATE, SCR_NO_LOCK);
    MP_reinit(mp);
    return(0);
}

/*NH*/
/* =+=== MP_close ==================================================
    Close the isam file associated with the MPAGE
    mp : IO : pointer to the multi-page
   =================================================================== */

MP_close(mp)
MPAGE   *mp;
{
    IS_close(ISAM_PTR(mp->mp_isam));
    MP_reinit(mp);
    return(0);
}


/*NH*/
MP_set_cbuf(mp)
MPAGE   *mp;
{
    U_ch    *rec;
    ISAM    *is = (ISAM *)SCR_PTR(mp->mp_isam);

    if(mp->mp_c_line < 0) return(0);
    rec = mp->mp_buf + (mp->mp_line0 + mp->mp_c_line) * is->is_rec_len;
    memcpy(rec, is->is_rec, is->is_rec_len);
    return(0);
}

/*NH*/
MP_alloc_bufs(mp)
MPAGE   *mp;
{
    int     i;
    PAGE    *lpg;
    FIELD   *fld;
    ISAM    *is = (ISAM *)SCR_PTR(mp->mp_isam), *isj;
    int     len = is->is_rec_len;

    /* ALLOCATE AND INITIALIZE */
    mp->mp_buf_def = SCR_malloc(mp->mp_buf_size * 2);
    memset(mp->mp_buf_def, 'N', mp->mp_buf_size);

    for(i = 0 ; i < is->is_nb_join ; i++) {
	isj = (ISAM *)SCR_PTR(is->is_join[i].is);
	len += isj->is_rec_len;
    }
    mp->mp_tot_len_rec = len;
    mp->mp_buf = SCR_malloc(len * mp->mp_buf_size * 2);

    /* ALLOCATE SPACE FOR LITTLE PAGES */
    lpg = (PAGE *)SCR_PTR(mp->mp_lpg);
    mp->mp_lg_lpg = lpg->pg_rec_len;
    if(mp->mp_lpg_mem)
	    mp->mp_lpgs = SCR_malloc(lpg->pg_rec_len * mp->mp_buf_size * 2);

    return(0);
}

/*NH*/
MP_free_bufs(mp)
MPAGE   *mp;
{
    PAGE    *spg = (PAGE *)SCR_PTR(mp->mp_spg);

    if(mp->mp_c_rec > 0 && mp->mp_c_line >= 0) {
	SCR_free(mp->mp_buf_def);
	SCR_free(mp->mp_buf);   /* BP_M 16-07-2010 11:42 */
	if(mp->mp_lpg_mem) SCR_free(mp->mp_lpgs);
    }
    MP_free_scan(mp);
    return(0);
}

/*NH*/
MP_free_scan(mp)
MPAGE   *mp;
{
    PAGE    *spg = (PAGE *)SCR_PTR(mp->mp_spg);

    if(mp->mp_spg && spg->pg_scan) {
	SCR_free(spg->pg_scan->from);
	SCR_free(spg->pg_scan->to);
	SCR_free(spg->pg_scan->from_def);
	SCR_free(spg->pg_scan);
	spg->pg_scan = 0;
	}
    return(0);
}

/*NH*/
/* =+=== MP_reinit ==================================================
    Re-intialize the mpage (set to the first record)
    mp : IO : pointer to the multi-page
   =================================================================== */

MP_reinit(mp)
MPAGE   *mp;
{
    int     i;

    mp->mp_c_line = -1;
    mp->mp_line0 = 0;
    for(i = 0 ; i < mp->mp_buf_size ; i++)
	mp->mp_buf_def[i] = 'N';
    return(0);
}

/*NH*/
/* =+========== MP_FNKEYS ======================================
    Execute the fnkeys
	mp     : IN : pointer to the page
	key    : IN : key pressed
    Return = 1 if MP_edit must be leaved, 0 else
    =================================================================== */

MP_fnkeys(mp, key)
MPAGE   *mp;
int     key;
{
    int     i, pg_abort, ap_abort;
    FNKEYS  *fk = (FNKEYS *)SCR_PTR(mp->mp_fnks);

    i = FNK_is_fnk(fk, key);
    if(i < 0) {
	SCR_beep();
	return(1);
    }
/* JMP 11-07-91 */
    if(mp->mp_c_line >= 0 &&
	MP_reposition(mp, mp->mp_line0 + mp->mp_c_line) != 0) {
	    MP_error();
	    SCR_record_key(SCR_ESCAPE);
	    return(-1);
	    }

    /* BP_M 05-03-2010 12:28 */
    if(SCR_check_prot_vt(fk->fk_fnks[i].fnk_prot)) return(-1);

    i = ACT_exec_nu(fk->fk_fnks[i].fnk_act);

    /* BP_M 14-06-1999 */
    pg_abort = SCR_PG_ABORT;
    ap_abort = SCR_APP_ABORT;
    if(i == 0) MP_adjust_bufs(mp);
    /* BP_M 14-06-1999 */
    SCR_PG_ABORT  = pg_abort;
    SCR_APP_ABORT = ap_abort;

    return(i);
}
/*NH*/
MP_move(mp, diff)
MPAGE   *mp;
int     diff; // Difference par rapport à la pos courante
{
    mp->mp_c_line = diff;
    MP_display_lpg(mp, mp->mp_c_line, 1);
    return(0);
}

/*NH*/
MP_up(mp)
MPAGE   *mp;
{
    if(mp->mp_c_line < 0) {
	SCR_beep();
	return(1);
	}

/* FIRST RECORD OF THE SCREEN */
    if(mp->mp_c_line == 0)
	return(MP_up_window(mp, 1));

/* RECORD WITHIN THE SCREEN (NOT FIRST) */
    MP_display_lpg(mp, mp->mp_c_line, 0);
    mp->mp_c_line--;
    MP_display_lpg(mp, mp->mp_c_line, 1);
    return(0);
}

/*NH*/
MP_up_silent(mp) /* JMP 02-03-03 */
MPAGE   *mp;
{
    if(mp->mp_c_line < 0) {
	SCR_beep();
	return(1);
	}

    if(mp->mp_c_line == 0)
	return(MP_up_window(mp, 1));

    mp->mp_c_line--;
    return(0);
}
/*NH*/
/* =+===== MP_DOWN ===========
    Move one record down.
    mp : IO : MPAGE *
   ========================= */

MP_down(mp)
MPAGE   *mp;
{
    if(mp->mp_c_line < 0) {
	SCR_beep();
	return(-1);
	}

/* LAST RECORD OF THE SCREEN */
    if(mp->mp_c_line == mp->mp_nb_recs - 1)
	return(MP_down_window(mp, 1));

/* RECORD WITHIN THE SCREEN (NOT LAST) */
/*    if(MP_fill_screen(mp) != 0) {
	MP_redisplay(mp);
	return(-1);
	}
/* JMP_M 4.19 03-06-95 */

    if(mp->mp_buf_def[mp->mp_line0 + mp->mp_c_line + 1] != 'Y') {
	SCR_beep();
	return(0);
	}

     MP_display_lpg(mp, mp->mp_c_line, 0);
     mp->mp_c_line++;
     MP_display_lpg(mp, mp->mp_c_line, 1);
     return(0);
}
/*NH*/
MP_down_silent(mp)
MPAGE   *mp;
{
    if(mp->mp_c_line < 0) {
	SCR_beep();
	return(-1);
	}

    if(mp->mp_c_line == mp->mp_nb_recs - 1)
	return(MP_down_window(mp, 1));

    if(mp->mp_buf_def[mp->mp_line0 + mp->mp_c_line + 1] != 'Y') {
	SCR_beep();
	return(0);
	}

     mp->mp_c_line++;
     return(0);
}

/*NH*/
/* =+===== MP_PG_UP ===========
    Move one page up.
    mp : IO : MPAGE *
   ========================= */

MP_pg_up(mp)
MPAGE   *mp;
{
    if(mp->mp_c_line < 0) {
	SCR_beep();
	return(1);
	}

    return(MP_up_window(mp, mp->mp_nb_recs));
}
/*NH*/
/* =+===== MP_PG_DOWN ===========
    Move one page down.
    mp : IO : MPAGE *
   ========================= */

MP_pg_down(mp)
MPAGE   *mp;
{
    if(mp->mp_c_line < 0) {
	SCR_beep();
	return(-1);
	}

    if(mp->mp_buf_def[mp->mp_line0 + mp->mp_nb_recs - 1] != 'Y') {
	SCR_beep();
	return(-1);
	}
    return(MP_down_window(mp, mp->mp_nb_recs));
}
/*NH*/
/* =+===== MP_HOME ================
    Move to first rec
    mp : IO : MPAGE *
   ================================ */

MP_home(mp)
MPAGE   *mp;
{
    /* BP_MODIF 29-10-98 */
    extern int MP_SEARCH_LWHERE;

    mp->mp_c_line = -1;
#ifdef PDEV
    if(PG_pgvr())return(0);
#endif

    if(((ISAM *)SCR_PTR(mp->mp_isam))->is_type_dbd > 0) /* JMP 02-11-98 */
	    MP_SEARCH_LWHERE = 1;
    MP_redisplay(mp);
    MP_SEARCH_LWHERE = 0;
    return(0);
}
/*NH*/
/* =+===== MP_END ================
    Move to last rec of the screen
    mp : IO : MPAGE *
   ================================ */

MP_end(mp)
MPAGE   *mp;
{
    if(mp->mp_c_line < 0) {
	SCR_beep();
	return(1);
	}

    MP_display_lpg(mp, mp->mp_c_line, 0);
    while(mp->mp_c_line + 1 < mp->mp_nb_recs &&
	  mp->mp_buf_def[mp->mp_c_line + 1] == 'Y') mp->mp_c_line++;

    MP_display_lpg(mp, mp->mp_c_line, 1);
    return(0);
}
/*NH*/
/* =+=== MP_UP_WINDOW ==============================================
    Scroll n lines in buffer (and redisplay).
    A current record will be defined if needed
    Read the nl previous records (if necessary) and store in buffer.
     mp : IO : MPAGE *
     nl : IN : nb of lines to scroll
   ================================================================= */

MP_up_window(mp, nl)
MPAGE   *mp;
int     nl;
{
    int     i;
    int     scroll = 0;

    if(mp->mp_c_line < 0) {
	SCR_beep();
	return(-1);
	}

    if(mp->mp_line0 >= nl) { /* previous part of the screen is in buffer */
	mp->mp_line0 -= nl;
	MP_redisplay(mp);
	return(0);
	}

    if(mp->mp_line0 != 0) {
	scroll = 1;
	nl -= mp->mp_line0;
	mp->mp_line0 = 0;
	}

    /* IF PREVIOUS IMPOSSIBLE, RETURN */
    if(IS_def_prev((ISAM *)SCR_PTR(mp->mp_isam)) == 0) { /* JMP 02-11-98 */
	if(mp->mp_line0 == 0) SCR_beep();
	mp->mp_line0 = 0;
	MP_redisplay(mp);
	return(0);
	}

    /* REPOSITION ON RECORD 0 */
    if(MP_reposition(mp, 0) != 0) {
	MP_error();
	MP_redisplay(mp);
	return(-1);
	}

    for(i = 0 ; i < nl ; i++) {
	if(MP_prev(mp, 1) != 0 ) {
	    if(i == 0 && scroll == 0) SCR_beep();
	    MP_redisplay(mp);
	    return(0);
	    }
	MP_shift_buf_up(mp);
	MP_isam_to_buf(mp);
	}

    mp->mp_line0 = 0;
    MP_redisplay(mp);
    return(0);
}
/*NH*/
/* =+=== MP_DOWN_WINDOW ==============================================
    Scroll n lines in buffer (and redisplay).
    Read the nl next records (if necessary) and store in buffer.
     mp : IO : MPAGE *
     nl : IN : nb of lines to scroll
   ================================================================= */

MP_down_window(mp, nl)
MPAGE   *mp;
int     nl;
{
    int     i;

    if(mp->mp_c_line < 0) {
	SCR_beep();
	return(-1);
	}

    /* SCROLL ON KNOWN RECORDS */
    while(mp->mp_line0 + mp->mp_nb_recs < mp->mp_buf_size &&
	mp->mp_buf_def[mp->mp_line0 + mp->mp_nb_recs] == 'Y' &&
	nl > 0) {
	  mp->mp_line0++;
	  nl--;
	  }
    if(nl == 0) {
	MP_redisplay(mp);
	return(0);
	}

    /* REPOSITION ON LAST KNOWN RECORD */
    if(MP_reposition(mp, mp->mp_line0 + mp->mp_nb_recs - 1) != 0) {
	MP_error();
	MP_redisplay(mp);
	return(-1);
	}

    /* FILL BUFFER */
    i = 0;
    while(mp->mp_line0 + mp->mp_nb_recs < mp->mp_buf_size &&
		nl > 0) {
	if(MP_next(mp, 1) != 0) {
	    if(i == 0) SCR_beep();
	    MP_redisplay(mp);
	    return(0);
	    }
	i = 1;
	mp->mp_c_rec++;
	mp->mp_line0++;
	nl--;
	MP_isam_to_buf(mp);
	}

    /* SCROLL BUFFER AND FILL IT RECORD BY RECORD */
    i = 0;
    while(nl > 0) {
	if(MP_next(mp, 1) != 0) {
	    if(i == 0) SCR_beep();
	    MP_redisplay(mp);
	    return(0);
	    }
	i = 1;
	MP_shift_buf_down(mp);
	MP_isam_to_buf(mp);
	nl--;
	}

    MP_redisplay(mp);
    return(0);
}


/*NH*/
/* =+=== MP_CMP_TO_BUF ==============================================
    Compare the isam record to the record rec_nb in buf
    Return -1 if different, 0 else
     mp : IO : MPAGE *
     rec_nb : IN : nb of record in buffer to be compared
   ================================================================= */

MP_cmp_to_buf(mp, rec_nb)
MPAGE   *mp;
int     rec_nb;
{
    U_ch    *rec;
    ISAM    *is = (ISAM *)SCR_PTR(mp->mp_isam);
    ISFIELD *isf = is->is_flds;
    int     i;
    float   f1, f2;

    rec = mp->mp_buf + rec_nb * mp->mp_tot_len_rec;

    for(i = 0 ; i < is->is_nb_flds ; i++, isf++)
	if(IS_cmp_rec(is->is_rec + isf->isf_start, rec + isf->isf_start, isf->isf_type, isf->isf_length) != 0) {
	    Debug("MP: fld %d different\n", i);
	    return(-1);
	}
    return(0);
}


/*
   Permet de faire exécuter la display_fn de la lpg d'une MPAGE avant de
   copier le contenu du record de la page dans le buffer de le MPAGE
*/

int     SCR_MP_EXEC_LPG_DISP_ACT = 0; /* BP_M 01-11-1999 */

/*NH*/
/* =+=== MP_ISAM_TO_BUF ============================================
    Copy the isam record in buffer and lpgs at the current position
     mp : IO : MPAGE *
   ================================================================= */

MP_isam_to_buf(mp)
MPAGE   *mp;
{
    PAGE    *lpg = (PAGE *)SCR_PTR(mp->mp_lpg);
    ISAM    *is = (ISAM *)SCR_PTR(mp->mp_isam), *isj;
    int     i, len;

    memcpy(mp->mp_buf + mp->mp_c_rec * mp->mp_tot_len_rec,
	   is->is_rec,
	   is->is_rec_len);
    mp->mp_buf_def[mp->mp_c_rec] = 'Y';
    len = is->is_rec_len;
    for(i = 0 ; i < is->is_nb_join ; i++) {
	isj = (ISAM *)SCR_PTR(is->is_join[i].is);
	memcpy(mp->mp_buf + mp->mp_c_rec * mp->mp_tot_len_rec + len,
	       isj->is_rec,
	       isj->is_rec_len);
	len += isj->is_rec_len;
    }

    /* COPY PAGE CONTENTS TO LPGS */
    if(mp->mp_lpg_mem) {
	SCR_isam_to_page(lpg, is);
	if(SCR_MP_EXEC_LPG_DISP_ACT) ACT_exec_nu(lpg->pg_disp_act);   /* BP_M 01-11-1999 */
	memcpy(mp->mp_lpgs + mp->mp_lg_lpg * mp->mp_c_rec,
	       lpg->pg_rec,
	       lpg->pg_rec_len);
	}

    return(0);
}
/*NH*/
/* =+=== MP_retrieve_rec =========================================
    Retrieve old rec before saving modified page
     mp     : IO : MPAGE *
   ================================================================= */

MP_retrieve_rec(mp)
MPAGE   *mp;
{
    char    *rec;
    ISAM    *is = (ISAM *)SCR_PTR(mp->mp_isam), *isj;
    int     len, i;

    if(mp->mp_c_line < 0) return(0);
    rec = mp->mp_buf + (mp->mp_line0 + mp->mp_c_line) * mp->mp_tot_len_rec;
    memcpy(is->is_rec, rec, is->is_rec_len);
    len = is->is_rec_len;
    for(i = 0 ; i < is->is_nb_join ; i++) {
	isj = (ISAM *)SCR_PTR(is->is_join[i].is);
	rec = mp->mp_buf + (mp->mp_line0 + mp->mp_c_line) * mp->mp_tot_len_rec + len;
	memcpy(isj->is_rec, rec, isj->is_rec_len);
	len += isj->is_rec_len;
    }

    return(0);
}

/* ======================================================================
Reconstruit le buffer des records affichés dans une MPAGE. Cette fonction
doit être utilisée après création, modification ou destruction de records
en cours d'utilisation d'une MPAGE.

Par exemple, si une touche fonction demande la construction d'un record (en
C), l'éditeur de MPAGE ne peut en être informé. Cette fonction force la
reconstruction de la fenêtre de la MPAGE.

&NO Cette fonction doit être utilisée avec précaution.
------------------------------------------------------------------------- */
MP_rebuild_buf(mp)
MPAGE   *mp;
{
    int     i;
    int     nb;

    /* REPOSITION ON FIRST KNOWN RECORD */
    if(MP_reposition(mp, 0) != 0) {
	MP_read_screen(mp, mp->mp_ref_rec);
	return(0);
	}
    mp->mp_c_rec = 0;
    mp->mp_buf_def[0] = 'Y';

    /* NO MORE KNOWN RECORDS */
    for(i = 1 ; i < mp->mp_buf_size ; i++)
	mp->mp_buf_def[i] = 'N';


    /* NEXT TO END OF SCREEN */
    nb = mp->mp_line0 + mp->mp_nb_recs;
    for(i = 1 ; i < nb ; i++) {
	if(MP_next(mp, 1) != 0) break;
	mp->mp_buf_def[i] = 'Y';
	mp->mp_c_rec++;
	MP_isam_to_buf(mp);
	}

    /* ADJUST C_LINE AND LINE0 */
    if(mp->mp_buf_def[mp->mp_line0 + mp->mp_c_line] != 'Y') {
	if(mp->mp_c_line != 0) mp->mp_c_line--;
	else mp->mp_line0--;
	}
    return(0);
}

#ifdef MS_MOUSE

/*NH*/
MP_mouse_in_lpg(mp)
MPAGE    *mp;
{
    PAGE    *lpg   = (PAGE *)SCR_PTR(mp->mp_lpg);
    PAGE    *apg   = (PAGE *)SCR_PTR(mp->mp_apg);
    int     line   = mp->mp_line,
	    col    = lpg->pg_column,
	    lpg_nl = mp->mp_lpg_nl,
	    nl     = lpg_nl * mp->mp_nb_recs,
	    nc     = lpg->pg_nb_column,
	    mx     = MS_C,
	    my     = MS_L,
	    aline, acol;

    if(PG_closebox_pos(apg, &aline, &acol) == 0 &&
	   MS_L == aline && MS_C == acol) return(-4);
    if(mx < col || mx > col + nc) return(-1);
    if(my < line - 1) return(-1);
    if(my == line - 1) return(-2);
    if(my > line + nl) return(-1);
    if(my == line + nl) return(-3);
    return((my - line) / lpg_nl);
}

/*NH*/
MP_mouse_event(mp)
MPAGE   *mp;
{
    int     i, j, key;

    if(SCR_app_mouse()) return(0);      /* 4.02 */
    if(MS_E != MS_PRESS) return(0);
    if(SCR_scmt_mouse()) return(0);
    i = MP_mouse_in_lpg(mp);
    if(i == mp->mp_c_line) SCR_record_key(SCR_ENTER);
    else if(i == -1) SCR_scmt_menu(MS_L, MS_C);
    else if(i == -2) SCR_record_key(SCR_CSR_PG_UP);
    else if(i == -3) SCR_record_key(SCR_CSR_PG_DN);
    else if(i == -4) SCR_PG_ABORT = 1;
    else {
	key = (mp->mp_c_line > i) ? SCR_CSR_UP : SCR_CSR_DOWN;
	i = abs(mp->mp_c_line - i);
	for(j = 0 ; j < i ; j++) SCR_record_key(key);
	}
    return(0);
}
#endif

/*NH*/
SC_set_field_FF(is, fld_nb)        /* JMP_M 4.19 31-05-95 */
ISAM        *is;
int         fld_nb;
{
    float       buff = 1E37;  /* MAX FLOAT JMP */
    double      bufd = 1E300; /* MAX DOUBLE JMP */
    ISFIELD     *isf = is->is_flds + fld_nb;
    U_ch        *rec = is->is_rec + isf->isf_start;
    int         lg = isf->isf_length;

    memset(rec, 0xFF, lg);
    isf = is->is_flds + fld_nb;
    switch(isf->isf_type) {
	case SCR_ZSTRING : case SCR_STRING :
	    break;
	case SCR_CHAR : case SCR_SHORT1:
	    break;
	case SCR_SHORT :  case SCR_AUTO : case SCR_REF  :
	case SCR_LONG :   case SCR_DATE : case SCR_TIME :
	    rec[0] = 0x7F;
	    break;
	case SCR_FLOAT :
	    memcpy(rec, &buff, sizeof(float));
	    break;
	case SCR_DOUBLE :
	    memcpy(rec, &bufd, sizeof(double));
	    break;
	default :
	    break;
	}
    return(0);
}

/*NH*/
SC_set_rec_FF(is)        /* JMP_M 4.19 31-05-95 */
ISAM        *is;
{
    int         i;

    for(i = 0 ; i < is->is_nb_flds ; i++)
	SC_set_field_FF(is, i);
    return(0);
}

/*NH*/
SC_set_cmp_FF(is, cmp_nb, to_def)        /* JMP_M 4.19 31-05-95 */
ISAM        *is;
int         cmp_nb;
char        *to_def;
{
    int         i;
    COMPOSITE   *cmp;       /* BP_M 25-10-95 */

    if(cmp_nb < 0) return(0);
    cmp = is->is_cmps + cmp_nb;
    for(i = 0 ; i < cmp->cmp_nb_flds ; i++) {
	if(!to_def[cmp->cmp_flds[i]]) continue;
	SC_set_field_FF(is, cmp->cmp_flds[i]);
	}
    return(0);
}

/* ====================================================================
Remplace l'isam is1 par l'isam is2 dans tous les champs des pages de la
MPAGE (APG, SPG, BPG, LPG).

&SA MP_set_index(), MP_set_ref(), MP_set_cond()
======================================================================= */

MP_set_isam(mp, is1, is2)
MPAGE   *mp;
ISAM    *is1, *is2;
{
    if(mp->mp_apg) PG_set_isam(PAGE_PTR(mp->mp_apg), is1, is2);
    if(mp->mp_bpg) PG_set_isam(PAGE_PTR(mp->mp_bpg), is1, is2);
    if(mp->mp_lpg) PG_set_isam(PAGE_PTR(mp->mp_lpg), is1, is2);
    if(mp->mp_spg) PG_set_isam(PAGE_PTR(mp->mp_spg), is1, is2);
    if(mp->mp_isam == SCR_NU(is1))
	mp->mp_isam = SCR_NU(is2);
    return(0);
}


/* ====================================================================
Remplace l'index de recherche de la MPAGE. Si la valeur de cmp_nb
est négative, l'affichage se fera dans l'ordre physique de la base.

&SA MP_set_ref(), MP_set_cond(), MP_set_isam()
======================================================================= */

MP_set_index(mp, cmp_nb)
MPAGE   *mp;
int     cmp_nb;
{
    mp->mp_index = cmp_nb;
    return(0);
}

/* ====================================================================
Remplace la condition de recherche de la MPAGE. cond vaut SCR_GE, SCR_LE,
SCR_EQ, SCR_EQ1, ... Dans le cas de SCR_LE, l'ordre d'affichage sera
inversé.

&SA MP_set_index(), MP_set_ref(), MP_set_isam()
======================================================================= */

MP_set_cond(mp, cond)
MPAGE   *mp;
int     cond;
{
    mp->mp_cond = cond;
    return(0);
}

