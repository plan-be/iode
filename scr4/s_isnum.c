#include "scr4.h"

/*NH******* ANCIEN S_SQL.C *****************/

extern int      SCR_STRIP_TBLNAME_NUM;
extern ISAM     *ScrAutoNum;

/*NH*/
long IS_IncrAutoNum(file)
char    *file;
{
    long    l;
    char    buf[256];

    IS_check_autonum();     /* BP_M 07-11-1998 */

    if(SCR_STRIP_TBLNAME_NUM)
	IS_strip_tblname(buf, file);
    else
	strcpy(buf, file);

    IS_empty_rec(ScrAutoNum);
    IS_set_field(ScrAutoNum, 0, buf);
    IS_open_isam(ScrAutoNum, SCR_UPDATE, SCR_NO_LOCK);  /* JMP 03-12-99 */
    if(IS_search(ScrAutoNum, 0, SCR_EQ, (char *)0) == 0) {
	IS_get_field(ScrAutoNum, 1, &l);
	l++;
	IS_set_field(ScrAutoNum, 1, &l);
	if(IS_rewrite(ScrAutoNum) != 0) {
	    /*PG_display_error(IS_error_text());/* JMP 03-12-99 */
	    return(-1L);
	}
	return(l);
    }
    else {
	IS_empty_rec(ScrAutoNum);
	IS_set_field(ScrAutoNum, 0, buf);
	l = 1L;                                 /* BP_M 26-06-2002 21:12 */
	IS_set_field(ScrAutoNum, 1, &l);        /* BP_M 26-06-2002 21:12 */
	if(IS_write(ScrAutoNum) != 0) {
	    /*PG_display_error(IS_error_text());/* JMP 03-12-99 */
	    return(-1);
	}
	return(1L);                             /* BP_M 26-06-2002 21:12 */
    }
}

long IS_GetAutoNum(file)
char    *file;
{
    long    l;
    char    buf[256];

    IS_check_autonum();     /* BP_M 07-11-1998 */

    if(SCR_STRIP_TBLNAME_NUM)
	IS_strip_tblname(buf, file);
    else
	strcpy(buf, file);

    IS_empty_rec(ScrAutoNum);
    strcpy(buf, file);
    IS_set_field(ScrAutoNum, 0, buf);
    IS_open_files(ScrAutoNum, 0L);
    if(IS_search(ScrAutoNum, 0, SCR_EQ, (char *)0) == 0) {
	IS_get_field(ScrAutoNum, 1, &l);
	return(l);
    }
    return(-1L);
}

IS_SetAutoNum(file, l)
char    *file;
long    l;
{
    char    buf[256];

    IS_check_autonum();     /* BP_M 07-11-1998 */

    if(SCR_STRIP_TBLNAME_NUM)
	IS_strip_tblname(buf, file);
    else
	strcpy(buf, file);

    IS_empty_rec(ScrAutoNum);
    strcpy(buf, file);
    IS_set_field(ScrAutoNum, 0, buf);
    IS_open_files(ScrAutoNum, 0L);
    if(IS_search(ScrAutoNum, 0, SCR_EQ, (char *)0) == 0) {
	IS_set_field(ScrAutoNum, 1, &l);
	if(IS_rewrite(ScrAutoNum) != 0)
	    return(-1);
	return(0);
    }
    return(-1);
}

long SCR_auto_num_sql(filename)
char    *filename;
{
    return(IS_IncrAutoNum(filename));
}

/*NH*/
IS_CalcAutoNum(is)
ISAM    *is;
{
    long    c_num;
    int     i;
    char    buf[256];

    IS_check_autonum();     /* BP_M 07-11-1998 */
    if(SCR_STRIP_TBLNAME_NUM)
	IS_strip_tblname(buf, is->is_filename);
    else
	strcpy(buf, is->is_filename);

    for(i = 0 ; i < is->is_nb_flds ; i++)
	if(is->is_flds[i].isf_type == SCR_AUTO) {
	    IS_get_field(is, i, (char *)&c_num);
	    if(c_num != 0) return(0);
	    c_num = SCR_auto_num_sql(buf);
	    if(c_num < 0) return(-1);
	    IS_set_field(is, i, (char *)&c_num);
	    break;
	    }

    return(0);
}


/*NH*/
IS_CreateAutoNum(is)
ISAM    *is;
{
    long    l;
    int     i;
    char    buf[256];

    for(i = 0 ; i < is->is_nb_flds ; i++)
	if(is->is_flds[i].isf_type == SCR_AUTO) break;
    if(i == is->is_nb_flds) return(0);

    IS_check_autonum();     /* BP_M 07-11-1998 */

//    if(ScrAutoNum->is_isam == 0L) IS_create_struct(ScrAutoNum);
    IS_empty_rec(ScrAutoNum);
    if(SCR_STRIP_TBLNAME_NUM)
	IS_strip_tblname(buf, is->is_filename);
    else
	strcpy(buf, is->is_filename);
    IS_set_field(ScrAutoNum, 0, buf);
    IS_open(ScrAutoNum, SCR_UPDATE, SCR_NO_LOCK);
    if(IS_search(ScrAutoNum, 0, SCR_EQ, (char *)0) == 0) {
	l = 0L;
	IS_set_field(ScrAutoNum, 1, &l);
	if(IS_rewrite(ScrAutoNum) != 0)
	    return(-1);
    }
    else {
	IS_empty_rec(ScrAutoNum);
	IS_set_field(ScrAutoNum, 0, buf);
	if(IS_write(ScrAutoNum) != 0)
	    return(-1);
    }

    return(0);
}

