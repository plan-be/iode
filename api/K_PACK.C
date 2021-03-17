#include "iode.h"
/*
char k_magic[][LMAGIC] =  {
    "ODE-CMT 4.0 ",
    "ODE-EQS 4.02",
    "ODE-IDT 4.02",
    "ODE-LST 4.0 ",
    "ODE-SCL 4.0 ",
    "ODE-TBL 4.02",
    "ODE-VAR 4.0 ",
    "ODE-OBJ 4.0 "
    };
*/
char k_ext[][4] =  {
    "cmt", // 0
    "eqs",
    "idt",
    "lst",
    "scl",
    "tbl",
    "var",
    "ooo", // 7

    "ac",  // 8
    "ae",
    "ai",
    "al",
    "as",
    "at",
    "av",
    "",   // 15

    "rep", // 16
    "a2m",
    "agl",
    "prf",
    "dif",
    "mif",
    "rtf",
    "ps",
    "asc",
    "txt",
    "csv",  // 26  // JMP 2-3-2016  -> TODO: pas très propre, à modifier

    "xxx"
    };

/* ======================== PACK ==============================
From a real (allocated) object (VAR, TBL, ...), creates a packed version.
The packed version is stored in memory and in iode files.
---------------------------------------------------------------- */

int   K_vpack(pack, a1, a2)
char    **pack;
IODE_REAL   *a1;
int    *a2;
{
    int     i;
    IODE_REAL    *ptr;

    if(*a2 == 0 ) return(-1);
    *pack = P_create();
    if(a1 == NULL) {
	*pack = P_add(*pack, NULL, sizeof(IODE_REAL) * *a2);
	ptr = (IODE_REAL *) P_get_ptr(*pack, 0);
	for(i = 0; i < *a2; i++) ptr[i] = L_NAN;
    }
    else *pack = P_add(*pack, (char *) a1, sizeof(IODE_REAL) * *a2);
    return(0);
}

/*
    Allocates a variable of length nb, sets its elements to NA
    and returns the position in the swap
*/
KV_alloc_var(nb)
int nb;
{
    char    *pack, *ptr;
    int     pos;

    K_vpack(&pack,  NULL, &nb);
    pos = SW_alloc(P_len(pack));
    ptr = SW_getptr(pos);
    memcpy(ptr, pack, P_len(pack));
    SW_nfree(pack);
    return(pos);
}

/*
    Tables
*/

char    *T_cell_pack(pack, cell)
char    *pack;
TCELL   *cell;
{
    if(cell->tc_val == NULL) return(pack);
    if(cell->tc_type == KT_LEC)
	pack = P_add(pack, cell->tc_val, P_len(cell->tc_val));
    else
	pack = P_add(pack, cell->tc_val, strlen(cell->tc_val) + 1);

    return(pack);
}

int K_pos_struct(void *base, void *element)
{
    return((char *)element - (char *)base);
}

K_tcell64_32(TCELL *tc64, TCELL32 *tc32)
{
    int pos_tc_type;

    // Copy de tl_type à la fin
    pos_tc_type = K_pos_struct(tc32, &tc32->tc_type);
    memcpy((char *) &tc32->tc_type, (char *) &tc64->tc_type, sizeof(TCELL32) - pos_tc_type);
	if (tc64->tc_val)
		tc32->tc_val = 1;
	else
		tc32->tc_val = 0;
}

K_tline64_32(TLINE *tl64, TLINE32 *tl32)
{
    int pos_tl_type;

    // Copy de tl_type à la fin
    pos_tl_type = K_pos_struct(tl32, &tl32->tl_type);
    memcpy((char *) &tl32->tl_type, (char *) &tl64->tl_type, sizeof(TLINE32) - pos_tl_type);
}

K_tbl64_32(TBL *tbl64, TBL32 *tbl32)
{
    int     pos_t_div, pos_t_zmin;

    // Copy jusque t_div non compris
    pos_t_div  = K_pos_struct(tbl32, &tbl32->t_div);
    memcpy((char *) tbl32, (char *) tbl64, pos_t_div);

    // Copy de t_zmin à la fin
    pos_t_zmin = K_pos_struct(tbl32, &tbl32->t_zmin);
    memcpy((char *) &tbl32->t_zmin, (char *) &tbl64->t_zmin, sizeof(TBL32) - pos_t_zmin);

    // Copie de t_div (TLINE) sans les pointeurs
    K_tline64_32((TLINE *)&tbl64->t_div, (TLINE32 *)&tbl32->t_div);
}

