#pragma once

#include "api/constants.h"
#include "api/objs/kdb.h"


struct KDBMacros : public KDBTemplate<KDBMacros, std::string>
{
private:
    // Constructors are private - use Create() factory method instead
    // global or standalone database
    KDBMacros(const bool is_global) : KDBTemplate(OBJECTS, is_global) {}

    // copy constructor
    KDBMacros(const KDBMacros& other): KDBTemplate(other) {}

public:
    /**
     * Factory method to create a managed instance with std::shared_ptr
     * 
     * Usage: auto db = KDB[...]::Create(is_global);
     * 
     * This ensures the instance is managed by shared_ptr, preventing bad_weak_ptr errors
     * when using get_subset() or other methods that rely on std::enable_shared_from_this.
     * 
     * Note: KDB[...] classes should also use KDB[...]::Create(...) for creation.
     * 
     * @param is_global Whether to create a global database (true) or standalone (false)
     * @return          std::shared_ptr<KDB[...]> pointing to the newly created instance
     */
    static std::shared_ptr<KDBMacros> Create(const bool is_global)
    {
        return std::shared_ptr<KDBMacros>(new KDBMacros(is_global));
    }

    std::string get_macro(const std::string& name) const;
    bool set_macro(const std::string& name, std::string& macro);

    bool load_asc(const std::string& filename) override;
    bool save_asc(const std::string& filename) override;

    char* dde_create_obj_by_name(const std::string& name, int* nc, int* nl) override;

    bool print_obj_def(const std::string& name) override;

    void merge_from(const std::string& input_file) override {}

private:
    bool binary_to_obj(const std::string& name, char* pack) override
    {
        return false;
    }

    bool obj_to_binary(char** pack, const std::string& name) override
    {
        return false;
    }

    char* sub_convert_obj_version(char* npack, const std::string& name, const int vers) override
    {
        return npack;
    }

    bool grep_obj(const std::string& name, const std::string& pattern, 
        const bool ecase, const bool forms, const bool texts, const char all) const override;
    
    void update_reference_db() override;
};

using KDBMacrosPtr = std::shared_ptr<KDBMacros>;
