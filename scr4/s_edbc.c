#include "scr.h"

long BC_get_count();
long SCR_STEP_BCCOUNT = 20L; /* BP_M 20-10-95 */

#define BCDEF_LOSTCMT 1  /* 0 0 1 */
#define BCDEF_KEYUSED 2  /* 0 1 0 */
#define BCDEF_FNKEY   4  /* 1 0 0 */


BC_edit_1key(bc, key, diff)
BC      *bc;
int     key, diff; /* JMP 05-03-03 */
{
	int     cline = bc->bc_line0 + bc->bc_c_line;

	if(BC_search_lbc(bc, cline) != 0) {

	    if(key != -1 &&
	      (key == SCR_F3 || key == SCR_ESCAPE || key == SCR_F2)) {
		    Debug("EDBC: search_lbc = -1 && key = %d\n", key);
		    return(-1);
	    }
	    goto fnk;   /* BP_M 25-06-2003 */
	}
	if(SCR_app_key()) key = -1;
	bc->bc_last_key = key;
GMSG("bc_last_key = %d\n", key);
	if(BC_fnkeys(bc, bc->bc_last_key) != 0) {
	    // return(BCDEF_LOSTCMT + BCDEF_KEYUSED); /* BP_M 19-02-2003 12:38 */
	    return(-1);
	    }
	if(BC_if_fnkeys(bc, key)) return(0);  /* BP_M 31-08-2005 */
	if(key == SCR_F3 || key == SCR_ESCAPE || key == SCR_F2) return(-1);

fnk:
	switch(key) {
#if !defined(SCRW32) && !defined(SCRGNOME)
	    case ' ' :
		BC_fnk_menu(bc);
		return(BCDEF_LOSTCMT + BCDEF_KEYUSED);
#endif

	    case -2            : BC_move(bc, diff); return(BCDEF_KEYUSED);
	    case SCR_CSR_UP    : BC_up(bc);      return(BCDEF_KEYUSED);
	    case SCR_CSR_DOWN  : BC_down(bc);    return(BCDEF_KEYUSED);
	    case SCR_CSR_PG_UP : BC_pg_up(bc);   return(BCDEF_KEYUSED);
	    case SCR_CSR_PG_DN : BC_pg_down(bc); return(BCDEF_KEYUSED);
	    case SCR_CSR_HOME  : BC_home(bc);    return(BCDEF_KEYUSED);
	    case SCR_CSR_END   : BC_end(bc, 1);  return(BCDEF_KEYUSED);

	    case SCR_S_CSR_UP    : BC_up(bc);      BC_mark(bc); return(BCDEF_KEYUSED);
	    case SCR_S_CSR_DOWN  : BC_down(bc);    BC_mark(bc); return(BCDEF_KEYUSED);
	    case SCR_S_CSR_PG_UP : BC_pg_up(bc);   BC_mark(bc); return(BCDEF_KEYUSED);
	    case SCR_S_CSR_PG_DN : BC_pg_down(bc); BC_mark(bc); return(BCDEF_KEYUSED);
	    case SCR_S_CSR_HOME  : BC_home(bc);    BC_mark(bc); return(BCDEF_KEYUSED);
	    case SCR_S_CSR_END   : BC_end(bc, 1);  BC_mark(bc); return(BCDEF_KEYUSED);

	    case SCR_ENTER :
		BC_modify_lbc(bc);
		BC_exec_link(bc);
		return(BCDEF_LOSTCMT + BCDEF_KEYUSED);

	    case SCR_DELETE :
		if(!bc->bc_delete) return(0);   /* BP_M 25-08-2005 */
		if(CONFIRME) return(BCDEF_LOSTCMT + BCDEF_KEYUSED);
		BC_delete_lbc(bc);
		BC_exec_link(bc);
		return(BCDEF_LOSTCMT + BCDEF_KEYUSED);

	    case SCR_INSERT :
		if(bc->bc_insert_lbc == 0) BC_end(bc, 1);
		if(BC_create_lbc(bc) == 0) BC_exec_link(bc);
		return(BCDEF_LOSTCMT + BCDEF_KEYUSED);

	    case SCR_C_L :
	    case SCR_A_L :
		if(bc->bc_no_mark) break; /* JMP_M 4.20 01-09-95 */
		BC_mark(bc);
		return(BCDEF_KEYUSED);

	    case SCR_C_U :
	    case SCR_A_U :
		if(bc->bc_no_mark) break;
		BC_unmark(bc);
		return(BCDEF_KEYUSED);

	    case SCR_C_C :
	    case SCR_A_C :
		if(bc->bc_no_mark) return(BCDEF_KEYUSED);
		BC_cp_mark(bc);
		BC_exec_link(bc);
		return(BCDEF_LOSTCMT + BCDEF_KEYUSED);

	    case SCR_C_D :
	    case SCR_A_D :
		if(bc->bc_no_mark) return(BCDEF_KEYUSED);
		if(CONFIRME == 0) {
		    BC_del_mark(bc);
		    BC_exec_link(bc);
		    }
		return(BCDEF_LOSTCMT + BCDEF_KEYUSED);

	    case SCR_HELP_KEY :
		HLP_stack_edit();
		return(BCDEF_LOSTCMT + BCDEF_KEYUSED);

	    default :
		if(bc->bc_act_anyk > 0 && ACT_exec_nu(bc->bc_act_anyk) == 0)
		    return(BCDEF_LOSTCMT);
//              bc->bc_last_key = SCR_LKEY;
//              if(BC_fnkeys(bc, bc->bc_last_key) != 0) return(0); /* BP_M 19-02-2003 12:38 */
		BC_display_lpg(bc, bc->bc_c_line, 0);
		return(-1);
	    }
    return(0);
}

/* =+========== BC_FNKEYS ======================================
    Execute the fnkeys :
	bc     : IN : pointer to the BC
	key    : IN : key pressed
    Return = 0 if the field must not be leaved, 1 else
    ================================================================== */

BC_fnkeys(bc, key)
BC      *bc;
int     key;
{
    int     i;
    int     fn;
    int     nb_fnk;
    FNKEY   *fnk;
    FNKEYS  *fk = (FNKEYS *)SCR_PTR(bc->bc_fnks);


    if(bc->bc_fnks == 0) return(0);
    fnk    = fk->fk_fnks;
    nb_fnk = fk->fk_nb_fnks;

    for(i = 0 ; i < nb_fnk ; i++)
	if(fnk[i].fnk_key == key) {
	    if(SCR_check_prot_vt(fnk[i].fnk_prot)) return(-1);
	    if(fnk[i].fnk_act == 0) return(-1);
//          Debug("EDBC - BC_fnkeys - exec_act\n");
	    return(ACT_exec_nu(fnk[i].fnk_act));
	}
    return(0);
}




/* =+========== BC_IF_FNKEYS ======================================
    Recherche si une touche fonction est dans les touches fonctions
    du bon de commande
	bc     : IN : pointer to the BC
	key    : IN : key pressed
    Return = 0 si non 1 si oui
    =================================================================== */

BC_if_fnkeys(bc, key)    /* BP_MODIF 12-12-92 10:53 */
BC      *bc;
int     key;
{
    int     i;
    FNKEY   *fnk;
    FNKEYS  *fk = (FNKEYS *)SCR_PTR(bc->bc_fnks);


    if(bc->bc_fnks == 0) return(0);
    fnk    = fk->fk_fnks;

    for(i = 0 ; i < fk->fk_nb_fnks ; i++)
	if(fnk[i].fnk_key == key) return(1);

    return(0);
}

