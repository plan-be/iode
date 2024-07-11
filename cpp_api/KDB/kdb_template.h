#pragma once
#include "kdb_abstract.h"


template<class T> class KDBTemplate: public KDBAbstract
{
private:
    bool check_object_already_exists(KDB* kdb, const std::string& name)
    {
        char* c_name = to_char_array(name);
        return K_find(kdb, c_name) >= 0;
    }

protected:
    virtual T copy_obj(const T original) const = 0;

    virtual T get_unchecked(const int pos) const = 0;

    KDBTemplate(KDBAbstract* kdb, const bool deep_copy, const std::string& pattern) :
        KDBAbstract(kdb, deep_copy, pattern) {}

    template<class... Args> int add_or_update(KDB* kdb, const std::string& name, Args... args)
    {
        char* c_name = to_char_array(name);
        int pos = K_add(kdb, c_name, args...);
        if (pos == -1) 
            throw std::runtime_error("Cannot add or update " + v_iode_types[k_type] + " with name '" + name + ".'\n" +  
                                     "Iode has not been initialized.");
        if (pos < -1) 
            throw std::runtime_error("Cannot add or update " + v_iode_types[k_type] + " with name '" + name + "'.\n" + 
                                     "Reason: unknown");
        return pos;
    }

public:
    KDBTemplate(const IodeDatabaseType iode_type, const std::string& filepath) : 
        KDBAbstract(iode_type, filepath) {}

    //  Create + Update + Get + Copy methods

    // Note: The definition of a template must be visible in the translation unit in which it is instantiated 
    // (which usually means your code in the header file), or it must be explicitly instantiated for the desired types. 
    // That is simply how C++ templates work.
    // https://stackoverflow.com/questions/23412703/visual-studio-static-libraries-and-variadic-template-classes#comment35876150_23412703
    // https://stackoverflow.com/a/47890906

    template<class... Args> int add(const std::string& name, Args... args)
    {
        int pos;
        std::string error_msg = "Cannot add " + v_iode_types[k_type] + " with name '" + name + "'.\n";
        error_msg += "The " + v_iode_types[k_type] + " with name '" + name + "' already exists in the database.\n";
        error_msg += "Use the update() method instead.";

        check_name(name, k_type);

        KDB* kdb = get_database();
        if(!is_shallow_copy_database())
        {
            if(check_object_already_exists(kdb, name))
                throw std::invalid_argument(error_msg);
            pos = add_or_update(kdb, name, args...);
        }
        else
        {
            KDB* global_kdb = K_WS[k_type];
            if(check_object_already_exists(global_kdb, name))
                throw std::invalid_argument(error_msg);
            // add new obj to the global KDB
            int pos_global = add_or_update(global_kdb, name, args...);
            // add a new entry and copy the pointer
            char* c_name = to_char_array(name);
            pos = K_add_entry(kdb, c_name);
            KSOVAL(kdb, pos) = KSOVAL(global_kdb, pos_global);
        }

        return pos;
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
        if(!check_object_already_exists(kdb, name))
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
