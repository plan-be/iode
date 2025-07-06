/**
 * @author Jean-Marc PAUL 
 * @author Geert BRYON
 * 
 * Functions for "packing" and "unpacking" IODE objects.
 * 
 * IODE stores objects in a "packed" form both in memory and on disk. 
 *   - to minimize memory usage
 *   - to avoid memory fragmentation (alloc/realloc/free...)
 * 
 * Object are unpacked "just in time" and repacked when they are not immediately needed.

 * A "packed" object is independent from the architecture (32|64 bits).
 * An object in its "struct" form, at the contrary, is different in 64 bits and 32 bits 
 * because of the different pointer lengths (4 bytes vs 8 bytes). 
 * Consequently, translations 64-32 and 32-64 are required both to pack and unpack objects.
 *
 * @note The 32-64 and 64-32 "translation" functions are local and hidden from the exposed API.
 * 
 * Packing functions
 * ----------------- 
 *      int K_cpack(char **pack, char *a1)                  Packs an IODE CMT object 
 *      int K_epack(char **pack, char *a1, char *a2)        Packs an IODE EQ object 
 *      int K_ipack(char **pack, char *a1)                  Packs an IODE IDT object 
 *      int K_lpack(char** pack, char* a1)                  Packs an IODE LST object 
 *      int K_spack(char **pack, char *a1)                  Packs an IODE SCL object 
 *      int K_tpack(char** pack, char* a1)                  Packs an IODE TBL object 
 *      int K_vpack(char **pack, double *a1, int *a2)    Packs an IODE VAR object. 
 *      int K_opack(char** pack, char* a1, int* a2)         Reserved for future new objects
 * 
 * Unpacking functions
 * -------------------- 
 *      TBL* K_tunpack(char *pack)                   Creates a TBL struct from a packed TBL 
 *      EQ*  K_eunpack(char *pack, char *name)       Creates an EQ struct from a packed EQ
 *      IDT* K_iunpack(char *pack)                   Creates an IDT struct from a packed IDT
 * 
 * Allocation functions (VAR & SCL only)
 * -------------------------------------
 *      int KS_alloc_scl()          Allocates space for a new SCL (0.9, 1.0, NaN) in the the "swap area". Returns the "swap" handle.
 *      int KV_alloc_var(int nb)    Allocates space for a new VAR of length nb in the swap area, initialises it to IODE_NAN and returns the "swap" handle.
 * 
 * @note The "modern" terminology for pack and unpack is "serialize" and "deserialize".
 * @see scr4/s_swap.h (http://www.xon.be/scr4/libs1/libs1236.htm) for more details.
 */
#include "api/constants.h"
#include "api/lec/lec.h"
#include "api/objs/pack.h"
#include "api/objs/equations.h"
#include "api/objs/identities.h"
#include "api/objs/scalars.h"
#include "api/objs/tables.h"
#include "api/objs/structs_32.h"


/**
 * Packs an IODE VAR object. 
 * 
 * If a1 is NULL, creates a VAR of length *a2 filled with IODE_NAN.
 * 
 * If a1 is defined, packs a1.
 * 
 * @param [out] pack    (char **)       placeholder for the pointer to the packed object 
 * @param [in]  a1      (double*)    pointer to the first VAR element 
 * @param [in]  a2      (int *)         pointer to the number of elements in the VAR
 * 
 * @return int -1 if the variable is empty, 0 if ok
*/
int K_vpack(char **pack, double *a1, int *a2)
{
    int     i;
    double* ptr;

    if(*a2 == 0) return(-1);
    *pack = P_create();
    if(a1 == NULL) {
        *pack = P_add(*pack, NULL, sizeof(double) * *a2);
        ptr = (double*)P_get_ptr(*pack, 0);
        for(i = 0; i < *a2; i++) ptr[i] = IODE_NAN;
    }
    else *pack = P_add(*pack, (char*)a1, sizeof(double) * *a2);
    return(0);
}

/*
 * Function to pack TBL (in 32 and 64 bits)
 * -------------------------------------------
*/

/**
 *   Function to pack a TBL cell (in 32 and 64 bits)
 *  
 *  @param [in, out]    pack pointer to the pack before adding the cell (can be NULL)
 *  @param [in]         cell 
 *  @return             pointer to the new pack
 */
