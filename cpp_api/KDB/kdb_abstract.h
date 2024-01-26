#pragma once
#include "utils/utils.h"
#include "time/sample.h"
#include "kdb_global.h"

#include <string>
#include <vector>
#include <stdexcept>

#include <boost/functional/hash.hpp>
#include <boost/algorithm/string.hpp>


// using is the C++11 version of typedef
using Comment = std::string;
using List = std::string;
using Variable = std::vector<IODE_REAL>;


const static char DB_GLOBAL       = (char) 0;       //< K_WS[iode_type]
const static char DB_DEEP_COPY    = (char) 1;       //< deep copy of a global KDB (useful when working with estimation of block of eqs in the GUI)
const static char DB_SHALLOW_COPY = (char) 2;       //< shallow copy (useful when working on subset in the GUI)


/**
 * @note We use k_reserve[0] to store if the current database refers to a global database or 
 *       if it is a deep or a shallow copy of a global database  
 * 
 */
class KDBAbstract: public KDB
{
private:
    /**
     * @brief Create a list of IODE object names extracted from the corresponding global database given a pattern.
     * 
     * @param pattern 
     * @return std::vector<std::string>
     */
    static std::vector<std::string> filter_names_from_global_db(const short db_type, const std::string& pattern);

protected:
    KDBAbstract(KDBAbstract* kdb, const bool deep_copy, const std::string& pattern);

public:
    KDBAbstract(const EnumIodeType iode_type, const std::string& filepath = "");
    
    // No copy constructor --> replaced by subset() method in derived classes
    KDBAbstract(const KDBAbstract&) = delete;

    ~KDBAbstract();

    int get_iode_type() const { return k_type; }

    bool is_global_database() const { return k_reserved[0] == DB_GLOBAL; }

    bool is_local_database() const { return k_reserved[0] == DB_DEEP_COPY; }

    bool is_shallow_copy_database() const { return k_reserved[0] == DB_SHALLOW_COPY; }

    KDB* get_database() const
    {
        KDB* kdb = is_global_database() ? K_WS[k_type] : const_cast<KDBAbstract*>(this);
        if(kdb == NULL)
            kwarning(("The current " + vIodeTypes[k_type] + " has not been set yet").c_str());
        return kdb;
    } 

    int count() const 
    { 
        KDB* kdb = get_database();
        if(kdb)
            return kdb->k_nb;
        else
            return 0;
    }

    std::string get_filename() const 
    {
        KDB* kdb = get_database();
        if(kdb != NULL && kdb->k_nameptr != NULL) 
            return std::string(kdb->k_nameptr); 
        else
            return "";
    }

    void set_filename(const std::string& filename) 
    { 
        KDB* kdb = get_database();
        if(kdb != NULL)
            set_kdb_filename(kdb, filename); 
    }

    std::string get_description() const 
    { 
        KDB* kdb = get_database();
        if(kdb != NULL && kdb->k_desc != NULL) 
            return std::string(kdb->k_desc);
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

        size_t size = description.size() + 1;
        if(size > K_MAX_DESC) 
            size = (size_t) K_MAX_DESC;
        strncpy(kdb->k_desc, description.c_str(), size);
    }

    int get_position(const std::string& name) const
    {
        check_name(name, k_type);

        KDB* kdb = get_database();
        int pos = -1;
        if(kdb != NULL) 
            pos = K_find(kdb, to_char_array(name));
        if(pos < 0) 
            throw std::invalid_argument("Cannot get the position of the object named '" + name + 
                                        "' in the database of '" + vIodeTypes[k_type] + "'.\n" +  
                                        "The object with name '" + name + "' does not exist.");
        return pos;
    }

    // object name

    std::string get_name(const int pos) const 
    {
        KDB* kdb = get_database();
        if(kdb == NULL) 
            return "";
        if(pos < 0 || pos >= kdb->k_nb) 
            throw std::invalid_argument("Cannot get the name of the object at position " + std::to_string(pos) + ".\n" +  
                                        "The position must be in the range [0, " + std::to_string(kdb->k_nb - 1) + "].");
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
            return K_find(kdb, to_char_array(name)) >= 0; 
        else
            return false;
    }

    // delete

    void remove(const int pos);

    void remove(const std::string& name);

    // Other methods

    void merge(const KDBAbstract& other, const bool overwrite=true);

    void copy_into(const std::string& input_file, const std::string objects_names = "");

    /**
     *  Syntax: $WsMerge<type> filename
     *  where type is in {Cmt, Eqs, Idt, Lst, Scl, Tbl, Var}
     *  
     *  @see B_WsLoad() for the syntax of that group of functions.
     *  
     *  @see https://iode.plan.be/doku.php?id=wsmerge
     */
    void merge_into(const std::string& input_file);

    std::vector<std::string> get_associated_objects_list(const std::string& name, const EnumIodeType other_type);

    // save - clear

    void save(const std::string& filepath);

    void clear();

    // overloaded operators

    // TODO : overload subscript operator 
    //T operator[](const char* name) { ... }

    // TODO : overload subscript operator 
    //T operator[](const int pos) { ... }
};
