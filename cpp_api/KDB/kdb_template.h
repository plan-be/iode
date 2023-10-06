#pragma once
#include "kdb_abstract.h"


template<class T> class KDBTemplate: public KDBAbstract
{
private:
    void check_object_already_exists(KDB* kdb, const std::string& name)
    {
        // throw exception if object with passed name already exist
        char* c_name = to_char_array(name);
        if (K_find(kdb, c_name) >= 0)
            throw IodeExceptionInitialization(iode_type_name + " with name " + name, 
                iode_type_name + " with name " + name + " already exists. Use update() method instead.");
    }

protected:
    virtual T copy_obj(const T& original) const = 0;

    virtual T get_unchecked(const int pos) const = 0;

public:
    KDBTemplate(const EnumIodeKDBType kdb_type, const EnumIodeType iode_type, const std::string& pattern) : 
        KDBAbstract(kdb_type, iode_type, pattern) {}

    KDBTemplate(const KDBTemplate& kdb_to_copy) : KDBAbstract(kdb_to_copy) {}

    //  Create + Update + Get + Copy methods

    // Note: The definition of a template must be visible in the translation unit in which it is instantiated 
    // (which usually means your code in the header file), or it must be explicitly instantiated for the desired types. 
    // That is simply how C++ templates work.
    // https://stackoverflow.com/questions/23412703/visual-studio-static-libraries-and-variadic-template-classes#comment35876150_23412703
    // https://stackoverflow.com/a/47890906

    template<class... Args> int add_or_update(KDB* kdb, const std::string& name, Args... args)
    {
        char* c_name = to_char_array(name);
        int pos = K_add(kdb, c_name, args...);
        if (pos == -1) throw IodeExceptionFunction("Cannot add or update " + iode_type_name + " with name " + name,  
            "Iode has not been initialized");
        if (pos < -1) throw IodeExceptionFunction("Cannot add or update " + iode_type_name + " with name " + name, "Unknown");
        return pos;
    }

    template<class... Args> int add(const std::string& name, Args... args)
    {
        int pos;
        KDB* global_kdb = K_WS[iode_type];

        check_name(name, iode_type);

        switch (kdb_type)
        {
        case KDB_GLOBAL:
            check_object_already_exists(global_kdb, name);
            pos = add_or_update(global_kdb, name, args...);
            break;
        case KDB_LOCAL:
            check_object_already_exists(local_kdb, name);
            pos = add_or_update(local_kdb, name, args...);
            break;
        case KDB_SHALLOW_COPY:
            {
                // add new obj to global KDB if doesn't exist yet
                check_object_already_exists(global_kdb, name);
                int pos_global = add_or_update(global_kdb, name, args...);
                // add new entry to shallow copy KDB
                char* c_name = to_char_array(name);
                pos = K_add_entry(local_kdb, c_name);
                KSOVAL(local_kdb, pos) = KSOVAL(global_kdb, pos_global);
                break;
            }
        default:
            break;
        }

        return pos;
    }

    template<class... Args> void update(const int pos, Args... args)
    {
        // throw exception if object with passed position is not valid
        std::string name = get_name(pos);

        KDB* global_kdb = K_WS[iode_type];
        switch (kdb_type)
        {
        case KDB_GLOBAL:
            add_or_update(global_kdb, name, args...);
            break;
        case KDB_LOCAL:
            add_or_update(local_kdb, name, args...);
            break;
        case KDB_SHALLOW_COPY:
            // NOTE: in case of a shallow copy, only pointers to objects 
            //       are duplicated, not the objects.
            //       Modifying an object passing either the shallow copy 
            //       or the global KDB modifies the same object.
            add_or_update(local_kdb, name, args...);
            break;
        default:
            break;
        }
    }

    template<class... Args> void update(const std::string& name, Args... args)
    {
        // throw exception if object with passed name does not exist
        get_position(name);

        KDB* global_kdb = K_WS[iode_type];
        switch (kdb_type)
        {
        case KDB_GLOBAL:
            add_or_update(global_kdb, name, args...);
            break;
        case KDB_LOCAL:
            add_or_update(local_kdb, name, args...);
            break;
        case KDB_SHALLOW_COPY:
            // NOTE: in case of a shallow copy, only pointers to objects 
            //       are duplicated, not the objects.
            //       Modifying an object passing either the shallow copy 
            //       or the global KDB modifies the same object.
            add_or_update(local_kdb, name, args...);
            break;
        default:
            break;
        }
    }

    T get(const int pos) const
    {
        // throw exception if object with passed position is not valid
        get_name(pos);
        return get_unchecked(pos);
    }

    T get(const std::string& name) const
    {
        // throw exception if object with passed name does not exist
        int pos = get_position(name);
        return get_unchecked(pos);
    }

    T copy(const int pos) const
    {
        // throw exception if object with passed position is not valid
        T obj = get(pos);
        return copy_obj(obj);
    }

    T copy(const std::string& name) const
    {
        // throw exception if object with passed name does not exist
        T obj = get(name);
        return copy_obj(obj);
    }
};