static char* K_tcell_pack(char *pack, TCELL *cell)
{
    if(cell->tc_val == NULL) return(pack);
    if(cell->tc_type == TABLE_CELL_LEC)
        pack = P_add(pack, cell->tc_val, P_len(cell->tc_val));
    else
        pack = P_add(pack, cell->tc_val, (int)strlen(cell->tc_val) + 1);

    return(pack);
}

/**
 *  Give the offset in bytes of a struct element.
 *  
 *  @param [in] base    position of the first field of the struct (&struct)
 *  @param [in] element position of the element (&(struct.element))
 *  @return int         offset in bytes
 */
static int K_pos_struct(void* base, void* element)
{
    return((int)((char*)element - (char*)base));
}

/**
 *  Convert a TCELL64 struct (64 bits architecture) into a TCELL32 (32 bits arch)
 *  
 *  @param [in, out] tc64 pointer to a TCELL32 (Table cell in 32 bits arch)
 *  @param [in]      tc32 pointer to a TCELL64 (Table cell in 64 bits arch)
 *  @return void
 *  
 *  @details Copy the the first bytes of tc64 up to tc_type, then set tc32->tc_val as 1 or 0 
 *           according to the value of tc64->tc_val.
 */
 
static void K_tcell64_32(TCELL* tc64, TCELL32* tc32)
{
    int pos_tc_type;

    // Binary copy from tl_type to the end of the struct
    pos_tc_type = K_pos_struct(tc32, &tc32->tc_type);
    memcpy((char*)&tc32->tc_type, (char*)&tc64->tc_type, sizeof(TCELL32) - pos_tc_type);
    if(tc64->tc_val)
        tc32->tc_val = 1;
    else
        tc32->tc_val = 0;
}

/**
 *  Convert a TLINE64 into TLINE32 struct.
 *  
 *  @param [in] tl64 TLINE in 64 bits arch
 *  @param [in] tl32 TLINE in 32 bit arch
 *  @return void
 *  
 *  @details See K_tcell64_32()
 */
 
static void K_tline64_32(TLINE* tl64, TLINE32* tl32)
{
    int pos_tl_type;

    // Binary copy from tl_type to the end of the struct
    pos_tl_type = K_pos_struct(tl32, &tl32->tl_type);
    memcpy((char*)&tl32->tl_type, (char*)&tl64->tl_type, sizeof(TLINE32) - pos_tl_type);
}

/**
 *  Convert a TBL64 into TBL32 struct.
 *  
 *  @param [in] tbl64 TBL in 64 bits arch
 *  @param [in] tbl32 TBL in 32 bit arch
 *  @return void
 *  
 *  @details See K_tcell64_32()
 */

static void K_tbl64_32(TBL* tbl64, TBL32* tbl32)
{
    int     pos_t_div, pos_t_zmin;

    // Copy jusque t_div non compris
    pos_t_div = K_pos_struct(tbl32, &tbl32->t_div);
    memcpy((char*)tbl32, (char*)tbl64, pos_t_div);

    // Binary copy from t_zmin to the end of the struct
    pos_t_zmin = K_pos_struct(tbl32, &tbl32->t_zmin);
    memcpy((char*)&tbl32->t_zmin, (char*)&tbl64->t_zmin, sizeof(TBL32) - pos_t_zmin);

    // Copie de t_div (TLINE) sans les pointeurs
    K_tline64_32((TLINE*)&tbl64->t_div, (TLINE32*)&tbl32->t_div);
}

/**
 *  Pack a 32 bits TBL struct.
 *  
 *  @param [in] pack pointer to the packed table placeholder
 *  @param [in] a1   pointer to the 32 bits TBL
 *  @return -1 if error (lack of memory), 0 if ok
 *  
 *  @details Sub function of K_tpack(). 
 */

