#pragma once
#include "../utils.h"
#include "../sample.h"
#include "kdb_global.h"


// using is the C++11 version of typedef
using Comment = std::string;
using Identity = IDT;
using List = std::string;
using Scalar = SCL;
using Variable = std::vector<IODE_REAL>;


class KDBAbstract
{
protected:
    // Note: Cannot define a KDB* global_kdb = K_WS[type] member because the pointer contained in 
    // K_WS[type] may change in the course of the program (when loading files for example)

    EnumIodeType iode_type;
    std::string iode_type_name;
    KDB* shallow_copy_kdb;         //< local KDB returned by K_refer()

protected:
    KDB* get_KDB() const
    {
        if (shallow_copy_kdb) return shallow_copy_kdb;
        if (K_WS[iode_type] == NULL) throw std::runtime_error("There is currently no " + iode_type_name + " database in memory.");
        return K_WS[iode_type];
    }

public:
    KDBAbstract(EnumIodeType iode_type, const std::string& pattern);

    ~KDBAbstract();

    int get_iode_type() const { return iode_type; }

    int count() const { return get_KDB()->k_nb; }

    bool is_global_kdb() const { return shallow_copy_kdb == nullptr; }

    int get_position(const std::string& name) const
    {
        check_name(name, iode_type);
        KDB* kdb = get_KDB();
        int pos = K_find(kdb, const_cast<char*>(name.c_str()));
        if (pos < 0) throw std::runtime_error(iode_type_name + " with name " + name + " does not exist.");
        return pos;
    }

    // object name

    std::string get_name(const int pos) const 
    {
        KDB* kdb = get_KDB();
        if (pos < 0 || pos > kdb->k_nb) throw std::runtime_error(iode_type_name + " at position " + std::to_string(pos) + " does not exist.");
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
