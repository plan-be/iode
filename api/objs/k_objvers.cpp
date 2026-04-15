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
 *     int get_version(char* label): returns the current object version (0-2) from an IODE file header. 
 */
#include "scr4/s_swap.h"        // SWHDL

#include "api/objs/kdb.h"
#include "api/objs/objs.h"
#include "api/objs/pack.h"
#include "api/objs/comments.h"
#include "api/objs/equations.h"
#include "api/objs/identities.h"
#include "api/objs/lists.h"
#include "api/objs/scalars.h"
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
int get_version(char* label)
{
    // version >= 5.64
    if(memcmp(label, K_LABEL, strlen(K_LABEL)) == 0) 
        return 0;
    
    // version >= 4.02
    if(memcmp(label, K_LABELS, strlen(K_LABELS)) == 0) 
        return 1;
    
    // version >= 5.04d
    if(memcmp(label, K_LABELD, strlen(K_LABELD)) == 0) 
        return 2;
    
    return -1;
}


/**
 *  Transforms a packed object from 16 bits integers to 32 bits integers. 
 *  
 *  @param [in]    opack    char*    16 bits integer packed object
 *  @return                 char*    32 bits integer packed object
 *  
 *  TODO: reuse the function in K_repack() => duplicate code.
 */

static char* Pack16To32(char* opack)
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

static bool lec_repack(char** pack, char* lec)
{
    *pack = 0;
    if(!lec) 
        return false;
    
    *pack = (char*) P_create();
    *pack = (char*) P_add(*pack, lec, (int) strlen(lec) + 1);
    // for backward compatibility with old binary files
    *pack = (char*) P_add(*pack, NULL, 0);

    return true;
}

/**
 *  Transforms a TableCell from 16 bits integers to 32 bits integers and appends the result
 *  to a pack which accumulates the TableCells of a Table (see K_repack_tbl()).
 *  
 *  @param [in]    pack    char*        NULL or pointer to a partial packed Table 
 *  @param [in]    cell    TableCell*   16 bits integer packed object
 *  @return                char*        pointer to pack with the cell appended
 *  
 */
static char* T_cell_repack(char* pack, TableCell* cell)
{
    char* ipack = NULL;
    char* opack = NULL;
   
    if(cell->get_type() == TABLE_CELL_LEC) 
    {
        if(cell->is_null()) 
            return(pack);
        std::string lec = cell->get_content();
        char* c_lec = (char*) lec.c_str();

        lec_repack(&opack, c_lec);
        char* npack = Pack16To32(opack);
        c_lec = (char*) P_get_ptr(npack, 0);
        lec_repack(&ipack, c_lec);

        pack = (char*) P_add(pack, ipack, P_len(ipack));
        SW_nfree(npack);
    }
    else
    {
        std::string text = cell->get_content(false);
        if(text.empty()) 
            return(pack);
        pack = (char*) P_add(pack, (void*) text.c_str(), (int) text.size() + 1);
    }

    return pack;
}


/**
 *  Packs a Table in IODE objects version 1 or 2 to a pack with long integer i.e. in the 
 *  current IODE objects version (version 0 = version 3).
 *  
 *  @param [in] tbl     Table*    pointer to a Table in IODE objects version 1 or 2
 *  @return             char*   packed Table in version 0 
 *  
 */

static char* K_repack_tbl(Table *tbl)
{
    char* pack = (char*) P_create();
    if(tbl == NULL) 
        return(NULL);

    /* tbl */
    pack = (char*) P_add(pack, (char *) tbl, sizeof(Table));

    /* div */
    TableCell* cells = tbl->divider_line.cells.data();
    pack = (char*) P_add(pack, (char *) cells, sizeof(TableCell) * (int) tbl->nb_columns);
    for(TableCell& cell: tbl->divider_line.cells)
        pack = T_cell_repack(pack, &cell);

    /* lines */
    TableCell* cell;
    TableLine* lines = tbl->lines.data();
    pack = (char*) P_add(pack, (char *) lines, sizeof(TableLine) * (int) tbl->lines.size());
    for(int i = 0; i < tbl->lines.size(); i++) 
    {
        switch(tbl->lines[i].get_type()) 
        {
            case TABLE_LINE_CELL :
                cells = tbl->lines[i].cells.data();
                pack = (char*) P_add(pack, (char *) cells, sizeof(TableCell) * (int) tbl->nb_columns);
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
 *  @param [in]         opack   char*   packed IODE object in version 1 or 2
 *  @param [in]         vers    int     IODE objects version to convert to
 *  @return                     char*   packed IODE object in version 0 or 3 (current version)
 */
char* KDB::convert_obj_version(const std::string& name, char* opack, const int vers)
{
    // nothing to do
    if(vers == 0 || vers == 3) 
        return opack;

    char* npack = Pack16To32(opack);

    npack = sub_convert_obj_version(npack, name, vers);
    return npack;
}

char* KDBComments::sub_convert_obj_version(char* npack, const std::string& name, const int vers)
{
    return npack;
}

char* KDBEquations::sub_convert_obj_version(char* npack, const std::string& name, const int vers)
{
    char *pack = NULL;

    Equation* eq = binary_to_eqs(npack, name);
    eq->to_binary(&pack);
    delete eq;

    return pack;
}

char* KDBIdentities::sub_convert_obj_version(char* npack, const std::string& name, const int vers)
{
    char *pack = NULL;

    char* lec = (char*) P_get_ptr(npack, 0);
    lec_repack(&pack, lec);

    return pack;
}

char* KDBLists::sub_convert_obj_version(char* npack, const std::string& name, const int vers)
{
    return npack;
}

char* KDBScalars::sub_convert_obj_version(char* npack, const std::string& name, const int vers)
{
    char *pack = NULL;

    if(vers == 2) 
        return npack;
            
    int nb = 3;
    float* f = (float*) P_get_ptr(npack, 0);

    Scalar scl;
    double value;
    char buf[512];
    for(int j = 0 ; j < 3 ; j++) 
    {
        value = (double) f[j];
        sprintf(buf, "%.8lg", value);
        value = atof(buf);
        if(IODE_IS_0(value)) 
            value = 0.0;
        
        if(j == 0) 
            scl.value = value;
        else if(j == 1) 
            scl.relax = value;
        else if(j == 2) 
            scl.std = value;
    }

    scl.to_binary(&pack);
    return pack;
}

char* KDBTables::sub_convert_obj_version(char* npack, const std::string& name, const int vers)
{
    char *pack = NULL;

    Table* tbl = binary_to_tbl(npack);
    pack = K_repack_tbl(tbl);
    delete tbl;

    return pack;
}

char* KDBVariables::sub_convert_obj_version(char* npack, const std::string& name, const int vers)
{   
    if(vers == 2) 
        return npack;

    int nb = sample->nb_periods;
    float* f = (float*) P_get_ptr(npack, 0);
    
    Variable var;
    double value;
    char buf[512];
    for(int j = 0 ; j < nb ; j++) 
    {
        sprintf(buf, "%.8g", f[j]);
        value = atof(buf);
        if(IODE_IS_0(value)) 
            value = 0.0;
        var.push_back(value);
    }

    char *pack = NULL;
    var_to_binary(&pack, var);
    return pack;
}
