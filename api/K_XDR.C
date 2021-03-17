#include "iode.h"

#ifdef INTEL
void K_xdrPINT(a)
unsigned    char    *a;
{
}

void K_xdrPLONG(a)
long    *a;
{
}

void K_xdrKDB(ikdb, okdb)
KDB *ikdb, **okdb;
{
    if(okdb == NULL) return;

    *okdb = (KDB *)SW_nalloc(sizeof(KDB));
    memcpy(*okdb, ikdb, sizeof(KDB));
}

int K_cxdr(ptr, xdr_ptr)
unsigned char *ptr, **xdr_ptr;
{
    return(K_oxdr(ptr, xdr_ptr));
}

K_exdr(ptr, xdr_ptr)
unsigned char *ptr, **xdr_ptr;
{
    return(K_oxdr(ptr, xdr_ptr));
}
K_ixdr(ptr, xdr_ptr)
unsigned char *ptr, **xdr_ptr;
{
    return(K_oxdr(ptr, xdr_ptr));
}
K_lxdr(ptr, xdr_ptr)
unsigned char *ptr, **xdr_ptr;
{
    return(K_oxdr(ptr, xdr_ptr));
}
K_sxdr(ptr, xdr_ptr)
unsigned char *ptr, **xdr_ptr;
{
    return(K_oxdr(ptr, xdr_ptr));
}
K_txdr(ptr, xdr_ptr)
unsigned char *ptr, **xdr_ptr;
{
    return(K_oxdr(ptr, xdr_ptr));
}
K_vxdr(ptr, xdr_ptr)
unsigned char *ptr, **xdr_ptr;
{
    return(K_oxdr(ptr, xdr_ptr));
}

K_oxdr(ptr, xdr_ptr)
unsigned char *ptr, **xdr_ptr;
{
    int     len;

    if(xdr_ptr == NULL) return(-1);

    len = P_len(ptr);
    *xdr_ptr = SW_nalloc(len);
    memcpy(*xdr_ptr, ptr, len);
    return(0);
}

#else


#define K_xdrSHORT(a)   XDR_rev(a, 1, sizeof(short))
#define K_xdrLONG(a)    XDR_rev(a, 1, sizeof(long))
#define K_xdrREAL(a)    XDR_rev(a, 1, sizeof(IODE_REAL))

K_xdrPINT(a)
unsigned char  *a;
{
    XDR_rev(a, 1, sizeof(U_sh));
}

K_xdrPLONG(a)
unsigned char  *a;
{
    XDR_rev(a, 1, sizeof(long));
}

K_xdrPERIOD(a)
unsigned char  *a;
{
    XDR_rev(a, 2, sizeof(long));    /* PERIOD LONG */
}

K_xdrSMPL(a)
unsigned char  *a;
{
    K_xdrPERIOD(a);
    a += sizeof(PERIOD);
    K_xdrPERIOD(a);
    a += sizeof(PERIOD);
    K_xdrSHORT(a);
}

K_xdrPACK(ptr, mode)
unsigned char   *ptr;
int             mode;
{
    OSIZE   *pos, nb;
    int     i;

    pos = (OSIZE *) ptr;
    nb = pos[1];

    if(mode == 0) {
	/* intel read */
	K_xdrPINT(&nb);
    }
    /* pack = 2 + nb elements + data */
    for(i = 0; i < nb + 2; i++) K_xdrPLONG(pos + i);
}

K_xdrKDB(ikdb, okdb)
KDB *ikdb, **okdb;
{
    short   type;
    KDB     *xdr_kdb;
    SAMPLE  *smpl;

    type = ikdb->k_type;

    if(okdb == NULL) {
	/* intel read */
	xdr_kdb = ikdb;
	K_xdrSHORT(&type);
    }
    else {
	/* intel write */
	xdr_kdb = SW_nalloc(sizeof(KDB));
	memcpy(xdr_kdb, ikdb, sizeof(KDB));

	*okdb = xdr_kdb;
    }

    XDR_rev(&(xdr_kdb->k_type), 1, sizeof(short));
    XDR_rev(&(xdr_kdb->k_mode), 1, sizeof(short));
    XDR_rev(&(xdr_kdb->k_nb), 1, sizeof(long));

    if(type == K_VAR) {
	smpl = (SAMPLE *) KDATA(xdr_kdb);
	K_xdrSMPL(smpl);
    }
}