BC_move(BC *bc, int diff)
{
    bc->bc_c_line = diff;
    BC_display_lpg(bc, bc->bc_c_line, 1);
    return(0);
}
/* =+===== BC_UP ===========
    Move one LBC up.
    bc : IO : BC    *
   ========================= */

BC_up(bc)
BC      *bc;
{
    /* IF NO RECORD DEFINED BEEP */
    if(bc->bc_list == 0) {
	SCR_beep();
	return(-1);
	}

    /* FIRST RECORD OF THE SCREEN */
    if(bc->bc_c_line == 0) {
	if(bc->bc_line0 <= 0) {
	    SCR_beep();
	    return(0);
	    }
	bc->bc_line0--;
	BC_display_lbcs(bc);
	BC_display_lpg(bc, bc->bc_c_line, 1);
	return(0);
	}

    /* RECORD WITHIN THE SCREEN (NOT FIRST) */
    BC_display_lpg(bc, bc->bc_c_line, 0);
    bc->bc_c_line--;
    BC_display_lpg(bc, bc->bc_c_line, 1);
    return(0);
}
/**/
/* =+===== BC_DOWN ===========
    Move one record down.
    bc : IO : BC    *
   ========================= */

BC_down(bc)
BC      *bc;
{
    /* IF NO RECORD DEFINED BEEP */
    if(bc->bc_list == 0) {
	SCR_beep();
	return(-1);
	}

    /* LAST RECORD OF THE SCREEN */
    if(bc->bc_c_line == bc->bc_nb_recs - 1) {
	if(bc->bc_nb_lbc <= bc->bc_line0 + bc->bc_c_line + 1) {
	    SCR_beep();
	    return(0);
	    }
	bc->bc_line0++;
	BC_display_lbcs(bc);
	BC_display_lpg(bc, bc->bc_c_line, 1);
	return(0);
	}

    /* RECORD WITHIN THE SCREEN (NOT LAST) */
    if(bc->bc_nb_lbc <= bc->bc_line0 + bc->bc_c_line + 1) {
	SCR_beep();
	return(0);
	}

     BC_display_lpg(bc, bc->bc_c_line, 0);
     bc->bc_c_line++;
     BC_display_lpg(bc, bc->bc_c_line, 1);
     return(0);
}

/**/
/* =+===== BC_PG_UP ===========
    Move one page up.
    bc : IO : BC    *
   ========================= */

BC_pg_up(bc)
BC      *bc;
{
    /* IF NO RECORD DEFINED BEEP */
    if(bc->bc_list == 0) {
	SCR_beep();
	return(1);
	}

    /* IF FIRST RECORD ALREADY ON SCREEN $BEEP */
    if(bc->bc_line0 <= 0) {
	SCR_beep();
	return(0);
	}

    /* MOVE nb_recs RECORDS UP */
    bc->bc_line0 -= bc->bc_nb_recs;
    if(bc->bc_line0 < 0) bc->bc_line0 = 0;
    BC_display_lbcs(bc);
    BC_display_lpg(bc, bc->bc_c_line, 1);
    return(0);
}
/**/
/* =+===== BC_PG_DOWN ===========
    Move one page down.
    bc : IO : BC    *
   ========================= */

BC_pg_down(bc)
BC      *bc;
{
    /* IF NO RECORD DEFINED BEEP */
    if(bc->bc_list == 0) {
	SCR_beep();
	return(-1);
	}

    /* IF LAST RECORD ALREADY ON SCREEN BEEP */
    if(bc->bc_line0 + bc->bc_nb_recs >= bc->bc_nb_lbc) {
	SCR_beep();
	return(0);
	}

    /* MOVE nb_recs RECORDS DOWN */
    bc->bc_line0 += bc->bc_nb_recs;
    if(bc->bc_line0 + bc->bc_nb_recs >= bc->bc_nb_lbc)
	bc->bc_line0 = bc->bc_nb_lbc - bc->bc_nb_recs;
    BC_display_lbcs(bc);
    BC_display_lpg(bc, bc->bc_c_line, 1);
    return(0);
}
/**/
/* =+===== BC_HOME ================
    Move to first rec
    bc : IO : BC    *
   ================================ */

BC_home(bc)
BC      *bc;
{
    /* IF NO RECORD DEFINED BEEP */
    if(bc->bc_list == 0) {
	SCR_beep();
	return(-1);
	}

    if(bc->bc_c_line == 0 && bc->bc_line0 == 0) {
	SCR_beep();
	return(-1);
	}

    bc->bc_c_line = 0;
    bc->bc_line0  = 0;
    BC_display_lbcs(bc);
    BC_display_lpg(bc, bc->bc_c_line, 1);
    return(0);
}
/**/
/* =+===== BC_END ================
    Move to last rec
    bc : IO : BC    *
   ================================ */

BC_end(bc, disp)
BC      *bc;
int     disp;
{
    /* IF NO RECORD DEFINED BEEP */
    if(bc->bc_list == 0) {
	SCR_beep();
	return(-1);
	}

    /* IF LAST RECORD ALREADY ON SCREEN, MOVE */
    if(bc->bc_line0 + bc->bc_nb_recs >= bc->bc_nb_lbc) {
	if(disp) BC_display_lpg(bc, bc->bc_c_line, 0);
	bc->bc_c_line = bc->bc_nb_lbc - bc->bc_line0 - 1;
	if(disp) BC_display_lpg(bc, bc->bc_c_line, 1);
	return(0);
	}

    /* ELSE MOVE TO THE LAST PAGE */
    bc->bc_line0 = bc->bc_nb_lbc - bc->bc_nb_recs;
    bc->bc_c_line = bc->bc_nb_recs - 1;
    if(disp) {
	BC_display_lbcs(bc);
	BC_display_lpg(bc, bc->bc_c_line, 1);
    }
    return(0);
}

/*NH*/
long BC_calc_count(bc, line, pos, nl)     /* JMP_M 4.19 09-06-95 */
BC      *bc;
int     line, pos, nl;
{
    long    l1, l2;
    BCLIST  *bcl;
    int     i, incr;

    if(line < 0) {
	line = 0;
	pos = 1;
	}
beg:
    if(pos == 2 || bc->bc_insert_lbc == 0)
	return(SCR_STEP_BCCOUNT + BC_get_count(bc, bc->bc_nb_lbc - 1)); /* BP_M 20-10-95 */

    l1 = BC_get_count(bc, line);
    if(pos == 0) { /* After */
	if(line == bc->bc_nb_lbc - 1) return(l1 + SCR_STEP_BCCOUNT); /* BP_M 20-10-95 */
	l2 = BC_get_count(bc, line + 1);
	}

    else if(pos == 1) { /* Before */
	l2 = l1;
	if(line == 0) l1 = 0L;
	else l1 = BC_get_count(bc, line - 1);
	}
    if(l2 > l1 + 1) {
	incr = (l2 - l1) / (nl + 1);
	if(incr == 0) incr = 1;
	return(l1 + incr);
	}

    SCR_comment(SCR_err_txt(518));
    if(BC_renum_lbcs(bc)) pos = 2;
    goto beg;
}

