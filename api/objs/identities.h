#pragma once

#include "api/constants.h"
#include "api/objs/objs.h"
#include "api/objs/kdb.h"
#include "api/objs/variables.h"
#include "api/lec/lec.h"    


/*----------------------- STRUCTS ----------------------------*/

// Identity (struct with the identity LEC formula and the compiled CLEC structure)
class Identity
{
    std::string lec;        // LEC expression (may not be an equation)
    CLEC* clec;             // Compiled version of LEC

public:
    Identity(const std::string& lec)
    {
        set_lec(lec);
    }

    Identity(const Identity& other)
    {
        this->lec = other.lec;
        // NOTE : we do not use memcpy() because memcpy() actually makes  
        //        a shallow copy of a struct instead of a deep copy
        this->clec = clec_deep_copy(other.clec);
    }

    ~Identity()
    {
        this->lec.clear();

        if(this->clec)
            SW_nfree(this->clec);
    }

    std::string get_lec() const 
    {
        return this->lec;
    }

    void set_lec(const std::string& lec) 
    {
        this->lec = lec;
        // L_cc returns an allocated CLEC struct pointer.
        this->clec = L_cc((char*) lec.c_str());
    }

    CLEC* get_compiled_lec()
    {
        if(this->clec)
            SW_nfree(this->clec);
        this->clec = L_cc((char*) lec.c_str());
        return this->clec;
    }

    // required to be used in std::map
    Identity& operator=(const Identity& other)
    {
        this->lec = other.lec;
        // NOTE : we do not use memcpy() because memcpy() actually makes  
        //        a shallow copy of a struct instead of a deep copy
        this->clec = clec_deep_copy(other.clec);
        return *this;
    }

    bool operator==(const Identity& other) const
    {
        CLEC* cl1 = L_cc((char*) this->lec.c_str());
        CLEC* cl2 = L_cc((char*) other.lec.c_str());

        if(cl1 == NULL || cl2 == NULL || cl1->tot_lg != cl2->tot_lg || memcmp(cl1, cl2, cl1->tot_lg) != 0)
            return false;
        return true;
    }

    std::vector<std::string> get_coefficients_list(const bool create_if_not_exit=true);
    std::vector<std::string> get_variables_list(const bool create_if_not_exit=true);
};

/*----------------------- STRUCTS ----------------------------*/

struct KDBIdentities : public KDBTemplate<Identity>
{
private:
    int set(const std::string& name, const std::string& lec);

public:
    // global or standalone database
    KDBIdentities(const bool is_global) : KDBTemplate(IDENTITIES, is_global) {}

    // subset (shallow or deep copy) 
    KDBIdentities(KDBIdentities* db_parent, const std::string& pattern, const bool copy) 
        : KDBTemplate(db_parent, pattern, copy) {}

    // copy constructor
    KDBIdentities(const KDBIdentities& other): KDBTemplate(other) {}

    // NOTE: get_obj() and set_obj() methods to be replaced by operator[] when 
    //       k_objs will be changed to std::map<std::string, T>
    //       T& operator[](const std::string& name)

    Identity* get_obj(const SWHDL handle) const override;
    Identity* get_obj(const std::string& name) const override;

    bool set_obj(const std::string& name, const Identity* value) override;

    Identity* get(const std::string& name) const;
    bool add(const std::string& name, const Identity& obj);
    void update(const std::string& name, const Identity& obj);
    
    bool add(const std::string& name, const std::string& lec);
    void update(const std::string& name, const std::string& lec);

    std::string get_lec(const std::string& name) const;

    bool execute_identities(const Period& from, const Period& to, const std::string& identities_list ="", 
                            const std::string& var_files = "", const std::string& scalar_files = "", const bool trace = false);

    bool execute_identities(const std::string& from, const std::string& to, const std::string& identities_list = "", 
                            const std::string& var_files = "", const std::string& scalar_files = "", const bool trace = false);

    bool load_asc(const std::string& filename) override;
    bool save_asc(const std::string& filename) override;

    char* dde_create_obj_by_name(const std::string& name, int* nc, int* nl) override;

    bool print_obj_def(const std::string& name) override;

    void merge_from(const std::string& input_file) override
    {
        KDBIdentities from(false);
        KDB::merge_from(from, input_file);
    }

    bool copy_from_file(const std::string& file, const std::string& objs_names, 
        std::set<std::string>& v_found)
    {
        KDBIdentities from(false);
        return KDB::copy_from_file(from, file, objs_names, v_found);
    }

private:
    bool grep_obj(const std::string& name, const SWHDL handle, 
        const std::string& pattern, const bool ecase, const bool forms, 
        const bool texts, const char all) const override;
    
    void update_reference_db() override;
};

/*----------------------- GLOBALS ----------------------------*/
// unique_ptr -> automatic memory management
//            -> no need to delete KDB workspaces manually
inline std::unique_ptr<KDBIdentities> global_ws_idt = std::make_unique<KDBIdentities>(true);
inline std::array<KDBIdentities*, 5> global_ref_idt = { nullptr };

/*----------------------- FUNCTIONS ----------------------------*/

Identity* K_iunpack(char *);

inline std::size_t hash_value(KDBIdentities const& cpp_kdb)
{
    if(cpp_kdb.size() == 0)
        return 0;

    std::string lec;
    std::size_t seed = 0;
    for(const auto& [name, handle] : cpp_kdb.k_objs)
    {
        hash_combine<std::string>(seed, name);  
        lec = cpp_kdb.get_lec(name);
        hash_combine<std::string>(seed, std::string(lec));
    }
    return seed;
}