static int K_tpack32(char **pack, char *a1)
{
    TBL* tbl = (TBL*)a1;
    //TLINE* line;
    TCELL* cell;
    int     i, j;

    *pack = P_create();
    if(a1 == NULL) return(-1);

    /* tbl */
    *pack = P_add(*pack, (char*)tbl, sizeof(TBL));

    /* div */
    /* 1. : [nc x TCELL] */
    cell = (TCELL*)tbl->t_div.tl_val;
    *pack = P_add(*pack, (char*)cell, sizeof(TCELL) * (int)T_NC(tbl));

    /* 2. : nc x [TCELL->tc_val] */
    for(j = 0; j < T_NC(tbl); j++)
        *pack = K_tcell_pack(*pack, cell + j);

    /* lines */
    /* 1. : [nl x TLINE] */
    *pack = P_add(*pack, (char*)tbl->t_line, sizeof(TLINE) * (int)T_NL(tbl));
    for(i = 0; i < T_NL(tbl); i++) {
        switch(tbl->t_line[i].tl_type) {
            case TABLE_LINE_CELL:
                cell = (TCELL*)tbl->t_line[i].tl_val;
                /* [nc x TCELL] */
                *pack = P_add(*pack, (char*)cell, sizeof(TCELL) * (int)T_NC(tbl));
                /* 2. : nc x [TCELL->tc_val] */
                for(j = 0; j < T_NC(tbl); j++)
                    *pack = K_tcell_pack(*pack, cell + j);
                break;
            case TABLE_LINE_TITLE:
                cell = (TCELL*)tbl->t_line[i].tl_val;
                /* [1 x TCELL] */
                *pack = P_add(*pack, (char*)cell, sizeof(TCELL));
                /* 1 x [TCELL->tc_val] */
                *pack = K_tcell_pack(*pack, cell);
                break;
        }
    }

    return(0);
}

/**
 *  Packs a 64 bits TBL struct by first converting the TBL64, TLINE64 and TCELL64 into 32 bits struct.
 *  
 *  @param [in] pack pointer to the packed table placeholder
 *  @param [in] a1   pointer to the 64 bits TBL
 *  @return     -1 if error (lack of memory), 0 if ok
 *  
 *  @details Sub function of K_tpack(). 
 */

static int K_tpack64(char **pack, char *a1)
{
    TBL* tbl = (TBL*)a1;
    TBL32   tbl32;
    TLINE* line;
    TLINE32* line32;
    TCELL* cell;
    TCELL32* cell32;
    int     i, j;

    *pack = P_create();
    if(a1 == NULL) return(-1);

    /* tbl */
    K_tbl64_32(tbl, &tbl32);
    *pack = P_add(*pack, (char*)&tbl32, sizeof(TBL32));

    /* div */
    /* 1. : [nc x TCELL32] */
    cell = (TCELL*)tbl->t_div.tl_val;
    cell32 = (TCELL32*)SW_nalloc(sizeof(TCELL32) * (int)T_NC(tbl));
    for(j = 0; j < T_NC(tbl); j++)
        K_tcell64_32(cell + j, cell32 + j);

    *pack = P_add(*pack, (char*)cell32, sizeof(TCELL32) * (int)T_NC(tbl));

    /* 2. : [cell] [cell] (nc x [TCELL->tc_val]) */
    for(j = 0; j < T_NC(tbl); j++)
        *pack = K_tcell_pack(*pack, cell + j);

    /* pack lines */
    /* 1. : [nl x TLINE] */
    line = (TLINE*)tbl->t_line;
    line32 = (TLINE32*)SW_nalloc(sizeof(TLINE32) * (int)T_NL(tbl));
    for(j = 0; j < T_NL(tbl); j++)
        K_tline64_32(line + j, line32 + j);

    *pack = P_add(*pack, (char*)line32, sizeof(TLINE32) * (int)T_NL(tbl));

    /* 2. For each line and each col, pack cell
      [cell] [cell] ... */
    for(i = 0; i < T_NL(tbl); i++) {
        switch(tbl->t_line[i].tl_type) {
            case TABLE_LINE_CELL:
                /* [TLINE32 * NC] */
                cell = (TCELL*)tbl->t_line[i].tl_val;
                for(j = 0; j < T_NC(tbl); j++) 
                    K_tcell64_32(cell + j, cell32 + j);
                *pack = P_add(*pack, (char*)cell32, sizeof(TCELL32) * (int)T_NC(tbl));

                /* [TCELL32] [TCELL32] ... */
                for(j = 0; j < T_NC(tbl); j++)
                    *pack = K_tcell_pack(*pack, cell + j);
                break;

            case TABLE_LINE_TITLE:
                cell = (TCELL*)tbl->t_line[i].tl_val;
                /* [1 x TCELL] */
                K_tcell64_32(cell + 0, cell32 + 0);
                *pack = P_add(*pack, (char*)cell32, sizeof(TCELL32) * 1);

                /* 1 x [TCELL->tc_val] */
                *pack = K_tcell_pack(*pack, cell);
                break;
        }
    }

    SW_nfree(cell32);
    SW_nfree(line32);
    return(0);
}