/*NH*/
BC_renum_lbcs(bc)     /* JMP_M 4.19 09-06-95 */
BC  *bc;
{
    int     i, j, reste = 1;
    long    *ocounts, *ncounts;

    ocounts = (long *) SCR_malloc(2 * bc->bc_nb_lbc * sizeof(long));
    ncounts = ocounts + bc->bc_nb_lbc;

    for(i = 0; i < bc->bc_nb_lbc ; i++) {
	ocounts[i] = BC_get_count(bc, i);
	ncounts[i] = SCR_STEP_BCCOUNT * (i + 1L); /* BP_M 20-10-95 */
	if(ncounts[i] == ocounts[i]) ncounts[i] = -1L;
	}

    while(reste) {
	reste = 0;
	for(i = bc->bc_nb_lbc - 1 ; i >= 0 ; i--) {
	    if(ncounts[i] < 0) continue;
	    reste = 1;
	    for(j = 0 ; j < bc->bc_nb_lbc ; j++)
		if(ncounts[i] == ocounts[j]) break;
	    if(j < bc->bc_nb_lbc) continue;
	    BC_renum_lbc_1(bc, i, ncounts[i]);
	    ocounts[i] = ncounts[i];
	    ncounts[i] = -1L;
	    }
	}
    SCR_free(ocounts);
    return(0);
}

/*NH*/
BC_renum_lbc_1(bc, i, count)     /* JMP_M 4.19 09-06-95 */
BC      *bc;
int     i;
long    count;
{
    ISAM    *is_lbc = (ISAM *)SCR_PTR(bc->bc_is_lbc);
    PAGE    *lpg = (PAGE *) SCR_PTR(bc->bc_lpg);
    BCLIST  *bcl;

    bcl = BCL_locate(bc->bc_list, i);
    memcpy(is_lbc->is_rec, bcl->bcl_rec, is_lbc->is_rec_len);
    if(IS_search(is_lbc, bc->bc_is_lbc_cmp, SCR_EQ, (char *)0) != 0) {
	PG_display_error("BC error : %s", IS_error());
	return(-1);
	}
    BC_set_lbc_count(bc, count);
    if(IS_rewrite(is_lbc)) {
	PG_display_error("BC error : %s", IS_error());
	return(-1);
	}
    memcpy(bcl->bcl_rec, is_lbc->is_rec, is_lbc->is_rec_len);

/*    if(bc->bc_lpg_mem) {
	memcpy(lpg->pg_rec, bcl->bcl_pgrec, lpg->pg_rec_len);
	SCR_rec_to_page(lpg, is_lbc);
	memcpy(bcl->bcl_pgrec, lpg->pg_rec, lpg->pg_rec_len);
	}
/* JMP_M 4.20 17-06-95 */

    return(0);
}

BC_ask_insert_pos(bc)
BC      *bc;
{
    MENU    *mn;

    if(bc->bc_insert_lbc == 0 || bc->bc_nb_lbc == 0) return(2);
    mn = MN_read("scr_bcie");
    if(mn == 0) return(2);
    return(MN_edit(mn) - 1);
}


/* =+=== BC_CREATE_LBC =============================================
    Display an empty the big page and wait for a new record.
    If F10 end the page, add in the buffer
    If F3 is pressed, does not add a record
     bc     : IO : BC *
   ================================================================= */

BC_create_lbc(bc)
BC      *bc;
{
    int     quit = 0, i, insert_pos, nline, cline = bc->bc_line0 + bc->bc_c_line;
    long    s1, s2, count;
    char    *is_rec;
    PAGE    *bpg;
    ISAM    *is, *lbcis = (ISAM *)SCR_PTR(bc->bc_is_lbc);
    MENU    *mn;

    if(bc->bc_bpg == 0 ) {
	SCR_beep();
	return(-1);
	}

    insert_pos = BC_ask_insert_pos(bc);
    if(insert_pos < 0 || insert_pos > 2) return(-1);
    BC_empty_views(bc); /* JMP_M 4.20 01-09-95 */
    if(BC_get_view(bc, &bpg, &is, 1) < 0) return(-1);

    /* DISPLAY AN EMPTY PAGE */
    PG_reset(bpg);              /* BP_M 19-10-95 */
    SCR_isam_to_page(bpg, is);  /* BP_M 19-10-95 */
    memcpy(is->is_rec, lbcis->is_rec, is->is_rec_len);    /* BP_M 19-10-95 */
    SCR_isam_to_page(bpg, lbcis);                         /* BP_M 19-10-95 */
    SCR_isam_to_page(bpg, (ISAM *)SCR_PTR(bc->bc_is_bc)); /* JMP 05-06-96 */
    bpg->pg_operation = 1;
    if(bpg->pg_fld_count != MAX_BFIELD12) {
	nline = cline + 1;
	if(insert_pos == 0) nline += 1;
	else if(insert_pos == 2) nline = bc->bc_nb_lbc + 1;
	SCR_set_field_long(bpg, bpg->pg_fld_count, (long)nline);
	}
    PG_display_im(bpg);

    while(quit == 0) {
	if(PG_edit(bpg)) {
	    quit = 1;
	    break;
	    }
	switch(SCR_LKEY) {
	    case SCR_ESCAPE  :
	    case SCR_A_F4 :
	    case SCR_F3  :
		quit = 1;
		break;

	    case SCR_F10 :
		count = BC_calc_count(bc, cline, insert_pos, 1);
		if(insert_pos == 2 && bc->bc_list != 0) BC_end(bc, 0);

		SCR_page_to_isam(bpg, is);
		memcpy(lbcis->is_rec, is->is_rec, lbcis->is_rec_len);
		BC_bc_to_lbc(bc, 1, count);
		if(IS_write(lbcis) != 0) {
		    PG_display_error(IS_error_text());
		    break;
		    }
		switch(insert_pos) {
		    case 0 : /* après la ligne courante */
			BC_add_bcl(bc, cline);
			BC_isam_to_bcl(bc, cline + 1);
			break;
		    case 1 : /* avant la ligne courante */
			BC_add_bcl(bc, cline - 1);
			BC_isam_to_bcl(bc, cline);
			break;
		    case 2 : /* à la fin */                 /* JMP_M 4.21 11-10-95 */
			BC_add_bcl(bc, bc->bc_nb_lbc);      /* JMP_M 4.21 11-10-95 */
			BC_isam_to_bcl(bc, bc->bc_nb_lbc);  /* JMP_M 4.21 11-10-95 */
			break;                              /* JMP_M 4.21 11-10-95 */
		    }
		BC_set_lpg_count(bc);
		quit = 2;
#ifdef WINDOWS                  /* BP_M 30-03-2000 */
		WscrRecordKey2(SCR_CSR_HOME);
#endif
		break;

	    default :
		SCR_beep();
		break;
	    }
	}

    /* REDRAW THE BACKGROUND AND FREE */
    PG_undisplay(bpg);
    bpg->pg_operation = 0;
    if(quit == 1) return(0);

    /* ELSE, CHANGE C_LINE OR LINE0 */
    if(bc->bc_c_line < bc->bc_nb_recs - 1) {
	if(bc->bc_nb_lbc != 1) bc->bc_c_line++;
	}
    else
	bc->bc_line0++;

    BC_display_lbcs(bc);
    BC_display_lpg(bc, bc->bc_c_line, 1);
    ACT_exec_nu(bc->bc_write_lbc_act);

    return(0);
}

BC_view_lbc(bc)
BC      *bc;
{
    return(BC_view_modify_lbc(bc, 0));
}

BC_modify_lbc(bc)
BC      *bc;
{
    return(BC_view_modify_lbc(bc, 1));
}

/* =+=== BC_MODIFY_LBC =============================================
    Display the contents of the current record in the big page
    and wait for modification
     bc     : IO : BC *
   ================================================================= */

