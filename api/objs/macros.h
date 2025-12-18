#pragma once

#include "api/constants.h"
#include "api/objs/kdb.h"


struct KDBMacros : public KDB
{
    // global or standalone database
    KDBMacros(const bool is_global) : KDB(OBJECTS, is_global) {}

    // shallow copy database
    KDBMacros(KDBMacros* db_parent, const std::string& pattern = "*") 
        : KDB(db_parent, pattern) {}

    // copy constructor
    KDBMacros(const KDBMacros& other): KDB(other) {}

    char* get_macro(const SWHDL handle) const;
    char* get_macro(const std::string& name) const;
    bool set_macro(const std::string& name, char* value, const int length);

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
