#include "scr.h"

int MP_SEARCH_LWHERE;  /* BP_MODIF 29-10-98 */
int MP_NO_RESEARCH; /* BP_M 22-11-1998 */

int     MP_STOPPED = 0;

MP_view_pg(mp, bpg, is)
MPAGE   *mp;
PAGE    *bpg;
ISAM    *is;
{
    return(MP_view_add_modify_pg(mp, bpg, is, 0));
}

MP_modify_pg(mp, bpg, is)
MPAGE   *mp;
PAGE    *bpg;
ISAM    *is;
{
    return(MP_view_add_modify_pg(mp, bpg, is, 2));
}

MP_create_pg(mp, bpg, is)
MPAGE   *mp;
PAGE    *bpg;
ISAM    *is;
{
    return(MP_view_add_modify_pg(mp, bpg, is, 1));
}

/*NH*/
MP_view_add_modify_pg(mp, bpg, is, vam) /* vam= 0:view, 1:add, 2:modify*/
MPAGE   *mp;
PAGE    *bpg;
ISAM    *is;
int     vam;
{
    int     mod = 0, lkey, rc;
    ISAM    *mpis = (ISAM *)SCR_PTR(mp->mp_isam);
    PAGE    *apg = PAGE_PTR(mp->mp_apg);

GMSGC;
    /* BP_M 20-02-2012 09:36 */
    if(vam == 0 && mp->mp_bpg_view)
	bpg = (PAGE *)SCR_PTR(mp->mp_bpg_view);
    if(vam == 2 && mp->mp_bpg_mod)
	bpg = (PAGE *)SCR_PTR(mp->mp_bpg_mod);
    /* BP_M 20-02-2012 09:37 */

    PG_reset(bpg);
    if(vam != 1) {  /* VIEW OU MODIFY */
	memcpy(is->is_rec, mpis->is_rec, is->is_rec_len);
	SCR_isam_to_page(bpg, is);
	}
    else {
	if(mp->mp_use_ref) { /* ADD */
	    memcpy(is->is_rec, mp->mp_ref_rec, is->is_rec_len);
	    SCR_isam_to_page(bpg, is);
	    }
	else {
	    memset(is->is_rec, 0, is->is_rec_len);
	    }
	}

#ifndef WINDOWS
    PG_display_im(bpg);
    PG_enable(bpg, -2);
#endif
    mp->mp_operation = vam;
GMSGC;
    if(vam == 0) {
	GMSGC;
	PG_display_slide(bpg);  /* BP_M 11-01-2012 18:18 */
//      PG_disable(apg);
//      SCR_clear_key();
//      SCR_get_key();
//      PG_enable(apg, -2);
	GMSGC;
	}
    else while(1) {

#ifdef WINDOWS
	if(PG_edit_window(bpg)) goto fin;  /* JMP 21-08-97 */
#else
	PG_disable(PAGE_PTR(mp->mp_apg));
GMSGC;
	rc = PG_edit(bpg);
	PG_enable(PAGE_PTR(mp->mp_apg), -1);
	if(rc) goto fin;
#endif

	if(SCR_PTR_TYPE(bpg) == SCR_TYPE_TPAGES) /* JMP 21-08-97 */
	    lkey = ((TPAGES *)bpg)->tp_last_key; /* JMP 21-08-97 */
	else                                     /* JMP 21-08-97 */
	    lkey = bpg->pg_last_key;             /* JMP 21-08-97 */

	switch(lkey) {
	    case SCR_A_F4   :
	    case SCR_F3     :
	    case SCR_ESCAPE : goto fin;
	    case SCR_F10    :
		SCR_page_to_isam(bpg, is);
		memcpy(mpis->is_rec, is->is_rec, mpis->is_rec_len);
		if(vam == 1 && !mp->mp_save_rec) {mod = 1; goto fin;}   /* BP_M 20-02-2012 09:39 */
		if(vam == 1 && IS_write(mpis) == 0) {mod = 1; goto fin;}
		if(vam == 2 && IS_rewrite(mpis) == 0) {mod = 1; goto fin;}
		PG_display_error(IS_error_text());
		break;
	    default :
		if(SCR_app_key()) goto fin;
	    }
	}
fin:

#ifndef WINDOWS
    PG_undisplay(bpg);
#endif
    mp->mp_operation = 0;
    if(mod == 0) return(0);

    if(vam == 1) { /* ADD */
	mp->mp_c_rec = -1;
	if(mp->mp_c_line < 0) {
	    MP_redisplay(mp);
	    return(0);
	    }
	MP_rebuild_buf(mp);
	MP_display_screen(mp, 0, mp->mp_nb_recs - 1);
	}

    else MP_adjust_bufs(mp); /* MODIFY */

    return(mod);
}