BC_view_modify_lbc(bc, view_mod)
BC      *bc;
{
    int     quit = 0, cline = bc->bc_line0 + bc->bc_c_line;
    PAGE    *bpg;
    ISAM    *lbcis = (ISAM *)SCR_PTR(bc->bc_is_lbc), *is;
    char    *is_rec;

GMSGC;
    if(bc->bc_bpg == 0 || bc->bc_list == 0) {
	SCR_beep();
	return(-1);
	}

    /* Retrieve to old record */
//  if(BC_search_lbc(bc, cline) != 0) return(-1);   /* BP_M 19-02-2003 13:23 */

    BC_get_view(bc, &bpg, &is, 0);
    if(bpg == 0) return(-1);

    is_rec = is->is_rec;
    is->is_rec = lbcis->is_rec;
    SCR_isam_to_page(bpg, is);
    is->is_rec = is_rec;
    if(bpg->pg_fld_count != MAX_BFIELD12)
	SCR_set_field_long(bpg, bpg->pg_fld_count, (long)cline + 1L);

    if(view_mod == 0) {
	PG_display_slide(bpg);
	return(0);
	}

    PG_display_im(bpg);
    BC_search_lbc(bc, cline);       /* BP_M 20-02-2003 10:59 */

    while(quit == 0) {
	PG_disable(PAGE_PTR(bc->bc_apg)); /* JMP 09-03-03 */
GMSGC;
	PG_edit(bpg);
	PG_enable(PAGE_PTR(bc->bc_apg), -1);  /* JMP 09-03-03 */
#ifdef SCRW32
	BC_search_lbc(bc, cline);       /* BP_M 20-02-2003 10:59 */
#endif
	if(bpg->pg_abort) {  /*JMP 15-05-95*/
	    quit = 1;
	    continue;
	    }
	switch(SCR_LKEY) {
	    case SCR_F3  :
	    case SCR_A_F4  :
	    case SCR_ESCAPE : /* JMP_M 4.19 08-06-95 */
		quit = 1;
		break;

	    case SCR_F10 :
		SCR_page_to_isam(bpg, is);
		memcpy(lbcis->is_rec, is->is_rec, lbcis->is_rec_len);
		if(IS_rewrite(lbcis) != 0) {
		    PG_display_error(IS_error_text());
		    break;
		    }
		BC_isam_to_bcl(bc, bc->bc_line0 + bc->bc_c_line);
		quit = 2;
		break;

	    default :
		SCR_beep();
		break;
	    }
	}

    /* REDRAW THE BACKGROUND AND FREE */
    PG_undisplay(bpg);

    /* DISPLAY THE LINE MODIFIED */
    if(quit == 1) return(0);
    BC_display_lpg(bc, bc->bc_c_line, 1);
    return(0);
}


BC_search_lbc(bc, line)
BC      *bc;
int     line;
{
    ISAM    *is = (ISAM *)SCR_PTR(bc->bc_is_lbc);
    BCLIST  *bcl;

    if(bc->bc_nb_lbc <= 0) {
	Debug("EDBC: search_lbc : nb_lbc <= 0\n");
	return(-1);
    }
    bcl = BCL_locate(bc->bc_list, line);

    /* STORE OLD RECORD IN ISAM */
    memcpy(is->is_rec, bcl->bcl_rec, is->is_rec_len);

    if(IS_search(is, bc->bc_is_lbc_cmp, SCR_EQ, (char *)0) != 0) {
	Debug("EDBC: is_search lbc : ret -1\n");
//        PG_display_error(IS_error_text());    /* BP_M 25-06-2003 */
	return(-1);
	}
    SCR_isam_to_page((PAGE *)SCR_PTR(bc->bc_lpg), is);  /* BP_M 19-02-2003 13:23 */
    return(0);
}

/* =+=== BC_DELETE_LBC =============================================
    Delete the current record and redisplay the screen
     bc     : IO : BC *
   ================================================================= */

BC_delete_lbc(bc)
BC      *bc;
{
    if(bc->bc_list == 0) {
	SCR_beep();
	return(-1);
	}
#ifdef SCRW32
    if(BC_search_lbc(bc, bc->bc_line0 + bc->bc_c_line) != 0) return(-1);
#endif
    SCR_SQL_REPOSIT = 0;
    if(IS_delete((ISAM *)SCR_PTR(bc->bc_is_lbc)) != 0) {
	PG_display_error(IS_error_text());
	return(-1);
	}
    SCR_SQL_REPOSIT = 1;
    /* DELETE IN BCLIST */
    BC_delete_bcl(bc, bc->bc_c_line + bc->bc_line0);
    BC_set_lpg_count(bc);

    /* ADJUST C_LINE AND LINE0 */
    if(bc->bc_nb_lbc == 0) {
	bc->bc_c_line = 0;
	BC_display_screen(bc, bc->bc_c_line, bc->bc_nb_recs - 1);
	return(0);
	}

    /* IF LINE0 IS 0 , DECREMENT C_LINE AND REDISPLAY */
    if(bc->bc_line0 == 0) {
	if(bc->bc_c_line >= bc->bc_nb_lbc) bc->bc_c_line--;
	BC_display_screen(bc, bc->bc_c_line, bc->bc_nb_recs - 1);
	BC_display_lpg(bc, bc->bc_c_line, 1);
	return(0);
	}

    /* ELSE IF LAST RECORD OF THE SCREEN NOT DEFINED, LINE0-- */
    if(bc->bc_nb_recs + bc->bc_line0 > bc->bc_nb_lbc) {
	bc->bc_line0--;
	BC_display_lbcs(bc);
	BC_display_lpg(bc, bc->bc_c_line, 1);
	return(0);
	}

    /* ELSE REDISPLAY */
    BC_display_screen(bc, bc->bc_c_line, bc->bc_nb_recs - 1);
    BC_display_lpg(bc, bc->bc_c_line, 1);
    return(0);
}

/* =+=== BC_OPEN ==================================================
    Open the ISAM defined in a BC
	bc : IO : pointer ot the BC
   ================================================================== */

BC_open(bc)
BC   *bc;
{
    if(IS_open((ISAM *)SCR_PTR(bc->bc_is_bc), SCR_UPDATE, SCR_NO_LOCK) != 0) return(-1);
    if(IS_open((ISAM *)SCR_PTR(bc->bc_is_lbc), SCR_UPDATE, SCR_NO_LOCK) != 0) return(-1);
    return(0);
}
/**/
/* =+=== BC_CLOSE ==================================================
    Close the ISAM defined in a BC
	bc : IO : pointer ot the BC
   ================================================================== */

BC_close(bc)
BC   *bc;
{
    IS_close((ISAM *)SCR_PTR(bc->bc_is_bc));
    IS_close((ISAM *)SCR_PTR(bc->bc_is_lbc));
    return(0);
}
/**/
/* =+=== BC_SEARCH ==================================================
    Search for a BC on a index. Save the BC found. Clear the BC if found
    before storing the new BC.
	bc : IO : pointer ot the BC
	cmp_nb : IN : index for search
   ================================================================== */