K_xdrCVAR(a)
unsigned char  *a;
{
    CVAR    *cv = a;

    XDR_rev(a, 4, sizeof(short));
    K_xdrPERIOD(&(cv->per));
}

K_xdrCLEC(expr, mode)
CLEC    *expr;
int     mode;
{
    long    tot_lg;
    short   nb_names;
    int     pos;

    nb_names = expr->nb_names;
    tot_lg =   expr->tot_lg;
    if(mode == 0) {
	K_xdrLONG(&tot_lg);
	K_xdrSHORT(&nb_names);
    }

    XDR_rev(&(expr->tot_lg), 1, sizeof(long));
    XDR_rev(&(expr->exec_lg), 1, sizeof(long));
    XDR_rev(&(expr->nb_names), 1, sizeof(short));

    pos = sizeof(CLEC) + (nb_names - 1) * sizeof(LNAME);
    return(K_xdrCLEC_sub((char *) expr + pos, tot_lg - pos, mode));
}

K_xdrCLEC_sub(expr, lg, mode)
char    *expr;
int     lg, mode;
{
    int     j, keyw, nvargs;
    short   len, s;

    for(j = 0 ; j < lg ; ) {
	keyw = expr[j++];
	if(keyw == L_VAR || keyw == L_VART) {
	    K_xdrCVAR(expr + j);
	    j += sizeof(CVAR);
	}
	else switch(keyw) {
	    case L_PLUS  :
	    case L_MINUS :
	    case L_TIMES : break;
	    case L_DCONST    :
		K_xdrREAL(expr + j);
		j += s_dbl;
		break;
	    case L_LCONST    :
		K_xdrLONG(expr + j);
		j += s_long;
		break;
	    case L_COEF :
		K_xdrCVAR(expr + j);
		j += sizeof(CVAR);
		break;
	    case L_PERIOD :
		K_xdrPERIOD(expr + j);
		j += sizeof(PERIOD);
		K_xdrSHORT(expr + j);
		j += s_short;
		break;
	    default :
		if(is_op(keyw) || is_val(keyw)) {
		    break;
		}
		if(is_fn(keyw)) {
		    /* nargs  = char */
		    j++;
		    break;
		}
		if(is_tfn(keyw)) {
		    /* nargs  = char */
		    memcpy(&len, expr + j + 1, sizeof(short));
		    if(mode == 0) K_xdrSHORT(&len); /* intel read */
		    K_xdrSHORT(expr + j + 1);

		    j += 1 + sizeof(short);
		    K_xdrTFN(expr + j, len, mode);

		    j += len;
		    break;
		}
		if(is_mtfn(keyw)) {
		    /* nargs  = char */
		    /* nvargs  = char */
		    nvargs = expr[j + 1];
		    memcpy(&len, expr + j + 2, sizeof(short));
		    if(mode == 0) K_xdrSHORT(&len); /* intel read */
		    K_xdrSHORT(expr + j + 2);

		    j += 2 + sizeof(short);
		    K_xdrMTFN(expr + j, mode, nvargs);

		    j += len;
		    break;
		}
	 }
    }
    return(0);

}

K_xdrTFN(expr, len, mode)
unsigned char    *expr;
short   len;
int     mode;
{
    int lg = len;

    K_xdrCLEC_sub(expr, lg, mode);
}

K_xdrMTFN(expr, mode, nvargs)
unsigned char   *expr;
int             mode, nvargs;
{
    int     i;
    short   len1, pos = 0;

    for(i = 0 ; i < nvargs ; i++) {
	memcpy(&len1, expr + pos, sizeof(short));
	if(mode == 0) K_xdrSHORT(&len1); /* intel read */
	K_xdrSHORT(expr + pos);
	K_xdrCLEC_sub(expr + pos + sizeof(short), len1, mode);
	pos += sizeof(short) + len1;
	}
}