/*NH*/
MP_view_modify(mp, view)   /* view = 0 ==> view, 2 ==> modify */
MPAGE   *mp;
int     view;
{
    int     mod, type;
    ISAM    *is;
    PAGE    *bpg;

    if(mp->mp_c_line < 0) return(-1);
    if(MP_check_repos(mp)) return(-1);
    MP_get_view(mp, &bpg, &is, &type, 0);
    if(bpg == 0) return(-1);

    if(type == 0) MP_view_add_modify_pg(mp, bpg, is, view);
    else          MP_view_modify_bc(mp, (BC *)bpg, view);

    return(0);
}
/*NH*/
MP_add(mp)
MPAGE   *mp;
{
    int     mod, type;
    ISAM    *is;
    PAGE    *bpg;

    if(MP_get_view(mp, &bpg, &is, &type, 1) < 0) return(-1);
    if(bpg == 0) return(-1);

    if(type == 0) MP_create_pg(mp, bpg, is);
    else          MP_create_bc(mp, (BC *)bpg);

    if(SCR_MP_REDISPLAY) {
	SCR_MP_REDISPLAY = 0;
	MP_home(mp);
    }

    return(0);
}

/*NH*/
MP_adjust_bufs(mp)
MPAGE   *mp;
{
    /* CHECK IF INDEX MODIFIED */
    if(MP_is_rec_ok(mp) || MP_cmp_index(mp) != 0) { /* JMP_M 4.19 13-06-95 */
	/* IF YES, REDISPLAY */
	mp->mp_c_rec = -1;
	if(mp->mp_line0 + mp->mp_c_line == 0) {
	    mp->mp_c_line = -1;
	    MP_redisplay(mp);
	    return(0);
	    }
	if(SCR_MPRETRIEVE_REC) {    /* BP_M 19-10-1999 */
	    MP_rebuild_buf(mp);
	    MP_display_screen(mp, 0, mp->mp_nb_recs - 1);
	    }
	}
    else {
	/* IF NO, CONTINUE */
	MP_isam_to_buf(mp);
	MP_display_lpg(mp, mp->mp_c_line, 1);
	}
    return(0);
}

/* =+=== MP_CMP_INDEX ==============================================
    Compare the index in the isam record and in the buffer
    Return -1 if different, 0 else
     mp : IO : MPAGE *
   ================================================================= */

MP_cmp_index(mp)
MPAGE   *mp;
{
    int         i, lg, start;
    char        *rec, *is_rec;
    COMPOSITE   *cmp;
    ISAM        *is  = (ISAM *)SCR_PTR(mp->mp_isam);

    if(mp->mp_index < 0) return(0); /* JMP_M 4.19 01-06-95 */
    rec    = mp->mp_buf + mp->mp_c_rec * mp->mp_tot_len_rec,
    is_rec = is->is_rec;

    cmp =  is->is_cmps + mp->mp_index;
    for(i = 0 ; i < cmp->cmp_nb_flds ; i++) {
	lg    = (is->is_flds + cmp->cmp_flds[i])->isf_length;
	start = (is->is_flds + cmp->cmp_flds[i])->isf_start;
	if(memcmp(rec + start, is_rec + start, lg)) return(-1); /* BP_MODIF 29-10-98 */
	}
    return(0);
}
/**/
/* =+=== MP_DELETE =====================================================
    Delete the current record and redisplay the screen

     mp     : IO : MPAGE *
    Return : 0 if found, -1 if not found
   ================================================================= */

MP_delete(mp)
MPAGE   *mp;
{
    ISAM    *is = ISAM_PTR(mp->mp_isam);

    if(mp->mp_c_line < 0)
	return(-1);

    /* REPOSITION ON CURRENT LINE */
    if(MP_check_repos(mp)) return(-1);

    /* DELETE IN ISAM FILE */
    memcpy(is->is_rrec, is->is_rec, is->is_rec_len);
    if(IS_delete(is) != 0) {
	PG_display_error(IS_error_text());
	return(-1);
	}

    if(is->is_type_dbd == 0) mp->mp_c_rec = -1;

    /* DELETE IN BUFFER */
    MP_del_buf(mp);

    /* IF NO MORE KNOWN RECORD, REDISPLAY */
    if(mp->mp_buf_def[0] != 'Y') {
	mp->mp_c_line = -1;
	MP_redisplay(mp);
	return(0);
	}

    /* IF SCREEN FULL, REDISPLAY */
    if(mp->mp_buf_def[mp->mp_line0 + mp->mp_nb_recs - 1] == 'Y') {
	MP_display_screen(mp, mp->mp_c_line, mp->mp_nb_recs - 1);
	return(0);
	}

    /* IF LINE0 > 0, DECREMENT LINE0 AND REDISPLAY */
    if(mp->mp_line0 > 0) {
	mp->mp_line0--;
	MP_display_screen(mp, 0, mp->mp_c_line);
	return(0);
	}

    /* ELSE TRY TO FILL THE SCREEN AND REDISPLAY */
    MP_fill_screen(mp, 0);
    if(mp->mp_buf_def[mp->mp_c_line] != 'Y') {
	mp->mp_c_line--;
	mp->mp_c_rec--;  /* JMP 02-12-98 */
	}
    MP_display_screen(mp, mp->mp_c_line, mp->mp_nb_recs - 1);
/*
    if(is->is_type_dbd)         /* BP_M 24-11-1998
	MP_check_repos(mp);
*/
//  if(MP_prev(mp, 0) != 0) MP_next(mp, 0);

    return(0);
}
/**/
/* =+=== MP_RE_SEARCH ===============================================
    Re-position the file on a known record of the file (rec_nb of
    the buffer. If there is no known record in the buffer, rebuild
    from the beginning of the file. Used by MP_reposition.

     mp     : IO : MPAGE *
     rec_nb : IN : nb of the record in the buffer
     Return : 0 if found, -1 if not found
   ================================================================= */

