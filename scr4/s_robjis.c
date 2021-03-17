#include "scr.h"

#ifndef SCRPROTO
char      *O_read_text();
U_ch      *OBJ_search_ptr();
char      *OBJ_read_record_key();
char      *SCR_palloc();
#endif

/* === OBJ_READ_ISAM ============================================
   ============================================================== */
OBJ_read_isam(is)       /* BP_M 12-08-1997 */
ISAM    *is;
{
    int     i, nu_vfld;
    COMPOSITE *cmp;

    /* READ STRUCTURE ISAM */
    O_read(is, sizeof(ISAM));

    /* READ IS_FILENAME */
    O_read_text(&(is->is_filename));
    O_read_text(&(is->is_s4srv));   /* BP_M 14-01-2008 */

    if(is->is_nb_join > 0) {
	is->is_join = (ISJOIN *)SCR_malloc(sizeof(ISJOIN) * is->is_nb_join);
	O_read(is->is_join, sizeof(ISJOIN) * is->is_nb_join);
	for(i = 0 ; i < is->is_nb_join ; i++)
	    O_read_text(&(is->is_join[i].req));   /* BP_M 14-01-2008 */
    }

    /* READ ISFIELD */
    if(is->is_nb_flds != 0) {
	is->is_flds = (ISFIELD *)SCR_malloc(sizeof(ISFIELD) * is->is_nb_flds);
	O_read(is->is_flds, sizeof(ISFIELD) * is->is_nb_flds);
    }

    /* READ COMPOSITE */
    if(is->is_nb_cmps != 0) {
	is->is_cmps = (COMPOSITE *)SCR_palloc(sizeof(COMPOSITE) * is->is_nb_cmps);
	O_read(is->is_cmps, sizeof(COMPOSITE) * is->is_nb_cmps);
    }

    /* READ LINK */
    if(is->is_nb_lnks != 0) {
	is->is_lnks = (LINK *)SCR_palloc(sizeof(LINK) * is->is_nb_cmps);
	O_read(is->is_lnks, sizeof(LINK) * is->is_nb_cmps);
    }
    if(is->is_isimem)       /* BP_M 15-01-97 */
	OBJ_read_isi(is);

    /* READ IS_DB_STRING */        /* BP_M 14-06-1999 */
    O_read_text(&(is->is_db_string));

    if(is->is_nb_isrefs) {
	is->is_isrefs = (ISREF *)SCR_palloc(sizeof(ISREF) * is->is_nb_isrefs);
	O_read((char *)is->is_isrefs, sizeof(ISREF) * is->is_nb_isrefs);
	}

    if(is->is_nb_isrels) {
	is->is_isrels = (ISREF *)SCR_palloc(sizeof(ISREF) * is->is_nb_isrels);
	O_read((char *)is->is_isrels, sizeof(ISREF) * is->is_nb_isrels);
	}

    /* INIT RECORD */
    is->is_rec    = SCR_malloc(is->is_rec_len + 1);
    is->is_rrec   = SCR_malloc(is->is_rec_len + 1);
    is->is_rrrec  = SCR_malloc(is->is_rec_len + 1);
    if(is->is_nb_isbs) {                                            /* JMP 11-12-98 */
	is->is_isbs = (ISAMBUF **) SCR_palloc(sizeof(ISAMBUF *) * is->is_nb_isbs);
	for(i = 0; i < is->is_nb_isbs ; i++)
	    is->is_isbs[i] = (ISAMBUF *)
		SCR_palloc(sizeof(ISAMBUF) + is->is_rec_len + is->is_cmp_len);
	}

    /* BP_M 02-10-97 */
    is->is_maxrec = SCR_malloc(is->is_rec_len + 1);

    /* BP_M 26-10-95 */
    IS_alloc_vfld(is);
    return(0);
}

OBJ_read_isi(is)        /* BP_M 15-01-97 */
ISAM    *is;
{
    ISFINFO *isi;
    int     i;

    is->is_info = (ISFINFO *)SCR_malloc(is->is_nb_flds * sizeof(ISFINFO));
    O_read(is->is_info, is->is_nb_flds * sizeof(ISFINFO));
    isi = is->is_info;
    for(i = 0 ; i < is->is_nb_flds ; i++, isi++) {
	O_read_text(&(isi->isi_name));
	O_read_text(&(isi->isi_title));
	O_read_text(&(isi->isi_cmt));
	O_read_text(&(isi->isi_fmt));
	O_read_text(&(isi->isi_dtext)); /* BP_M 21-01-1999 */
    }

    return(0);
}


/* === OBJ_READ_MPAGE ===========================================
   ============================================================== */
OBJ_read_mpage(mp)      /* BP_M 23-01-97 */
MPAGE   *mp;
{
    int     i;  /* BP_M 22-01-97 */

    /* READ STRUCTURE */
    O_read(mp, sizeof(MPAGE));
    O_read_text(&(mp->mp_help));
    mp->mp_views = (VIEW *)SCR_malloc(sizeof(VIEW) * mp->mp_nb_views); /* JMP_M 4.20 14-06-95 */
    O_read(mp->mp_views, sizeof(VIEW) * mp->mp_nb_views);              /* JMP_M 4.20 14-06-95 */

    /* BP_M 22-01-97 */
    if(mp->mp_nb_tcol > 0) {
	mp->mp_tcol = (MPTCOL *)SCR_malloc(sizeof(MPTCOL) * mp->mp_nb_tcol);
	O_read(mp->mp_tcol, sizeof(MPTCOL) * mp->mp_nb_tcol);
	for(i = 0 ; i < mp->mp_nb_tcol ; i++)
	    O_read_text(&(mp->mp_tcol[i].tc_title));
    }
    return(0);
}

/* === OBJ_READ_BC ==============================================
   ============================================================== */
OBJ_read_bc(bc)         /* BP_M 23-01-97 */
BC  *bc;
{
    int     i;

    /* READ STRUCT BC */
    O_read(bc, sizeof(BC));
    O_read_text(&(bc->bc_help));
    bc->bc_views = (VIEW *)SCR_malloc(sizeof(VIEW) * bc->bc_nb_views); /* JMP_M 4.20 14-06-95 */
    O_read(bc->bc_views, sizeof(VIEW) * bc->bc_nb_views);              /* JMP_M 4.20 14-06-95 */
    /* BP_M 22-01-97 */
    if(bc->bc_nb_tcol > 0) {
	bc->bc_tcol = (MPTCOL *)SCR_malloc(sizeof(MPTCOL) * bc->bc_nb_tcol);
	O_read(bc->bc_tcol, sizeof(MPTCOL) * bc->bc_nb_tcol);
	for(i = 0 ; i < bc->bc_nb_tcol ; i++)
	    O_read_text(&(bc->bc_tcol[i].tc_title));
    }
    return(0);
}

/* === OBJ_MPAGE_ABJUST ==========================================
   ============================================================== */
OBJ_mpage_adjust(mp)
MPAGE   *mp;
{
    int     i;
    ISAM    *is = (ISAM *)SCR_PTR(mp->mp_isam);

    mp->mp_ref_rec = SCR_palloc(is->is_rec_len);
    return(0);
}