K_xdrCELL(ptr, type, mode)
unsigned    char    *ptr;
char        type;
int         mode;
{
    if(type == KT_LEC) {
	if(mode == 0) {
	    K_xdrPACK(ptr, mode);
	    K_xdrCLEC(P_get_ptr(ptr, 1), mode);
	}
	else {
	    K_xdrCLEC(P_get_ptr(ptr, 1), mode);
	    K_xdrPACK(ptr, mode);
	}
    }
}

K_xdrTBL(pack, mode)
unsigned    char    *pack;
int         mode;
{
    int             i, j, p, len;
    short           nc, nl;
    unsigned char   *pdiv;
    TBL             *tbl;
    TCELL           *cell, *pcell;
    TLINE           *line, *pline;

    /* TBL:0 */
    tbl = P_get_ptr(pack, 0);
    nc = T_NC(tbl);
    nl = T_NL(tbl);
    if(mode == 0) { /* intel read */
	K_xdrSHORT(&nc);
	K_xdrSHORT(&nl);
    }
    XDR_rev(tbl, 4, sizeof(short));
    XDR_rev(&(tbl->t_zmin), 4, sizeof(IODE_REAL));

    /* div:1  TCELL machine independent */
    len = P_get_len(pack, 1);
    pcell = (TCELL *) P_get_ptr(pack, 1);
    cell = SW_nalloc(len);
    memcpy((char *) cell, (char *) pcell, len);

    /* div:2..nc + 2 */
    for(j = 0, p = 2; j < nc; j++)
	if(cell[j].tc_val !=  NULL) {
	    K_xdrCELL(P_get_ptr(pack, p), cell[j].tc_type, mode);
	    p++;
	}
    SW_nfree(cell);

    /* lines */
    len = P_get_len(pack, p);
    pline = (TLINE *) P_get_ptr(pack, p);
    line = SW_nalloc(len);
    memcpy((char *) line, (char *) pline, len);
    p++;

    for(i = 0; i < nl; i++) {
	switch(line[i].tl_type) {
	case KT_CELL :
	    len = P_get_len(pack, p);
	    pcell = P_get_ptr(pack, p);
	    cell = SW_nalloc(len);
	    memcpy(cell, pcell, len);
	    p++;

	    for(j = 0; j < nc; j++)
		if(cell[j].tc_val != NULL) {
		    K_xdrCELL(P_get_ptr(pack, p), cell[j].tc_type, mode);
		    p++;
		}
	    SW_nfree(cell);
	    break;

	case KT_TITLE :
	    len = P_get_len(pack, p);
	    pcell = P_get_ptr(pack, p);
	    cell =  SW_nalloc(len);
	    memcpy(cell, pcell, len);
	    p++;

	    if(cell->tc_val != NULL) {
		K_xdrCELL(P_get_ptr(pack, p), cell->tc_type, mode);
		p++;
	    }
	    SW_nfree(cell);
	    break;
	}
    }
    SW_nfree(line);
}

/* ODE OBJECTS */

int K_cxdr(ptr, xdr_ptr)
unsigned char *ptr, **xdr_ptr;
{
    return(K_oxdr(ptr, xdr_ptr));
}

int K_exdr(ptr, xdr_ptr)
unsigned char *ptr, **xdr_ptr;
{
    int     len = 0;

    if(xdr_ptr == NULL) {
	/* intel read */
	K_xdrPACK(ptr, 0);
	K_xdrCLEC(P_get_ptr(ptr, 1), 0);
	K_xdrSMPL(P_get_ptr(ptr, 4));
	K_xdrLONG(P_get_ptr(ptr, 8));
	XDR_rev(P_get_ptr(ptr, 9), 20, sizeof(IODE_REAL));
    }
    else {
	/* intel write */
	len = P_len(ptr);
	*xdr_ptr = SW_nalloc(len);
	memcpy(*xdr_ptr, ptr, len);

	K_xdrCLEC(P_get_ptr(*xdr_ptr, 1), 1);
	K_xdrSMPL(P_get_ptr(*xdr_ptr, 4));
	K_xdrLONG(P_get_ptr(*xdr_ptr, 8));
	XDR_rev(P_get_ptr(*xdr_ptr, 9), EQS_NBTESTS, sizeof(float)); /* FLOAT 12-04-98 */

	K_xdrPACK(*xdr_ptr, 1);
    }
}

