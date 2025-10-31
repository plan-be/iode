#include "scr4/s_args.h"
#include "scr4/s_prodt.h"

#include "api/constants.h"
#include "api/b_args.h"
#include "api/b_errors.h"
#include "api/k_super.h"
#include "api/utils/buf.h"
#include "api/objs/kdb.h"
#include "api/objs/pack.h"
#include "api/objs/grep.h"
#include "api/objs/objs.h"
#include "api/objs/equations.h"
#include "api/objs/tables.h"
#include <algorithm>                // for std::min, std::max


// API
// ---


/**
 *  Duplicates an IODE object. Copies name1 of kdb_source to name2 in kdb_dest.
 *  If name2 exists in kdb_dest, it is replaced by the value of name1.
 *  
 *  @param [in]         kdb_source    KDB*    KDB source    
 *  @param [in]         name1         char*   object name of the source in kdb_source
 *  @param [in, out]    kdb_dest      KDB*    KDB target
 *  @param [in]         name2         char*   name of the new object
 *  @return                           int     position of name2 in kdb_dest in case of success
 *                                            -1 if name1 does not exist in kdb_source
 *                                            -2 if name2 cannot be added to the kdb_dest (memory...)
 */

int K_dup(const KDB* kdb_source, const std::string& name1, KDB* kdb_dest, const std::string& name2)
{
    if(kdb_source == kdb_dest && name1 == name2) 
        return -2;

    if(!kdb_source->contains(std::string(name1))) 
        return -1;

    bool success;
    SWHDL handle_dest = kdb_dest->get_handle(name2);
    if(handle_dest > 0) 
        SW_free(handle_dest);
    else
        success = K_add_entry(kdb_dest, name2);
        if(!success) 
            return -2;
    
    SWHDL handle_source = kdb_source->get_handle(name1);
    if(handle_source == 0) 
        return -2;
    char* ptr_source = SW_getptr(handle_source);
    int lg = * (OSIZE *) ptr_source;

    // WARNING: (ALD) I have no idea why calling SW_nalloc() first and 
    //          using an intermediate pointer ptr is neccessary, but otherwise
    //          the copy does not work properly and results in corrupted data.
    char* ptr = SW_nalloc(lg);
    memcpy(ptr, ptr_source, lg);

    handle_dest = SW_alloc(lg);
    if(handle_dest == 0) 
        return -2;
    kdb_dest->k_objs[name2] = handle_dest;
    
    char* ptr_dest = SW_getptr(handle_dest);
    memcpy(ptr_dest, ptr, lg);

    SW_nfree(ptr);

    int pos = kdb_dest->index_of(name2);
    return pos;
}


/**
 *  Adds the new entry newname in kdb and returns true if successful. 
 *  If newname exists, returns true and if K_WARN_UP is not null, displays an error message.
 *
 *  The kdb can be of any type but the name must comply to the naming conventions of kdb's type (UPPER, LOWER...).
 *  
 *  @detail The object names are stored in the table kdb->k_objs. To avoid a reallocation on each new insertion, K_CHUNCK elements 
 *          are added to kdb->k_objs each time that more place is needed to store object names.
 *
 *          Names in KOBJS are stored in alphabetic order to speed up the retrieval of an object by its name. 
 *          Consequently, K_add_entry has to calculate the place where the name must be inserted. For (even) more speed when 
 *          elements are added in alphabetic order, the function first checks that the new name must not be placed at the end or the beginning 
 *          of the table in which case, no search is required.
 * 
 *  @param [in, out]    kdb     KDB*    KDB source and target
 *  @param [in]         newname char*   new object name 
 *  @return                     bool    true in case of success, false if kdb is NULL
 */
 
bool K_add_entry(KDB* kdb, const std::string& name)
{
    if(!kdb) 
        return false;
    
    check_name(std::string(name), kdb->k_type);
    
    bool found = kdb->contains(name);
    if(found) 
    {
        if(K_WARN_DUP)
        {
            kerror(0, "%s already defined", (char*) name.c_str());
            return false;
        }
    }
    else
        kdb->k_objs[name] = 0;

    return true;
}


/**
 *  Deletes an entry in a KDB __without__ deleting the referenced object. 
 *
 *  Warning : for internal use (no check is made on pos).
 *  
 *  @param [in, out]    kdb     KDB*    KDB source and target
 *  @param [in]         pos     int     object position in kdb
 *  @return                     int     0 (should be void fn)
 */