int   K_tpack32(pack, a1)
char    **pack;
char    *a1;
{
    TBL     *tbl = (TBL *)a1;
    TLINE   *line;
    TCELL   *cell;
    int     i, j;

    *pack = P_create();
    if(a1 == NULL) return(-1);

    /* tbl */
    *pack = P_add(*pack, (char *) tbl, sizeof(TBL));

    /* div */
    /* 1. : [nc x TCELL] */
    cell = (TCELL *) tbl->t_div.tl_val;
    *pack= P_add(*pack, (char *) cell, sizeof(TCELL) * (int) T_NC(tbl));

    /* 2. : nc x [TCELL->tc_val] */
    for(j = 0; j < T_NC(tbl); j++)
	*pack = T_cell_pack(*pack, cell + j);

    /* lines */
    /* 1. : [nl x TLINE] */
    *pack= P_add(*pack, (char *) tbl->t_line, sizeof(TLINE) * (int) T_NL(tbl));
    for(i = 0; i < T_NL(tbl); i++) {
	switch(tbl->t_line[i].tl_type) {
	    case KT_CELL :
		cell = (TCELL *) tbl->t_line[i].tl_val;
		/* [nc x TCELL] */
		*pack= P_add(*pack, (char *) cell, sizeof(TCELL) * (int) T_NC(tbl));
		/* 2. : nc x [TCELL->tc_val] */
		for(j = 0; j < T_NC(tbl); j++)
		    *pack = T_cell_pack(*pack, cell + j);
		break;
	    case KT_TITLE :
		cell = (TCELL *) tbl->t_line[i].tl_val;
		/* [1 x TCELL] */
		*pack= P_add(*pack, (char *) cell, sizeof(TCELL));
		/* 1 x [TCELL->tc_val] */
		*pack= T_cell_pack(*pack, cell);
		break;
	}
    }

    return(0);
}

int   K_tpack64(pack, a1)
char    **pack;
char    *a1;
{
    TBL     *tbl = (TBL *)a1;
    TBL32   tbl32;
    TLINE   *line;
    TLINE32 *line32;
    TCELL   *cell;
    TCELL32 *cell32;
    int     i, j;

    *pack = P_create();
    if(a1 == NULL) return(-1);

    /* tbl */
    K_tbl64_32(tbl, &tbl32);
    *pack = P_add(*pack, (char *)&tbl32, sizeof(TBL32));

    /* div */
    /* 1. : [nc x TCELL32] */
    cell   = (TCELL *) tbl->t_div.tl_val;
    cell32 = (TCELL32 *) SW_nalloc(sizeof(TCELL32) * (int) T_NC(tbl));
    for(j = 0; j < T_NC(tbl); j++)
	K_tcell64_32(cell + j, cell32 + j);

    *pack= P_add(*pack, (char *) cell32, sizeof(TCELL32) * (int) T_NC(tbl));

    /* 2. : [cell] [cell] (nc x [TCELL->tc_val]) */
    for(j = 0; j < T_NC(tbl); j++)
	*pack = T_cell_pack(*pack, cell + j);

    /* pack lines */
    /* 1. : [nl x TLINE] */
    line   = (TLINE *) tbl->t_line;
    line32 = (TLINE32 *) SW_nalloc(sizeof(TLINE32) * (int) T_NL(tbl));
    for(j = 0; j < T_NL(tbl); j++)
	K_tline64_32(line + j, line32 + j);

    *pack= P_add(*pack, (char *) line32, sizeof(TLINE32) * (int) T_NL(tbl));

    /* 2. For each line and each col, pack cell
      [cell] [cell] ... */
    for(i = 0; i < T_NL(tbl); i++) {
	switch(tbl->t_line[i].tl_type) {
	    case KT_CELL :
		/* [TLINE32 * NC] */
		cell = (TCELL *) tbl->t_line[i].tl_val;
		for(j = 0; j < T_NC(tbl); j++)
		    K_tcell64_32(cell + j, cell32 + j);
		*pack= P_add(*pack, (char *) cell32, sizeof(TCELL32) * (int) T_NC(tbl));

		/* [TCELL32] [TCELL32] ... */
		for(j = 0; j < T_NC(tbl); j++)
		    *pack = T_cell_pack(*pack, cell + j);
		break;

	    case KT_TITLE :
		cell = (TCELL *) tbl->t_line[i].tl_val;
		/* [1 x TCELL] */
		K_tcell64_32(cell + 0, cell32 + 0);
		*pack= P_add(*pack, (char *) cell32, sizeof(TCELL32) * 1);

		/* 1 x [TCELL->tc_val] */
		*pack= T_cell_pack(*pack, cell);
		break;
	}
    }

    SW_nfree(cell32);
    SW_nfree(line32);
    return(0);
}