/**
 * Packs a TBL object whether in 32 or 64 bits architecture. 
 *
 * @param [out] pack    (char **)       placeholder for the pointer to the packed TBL
 * @param [in]  a1      (char *)        pointer to the TBL (cast to char *)
 *
 * @return int 0 
*/

int K_tpack(char** pack, char* a1)
{
    if(X64)
        return(K_tpack64(pack, a1));
    else
        return(K_tpack32(pack, a1));
    return(0);
}


/**
 * Compiles an IDT and packs the resulting structure. 
 *
 * @param [out] pack    (char **)       placeholder for the pointer to the packed object
 * @param [in]  a1      (char *)        pointer to the LEC form of the IDT
 *
 * @return int 0 if ok, -1 if a1 is NULL or the comp
 */
 
int K_ipack(char **pack, char *a1)
{
    CLEC* clec;

    *pack = 0;
    if(a1 == NULL) return(-1);
    clec = L_cc(a1);
    if(clec == 0)  return(-1);
    *pack = P_create();
    *pack = P_add(*pack, a1, (int)strlen(a1) + 1);
    *pack = P_add(*pack, (char*)clec, clec->tot_lg);
    SW_nfree(clec);
    return(0);
}

/**
 * Packs an EQ object.
 *
 * @param [out] pack    (char **)       placeholder for the pointer to the packed object
 * @param [in]  a1      (char *)        pointer to the EQ (cast to EQ *)
 * @param [in]  endo    (char *)        pointer to the endogenous name
 *
 * @return int 
 *      0 if ok, 
 *      -1 if EQ does not contain a LEC formula, 
 *      -1 if the LEC formula cannot be compiled
 *  
 * @details The Endo (a2) is needed to "solve" the LEC equation. L_solve() try to analytically
 *          solve the equation to obtain a formula in the form :
 *              ENDO := f(x)
 *          If it is not possible, a Newton-Raphson like algorithm will be used to solve the equation
 *          for each iteration of the simulation process.
 */

int   K_epack(char **pack, char *a1, char *endo)
{
    EQ* eq;
    CLEC* clec;

    eq = (EQ*)a1;

    *pack = P_create();
    if(eq->lec == NULL) return(-1);
    clec = L_solve(eq->lec, endo);
    if(clec == 0)  return(-1);

    *pack = P_add(*pack, eq->lec, (int)strlen(eq->lec) + 1);             /* lec */
    *pack = P_add(*pack, (char*)clec, clec->tot_lg);                /* clec */
    *pack = P_add(*pack, &(eq->solved), 1);                         /* solved */
    *pack = P_add(*pack, &(eq->method), 1);                         /* method */
    *pack = P_add(*pack, (char*)&(eq->smpl), sizeof(SAMPLE));       /* sample */

    if(eq->cmt == NULL) *pack = P_add(*pack, NULL, 1);
    else *pack = P_add(*pack, (char*)eq->cmt, (int)strlen(eq->cmt) + 1);    /* cmt */

    if(eq->blk == NULL) *pack = P_add(*pack, NULL, 1);
    else *pack = P_add(*pack, (char*)eq->blk, (int)strlen(eq->blk) + 1);    /* blk */

    if(eq->instr == NULL) *pack = P_add(*pack, NULL, 1);
    else *pack = P_add(*pack, (char*)eq->instr, (int)strlen(eq->instr) + 1);/* instr */

    *pack = P_add(*pack, (char*)&(eq->date), sizeof(long));                 /* date */
    *pack = P_add(*pack, (char*)&(eq->tests), EQS_NBTESTS * sizeof(float)); /* tests*/ /* FLOAT 12-04-98 */

    SW_nfree(clec);
    return(0);
}


