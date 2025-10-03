/**
 * @header4iode
 * 
 * Functions to translate big-endian (b-e) data, used by processors like RISC, into little-endian (l-e) format (x86,...) and vice-versa.
 * 
 * Little-endian (l-e) processors order bytes in memory with the least significant byte in the lowest-numbered memory location. 
 * Big-endian (b-e) architectures instead arrange bytes with the most significant byte at the lowest-numbered address.
 *
 * In memory, the data must always be kept in the current processor architecture for obvious reasons.
 * 
 * On disk, IODE always stores the objects in the little-endian format even if the current executable uses big-endian data
 * representation. The data is thus converted when the file is loaded into memory or saved to disk. 
 *
 * These function make use of scr4 s_xdr group of functions. See http://xon.be/scr4/libs1/libs1340.htm.
 *
 * Functions
 * ---------
 *       void K_xdrPINT(unsigned char* a)           Translates un short int from l-e to b-e and vice-versa
 *       void K_xdrPLONG(unsigned char* a)          Translates un long int from l-e to b-e and vice-versa
 *       void K_xdrKDB(KDB* ikdb, KDB** okdb)       Translates a KDB t from l-e to b-e and vice-versa
 *
 * Vars
 * ----
 *      int (*K_xdrobj[])()                         Table of function pointers, one function for each object type, for translating
 *                                                  big-endian to little-endian and vice-versa
 */
#include "api/constants.h"
#include "api/time/period.h"
#include "api/time/sample.h"
#include "api/lec/lec.h"
#include "api/objs/equations.h"
#include "api/objs/tables.h"
#include "api/objs/pack.h"
#include "api/objs/xdr.h"


#ifdef INTEL
//  Little-endian Architecture
//
// This is the default architecture and the architecture used to store the data on disk.
// Therefore, most of the functions don't do anything (like K_xdrPINT) or are the same for different objects (like K_cxdr).
// Nevertheless, these fns need to be defined because their pointers are stored in a global table K_xdrobj[] share in both architectures.

/**
 *  Function created for compatibility with the big-endian version. 
 *  Translates a packed object from one architecture to another in an allocated pack.
 *  If xdr_ptr is null, nothing is done. 
 *  For compatibility with big-endian architecture (where the result must be allocated),
 *  this function allocates a new buffer and makes a simple copy into it.
 *  
 *  @param [in]     ptr     unsigned char*       pointer to the input packed object
 *  @param [out]    xdr_ptr unsigned char**      NULL or pointer to a pointer where the pointer to the allocated copy is to be stored
 *  @return                 int                  -1 if xdr_ptr is NULL, 
 *                                               0 otherwise 
 *  
 */
static int K_oxdr(unsigned char* ptr, unsigned char** xdr_ptr)
{
    int     len;

    if(xdr_ptr == NULL) return(-1);

    len = P_len(ptr);
    *xdr_ptr = (unsigned char*) SW_nalloc(len);
    memcpy(*xdr_ptr, ptr, len);
    return(0);
}

//// non-static functions ////

/**
 *  Translates a **short int** in the current processor representation into a little-endian **short int**. 
 *  (Does nothing in the x86 architecture).
 *  
 *  @param [in, out]    a   unsigned char*  pointer to a buffer containing a short int
 *  
 */
void K_xdrPINT(unsigned char* a)
{
}


/**
 *  Translates a **long int** in the current processor representation into a little-endian **long int**. 
 *  (Does nothing in the x86 architecture).
 *  
 *  @param [in, out]    a   unsigned char*  pointer to a buffer containing a long int
 *  
 */
void K_xdrPLONG(unsigned char* a)
{
}


/**
 *  Translates a little-endian KDB struct into a little-endian KDB.
 *  In the little-endian (x86) architecture, returns an **allocated** copy of the original KDB (but no translation is performed)
 *  for compatibility between architectures.
 *  
 *  @param [in]     ikdb    KDB*    input KDB struct
 *  @param [out]    okdb    KDB**   allocated copy of ikdb
 *
 */
void K_xdrKDB(KDB* ikdb, KDB** okdb)
{
    if(okdb == NULL) return;

    *okdb = (KDB *)SW_nalloc(sizeof(KDB));
    memcpy(*okdb, ikdb, sizeof(KDB));
}


