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
#include "api/objs/kdb.h"
#include "api/objs/objs.h"
#include "api/objs/pack.h"
#include "api/objs/equations.h"
#include "api/objs/tables.h"
#include "api/objs/variables.h"

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
 *  @param [in]         index   int     position of the object to modify in kdb
 *  @return                     void
 *  
 *  @details Called by K_setvers() only for IODE object version 1 and 2.
 */

static void K_repack(KDB* kdb, int index)
{
    U_sh*  old_ptr;
    int    nlg, add, lendata, i;

    std::string name = kdb->get_name(index);
    SWHDL old_handle = kdb->get_handle(name);
    old_ptr = (U_sh *) SW_getptr(old_handle);
    add = (old_ptr[1] + 2 + old_ptr[1] % 2) * (sizeof(OSIZE) - sizeof(U_sh));
    nlg = old_ptr[0] + add;
    SWHDL handle = SW_alloc(nlg);
    old_ptr = (U_sh *) SW_getptr(old_handle);
    OSIZE* ptr = (OSIZE *) SW_getptr(handle);
    ptr[0] = nlg;
    ptr[1] = old_ptr[1];
    for(i = 2 ; i < old_ptr[1] + 2 ; i++)
        ptr[i] = old_ptr[i] + add;

    lendata = nlg - (ptr[1] + 2 + old_ptr[1] % 2) * sizeof(OSIZE);
    memcpy(((char *) ptr) + (ptr[1]  + 2 + ptr[1] % 2) * sizeof(OSIZE),
           ((char *)old_ptr) + (old_ptr[1] + 2 + old_ptr[1] % 2) * sizeof(U_sh),
           lendata);
    SW_free(old_handle);
    kdb->k_objs[name] = handle;
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
    U_sh    *old_ptr = (U_sh *)opack;
    int     nlg, add, lendata, i;

    add = (old_ptr[1] + 2 + old_ptr[1] % 2) * (sizeof(OSIZE) - sizeof(U_sh));
    nlg = old_ptr[0] + add;
    ptr = (OSIZE *)SW_nalloc(nlg);
    ptr[0] = nlg;
    ptr[1] = old_ptr[1];
    for(i = 2 ; i < old_ptr[1] + 2 ; i++)
        ptr[i] = old_ptr[i] + add;

    lendata = nlg - (ptr[1] + 2 + old_ptr[1] % 2) * sizeof(OSIZE);
    memcpy(((char *) ptr) + (ptr[1]  + 2 + ptr[1] % 2) * sizeof(OSIZE),
           ((char *)old_ptr) + (old_ptr[1] + 2 + old_ptr[1] % 2) * sizeof(U_sh),
           lendata);
    return((char *)ptr);
}


/**
 *  Transforms a TableCell from 16 bits integers to 32 bits integers and appends the result
 *  to a pack which accumulates the TableCells of a Table (see K_repack_tbl()).
 *  
 *  @param [in]    pack    char*    NULL or pointer to a partial packed Table 
 *  @param [in]    cell    TableCell*   16 bits integer packed object
 *  @return                char*    pointer to pack with the cell appended
 *  
 */

static char *T_cell_repack(char* pack, TableCell* cell)
{
    char *npack, *ipack, *opack, *c_lec;
    
    if(cell->get_type() == TABLE_CELL_LEC) 
    {
        if(cell->is_null()) 
            return(pack);
        std::string lec = cell->get_content();
        c_lec = (char*) lec.c_str();
        K_ipack(&opack, c_lec);
        npack = Pack16To32(opack);
        ipack = 0;
        K_ipack(&ipack, (char*) P_get_ptr(npack, 0));
        pack = (char*) P_add(pack, ipack, P_len(ipack));
        SW_nfree(npack);
    }
    else
    {
        std::string text = cell->get_content(false, false);
        if(text.empty()) 
            return(pack);
        pack = (char*) P_add(pack, (void*) text.c_str(), (int) text.size() + 1);
    }

    return(pack);
}


/**
 *  Packs a Table in IODE objects version 1 or 2 to a pack with long integer i.e. in the 
 *  current IODE objects version (version 0 = version 3).
 *  
 *  @param [in] tbl     Table*    pointer to a Table in IODE objects version 1 or 2
 *  @return             char*   packed Table in version 0 
 *  
 */