MP_re_search(mp, rec_nb)
MPAGE   *mp;
int     rec_nb;
{
    ISAM    *is  = ISAM_PTR(mp->mp_isam);       /* BP_M 23-11-1998 */
    char    *rec;
    int     i;

    mp->mp_c_rec = -1;

    /* COPY REC TO ISAM */
    rec = mp->mp_buf + rec_nb * mp->mp_tot_len_rec;
    memcpy(is->is_rec, rec, is->is_rec_len);

    /* SEARCH FIRST RECORD */
    if(MP_search(mp) != 0) {
	mp->mp_c_line = -1;
	return(-1);
	}

    /* NEXT TO SEARCHED RECORD */
    while(1) {
	if(MP_cmp_to_buf(mp, rec_nb) == 0) {
	    mp->mp_c_rec = rec_nb;
	    return(0);
	    }
	if(MP_next(mp, 1) != 0) {
	    mp->mp_c_line = -1;
	    return(-1);
	    }
	}
}
/**/
/* =+=== MP_REPOSITION ============================================
    Reposition on a record already known
     mp : IO : MPAGE *
     rec_nb : IN : nb of the record in buffer
   ================================================================= */

MP_reposition(mp, rec_nb)
MPAGE   *mp;
int     rec_nb;
{
    int     i, steps;
    PAGE    *lpg = (PAGE *)SCR_PTR(mp->mp_lpg);

    /* IF NO CURRENT RECORD, RE SEARCH */
beg:
    if(mp->mp_c_rec < 0) return(MP_re_search(mp, rec_nb));

    /* ELSE PREV OR NEXT */
    if(mp->mp_c_rec == rec_nb) goto setlpg;
    if(mp->mp_c_rec > rec_nb) { /* PREVIOUS */
	steps = mp->mp_c_rec - rec_nb;
	if(IS_def_prev((ISAM *)SCR_PTR(mp->mp_isam)) == 0) { /* JMP 02-11-98 */
	    mp->mp_c_rec = -1;
	    goto beg;
	    }

	for(i = 0 ; i < steps ; i++) {    /* BP_M 30-07-2007 */
	    if(MP_prev(mp, 1) != 0) {
		mp->mp_c_rec = -1;
		mp->mp_c_line = -1;
		return(-1);
		}
	    mp->mp_c_rec--;
	    }
	}

    else { /* NEXT */
	steps = rec_nb - mp->mp_c_rec;
	for(i = 0 ; i < steps ; i++) {
	    if(MP_next(mp, 1) != 0 ) {
		mp->mp_c_rec = -1;
		mp->mp_c_line = -1;
		return(-1);
		}
	    mp->mp_c_rec++;
	    }
	}

    /* IF RECORD FOUND != THIS IN BUFFER : ERROR */
    if(MP_cmp_to_buf(mp, rec_nb) != 0) {
	mp->mp_c_rec = -1;
	mp->mp_c_line = -1;
	return(-2);
	}

setlpg:
    memcpy(lpg->pg_rec,
	mp->mp_lpgs + mp->mp_lg_lpg * mp->mp_c_rec,
	lpg->pg_rec_len);

    return(0);
}

/* =+=== MP_DEL_BUF ================================================
    Delete te current line in the buffer
     mp : IO : MPAGE *
   ================================================================= */

MP_del_buf(mp)
MPAGE   *mp;
{
    char    *rec;
    int     lg, lg_lpg, i;
    ISAM    *is  = (ISAM *)SCR_PTR(mp->mp_isam);
    PAGE    *lpg = (PAGE *)SCR_PTR(mp->mp_lpg);

    lg = mp->mp_tot_len_rec;
    lg_lpg = lpg->pg_rec_len;
    for(i = mp->mp_line0 + mp->mp_c_line ; i < mp->mp_buf_size - 1 ; i++) {
	mp->mp_buf_def[i] = mp->mp_buf_def[i + 1];
	rec = mp->mp_buf + i * lg;
	memcpy(rec, rec + lg, lg);
	if(mp->mp_lpg_mem == 0) continue;       /* JMP_M 4.20 16-06-95 */
	rec = mp->mp_lpgs + i * lg_lpg;
	memcpy(rec, rec + lg_lpg, lg_lpg);
	}

    mp->mp_buf_def[mp->mp_buf_size - 1] = 'N';
    return(0);
}
/**/
/* =+=== MP_SHIFT_BUF_UP ============================================
    Shift the buffer to see one more line up.
     mp : IO : MPAGE *
   ================================================================= */

MP_shift_buf_up(mp)
MPAGE   *mp;
{
    char    *rec;
    int     lg, lg_lpg, i, j;

    lg = mp->mp_tot_len_rec;
    lg_lpg = ((PAGE *)SCR_PTR(mp->mp_lpg))->pg_rec_len;
    for(i = mp->mp_buf_size - 1 ; i > 0 ; i--) {
	mp->mp_buf_def[i] = mp->mp_buf_def[i - 1];
	rec = mp->mp_buf + i * lg;
	memcpy(rec, rec - lg, lg);
	if(mp->mp_lpg_mem == 0) continue;       /* JMP_M 4.20 16-06-95 */
	rec = mp->mp_lpgs + i * lg_lpg;
	memcpy(rec, rec - lg_lpg, lg_lpg);
	}
    mp->mp_buf_def[0] = 'N';

    return(0);
}
/**/
/* =+=== MP_SHIFT_BUF_DOWN ============================================
    Shift the buffer to see one more line down.
     mp : IO : MPAGE *
   ================================================================= */

