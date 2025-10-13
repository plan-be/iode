#pragma once
#include "kdb_abstract.h"


template<class T> class KDBTemplate: public KDBAbstract
{
protected:
    virtual T copy_obj(const T original) const = 0;

    virtual T get_unchecked(const int pos) const = 0;

    KDBTemplate(KDBAbstract* kdb, const bool deep_copy, const std::string& pattern) :
        KDBAbstract(kdb, deep_copy, pattern) {}

    template<class... Args> bool add_or_update(KDB* kdb, const std::string& name, Args... args)
    {
        bool success;
        char* c_name = to_char_array(name);
        if (!kdb) 
            throw std::runtime_error("Cannot add or update " + v_iode_types[k_type] + " with name '" + name + ".'\n" +  
                                     "Iode has not been initialized.");
        success = K_add(kdb, c_name, args...);
        if(!success) 
            throw std::runtime_error("Cannot add or update " + v_iode_types[k_type] + " with name '" + name + "'.\n" + 
                                     "Reason: unknown");
        return success;
    }

public:
    KDBTemplate(const IodeType iode_type, const std::string& filepath) : 
        KDBAbstract(iode_type, filepath) {}

    //  Create + Update + Get + Copy methods

    // Note: The definition of a template must be visible in the translation unit in which it is instantiated 
    // (which usually means your code in the header file), or it must be explicitly instantiated for the desired types. 
    // That is simply how C++ templates work.
    // https://stackoverflow.com/questions/23412703/visual-studio-static-libraries-and-variadic-template-classes#comment35876150_23412703
    // https://stackoverflow.com/a/47890906

    template<class... Args> bool add(const std::string& name, Args... args)
    {
        bool success;
        std::string error_msg = "Cannot add " + v_iode_types[k_type] + " with name '" + name + "'.\n";
        error_msg += "The " + v_iode_types[k_type] + " with name '" + name + "' already exists in the database.\n";
        error_msg += "Use the update() method instead.";

        check_name(name, k_type);

        KDB* kdb = get_database();
        if(!is_shallow_copy_database())
        {
            if(kdb->contains(name))
                throw std::invalid_argument(error_msg);
            success = add_or_update(kdb, name, args...);
            return success;
        }
        else
        {
            KDB* global_kdb = K_WS[k_type];
            if(global_kdb->contains(name))
                throw std::invalid_argument(error_msg);
            // add new obj to the global KDB
            success = add_or_update(global_kdb, name, args...);
            if(!success)
                return false;
            // add a new entry and copy the pointer
            K_add_entry(kdb, name);
            kdb->k_objs[name] = global_kdb->get_handle(name);
            return true;
        }
    }

    template<class... Args> void update(const int pos, Args... args)
    {
        // throw exception if object with passed position is not valid
        std::string name = get_name(pos);
        
        // NOTE: In the case of a shallow copy, only pointers to objects 
        //       are duplicated, not the objects.
        //       Modifying an object passing either the shallow copy
        //       or the global KDB (K_WS[k_type]) modifies the same object.
        KDB* kdb = get_database();
        add_or_update(kdb, name, args...);
    }

    template<class... Args> void update(const std::string& name, Args... args)
    {
        std::string error_msg = "Cannot update " + v_iode_types[k_type] + " with name '" + name + "'.\n";
        error_msg += "Name '" + name + "' not found in the database.\nUse the add() method instead.";

        KDB* kdb = get_database();
        if(!kdb->contains(name))
            throw std::invalid_argument(error_msg);
        
        // NOTE: In the case of a shallow copy, only pointers to objects 
        //       are duplicated, not the objects.
        //       Modifying an object passing either the shallow copy
        //       or the global KDB (K_WS[k_type]) modifies the same object.
        add_or_update(kdb, name, args...);
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
        int pos = index_of(name);
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