BC_search(bc, cmp_nb, mode) /* BP_M 01-10-97 */
BC   *bc;
int  cmp_nb;
int  mode;
{
    int     cond = SCR_GE;
    extern int BC_NO_RESEARCH;

    /* SEARCH THE BC */
    /* BP_M 01-10-97 */
    if(SCR_MODE_SQL) cond = SCR_EQ;

    if(!BC_NO_RESEARCH) { /* BP_M 12-05-1999 */
	if(mode == 0) {
	    SCR_page_to_isam((PAGE *)SCR_PTR(bc->bc_apg), (ISAM *)SCR_PTR(bc->bc_is_bc));
	    if(0 != IS_search((ISAM *)SCR_PTR(bc->bc_is_bc), cmp_nb, cond, (char *)0)) {
		IS_empty_rec((ISAM *)SCR_PTR(bc->bc_is_bc));
		if(0 != IS_search((ISAM *)SCR_PTR(bc->bc_is_bc), cmp_nb, SCR_GE, (char *)0))
		    return(-1);
	    }
	}
    }
    else
	BC_NO_RESEARCH = 0;

    SCR_isam_to_page((PAGE *)SCR_PTR(bc->bc_apg), (ISAM *)SCR_PTR(bc->bc_is_bc));

    /* SEARCH THE LBC */
    BC_search_lbcs(bc);
    return(0);
}
/**/
/* =+=== BC_NEXT ==================================================
    Search the next BC on a index. Save the BC found. Clear the BC if found
    before storing the new BC.
	bc : IO : pointer ot the BC
   ================================================================== */

BC_next(bc)
BC   *bc;
{
    /* NEXT BC */
    if(0 != IS_next((ISAM *)SCR_PTR(bc->bc_is_bc))) return(-1);
    SCR_isam_to_page((PAGE *)SCR_PTR(bc->bc_apg), (ISAM *)SCR_PTR(bc->bc_is_bc));

    /* SEARCH THE LBC */
    BC_search_lbcs(bc);
    return(0);
}
/**/
/* =+=== BC_PREV ==================================================
    Search the previous BC on a index. Save the BC found. Clear the BC if found
    before storing the new BC.
	bc : IO : pointer ot the BC
   ================================================================== */

BC_prev(bc)
BC   *bc;
{
    /* PREV BC */
    if(0 != IS_prev((ISAM *)SCR_PTR(bc->bc_is_bc))) return(-1);
    SCR_isam_to_page((PAGE *)SCR_PTR(bc->bc_apg), (ISAM *)SCR_PTR(bc->bc_is_bc));

    /* SEARCH THE LBC */
    BC_search_lbcs(bc);
    return(0);
}
/**/
/* =+=== BC_SEARCH_LBCS ==============================================
    Search the LBCs for the LBC in the PAGE.
	bc : IO : pointer ot the BC
   ================================================================== */

BC_search_lbcs(bc)  /* BP_M 01-10-97 */
BC   *bc;
{
    char    *buf, buf2[80];
    int     c_type, flag = 0, fld_lg, bcl_nb = 0, cond;
    ISAM    *is = (ISAM *)SCR_PTR(bc->bc_is_lbc),
	    *is2 = (ISAM *)SCR_PTR(bc->bc_is_bc);

    /* FREE THE LBC LIST */
    BC_free_lbcs(bc);

    /* READ THE LBCs */
    BC_bc_to_lbc(bc, 0, -1L);

    buf = SCR_malloc(is2->is_rec_len);
    memcpy(buf, is2->is_rec, is2->is_rec_len);

    BC_count_fld(bc, &c_type);
    if(c_type == SCR_LONG || c_type == SCR_SHORT) flag = 1;

    /* BP_M 01-10-97 */
    cond = is->is_cmps[bc->bc_is_lbc_cmp].cmp_nb_flds;

    /* BP_M 17-07-1998 */
    cond = SCR_EQ1 + cond - 2;
    /* BP_M 17-07-1998 */
/*
    if(cond >= 2) cond += SCR_EQ1 - 2;
    else
		  cond = SCR_GE;
*/
    /* BP_M 01-10-97 */
GMSGC;
    if(0 != IS_search(is, bc->bc_is_lbc_cmp, cond, (char *)0)) {
	SCR_free(buf);
GMSGC;
	return(0);
	}
    while(1) {
	/* BP_M 17-07-1998 */
/*      if(BC_check_eq(bc, buf, flag)) break; */
	BC_add_bcl(bc, bcl_nb);
	BC_isam_to_bcl(bc, bcl_nb);
	bcl_nb++;

	if(IS_next(is) != 0) break;
    }

GMSG("Nb recs : %d\n", bcl_nb);
    SCR_free(buf);
    BC_set_lpg_count(bc);
    bc->bc_mark_1 = bc->bc_mark_2 = -1;
    if(bc->bc_sbar) bc->bc_sbar->sb_l1v = -1;
    return(0);
}

BC_check_eq(bc, val, flag)
BC      *bc;
char    *val;
int     flag;
{
    char    buf2[80];
    int     lbc_fld, nb_fld, bc_fld, fld_lg, i;
    ISAM    *is  = (ISAM *)SCR_PTR(bc->bc_is_lbc);
    ISAM    *is2 = (ISAM *)SCR_PTR(bc->bc_is_bc);

    nb_fld = (is->is_cmps + bc->bc_is_lbc_cmp)->cmp_nb_flds - flag;

    for(i = 0 ; i < nb_fld ; i++) {
	lbc_fld   = (is->is_cmps + bc->bc_is_lbc_cmp)->cmp_flds[i];
	fld_lg    = (is->is_flds + lbc_fld)->isf_length;

	bc_fld    = is2->is_flds[bc->bc_is_bc_fld[i]].isf_start;

	IS_get_field(is, lbc_fld, buf2);
	if(memcmp(val + bc_fld, buf2, fld_lg) != 0) return(-1);
    }
    return(0);
}

/*NH*/
BC_set_lbc_count(bc, count)
BC      *bc;
long    count;
{
    short   s;
    int     c_type, lbc_fldc_nb;
    ISAM    *is = (ISAM *)SCR_PTR(bc->bc_is_lbc);

    lbc_fldc_nb = BC_count_fld(bc, &c_type);
    if(c_type == SCR_LONG || c_type == SCR_SHORT) {
	s = count;
	if(c_type == SCR_LONG)  IS_set_field(is, lbc_fldc_nb, (char *)&count);
	if(c_type == SCR_SHORT) IS_set_field(is, lbc_fldc_nb, (char *)&s);
	}
    return(0);
}

/* =+=== BC_BC_TO_LBC ============================================
    Copy the field of BC to the index of LBC
     bc : IO : BC *
   =============================================================== */
BC_bc_to_lbc(bc, flag, count)
BC      *bc;
int     flag;
long    count;
{
    int     nb_fld, lbc_fld_nb, i, lg;
    char    *rec_bc, *rec_lbc;
    ISAM    *is2 = (ISAM *)SCR_PTR(bc->bc_is_bc),
	    *is = (ISAM *)SCR_PTR(bc->bc_is_lbc);

    if(!flag) count = 0;
    BC_set_lbc_count(bc, count);
    SCR_page_to_isam((PAGE *)SCR_PTR(bc->bc_apg), is2);

    nb_fld = is->is_cmps[bc->bc_is_lbc_cmp].cmp_nb_flds - 1;
    for(i = 0 ; i < nb_fld ; i++) {
	rec_lbc  = is->is_rec;
	rec_bc   = is2->is_rec;

	lbc_fld_nb  = (is->is_cmps + bc->bc_is_lbc_cmp)->cmp_flds[i];

	rec_lbc += (is->is_flds + lbc_fld_nb)->isf_start;
	lg       = (is->is_flds + lbc_fld_nb)->isf_length;
	rec_bc  += (is2->is_flds + bc->bc_is_bc_fld[i])->isf_start;

	memcpy(rec_lbc, rec_bc, lg);
    }
    return(0);
}

/*
   Permet de faire exécuter la display_fn de la lpg d'un BC avant de
   copier le contenu du record de la page dans le buffer du BC
*/

int SCR_BC_EXEC_LPG_DISP_ACT = 0;/* BP_M 01-11-1999 */