MP_shift_buf_down(mp)
MPAGE   *mp;
{
    char    *rec;
    int     lg, lg_lpg, i, j;

    lg = mp->mp_tot_len_rec;
    lg_lpg = ((PAGE *)SCR_PTR(mp->mp_lpg))->pg_rec_len;
    for(i = 0 ; i < mp->mp_buf_size - 1 ; i++) {
	mp->mp_buf_def[i] = mp->mp_buf_def[i + 1];
	rec = mp->mp_buf + i * lg;
	memcpy(rec, rec + lg, lg);
	if(mp->mp_lpg_mem == 0) continue;       /* JMP_M 4.20 16-06-95 */
	rec = mp->mp_lpgs + i * lg_lpg;
	memcpy(rec, rec + lg_lpg, lg_lpg);
	}
    mp->mp_buf_def[mp->mp_buf_size - 1] = 'N';
    return(0);
}

/* =+=== MP_IS_REC_OK ============================================
    Check the criteria on a the current record
     mp : IO : MPAGE *
   =============================================================== */

MP_is_rec_ok(mp)
MPAGE   *mp;
{
    int     rc;

    rc = MP_is_spg_ok(mp);
    if(rc) return(rc);
    return(ACT_exec_nu(mp->mp_crit));
}

MP_is_spg_ok(mp)
MPAGE   *mp;
{
    ISAM    *is = (ISAM *)SCR_PTR(mp->mp_isam);
    PAGE    *spg = (PAGE *)SCR_PTR(mp->mp_spg);
    int     rc = 0;

    if(is->is_type_dbd > 0) return(0); /* JMP 02-11-98 */
    if(mp->mp_spg == 0) return(0);
    if(SC_check_index(is, spg->pg_scan->to, spg->pg_scan->to_def, mp->mp_index, 1)) rc = -1;
    else if(SC_check_index(is, spg->pg_scan->from, spg->pg_scan->from_def, mp->mp_index, -1)) rc = -1;
    else if(SC_check_rec(is, spg->pg_scan->from, spg->pg_scan->to, spg->pg_scan->from_def, spg->pg_scan->to_def)) rc = 1;
    return(rc);
}

/* =+=== MP_CMP_FLDS ============================================
    Compare two fields of isam records
     is_rec : IN : isam field
     rec    : IN : reference rec
     lg     : IN : length
   ===============================================================

MP_cmp_flds(is_rec, rec, lg)
char    *is_rec;
char    *rec;
int     lg;
{
    return(memcmp(is_rec, rec, lg));
}

/* JMP_M 4.19 19-05-95 */

/* =+=== MP_SEARCH ============================================
    Search a record and check criteria
     mp : IN : MPAGE *
     return : -1 if not found, 0 else
   =============================================================== */
MP_search(mp)   /* BP_M 16-07-1998 */
MPAGE   *mp;
{
    int     rc, cond, fld;
    short   val;
    ISAM    *is  = (ISAM *)SCR_PTR(mp->mp_isam);
    PAGE    *spg = (PAGE *)SCR_PTR(mp->mp_spg);
    PAGE    *apg = (PAGE *)SCR_PTR(mp->mp_apg);
    extern int MP_SEARCH_LWHERE;   /* BP_MODIF 21-09-98 */

    if(MP_NO_RESEARCH && apg->pg_nb_flds > 0 && !is->is_query)  /* JMP 08-11-99 */
	MP_NO_RESEARCH = 0;
    else {
	/* BP_M 27-02-1998 */
	if(is->is_type_dbd > 0) { /* JMP 02-11-98 */
	    if(!MP_SEARCH_LWHERE) {
		cond = mp->mp_cond;
		if(spg != 0) {
		    cond += SCR_MINMAX;
		    memcpy(is->is_maxrec, spg->pg_scan->to, is->is_rec_len);
		    if(IS_search(is, mp->mp_index, cond, (char *)0) != 0)
			return(-1);
		}
		else {
		    /* BP_M 18-01-2013 18:06 */
		    if(mp->mp_cond == SCR_LE) {
			fld = is->is_cmps[mp->mp_index].cmp_flds[0];
			if(is->is_flds[fld].isf_type == SCR_SHORT) {
			    val = 32000;
			    IS_set_field(is, fld, &val);
			}
		    }
		    /* BP_M 18-01-2013 18:06 */
		    if(IS_search(is, mp->mp_index, mp->mp_cond, (char *)0) != 0)
			return(-1);
		}
	    }
	    else {
		IS_resync(is);                              /* JMP 02-12-99 */
		if(IS_search_lwhere(is) != 0) return(-1);
		if(IS_search_ph_end(is, -1)) return(-1);    /* JMP 02-12-99 */
	    }
	}
	else {
	    if(IS_search(is, mp->mp_index, mp->mp_cond, (char *)0) != 0)
		return(-1);
	}
    }
    switch(MP_is_rec_ok(mp)) {
	case 0  :
	    MP_count(mp, 1, 1);
	    return(0);
	case -1 :
	    MP_count(mp, 0, 0);
	    return(-1);
	default :
	    mp->mp_in_search = 1;
	    MP_count(mp, 1, 0);
	    rc = MP_next(mp, 0);
	    mp->mp_in_search = 0;
	    return(rc);
	}
}