/**
 * Packs a SCL object. 
 *
 * @param [out] pack    (char **)       placeholder for the pointer to the packed object
 * @param [in]  a1      (char *)        pointer to the SCL. If NULL, a1 is replaced by a default SCL (.9, 1.0, NaN).
 *
 * @return int 0 
 *
 * @note No pointer in SCL, thus the pack == binary copy of the SCL
 */

int K_spack(char **pack, char *a1)
{
    static    SCL scl = { 0.9, 1.0, IODE_NAN };

    *pack = P_create();
    if(a1 == NULL)
        *pack = P_add(*pack, &scl, sizeof(SCL));
    else
        *pack = P_add(*pack, a1, sizeof(SCL));
    return(0);
}


/**
 * Packs a CMT object. 
 *
 * @param [out] pack    (char **)       placeholder for the pointer to the packed object
 * @param [in]  a1      (char *)        NULL or pointer to the CMT. If NULL, the CMT is replaced by "".
 *
 * @return int 0 
 *
 * @note A CMT is a simple char *, thus the pack == binary copy of the CMT
 */

int K_cpack(char **pack, char *a1)
{
    *pack = P_create();
    if(a1 == NULL) *pack = P_add(*pack, "", 1);
    else *pack = P_add(*pack, a1, (int)strlen(a1) + 1);
    return(0);
}


/**
 * Packs a LST object. 
 *
 * @param [out] pack    (char **)       placeholder for the pointer to the packed object
 * @param [in]  a1      (char *)        NULL or pointer to the LST. If NULL, the LST is replaced by "".
 *
 * @return int 0 
 *
 * @note A LST is a simple char *, thus the pack == binary copy of the struct.
 */

int K_lpack(char** pack, char* a1)
{
    *pack = P_create();
    if(a1 == NULL) *pack = P_add(*pack, "", 1);
    else *pack = P_add(*pack, a1, (int)strlen(a1) + 1);
    return(0);
}

/**
 *  Reserved for future new objects.
 *  
 *  @param [out] pack  char **      placeholder for the pointer to the packed object
 *  @param [in]  a1    char*        NULL or pointer the the object.
 *  @param [in]  a2    char*        extra info 
 *  @return 0 for compatibily with other functions 
 */
 
int K_opack(char** pack, char* a1, int* a2)
{
    *pack = P_create();
    if(a1 == NULL || *a2 == 0) *pack = P_add(*pack, "", 1);
    else *pack = P_add(*pack, a1, *a2);
    return(0);
}

/**
 * Table of pointers to the 7 K_*pack() functions.
*/
int (*K_pack[])() = {
    K_cpack,
    K_epack,
    K_ipack,
    K_lpack,
    K_spack,
    K_tpack,
    K_vpack,
    K_opack
};

/* 
 * UNPACK functions
 * ----------------
 * From a packed object (architecture insensitive, see above), creates an IODE object (VAR, EQ, TBL, ...) 
 * in the current archicture (32|64).
 */

// see T_create() for the initialization of the special 'div' line
static void K_tcell_div_sanitize(TCELL* cell, int j)
{
    // if cell type not set, assume it is a LEC cell
    if(cell->tc_type == 0){
        cell->tc_type = TABLE_CELL_LEC;
        cell->tc_val = NULL;
    }
    if(j == 0 && cell->tc_val == NULL)
        K_ipack(&(cell->tc_val), "1");
}

static void K_tcell_sanitize(TCELL* cell, int j)
{
    // if cell type not set, assume it is a LEC cell
    if(cell->tc_type == 0)
        cell->tc_type = TABLE_CELL_LEC;
    // if it is a string cell and its content is NULL, initialize it to an empty string
    if(cell->tc_type == TABLE_CELL_STRING && cell->tc_val == NULL)
        T_set_string_cell(cell, "");
}

