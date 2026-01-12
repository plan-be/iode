/**
 * @header4iode
 *
 * KDB management functions
 * ------------------------
 *
 * KDB structs contain the handles of all IODE objects of the same type plus some technical information like the 
 * filename the kdb has been read from, the processor architecture...
 *
 * For each IODE object type, there is one KDB (also called "workspace") in memory. 
 * 
 * To simplify the use of these structs, some MACRO have been defined for KDB in iode.h: for example "global_ws_var" for the KDB of variables...
 *
 * For some very specific operations (comparison of workspaces for example), temporary KDB may be created for the duration 
 * of the operation.
 * 
 *      int K_merge(KDB* kdb1, KDB* kdb2, int replace)                  // merges two databases : kdb1 <- kdb1 + kdb2. 
 *      int K_merge_del(KDB* kdb1, KDB* kdb2, int replace)              // merges two databases : kdb1 <- kdb1 + kdb2 then deletes kdb2. 
 */
#include "scr4/s_dir.h"

#include "api/b_errors.h"
#include "api/k_lang.h"
#include "api/time/period.h"
#include "api/time/sample.h"
#include "api/gsample/gsample.h"
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
#include "api/print/print.h"
#include "api/report/undoc/undoc.h"


// API 
// ---

KDB* get_global_db(const int iode_type)
{
    switch(iode_type)
    {
        case COMMENTS :
            return global_ws_cmt.get();
        case EQUATIONS :
            return global_ws_eqs.get();
        case IDENTITIES :
            return global_ws_idt.get();
        case LISTS :
            return global_ws_lst.get();
        case SCALARS :
            return global_ws_scl.get();
        case TABLES :
            return global_ws_tbl.get();
        case VARIABLES :
            return global_ws_var.get();
        default :
            return nullptr;
    }
}

bool KDB::set_packed_object(const std::string& name, char* pack)
{
    if(pack == NULL) 
    {
        std::string error_msg = "Failed to add the " + v_iode_types[this->k_type];
        error_msg += + " object '" + name + "' to the database.";
        kerror(0, error_msg.c_str());
        return false;
    }

    // Add an entry (name) if not present yet
    bool success = this->add_entry(name);
    if(!success) 
    {
        error_manager.append_error(v_iode_types[this->k_type] + " " + name + 
                                   " cannot be created (syntax ?)");
        SW_nfree(pack);
        return false;
    }
    
    // frees existing object but keeps the entry in kdb
    SWHDL handle = this->get_handle(name);
    if(handle > 0) 
        SW_free(handle);

    // allocate space in SCR SWAP memory 
    // NOTE: if handle > 0 (object previously present in the KDB), 
    //       SW_alloc() will return the same handle as SW_alloc()
    //       first searches for handle associated with freed item 
    //       and reallocates it
    int lg = * (OSIZE *) pack;
    handle = SW_alloc(lg);
    if(handle == 0) 
    {
        error_manager.append_error("Low memory: cannot allocate " + std::to_string(lg) + 
                                   " bytes for " + v_iode_types[this->k_type] + " " + std::string(name));
        SW_nfree(pack);
        return false;
    }

    // Copy allocated pack into the SCR SWAP memory
    char* ptr_swap = SW_getptr(handle);
    memcpy(ptr_swap, pack, lg);
    SW_nfree(pack);    

    // store handle in top-level database
    KDB* top_db = get_top_level_db();
    top_db->k_objs[name] = handle;

    // store handle in all subset instances (including 'this' if 'this' is a subset)
    for(KDB* subset : get_subsets())
        if(subset->contains(name))
            subset->k_objs[name] = handle;

    return true;
}