/**/
/* =+=== MP_NEXT ============================================
    Search the next record and check criteria
     mp : IN : MPAGE *
     return : -1 if not found, 0 else
   =============================================================== */
MP_next(mp, recur)
MPAGE   *mp;
int     recur;
{
    int     rc, prev = 0;

    while(1) {
	if(IS_next(ISAM_PTR(mp->mp_isam)) != 0) {
	    if(prev && mp->mp_in_search == 0 && recur) MP_prev(mp, 0);
	    return(-1);
	    }

	switch(MP_is_rec_ok(mp)) {
	    case 0  :
		MP_count_add(mp, 1, 1);
		return(0);
	    case -1 :
		MP_count_add(mp, 1, 0);
		if(mp->mp_in_search == 0) MP_prev(mp, 0);
		return(-1);
	    default :
		if(MP_check_key()) return(-1);
		MP_count_add(mp, 1, 0);
		prev = 1;
		break;
	    }
	}
}

/**/
/* =+=== MP_PREV ============================================
    Search the previous record and check criteria
     mp : IN : MPAGE *
     return : -1 if not found, 0 else
   =============================================================== */
MP_prev(mp, recur)
MPAGE   *mp;
int     recur;
{
    int     rc, next = 0;

    while(1) {
	if(IS_prev(ISAM_PTR(mp->mp_isam)) != 0) {
	    if(next && recur) MP_next(mp, 0);
	    return(-1);
	    }

	switch(MP_is_rec_ok(mp)) {
	    case 0  :
		MP_count_add(mp, -1, 1);
		return(0);
	    case -1 :
		MP_count_add(mp, -1, 0);
		MP_next(mp, 0);
		return(-1);
	    default :
		if(MP_check_key()) return(-1);
		MP_count_add(mp, -1, 0);
		next = 1;
		break;
	    }
	}
}

MP_check_key()
{
    int     key, vtime = SCR_vtime;

    SCR_term_vkey(0);
    if(SCR_check_key()) {                       /* JMP 29-01-98 */
       key = SCR_get_key();                     /* JMP 29-01-98 */
       if(key == SCR_ESCAPE || key == SCR_F3) { /* JMP 29-01-98 */
	    SCR_term_vkey(vtime);
	    SCR_record_key(SCR_ESCAPE);
	    MP_STOPPED = 1;
	    return(-1);
	    }
       }
    SCR_term_vkey(vtime);
    return(0);
}


/*
IMAGE *MP_save(mp)
MPAGE   *mp;
{
    int     line,
	    col,
	    ml,
	    mc;
    PAGE    *lpg = (PAGE *)SCR_PTR(mp->mp_lpg);
    PAGE    *apg = (PAGE *)SCR_PTR(mp->mp_apg);

    line = mp->mp_line;
    col  = lpg->pg_column;
    ml   = line + mp->mp_lpg_nl * mp->mp_nb_recs;
    mc   = col + lpg->pg_nb_column;

    if(mp->mp_apg != 0) {
	line = _min_(line, apg->pg_line);
	ml   = _max_(ml, apg->pg_line + apg->pg_nb_line);
	col  = _min_(col, apg->pg_column);
	mc   = _max_(mc, apg->pg_column + apg->pg_nb_column);
	}
    return(SCR_save_image(line, col, ml-line, mc-col));
}

MP_display(mp)
MPAGE   *mp;
{
    if(mp->mp_apg != 0) PG_display(SCR_PTR(mp->mp_apg));
    MP_redisplay(mp);
}
*/

/* =+=== MP_redisplay ==================================================
    Re-display a multi-page (read first the records if no current record)
    Display only the records, not the page around
    mp : IO : pointer to the multi-page
   =================================================================== */

MP_redisplay(mp)
MPAGE   *mp;
{
    ACT_exec_nu(mp->mp_link_fn);    /* BP_M 04-02-2013 16:10 */

    if(mp->mp_c_line < 0)
	MP_read_screen(mp, mp->mp_ref_rec);

    MP_display_screen(mp, 0, mp->mp_nb_recs - 1);
    return(0);
}
/* =+=== MP_display_screen ===========================================
    Display the screen of a multi-page
    mp : IO : pointer to the multi-page
    from : IN : first lpg to be displayed
    to   : IN : last little page to be displayed
   =================================================================== */

MP_display_screen(mp, from, to)
MPAGE   *mp;
int     from;
int     to;
{
    int     i;

    for(i = from ; i <= to ; i++)
	if(i != mp->mp_c_line) MP_display_lpg(mp, i, 0);
    if(mp->mp_c_line >= from && mp->mp_c_line <= to)
	MP_display_lpg(mp, mp->mp_c_line, 1);
    return(0);
}
/* =+=== MP_read_screen ===========================================
    Read the contents of the first screen of a multi-page
    beginning with a defined record
    mp : IO : pointer to the multi-page
    rec : IN : pointer to the prototype record (used to search)
   =================================================================== */

