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
    this->k_objs[name] = handle;

    // Copy allocated pack into the SCR SWAP memory
    char* ptr_swap = SW_getptr(handle);
    memcpy(ptr_swap, pack, lg);

    SW_nfree(pack);    
    return true;
}

// comments, equations, identities, lists, scalars, tables, objects
bool KDB::set(const std::string& name, char* value)
{
    char error[1024];
    char* pack = NULL;
    switch(this->k_type)
    {
        case COMMENTS :
            K_cpack(&pack, value);
            break;
        case EQUATIONS :
            // value = equation pointer
            K_epack(&pack, value, (char*) name.c_str());
            break;
        case IDENTITIES :
            // value = lec expression
            K_ipack(&pack, value);
            break; 
        case LISTS :
            K_lpack(&pack, value);
            break;
        case SCALARS :
            // value = scalar pointer
            K_spack(&pack, value);
            break; 
        case TABLES :
            // value = table pointer
            K_tpack(&pack, value, (char*) name.c_str());
            break;
        default :
            sprintf(error, "Invalid database type for adding a %s object", 
                           v_iode_types[this->k_type].c_str());
            kerror(0, error);
            return false;
    }

    bool success = set_packed_object(name, pack);
    return success;
}

// variables
bool KDB::set(const std::string& name, double* var, const int nb_obs)       
{
    if(this->k_type != VARIABLES)
    {
        kerror(0, "Invalid database type for adding a Variable object");
        return false;
    }

    char* pack = NULL;
    K_vpack(&pack, var, (int*) &nb_obs);
    bool success = set_packed_object(name, pack);
    return success;
}