/**
 * Unpacks a packed TBL into a TBL structure in the current architecture (32|64 bits).
 * Converts simultaneously TLINE and TCELL to 32 bits structs.
 * 
 * @param [in]      pack char * packed TBL
 *
 * @return TBL *    Allocated TBL in 32 or 64 bits
*/
static TBL* K_tunpack32(char *pack)
{
    TBL* ptbl, * tbl;
    //TLINE* line;
    TCELL* pcell, * cell;
    int     i, j, p, len = 0;

    /* tbl */
    len = P_get_len(pack, 0);
    ptbl = (TBL*)P_get_ptr(pack, 0);
    tbl = (TBL*)SW_nalloc(len);
    memcpy((char*)tbl, (char*)ptbl, len);

    /* div */
    len = P_get_len(pack, 1);
    ptbl->t_div.tl_val = P_get_ptr(pack, 1);
    pcell = (TCELL*)ptbl->t_div.tl_val;

    tbl->t_div.tl_val = SW_nalloc(len);
    cell = (TCELL*)tbl->t_div.tl_val;
    memcpy((char*)cell, (char*)pcell, len);

    for(j = 0, p = 2; j < T_NC(tbl); j++) {
        if(pcell[j].tc_val != NULL) {
            len = P_get_len(pack, p);
            pcell[j].tc_val = P_get_ptr(pack, p);
            cell[j].tc_val = SW_nalloc(len);
            memcpy(cell[j].tc_val, pcell[j].tc_val, len);
            p++;
        }
        K_tcell_div_sanitize(cell + j, j);
    }

    /* lines */
    len = P_get_len(pack, p);
    T_L(ptbl) = P_get_ptr(pack, p);
    p++;

    /* alloc must be a multiple of KT_CHUNCK */
    T_L(tbl) = (void*)SW_nalloc(sizeof(TLINE) * ((int)T_NL(tbl) / KT_CHUNCK + 1) * KT_CHUNCK);
    memcpy(T_L(tbl), T_L(ptbl), len);

    for(i = 0; i < T_NL(tbl); i++) {
        switch(ptbl->t_line[i].tl_type) {
            case TABLE_LINE_CELL:
                len = P_get_len(pack, p);
                pcell = (TCELL*) P_get_ptr(pack, p);
                ptbl->t_line[i].tl_val = (char *)pcell;
                cell = (TCELL*) SW_nalloc(len);
                tbl->t_line[i].tl_val = (char *)cell;
                memcpy(cell, pcell, len);
                p++;

                for(j = 0; j < T_NC(tbl); j++){
                    if(cell[j].tc_val != NULL) {
                        len = P_get_len(pack, p);
                        pcell[j].tc_val = P_get_ptr(pack, p);
                        cell[j].tc_val = SW_nalloc(len);
                        memcpy(cell[j].tc_val, pcell[j].tc_val, len);
                        p++;
                    }
                    K_tcell_sanitize(cell + j, j);
                }
                break;
            case TABLE_LINE_TITLE:
                len = P_get_len(pack, p);
                /*
                pcell = (TCELL *) ptbl->t_line[i].tl_val = P_get_ptr(pack, p);
                tbl->t_line[i].tl_val = SW_nalloc(len);
                cell = (TCELL*)tbl->t_line[i].tl_val;
                */
                pcell = (TCELL *) P_get_ptr(pack, p);
                ptbl->t_line[i].tl_val = (char *)pcell;
                cell = (TCELL *)SW_nalloc(len);
                tbl->t_line[i].tl_val = (char *)cell;
                memcpy(cell, pcell, len);
                p++;

                if(cell->tc_val != NULL) {
                    len = P_get_len(pack, p);
                    pcell->tc_val = P_get_ptr(pack, p);
                    cell->tc_val = SW_nalloc(len);
                    memcpy(cell->tc_val, pcell->tc_val, len);
                    p++;
                }
                K_tcell_sanitize(cell, 0);
                break;
        }
    }
    return(tbl);
}

/**
 *  Converts a 32 bits table **cell** (TCELL32) to a 64 bits TCELL. 
 *  
 *  @param [in]         tc32 TCELL32 * pointer to the TCELL32 
 *  @param [in, out]    tc64 TCELL *   pointer to the resulting TCELL (64 bits)
 *  @return void
 *  
 *  @note Only used in 64 bit architecture.
 *  
 */
static void K_tcell32_64(TCELL32* tc32, TCELL* tc64)
{
    int pos_tc_type;

    // Binary copy from tc_type to the end of the struct
    pos_tc_type = K_pos_struct(tc32, &tc32->tc_type);
    memcpy((char*)&tc64->tc_type, (char*)&tc32->tc_type, sizeof(TCELL32) - pos_tc_type);
    if(tc32->tc_val) tc64->tc_val = (char*)1;
}