MP_read_screen(mp, rec)
MPAGE   *mp;
char    *rec;
{
    int     i;
    ISAM    *is = (ISAM *)SCR_PTR(mp->mp_isam);
    PAGE    *apg = PAGE_PTR(mp->mp_apg);

    mp->mp_line0 = 0;
    mp->mp_c_line = -1;
    mp->mp_c_rec  = -1;

//  if(mp->mp_buf_def == 0)
//      mp->mp_buf_def = SCR_malloc(mp->mp_buf_size);
    for(i = 0 ; i < mp->mp_buf_size ; i++)
	mp->mp_buf_def[i] = 'N';

    if(MP_NO_RESEARCH == 0 || apg->pg_nb_flds == 0)
	memcpy(is->is_rec, rec, is->is_rec_len);

    /* BP_M 01-10-97 */
    if(MP_search(mp) != 0) return(-1);

    mp->mp_c_line = 0;
    mp->mp_c_rec  = 0;
    mp->mp_buf_def[0] = 'Y';
    MP_isam_to_buf(mp);
    MP_display_lpg(mp, 0, 1); /* JMP_M 4.19 31-05-95 */

    return(MP_fill_screen(mp, 1));
}
/* =+=== MP_fill_screen ===========================================
    Fill the end of the screen (read the records if needed)
    mp : IO : pointer to the multi-page
   =================================================================== */

MP_fill_screen(mp, flag)
MPAGE   *mp;
int     flag;
{
    int     i;
    ISAM    *is = ISAM_PTR(mp->mp_isam);

    for(i = 1 ; i < mp->mp_nb_recs ; i++)
	if(mp->mp_buf_def[mp->mp_line0 + i] == 'N') break;
    if(i == mp->mp_nb_recs) return(0);

/*  if(is->is_type_dbd == 0 || mp->mp_c_rec <= 0) { /* BP_M 23-11-1998 */
    if(is->is_type_dbd == 0 || mp->mp_c_rec < 0) { /* JMP 02-12-98 */
	if(MP_reposition(mp, mp->mp_line0 + i - 1) != 0) {
	    MP_error();
	    MP_read_screen(mp, mp->mp_ref_rec);
	    return(-1);
	    }
    }
    else
	i = mp->mp_c_rec + flag;

    for( ; i < mp->mp_nb_recs ; i++) {
	if(MP_next(mp, 1) != 0) break;
	mp->mp_buf_def[i] = 'Y';
	mp->mp_c_rec = i;
	MP_isam_to_buf(mp);
	MP_display_lpg(mp, i, 0); /* JMP_M 4.19 31-05-95 */
	}


    return(0);
}

/* =+=== MP_DISPLAY_LPG ============================================
    Display the little page (position line of the screen)
    Invert the attributes if invert = 1 (not if only 1 lpage)
     mp : IO : MPAGE *
     line : IN : number of little page in the screen
     invert : 0 for no inversion, 1 else
   ================================================================= */

MP_display_lpg(mp, line, invert)
MPAGE   *mp;
int     line;
int     invert;
{
    PAGE    *lpg = (PAGE *)SCR_PTR(mp->mp_lpg);
    ISAM    *is, *isj;
    char    *rec;
    int     v_nb, i, len;

    lpg->pg_line = mp->mp_line + line * mp->mp_lpg_nl;

    /* EMPTY PAGE */
    if(mp->mp_buf_def[mp->mp_line0 + line] != 'Y') {
	MP_empty_lpg_dw(mp, lpg, line,
	    (line == 0 || mp->mp_buf_def[mp->mp_line0 + line - 1] == 'Y') ? '─' : ' ');
	return(0);
	}

    v_nb = MP_select_view(mp, line, &lpg, &is);
    if(v_nb == 0 && mp->mp_lpg_mem) {
	rec = mp->mp_lpgs + mp->mp_lg_lpg * (mp->mp_line0 + line);
	memcpy(lpg->pg_rec, rec, lpg->pg_rec_len);
	MP_copy_buf_to_isam(mp, line);
	}
    else {
	MP_copy_buf_to_isam(mp, line);
	SCR_isam_to_page(lpg, is);
	lpg->pg_line = mp->mp_line + line * mp->mp_lpg_nl;
	}


    MP_display_lpg_dw(mp, lpg, line, invert);
    return(0);
}

MP_copy_buf_to_isam(MPAGE *mp, int line)
{
    char    *rec;
    int     i, len;
    ISAM    *is, *isj;

    is = (ISAM *)SCR_PTR(mp->mp_isam);
    rec = is->is_rec;
    is->is_rec = mp->mp_buf + (mp->mp_line0 + line) * mp->mp_tot_len_rec;
    len = is->is_rec_len;
    for(i = 0 ; i < is->is_nb_join ; i++) {
	isj = (ISAM *)SCR_PTR(is->is_join[i].is);
	memcpy(isj->is_rec,
	       mp->mp_buf + (mp->mp_line0 + line) * mp->mp_tot_len_rec + len,
	       isj->is_rec_len);
	len += isj->is_rec_len;
    }
    is->is_rec = rec;
}


