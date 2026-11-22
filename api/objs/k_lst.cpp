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
 *  Creates a list from a vector of strings. The elements in the new list are separated by semi-colons.
 *  If the list contains chunck strings or more, it is split into sublists with each max chunck
 *  elements. The sublists have the same name with a numerical suffix (starting at 0) and the
 *  list 'name' then contains the references to the sublists.
 *
 *  Example:
 *      set_list("MYLIST", {"A", "B", "C", "D", "E", "F", "G", "H"}, 5);
 *      // Creates 3 lists :
 *      //     MYLIST  => "$MYLIST0;$MYLIST1;"
 *      //     MYLIST0 => "A;B;C;D;E"
 *      //     MYLIST1 => "F;G;H"
 *
 *  @param [in] name    const std::string&                  name of the resulting list
 *  @param [in] values  const std::vector<std::string>&     list elements
 *  @param [in] chunck  int                                 max number of strings per (sub-)list.
 *                                                          A value <= 0 means no limit.
 *  @throw  std::invalid_argument if the name of the list (or of a sublist) is not valid
 */
void KDBLists::set_list(const std::string& name, const std::vector<std::string>& values,
    const int chunck)
{
    const int nb = (int) values.size();
    if(nb < chunck || chunck <= 0)
    {
        this->set(name, join(values, ";"));
        return;
    }

    std::string list_of_sublists;
    int j = 0;
    for(int i = 0; i < nb; i += chunck, j++)
    {
        auto first = values.begin() + i;
        auto last = (i + chunck < nb) ? first + chunck : values.end();
        std::vector<std::string> sub_values(first, last);

        std::string sub_name = name + std::to_string(j);
        if(sub_name.size() > K_MAX_NAME)
            sub_name = sub_name.substr(0, K_MAX_NAME);
        this->set(sub_name, join(sub_values, ";"));

        list_of_sublists += "$" + sub_name + ";";
    }

    this->set(name, list_of_sublists);
}

/**
 *  Replaces recursively list names in a string. Returns a vector containing all terms in the
 *  string after replacement. The lists are searched in the present KDB of lists.
 *
 *  Example:
 *      Suppose we have the lists below in the KDB of lists:
 *          A = "$A1;$A2"
 *          A1 = "X;Y"
 *          A2 = "Z"
 *
 *      expand_lists("$A B C") => "X", "Y", "Z", "B", "C"
 *
 *  @param [in] lists   const std::string&          list of terms separated by one of the chars
 *                                                      in " ,;\t\n\r\f" and containing IODE lists
 *                                                      under the form of $LISTNAME.
 *  @return                 std::vector<std::string>    all terms in lists after replacement.
 *
 */
std::vector<std::string> KDBLists::expand_lists(const std::string& lists) const
{
    std::vector<std::string> v_expanded;

    const std::string seps = " ,;\t\n\r\f";
    std::vector<std::string> v_terms = split_multi(lists, seps);
    for(const std::string& term : v_terms)
    {
        // skip empty terms (consecutive separators)
        if(term.empty())
            continue;

        if(term[0] == '$')
        {
            std::string list_name = term.substr(1);
            if(this->contains(list_name))
            {
                // replace $LISTNAME by the (recursively expanded) content of the list
                std::shared_ptr<List> list_ptr = this->get_obj_ptr(list_name);
                std::vector<std::string> v_sub_terms = expand_lists(*list_ptr);
                v_expanded.insert(v_expanded.end(), v_sub_terms.begin(), v_sub_terms.end());
                continue;
            }
        }

        v_expanded.push_back(term);
    }

    return v_expanded;
}

bool KDBLists::binary_to_obj(const std::string& name, char* pack)
{
    size_t len = (size_t) P_get_len(pack, 0);
    char* value = new char[len];
    strncpy(value, (char*) P_get_ptr(pack, 0), len);
    std::shared_ptr<List> lst_ptr = std::make_shared<List>(value);
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
    std::string list = *list_ptr;
    char* c_list = (char*) list.c_str();

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
    // W_Print(...) functions expect OEM encoding
    std::string list_oem = this->get_list_oem(name);
    char* c_list = (char*) list_oem.c_str();
    print_definition_generic(name, c_list);
    return true;
}

void KDBLists::update_reference_db()
{
    global_ref_lst[0] = this->shared_from_this();
}

bool KDBLists::scan(const std::string& list_var, const std::string& list_scal) 
{
    std::string msg = "scan() is not implemented for database of type Lists";
    kwarning(msg.c_str());
    return false;
}