static char *K_repack_tbl(Table *tbl)
{
    char* pack = (char*) P_create();
    if(tbl == NULL) 
        return(NULL);

    /* tbl */
    pack = (char*) P_add(pack, (char *) tbl, sizeof(Table));

    /* div */
    TableCell* cells = tbl->divider_line.cells.data();
    pack = (char*) P_add(pack, (char *) cells, sizeof(TableCell) * (int) T_NC(tbl));
    for(TableCell& cell: tbl->divider_line.cells)
        pack = T_cell_repack(pack, &cell);

    /* lines */
    TableCell* cell;
    TableLine* lines = tbl->lines.data();
    pack = (char*) P_add(pack, (char *) lines, sizeof(TableLine) * (int) T_NL(tbl));
    for(int i = 0; i < T_NL(tbl); i++) 
    {
        switch(tbl->lines[i].get_type()) 
        {
            case TABLE_LINE_CELL :
                cells = tbl->lines[i].cells.data();
                pack = (char*) P_add(pack, (char *) cells, sizeof(TableCell) * (int) T_NC(tbl));
                for(TableCell& cell: tbl->lines[i].cells)
                    pack = T_cell_repack(pack, &cell);
                break;

            case TABLE_LINE_TITLE :
                cell = &(tbl->lines[i].cells[0]);
                pack = (char*) P_add(pack, (char *) cell, sizeof(TableCell));
                pack = T_cell_repack(pack, cell);
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
    float*    f;
    double*   d;
    int       j, nb, lg;
    char      *ptr, *old_ptr, *pack;
    SWHDL     handle, old_handle;
    char      buf[512];
    Equation* eq;
    Table*    tbl;
    
    if(vers == 0 || vers == 3) 
    return;
    
    std::string name = kdb->get_name(i);
    K_repack(kdb, i);
    switch(kdb->k_type) 
    {
        case VARIABLES :
            if(vers == 2) 
                return;
            old_handle = kdb->get_handle(name);
            nb = kdb->sample->nb_periods;
            handle = KV_alloc_var(nb);
            ptr = SW_getptr(handle);
            d = (double *)P_get_ptr(ptr, 0);
            old_ptr = SW_getptr(old_handle);
            f = (float *)P_get_ptr(old_ptr, 0);
            for(j = 0 ; j < nb ; j++) 
            {
                sprintf(buf, "%.8g", f[j]);
                d[j] = atof(buf);
                if(IODE_IS_0(d[j])) 
                    d[j] = 0.0;
            }
            SW_free(old_handle);
            kdb->k_objs[name] = handle;
            break;

        case SCALARS :
            if(vers == 2) 
                return;
            nb = 3;
            /* Do New allocation & get dbl ptr*/
            handle = KS_alloc_scl();
            ptr = SW_getptr(handle);
            d = (double *)P_get_ptr(ptr, 0);

            /* Get old allocation and get float ptr */
            old_handle = kdb->get_handle(name);
            old_ptr = SW_getptr(old_handle);
            f = (float *)P_get_ptr(old_ptr, 0);

            /* Convert */
            for(j = 0 ; j < 3 ; j++) 
            {
                d[j] = (double)f[j];
                //Debug("%.8lg", d[j]);
                sprintf(buf, "%.8lg", d[j]);
                d[j] = atof(buf);
                if(IODE_IS_0(d[j])) 
                    d[j] = 0.0; /* JMP 30-10-98 */
            }

            SW_free(old_handle);
            kdb->k_objs[name] = handle;
            break;

        case IDENTITIES :
            old_handle = kdb->get_handle(name);
            old_ptr = SW_getptr(old_handle);
            K_ipack(&pack, (char*) P_get_ptr(old_ptr, 0));
            SW_free(old_handle);
            lg = P_get_len(pack, -1);
            handle = SW_alloc(lg);
            memcpy(SW_getptr(handle), pack, lg);
            SW_nfree(pack);
            kdb->k_objs[name] = handle;
            break;

        case EQUATIONS :
            old_handle = kdb->get_handle(name);
            old_ptr = SW_getptr(old_handle);
            eq = K_eunpack(old_ptr, (char*) name.c_str());
            SW_free(old_handle);
            K_epack(&pack, (char*) eq, (char*) name.c_str());
            delete eq;
            eq = nullptr;
            lg = P_get_len(pack, -1);
            handle = SW_alloc(lg);
            memcpy(SW_getptr(handle), pack, lg);
            SW_nfree(pack);
            kdb->k_objs[name] = handle;
            break;

        case TABLES :
            old_handle = kdb->get_handle(name);
            old_ptr = SW_getptr(old_handle);
            tbl = K_tunpack(old_ptr, (char*) name.c_str());
            SW_free(old_handle);
            pack = K_repack_tbl(tbl);
            delete tbl;
            tbl = nullptr;
            lg = P_get_len(pack, -1);
            handle = SW_alloc(lg);
            memcpy(SW_getptr(handle), pack, lg);
            SW_nfree(pack);
            kdb->k_objs[name] = handle;
            break;

        default :
            break;
    }

    return;
}

