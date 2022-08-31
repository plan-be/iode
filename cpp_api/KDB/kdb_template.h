#pragma once
#include "kdb_abstract.h"


template<class T> class KDBTemplate: public KDBAbstract
{
protected:
    virtual T copy_obj(const T& original) const = 0;

    virtual T get_unchecked(const int pos) const = 0;

public:
    KDBTemplate(EnumIodeType type, const std::string& pattern) : KDBAbstract(type, pattern) {}

    //  Create + Update + Get + Copy methods

    // Note: The definition of a template must be visible in the translation unit in which it is instantiated 
    // (which usually means your code in the header file), or it must be explicitly instantiated for the desired types. 
    // That is simply how C++ templates work.
    // https://stackoverflow.com/questions/23412703/visual-studio-static-libraries-and-variadic-template-classes#comment35876150_23412703
    // https://stackoverflow.com/a/47890906

    template<class... Args> int add_or_update(const std::string& name, Args... args)
    {
        char* c_name = const_cast<char*>(name.c_str());
        int pos = K_add(K_WS[iode_type], c_name, args...);
        if (pos == -1) throw IodeExceptionFunction("Cannot add or update " + iode_type_name + " with name " + name,  
            "Iode has not been initialized");
        if (pos < -1) throw IodeExceptionFunction("Cannot add or update " + iode_type_name + " with name " + name, "Unknown");
        return pos;
    }

    template<class... Args> int add(const std::string& name, Args... args)
    {
        int pos;

        check_name(name, iode_type);

        // throw exception if object with passed name already exist
        char* c_name = const_cast<char*>(name.c_str());
        if (K_find(get_KDB(), c_name) >= 0)
            throw IodeExceptionInitialization(iode_type_name + " with name " + name, 
                iode_type_name + " with name " + name + " already exists. Use update() method instead.");

        if (shallow_copy_kdb)
        {
            // add new obj to global KDB if doesn't exist yet
            int pos_global = K_find(K_WS[iode_type], c_name);
            if (pos_global < 0) pos_global = add_or_update(name, args...);
            // add new entry to shallow copy KDB
            pos = K_add_entry(shallow_copy_kdb, c_name);
            KSOVAL(shallow_copy_kdb, pos) = KSOVAL(K_WS[iode_type], pos_global);
        }
        else
        {
            pos = add_or_update(name, args...);
        }

        return pos;
    }

    template<class... Args> void update(const int pos, Args... args)
    {
        // throw exception if object with passed position is not valid
        std::string name = get_name(pos);
        add_or_update(name, args...);
    }

    template<class... Args> void update(const std::string& name, Args... args)
    {
        // throw exception if object with passed name does not exist
        get_position(name);
        add_or_update(name, args...);
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