bool KDB::duplicate(const KDB& other, const std::string& old_name, const std::string& new_name)
{
    std::string error_msg = "Cannot duplicate object '" + old_name + "' as '" + new_name + "': ";
    if(this == &other && old_name == new_name)
    {
        error_msg += "source and destination are identical.";
        kwarning(error_msg.c_str());
        return false;
    } 

    if(!other.contains(old_name))
    {
        error_msg += "object '" + old_name + "' does not exist in the source database";
        kwarning(error_msg.c_str());
        return false;
    }

    bool success;
    SWHDL handle_dest = this->get_handle(new_name);
    if(handle_dest > 0) 
        SW_free(handle_dest);
    else
        success = this->add_entry(new_name);
        if(!success)
        {
            error_msg += "Failed to add a new entry to destination database.";
            kwarning(error_msg.c_str());
            return false;
        }
    
    SWHDL handle_source = other.get_handle(old_name);
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
    k_objs[new_name] = handle_dest;
    
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

char* KDB::dde_create_obj(int objnb, int *nc, int *nl)
{
    if(objnb < 0 || objnb >= this->size())
        return NULL;

    std::string name = this->get_name(objnb);
    
    char *res;
    if(this->k_type != TABLES) 
    {
        *nc = 2;
        *nl = 1;

        char* obj = dde_create_obj_by_name(name, nc, nl);
        if(obj == NULL) 
            obj = (char*) " ";
        res = SCR_malloc((int)sizeof(ONAME) + 10 + (int)strlen(obj));
        strcpy(res, name.c_str());
        strcat(res, "\t");
        strcat(res, obj);
    }
    else
    {
        CKDBTables* tbl_db = (CKDBTables*) this;
        res = tbl_db->dde_create_table(name, NULL, nc, nl, -1);
    }

    SCR_OemToAnsi((unsigned char*) res, (unsigned char*) res);
    return(res);
}


/**
 *  Print a header and a modified text: spaces are added before and after specific characters in the text.
 *  If the text is NULL, print only a \n.
 *  
 *  A space is added before each '+' ':' and ']'.
 *  A space is added after  each '=', '+', ',', ')' '\n'.
 *  
 *  @param [in] head char *     header
 *  @param [in] txt  char *     text 
 *  @return          bool       true if success
 */
bool dump_string(char* head, char* txt)
{
    if(txt) 
    {
        W_printf((char*) "\n%s", head);

        char ch;
        std::string str_txt = std::string(txt);
        for(int i = 0; i < str_txt.size(); i++) 
        {
            ch = txt[i];

            if(ch == '+' || ch == ':' || ch == ']') 
                W_putc(' ');
            
            W_putc(ch);

            if(ch == '=' || ch == '+' || ch == ',' || ch == ')' || ch == '\n') 
                W_putc(' ');
        }
    }

    W_printf((char*) "\n");
    return true;
}


/**
 *  Print an object name and its title in an enum_1 paragraph.
 *          
 *  @param [in] name    char*   object name
 *  @param [in] text    char*   object title of definition    
 *  @return             bool    true if success
 */
bool print_definition_generic(const std::string& name, char* text)
{
    char buf[80];

    sprintf(buf, ".par1 enum_1\n%cb%s%cB : ", A2M_ESCCH, name.c_str(), A2M_ESCCH);
    dump_string(buf, text);
    return true;
}

/**
 *  Sets the KDB full path name. 
 *  
 *  The current filename stored in the KDB is freed and space for the new filename is allocated in the KDB.
 *  
 *  @param [in, out]    kdb      KDB*   KDB whose name will be changed
 *  @param [in]         filename char*  new filename
 *  
 *  @details More details   
 */
void K_set_kdb_fullpath(KDB *kdb, U_ch *filename) 
{
    char    *ptr, fullpath[1024];
    
    ptr = SCR_fullpath((char*) filename, fullpath);
    if(ptr == 0) 
        ptr = (char*) filename;
    kdb->filepath = std::string((char*) ptr);  
}

/**
 *  Merges two databases : kdb1 <- kdb1 + kdb2. 
 *   If replace == 0, existing elements in kdb1 are not overwritten.
 *   If replace == 1, the values of existing elements are replaced.
 *
 *  kdb2 is kept unmodified.
 *  
 *  @param [in, out]    kdb1    KDB*      source and target KDB
 *  @param [in]         kdb2    KDB*      source KDB
 *  @param [in]         replace int       0 to keep elements of kdb1 when duplicate objects exist
 *                                        1 to replace values in kdb1 by these in kdb2
 *  @return                     int       -1 if kdb1 or kdb2 is null
 */

int K_merge(KDB* kdb1, KDB* kdb2, int replace)
{
    bool  found;
    char  *ptr_1, *ptr_2;
    SWHDL handle_1;

    if(!kdb1)
    {
        kwarning("Cannot merge into a NULL database");
        return -1;
    }

    if(!kdb2)
    {
        kwarning("Cannot merge from a NULL database");
        return -1;
    }
    
    for(const auto& [name, handle_2] : kdb2->k_objs) 
    {
        if(handle_2 == 0) 
            continue;

        found = kdb1->contains(name);
        if(found)
        {
            // do not replace existing object in kdb1 if replace == 0
            if(replace == 0) 
                continue;
            // delete object in kdb1 if replace == 1
            handle_1 = kdb1->get_handle(name);
            if(handle_1 > 0)
                SW_free(handle_1);
        }
        
        ptr_2 = SW_getptr(handle_2);

        // allocate new object in kdb1 on the SCR swap
        handle_1 = SW_alloc(P_len(ptr_2));
        if(handle_1 == 0) 
            return -1;
        kdb1->k_objs[name] = handle_1;

        // pointer behind handle_2 may have changed after allocation of handle_1
        // The SW_alloc(lg) function searches for a contiguous space of lg bytes within one 
        // of the segments allocated by the SWAP system. If a segment contains enough free space 
        // but with gaps, the SW_alloc() function compresses the segment to have lg bytes contiguous.
        // In doing so, it (potentially) shifts the pack within the segment, and therefore 
        // ptr may change value after an SW_alloc() call.
        ptr_1 = SW_getptr(handle_1);
        ptr_2 = SW_getptr(handle_2);
        memcpy(ptr_1, ptr_2, P_len(ptr_2));
    }

    return 0;
}


/**
 *  Merges two databases : kdb1 <- kdb1 + kdb2 then deletes kdb2 (but not the IODE objects in memory). 
 *   If replace == 0, existing elements in kdb1 are not overwritten.
 *   If replace == 1, the values of existing elements are replaced.
 *  
 *  @param [in, out]    kdb1    KDB*      source and target KDB
 *  @param [in, out     kdb2    KDB*      source KDB. Delete at the end of the function
 *  @param [in]         replace int       0 to keep elements of kdb1 when duplicate objects exist
 *                                        1 to replace values in kdb1 by these in kdb2
 *  @return                     int       -1 if kdb1 or kdb2 is null or if kdb2 is empty.
 */

int K_merge_del(KDB* kdb1, KDB* kdb2, int replace)
{

    if(!kdb1)
    {
        kwarning("Cannot merge into a NULL database");
        return -1;
    }

    if(!kdb2)
    {
        kwarning("Cannot merge from a NULL database");
        return -1;
    }
    
    if(kdb2->size() == 0) 
        return -1;
    
    if(kdb1->size() == 0) 
    {
        kdb1->k_objs = kdb2->k_objs;
        kdb2->clear(false);
        kdb2 = nullptr;
        return 0;
    }

    K_merge(kdb1, kdb2, replace);
    kdb2->clear(false);
    kdb2 = nullptr;
    return 0;
}
