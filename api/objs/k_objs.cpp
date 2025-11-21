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
 *  @return                           bool    true in case of success, false otherwise
 */
int K_dup(const KDB* kdb_source, const std::string& name1, KDB* kdb_dest, const std::string& name2)
{
    std::string error_msg = "Cannot duplicate object '" + name1 + "' as '" + name2 + "': ";
    if(kdb_source == kdb_dest && name1 == name2)
    {
        error_msg += "source and destination are identical.";
        kwarning(error_msg.c_str());
        return false;
    } 

    if(!kdb_source->contains(std::string(name1)))
    {
        error_msg += "object does not exist in the source database";
        kwarning(error_msg.c_str());
        return false;
    }

    bool success;
    SWHDL handle_dest = kdb_dest->get_handle(name2);
    if(handle_dest > 0) 
        SW_free(handle_dest);
    else
        success = kdb_dest->add_entry(name2);
        if(!success)
        {
            error_msg += "cannot add new entry to destination database.";
            kwarning(error_msg.c_str());
            return false;
        }
    
    SWHDL handle_source = kdb_source->get_handle(name1);
    if(handle_source == 0)
    {
        error_msg += "could not retrieve the object in the source database.";
        kwarning(error_msg.c_str());
        return false;
    } 

    char* ptr_source = SW_getptr(handle_source);
    int lg = * (OSIZE *) ptr_source;
    handle_dest = SW_alloc(lg);
    if(handle_dest == 0)
    {
        kwarning("failed to allocate memory for the duplicated object.");
        return false;
    }
    kdb_dest->k_objs[name2] = handle_dest;
    
    // pointer behind handle_2 may have changed after allocation of handle_1
    // The SW_alloc(lg) function searches for a contiguous space of lg bytes within one 
    // of the segments allocated by the SWAP system. If a segment contains enough free space 
    // but with gaps, the SW_alloc() function compresses the segment to have lg bytes contiguous.
    // In doing so, it (potentially) shifts the pack within the segment, and therefore 
    // ptr may change value after an SW_alloc() call.
    char* ptr_dest;
    ptr_source = SW_getptr(handle_source);
    ptr_dest = SW_getptr(handle_dest);
    memcpy(ptr_dest, ptr_source, lg);

    return true;
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
    if(!KE_WS->contains(name))
    {
        Period from_period = (smpl !=  NULL) ? smpl->start_period : Period();
        Period to_period = (smpl !=  NULL) ? smpl->end_period : Period();
        eq = new Equation(endo, lec, method, from_period.to_string(), to_period.to_string(), 
                          comment, instruments, block, false);
    } 
    else
    {
        eq = KEVAL(KE_WS, name);
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