int K_ixdr(ptr, xdr_ptr)
unsigned char *ptr, **xdr_ptr;
{
    int     len = 0;

    if(xdr_ptr == NULL) {
	/* intel read */
	K_xdrPACK(ptr, 0);
	K_xdrCLEC(P_get_ptr(ptr, 1), 0);
    }
    else {
	/* intel write */
	len = P_len(ptr);
	*xdr_ptr = SW_nalloc(len);
	memcpy(*xdr_ptr, ptr, len);
	len = P_get_len(ptr, 1);
	K_xdrCLEC(P_get_ptr(*xdr_ptr, 1), len);

	K_xdrPACK(*xdr_ptr, len);
    }
}

int K_lxdr(ptr, xdr_ptr)
unsigned char *ptr, **xdr_ptr;
{
    return(K_oxdr(ptr, xdr_ptr));
}

int K_sxdr(ptr, xdr_ptr)
unsigned char *ptr, **xdr_ptr;
{
    int     len;

    if(xdr_ptr == NULL) {
	/* intel read */
	K_xdrPACK(ptr, 0);
	XDR_rev(P_get_ptr(ptr, 0), 3, sizeof(IODE_REAL));
    }
    else {
	/* intel write */
	len = P_len(ptr);
	*xdr_ptr = SW_nalloc(len);
	memcpy(*xdr_ptr, ptr, len);
	len = P_get_len(ptr, 0);
	XDR_rev(P_get_ptr(*xdr_ptr, 0), 3, sizeof(IODE_REAL));

	K_xdrPACK(*xdr_ptr, len);
    }
}

int K_txdr(ptr, xdr_ptr)
unsigned char *ptr, **xdr_ptr;
{
    int     len = 0;

    if(xdr_ptr == NULL) {
	/* intel read */
	K_xdrPACK(ptr, 0);
	K_xdrTBL(ptr, 0);
    }
    else {
	/* intel write */
	len = P_len(ptr);
	*xdr_ptr = SW_nalloc(len);
	memcpy(*xdr_ptr, ptr, len);

	K_xdrTBL(*xdr_ptr, 1);
	K_xdrPACK(*xdr_ptr, 1);
    }
}


K_vxdr(ptr, xdr_ptr)
unsigned char *ptr, **xdr_ptr;
{
    int     len;
    U_sh    *pos;

    if(xdr_ptr == NULL) {
	/* intel read */
	K_xdrPACK(ptr, 0);

	pos = (U_sh *) ptr;
	len = P_get_len(ptr, 0);
	XDR_rev(P_get_ptr(ptr, 0), len/sizeof(IODE_REAL), sizeof(IODE_REAL));
    }
    else {
	/* intel write */
	len = P_len(ptr);
	*xdr_ptr = SW_nalloc(len);
	memcpy(*xdr_ptr, ptr, len);
	len = P_get_len(ptr, 0);
	XDR_rev(P_get_ptr(*xdr_ptr, 0), len/sizeof(IODE_REAL), sizeof(IODE_REAL));

	K_xdrPACK(*xdr_ptr, len);
    }
}

K_oxdr(ptr, xdr_ptr)
unsigned char *ptr, **xdr_ptr;
{
    int     len;
    U_sh    *pos;

    if(xdr_ptr == NULL) {
	/* intel read */
	K_xdrPACK(ptr, 0);
    }
    else {
	/* intel write */
	len = P_len(ptr);
	*xdr_ptr = SW_nalloc(len);
	memcpy(*xdr_ptr, ptr, len);
	K_xdrPACK(*xdr_ptr, len);
    }
    return(0);
}

#endif

int   (*K_xdrobj[])() = {
    K_cxdr,
    K_exdr,
    K_ixdr,
    K_lxdr,
    K_sxdr,
    K_txdr,
    K_vxdr,
    K_oxdr
};






