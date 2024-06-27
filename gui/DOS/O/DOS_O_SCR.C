#include "iodedos.h"

ODE_doc(kdb, pos, type)
KDB *kdb;
int pos, type;
{
    char    **lst = NULL;

    if(KNB(kdb) == 0) return(0);
    switch(KTYPE(kdb)) {
	case COMMENTS :
	case LISTS :
	case SCALARS :
	case TABLES :
	case VARIABLES :
	    lst = A_init(KONAME(kdb, pos)); break;
	case EQUATIONS :
	    lst = A_clec(KONAME(kdb, pos), KECLEC(kdb, pos), type); break;
	case IDENTITIES :
	    lst = A_clec(KONAME(kdb, pos), KICLEC(kdb, pos), type); break;
	default    : break;
    }

    if(SCR_tbl_size(lst)) ODE_scroll(K_WS[type], lst);

    SCR_free_tbl(lst);
    return(0);
}

ODE_tins(type, where)
int *type, *where;
{

    PG_edit_window(vkp_tins);
    if(SCR_LKEY != SCR_F10) return(-1);

    switch(vkp_tins_TYPE) {
    case 0 : *type = KT_CELL; break;
    case 1 : *type = KT_TITLE; break;
    case 2 : *type = KT_LINE; break;
    case 3 : *type = KT_FILES; break;
    case 4 : *type = KT_MODE; break;
    case 5 : *type = KT_DATE; break;
    }

    *where = vkp_tins_WHERE;
    return(*type);
}

ODE_status()
{
    SWSTAT  st;

    SW_status(&st);
    vkp_status_NBALLOC   = st.st_nballoc - st.st_nbfree;
    vkp_status_NBBLKS    = st.st_nbblks;
    vkp_status_NBSEGS    = st.st_nbsegs;
    vkp_status_SEGSIZE   = st.st_segsize;
    vkp_status_EMSALLOC  = st.st_emsalloc;
    vkp_status_EMSAFREE  = st.st_emsafree;
    vkp_status_EMSAUSED  = st.st_emsalloc - st.st_emsafree;
    vkp_status_FILEALLOC = st.st_filealloc;
    vkp_status_FILEAFREE = st.st_fileafree;
    vkp_status_FILEAUSED = st.st_filealloc - st.st_fileafree;
    vkp_status_CONVALLOC = st.st_convalloc;
    vkp_status_CONVAFREE = st.st_convafree + st.st_convfree;
    vkp_status_CONVAUSED = st.st_convalloc - st.st_convafree;
    return(0);
}

SB_status()
{
    ODE_status();
    PG_edit_window(vkp_status);
    return(0);
}

char    **A_clec(name, clec, type)
char    *name;
CLEC    *clec;
int     type;
{
    int     i, nlst = 0, scl;
    char    **lst = NULL;
    LNAME   *lname;

    lname = &(clec->lnames[0]);
    if(type != SCALARS) SCR_add_ptr(&lst, &nlst, name);

    for(i = 0; i < clec->nb_names; i++) {
	scl = L_ISCOEF(lname[i].name);
	if((scl && type != SCALARS) || (!scl && type == SCALARS)) continue;
	SCR_add_ptr(&lst, &nlst, lname[i].name);
	}

    SCR_add_ptr(&lst, &nlst, NULL);
    return(lst);
}






