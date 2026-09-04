#include "api/b_errors.h"
#include "api/lec/lec.h"
#include "api/objs/kdb.h"
#include "api/objs/objs.h"
#include "api/objs/pack.h"
#include "api/objs/grep.h"
#include "api/objs/equations.h"
#include "api/objs/identities.h"
#include "api/objs/lists.h"
#include "api/objs/scalars.h"
#include "api/objs/tables.h"


/**
 *  Creates a list from a table of strings. The elements in the new list are separated by semi-colons.
 *  If the list exceeds chunck strings, it is split into sublists with each max chunck elements. The
 *  sublists have the same name with a numerical suffix.
 *  
 *  Example: 
 *      char** tbl;
 *      tbl = SCR_vtom("A,B,C,D,E,F,G,H", ',');
 *      KL_lst("MYLIST", tbl, 5); // Creates 3 lists : 
 *                                //     MYLIST  => "$MYLIST1;$MYLIST2"
 *                                //     MYLIST1 => "A,B,C,D,E"
 *                                //     MYLIST2 => "F,G,H"
 *    
 *  @param [in] char*   name    name or the resulting list
 *  @param [in] char**  lst     table of strings
 *  @param [in] int     chunck  max number of strings per (sub-)list
 *  @return     int             0 on success, < 0 on, error (rc from add())
 *  
 */
int KL_lst(char* name, char** lst, int chunck)
{
    int     rc = 0, i, j, nb;
    char    *str, *ptr, buf[30];

    nb = SCR_tbl_size((unsigned char**) lst);
    if(nb == 0) 
    {
        try
        {
            global_ws_lst->set(name, List(""));    
        }
        catch(const std::exception& e)
        {
            kwarning(e.what());
            rc = -1;
        }
        goto done;
    }

    if(nb < chunck || chunck < 0) 
    {
        str = (char*) SCR_mtov((unsigned char**) lst, (int) ';');
        try
        {
            global_ws_lst->set(name, List(str)); 
        }
        catch(const std::exception& e)
        {
            kwarning(e.what());
            rc = -1;
        }
        SCR_free(str);
        return rc;
    }

    for(i = 0, j = 0; i < nb && !rc; i+= chunck, j++) 
    {
        if(i + chunck < nb) 
        {
            ptr = lst[i + chunck];
            lst[i + chunck] = NULL;
        }

        str = (char*) SCR_mtov((unsigned char**) lst + i, ';');
        sprintf(buf, "%s%d", name, j);
        buf[K_MAX_NAME] = 0;
        try
        {
            global_ws_lst->set(buf, List(str));
        }
        catch(const std::exception& e)
        {
            kwarning(e.what());
            rc = -1;
        }
        SCR_free(str);

        if(i + chunck < nb)  
            lst[i + chunck] = ptr;
    }
    if(rc < 0) 
        goto done;

    str = (char *) SW_nalloc(j * (K_MAX_NAME + 2));
    str[0] = 0;
    for(i = 0; i < j; i++) 
    {
        sprintf(buf, "$%s%d;", name, i); /* GB 23/01/98 */
        buf[K_MAX_NAME] = 0;
        strcat(str, buf);
    }

    try
    {
        global_ws_lst->set(name, List(str));
    }
    catch(const std::exception& e)
    {
        kwarning(e.what());
        rc = -1;
    }

    SW_nfree(str);

done:
    return rc;
}


/**
 *  Replaces recursively list names in a string. Returns a table containing all terms in the string after replacement.
 *  The lists are searched in the current LST KDB.
 *  
 *  Example:
 *      Suppose we have the lists below in the current KDB of lists:
 *          A = "$A1;$A2"
 *          A1 = "X;Y"
 *          A2 = "Z"
 *  
 *      KL_expand("$A B C") => "X", "Y", "Z", "B", "C"
 *  
 *  @param [in] char*   str     list of terms separated by one of the chars in " ,;\t\n\r\f" and 
 *                              containing IODE lists under the form of $LISTNAME.
 *  @return     char**          table containing all terms in str after replacement.
 *  
 */
