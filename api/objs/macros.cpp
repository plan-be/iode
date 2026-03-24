#pragma once

#include "api/constants.h"
#include "api/objs/kdb.h"
#include "api/objs/pack.h"
#include "api/objs/macros.h"


std::string KDBMacros::get_macro(const std::string& name) const
{    
    std::shared_ptr<std::string> macro_ptr = this->get_obj_ptr(name);
    if(!macro_ptr)
        return "";
    return *macro_ptr;
}

bool KDBMacros::set_macro(const std::string& name, std::string& macro)
{
    if(this->k_type != OBJECTS)
    {
        kwarning("Invalid database type for adding an Object");
        return false;
    }

    this->set(name, macro);
    return true;
}

bool KDBMacros::grep_obj(const std::string& name, const std::string& pattern, 
    const bool ecase, const bool forms, const bool texts, const char all) const
{
    bool found = false;
    if(texts) 
        found = wrap_grep_gnl(pattern, this->get_macro(name), ecase, all);
    return found;
}

char* KDBMacros::dde_create_obj_by_name(const std::string& name, int* nc, int* nl)
{
    std::string macro = this->get_macro(name);
    char* obj = new char[macro.size() + 1];
    strcpy(obj, macro.c_str());
    return obj;
}

bool KDBMacros::print_obj_def(const std::string& name)
{
    std::string macro = this->get_macro(name);
    // W_Print(...) functions expect OEM encoding, so convert value from UTF-8 to OEM before printing 
    macro = utf8_to_oem(macro);
    bool success = print_definition_generic(name, (char*) macro.c_str());
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
