/**
 * @header4iode
 *
 * IODE objects version management
 * -------------------------------
 * The first 11 bytes of each IODE DB file determine the "version" of the objects contained in the file. 
 * (not to be confused with the IODE software version (6.xx for ex.).
 *
 * These HEADERS are defined in the variables below : 
 *    K_LABEL = current version of the IODE file headers
 *    K_LABEL{S,D,X,...} =  previous versions
 *  
 *  This way, if a new version of the objects is defined in the future, K_LABELY for example, 
 *  K_LABEL will become K_LABELY and will be associated with object version 0 (the new current version), 
 *  whereas K_LABELX will be associated with object version 3 (instead of 0).
 *  
 *  Functions
 *  ---------
 *  Determine IODE object file version and convert an object to the current IODE version.
 *  
 *     int K_calcvers(char* label):               returns the current object version (0-2) from an IODE file header. 
 *     void K_setvers(KDB* kdb, int i, int vers): converts an IODE object from IODE objects version 1 or 2 to the current version (0).
 *  
 */

#include "iode.h"

char    K_LABELS[] = "KOBJS 4.02\032";  // Version 1
char    K_LABELD[] = "KOBJS 504d\032";  // Version 2
char    K_LABELX[] = "KOBJS 564A\032";  // Version 3 
char    K_LABEL[]  = "KOBJS 564A\032";  // Version 0 = Current version = Version 3

/**
 *  Returns the current object version (0-2) of an IODE file header. 
 *  
 *  Three formats of the IODE databases files are currently recognized. 
 *  Each is identified by an index between 0 and 2 :
 *      4.02  (version 1) 
 *      5.04d (version 2) 
 *      5.64a (version 0) = last version of the IODE databases
 *  
 *  @param [in]     label   char*   header of an IODE file
 *  @return                 int     0 for format of IODE 5.64 (current version of IODE)
 *                                  1 for format of IODE 4.02
 *                                  2 for format of IODE 5.04d
 *                                  -1 if not recognized
 */
 
 int K_calcvers(char* label)
{
    if(memcmp(label, K_LABEL, strlen(K_LABEL)) == 0) return(0);     // Current version (5.64)
    if(memcmp(label, K_LABELS, strlen(K_LABELS)) == 0) return(1);
    if(memcmp(label, K_LABELD, strlen(K_LABELD)) == 0) return(2);
//      if(memcmp(label, K_LABELX, strlen(K_LABELX)) == 0) return(3);
    return(-1);
}

/**
 *  Adds bytes to a packed object to comply with the current version of IODE objects where
 *  short integers become long integers.
 *  The object content is replaced by a new allocated one in the swap memory.
 *  
 *  @param [in, out]    kdb     KDB*    KDB containing object to modify
 *  @param [in]         objnb   int     position of the object to modify in kdb
 *  @return                     void
 *  
 *  @details Called by K_setvers() only for IODE object version 1 and 2.
 */

static void K_repack(KDB* kdb, int objnb)
{
    SWHDL   opos, pos;
    OSIZE   *ptr;
    U_sh    *optr;
    int     nlg, add, lendata, i;

    opos = KOBJS(kdb)[objnb].o_val;
    optr = (U_sh *) SW_getptr(opos);
    add = (optr[1] + 2 + optr[1] % 2) * (sizeof(OSIZE) - sizeof(U_sh));
    nlg = optr[0] + add;
    pos = SW_alloc(nlg);
    optr = (U_sh *) SW_getptr(opos);
    ptr = (OSIZE *) SW_getptr(pos);
    ptr[0] = nlg;
    ptr[1] = optr[1];
    for(i = 2 ; i < optr[1] + 2 ; i++)
        ptr[i] = optr[i] + add;

    lendata = nlg - (ptr[1] + 2 + optr[1] % 2) * sizeof(OSIZE);
    memcpy(((char *) ptr) + (ptr[1]  + 2 + ptr[1] % 2) * sizeof(OSIZE),
           ((char *)optr) + (optr[1] + 2 + optr[1] % 2) * sizeof(U_sh),
           lendata);
    SW_free(opos);
    KOBJS(kdb)[objnb].o_val = pos;
}

