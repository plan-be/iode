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
#include "api/report/commands/commands.h"
#include "api/report/undoc/undoc.h"


// API 
// ---

KDB& get_global_db(const int iode_type)
{
    KDB* kdb = nullptr;
    switch(iode_type)
    {
        case COMMENTS :
            kdb = global_ws_cmt.get();
            break;
        case EQUATIONS :
            kdb = global_ws_eqs.get();
            break;
        case IDENTITIES :
            kdb = global_ws_idt.get();
            break;
        case LISTS :
            kdb = global_ws_lst.get();
            break;
        case SCALARS :
            kdb = global_ws_scl.get();
            break;
        case TABLES :
            kdb = global_ws_tbl.get();
            break;
        case VARIABLES :
            kdb = global_ws_var.get();
            break;
        default :
            throw std::runtime_error("get_global_db(): invalid IODE type");
    }

    if(!kdb)
    {
        std::string error_msg = "Global " + v_iode_types[iode_type] + " database not loaded";
        throw std::runtime_error(error_msg);
    }

    return *kdb;
}

SWHDL KDB::kdb_set_packed_object(const std::string& name, char* pack)
{
    if(pack == NULL) 
    {
        std::string error_msg = "Failed to add the " + v_iode_types[this->k_type];
        error_msg += + " object '" + name + "' to the database.";
        kerror(0, error_msg.c_str());
        return 0;
    }

    // Add an entry (name) if not present yet
    bool success = this->add_entry(name);
    if(!success) 
    {
        error_manager.append_error(v_iode_types[this->k_type] + " " + name + 
                                   " cannot be created (syntax ?)");
        SW_nfree(pack);
        return 0;
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
        return 0;
    }

    // Copy allocated pack into the SCR SWAP memory
    char* ptr_swap = SW_getptr(handle);
    memcpy(ptr_swap, pack, lg);
    SW_nfree(pack);

    return handle;
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

    SWHDL handle_dest = this->get_handle(new_name);
    if(handle_dest > 0) 
        SW_free(handle_dest);
    else
    {
        bool success = this->add_entry(new_name);
        if(!success)
        {
            error_msg += "Failed to add a new entry to destination database.";
            kwarning(error_msg.c_str());
            return false;
        }
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
    this->set_handle(new_name, handle_dest);
    
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
        KDBTables* tbl_db = (KDBTables*) this;
        res = tbl_db->dde_create_table(name, NULL, nc, nl, -1);
    }

    SCR_OemToAnsi((unsigned char*) res, (unsigned char*) res);
    return(res);
}