int K_tpack(char **pack, char *a1)
{
    if(X64)
	return(K_tpack64(pack, a1));
    else
	return(K_tpack32(pack, a1));
}


int   K_ipack(pack, a1)
char    **pack;
char    *a1;
{
    CLEC    *clec;

    *pack = 0;
    if(a1 == NULL) return(-1);
    clec = L_cc(a1);
    if(clec == 0)  return(-1);
    *pack = P_create();
    *pack = P_add(*pack, a1, strlen(a1) + 1);
    *pack = P_add(*pack, (char *) clec, clec->tot_lg);
    SW_nfree(clec);
    return(0);
}

/*NH*/
/* a1 pointer to eq, a2 name of the exogeneous series */
int   K_epack(pack, a1, a2)
char    **pack;
char    *a1, *a2;
{
    EQ      *eq;
    CLEC    *clec;

    eq = (EQ *) a1;

    *pack = P_create();
    if(eq->lec == NULL) return(-1);
    clec = L_solve(eq->lec, a2);
    if(clec == 0)  return(-1);

    *pack = P_add(*pack, eq->lec, strlen(eq->lec) + 1);             /* lec */
    *pack = P_add(*pack, (char *) clec, clec->tot_lg);              /* clec */
    *pack = P_add(*pack, &(eq->solved), 1);                         /* solved */
    *pack = P_add(*pack, &(eq->method), 1);                         /* method */
    *pack = P_add(*pack, (char *) &(eq->smpl), sizeof(SAMPLE));     /* sample */

    if(eq->cmt == NULL) *pack = P_add(*pack, NULL, 1);
    else *pack = P_add(*pack, (char *) eq->cmt, strlen(eq->cmt) + 1);    /* cmt */

    if(eq->blk == NULL) *pack = P_add(*pack, NULL, 1);
    else *pack = P_add(*pack, (char *) eq->blk, strlen(eq->blk) + 1);    /* blk */

    if(eq->instr == NULL) *pack = P_add(*pack, NULL, 1);
    else *pack = P_add(*pack, (char *) eq->instr, strlen(eq->instr) + 1);/* instr */

    *pack = P_add(*pack, (char *)&(eq->date), sizeof(long));                /* date */
    *pack = P_add(*pack, (char *)&(eq->tests), EQS_NBTESTS * sizeof(float)); /* tests*/ /* FLOAT 12-04-98 */

    SW_nfree(clec);
    return(0);
}

int   K_spack(pack, a1)
char    **pack;
char    *a1;
{
static    SCL scl = {0.9, 1.0, L_NAN};

    *pack = P_create();
    if(a1 == NULL)
	*pack = P_add(*pack, &scl, sizeof(SCL));
    else
	*pack = P_add(*pack, a1, sizeof(SCL));
    return(0);
}

/*
    Allocates a scalar and its elements to NA
    and returns the position in the swap
*/
KS_alloc_scl()
{
    char    *pack, *ptr;
    int     pos;

    K_spack(&pack,  NULL);
    pos = SW_alloc(P_len(pack));
    ptr = SW_getptr(pos);
    memcpy(ptr, pack, P_len(pack));
    SW_nfree(pack);
    return(pos);
}

int   K_cpack(pack, a1)
char    **pack;
char    *a1;
{
    *pack = P_create();
    if (a1 == NULL) *pack = P_add(*pack, "", 1);
    else *pack = P_add(*pack, a1, strlen(a1) + 1);
    return(0);
}

int   K_lpack(pack, a1)
char    **pack;
char    *a1;
{
    *pack = P_create();
    if (a1 == NULL) *pack = P_add(*pack, "", 1);
    else *pack = P_add(*pack, a1, strlen(a1) + 1);
    return(0);
}

int   K_opack(pack, a1, a2)
char    **pack;
char    *a1;
int     *a2;
{
    *pack = P_create();
    if (a1 == NULL || *a2 == 0) *pack = P_add(*pack, "", 1);
    else *pack = P_add(*pack, a1, *a2);
    return(0);
}

int   (*K_pack[])() = {
    K_cpack,
    K_epack,
    K_ipack,
    K_lpack,
    K_spack,
    K_tpack,
    K_vpack,
    K_opack
    };

/* ======================== UNPACK =================================
From a packed object, creates an allocated object (VAR, EQ, TBL, ...).
---------------------------------------------------------------- */

void K_vunpack()  { }
void K_iunpack()  { }


