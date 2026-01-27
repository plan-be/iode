#pragma once

#include "api/constants.h"
#include "api/objs/kdb.h"


struct KDBMacros : public KDBTemplate<char>
{
    // global or standalone database
    KDBMacros(const bool is_global) : KDBTemplate(OBJECTS, is_global) {}

    // subset (shallow or deep copy) 
    KDBMacros(KDBMacros* db_parent, const std::string& pattern, const bool copy) 
        : KDBTemplate(db_parent, pattern, copy) {}

    // copy constructor
    KDBMacros(const KDBMacros& other): KDBTemplate(other) {}

    char* get_macro(const SWHDL handle) const;
    char* get_macro(const std::string& name) const;
    bool set_macro(const std::string& name, char* value, const int length);

    char* get_obj(const SWHDL handle) const override
    {
        return get_macro(handle);
    }

    char* get_obj(const std::string& name) const override
    {
        return get_macro(name);
    }

    bool set_obj(const std::string& name, const char* value) override
    {
        return set_macro(name, (char*) value, (int) strlen(value));
    }

    bool load_asc(const std::string& filename) override;
    bool save_asc(const std::string& filename) override;

    char* dde_create_obj_by_name(const std::string& name, int* nc, int* nl) override;

    bool print_obj_def(const std::string& name) override;

private:
    bool grep_obj(const std::string& name, const SWHDL handle, 
        const std::string& pattern, const bool ecase, const bool forms, 
        const bool texts, const char all) const override;
    
    void update_reference_db() override;
};