int K_del_entry(KDB* kdb, int pos)
{
    if(!kdb) 
        return -1;

    std::string name = kdb->get_name(pos);
    if(name.empty()) 
        return -1;
    
    kdb->k_objs.erase(name);
    return 0;
}


/**
 *  Updates equation field(s). Creates the equation if it doesn't exist). All fields do not have to be updated during one call: only the non NULL
 *  parameters are taken into account.
 *  
 *  @param [in] char*   name    equation name (also endogenous)
 *  @param [in] char*   lec     NULL or LEC equation    
 *  @param [in] char*   cmt     NULL or free comment
 *  @param [in] int     method  -1 or estimation method
 *  @param [in] Sample* smpl    NULL or estimation sample
 *  @param [in] char*   instr   NULL or list of instruments 
 *  @param [in] char*   blk     NULL or list of simultaneously esstimated equations (block) 
 *  @param [in] float*  tests   NULL or vector of statistical tests
 *  @param [in] int     date    0 or last estimation date
 *  @return     int             0 on success, -1 on error
 *   
 */
int K_upd_eqs(char* name, char* c_lec, char* cmt, int i_method, Sample* smpl, char* instr, char* blk, float* tests, int i_date)
{
    bool success;

    std::string endo(name);
    std::string lec;
    std::string comment;
    std::string instruments;
    std::string block;
    IodeEquationMethod method = EQ_LSQ;

    if(c_lec != NULL)
        lec = std::string(c_lec);

    if(cmt != NULL)
        comment = std::string(cmt);

    if(instr != NULL)
        instruments = std::string(instr);

    if(blk != NULL)
        block = std::string(blk);

    if(i_method >= 0 && i_method < IODE_NB_EQ_METHODS) 
        method = (IodeEquationMethod) i_method;

    Equation* eq;
    int pos = KE_WS->index_of(name);
    if(pos < 0)
    {
        Period from_period = (smpl !=  NULL) ? smpl->start_period : Period();
        Period to_period = (smpl !=  NULL) ? smpl->end_period : Period();
        eq = new Equation(endo, lec, method, from_period.to_string(), to_period.to_string(), 
                          comment, instruments, block, false);
    } 
    else
    {
        eq = KEVAL(KE_WS, pos);
        // modify only if not empty
        if(!lec.empty())
            eq->set_lec(lec);
        // modify only if not null (negative)
        if(i_method >= 0)
            eq->set_method(method);
        // modify only if not empty
        if(!comment.empty()) 
            eq->set_comment(comment);
        // modify only if not empty
        if(!instruments.empty())
            eq->instruments = instruments;
        // modify only if not empty
        if(!block.empty())
            eq->block = block;
        // modify only if not null
        if(smpl) 
            eq->sample = *smpl;
    }

    if(tests != NULL)  
        memcpy(eq->tests.data(), tests, EQS_NBTESTS * sizeof(float));

    if(i_date > 0)
        eq->update_date();

    success = K_add(K_WS[EQUATIONS], name, eq, name);
    delete eq;
    eq = nullptr;
    if(!success) 
    {
        error_manager.append_error(std::string(L_error()));
        return -1;
    }
    
    return 0;
}


char T_SEPS[] = ";\n\t";


/**
 *  Creates a basic table with an optional TITLE and optional variable names and/or lec formulas separated by semi-colons.
 *  
 *  For the variables having a comment (with the same name) in the current CMT WS, the line title is replaced by the comment.
 *  For the LEC formulas and the other variables, the line titles are the formulas / var names.
 *  
 *  @param [in] char*  name     name of the new table
 *  @param [in] char*  arg      initial parameters separated by semi-colons: "Title;LEC_line1;LEC_line2..."
 *  @return     int             0 on success, -1 on error        
 *  
 */
int K_upd_tbl(char* name, char* arg)
{
    Table* tbl = nullptr;

    if(arg == NULL)
        tbl = new Table(2);
    else
    {
        char* OLD_SEPS = A_SEPS;
        A_SEPS = T_SEPS;
        std::vector<std::string> v_args = split_multi(std::string(arg), std::string(A_SEPS));
        A_SEPS = OLD_SEPS;

        if(v_args.size() == 0)
            tbl = new Table(2);
        else
        {
            std::string title = v_args.front();
            v_args.erase(v_args.begin());
            tbl = new Table(2, title, v_args, false, false, false);
        }
    }

    bool success = K_add(K_WS[TABLES], name, tbl);
    delete tbl;
    tbl = nullptr;

    return success ? 0 : -1;
}