/**
 *  Transforms a packed object from 16 bits integers to 32 bits integers. 
 *  
 *  @param [in]    opack    char*    16 bits integer packed object
 *  @return                 char*    32 bits integer packed object
 *  
 *  TODO: reuse the function in K_repack() => duplicate code.
 */

static char *Pack16To32(char* opack)
{
    OSIZE   *ptr;
    U_sh    *optr = (U_sh *)opack;
    int     nlg, add, lendata, i;

    add = (optr[1] + 2 + optr[1] % 2) * (sizeof(OSIZE) - sizeof(U_sh));
    nlg = optr[0] + add;
    ptr = (OSIZE *)SW_nalloc(nlg);
    ptr[0] = nlg;
    ptr[1] = optr[1];
    for(i = 2 ; i < optr[1] + 2 ; i++)
        ptr[i] = optr[i] + add;

    lendata = nlg - (ptr[1] + 2 + optr[1] % 2) * sizeof(OSIZE);
    memcpy(((char *) ptr) + (ptr[1]  + 2 + ptr[1] % 2) * sizeof(OSIZE),
           ((char *)optr) + (optr[1] + 2 + optr[1] % 2) * sizeof(U_sh),
           lendata);
    return((char *)ptr);
}


/**
 *  Transforms a TCELL from 16 bits integers to 32 bits integers and appends the result
 *  to a pack which accumulates the TCELLs of a TBL (see K_repack_tbl()).
 *  
 *  @param [in]    pack    char*    NULL or pointer to a partial packed TBL 
 *  @param [in]    cell    TCELL*   16 bits integer packed object
 *  @return                char*    pointer to pack with the cell appended
 *  
 */

static char *T_cell_repack(char* pack, TCELL* cell)
{
    char    *npack, *ipack;

    if(cell->tc_val == NULL) return(pack);
    if(cell->tc_type == KT_LEC) {
        npack = Pack16To32(cell->tc_val);
        ipack = 0;
        K_ipack(&ipack, P_get_ptr(npack, 0));
        pack  = P_add(pack, ipack, P_len(ipack));
        SW_nfree(npack);
    }
    else
        pack = P_add(pack, cell->tc_val, (int)strlen(cell->tc_val) + 1);

    return(pack);
}


/**
 *  Packs a TBL in IODE objects version 1 or 2 to a pack with long integer i.e. in the 
 *  current IODE objects version (version 0 = version 3).
 *  
 *  @param [in] tbl     TBL*    pointer to a TBL in IODE objects version 1 or 2
 *  @return             char*   packed TBL in version 0 
 *  
 */

static char *K_repack_tbl(TBL *tbl)
{
    //TLINE   *line;
    TCELL   *cell;
    int     i, j;
    char    *pack;

    pack = P_create();
    if(tbl == NULL) return(NULL);

    /* tbl */
    pack = P_add(pack, (char *) tbl, sizeof(TBL));

    /* div */
    cell = (TCELL *) tbl->t_div.tl_val;
    pack= P_add(pack, (char *) cell, sizeof(TCELL) * (int) T_NC(tbl));
    for(j = 0; j < T_NC(tbl); j++)
        pack = T_cell_repack(pack, cell + j);

    /* lines */
    pack= P_add(pack, (char *) tbl->t_line, sizeof(TLINE) * (int) T_NL(tbl));
    for(i = 0; i < T_NL(tbl); i++) {
        switch(tbl->t_line[i].tl_type) {
            case KT_CELL :
                cell = (TCELL *) tbl->t_line[i].tl_val;
                pack= P_add(pack, (char *) cell, sizeof(TCELL) * (int) T_NC(tbl));
                for(j = 0; j < T_NC(tbl); j++)
                    pack = T_cell_repack(pack, cell + j);
                break;
            case KT_TITLE :
                cell = (TCELL *) tbl->t_line[i].tl_val;
                pack= P_add(pack, (char *) cell, sizeof(TCELL));
                pack= T_cell_repack(pack, cell);
                break;
        }
    }

    return(pack);
}


