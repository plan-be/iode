#include "scr4.h"

extern int     SCR_flag_fork;  /* BP_M 12-12-2009 13:49 */

/* ====================================================================
Chaque action SCR, comme tout autre objet, est identifiée par un numéro.
C'est ce numéro qui est stocké dans les structures définissant les
objets. ACT_exec_nu() sera utilisée lorsqu'une action doit être exécutée
et qu'on en connaît que le numéro.

&EX
    Exécution de la DELETE_FN de l'isam is :

	ACT_exec_nu(is->is_delete_act);
&TX
&SA ACT_exec(), ACT_ufn_exec()
======================================================================== */

ACT_exec_nu(nu)
int     nu;
{
    ACTION      *act;
    FILE_TYPE   *flt;
    int         mem, rc;

    if(nu == 0) return(0);

    flt = SCR_READ_FILES.tbl_fl[nu];
    mem = flt->fl_mem;
    if(mem == 0)
	act = (ACTION *)SCR_find_load_obj(nu);
    else
	act = (ACTION *)flt->fl_ptr;
    rc = ACT_exec(act);
    if(mem == 0)
	SCR_unload_obj(nu);
    return(rc);
/*  OLD JMP
    if(nu == 0) return(0);
    return(ACT_exec(SCR_PTR(nu)));
*/
}

/* ====================================================================
Exécute une action SCR.

&EX
    Exécution de l'action start définie dans les sources SCR :

	ACTION start {
	    C_FN my_init();
	    APPL_RUN appl
	}

    Dans le programme C :

	ACT_exec(start);
&TX
&SA ACT_exec_nu(), objet ACTION
======================================================================== */

ACT_exec(act)
ACTION  *act;
{
    int     i, j, rc = 0, rc2;
    STATUS  *sta;
    char    *buf;

    if(act == 0) return(0);
    if(SCR_USER_CATG >= 0 /*&& act->act_catg >= 0*/ && act->act_catg < SCR_USER_CATG) { /* JMP 4.23 19-12-95 */
	PG_display_error(SCR_err_txt(113));
	return(-1);
	}
    sta = SCR_save_status();
    buf = (char *)act->act_aa;
    SCR_flag_fork = -1;
    SCR_PG_ABORT = SCR_APP_ABORT = 0;
    for(i = 0 ; i < act->act_nb ; i++) {
	rc = ACT_exec_aa((A_ACTION *)buf, 0);
	if(SCR_APP_ABORT || SCR_PG_ABORT) break;
	if(rc != 0) {
	    for(j = i + 1 ; j < act->act_nb ; j++) {
		buf += ((A_ACTION *)buf)->aa_len;
		rc2 = ACT_exec_aa((A_ACTION *)buf, 1);  /* BP_M 27-07-2010 13:39 */
		if(rc2 == 0) break; /* BP_M 27-07-2010 13:39 */
	    }
	    break;
	}
	buf += ((A_ACTION *)buf)->aa_len;
    }
    if(SCR_flag_fork == 0)
	exit(0);
    SCR_reset_status(sta);
    return(rc);
}

extern int SCR_COMP_PG_ABORT;

/*NH*/
/* =+====== ACT_exec_aa ====================================
    Execute one atomic ACTION
	aa : IN : pointer to the A_ACTION
   ========================================================= */