/* =+=== MP_ERROR ==================================================
    Display the error message "The file has been corrupted or
       modified by another user".
   ================================================================= */

MP_error()
{
    if(MP_STOPPED == 0) PG_display_error(SCR_err_txt(107));
    MP_STOPPED = 0;
    return(0);
}

char MP_counter[] = "|/-\\";
/*
#ifdef DOS
char MP_counter[] = ".oO0";
#else
char MP_counter[] = "░▒▓█";
#endif
*/

int MP_COUNT_FREQ  = 10;
int MP_COUNT_FORCE, MP_COUNT_POS = 0;

MP_count_add(mp, nbread, nbsel)
MPAGE   *mp;
int     nbread, nbsel;
{
    PAGE    *apg;
    FIELD   *fld;
    char    buf[81];

    mp->mp_count += nbread;
    if(MP_COUNT_FREQ <= 0) return(0);
    if(mp->mp_apg == 0) return(0);
    apg = (PAGE *)SCR_PTR(mp->mp_apg);
    if(apg->pg_fld_count == MAX_BFIELD12) return(0);
    fld = apg->pg_flds + apg->pg_fld_count;

    if(fld->fld_type == SCR_STRING) {
	if(MP_COUNT_FORCE == 0 && mp->mp_count % MP_COUNT_FREQ) return(0);
	MP_COUNT_FORCE = 0;
	if(nbread > 0) MP_COUNT_POS++;
	else           MP_COUNT_POS--;
	if(MP_COUNT_POS < 0) MP_COUNT_POS = 3;
	if(MP_COUNT_POS > 3) MP_COUNT_POS = 0;
	SCR_set_field_text(apg, apg->pg_fld_count, MP_counter + MP_COUNT_POS);
	}
    else
	SCR_set_field_long(apg, apg->pg_fld_count, mp->mp_count);

    PG_display_field(apg, apg->pg_fld_count);

    return(0);
}


MP_count(mp, nbread, nbsel)
MPAGE   *mp;
int     nbread, nbsel;
{
    mp->mp_count = nbread;
    MP_count_add(mp, 0, 0);
    return(0);
}

MP_check_repos(mp)
MPAGE   *mp;
{
    if(MP_reposition(mp, mp->mp_line0 + mp->mp_c_line) == 0) return(0);
    MP_error();
    MP_redisplay(mp);
    return(-1);
}


MP_select_view(mp, line, lpg, is)
MPAGE   *mp;
int     line;
PAGE    **lpg;
ISAM    **is;
{
    char    *is_rec;
    int     rc;

    *lpg = (PAGE *) SCR_PTR(mp->mp_lpg);
    *is = (ISAM *) SCR_PTR(mp->mp_isam);
    if(mp->mp_view_fn == 0) return(0);
    is_rec = (*is)->is_rec;
    (*is)->is_rec = mp->mp_buf + (mp->mp_line0 + line) * mp->mp_tot_len_rec;
    rc = ACT_exec_nu(mp->mp_view_fn);
    (*is)->is_rec = is_rec;
    if(rc <= 0 || rc > mp->mp_nb_views) return(0);
    *lpg = (PAGE *) SCR_PTR(mp->mp_views[rc - 1].v_lpg);
    *is = (ISAM *) SCR_PTR(mp->mp_views[rc - 1].v_isam);
    return(rc);
}

MP_get_view(mp, bpg, is, type, view_create)
MPAGE   *mp;
PAGE    **bpg;
ISAM    **is;
int     *type, view_create;
{
    int     rc, act;

    *type = 0;
    *bpg = (PAGE *) SCR_PTR(mp->mp_bpg);
    if(SCR_OBJ_TYPE(mp->mp_bpg) == SCR_TYPE_BC) *type = 1;
    *is = (ISAM *) SCR_PTR(mp->mp_isam);
    if(view_create == 0) act = mp->mp_view_fn;
    else                 act = mp->mp_view_crfn;
    if(act == 0) return(0);
    rc = ACT_exec_nu(act);
    if(rc < 0) return(-1);
    if(rc == 0 || rc > mp->mp_nb_views) return(0);
    *type = mp->mp_views[rc - 1].v_type;
    *bpg = (PAGE *) SCR_PTR(mp->mp_views[rc - 1].v_bpg);
    *is = (ISAM *) SCR_PTR(mp->mp_views[rc - 1].v_isam);
    return(rc);
}

MP_retr_apg(mp)
MPAGE   *mp;
{
    if(mp->mp_c_rec >= 0) { /* JMP_M 4.19 16-05-95 */
	MP_retrieve_rec(mp);
	if(mp->mp_apg != mp->mp_spg) {  /* JMP_M 4.19 02-06-95 */
	    SCR_isam_to_page(PAGE_PTR(mp->mp_apg), ISAM_PTR(mp->mp_isam));
	    PG_display_flds_no_cmp(PAGE_PTR(mp->mp_apg));
	    }
	}
    return(0);
}

/*
    Return : -1 : quit MP_edit
    sinon  : bit 0 : lostcmt
	     bit 1 : touche bouffée
	     bit 2 : action utilisateur imprévisible (record key ..)
*/

#define MPDEF_LOSTCMT 1  /* 0 0 1 */
#define MPDEF_KEYUSED 2  /* 0 1 0 */
#define MPDEF_FNKEY   4  /* 1 0 0 */