/**
 *  Converts a 32 bits table **line** (TLINE32) to a 64 bits TLINE. 
 *  
 *  @param [in]         tc32 TLINE32 * pointer to the TLINE32 
 *  @param [in, out]    tc64 TLINE *   pointer to the resulting TLINE (64 bits)
 *  @return void
 *  
 *  @note Only used in 64 bit architecture.
 *  
 */
static void K_tline32_64(TLINE32* tl32, TLINE* tl64)
{
    int pos_tl_type;

    // Binary copy from tl_type to the end of the struct
    pos_tl_type = K_pos_struct(tl32, &tl32->tl_type);
    memcpy((char*)&tl64->tl_type, (char*)&tl32->tl_type, sizeof(TLINE32) - pos_tl_type);
}

/**
 *  Converts a 32 bits table (TBL32) to a 64 bits TBL. 
 *  
 *  @param [in]         tc32 TBL32 * pointer to the TBL32 
 *  @param [in, out]    tc64 TBL *   pointer to the resulting TBL (64 bits)
 *  @return void
 *  
 *  @note Only used in 64 bit architecture.
 *  
 */
static void K_tbl32_64(TBL32* tbl32, TBL* tbl64)
{
    int     pos_t_div, pos_t_zmin;

    // Copy jusque t_div non compris
    pos_t_div = K_pos_struct(tbl32, &tbl32->t_div);
    memcpy((char*)tbl64, (char*)tbl32, pos_t_div);

    // Binary copy from t_zmin to the end of the struct
    pos_t_zmin = K_pos_struct(tbl32, &tbl32->t_zmin);
    memcpy((char*)&tbl64->t_zmin, (char*)&tbl32->t_zmin, sizeof(TBL32) - pos_t_zmin);

    // Copie de t_div (TLINE) sans les pointeurs
    K_tline32_64((TLINE32*)&tbl32->t_div, (TLINE*)&tbl64->t_div);
}

/**
 * Unpacks a packed TBL into a TBL structure in the 64 bits architecture.
 * @note Only used in 64 bit architecture.
 * 
 * @param [in]      pack char * packed TBL
 *
 * @return TBL *    allocated TBL (64 bits)
*/

static TBL* K_tunpack64(char *pack)
{
    // 64 bits structs
    TBL* tbl;
    TCELL* cell;
    // 32 bits struct (iode std)
    TLINE32* line32;
    TCELL32* cell32;
    int     i, j, p;

    /* tbl */
    tbl = (TBL*)SW_nalloc(sizeof(TBL));
    K_tbl32_64((TBL32*)P_get_ptr(pack, 0), tbl); // Inclut la transposition de t_div (sans t_div.tl_val)

    /* div (TLINE) */
    cell32 = (TCELL32*)P_get_ptr(pack, 1);
    cell = (TCELL*)SW_nalloc(T_NC(tbl) * sizeof(TCELL));
    tbl->t_div.tl_val = (char*)cell;
    for(j = 0, p = 2; j < T_NC(tbl); j++) {
        K_tcell32_64(cell32 + j, cell + j);
        if(cell32[j].tc_val != 0) {
            cell[j].tc_val = P_alloc_get_ptr(pack, p);
            p++;
        }
        K_tcell_div_sanitize(cell + j, j);
    }
    
    /* lines */
    /* alloc must be a multiple of KT_CHUNCK */
    T_L(tbl) = (TLINE*)SW_nalloc(sizeof(TLINE) * ((int)T_NL(tbl) / KT_CHUNCK + 1) * KT_CHUNCK);
    line32 = (TLINE32*)P_get_ptr(pack, p);
    p++;

    for(i = 0; i < T_NL(tbl); i++) {
        K_tline32_64(line32 + i, tbl->t_line + i);
        switch(tbl->t_line[i].tl_type) {
            case TABLE_LINE_CELL:
                cell = (TCELL*)SW_nalloc(T_NC(tbl) * sizeof(TCELL));
                tbl->t_line[i].tl_val = (char*)cell;
                cell32 = (TCELL32*)P_get_ptr(pack, p);
                p++;
                for(j = 0; j < T_NC(tbl); j++) {
                    K_tcell32_64(cell32 + j, cell + j);
                    if(cell32[j].tc_val != 0) {
                        cell[j].tc_val = P_alloc_get_ptr(pack, p);
                        p++;
                    }
                    K_tcell_sanitize(cell + j, j);
                }
                break;

            case TABLE_LINE_TITLE:
                cell = (TCELL*)SW_nalloc(sizeof(TCELL));
                tbl->t_line[i].tl_val = (char*)cell;
                cell32 = (TCELL32*)P_get_ptr(pack, p);
                p++;
                K_tcell32_64(cell32 + 0, cell + 0);
                if(cell32[0].tc_val != 0) {
                    cell[0].tc_val = (char *)P_alloc_get_ptr(pack, p);
                    p++;
                }
                K_tcell_sanitize(cell, 0);
                break;
        }
    }
    return(tbl);
}