/**
 *  Table of function pointers, one function for each object type. 
 *  Each function converts an IODE packed object from little-endian architecture to little-endian architecture.
 *  In the case of little-endian architecture, the same function is used for all types.
 *    
 */
 
int (*K_xdrobj[])(unsigned char* ptr, unsigned char** xdr_ptr) = 
{
    K_oxdr,
    K_oxdr,
    K_oxdr,
    K_oxdr,
    K_oxdr,
    K_oxdr,
    K_oxdr,
    K_oxdr
};


#else 

// Architecture big-endian (RISC, SPARC...)
// ----------------------------------------
// Not tested since the 90s and the SUN Sparc workstations. -:)

#define K_xdrSHORT(a)   XDR_rev(a, 1, sizeof(short))
#define K_xdrLONG(a)    XDR_rev(a, 1, sizeof(long))
#define K_xdrREAL(a)    XDR_rev(a, 1, sizeof(double))

/**
 *  Translates a Period struct from little-endian to big-endian or the other way round.
 *  
 *  @param [in, out]    a   unsigned char*  pointer to a Period. 
 *                                          in: little-endian, out:big-endian
 */
static void K_xdrPeriod(unsigned char* a)
{
    XDR_rev(a, 2, sizeof(long));    /* Period LONG */
}


/**
 *  Translates a Sample struct from little-endian to big-endian or the other way round.
 *  
 *  @param [in, out]    a   unsigned char*  pointer to a Period. 
 *                                          in: little-endian, out:big-endian
 */
static void K_xdrSMPL(unsigned char* a)
{
    K_xdrPeriod(a);
    a += sizeof(Period);
    K_xdrPeriod(a);
    a += sizeof(Period);
    K_xdrSHORT(a);
}


/**
 *  Switches the architecture of a packed object (little-endian to big-endian or the other way round). 
 *  The parameter mode indicates the arch of ptr.
 *  
 *  @param [in, out]    ptr     unsigned char*  pointer to a IODE packed object 
 *                                              in: little-endian, out:big-endian
 *  @param [in]         mode    int             0 : the input packed object is little-endian (when reading a file)
 *                                              1 : the input packed object is big-endian (when in memory)
 */

static void K_xdrPACK(unsigned char* ptr, int mode)
{
    OSIZE   *pos, nb;
    int     i;

    pos = (OSIZE *) ptr;
    nb = pos[1];

    if(mode == 0) {
        /* ptr in little-endian arch => nb must be inverted for use in current big-endian program */
        K_xdrPINT(&nb);
    }
    /* pack = 2 + nb elements + data */
    for(i = 0; i < nb + 2; i++) K_xdrPLONG(pos + i);
}

//// CLEC transformation ////

// declare static function 'K_xdrCLEC_sub()'
// to avoid error: static declaration of 'K_xdrCLEC_sub' follows non-static declaration
static void K_xdrCLEC_sub(char* expr, int lg, int mode);

/**
 *  Translates a CVAR struct from little-endian to big-endian or the other way round.
 *  
 *  @param [in] a   unsigned char*  pointer to a CVAR
 */
static void K_xdrCVAR(unsigned char* a)
{
    CVAR    *cv = a;

    XDR_rev(a, 4, sizeof(short));
    K_xdrPeriod(&(cv->per));
}

/**
 *  Translates a TFN function from little-endian to big-endian or the other way round.
 *  
 *  @param [in, out]    expr    char*    sub CLEC expression
 *  @param [in]         len     short    nb of atomic elements in expr
 *  @param [in]         mode    int      0 if expr is in little-endian format, 1 otherwise
 */
static void K_xdrTFN(unsigned char* expr, short len, int mode)
{
    int lg = len;

    K_xdrCLEC_sub(expr, lg, mode);
}


/**
 *  Translates a MTFN function from little-endian to big-endian or the other way round.
 *  
 *  @param [in, out]    expr    char*    sub CLEC expression
 *  @param [in]         mode    int      0 if expr is in little-endian format, 1 otherwise
 *  @param [in]         nvargs  int      number of args of the MTFN function
 *  
 */

static void K_xdrMTFN(unsigned char* expr, int mode, int nvargs)
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

