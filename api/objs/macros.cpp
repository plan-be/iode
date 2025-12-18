#pragma once

#include "api/constants.h"
#include "api/objs/kdb.h"
#include "api/objs/pack.h"
#include "api/objs/macros.h"


char* KDBMacros::get_macro(const SWHDL handle) const
{    
    return (char*) P_get_ptr(SW_getptr(handle), 0);
}

char* KDBMacros::get_macro(const std::string& name) const
{
    SWHDL handle = this->get_handle(name);
    if(handle == 0)  
        throw std::invalid_argument("comment with name '" + name + "' not found.");
    
    return get_macro(handle);
}

bool KDBMacros::set_macro(const std::string& name, char* value, const int length)
{
    if(this->k_type != OBJECTS)
    {
        kerror(0, "Invalid database type for adding an Object");
        return false;
    }

    char* pack = NULL;
    K_opack(&pack, value, (int*) &length);
    bool success = set_packed_object(name, pack);
    return success;
}

bool KDBMacros::grep_obj(const std::string& name, const SWHDL handle, 
    const std::string& pattern, const bool ecase, const bool forms, const bool texts, 
    const char all) const
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