/*NH*/
IS_check_autonum()
{
    FILE_TYPE   *flt;
    extern ISAM *ScrAutoNum;

    if(ScrAutoNum) return(0);

    flt = (FILE_TYPE *)SCR_find_flt("scr_auto_num");
    if(flt == 0) {
	/*PG_display_error("No ISAM structure 'scr_auto_num'"); /* JMP 03-12-99 */
	return(-1);
	}
    else {
	ScrAutoNum = (ISAM *)flt->fl_ptr;
	if(IS_open_isam(ScrAutoNum, SCR_READ, SCR_NO_LOCK) != 0)
	    return(IS_create_db(ScrAutoNum));
	else
	    return(0);
    }
}


/* ====================================================================
Fixe le champ AUTO éventuel du record de l'ISAM. Pour ce faire, se base
sur la valeur inscrite dans le fichier .num de la database ou, dans le cas
d'une database relationnelle, dans la table scr_auto_num.

&RT 0 en cas de réussite (pas de fichier .num ou valeur incorrecte dans
ce fichier) , -1 sinon.

&NO Cette fonction est appelée par IS_write() avant l'écriture
proprement dite.

&EX
    AddClient()
    {
	IS_empty_rec(is_client);
	IS_calc_auto_num(is_client);
	SCR_isam_to_page(p_is_client, is_client);
	if(PG_edit_window(p_is_client) != 0) return;
	SCR_page_to_isam(p_is_client, is_client);
	if(SCR_LKEY != SCR_F10) return;
	if(IS_write(is_client)) {
	    PG_display_error(IS_error_text());
	    goto again;
	    }
	return;
    }
&TX
&SA SCR_auto_num(), SCR_current_auto_num()
=======================================================================*/

IS_calc_auto_num(is)
ISAM    *is;
{
    switch(is->is_type_dbd) {
	case CTREE4  : return(CT4ISCalcAutoNum(is));
	case ODBC    :
	case ORACLE  :
	case POSTGRES:
	case IS_MYSQL:
	case INFORMIX_DB: return(IS_CalcAutoNum(is));
	default : return(-1);
	}
}

long IS_get_auto_num(is)
ISAM    *is;
{
    switch(is->is_type_dbd) {
	case CTREE4  : return(ScrGetAutoNum(is->is_filename));
	case ODBC    :
	case ORACLE  :
	case POSTGRES:
	case IS_MYSQL:
	case INFORMIX_DB: return(IS_GetAutoNum(is->is_filename));
	default : return(-1);
	}
}

long IS_incr_auto_num(is)
ISAM    *is;
{
    switch(is->is_type_dbd) {
	case CTREE4  : return(ScrIncrAutoNum(is->is_filename));
	case ODBC    :
	case ORACLE  :
	case POSTGRES:
	case IS_MYSQL:
	case INFORMIX_DB: return(IS_IncrAutoNum(is->is_filename));
	default : return(-1);
	}
}

IS_set_auto_num(is, l)
ISAM    *is;
long    l;
{
    switch(is->is_type_dbd) {
	case CTREE4  : return(ScrSetAutoNum(is->is_filename, l));
	case ODBC    :
	case ORACLE  :
	case POSTGRES:
	case IS_MYSQL:
	case INFORMIX_DB: return(IS_SetAutoNum(is->is_filename, l));
	default : return(-1);
	}
}

/* ====================================================================
Lit dans le fichier filename.num la valeur qui s'y trouve et retourne
cette valeur. Utilisé dans les ISAM dont un champ est AUTO.

&RT valeur du fichier (long) ou -1 si le fichier n'est pas trouvé

&SA SCR_auto_num()
=======================================================================*/

long SCR_current_auto_num(is)
ISAM    *is;
{
    return(IS_get_auto_num(is));
}


SCR_set_auto_num_sql(filename, l)
char    *filename;
long    l;
{
    return(IS_SetAutoNum(filename, l));
}

/*NH*/
/* ====================================================================
Fixe le champ AUTO éventuel du record de l'ISAM. Pour ce faire, se base
sur la valeur inscrite dans le fichier .num de la database.

&RT 0 en cas de réussite (pas de fichier .num ou valeur incorrecte dans
ce fichier) , -1 sinon.

&NO Cette fonction est appelée par IS_write() avant l'écriture
proprement dite.

&EX
    AddClient()
    {
	IS_empty_rec(is_client);
	IS_auto_num(is_client);
	SCR_isam_to_page(p_is_client, is_client);
	if(PG_edit_window(p_is_client) != 0) return;
	SCR_page_to_isam(p_is_client, is_client);
	if(SCR_LKEY != SCR_F10) return;
	if(IS_write(is_client)) {
	    PG_display_error(IS_error_text());
	    goto again;
	    }
	return;
    }
&TX
&SA SCR_auto_num(), SCR_current_auto_num()
=======================================================================*/

CT4ISCalcAutoNum(is)
ISAM    *is;
{
    long    c_num;
    int     i;

    SCR_std_filename(is->is_filename); /* JMP 04-11-98 */
    for(i = 0 ; i < is->is_nb_flds ; i++)
	if(is->is_flds[i].isf_type == SCR_AUTO) {
	    IS_get_field(is, i, (char *)&c_num);
	    if(c_num != 0) return(0);
	    c_num = ScrIncrAutoNum(is->is_filename);
	    if(c_num < 0) return(-1);
	    IS_set_field(is, i, (char *)&c_num);
	    break;
	    }

    return(0);
}