/**
 *  Translates a sub-CLEC expression from little-endian to big-endian or the other way round.
 *  
 *  @param [in, out]    expr    char*    sub lec expressions
 *  @param [in]         lg      int      nb of atomic elements in expr
 *  @param [in]         mode    int      0: if expr is in little-endian format
 */

static void K_xdrCLEC_sub(char* expr, int lg, int mode)
{
    int     j, keyw, nvargs;
    short   len, s;

    for(j = 0 ; j < lg ;) {
        keyw = expr[j++];
        if(keyw == L_VAR || keyw == L_VART) {
            K_xdrCVAR(expr + j);
            j += sizeof(CVAR);
        }
        else switch(keyw) {
                case L_PLUS  :
                case L_MINUS :
                case L_TIMES :
                    break;
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
                case L_Period :
                    K_xdrPeriod(expr + j);
                    j += sizeof(Period);
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
}

/**
 *  Translates a CLEC struct from little-endian to big-endian or the other way round.
 *  
 *  @param [in, out]    expr    CLEC*   Compiled LEC expression
 *  @param [in]         mode    int     0: if CLEC is in little-endian format
 */

static void K_xdrCLEC(CLEC* expr, int mode)
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
    K_xdrCLEC_sub((char *) expr + pos, tot_lg - pos, mode);
}


//// Table transformation ////

/**
 *  Translates a table CELL from little-endian to big-endian or the other way round.
 *  
 *  @param [in, out]    ptr     unsigned char*      pointer to the CELL contents (LEC or text)
 *  @param [in]         type    char                type of the CELL: 0 for LEC, 1 for text
 *  @param [in]         mode    int                 0 if ptr is in little-endian format, 1 otherwise
 *  
 */
static void K_xdrCELL(unsigned char* ptr, char type, int mode)
{
    if(type == TABLE_CELL_LEC) 
    {
        if(mode == 0) 
        {
            K_xdrPACK(ptr, mode);
            K_xdrCLEC(P_get_ptr(ptr, 1), mode);
        }
        else 
        {
            K_xdrCLEC(P_get_ptr(ptr, 1), mode);
            K_xdrPACK(ptr, mode);
        }
    }
}

/**
 *  Translates a table CELL from little-endian to big-endian or the other way round.
 *  
 *  @param [in, out]    ptr     unsigned char*      pointer to the CELL contents (LEC or text)
 *  @param [in]         type    char                type of the CELL: 0 for LEC, 1 for text
 *  @param [in]         mode    int                 0 if ptr is in little-endian format, 1 otherwise
 *  
 */

static void K_xdrTable(unsigned char* pack, int mode)
{
    int             i, j, p, len;
    short           nc, nl;
    unsigned char   *pdiv;
    Table             *tbl;
    TableCell           *cell, *pcell;
    TableLine           *line, *pline;

    /* Table:0 */
    tbl = P_get_ptr(pack, 0);
    nc = T_NC(tbl);
    nl = T_NL(tbl);
    if(mode == 0) { /* intel read */
        K_xdrSHORT(&nc);
        K_xdrSHORT(&nl);
    }
    XDR_rev(tbl, 4, sizeof(short));
    XDR_rev(&(tbl->z_min), 4, sizeof(double));

    /* div:1  TableCell machine independent */
    len = P_get_len(pack, 1);
    pcell = (TableCell *) P_get_ptr(pack, 1);
    cell = SW_nalloc(len);
    memcpy((char *) cell, (char *) pcell, len);

    /* div:2..nc + 2 */
    for(j = 0, p = 2; j < nc; j++)
        if(!cell[j].is_null()) 
        {
            K_xdrCELL(P_get_ptr(pack, p), cell[j].type, mode);
            p++;
        }
    SW_nfree(cell);

    /* lines */
    len = P_get_len(pack, p);
    pline = (TableLine *) P_get_ptr(pack, p);
    line = SW_nalloc(len);
    memcpy((char *) line, (char *) pline, len);
    p++;

    for(i = 0; i < nl; i++) 
    {
        switch(line[i].type) 
        {
            case TABLE_LINE_CELL :
                len = P_get_len(pack, p);
                pcell = P_get_ptr(pack, p);
                cell = SW_nalloc(len);
                memcpy(cell, pcell, len);
                p++;

                for(j = 0; j < nc; j++)
                    if(!cell[j].is_null()) 
                    {
                        K_xdrCELL(P_get_ptr(pack, p), cell[j].type, mode);
                        p++;
                    }
                SW_nfree(cell);
                break;

            case TABLE_LINE_TITLE :
                len = P_get_len(pack, p);
                pcell = P_get_ptr(pack, p);
                cell =  SW_nalloc(len);
                memcpy(cell, pcell, len);
                p++;

                if(!cell[j].is_null()) 
                {
                    K_xdrCELL(P_get_ptr(pack, p), cell->type, mode);
                    p++;
                }
                SW_nfree(cell);
                break;
        }
    }
    SW_nfree(line);
}

/**
 *  Translates a packed object between little- and big-endian architectures.
 *  
 *  If xdr_ptr is null, the input is little-endian and the output is big-endian. 
 *  This is the case when we read a file (always little-endian). The transformation is done in-place, without allocation.
 *  
 *  If xdr_ptr is not null, the input is big-endian and the output is little-endian (used when saving the data into a file). 
 *  A new pack is allocated and must be freed after writing to the file.
 *  
 *  @param [in]     ptr         unsigned char*      pointer to a pack object
 *  @param [out]    xdr_ptr     unsigned char**     if not null, pointer to the resulting allocated copy of the packed object
 *  @return                     int                 0 (for compatibility reason)
 *  
 *  @details 
 */

static int K_oxdr(unsigned char* ptr, unsigned char** xdr_ptr)
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


/**
 *  Translates a little-endian packed CMT into a big-endian packed CMT or the opposite. See K_oxdr() for more information.
 *  
 *  @param [in]     ptr     unsigned char*      pointer to a packed object
 *  @param [out]    xdr_ptr unsigned char**     if null, the input is l-e and the replacement is done in-place
 *                                              if not null, the input is b-e and the resulting allocated copy of the 
 *                                              packed object is stored in *xdr_ptr
 *  @return                 int                 0 always (for compatibility with other functions of the same type)
 *
 */

static int K_cxdr(unsigned char* ptr, unsigned char** xdr_ptr)
{
    return(K_oxdr(ptr, xdr_ptr));
}


/**
 *  Translates a little-endian packed EQ into a big-endian packed EQ or the opposite.
 *  Same behaviour as K_cxdr().
 */

static int K_exdr(unsigned char* ptr, unsigned char** xdr_ptr)
{
    int     len = 0;

    if(xdr_ptr == NULL) {
        /* intel read */
        K_xdrPACK(ptr, 0);
        K_xdrCLEC(P_get_ptr(ptr, 1), 0);
        K_xdrSMPL(P_get_ptr(ptr, 4));
        K_xdrLONG(P_get_ptr(ptr, 8));
        XDR_rev(P_get_ptr(ptr, 9), 20, sizeof(double));
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


/**
 *  Translates a little-endian packed IDT into a big-endian packed IDT or the opposite.
 *  Same behaviour as K_cxdr().
 */
 
static int K_ixdr(unsigned char* ptr, unsigned char** xdr_ptr)
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


/**
 *  Translates a little-endian packed LST into a big-endian packed LST or the opposite.
 *  Same behaviour as K_cxdr().
 */
 
static int K_lxdr(unsigned char* ptr, unsigned char** xdr_ptr)
{
    return(K_oxdr(ptr, xdr_ptr));
}


/**
 *  Translates a little-endian packed Scalar into a big-endian packed Scalar or the opposite.
 *  Same behaviour as K_cxdr().
 */
 
static int K_sxdr(unsigned char* ptr, unsigned char** xdr_ptr)
{
    int     len;

    if(xdr_ptr == NULL) {
        /* intel read */
        K_xdrPACK(ptr, 0);
        XDR_rev(P_get_ptr(ptr, 0), 3, sizeof(double));
    }
    else {
        /* intel write */
        len = P_len(ptr);
        *xdr_ptr = SW_nalloc(len);
        memcpy(*xdr_ptr, ptr, len);
        len = P_get_len(ptr, 0);
        XDR_rev(P_get_ptr(*xdr_ptr, 0), 3, sizeof(double));

        K_xdrPACK(*xdr_ptr, len);
    }
}


/**
 *  Translates a little-endian packed Table into a big-endian packed Table or the opposite.
 *  Same behaviour as K_cxdr().
 */
 
static int K_txdr(unsigned char* ptr, unsigned char** xdr_ptr)
{
    int     len = 0;

    if(xdr_ptr == NULL) {
        /* intel read */
        K_xdrPACK(ptr, 0);
        K_xdrTable(ptr, 0);
    }
    else {
        /* intel write */
        len = P_len(ptr);
        *xdr_ptr = SW_nalloc(len);
        memcpy(*xdr_ptr, ptr, len);

        K_xdrTable(*xdr_ptr, 1);
        K_xdrPACK(*xdr_ptr, 1);
    }
}


/**
 *  Translates a little-endian packed VAR into a big-endian packed VAR or the opposite.
 *  Same behaviour as K_cxdr().
 */
 
static int K_vxdr(unsigned char* ptr, unsigned char** xdr_ptr)
{
    int     len;
    U_sh    *pos;

    if(xdr_ptr == NULL) {
        /* intel read */
        K_xdrPACK(ptr, 0);

        pos = (U_sh *) ptr;
        len = P_get_len(ptr, 0);
        XDR_rev(P_get_ptr(ptr, 0), len/sizeof(double), sizeof(double));
    }
    else {
        /* intel write */
        len = P_len(ptr);
        *xdr_ptr = SW_nalloc(len);
        memcpy(*xdr_ptr, ptr, len);
        len = P_get_len(ptr, 0);
        XDR_rev(P_get_ptr(*xdr_ptr, 0), len/sizeof(double), sizeof(double));

        K_xdrPACK(*xdr_ptr, len);
    }
}

//// non-static functions ////

/**
 *  Translates a short int from little-endian to big-endian or the other way round (the translation is symmetrical ?).
 *  
 *  @param [in, out]    a   unsigned char*  pointer to a short int
 *  
 */
void K_xdrPINT(unsigned char* a)
{
    XDR_rev(a, 1, sizeof(U_sh));
}


/**
 *  Translates a long int from little-endian to big-endian or the other way round.
 *  
 *  @param [in, out]    a   unsigned char*  pointer to a long int. 
 *                                          in: little-endian, out:big-endian
 */
void K_xdrPLONG(unsigned char* a)
{
    XDR_rev(a, 1, sizeof(long));
}


/**
 *  Switches the architecture of a KDB (little-endian to big-endian or the other way round). 
 *
 *  If okdb is null, the ikdb architecture is little-endian (when reading from a file) and the 
 *  transformation is done "in-place". No allocation is done.
 *  
 *  If okdb is not null, the ikdb is big-endian (in mem) and the result (little-endian) is allocated.
 *  ikdb is untouched in this case.
 * 
 *  @param [in, out]    ikdb    KDB*  pointer to a IODE packed object 
 *  @param [out]        okdb    KDB** NULL or pointer to the new allocated KDB (little-endian)
 */

void K_xdrKDB(KDB* ikdb, KDB** okdb)
{
    short   type;
    KDB     *xdr_kdb;
    Sample  *smpl;

    type = ikdb->k_type;

    if(okdb == NULL) {
        /* intel read */
        xdr_kdb = ikdb;
        K_xdrSHORT(&type);
    }
    else {
        /* intel write */
        xdr_kdb = (KDB*) SW_nalloc(sizeof(KDB));
        memcpy(xdr_kdb, ikdb, sizeof(KDB));

        *okdb = xdr_kdb;
    }

    XDR_rev(&(xdr_kdb->k_type), 1, sizeof(short));
    XDR_rev(&(xdr_kdb->k_mode), 1, sizeof(short));
    XDR_rev(&(xdr_kdb->k_nb), 1, sizeof(long));

    if(type == VARIABLES) {
        smpl = (Sample *) xdr_kdb->k_data;
        K_xdrSMPL(smpl);
    }
}


/**
 *  Table of function pointers, one function for each object type. 
 *  Each function converts an IODE packed object from little-endian architecture to big-endian architecture and vice-versa.
 *    
 */

int (*K_xdrobj[])(unsigned char*, unsigned char**) = {
    K_cxdr,
    K_exdr,
    K_ixdr,
    K_lxdr,
    K_sxdr,
    K_txdr,
    K_vxdr,
    K_oxdr
};

#endif
