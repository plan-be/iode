#pragma once
#include "../utils.h"


// using is the C++11 version of typedef
using Comment = std::string;
using Identity = IDT;
using List = std::string;
using Scalar = SCL;
using Variable = std::vector<IODE_REAL>;


template<class T> 
class KDBAbstract
{
protected:
    EnumIodeType type;
    std::string type_name;

protected:
    // Cannot define a KDB *kdb member set to K_WS[type] in the constructor because the pointer contained in 
    // K_WS[type] may change in the course of the program (when loading files for example)
    KDB* get_KDB() const
    {
        KDB* kdb = K_WS[type];
        if (kdb == NULL) throw std::runtime_error("There is currently no " + type_name + " database in memory.");
        return kdb;
    }

    KOBJ get_iode_object(const int pos) const
    {
        KDB* kdb = get_KDB();
        int nb_objs = count();
        if (pos < 0 || pos > nb_objs) throw std::runtime_error(type_name + " at position " + std::to_string(pos) + " does not exist.");
        return kdb->k_objs[pos];
    }

    int get_position(const std::string& name) const
    {
        KDB* kdb = get_KDB();
        check_name(name, type);
        int pos = K_find(kdb, const_cast<char*>(name.c_str()));
        if (pos < 0) throw std::runtime_error(type_name + " with name " + name + " does not exist.");
        return pos;
    }

    // CRUD (Create - Read - Update - Delete) + Copy methods

    virtual void add_or_update(const std::string& name, const T& obj) = 0;

    virtual T copy_obj(const T& original) const = 0;

    virtual T get_unchecked(const int pos) const = 0;

public:
    KDBAbstract(EnumIodeType type);

    int get_iode_type() const { return type; }

    int count() const { return K_WS[type] != NULL ? K_WS[type]->k_nb : 0; }

    // object name

    std::string get_name(const int pos) const;

    int set_name(const int pos, const std::string& new_name);

    int rename(const std::string& old_name, const std::string& new_name);

    // CRUD (Create - Read - Update - Delete) + Copy methods

    void add(const std::string& name, const T& obj);

    T copy(const int pos) const;

    T copy(const std::string& name) const;

    T get(const int pos) const;

    T get(const std::string& name) const;

    void update(const int pos, const T& obj);

    void update(const std::string& name, const T& obj);

    void remove(const int pos);

    void remove(const std::string& name);

    // Load - Save - Clear methods

    void load(std::string& filepath);

    void save(std::string& filepath);

    void clear();

    // overloaded operators

    // TODO : overload subscript operator 
    //T operator[](const char* name) { ... }

    // TODO : overload subscript operator 
    //T operator[](const int pos) { ... }
};