/* =+=== BC_ISAM_TO_BCL ============================================
    Copy the isam record and lpg in bclist
     bc : IO : BC *
     bcl_nb : IN : position in BCLIST
   ================================================================= */

BC_isam_to_bcl(bc, bcl_nb)
BC      *bc;
int     bcl_nb;
{
    BCLIST  *bcl;
    ISAM    *is, *lbcis = (ISAM *)SCR_PTR(bc->bc_is_lbc);
    char    *is_rec;
    PAGE    *lpg;

    bcl = BCL_locate(bc->bc_list, bcl_nb);
    memcpy(bcl->bcl_rec, lbcis->is_rec, lbcis->is_rec_len);
    if(bc->bc_lpg_mem == 0) return(0);

    BC_select_view(bc, bcl, &lpg, &is);
    is_rec = is->is_rec;
    is->is_rec = lbcis->is_rec;
    SCR_isam_to_page(lpg, is);
    if(SCR_BC_EXEC_LPG_DISP_ACT) ACT_exec_nu(lpg->pg_disp_act); /* BP_M 01-11-1999 */
    is->is_rec = is_rec;
    memcpy(bcl->bcl_pgrec, lpg->pg_rec, lpg->pg_rec_len);
    return(0);
}

/* =+=== BC_WRITE_BC ===========================================
    Write a new BC
	bc : IO : pointer ot the BC
   ================================================================== */

BC_write_bc(bc)
BC   *bc;
{
    ISAM    *is = (ISAM *)SCR_PTR(bc->bc_is_bc);
    PAGE    *apg = (PAGE *)SCR_PTR(bc->bc_apg);

    /* WRITE THE BC */
    /* BP_M 24-06-1998 */
    PG_set_auto_num(apg, is);
    SCR_page_to_isam(apg, is); /* BP_M 26-06-1998 */
    if(IS_write(is) != 0) {
	PG_display_error(IS_error_text());
	return(-1);
    }

    SCR_isam_to_page(apg, is);      /* BP_M 29-07-1999 */
    PG_display_flds(apg);           /* BP_M 29-07-1999 */
    apg->pg_operation = 2;
    return(0);
}

BC_rewrite_bc(bc)
BC   *bc;
{
    ISAM    *is = (ISAM *)SCR_PTR(bc->bc_is_bc);

    /* WRITE THE BC */
    SCR_page_to_isam((PAGE *)SCR_PTR(bc->bc_apg), is);
    if(IS_rewrite(is) != 0) {
	PG_display_error(IS_error_text());
	return(-1);
    }
    return(0);
}

/* =+=== BC_DELETE ==============================================
    Delete a BC
	bc : IO : pointer ot the BC
   ================================================================== */

BC_delete(bc)
BC   *bc;
{
    /* DELETE THE LBC */
    if(BC_delete_lbcs(bc) != 0) return(-1);

    /* DELETE THE BC */
    return(IS_delete((ISAM *)SCR_PTR(bc->bc_is_bc)));
}

/* =+=== BC_DELETE_LBCS ==============================================
    DELETE the LBCs
	bc : IO : pointer ot the BC
   ================================================================== */

BC_delete_lbcs(bc)
BC   *bc;
{
    char    *buf;
    ISAM    *is = (ISAM *)SCR_PTR(bc->bc_is_lbc);
    ISAM    *is2 = (ISAM *)SCR_PTR(bc->bc_is_bc);

    BC_bc_to_lbc(bc, 0, -1L);
    if(0 != IS_search(is, bc->bc_is_lbc_cmp, SCR_GE, (char *)0))
	return(0);

    buf = SCR_malloc(is2->is_rec_len);
    memcpy(buf, is2->is_rec, is2->is_rec_len);

    while(1) {
	if(BC_check_eq(bc, buf, 1)) break;
	if(IS_delete(is) != 0) {
	    PG_display_error(IS_error_text());
	    SCR_free(buf);
	    return(-1);
	}
	if(IS_next(is)) break;
    }
    SCR_free(buf);
    return(0);
}

/**/
/* =+=== BC_DISPLAY_LBCS ==============================================
    DISPLAY THE LBCs
	bc : IO : pointer ot the BC
   ================================================================== */

BC_display_lbcs(bc)
BC   *bc;
{
    BC_display_screen(bc, 0, bc->bc_nb_recs - 1);
    BC_display_sbar(bc);
    return(0);
}

/* =+=== BC_display_screen ===========================================
    Display the LBC of a BC
    bc   : IO : pointer to the BC
    from : IN : first lpg to be displayed
    to   : IN : last little page to be displayed
   =================================================================== */

BC_display_screen(bc, from, to)
BC      *bc;
int     from;
int     to;
{
    int     i;

    for(i = from ; i <= to ; i++)
	BC_display_lpg(bc, i, 0);
    /* BP_M 17-06-1993 13:41 remis JMP_M 4.19 06-06-95 */

    BC_exec_link(bc);

    return(0);
}

/* =+=== BC_DISPLAY_FLDS ===========================================
    Display only the fields of the apg and the LBCs
     bc   : IO : BC *
   ================================================================= */

BC_display_flds(bc)
BC      *bc;
{
    PG_display_flds_no_cmp((PAGE *)SCR_PTR(bc->bc_apg));
    BC_display_lbcs(bc);
    return(0);
}

int     SCR_BC_EMPTY_REC = 1;   /* BP_M 26-06-1998 */
int     SCR_BC_RESET_APG = 1;   /* BP_MODIF 29-10-98 */

/* =+=== BC_EMPTY ===========================================
    Empty a BC
     bc   : IO : BC *
   ================================================================= */

BC_empty(bc)
BC      *bc;
{
    if(SCR_BC_RESET_APG)
	PG_reset((PAGE *)SCR_PTR(bc->bc_apg));
    BC_free_lbcs(bc);

    /* BP_M 26-06-1998 */
    if(SCR_BC_EMPTY_REC == 1)
	IS_empty_rec((ISAM *)SCR_PTR(bc->bc_is_bc));

    return(0);
}

long BC_get_count(bc, line)
BC  *bc;
int line;
{
    char    *rec;
    ISAM    *is = (ISAM *)SCR_PTR(bc->bc_is_lbc);
    BCLIST  *bcl;
    int     lbc_fldc_nb, c_type;
    short   s;
    long    l;

    if(line < 0 || bc->bc_nb_lbc <= line) return(0L);
    bcl = BCL_locate(bc->bc_list, line);

    lbc_fldc_nb = BC_count_fld(bc, &c_type);
    if(c_type != SCR_LONG && c_type != SCR_SHORT) return(0L);

    rec = is->is_rec;
    is->is_rec = bcl->bcl_rec;
    if(c_type == SCR_LONG) IS_get_field(is, lbc_fldc_nb, (char *)&l);
    if(c_type == SCR_SHORT) {
	IS_get_field(is, lbc_fldc_nb, (char *)&s);
	l = s;
	}

    is->is_rec = rec;
    return(l);
}

BC_set_count(bc, line, count)
BC      *bc;
int     line;
long    count;
{
    char    *rec;
    ISAM    *is = (ISAM *)SCR_PTR(bc->bc_is_lbc);
    BCLIST  *bcl;
    int     nb_fld, lbc_fldc_nb, c_type;
    short   s = count;

    if(line < 0 || bc->bc_nb_lbc <= line) return(0);
    bcl = BCL_locate(bc->bc_list, line);

    lbc_fldc_nb = BC_count_fld(bc, &c_type);
    if(c_type != SCR_LONG && c_type != SCR_SHORT) return(0);

    rec = is->is_rec;
    is->is_rec = bcl->bcl_rec;
    if(c_type == SCR_LONG)  IS_set_field(is, lbc_fldc_nb, (char *)&count);
    if(c_type == SCR_SHORT) IS_set_field(is, lbc_fldc_nb, (char *)&s);
    is->is_rec = rec;
    return(0);
}