unsigned char **KL_expand(char *str)
{
    unsigned char 	**tbl, **tbl2;
    unsigned char*  seps = (unsigned char*) " ,;\t\n\r\f";
    int				i, nb, nb2;

    tbl = SCR_vtoms((unsigned char*) str, seps);
    nb = SCR_tbl_size(tbl);
    if(SCR_tbl_size(tbl) == 0) 
        return(tbl);
    
    std::string list_name;
    std::shared_ptr<List> list_ptr;
    char* c_list = NULL;
    for(i = 0 ; tbl[i] ; i++) 
    {
        if(tbl[i][0] == '$') 
        {
            list_name = std::string((char*) tbl[i] + 1);
            if(global_ws_lst->contains(list_name))
            {
                SCR_free(tbl[i]); // plus besoin car remplacé par sa valeur
                list_ptr = global_ws_lst->get_obj_ptr(list_name);
                c_list = (char*) list_ptr->c_str();
                tbl2 = KL_expand(c_list);
                nb2 = SCR_tbl_size(tbl2);
                // Insertion dans tbl de la liste à la place de tbl[i]
                tbl = (unsigned char **) SCR_realloc(tbl, sizeof(char *), nb + 1, (nb + 1 - 1) + nb2);
                // décaler ceux qui suivent i (nb -i - 1 éléments) de nb2 places vers la droite
                memcpy(tbl + i + nb2, tbl + i + 1, sizeof(char *) * (nb - i - 1));
                // recopier tbl2 à la place de i et suivants
                memcpy(tbl + i, tbl2, sizeof(char *) * nb2);
                nb += nb2 - 1; // -1 car on remplace le courant
                SCR_free(tbl2); // pas tbl_free car ptrs copiés dans tbl
            }
        }
    }
    return(tbl);
}

bool KDBLists::binary_to_obj(const std::string& name, char* pack)
{
    size_t len = (size_t) P_get_len(pack, 0);
    char* value = new char[len];
    strncpy(value, (char*) P_get_ptr(pack, 0), len);
    // NOTE: in binary files, the value is in OEM encoding, we need to convert it from OEM  
    //       to UTF-8 after reading the List object (IODE lists may contain non-ASCII characters  
    //       in comments written as /* ... */)
    std::string list_oem(value);
    std::string list_utf8 = oem_to_utf8(list_oem);
    std::shared_ptr<List> lst_ptr = std::make_shared<List>(list_utf8);
    this->add_obj_ptr(name, lst_ptr);
    return true;
}

/**
 * Serializes a list object. 
 *
 * @param [out] pack    (char **)   placeholder for the pointer to the serialized object
 * @param [in]  name    string      list name
 * @return                          true if the serialization succeeded, false otherwise 
 */
bool KDBLists::obj_to_binary(char** pack, const std::string& name)
{
    std::shared_ptr<List> list_ptr = this->get_obj_ptr(name);
    // NOTE: in binary files, the value is in OEM encoding, we need to convert it from UTF-8 
    //       to OEM before writing the List object (IODE lists may contain non-ASCII characters in  
    //       comments written as /* ... */)
    std::string list_utf8 = *list_ptr;
    std::string list_oem = utf8_to_oem(list_utf8);
    char* c_list = (char*) list_oem.c_str();

    *pack = (char*) P_create();
    *pack = (char*) P_add(*pack, c_list, (int) strlen(c_list) + 1);
    return true;
}

bool KDBLists::grep_obj(const std::string& name, const std::string& pattern, 
    const bool ecase, const bool forms, const bool texts, const char all) const
{
    bool found = false;
    if(texts)
    {
        std::shared_ptr<List> list = this->get_obj_ptr(name);
        found = wrap_grep_gnl(pattern, *list, ecase, all);
    }
    return found;
}

char* KDBLists::dde_create_obj_by_name(const std::string& name, int* nc, int* nl)
{
    std::shared_ptr<List> list = this->get_obj_ptr(name);
    char* obj = (char*) list->c_str();
    return obj;
}

bool KDBLists::print_obj_def(const std::string& name)
{
    std::shared_ptr<List> list_ptr = this->get_obj_ptr(name);
    // W_Print(...) functions expect OEM encoding, so convert value from UTF-8 to OEM before printing 
    std::string list_oem = utf8_to_oem(*list_ptr);
    char* c_list = (char*) list_oem.c_str();
    print_definition_generic(name, c_list);
    return true;
}

void KDBLists::update_reference_db()
{
    global_ref_lst[0] = this->get_subset("*", false);
}

bool KDBLists::scan(const std::string& list_var, const std::string& list_scal) 
{
    std::string msg = "scan() is not implemented for database of type Lists";
    kwarning(msg.c_str());
    return false;
}