TBL *K_tunpack32(pack)
char    *pack;
{
    TBL     *ptbl, *tbl;
    TLINE   *pline, *line;
    TCELL   *pcell, *cell;
    int     i, j, p, len = 0;

    /* tbl */
    len = P_get_len(pack, 0);
    ptbl = (TBL *) P_get_ptr(pack, 0);
    tbl = (TBL *)SW_nalloc(len);
    memcpy((char *) tbl, (char *) ptbl, len);

    /* div */
    len = P_get_len(pack, 1);
    ptbl->t_div.tl_val = P_get_ptr(pack, 1);
    pcell = (TCELL *) ptbl->t_div.tl_val;

    tbl->t_div.tl_val = SW_nalloc(len);
    cell = (TCELL *) tbl->t_div.tl_val;
    memcpy((char *) cell, (char *) pcell, len);

    for(j = 0, p = 2; j < T_NC(tbl); j++) {
	if(pcell[j].tc_val !=  NULL) {
	    len = P_get_len(pack, p);
	    pcell[j].tc_val = P_get_ptr(pack, p);
	    cell[j].tc_val = SW_nalloc(len);
	    memcpy(cell[j].tc_val , pcell[j].tc_val, len);
	    p++;
	}
    }

    /* lines */
    len = P_get_len(pack, p);
    T_L(ptbl) = P_get_ptr(pack, p); p++;

    /* alloc must be a multiple of KT_CHUNCK */
    T_L(tbl) = (void *)SW_nalloc(sizeof(TLINE) * ((int) T_NL(tbl)/KT_CHUNCK + 1) * KT_CHUNCK);
    memcpy(T_L(tbl), T_L(ptbl), len);

    for(i = 0; i < T_NL(tbl); i++) {
	switch(ptbl->t_line[i].tl_type) {
	case KT_CELL :
	    len = P_get_len(pack, p);
        pcell = ptbl->t_line[i].tl_val = (TCELL *) P_get_ptr(pack, p);
	    cell  = tbl->t_line[i].tl_val  = (TCELL *) SW_nalloc(len);
	    memcpy(cell, pcell, len);
	    p++;

	    for(j = 0; j < T_NC(tbl); j++)
		if(cell[j].tc_val != NULL) {
		    len = P_get_len(pack, p);
		    pcell[j].tc_val = P_get_ptr(pack, p);
		    cell[j].tc_val = SW_nalloc(len);
		    memcpy(cell[j].tc_val, pcell[j].tc_val, len);
		    p++;
		}
	    break;
	case KT_TITLE :
	    len = P_get_len(pack, p);
	    pcell = ptbl->t_line[i].tl_val = (TCELL *) P_get_ptr(pack, p);
	    cell  = tbl->t_line[i].tl_val  = (TCELL *) SW_nalloc(len);
	    memcpy(cell, pcell, len);
	    p++;

	    if(cell->tc_val != NULL) {
		len = P_get_len(pack, p);
		pcell->tc_val = P_get_ptr(pack, p);
		cell->tc_val = SW_nalloc(len);
		memcpy(cell->tc_val, pcell->tc_val, len);
		p++;
	    }
	    break;
	}
    }
    return(tbl);
}


/* Version 32/64 bits : décompacte un tableau code TBL32 bits vers une struct TBL 64
*/
K_tcell32_64(TCELL32 *tc32, TCELL *tc64)
{
    int pos_tc_type;

    // Copy de tl_type à la fin

    pos_tc_type = K_pos_struct(tc32, &tc32->tc_type);
    memcpy((char *) &tc64->tc_type, (char *) &tc32->tc_type, sizeof(TCELL32) - pos_tc_type);
	if (tc32->tc_val) tc64->tc_val = (char *)1;
}

K_tline32_64(TLINE32 *tl32, TLINE *tl64)
{
    int pos_tl_type;

    // Copy de tl_type à la fin

    pos_tl_type = K_pos_struct(tl32, &tl32->tl_type);
    memcpy((char *) &tl64->tl_type, (char *) &tl32->tl_type, sizeof(TLINE32) - pos_tl_type);
}

K_tbl32_64(TBL32 *tbl32, TBL *tbl64)
{
    int     pos_t_div, pos_t_zmin;

    // Copy jusque t_div non compris
    pos_t_div  = K_pos_struct(tbl32, &tbl32->t_div);
    memcpy((char *) tbl64, (char *) tbl32, pos_t_div);

    // Copy de t_zmin à la fin
    pos_t_zmin = K_pos_struct(tbl32, &tbl32->t_zmin);
    memcpy((char *) &tbl64->t_zmin, (char *) &tbl32->t_zmin, sizeof(TBL32) - pos_t_zmin);

    // Copie de t_div (TLINE) sans les pointeurs
    K_tline32_64((TLINE32 *)&tbl32->t_div, (TLINE *)&tbl64->t_div);
}