// objects
bool KDB::set(const std::string& name, char* value, const int length)
{
    if(this->k_type != OBJECTS)
    {
        kerror(0, "Invalid database type for adding an Object");
        return false;
    }

    char* pack = NULL;
    K_opack(&pack, value, (int*) &length);
    bool success = set_packed_object(name, pack);
    return success;
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
    this->k_objs[new_name] = handle_dest;
    
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

bool KDB::load_asc(const std::string& filename)
{
    bool success = false;
    switch(this->k_type)
    {
            break;
        case EQUATIONS :
            success = load_asc_eqs(filename);
            break;
        case IDENTITIES :
            success = load_asc_idt(filename);
            break; 
        case LISTS :
            success = load_asc_lst(filename);
            break;
        case SCALARS :
            success = load_asc_scl(filename);
            break; 
        case TABLES :
            success = load_asc_tbl(filename);
            break;
        case VARIABLES :
            success = load_asc_var(filename);
            break;
        default :
            break;
    }

    return success;
}

void KDB::update_reference_db()
{
    K_RWS[this->k_type][0] = new KDB(this, "*");
}

bool KDB::save_asc(const std::string& filename)
{
    bool success = false;
    switch(this->k_type)
    {
        case EQUATIONS :
            success = save_asc_eqs(filename);
            break;
        case IDENTITIES :
            success = save_asc_idt(filename);
            break; 
        case LISTS :
            success = save_asc_lst(filename);
            break;
        case SCALARS :
            success = save_asc_scl(filename);
            break; 
        case TABLES :
            success = save_asc_tbl(filename);
            break;
        case VARIABLES :
            success = save_asc_var(filename);
            break;
        default :
            break;
    }

    return success;
}

bool KDB::grep_obj(const std::string& name, const SWHDL handle, 
    const std::string& pattern, const bool ecase, const bool forms, 
    const bool texts, const char all) const
{
    bool found = false;
    KDB* _this_ = const_cast<KDB*>(this);
    switch(this->k_type) 
    {
        case LISTS :
            if(texts) 
                found = wrap_grep_gnl(pattern, KLVAL(_this_, handle), ecase, all);
            break;
        case IDENTITIES :
            if(forms) 
                found = wrap_grep_gnl(pattern, KILEC(_this_, handle), ecase, all);
            break;
        case EQUATIONS :
        {
            std::string lec = KELEC(_this_, name);
            std::string cmt = KECMT(_this_, name);
            if(forms) 
                found = wrap_grep_gnl(pattern, lec, ecase, all);
            if(!found && texts)
                found = wrap_grep_gnl(pattern, cmt, ecase, all);
            break;
        }
        case TABLES:
        {
            Table* tbl = KTVAL(_this_, name);

            found = false;
            std::string text;
            for(const TableLine& tline : tbl->lines) 
            {
                if(found) 
                    break;
                
                switch(tline.get_type()) 
                {
                    case TABLE_LINE_SEP   :
                    case TABLE_LINE_MODE  :
                    case TABLE_LINE_DATE  :
                    case TABLE_LINE_FILES :
                        break;
                    case TABLE_LINE_TITLE :
                    {
                        TableCell cell = tline.cells[0];
                        if(texts)
                        {
                            text = cell.get_content(true);
                            found = wrap_grep_gnl(pattern, text, ecase, all);
                        } 
                        break;
                    }
                    case TABLE_LINE_CELL :
                    {
                        found = false;
                        for(const TableCell& cell : tline.cells)
                        {
                            if(found) 
                                break;
                            
                            if((texts && cell.get_type() == TABLE_CELL_STRING) || 
                               (forms && cell.get_type() == TABLE_CELL_LEC))
                               {
                                    text = cell.get_content(true);
                                    found = wrap_grep_gnl(pattern, text, ecase, all);
                               }
                        }
                        break;
                    }
                }
            }

            delete tbl;
            break;
        }
    }

    return found;
}

bool KDB::print_eqs_def(const std::string& name)
{
    Equation* eq = KEVAL(this, name);
    if(!eq) 
        return false;
    
    bool success = eq->print_definition();

    delete eq;
    eq = nullptr;
    return success;
}

bool KDB::print_idt_def(const std::string& name)
{
    char* lec = KILEC(this, name);
    if(lec == NULL) 
        return false;
    
    std::string tmp = name + " : " + std::string(lec);
    W_printf((char*) ".par1 enum_1\n");
    CLEC* clec = KICLEC(this, name);
    print_lec_definition(name, tmp, clec, B_EQS_LEC);

    return true;
}

bool KDB::print_lst_def(const std::string& name)
{
    print_definition_generic(name, KLVAL(this, name));
    return true;
}

bool KDB::print_scl_def(const std::string& name)
{
    W_printfReplEsc((char*) ".par1 enum_%d\n~b%s~B : ", 1, name.c_str());

    Scalar* scl = KSVAL(this, name);
    if(!scl) 
    {
        W_printf((char*) "?\n");
        return false;
    }

    bool success = scl->print_definition();
    return success;
}

/**
 *  Print the table in position pos in kdb.  
 *  
 *  If B_TABLE_TITLE != 0, print the table title.
 *  If B_TABLE_TITLE == 1, print the table name and its title.
 *  If B_TABLE_TITLE == 0, print the table definition.
 *  
 *  @param [in] kdb KDB*        KDB of tables
 *  @param [in] pos int         position of the table in kdb
 *  @return         int         -1 if the table cannot be found in kdb
 */
bool KDB::print_tbl_def(const std::string& name)
{
    Table* tbl = KTVAL(this, name);
    if(!tbl) 
        return false;
    
    if(B_TABLE_TITLE) 
    {
        if(B_TABLE_TITLE == 1) 
            W_printfReplEsc((char*) "\n~b%s~B : %s\n", name.c_str(), T_get_title(tbl, false));
        else 
            W_printf((char*) "\n%s\n", T_get_title(tbl, false));
        
        delete tbl;
        tbl = nullptr;
        return true;
    }

    B_PrintRtfTopic((char*) T_get_title(tbl, false));
    W_printf((char*) ".tb %d\n", T_NC(tbl));
    W_printfRepl((char*) ".sep &\n");
    W_printfRepl((char*) "&%dC%cb%s : definition%cB\n", T_NC(tbl), A2M_ESCCH, name.c_str(), A2M_ESCCH);
    bool success = tbl->print_definition();
    W_printf((char*) ".te\n");

    delete tbl;
    tbl = nullptr;
    return success;
}

bool KDB::print_var_def(const std::string& name)
{
    Sample* smpl = this->sample;
    if(!smpl || smpl->nb_periods == 0) 
    {
        std::string msg = "Cannot print the variable '" + name + "' because ";
        msg += "the variable database has no sample defined";
        kwarning((char*) msg.c_str());
        return false;
    }

    double* val = KVVAL(this, name, 0); 
    if(val == NULL) 
        return false;
    
    W_printfRepl((char*) "&1L%s ", name.c_str());
    for(int j = 0 ; j < smpl->nb_periods; j++, val++) 
    {
        W_printfRepl((char*) "&1D");
        B_PrintVal(*val);
    }

    W_printf((char*) "\n");
    return true;
}

bool KDB::print_obj_def(const std::string& name)
{
    switch(this->k_type) 
    {
        case EQUATIONS :
            return print_eqs_def(name);
        case IDENTITIES :
            return print_idt_def(name);
        case LISTS :
            return print_lst_def(name);
        case SCALARS :
            return print_scl_def(name);
        case TABLES :
            return print_tbl_def(name);
        case VARIABLES :
            return print_var_def(name);
        default :
            kerror(0, "Invalid database type for printing object definition");
            return false;
    }

    return true;
}

char* KDB::dde_create_obj_by_name(const std::string& name, int* nc, int* nl)
{
    char* obj;
    switch(this->k_type) 
    {
        case EQUATIONS :
        {
            std::string lec = KELEC(this, name);
            obj = (char*) lec.c_str();
            break;
        }
        case IDENTITIES :
            obj = (char*) KILEC(this, name);
            break;
        case LISTS :
            obj = (char*) KLVAL(this, name);
            break;
        default:
            obj = (char*) SCR_stracpy((unsigned char*) "Not yet implemented") ;
            break;
    }

    return obj;
}

/**
 *  Compute a table on the GSample ismpl and return a string containing the result.
 */
char* KDB::dde_create_table(const std::string& name, char *ismpl, int *nc, int *nl, int nbdec)
{
    int     dim, i, j, d, rc = 0, nli = 0,
                          nf = 0, nm = 0;
    char    gsmpl[128], **l = NULL, *buf, *res = NULL;
    COLS    *cls;

    Table* tbl = KTVAL(global_ws_tbl.get(), name);
    Sample* smpl = global_ws_var->sample;

    /* date */
    char date[11];

    if(smpl == nullptr || smpl->nb_periods == 0) 
        return (char*) "";

    /* mode */
    if(!ismpl)
        sprintf(gsmpl, "%s:%d", (char*) smpl->start_period.to_string().c_str(), smpl->nb_periods);
    else
        sprintf(gsmpl, "%s", ismpl);

    dim = T_prep_cls(tbl, gsmpl, &cls);
    if(dim < 0) 
        return((char*) SCR_stracpy((unsigned char*) "Error in Tbl or Smpl"));

    KT_names = T_find_files(cls);
    KT_nbnames = SCR_tbl_size((unsigned char**) KT_names);
    if(KT_nbnames == 0) 
        return((char*) SCR_stracpy((unsigned char*) "Error in Tbl or Smpl"));
    COL_find_mode(cls, KT_mode, 2);

    *nc = dim + 1;
    *nl = 1;

    TableLine* line;
    buf = SCR_malloc(256 + dim * 128);
    for(i = 0; rc == 0 && i < T_NL(tbl); i++) 
    {
        buf[0] = 0;
        line = &tbl->lines[i];

        switch(line->get_type()) 
        {
            case TABLE_LINE_SEP   :
                break;
            case TABLE_LINE_DATE  :
                strcat(buf, SCR_long_to_fdate(SCR_current_date(), date, (char*) "dd/mm/yy"));
                break;
            case TABLE_LINE_MODE  :
                for(j = 0; j < MAX_MODE; j++) 
                {
                    if(KT_mode[j] == 0) 
                        continue;
                    sprintf(date, "(%s) ", COL_OPERS[j + 1]);
                    strcat(buf, date);
                    strcat(buf, KLG_OPERS_TEXTS[j + 1][K_LANG]);
                    strcat(buf, "\n");
                    nm ++;
                }
                break;
            case TABLE_LINE_FILES :
                for(j = 0; KT_names[j]; j++) 
                {
                    strcat(buf, KT_names[j]);
                    strcat(buf, "\n");
                    nf ++;
                }
                break;
            case TABLE_LINE_TITLE :
                strcat(buf, IodeTblCell(&(line->cells[0]), NULL, nbdec));
                //strcat(buf,"\x01\x02\03"); // JMP 13/7/2022
                break;
            case TABLE_LINE_CELL  :
                COL_clear(cls);
                if(COL_exec(tbl, i, cls) < 0)
                    strcat(buf, "Error in calc");
                else
                    for(j = 0; j < cls->cl_nb; j++) 
                    {
                        d = j % T_NC(tbl);
                        if(tbl->repeat_columns == 0 && d == 0 && j != 0) 
                            continue;
                        strcat(buf, IodeTblCell(&(line->cells[d]), cls->cl_cols + j, nbdec));
                        strcat(buf, "\t");
                    }
                break;
        }

        if(buf[0]) 
        {
            SCR_add_ptr((unsigned char***) &l, &nli, (unsigned char*) buf);
            (*nl)++;
        }
    }
    
    SCR_add_ptr((unsigned char***) &l, &nli, NULL);
    *nl += nf + nm;
    res = (char*) SCR_mtov((unsigned char**) l, '\n');

    COL_free_cols(cls);
    SCR_free_tbl((unsigned char**) l);
    SCR_free(buf);

    SCR_free_tbl((unsigned char**) KT_names);
    KT_names = NULL;
    KT_nbnames = 0;

    return(res);
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
        res = this->dde_create_table(name, NULL, nc, nl, -1);

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
    
    for(auto& [name, handle_2] : kdb2->k_objs) 
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
