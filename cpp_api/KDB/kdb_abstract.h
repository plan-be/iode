#pragma once
#include "utils/utils.h"
#include "time/sample.h"
#include "kdb_global.h"
#include <boost/functional/hash.hpp>


// using is the C++11 version of typedef
using Comment = std::string;
using List = std::string;
using Variable = std::vector<IODE_REAL>;


enum EnumIodeKDBType
{
    KDB_GLOBAL,         //< K_WS[iode_type]
    KDB_LOCAL,          //< hard copy of a global KDB (useful when working with estimation of block of eqs in the GUI)
    KDB_SHALLOW_COPY    //< shallow copy (useful when working on subset in the GUI)
};


class KDBAbstract
{
protected:
    // Note: Cannot define a KDB* global_kdb = K_WS[type] member because the pointer contained in 
    // K_WS[type] may change in the course of the program (when loading files for example)

    EnumIodeType iode_type;
    std::string iode_type_name;
    EnumIodeKDBType kdb_type;
    KDB* local_kdb;         //< either a shallow copy (K_refer()) of a subset of a global KDB or a local kdb

public:
    KDBAbstract(const EnumIodeKDBType kdb_type, const EnumIodeType iode_type, const std::string& pattern);

    ~KDBAbstract();

    int get_iode_type() const { return iode_type; }

    int count() const 
    { 
        KDB* kdb = get_KDB();
        if (kdb)
            return kdb->k_nb;
        else
            return 0;
    }

    KDB* get_KDB() const
    {
        if (local_kdb) 
            return local_kdb;
        else
            return K_WS[iode_type];
    }

    bool is_global_kdb() const { return local_kdb == NULL; }

    bool is_shallow_copy() const { return kdb_type == KDB_SHALLOW_COPY; }

    bool is_local_kdb() const { return kdb_type == KDB_LOCAL; }

    std::string get_filename() const 
    {
        KDB* kdb = get_KDB();
        if (kdb) 
            return std::string(K_get_kdb_nameptr(kdb)); 
        else
            return "";
    }

    void set_filename(const std::string& filename) 
    { 
        KDB* kdb = get_KDB();
        if (kdb) set_kdb_filename(kdb, filename); 
    }

    int get_position(const std::string& name) const
    {
        check_name(name, iode_type);
        KDB* kdb = get_KDB();
        int pos = -1;
        if (kdb) pos = K_find(kdb, to_char_array(name));
        if (pos < 0) throw IodeExceptionFunction("Cannot get position of " + iode_type_name + " named " + name,  
            iode_type_name + " with name " + name + " does not exist.");
        return pos;
    }

    // object name

    std::string get_name(const int pos) const 
    {
        KDB* kdb = get_KDB();
        if (kdb == NULL) return "";
        if (pos < 0 || pos > kdb->k_nb) 
            throw IodeExceptionFunction("Cannot get name of " + iode_type_name + " at position " + std::to_string(pos),  
                                        iode_type_name + " at position " + std::to_string(pos) + " does not exist.");
        std::string name_oem = std::string(kdb->k_objs[pos].o_name);
        std::string name = oem_to_utf8(name_oem);
        return name;
    }

    std::vector<std::string> get_names() const
    {
        std::vector<std::string> v_names;
        for (int i=0; i < count(); i++) v_names.push_back(get_name(i));
        return v_names;
    }

    std::string get_names_as_string() const
    {
        std::string names;
        for (int i=0; i < count(); i++) names += get_name(i) + ";";
        // remove last ;
        if(!names.empty()) names.pop_back();
        return names;
    }

    int set_name(const int pos, const std::string& new_name);

    int rename(const std::string& old_name, const std::string& new_name);

    bool contains(const std::string& name) 
    { 
        KDB* kdb = get_KDB();
        if (kdb)
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

    void dump(std::string& filepath);

    void clear();

    // overloaded operators

    // TODO : overload subscript operator 
    //T operator[](const char* name) { ... }

    // TODO : overload subscript operator 
    //T operator[](const int pos) { ... }
};
