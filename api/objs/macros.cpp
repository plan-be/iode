#pragma once

#include "api/constants.h"
#include "api/objs/kdb.h"
#include "api/objs/pack.h"
#include "api/objs/macros.h"


std::string KDBMacros::get_macro(const std::string& name) const
{
    std::string key = to_key(name);
    if(!this->contains(key))  
        throw std::invalid_argument("macro with name '" + name + "' not found.");
    
    return *(this->k_objs[key]);
}

bool KDBMacros::set_macro(const std::string& name, std::string& macro)
{
    if(this->k_type != OBJECTS)
    {
        kerror(0, "Invalid database type for adding an Object");
        return false;
    }

    std::string key = to_key(name);
    std::string* macro_copy = new std::string(macro);
    if(this->contains(key))
        delete this->k_objs[key];
    this->k_objs[key] = macro_copy;
    return true;
}

bool KDBMacros::grep_obj(const std::string& name, const std::string& pattern, 
    const bool ecase, const bool forms, const bool texts, const char all) const
{
    bool found = false;
    if(texts) 
        found = wrap_grep_gnl(pattern, this->get_macro(handle), ecase, all);
    return found;
}

char* KDBMacros::dde_create_obj_by_name(const std::string& name, int* nc, int* nl)
{
    char* obj = this->get_macro(name);
    return obj;
}

bool KDBMacros::print_obj_def(const std::string& name)
{
    bool success = print_definition_generic(name, this->get_macro(name));
    return success;
}

void KDBMacros::update_reference_db()
{
    // not applicable for macros    
}

bool KDBMacros::load_asc(const std::string& filename)
{
    // not implemented for macros 
    return false;
}

bool KDBMacros::save_asc(const std::string& filename)
{
    // not implemented for macros
    return false;    
}
