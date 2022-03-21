#pragma once
#include "kdb_abstract.h"

// see https://www.codeproject.com/Articles/48575/How-to-Define-a-Template-Class-in-a-h-File-and-Imp 
// for more details


template<class T>
KDBAbstract<T>::KDBAbstract(EnumIodeType type) : type(type), type_name(vIodeTypes[type])
{
    if (K_WS[type] == NULL) IodeInit();
}

// object name

template<class T>
std::string KDBAbstract<T>::get_name(const int pos) const
{
    KOBJ obj = get_iode_object(pos);
    std::string name_oem = std::string(obj.o_name);
    std::string name = IodeString(name_oem, CP_OEMCP).to_utf8();
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
    if (new_name.size() > K_MAX_NAME) throw std::runtime_error("Iode names cannot exceed " + std::to_string(K_MAX_NAME) + " characters." + new_name + " : " + std::to_string(new_name.size()));
    // TODO : check name follow IODE standard (upper case vs lower case)
    KDB* kdb = get_KDB();
    char* c_old_name = const_cast<char*>(old_name.c_str());
    char* c_new_name = const_cast<char*>(new_name.c_str());
    int pos = K_ren(kdb, c_old_name, c_new_name);
    // see K_ren documentation
    if (pos < 0)
    {
        std::string msg = type_name + " cannot be renamed as " + new_name + ".";
        if (pos == -1) throw std::runtime_error("Name " + old_name + " does not exist.\n" + msg);
        else if (pos == -2) throw std::runtime_error(type_name + " with name " + new_name + " already exists.\n" + msg);
        else throw std::runtime_error("Something went wrong.\n" + msg);
    }
    return pos;
}

// CRUD (Create - Read - Update - Delete) + Copy methods

template<class T>
void KDBAbstract<T>::add(const std::string& name, const T& obj)
{
    // TODO : check name follow IODE standard (upper case vs lower case) + name is not too long
    // throw exception if object with passed name already exist
    char* c_name = const_cast<char*>(name.c_str());
    if (K_find(get_KDB(), c_name) >= 0) throw std::runtime_error(type_name + " with name " + name + " already exists. Use update() method instead.");
    add_or_update(name, obj);
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
    // TODO : check name follow IODE standard (upper case vs lower case) + name is not too long
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
    K_del(get_KDB(), pos);
}


template<class T>
void KDBAbstract<T>::remove(const std::string& name)
{
    // throw exception if object with passed name does not exist
    int pos = get_position(name);
    K_del(get_KDB(), pos);
}

// Load - Save - Clear methods

template<class T>
void KDBAbstract<T>::load(std::string& filepath)
{
    filepath = check_filepath(filepath, type, "load", true);

    char* c_filepath = const_cast<char*>(filepath.c_str());

    int res = B_WsLoad(c_filepath, type);
    if (res != EXIT_SUCCESS)
        throw std::runtime_error("Something went wrong when trying to import " + type_name + " from file " + filepath);
}

template<class T>
void KDBAbstract<T>::save(std::string& filepath)
{
    filepath = check_filepath(filepath, type, "save", false);

    char* c_filepath = const_cast<char*>(filepath.c_str());

    int res = B_WsSave(c_filepath, type);
    if (res != EXIT_SUCCESS)
        throw std::runtime_error("Something went wrong when trying to save " + type_name + " to file " + filepath);
}

template<class T>
void KDBAbstract<T>::clear()
{
    int res = B_WsClear("", type);
    if (res != EXIT_SUCCESS)
        throw std::runtime_error("Something went wrong when trying to clear objets of type " + type_name);
}