TBL *K_tunpack64(pack)
char    *pack;
{
    // 64 bits structs
    TBL     *tbl;
    TCELL   *cell;
    // 32 bits struct (iode std)
    TLINE32 *line32;
    TCELL32 *cell32;
    int     i, j, p;

    /* tbl */
    tbl = (TBL *) SW_nalloc(sizeof(TBL));
    K_tbl32_64((TBL32*) P_get_ptr(pack, 0), tbl); // Inclut la transposition de t_div (sans t_div.tl_val)

    /* div (TLINE) */
    cell32 = (TCELL32 *)P_get_ptr(pack, 1);
    cell   = (TCELL *) SW_nalloc(T_NC(tbl) * sizeof(TCELL));
    tbl->t_div.tl_val = (char *) cell;
    for(j = 0, p = 2; j < T_NC(tbl); j++) {
	if(cell32[j].tc_val != 0) {
	    K_tcell32_64(cell32 + j, cell + j);
	    cell[j].tc_val = P_alloc_get_ptr(pack, p);
	    p++;
	}
    }

    /* lines */
    /* alloc must be a multiple of KT_CHUNCK */
    T_L(tbl) = (TLINE *) SW_nalloc(sizeof(TLINE) * ((int) T_NL(tbl) / KT_CHUNCK + 1) * KT_CHUNCK);
    line32 = (TLINE32 *) P_get_ptr(pack, p);
    p++;

    for(i = 0; i < T_NL(tbl); i++) {
	K_tline32_64(line32 + i, tbl->t_line + i);
	switch(tbl->t_line[i].tl_type) {
	    case KT_CELL :
		cell = (TCELL *) SW_nalloc(T_NC(tbl) * sizeof(TCELL));
		tbl->t_line[i].tl_val = (char *) cell;
		cell32 = (TCELL32 *) P_get_ptr(pack, p);
		p++;
		for(j = 0; j < T_NC(tbl); j++) {
		    if(cell32[j].tc_val != 0) {
			K_tcell32_64(cell32 + j, cell + j);
			cell[j].tc_val = P_alloc_get_ptr(pack, p);
			p++;
		    }
		}
		break;

	    case KT_TITLE :
		cell = (TCELL *)SW_nalloc(sizeof(TCELL));
		tbl->t_line[i].tl_val = (char *)cell;
		cell32 = (TCELL32 *) P_get_ptr(pack, p);
		p++;
		if(cell32[0].tc_val != 0) {
		    K_tcell32_64(cell32 + 0, cell + 0);
		    cell[0].tc_val = P_alloc_get_ptr(pack, p);
		    p++;
		}
		break;
	    }
	}
    return(tbl);
}

TBL *K_tunpack(pack)
char    *pack;
{
    if(X64)
	return(K_tunpack64(pack));
    else
	return(K_tunpack32(pack));
}

EQ  *K_eunpack(pack)
char    *pack;
{
    EQ      *eq;
    char    *ptr;
    int     len = 0;

    eq = (EQ *) SW_nalloc(sizeof(EQ));

    len = P_get_len(pack, 0);
    eq->lec = SW_nalloc(len);
    memcpy(eq->lec, P_get_ptr(pack, 0), len);

    len = P_get_len(pack, 1);
    eq->clec = (CLEC *)SW_nalloc(len);
    memcpy(eq->clec, P_get_ptr(pack, 1), len);

    eq->solved = * (char *) (P_get_ptr(pack, 2));
    eq->method = * (char *) (P_get_ptr(pack, 3));
    memcpy(&(eq->smpl), P_get_ptr(pack, 4), sizeof(SAMPLE));

    len = P_get_len(pack, 5);
    eq->cmt = SW_nalloc(len);
    memcpy(eq->cmt, P_get_ptr(pack, 5), len);

    len = P_get_len(pack, 6);
    eq->blk = SW_nalloc(len);
    memcpy(eq->blk, P_get_ptr(pack, 6), len);

    len = P_get_len(pack, 7);
    eq->instr = SW_nalloc(len);
    memcpy(eq->instr, P_get_ptr(pack, 7), len);

    eq->date = * (long *) (P_get_ptr(pack, 8));

    memcpy(&(eq->tests), P_get_ptr(pack, 9), EQS_NBTESTS * sizeof(float)); /* FLOAT 12-04-98 */

    return(eq);
}

void K_sunpack()  { }
void K_cunpack()  { }
void K_lunpack()  { }
void K_onpack()  { }