/**
 * Unpacks a packed TBL into a TBL structure in the current (32|64 bits) architecture.
 * 
 * @param [in]      pack char * packed TBL
 *
 * @return TBL *    allocated TBL (32|64 bits according to the current architecture)
*/

TBL* K_tunpack(char *pack)
{
    if(X64)
        return(K_tunpack64(pack));
    else
        return(K_tunpack32(pack));
}


/**
 * Unpacks a packed EQ into a EQ structure in the current (32|64 bits) architecture.
 * 
 * @param [in]      pack    char *     packed EQ
 *
 * @return EQ *     allocated EQ (32|64 bits according to the current architecture)
*/

EQ* K_eunpack(char *pack, char *name)
{
    EQ* eq;
    //char* ptr;
    int     len = 0;

    eq = (EQ*)SW_nalloc(sizeof(EQ));

    eq->endo = SCR_stracpy(name);

    len = P_get_len(pack, 0);
    eq->lec = SW_nalloc(len);
    memcpy(eq->lec, P_get_ptr(pack, 0), len);

    len = P_get_len(pack, 1);
    eq->clec = (CLEC*)SW_nalloc(len);
    memcpy(eq->clec, P_get_ptr(pack, 1), len);

    eq->solved = *(char*)(P_get_ptr(pack, 2));

    eq->method = *(char*)(P_get_ptr(pack, 3));
    if(eq->method < 0 || eq->method >= IODE_NB_EQ_METHODS)
        eq->method = EQ_LSQ;    // Default method is LSQ
    
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

    eq->date = *(long*)(P_get_ptr(pack, 8));

    memcpy(&(eq->tests), P_get_ptr(pack, 9), EQS_NBTESTS * sizeof(float)); /* FLOAT 12-04-98 */

    return(eq);
}


/**
 * Unpacks a packed IDT into an allocated IDT structure..
 * 
 * @param [in]  char*     pack   packed IDT
 * @return      IDT *            allocated IDT structure 
*/

IDT* K_iunpack(char* pack)
{
    int len = 0;
    IDT* idt = (IDT*) SW_nalloc(sizeof(IDT));

    len = P_get_len(pack, 0);
    idt->lec = SCR_stracpy(P_get_ptr(pack, 0));
    len = P_get_len(pack, 1);
    idt->clec = (CLEC*) SW_nalloc(len);
    memcpy(idt->clec, P_get_ptr(pack, 1), len);

    return idt;
}

/*-- ALLOC SCALAR AND VAR --*/

/**
 *  Allocates space for a new scalar (0.9, 1.0, NaN) in the the "swap area" and returns the allocated handle.
 *  
 *  @return Handle to the new scalar.
 *
 */

int KS_alloc_scl()
{
    char* pack, * ptr;
    int     pos;

    K_spack(&pack, NULL);
    pos = SW_alloc(P_len(pack));
    ptr = SW_getptr(pos);
    memcpy(ptr, pack, P_len(pack));
    SW_nfree(pack);
    return(pos);
}


/**
 * Allocates space for a VAR of length nb in the swap area, initialises it to IODE_NAN and returns the swap handle.
 * 
 * @param nb  int length of the VAR element.
 *
 * @return int handle to the VAR in the swap space.
*/

int KV_alloc_var(int nb)
{
    char* pack, * ptr;
    int     pos;

    K_vpack(&pack, NULL, &nb);
    pos = SW_alloc(P_len(pack));
    ptr = SW_getptr(pos);
    memcpy(ptr, pack, P_len(pack));
    SW_nfree(pack);
    return(pos);
}