/**
 *  Converts an IODE object from IODE objects version 1 or 2 to the current version (0 or 3).
 *  
 *  @param [in, out]    kdb     KDB*    KDB source 
 *  @param [in]         i       int     position of the object on kdb
 *  @param [in]         vers    int     IODE objects version to convert to
 *  @return                     void
 */
 
void K_setvers(KDB* kdb, int i, int vers)
{
    float   *f;
    double  *d;
    int     j, nb, lg;
    char    *ptr, *optr, *pack;
    SWHDL   pos, opos;
    char    buf[512];
    //unsigned char *dptr;
    EQ      *eq;
    TBL     *tbl;

    if(vers == 0 || vers == 3) return;
    K_repack(kdb, i);
    switch(KTYPE(kdb)) {
        case K_VAR :
            if(vers == 2) return;
            opos = KOBJS(kdb)[i].o_val;
            nb = KSMPL(kdb)->s_nb;
            pos = KV_alloc_var(nb);
            ptr = SW_getptr(pos);
            d = (double *)P_get_ptr(ptr, 0);
            optr = SW_getptr(opos);
            f = (float *)P_get_ptr(optr, 0);
            for(j = 0 ; j < nb ; j++) {
                sprintf(buf, "%.8g", f[j]);
                d[j] = atof(buf);
                if(L_IS0(d[j])) d[j] = 0.0; /* JMP 30-10-98 */
                /*d[j] = (double)f[j]; */
            }
            SW_free(opos);
            KOBJS(kdb)[i].o_val = pos;
            break;

        case K_SCL :
            if(vers == 2) return;
            nb = 3;
            /* Do New allocation & get dbl ptr*/
            pos = KS_alloc_scl();
            ptr = SW_getptr(pos);
            d = (double *)P_get_ptr(ptr, 0);

            /* Get old allocation and get float ptr */
            opos = KOBJS(kdb)[i].o_val;
            optr = SW_getptr(opos);
            f = (float *)P_get_ptr(optr, 0);

            /* Convert */
            for(j = 0 ; j < 3 ; j++) {
                d[j] = (double)f[j];
                //Debug("%.8lg", d[j]);
                sprintf(buf, "%.8lg", d[j]);
                d[j] = atof(buf);
                if(L_IS0(d[j])) d[j] = 0.0; /* JMP 30-10-98 */
            }

            SW_free(opos);
            KOBJS(kdb)[i].o_val = pos;
            break;

        case K_IDT :
            opos = KOBJS(kdb)[i].o_val;
            optr = SW_getptr(opos);
            K_ipack(&pack, P_get_ptr(optr, 0));
            SW_free(opos);
            lg = P_get_len(pack, -1);
            pos = SW_alloc(lg);
            memcpy(SW_getptr(pos), pack, lg);
            SW_nfree(pack);
            KOBJS(kdb)[i].o_val = pos;
            break;

        case K_EQS :
            opos = KOBJS(kdb)[i].o_val;
            optr = SW_getptr(opos);
            eq = K_eunpack(optr);
            SW_free(opos);
            K_epack(&pack, (char *)eq, KOBJS(kdb)[i].o_name);
            E_free(eq);
            lg = P_get_len(pack, -1);
            pos = SW_alloc(lg);
            memcpy(SW_getptr(pos), pack, lg);
            SW_nfree(pack);
            KOBJS(kdb)[i].o_val = pos;
            break;

        case K_TBL :
            opos = KOBJS(kdb)[i].o_val;
            optr = SW_getptr(opos);
            tbl = K_tunpack(optr);
            SW_free(opos);
            pack = K_repack_tbl(tbl);
            T_free(tbl);
            lg = P_get_len(pack, -1);
            pos = SW_alloc(lg);
            memcpy(SW_getptr(pos), pack, lg);
            SW_nfree(pack);
            KOBJS(kdb)[i].o_val = pos;
            break;

        default :
            break;
    }

    return;
}