int SCR_MPRETRIEVE_REC = 1;  /* BP_M 16-10-1999 */

MP_edit_1key(mp, key, diff)
MPAGE   *mp;
int     key;
int     diff; /* JMP 02-03-03 */
{

    if(SCR_MP_FORCE_ADD) {  /* BP_M 15-02-2013 10:35 */
	SCR_MP_FORCE_ADD = 0;
	if(mp->mp_create == 0) return(MPDEF_KEYUSED);
	MP_add(mp);
	ACT_exec_nu(mp->mp_link_fn);
	return(MPDEF_LOSTCMT + MPDEF_KEYUSED);
    }

    MP_COUNT_FORCE = 1;
    if(SCR_app_key()) return(-1);
    if(FNK_is_fnk(FNKEYS_PTR(mp->mp_fnks), key) >= 0) {
	MP_fnkeys(mp, key);
	MP_COUNT_FORCE = 1;
	if(SCR_PG_ABORT == 1) {
	    if(SCR_MPRETRIEVE_REC)  /* BP_M 16-10-1999 */
		MP_retrieve_rec(mp);
	    SCR_MPRETRIEVE_REC = 1;
	    return(-1);
	}
	return(MPDEF_LOSTCMT + MPDEF_KEYUSED + MPDEF_FNKEY);
    }
    switch(key) {
#if !defined(SCRW32) && !defined(SCRGNOME)
	case ' ': MP_fnk_menu(mp); return(MPDEF_LOSTCMT + MPDEF_KEYUSED);
#endif
	case -2             : MP_move(mp, diff); return(MPDEF_KEYUSED);
	case SCR_S_CSR_UP   :
	case SCR_CSR_UP     : MP_up(mp);     return(MPDEF_KEYUSED);
	case SCR_C_CSR_UP   : MP_up_silent(mp); return(MPDEF_KEYUSED);
	case SCR_S_CSR_DOWN :
	case SCR_CSR_DOWN   : MP_down(mp);   return(MPDEF_KEYUSED);
	case SCR_C_CSR_DOWN : MP_down_silent(mp);  return(MPDEF_KEYUSED);
	case SCR_S_CSR_PG_UP:
	case SCR_CSR_PG_UP  : MP_pg_up(mp);  return(MPDEF_KEYUSED);
	case SCR_S_CSR_PG_DN:
	case SCR_CSR_PG_DN  : MP_pg_down(mp);return(MPDEF_KEYUSED);
	case SCR_S_CSR_HOME :
	case SCR_CSR_HOME   : MP_home(mp);   return(MPDEF_KEYUSED);
	case SCR_S_CSR_END  :
	case SCR_CSR_END    : MP_end(mp);    return(MPDEF_KEYUSED);

	case SCR_ENTER_1:
	    if(mp->mp_modify) {
		MP_view_modify(mp, 2);
		ACT_exec_nu(mp->mp_link_fn);
	    }
	    return(MPDEF_LOSTCMT + MPDEF_KEYUSED);

	case SCR_ENTER :
GMSGC;
	    if(mp->mp_modify) MP_view_modify(mp, 2);
	    else              MP_view_modify(mp, 0);
	    ACT_exec_nu(mp->mp_link_fn);
	    return(MPDEF_LOSTCMT + MPDEF_KEYUSED);

	case SCR_DELETE:
	    if(mp->mp_delete == 0) {
		SCR_beep();
		return(MPDEF_LOSTCMT + MPDEF_KEYUSED);
		}
	    if(CONFIRME) return(MPDEF_LOSTCMT + MPDEF_KEYUSED);

	case SCR_DELETE_1:
	    MP_delete(mp);
	    ACT_exec_nu(mp->mp_link_fn);
	    return(MPDEF_LOSTCMT + MPDEF_KEYUSED);

	case SCR_A_A:
	case SCR_INSERT :
	    if(mp->mp_create == 0) return(MPDEF_KEYUSED);
	case SCR_INSERT_1:
	    MP_add(mp);
	    ACT_exec_nu(mp->mp_link_fn);
	    return(MPDEF_LOSTCMT + MPDEF_KEYUSED);

	case SCR_F3 :
	case SCR_ESCAPE:
	case SCR_ESCAPE_1:
	    MP_retrieve_rec(mp);
	    return(-1);

	case SCR_HELP_KEY :
	    HLP_stack_edit();
	    return(MPDEF_LOSTCMT + MPDEF_KEYUSED);

#ifdef MS_MOUSE
	case SCR_MOUSE :
	    MP_mouse_event(mp);
	    if(SCR_PG_ABORT) return(-1);
	    return(MPDEF_LOSTCMT + MPDEF_KEYUSED); /* JMP 18-01-97 */
#endif
	default : return(0);
	}
}

    /* BP_M 07-10-97 */
MP_reset_apg(mp)
MPAGE   *mp;
{
    ISAM    *is = (ISAM *)SCR_PTR(mp->mp_isam);
    char    *rec = is->is_rec;

    is->is_rec = mp->mp_ref_rec;
    SCR_isam_to_page(PAGE_PTR(mp->mp_apg), is);
    is->is_rec = rec;
    PG_display_flds(PAGE_PTR(mp->mp_apg));
    return(0);
}


