#pragma once
#include "utils.h"
#include "time/sample.h"
#include "kdb_global.h"


// using is the C++11 version of typedef
using Comment = std::string;
using Identity = IDT;
using List = std::string;
using Scalar = SCL;
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

protected:
    KDB* get_KDB() const
    {
        if (local_kdb) return local_kdb;
        if (K_WS[iode_type] == NULL) throw IodeExceptionFunction("Cannot get KDB of " + iode_type_name + "s",  
            "There is currently no " + iode_type_name + "s database in memory.");
        return K_WS[iode_type];
    }

public:
    KDBAbstract(EnumIodeType iode_type, const std::string& pattern, const bool shallow_copy);

    ~KDBAbstract();

    int get_iode_type() const { return iode_type; }

    int count() const { return get_KDB()->k_nb; }

    bool is_global_kdb() const { return local_kdb == nullptr; }

    bool is_shallow_copy() const { return kdb_type == KDB_SHALLOW_COPY; }

    bool is_local_kdb() const { return kdb_type == KDB_LOCAL; }

    int get_position(const std::string& name) const
    {
        check_name(name, iode_type);
        KDB* kdb = get_KDB();
        int pos = K_find(kdb, const_cast<char*>(name.c_str()));
        if (pos < 0) throw IodeExceptionFunction("Cannot get position of " + iode_type_name + " named " + name,  
            iode_type_name + " with name " + name + " does not exist.");
        return pos;
    }

    // object name

    std::string get_name(const int pos) const 
    {
        KDB* kdb = get_KDB();
        if (pos < 0 || pos > kdb->k_nb) 
            throw IodeExceptionFunction("Cannot get name of " + iode_type_name + " at position " + std::to_string(pos),  
                                        iode_type_name + " at position " + std::to_string(pos) + " does not exist.");
        std::string name_oem = std::string(kdb->k_objs[pos].o_name);
        std::string name = oem_to_utf8(name_oem);
        return name;
    }

    int set_name(const int pos, const std::string& new_name);

    int rename(const std::string& old_name, const std::string& new_name);

    bool contains(const std::string& name) { return K_find(get_KDB(), const_cast<char*>(name.c_str())) >= 0; }

    // delete

    void remove(const int pos);

    void remove(const std::string& name);

    // Other methods

    void dump(std::string& filepath);

    void clear();

    // overloaded operators

    // TODO : overload subscript operator 
    //T operator[](const char* name) { ... }

    // TODO : overload subscript operator 
    //T operator[](const int pos) { ... }
};
