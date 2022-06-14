#pragma once
#include "kdb_abstract.h"

// see https://www.codeproject.com/Articles/48575/How-to-Define-a-Template-Class-in-a-h-File-and-Imp 
// for more details


template<class T>
KDBAbstract<T>::KDBAbstract(const EnumIodeType type, const std::string& pattern) : type(type)
{
    if (K_WS[type] == NULL) IodeInit();
    cpp_assign_super_API();

    if (pattern.empty()) shallow_copy_kdb = NULL;
    else
    {
        char** c_names = filter_kdb_names(type, pattern);
        shallow_copy_kdb = K_quick_refer(K_WS[type], c_names);
        SW_nfree(c_names);
        if (shallow_copy_kdb == NULL) throw std::runtime_error("Cannot filter " + vIodeTypes[type] + " with pattern " + pattern);
    }
}

template<class T>
KDBAbstract<T>::~KDBAbstract()
{
    if (shallow_copy_kdb != NULL) K_free_kdb(shallow_copy_kdb);
}

// object name

template<class T>
std::string KDBAbstract<T>::get_name(const int pos) const
{
    KOBJ obj = get_iode_object(pos);
    std::string name_oem = std::string(obj.o_name);
    std::string name = oem_to_utf8(name_oem);
    return name;
}

template<class T>
int KDBAbstract<T>::set_name(const int pos, const std::string& new_name)
{
    std::string old_name = get_name(pos);
    int new_pos = rename(old_name, new_name);
    return new_pos;
}

template<class T>
int KDBAbstract<T>::rename(const std::string& old_name, const std::string& new_name)
{
    if (type == I_EQUATIONS) throw std::runtime_error("Cannot rename an equation. The name of an equation is always its endogeneous variable");

    check_name(new_name, type);

    char* c_old_name = const_cast<char*>(old_name.c_str());
    char* c_new_name = const_cast<char*>(new_name.c_str());
    
    // first rename in global KDB
    int pos = K_ren(K_WS[type], c_old_name, c_new_name);
    // see K_ren documentation
    if (pos < 0)
    {
        std::string msg = vIodeTypes[type] + " cannot be renamed as " + new_name + ".\n";
        if (pos == -1) throw std::runtime_error(msg + "Name " + old_name + " does not exist.");
        else if (pos == -2) throw std::runtime_error(msg + vIodeTypes[type] + " with name " + new_name + " already exists.");
        else throw std::runtime_error(msg + "Something went wrong.");
    }
    
    // rename in shallow copy KDB
    if (shallow_copy_kdb != NULL) pos = K_ren(shallow_copy_kdb, c_old_name, c_new_name);

    return pos;
}

// CRUD (Create - Read - Update - Delete) + Copy methods

template<class T>
void KDBAbstract<T>::add(const std::string& name, const T& obj)
{
    check_name(name, type);
    
    // throw exception if object with passed name already exist
    char* c_name = const_cast<char*>(name.c_str());
    if (K_find(get_KDB(), c_name) >= 0) 
        throw std::runtime_error(vIodeTypes[type] + " with name " + name + " already exists. Use update() method instead.");
    
    if (shallow_copy_kdb != NULL)
    {
        // add new obj to global KDB if doesn't exist yet
        int pos_global = K_find(K_WS[type], c_name);
        if (pos_global < 0) pos_global = add_or_update(name, obj);
        // add new entry to shallow copy KDB
        int pos_copy = K_add_entry(shallow_copy_kdb, c_name);
        KSOVAL(shallow_copy_kdb, pos_copy) = KSOVAL(K_WS[type], pos_global);
    }
    else add_or_update(name, obj);
}

template<class T>
T KDBAbstract<T>::copy(const int pos) const
{
    // throw exception if object with passed position is not valid
    T obj = get(pos);
    return copy_obj(obj);
}

template<class T>
T KDBAbstract<T>::copy(const std::string& name) const
{   
    // throw exception if object with passed name does not exist
    T obj = get(name);
    return copy_obj(obj);
}

template<class T>
T KDBAbstract<T>::get(const int pos) const
{
    // throw exception if object with passed position is not valid
    get_name(pos);
    return get_unchecked(pos);
}

template<class T>
T KDBAbstract<T>::get(const std::string& name) const
{
    // throw exception if object with passed name does not exist
    int pos = get_position(name);
    return get_unchecked(pos);
}

template<class T>
void KDBAbstract<T>::update(const std::string& name, const T& obj)
{
    // throw exception if object with passed name does not exist
    get_position(name);
    add_or_update(name, obj);
}

template<class T>
void KDBAbstract<T>::update(const int pos, const T& obj)
{
    // throw exception if object with passed position is not valid
    std::string name = get_name(pos);
    add_or_update(name, obj);
}

template<class T>
void KDBAbstract<T>::remove(const int pos)
{
    // throw exception if object with passed position is not valid
    std::string name = get_name(pos);
    remove(name);
}

template<class T>
void KDBAbstract<T>::remove(const std::string& name)
{
    // throw exception if object with passed name does not exist
    int pos = get_position(name);

    if (shallow_copy_kdb != NULL)
    {
        K_del_entry(shallow_copy_kdb, pos);
        char* c_name = const_cast<char*>(name.c_str());
        pos = K_find(K_WS[type], c_name);
    }
    K_del(K_WS[type], pos);
}

// Other methods

template<class T>
void KDBAbstract<T>::dump(std::string& filepath)
{
    filepath = check_filepath(filepath, type, "save", false);
    char* c_filepath = const_cast<char*>(filepath.c_str());
    if (strlen(c_filepath) >= sizeof(FNAME)) throw std::runtime_error(std::string(B_msg(256)));   /* File name too long */

    int res = B_WsDump(get_KDB(), c_filepath);
    if (res != EXIT_SUCCESS)
        throw std::runtime_error("Something went wrong when trying to save " + vIodeTypes[type] + " to file " + filepath);
}

template<class T>
void KDBAbstract<T>::clear()
{
    int res;
    res = B_WsDescr("", type);
    res += B_WsName("", type);

    res += K_clear(get_KDB());
    if (res != EXIT_SUCCESS)
        throw std::runtime_error("Something went wrong when trying to clear objets of type " + vIodeTypes[type]);
}
