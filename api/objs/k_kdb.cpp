/**
 * @header4iode
 *
 * KDB management functions
 * ------------------------
 *
 * KDB structs contain the IODE objects of the same type plus some technical information like the 
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
    return res;
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