BC_set_lpg_count(bc)
BC      *bc;
{
    PAGE    *lpg;
    BCLIST  *bcl;
    char    *pg_rec;
    long    line = 1L;
    ISAM    *is;

    if(bc->bc_lpg_mem == 0) return(0);

    bcl = bc->bc_list;
    while(bcl != 0) {
	BC_select_view(bc, bcl, &lpg, &is);
	if(lpg->pg_fld_count != MAX_BFIELD12) {
	    pg_rec = lpg->pg_rec;
	    lpg->pg_rec = bcl->bcl_pgrec;
	    SCR_set_field_long(lpg, lpg->pg_fld_count, line);
	    lpg->pg_rec = pg_rec;
	    }
	bcl = bcl->bcl_next;
	line++;
	}
    return(0);
}

BC_in_mark(bc, line)
BC  *bc;
int line;
{
    if(bc->bc_mark_1 <= line && bc->bc_mark_2 >= line) return(1);
    if(bc->bc_mark_1 >= line && bc->bc_mark_2 <= line) return(1);
    return(0);
}

BC_mark(bc)
BC      *bc;
{
    if(bc->bc_no_mark) return(0);
    if(bc->bc_mark_1 < 0) {
	bc->bc_mark_1 = bc->bc_mark_2 = bc->bc_c_line + bc->bc_line0;
	return(0);
	}

    bc->bc_mark_2 = bc->bc_c_line + bc->bc_line0;
#ifdef WINDOWS
    BC_send_mark(bc);
#endif
    BC_display_lbcs(bc);
    BC_display_lpg(bc, bc->bc_c_line, 1);
    return(0);
}

BC_unmark(bc)
BC      *bc;
{
    bc->bc_mark_1 = bc->bc_mark_2 = -1;
#ifdef WINDOWS
    BC_send_mark(bc);
#endif
    BC_display_lbcs(bc);
    BC_display_lpg(bc, bc->bc_c_line, 1);
    return(0);
}

BC_pgrec_len(bc)
BC  *bc;
{
    int     lg, lg2, i;

    lg = ((PAGE *)SCR_PTR(bc->bc_lpg))->pg_rec_len;
    for(i = 0 ; i < bc->bc_nb_views ; i++) {
	lg2 = ((PAGE *)SCR_PTR(bc->bc_views[i].v_lpg))->pg_rec_len;
	lg = max(lg, lg2);
	}
    return(lg);
}

BC_cp_mark(bc)
BC      *bc;
{
    long    count;
    int     i, insert_pos, from = bc->bc_mark_1, to = bc->bc_mark_2,
	    cline, nl;
    BCLIST  *bcl, *nbcl;
    ISAM    *lbc = (ISAM *)SCR_PTR(bc->bc_is_lbc);

    if(bc->bc_list == 0 || from < 0) {
	SCR_beep();
	return(0);
	}
    if(from > to) {
	from = to;
	to = bc->bc_mark_1;
	}
    if(from >= bc->bc_nb_lbc) return(0);
    if(to   >= bc->bc_nb_lbc) to = bc->bc_nb_lbc - 1;

    insert_pos = BC_ask_insert_pos(bc);
    if(insert_pos < 0 || insert_pos > 2) return(0);

    cline = bc->bc_line0 + bc->bc_c_line;
    switch(insert_pos) {
	case 0 : cline = bc->bc_line0 + bc->bc_c_line; break;
	case 1 : cline = bc->bc_line0 + bc->bc_c_line - 1; break;
	case 2 : cline = bc->bc_nb_lbc - 1;
	}

    if(cline >= from && cline < to) {
	SCR_beep();
	return(0);
	}

    nl = to - from + 1;
    for(i = 0 ; i < nl ; i++) {
	count = BC_calc_count(bc, cline + i, 0, nl - i);
	bcl = BCL_locate(bc->bc_list, from + i);
	nbcl = BC_add_bcl(bc, cline + i);
	memcpy(nbcl->bcl_rec, bcl->bcl_rec, lbc->is_rec_len);
	if(bc->bc_lpg_mem)
	    memcpy(nbcl->bcl_pgrec, bcl->bcl_pgrec, BC_pgrec_len(bc));
	BC_set_count(bc, cline + i + 1, count);
	memcpy(lbc->is_rec, nbcl->bcl_rec, lbc->is_rec_len);
	if(IS_write(lbc) != 0) {
	    PG_display_error(IS_error_text());
	    break;
	    }
	if(cline < from) {
	    from++;
	    to++;
	    }
	}

    if(cline < from) {
	bc->bc_mark_1 += nl;
	bc->bc_mark_2 += nl;
	}

    BC_set_lpg_count(bc);
    BC_display_lbcs(bc);
    BC_display_lpg(bc, bc->bc_c_line, 1);
    return(0);
}

BC_del_mark(bc)
BC      *bc;
{
    int     i, from = bc->bc_mark_1, to = bc->bc_mark_2;

    if(bc->bc_list == 0 || from < 0) {
	SCR_beep();
	return(0);
	}
    if(from > to) {
	from = to;
	to = bc->bc_mark_1;
	}
    if(from >= bc->bc_nb_lbc) return(0);
    if(to   >= bc->bc_nb_lbc) to = bc->bc_nb_lbc - 1;

    for(i = from ; i <= to ; i++) {
	if(BC_search_lbc(bc, from) != 0) goto fin;
	if(IS_delete((ISAM *)SCR_PTR(bc->bc_is_lbc)) != 0) {
	    PG_display_error(IS_error_text());
	    goto fin;
	    }
	BC_delete_bcl(bc, from);
	}

fin:
    BC_adjust(bc);
    BC_set_lpg_count(bc);
    BC_unmark(bc);
    return(0);
}

BC_adjust(bc)
BC  *bc;
{
    if(bc->bc_line0 + bc->bc_nb_recs >= bc->bc_nb_lbc)
	bc->bc_line0 = bc->bc_nb_lbc - bc->bc_nb_recs;
    if(bc->bc_line0 < 0) bc->bc_line0 = 0;

    if(bc->bc_line0 + bc->bc_c_line >= bc->bc_nb_lbc)
	bc->bc_c_line = bc->bc_nb_lbc - bc->bc_line0 - 1;

    if(bc->bc_c_line < 0) bc->bc_c_line = 0;
    return(0);
}


BC_exec_link(bc)
BC      *bc;
{
    PAGE    *apg = (PAGE *)SCR_PTR(bc->bc_apg);

    if(apg->pg_fld_count != MAX_BFIELD12) {
	SCR_set_field_long(apg, apg->pg_fld_count, (long)bc->bc_nb_lbc);
	PG_display_field(apg, apg->pg_fld_count);
	}

    ACT_exec_nu(bc->bc_link_act);
    return(0);
}

BC_count_fld(bc, type)
BC      *bc;
int     *type;
{
    int     i, lbc_fld;
    ISAM    *is = (ISAM *)SCR_PTR(bc->bc_is_lbc);

    i       = is->is_cmps[bc->bc_is_lbc_cmp].cmp_nb_flds;
    lbc_fld = is->is_cmps[bc->bc_is_lbc_cmp].cmp_flds[i - 1];
    *type   = is->is_flds[lbc_fld].isf_type;
    return(lbc_fld);
}