ACT_exec_aa(aa, flag)
A_ACTION  *aa;
int       flag;
{
    int     i, nk;           /* JMP38 02-09-92 */
    short   keys;            /* JMP38 02-09-92 */
    char    *ptr = SCR_PTR(ACT_find_lg(aa, 0));
    char    *buf = ACT_find_str(aa, 0);

/*    if(flag && aa->aa_fn != SCR_IF_ERROR) return(0); */
    if(flag && aa->aa_fn != SCR_IF_ERROR) return(-1); /* JMP 23-10-97 */

    switch(aa->aa_fn) {
	case ACT_PG_RESET   :   PG_reset((PAGE *)ptr); break;
	case ACT_PG_DISPLAY :   PG_display_im((PAGE *)ptr); break;
	case ACT_PG_UNDISPLAY:  PG_undisplay((PAGE *)ptr); break;
	case ACT_PG_DISPLAY_FLDS: PG_display_flds_no_cmp((PAGE *)ptr); break;
	case ACT_PG_SLIDE :     PG_display_slide((PAGE *)ptr); break;
	case ACT_MN_EDIT :      MN_edit((MENU *)ptr); break;
	case ACT_INFO :         MMT_show_file(buf, 2, 1, 20, 76); break;    /* BP_M 25-10-95 */
	case ACT_PG_EDIT :      i = PG_edit_window((PAGE *)ptr);    /* BP_M 25-10-95 */
				if(SCR_COMP_PG_ABORT) SCR_PG_ABORT = 0; /* JMP 15-06-96 */
/*                                SCR_LKEY = SCR_ENTER;  */
				return(i);
	case ACT_PG_REEDIT :    return(PG_reedit((PAGE *)ptr));      /* JMP_M 4.19 16-05-95 */
	case ACT_AB_DISPLAY:    AB_display((A_BAR *)ptr);    break;
/*      case ACT_AB_UNDISPLAY:  AB_undisplay((A_BAR *)ptr);  break;   */
	case ACT_AB_EDIT:       AB_edit((A_BAR *)ptr);       break;
	case ACT_APPL_RUN:      AP_run((APPL *)ptr);        break;

	case ACT_SYSTEM :       ACT_exec_system(buf); break;
	case ACT_ASK :          return(SCR_confirme(buf));
	case ACT_ASKI :         return(SCR_ask(1, buf));     /* JMP 24-01-96 */
	case ACT_ASKS :         return(SCR_ask(2, buf));     /* JMP 24-01-96 */
	case SCR_MESSAGE:       PG_display_error(buf); break;
	case SCR_PROT :         return(SCR_check_prot_vt(buf)); /* JMP 04-06-01 */

	case ACT_REC_KEY :
/*            keys = (int *)buf; */
	    for(nk = 0 ; ; nk++) {           /* JMP38 02-09-92 */
		memcpy(&keys, buf + nk * sizeof(short), sizeof(short));
		if(keys == -1) break;
		}
	    for(i = nk - 1 ; i >= 0 ; i--) { /* JMP38 02-09-92 */
		memcpy(&keys, buf + i * sizeof(short), sizeof(short));
		if(keys == SCR_L_KEY) keys = SCR_LKEY;      /* BP_M 26-10-95 */
		if(keys == SCR_GET_KEY) {                   /* BP_M 26-10-95 */
		    SCR_get_key();
		    continue;
		}
		SCR_record_key(keys);
	    }
	    break;

	case ACT_C_FN:          return(ACT_functions(ACT_find_lg(aa, 0)));
	case ACT_PG_ABORT:      SCR_PG_ABORT = 1; break;
	case ACT_FLD_SKIP:      SCR_FLD_SKIP = 1; break;
	case ACT_FLD_REEDIT:    SCR_FLD_REEDIT = 1; break;
	case ACT_APPL_MAIN:
	case ACT_APPL_QUIT:     SCR_APP_ABORT = aa->aa_fn; break;
	case ACT_HELPFILE:      strcpy(HLP_NAME, buf); break;
	case SCR_HELP_KEY:      HLP_edit(buf); break;
	case SCR_LOCK_SCREEN:   SCR_lock_screen(SCR_LOCK_TXT); break;
	case SCR_STOP_ALARM:    SCR_set_alarm(0, 0); break;
	case SCR_RESTART_ALARM: SCR_restart_alarm(); break;

	case SCR_IF_ERROR     :
	    if(flag)
		return(ACT_exec_nu((int)ACT_find_lg(aa, 0))); /* JMP 23-10-97 */
	    break;

	case SCR_SET_ALARM:
/*
	    SCR_set_alarm((int)ACT_find_lg(aa, 0),
			  ACT_exec_nu((int)ACT_find_lg(aa, 1)));
*/
	    break;

	case SCR_TYPE_ACT:  /* BP_M 27-03-2012 14:39 */
	    ACT_exec_nu((int)ACT_find_lg(aa, 0));
	    break;

#ifdef UNIX
	case ACT_FORK:
	    SCR_flag_fork = fork();
	    if(SCR_flag_fork != 0) return(-1);
	    break;
#endif

	case ACT_EDIT_TPAGES:
	    return(TP_edit_window((TPAGES *)ptr));

	default :
	    if(PR_action(aa) != 0)
		return(ACT_exec_aa_isam(aa));       /* s_edis.c */
    }
    return(0);
}
/*
SCR_record_str(str)
char    *str;
{
    int     i;

    for(i = strlen(str) - 1 ; i >= 0 ; i--)
	SCR_record_key(str[i]);
}
/* JMP 17-04-92 */