void KDB::merge(KDB& other, const bool overwrite, const bool clear_source)
{
    char* ptr_1 = NULL; 
    char* ptr_2 = NULL;
    bool found = false;
    SWHDL handle_1 = 0;
    SWHDL handle_2 = 0;

    if(other.size() == 0) 
        return;

    for(const std::string& name : other.get_names()) 
    {
        handle_2 = other.get_handle(name);
        if(handle_2 == 0) 
            continue;

        found = this->contains(name);
        if(found)
        {
            // existing elements in this are not overwritten.
            if(!overwrite) 
                continue;
            
            // delete elements from this
            handle_1 = this->get_handle(name);
            if(handle_1 > 0)
                SW_free(handle_1);
        }
        
        ptr_2 = SW_getptr(handle_2);

        // allocate new object in this on the SCR swap
        handle_1 = SW_alloc(P_len(ptr_2));
        if(handle_1 == 0) 
            return;
        this->set_handle(name, handle_1);

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

    if(clear_source)
        other.clear(false);
}

void KDB::merge_from(KDB& from, const std::string& filename)
{
    // throw an error if the passed filepath is not valid
    IodeFileType file_type = (IodeFileType) this->k_type;
    std::string input_file = check_filepath(filename, file_type, "merge_from", true);
    
    bool success = from.load(input_file);
    if(!success)
        return;

    if(this->size() == 0) 
    {
        this->description = from.description;
        this->filepath = from.filepath;
    }

    if(this->k_type == VARIABLES) 
        KV_merge_del((KDBVariables*) this, (KDBVariables*) &from, 1);
    else 
        this->merge(from, true, true);
}

bool KDB::copy_from_file(KDB& from, const std::string& file, const std::string& objs_names, 
    std::set<std::string>& v_found)
{   
    bool success = from.load_binary(this->k_type, file);
    if(!success)
        return false;

    std::set<std::string> s_objs;
    try
    {
        s_objs = from.filter_names(objs_names);
    }
    catch(const std::exception& e)
    {
        kwarning(e.what());
        return false;
    }

    // delete objects already found in previous files
    std::vector<std::string> objs_to_copy;
    for(const std::string& name : s_objs)
    {
        if(!from.contains(name)) 
            continue;
        
        // if already found in previous files -> remove from "from"
        if(v_found.contains(name)) 
            from.remove(name);
        // object to be copied
        else
            objs_to_copy.push_back(name);
    }

    std::string msg = std::to_string(objs_to_copy.size()) + " "; 
    msg += to_lower(v_iode_types[this->k_type]) + " read from file '" + file + "'";
    kmsg(msg.c_str());

    char* ptr = NULL;
    char* pack = NULL;
    SWHDL handle_to = 0;
    for(const std::string& name : objs_to_copy)
    {
        // copy packed object from "from" to char* pack
        ptr = from.get_ptr_obj(name);
        pack = SW_nalloc(P_len(ptr));
        memcpy(pack, ptr, P_len(ptr));

        // check if the object already exists in this database
        handle_to = this->get_handle(name);
        // if yes -> delete the corresponding object
        if(handle_to > 0)
            SW_free(handle_to);
        // if not -> create a new entry
        else
        {
            success = this->add_entry(name);
            if(!success) 
            {
                SW_nfree(pack);
                std::string msg = "Cannot add entry '" + name + "' to the database";
                error_manager.append_error(msg);
                return false;
            }
        }

        // allocate memory and copy the packed object in this database
        handle_to = SW_alloc(P_len(pack));
        this->set_handle(name, handle_to);
        memcpy(SW_getptr(handle_to), pack, P_len(pack));
        SW_nfree(pack);

        v_found.insert(name);
    }
    
    return true;
}

bool KDB::copy_from(const std::vector<std::string>& input_files, const std::string& objects_names)
{
    if(input_files.size() == 0)
        throw std::runtime_error("Input file(s) name(s) is(are) empty.");
    // NOTE: objects_names.size() == 0 meaning all objects -> no error thrown

    // search in each file
    bool success;
    std::set<std::string> v_found;
    for(const std::string& file : input_files) 
    {
        success = this->copy_from_file(file, objects_names, v_found);
        if(!success)
        {
            error_manager.display_last_error();
            return false;
        }
    }

    return true;
}

/**
 *  Reads objects from a list of IODE files and adds them to an existing KDB.
 *  If more than one file contains an object, the priority is given to the first file in the list.
 */
bool KDB::copy_from(const std::string& input_files, const std::string& objects_names)
{
    if(input_files.empty())
        throw std::runtime_error("Input file(s) name(s) is(are) empty.");
    if(objects_names.empty())
        throw std::runtime_error("Object(s) name(s)/pattern is empty.");

    std::vector<std::string> files = split_multi(input_files, " ,;\t\n");
    bool success = copy_from(files, objects_names);
    return success;
}

// TODO ALD: rewrite B_DataSearchParms() in C++
std::vector<std::string> KDB::search(const std::string& pattern, const bool word, 
    const bool case_sensitive, const bool in_name, const bool in_formula, 
    const bool in_text, const std::string& list_result)
{
    std::vector<std::string> objs_list;

    int c_word  = word ? 1 : 0;
    int c_ecase = case_sensitive ? 1 : 0;
    int c_names = in_name ? 1 : 0;
    int c_forms = in_formula ? 1 : 0;
    int c_texts = in_text ? 1 : 0;
    char** c_list = B_DataSearchParms((char*) pattern.c_str(), c_word, c_ecase, 
                                      c_names, c_forms, c_texts, (int) this->k_type);

    // check if returned list of objects is not null
    if(c_list == NULL || SCR_tbl_size((unsigned char**) c_list) == 0)
        return objs_list;

    // save as IODE list (default to list _RES)
    KL_lst(to_char_array(list_result), c_list, 200);
    
    // convert C table as C++ vector
    for(int i=0; i < SCR_tbl_size((unsigned char**) c_list); i++)
        objs_list.push_back(std::string(c_list[i]));

    // free allocated memory for the returned C table
    SCR_free_tbl((unsigned char**) c_list);

    // return C++ vector
    return objs_list;
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