BC_select_view(bc, bcl, lpg, is)
BC      *bc;
BCLIST  *bcl;
PAGE    **lpg;
ISAM    **is;
{
    char    *is_rec;
    int     rc;

    *lpg = (PAGE *) SCR_PTR(bc->bc_lpg);
    *is = (ISAM *) SCR_PTR(bc->bc_is_lbc);
    if(bc->bc_view_fn == 0) return(0);
    is_rec = (*is)->is_rec;
    (*is)->is_rec = bcl->bcl_rec;
    rc = ACT_exec_nu(bc->bc_view_fn);
    (*is)->is_rec = is_rec;
    if(rc <= 0 || rc > bc->bc_nb_views) return(0);
    *lpg = (PAGE *) SCR_PTR(bc->bc_views[rc - 1].v_lpg);
    *is = (ISAM *) SCR_PTR(bc->bc_views[rc - 1].v_isam);
    return(rc);
}

BC_get_view(bc, bpg, is, view_create)
BC      *bc;
PAGE    **bpg;
ISAM    **is;
int     view_create;
{
    int     rc, act;

    *bpg = (PAGE *) SCR_PTR(bc->bc_bpg);
    *is = (ISAM *) SCR_PTR(bc->bc_is_lbc);
    if(view_create == 0) act = bc->bc_view_fn;
    else                 act = bc->bc_view_crfn;
    if(act == 0) return(0);
    rc = ACT_exec_nu(act);
    if(rc < 0) return(-1);
    if(rc == 0 || rc > bc->bc_nb_views) return(0);
    *bpg = (PAGE *) SCR_PTR(bc->bc_views[rc - 1].v_bpg);
    *is  = (ISAM *) SCR_PTR(bc->bc_views[rc - 1].v_isam);
    return(rc);
}

BC_empty_views(bc)
BC      *bc;
{
    int     i;
    ISAM    *is = (ISAM *) SCR_PTR(bc->bc_is_lbc);

    IS_empty_rec(is);

    for(i = 0 ; i < bc->bc_nb_views ; i++) {
	is = (ISAM *) SCR_PTR(bc->bc_views[i].v_isam);
	if(is) IS_empty_rec(is);
	}
    return(0);
}


int BC_NO_RESEARCH = 0; /* BP_M 12-05-1999 */

/*NH*/
BC_search_bc_lbc(bc)    /* BP_M 01-10-97 */
BC      *bc;
{
    int     cmp_nb;
    PAGE    *apg = (PAGE *)SCR_PTR(bc->bc_apg);
    ISAM    *is = (ISAM *)SCR_PTR(bc->bc_is_bc);
    MENU    *mn;
    char    *order;
    int     cond, mode = 0;

GMSGBC
    apg->pg_operation = 4;
    bc->bc_operation = 4;

    /* BP_M 01-10-97 */
GMSGC
    cmp_nb = PG_search(apg, is);
GMSGC
    if(cmp_nb < 0 && cmp_nb != -5) goto fin;
    if(cmp_nb == -5) mode = 1;

    BC_NO_RESEARCH = 1;
    if(0 != BC_search(bc, cmp_nb, mode)) {
	PG_display_error(IS_error_text());
	goto fin;
    }

GMSGC
    BC_display_flds(bc);
GMSGC
    apg->pg_operation = 2;
    bc->bc_operation = 2;

GMSGEC
    return(0);
fin:
    apg->pg_operation = 0;
    bc->bc_operation = 0;
GMSGEC
    return(-1);
}


/*NH =+=== BC_EDIT =====================================================
Edite un BC.

=================================================================== */

BC_edit(bc)
BC      *bc;
{
    int     create = 0, key, rc = 0;
    PAGE    *pg = (PAGE *)SCR_PTR(bc->bc_apg);  /*JMP 15-05-95*/

GMSGBC
#ifdef PDEV     /* BP_M 20-10-95 */
    if(PG_pgvr())return(0);
#endif

    BC_display_sbar(bc);
    while(1) {
GMSGC
	pg->pg_status = 0;                  /* JMP 10-03-03 */
GMSGC;
	rc = PG_edit(pg);                   /* JMP_M 4.20 14-06-95 */
	bc->bc_last_key = pg->pg_last_key; /* JMP_M 4.20 17-06-95 */
	if(SCR_app_key()) return(0);
	if(rc == -1) goto edbc;
	if(rc < 0) GMSGR(0);            /*JMP 15-05-95*/
	switch(pg->pg_last_key) {
	    case SCR_F2 : goto edbc;
	    case SCR_F3 :               /* BP_MODIF 12-12-92 10:30 */
	    case SCR_ESCAPE :           /* JMP_M 4.20 17-06-95 */
		if(bc->bc_operation == 1 && !create)
		    GMSGR(0);
	    case SCR_F10:
		if(ACT_exec_nu(bc->bc_end_act) != 0) goto edbc;
		if(bc->bc_operation == 1 && !create) {
		    if(BC_write_bc(bc) != 0) continue;
		    }
		else
		    if(BC_rewrite_bc(bc) != 0) continue;
		GMSGR(0);
	    default :
		if(BC_fnkeys(bc, bc->bc_last_key) != 0) GMSGR(0); /* JMP_M 4.20 17-06-95 */
		ACT_exec_nu(bc->bc_act_anyk); /* BP_MODIF 16-11-94 */
		continue;
	    }

edbc:
	while(1) {
GMSGC
	    if(ACT_exec_nu(bc->bc_beg_act) != 0) break;
	    if(bc->bc_operation == 1 && !create) {
		if(BC_write_bc(bc) != 0) {    /* JMP_M 4.20 14-06-95 */
		    if(rc == 0) break; /* APG quittée normalement */
		    GMSGR(0);
		    }
		create = 1;
		}
	    else
		if(BC_rewrite_bc(bc) != 0) { /* JMP_M 4.20 14-06-95 */
		    if(rc == 0) break; /* APG quittée normalement */
		    GMSGR(0);
		    }

	    BC_edit_lbcs(bc);
	    Debug("EDBC: End edit_lbcs\n");
	    if(SCR_app_key()) GMSGR(0);
	    key = SCR_LKEY;
	    Debug("EDBC: SCR_LKEY = %d\n", SCR_LKEY);
	    if(key == SCR_F2) break;
	    if(BC_if_fnkeys(bc, key) == 1) {
		Debug("EDBC: SCR_LKEY = bc_fnkeys\n");
		if(key == SCR_F3 || key == SCR_F10 || key == SCR_ESCAPE)
		    break;  /* BP_M 25-06-2003 */
//                BC_fnkeys(bc, key);   /* BP_M 25-06-2003 */
		continue;
	    }
	    if(key != SCR_F10 && key != SCR_F3 && key != SCR_ESCAPE) { /* JMP_M 4.19 06-06-95 */
		SCR_beep();
		continue;
	    }
	    if(ACT_exec_nu(bc->bc_end_act) != 0) continue;
	    GMSGR(0);    /* BP_MODIF 12-12-92 10:16 */
	    }
	}
}

/*NH =+=== BC_EDIT_LBCS ===============================================
    Edit the lbcs
    bc : IO : pointer to the BC
   =================================================================== */

BC_edit_lbcs(bc)
BC      *bc;
{
    int     key, lostcmt = 1;

GMSGBC
    if(bc->bc_nb_lbc > 0) BC_display_lpg(bc, bc->bc_c_line, 1);
    else if(bc->bc_auto_create) BC_create_lbc(bc);

    BC_exec_link(bc);
    BC_edit_lbcs_loop(bc);
    GMSGR(0);
}





