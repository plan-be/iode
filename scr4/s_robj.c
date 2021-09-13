#include "scr.h"

U_ch      OBJ_magic[] = " - Objects\032";

struct _check_ {
    long    crdate,
	    crtime,
	    limdate,
	    ak,
	    lastdate,
	    lasttime,
	    nbuses,
	    nbexp;
};

static    FILE *fd_read;

#ifndef SCRPROTO
void *O_read_text();
#endif

/*
U_ch      *OBJ_search_ptr();
char      *OBJ_read_record_key();
inutile !! /* JMP 05-01-98 */

/*char      *SCR_palloc(), *SCR_palloca(); /* JMP 05-01-98 */

/* === SCR_READ_OBJ =================================================
   ================================================================== */
SCR_read_obj(filename)
char    *filename;
{
    char    *pt, buf[80];
/*    char    *SCR_search_file(); /* JMP 05-01-98 */

    /* OPEN FILE OBJ */
    pt = SCR_search_file(filename);

#if defined(DOS) && !defined(DOSW32) && !defined(WIN32) /* JMP 10-10-96 */
    if(pt == 0 || (fd_read = fopen(pt, "r+b")) == 0) {
#else
    if(pt == 0 || (fd_read = fopen(pt, "rb")) == 0) {
#endif
	Werror("Fatal : cannot open '%s'\n", filename);
	exit(-1);
    }


    SCR_SCR_FILE_POS = SCR_get_app_pos(fd_read, SCR_SCR_FILE_NB); /* JMP 16-04-95 */
    if(SCR_SCR_FILE_POS < 0) {
	Werror( "Fatal : append error in '%s'\n", filename);
	exit(-1);
    }

    /* READ MAGIC NB */
    if(SCR_check_magic(fd_read, SCR_magic) ||       /* JMP 25-02-92 */
       SCR_check_magic(fd_read, OBJ_magic)) {
	Werror( "Fatal : bad SCR/AL1 objects file.\nOBJ:%s\nLIB:%s", OBJ_magic, SCR_magic);
	exit(1);
	}

    O_check_ak();

/*    SCRE_first_table = SCR_READ_FILES.tbl_nb_fl; */
    /* READ FILE_TABLE */
    OBJ_read_filetable();
    /* READ STRUCTURE */
    OBJ_read_structures();
    /* READ NAME OF FILE COMMENT */
    OBJ_read_cmt();

    fclose(fd_read);

    return(0);
}

O_check_ak()
{
    long    curp0, curp1;
/*  long ftell(); /* JMP 05-01-98 */
    extern long _SCR_current_date();
    extern long _SCR_current_time();
    long    cdt, ct;
    struct _check_ OBJ_check;
    
    cdt = _SCR_current_date();
    ct  = _SCR_current_time();
	
	

    curp0 = ftell(fd_read);
    O_read((U_ch *)&OBJ_check, sizeof(OBJ_check));
    curp1 = ftell(fd_read);

    SCR_limdate = OBJ_check.limdate;
    SCR_actkey  = OBJ_check.ak;
ag:
    if(SCR_serialnb != 0 && SCR_ak() != SCR_actkey) {
	printf("Activation key rejected. Sorry\n");
			 printf("Serial number is : '%s'\n", SCR_serialnb);
	if(SCR_username) printf("User name is     : '%s'\n", SCR_username);
ag1:
	if(SCR_limdate) {
	    printf("Enter new limit date (old is %ld): ", OBJ_check.limdate);
	    scanf("%ld", &SCR_limdate);
	    if(SCR_limdate <= OBJ_check.limdate) goto ag1;
	    }
	printf("Enter new activation key : ");
	scanf("%ld", &SCR_actkey);
	while(getchar() != '\n');
	OBJ_check.nbexp = 0;
	goto ag;
	}
    OBJ_check.ak = SCR_actkey;
    OBJ_check.limdate = SCR_limdate;
    if(SCR_limdate) {
	if(cdt < OBJ_check.lastdate ||
	  (cdt == OBJ_check.lastdate && ct <= OBJ_check.lasttime) ||
	   cdt > SCR_limdate) {
		OBJ_check.nbexp++;
		OBJ_check.lastdate = SCR_crdate;
		OBJ_check.lasttime = SCR_crtime;
		}
	    }

    if(OBJ_check.nbexp > 10) {
	printf("Fatal : synchronization problem or expiration date exceeded !\n");
	printf("Please contact your dealer.\n");
	goto ag1;
	}

    if(OBJ_check.nbexp > 5) {
	printf("Warning : date synchronization problem !\n");
	printf("Please contact your dealer. Press ENTER to continue\n");
	while(getchar() != '\n');
	}

    OBJ_check.nbuses++;
    OBJ_check.lastdate = cdt;
    OBJ_check.lasttime = ct;
#if !defined(DOSW32) && !defined(UNIX) && !defined(WIN32) /* JMP 10-10-96 */
    fseek(fd_read, curp0, 0);
    fwrite(&OBJ_check, sizeof(OBJ_check), 1, fd_read);
#endif
    fseek(fd_read, curp1, 0);
    return(0);
}

/* === O_READ ===================================================
   ============================================================== */

O_read(text, len)
void    *text; /* JMP 05-01-98 */
int     len;
{
    if(len == 0) return(0);            /* JMP_M 4.20 14-06-95 */
    fread(text, len, 1, fd_read);
    return(0);
}

/* === O_READ_TEXT ==============================================
   ============================================================== */
void *O_read_text(text)  /* JMP 05-01-98 */
void **text;             /* JMP 05-01-98 */
{
    char    buffer[512];

    if(*text == 0) return((char *)0);

    O_read_str(buffer);
/*    *text = SCR_palloca(strlen(buffer) + 1, 1); /* JMP 10-11-99 */
    *text = SCR_stracpy(buffer);                  /* JMP 10-11-99 */
//  strcpy(*text, buffer);  /* BP_M 02-12-2009 16:05 */
    return(*text);
}

/* === O_READ_STR ==============================================
   ============================================================== */
O_read_str(text)
unsigned char    *text;
{
    int     len = 0, c = 1;

    while(c) {
	c = fgetc(fd_read);
	text[len++] = c;
    }
    return(0);
}

/* === OBJ_READ_FILETABLE =======================================
   ============================================================== */
OBJ_read_filetable()
{
    short       i, type, nb_obj, size, mem, def; /* BP_M 19-11-93 18:16 */
    char        name[128], *ptr;
    long        pos;
    FILE_TYPE   *flt;

    /* READ NUMBER OF FILETYPE */
    O_read((char *)&nb_obj, sizeof(short));
    SCR_fix_filetable((int)nb_obj);

    for(i = 1 ; i < nb_obj ; i++) {
	O_read((U_ch *)&type, sizeof(short));
	O_read((U_ch *)&mem , sizeof(short));       /* BP_M 06-04-1993 15:46 */
	O_read((U_ch *)&def , sizeof(short));       /* BP_M 06-04-1993 15:55 */
	O_read_str(name);
	O_read((U_ch *)&pos, sizeof(long));
	ptr = 0;                    /* BP_M 06-04-1993 15:49 */
	if(mem == 1 && def == 1) {              /* BP_M 06-04-1993 15:46 */
	    switch(TYPE16(type)) {
		case SCR_TYPE_PAGE   : size = sizeof(PAGE); break;
		case SCR_TYPE_MENU   : size = sizeof(MENU); break;
		case SCR_TYPE_ISAM   : size = sizeof(ISAM); break;
		case SCR_TYPE_MPAGE  : size = sizeof(MPAGE); break;
		case SCR_TYPE_BC     : size = sizeof(BC); break;
		case SCR_TYPE_PRINTER: size = sizeof(PR_DEF); break;
		case SCR_TYPE_ACT    : size = sizeof(ACTION); break;
		case SCR_TYPE_FNKEYS : size = sizeof(FNKEYS); break;
		case SCR_TYPE_ABAR   : size = sizeof(A_BAR); break;
		case SCR_TYPE_APPL   : size = sizeof(APPL); break;
		case SCR_TYPE_REPORT : size = sizeof(REPORT); break;
		case SCR_TYPE_TPAGES : size = sizeof(TPAGES); break;    /* BP_M 26-01-97 */
		case SCR_TYPE_WTOOLB : size = sizeof(WTOOLBAR); break;  /* BP_M 18-04-1997 */
		}
	    ptr = SCR_palloc(size);
	    }
/*        SCR_add_flt_ptr(((name[0] == '_') ? (char *)0 : name), ptr, type); */
	flt = SCR_READ_FILES.tbl_fl[i];
	if(name[0] == '_') flt->fl_name = 0;
	else {
	    flt->fl_name = SCR_palloca(strlen(name) + 1, 1);
	    strcpy(flt->fl_name, name);
	    }
	flt->fl_ptr  = ptr;
	flt->fl_type = type;
	flt->fl_mem = mem;      /* BP_M 19-11-93 22:04 */
	flt->fl_pos = pos;      /* BP_M 19-11-93 22:04 */
	flt->fl_def = def;      /* BP_M 19-11-93 22:04 */
    }

    return(0);
}


/* === OBJ_READ_STRUCTURES ======================================
   ============================================================== */
OBJ_read_structures()
{
    int     i;
    FILE_TYPE *flt;

    for(i = 1 ; i < SCR_READ_FILES.tbl_nb_fl ; i++) {
	flt = SCR_READ_FILES.tbl_fl[i];
	if(flt->fl_mem == 0 || flt->fl_def == 0) continue; /* JMP 23-04-97 */
	switch(TYPE16(flt->fl_type)) {
	    case SCR_TYPE_PAGE: /* PAGE */
		OBJ_read_page((PAGE *)flt->fl_ptr);
		break;
	    case SCR_TYPE_MENU: /* MENU */
		OBJ_read_menu((MENU *)flt->fl_ptr);
		break;
	    case SCR_TYPE_ISAM: /* ISAM */
		OBJ_read_isam((ISAM *)flt->fl_ptr);
		break;
	    case SCR_TYPE_MPAGE: /* MPAGE */
		OBJ_read_mpage((MPAGE *)flt->fl_ptr);
		break;
	    case SCR_TYPE_BC: /* BC */
		OBJ_read_bc((BC *)flt->fl_ptr);
		break;
	    case SCR_TYPE_ACT: /* ACTION */
		OBJ_read_action((ACTION *)flt->fl_ptr);
		break;
	    case SCR_TYPE_PRINTER: /* PR_DEF */
		OBJ_read_prt((PR_DEF *)flt->fl_ptr);
		break;
	    case SCR_TYPE_FNKEYS:  /* FNKEYS */
		OBJ_read_fnkeys((FNKEYS *)flt->fl_ptr);
		break;
	    case SCR_TYPE_ABAR:  /* A_BAR */
		OBJ_read_abar((A_BAR *)flt->fl_ptr);
		break;
	    case SCR_TYPE_APPL:  /* APPL */
		OBJ_read_appl((APPL *)flt->fl_ptr);
		break;
	    case SCR_TYPE_TPAGES:           /* BP_M 26-01-97 */
		OBJ_read_tpages((TPAGES *)flt->fl_ptr);
		break;
	    case SCR_TYPE_WTOOLB:          /* BP_M 18-04-1997 */
		OBJ_read_wtoolb((WTOOLBAR *)flt->fl_ptr);
		break;
/*            case SCR_TYPE_REPORT:
		OBJ_read_report(flt->fl_ptr);
		break;
*/
	}
	flt->fl_def = 1;
    }
    OBJ_adjust_reference();
    return(0);
}

/* === OBJ_READ_PAGE ============================================
   ============================================================== */
OBJ_read_page(pg)
PAGE    *pg;
{
    int     i, j;
    FIELD   *fld;
    GRPBOX  *gr;        /* BP_M 18-01-97 */
    WBUTTON  *wb; /* BP_M 24-01-97 */

    /* READ STRUCTURE PAGE */
    O_read((U_ch *)pg, sizeof(PAGE));

    /* READ TITLES */
    if(pg->pg_drw_mem) {
	O_read_text(&(pg->pg_title));
	O_read_text(&(pg->pg_footnote));
	OBJ_read_drws(&(pg->pg_drws));
	}

    /* READ HELP */
    O_read_text(&(pg->pg_help));

    /* READ FIELDS */
    if(pg->pg_nb_flds != 0 && pg->pg_fld_mem) {
	pg->pg_flds = (FIELD *)SCR_malloc(sizeof(FIELD) * pg->pg_nb_flds);
	OBJ_read_fields(pg, pg->pg_nb_flds);
    }

    if(pg->pg_nb_flds != 0) {
	pg->pg_rec     = SCR_palloc(pg->pg_rec_len);
	O_read((U_ch *)pg->pg_rec, pg->pg_rec_len);
	if(pg->pg_name_mem)
	    O_read_lena(&(pg->pg_fld_name), 1);
	if(pg->pg_fld_mem) {
	    pg->pg_fld_pos = (short *)SCR_palloca(sizeof(short) * pg->pg_nb_flds, sizeof(short));
	    O_read((U_ch *)pg->pg_fld_pos, sizeof(short) * pg->pg_nb_flds);
	    O_read_lena(&(pg->pg_fld_help), 1);
	    O_read_lena(&(pg->pg_fld_allowed), 1);
	    O_read_lena(&(pg->pg_fld_fmt), 1);
	    O_read_lena(&(pg->pg_fld_cmt), 1);
	}
    }
    /* BP_M 17-04-1997 */
    if(pg->pg_nb_grpbox > 0) {
	gr = (GRPBOX *)SCR_palloc(pg->pg_nb_grpbox * sizeof(GRPBOX));
	pg->pg_grpbox = gr;  /* BP_M 18-01-97 */
	O_read((char *)gr, sizeof(GRPBOX) * pg->pg_nb_grpbox);
	for(i = 0 ; i < pg->pg_nb_grpbox ; i++, gr++) {
	    O_read_text(&(gr->gr_name));
	    O_read_text(&(gr->gr_title));
	}
    }
    /* BP_M 18-04-1997 */
    if(pg->pg_nb_tb) {
	pg->pg_toolb = (U_sh *)SCR_palloc(sizeof(U_sh) * pg->pg_nb_tb);
	O_read((char *)pg->pg_toolb, sizeof(U_sh) * pg->pg_nb_tb);
    }
    return(0);
}

OBJ_read_wtoolb(tb)     /* BP_M 18-04-1997 */
WTOOLBAR    *tb;
{
    int     j;
    WBUTTON  *wb; /* BP_M 24-01-97 */

    /* READ STRUCTURE WTOOLBAR */
    O_read((char *)tb, sizeof(WTOOLBAR));

    O_read_text(&(tb->wt_name));
    O_read_text(&(tb->wt_bitmapfile));
    tb->wt_buttons = (WBUTTON *)SCR_palloc(sizeof(WBUTTON) * tb->wt_nb);
    O_read((char *)tb->wt_buttons, sizeof(WBUTTON) * tb->wt_nb);
    for(j = 0 ; j < tb->wt_nb ; j++)
	O_read_text(&(tb->wt_buttons[j].wb_help));

    return(0);
}

O_read_lena(ptr, a)
unsigned char    **ptr;
int     a;
{
    short    i;

    O_read((char *)&i, sizeof(short));
    if(i == 0) return(0);
    *ptr = SCR_palloca(i, a);
    O_read(*ptr, (int)i);
    return(0);
}

/* === OBJ_READ_FIELDS ==========================================
   ============================================================== */
OBJ_read_fields(pg, nb_fld)
PAGE    *pg;
int     nb_fld;
{
    short   i;
    int     j;
    FIELD   *fld = pg->pg_flds;

    /* READ STRUCTURE FIELD */
    O_read((char *)fld, sizeof(FIELD) * nb_fld);

    for(j = 0 ; j < nb_fld ; j++, fld++) {
	fld->fld_isam = (ISFLD *)SCR_palloc(sizeof(ISFLD) * fld->fld_nb_isam);
	O_read((char *)fld->fld_isam, sizeof(ISFLD) * fld->fld_nb_isam);
	if(pg->pg_name_mem == 0) /* JMP 30-09-91 */
	    fld->fld_name = 0;   /* JMP 30-09-91 */

	/* READ DEFAULT TEXT */
	if(fld->fld_dtext != 0)
		O_read_text(&(fld-> fld_dtext));

	if(fld->fld_acmp != 0) {
	/*    O_read(&i, sizeof(short)); /* JMP 25-01-97 */
	/*    fld->fld_acmp = (short *)SCR_palloca(sizeof(short) * i, sizeof(short)); /* JMP 25-01-97 */
	    O_read_lena((U_ch **)&(fld->fld_acmp), sizeof(short));
	}
    }

    return(0);
}

/* === OBJ_READ_FNKEYS ==========================================
   ============================================================== */
OBJ_read_fnkeys(fk)
FNKEYS  *fk;
{
    int     i;
    FNKEY   *fnk;

    O_read((char *)fk, sizeof(FNKEYS));

    fk->fk_fnks = (FNKEY *)SCR_palloc(fk->fk_nb_fnks * sizeof(FNKEY));
    O_read((char *)fk->fk_fnks, fk->fk_nb_fnks * sizeof(FNKEY));
    fnk = fk->fk_fnks;
    for(i = 0 ; i < fk->fk_nb_fnks ; i++, fnk++) {
	if(fnk->fnk_cmt != 0)
	    O_read_text(&(fnk->fnk_cmt));
	if(fnk->fnk_prot != 0)
	    O_read_text(&(fnk->fnk_prot));
    }
    return(0);
}
/* === OBJ_READ_ACTION ==========================================
   ============================================================== */
OBJ_read_action(act)
ACTION  *act;
{
    int     i;

    /* READ STRUCTURE ACTION */
    O_read((char *)act, sizeof(ACTION));

    /* READ A_ACTION */
    if(act->act_nb != 0) {
	act->act_aa = (A_ACTION *)SCR_palloc(act->act_len);
	O_read((char *)act->act_aa, act->act_len);
    }
    return(0);
}

/* === OBJ_READ_DRAWS ===========================================
   ============================================================== */
OBJ_read_drws(dw)
DRAWS   *dw;
{
    int     i;
    DRAW    *drw;

    /* READ STRUCTURE */
    if(dw->dw_nb_drws == 0)
	return(0);

    dw->dw_drws = (DRAW *) SCR_malloc(sizeof(DRAW) * dw->dw_nb_drws);
    O_read((char *)dw->dw_drws, sizeof(DRAW) * dw->dw_nb_drws);
    /* IF TYPE TEXT READ TEXT */
    for(i = 0 ; i < dw->dw_nb_drws ; i++) {
	drw = dw->dw_drws + i;
	O_read_text(&(drw->drw_text));
    }
    return(0);
}

/* === OBJ_READ_MENU ============================================
   ============================================================== */
OBJ_read_menu(mn)
MENU    *mn;
{
    int     i;

    /* READE STRUCT MENU */
    O_read((char *)mn, sizeof(MENU));

    if(mn->mn_opt_mem == 0) return(0);

    /* READ TITLE AND FOOTNOTE */
    O_read_text(&(mn->mn_title));
    O_read_text(&(mn->mn_foot));

    /* READ HELP */
    O_read_text(&(mn->mn_help));

    /* READ OPTION */
    if(mn->mn_nb_opts != 0) {
	mn->mn_opts = (OPTION *)SCR_palloc(sizeof(OPTION) * mn->mn_nb_opts);
	OBJ_read_options(mn->mn_nb_opts, mn->mn_opts);
    }
    return(0);
}

/* === OBJ_READ_OPTIONS =========================================
   ============================================================== */
OBJ_read_options(nb_opt, opts)
int     nb_opt;
OPTION  *opts;
{
    int     j;
    OPTION  *opt;

    /* READ STRUCT OPT */
    O_read((char *)opts, sizeof(OPTION) * nb_opt);

    for(j = 0 ; j < nb_opt ; j++) {
	opt = opts + j;

	/* READ TEXT OF OPTION */
	O_read_text(&(opt->opt_text));
	O_read_text(&(opt->opt_help));
	O_read_text(&(opt->opt_pg_cmt));
	O_read_text(&(opt->opt_prot));
    }
    return(0);
}


/* === OBJ_ADJUST_REFERENCE =====================================
   ============================================================== */
OBJ_adjust_reference()
{
    int     i;

    for(i = 0 ; i < SCR_READ_FILES.tbl_nb_fl ; i++)
	switch(TYPE16(SCR_READ_FILES.tbl_fl[i]->fl_type)) {
	    case SCR_TYPE_MPAGE:
		OBJ_mpage_adjust((MPAGE *)SCR_READ_FILES.tbl_fl[i]->fl_ptr);
		break;
	}
    return(0);
}
OBJ_read_abar(ab)
A_BAR   *ab;
{
    int     i;

    O_read((char *)ab, sizeof(A_BAR));
    O_read_text(&(ab->ab_help));

    ab->ab_opts      = (A_OPT  *)SCR_palloc(sizeof(A_OPT) * ab->ab_nb_opts);
    O_read((char *)ab->ab_opts, sizeof(A_OPT) * ab->ab_nb_opts);

    for(i = 0 ; i < ab->ab_nb_opts ; i++) {
	O_read_text(&(ab->ab_opts[i].ao_text));
	O_read_text(&(ab->ab_opts[i].ao_help));
	O_read_text(&(ab->ab_opts[i].ao_prot));
    }
    return(0);
}

OBJ_read_appl(ap)
APPL    *ap;
{
    int     i;

    O_read((char *)ap, sizeof(APPL));
    O_read_text(&(ap->ap_title));
    O_read_text(&(ap->ap_foot));
    OBJ_read_drws(&(ap->ap_drws));
    /* BP_M 18-04-1997 */
    if(ap->ap_nb_tb) {
	ap->ap_toolb = (U_sh *)SCR_palloc(sizeof(U_sh) * ap->ap_nb_tb);
	O_read((char *)ap->ap_toolb, sizeof(U_sh) * ap->ap_nb_tb);
    }
    return(0);
}

OBJ_read_cmt()
{
    short   i;
    char    buf[128];

    O_read((char *)&i, sizeof(short));
    if(i) {
	O_read_str(buf);
/*        strcpy(SCR_CMT_FILE, buf); /* JMP 05-11-97 */
	O_read((char *)&SCR_CMT, sizeof(_SCR_CMT));
    }
    return(0);
}


OBJ_read_tpages(tp)      /* BP_M 26-01-97 */
TPAGES   *tp;
{
    int     i, j;

    O_read((char *)tp, sizeof(TPAGES));
    O_read_text(&(tp->tp_title));
    tp->tp_pgsnbs = (short *)SCR_malloc(sizeof(short) * tp->tp_nb);
    O_read((char *)tp->tp_pgsnbs, sizeof(short) * tp->tp_nb);
    tp->tp_pgstitles = (U_ch **)SCR_malloc(sizeof(U_ch *) * tp->tp_nb);
    for(i = 0 ; i < tp->tp_nb ; i++) {
	tp->tp_pgstitles[i] = (U_ch *)1;
	O_read_text(&(tp->tp_pgstitles[i]));
	}
    return(0);
}

/*
OBJ_read_report(rp)
REPORT  *rp;
{
    short   i, j, len;
    RFLD    *rf;
    RIFLD   *ri;

    O_read((char *)rp, sizeof(REPORT));
    O_read_text(&(rp->rp_filetxt));
    rp->rp_rfld = (RFLD *)SCR_palloc(sizeof(RFLD) * rp->rp_nbfld);
    O_read((char *)rp->rp_rfld, sizeof(RFLD) * rp->rp_nbfld);
    rf = rp->rp_rfld;
    for(i = 0 ; i < rp->rp_nbfld ; i++, rf++) {
	O_read_text(&(rf->rf_name));
	O_read_text(&(rf->rf_fmt));
	O_read((char *)&len, sizeof(short));
	if(len > 0) {
	    rf->rf_val1 = SCR_palloc(len);
	    O_read((char *)rf->rf_val1, len);
	}
	O_read((char *)&len, sizeof(short));
	if(len > 0) {
	    rf->rf_val2 = SCR_palloc(len);
	    O_read((char *)rf->rf_val2, len);
	}
	rf->rf_fld = (RIFLD *)SCR_palloc(sizeof(RIFLD) * rf->rf_nbi);
	ri = rf->rf_fld;
	O_read((char *)ri, sizeof(RIFLD) * rf->rf_nbi);
	for(j = 0 ; j < rf->rf_nbi ; j++, ri++)
	    if(ri->rf_nbapp != 0) {
		ri->rf_apply = SCR_palloc(sizeof(short) * ri->rf_nbapp);
		O_read((char *)ri->rf_apply, sizeof(short) * ri->rf_nbapp);
	    }
    }
}
*/


