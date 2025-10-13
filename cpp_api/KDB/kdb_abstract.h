#pragma once
#include "api/time/sample.h"
#include "api/objs/kdb.h"
#include "api/objs/comments.h"
#include "api/objs/equations.h"
#include "api/objs/identities.h"
#include "api/objs/lists.h"
#include "api/objs/scalars.h"
#include "api/objs/tables.h"
#include "api/objs/variables.h"
#include "cpp_api/common.h"
#include "kdb_global.h"

#include <string>
#include <vector>
#include <stdexcept>


// using is the C++11 version of typedef
using Comment = std::string;
using List = std::string;
using Variable = std::vector<double>;


class KDBAbstract: public KDB
{
protected:
    KDBAbstract(KDBAbstract* kdb, const bool deep_copy, const std::string& pattern);

public:
    KDBAbstract(const IodeType iode_type, const std::string& filepath = "");
    
    // No copy constructor --> replaced by subset() method in derived classes
    KDBAbstract(const KDBAbstract&) = delete;

    int get_iode_type() const { return k_type; }

    bool is_global_database() const { return k_db_type == DB_GLOBAL; }

    bool is_local_database() const { return k_db_type == DB_STANDALONE; }

    bool is_shallow_copy_database() const { return k_db_type == DB_SHALLOW_COPY; }

    KDB* get_database() const
    {
        KDB* kdb = is_global_database() ? K_WS[k_type] : const_cast<KDBAbstract*>(this);
        if(kdb == NULL)
            kwarning(("The current " + v_iode_types[k_type] + " has not been set yet").c_str());
        return kdb;
    } 

    int size() const 
    { 
        KDB* kdb = get_database();
        if(kdb)
            return kdb->size();
        else
            return 0;
    }

    std::string get_filename() const 
    {
        KDB* kdb = get_database();
        if(kdb != NULL && !kdb->filepath.empty()) 
        {
            std::string filename = oem_to_utf8(kdb->filepath);
            return filename;
        }
        else
            return "";
    }

    void set_filename(const std::string& filename) 
    { 
        KDB* kdb = get_database();
        if(kdb != NULL)
        {
            std::string filename_oem = utf8_to_oem(filename);
            set_kdb_filename(kdb, filename_oem); 
        }
    }

    std::string get_description() const 
    { 
        KDB* kdb = get_database();
        if(kdb != NULL && !kdb->description.empty())
        {
            std::string desc = oem_to_utf8(kdb->description);
            return desc;
        }
        else 
            return "";
    }

    /**
     *  Syntax: $WsDescr<type> free text
     *  where type is in {Cmt, Eqs, Idt, Lst, Scl, Tbl, Var}
     *  
     *  @see B_WsLoad() for the syntax of that group of functions.
     *  
     *  @see https://iode.plan.be/doku.php?id=wsdescr
     */
    void set_description(const std::string& description)
    {
        KDB* kdb = get_database();
        if (kdb == NULL) 
            return ;
        kdb->description = utf8_to_oem(description);
    }

    int find(const std::string& name) const
    {
        check_name(name, k_type);

        KDB* kdb = get_database();
        int pos = -1;
        if(kdb != NULL) 
            pos = kdb->find(to_char_array(name));
        if(pos < 0) 
            throw std::invalid_argument("Cannot get the position of the object named '" + name + 
                                        "' in the database of '" + v_iode_types[k_type] + "'.\n" +  
                                        "The object with name '" + name + "' does not exist.");
        return pos;
    }

    // object name

    std::string get_name(const int pos) const 
    {
        KDB* kdb = get_database();
        if(kdb == NULL) 
            return "";
        if(pos < 0 || pos >= kdb->size()) 
            throw std::invalid_argument("Cannot get the name of the object at position " + std::to_string(pos) + ".\n" +  
                                        "The position must be in the range [0, " + std::to_string(kdb->size() - 1) + "].");
        std::string name_oem = std::string(kdb->k_objs[pos].o_name);
        std::string name = oem_to_utf8(name_oem);
        return name;
    }

    std::vector<std::string> get_names(const std::string& pattern = "", const bool must_exist = true) const;

    std::string get_names_as_string(const std::string& pattern = "", const bool must_exist = true) const;

    int set_name(const int pos, const std::string& new_name);

    int rename(const std::string& old_name, const std::string& new_name);

    bool contains(const std::string& name) const
    { 
        KDB* kdb = get_database();
        if(kdb != NULL)
            return kdb->find(to_char_array(name)) >= 0; 
        else
            return false;
    }

    // delete

    void remove(const int pos);

    void remove(const std::string& name);

    void remove_entry(const int pos);

    void remove_entry(const std::string& name);

    // Other methods

    void merge(const KDBAbstract& other, const bool overwrite=true);

    void copy_from(const std::string& input_file, const std::string objects_names = "*");

    /**
     *  Syntax: $WsMerge<type> filename
     *  where type is in {Cmt, Eqs, Idt, Lst, Scl, Tbl, Var}
     *  
     *  @see B_WsLoad() for the syntax of that group of functions.
     *  
     *  @see https://iode.plan.be/doku.php?id=wsmerge
     */
    void merge_from(const std::string& input_file);

    /**
     *  Searches all objects containing a given string in their names and/or definitions.
     *  
     *  Syntax
     *  ------
     *      $DataSearch<type> mask word ecase in_name in_formula in_text list_result
     *        (word, ecase, in_name, in_formula, in_text := 0 ou 1)
     *        (mask := suite of chars or ?, *)
     *        (list_result := name of the resulting list
     *  
     *  @see https://iode.plan.be/doku.php?id=datasearch for details
     */
    std::vector<std::string> search(const std::string& pattern, const bool word=true, const bool case_sensitive=true, 
        const bool in_name=true, const bool in_formula=true, const bool in_text=true, const std::string& list_result="_RES");

    // save - clear

    void save(const std::string& filepath, const bool compress=false);

    void clear();

    // overloaded operators

    // TODO : overload subscript operator 
    //T operator[](const char* name) { ... }

    // TODO : overload subscript operator 
    //T operator[](const int pos) { ... }
};
