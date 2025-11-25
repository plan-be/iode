#pragma once
#include "kdb_template.h"
#include "api/objs/identities.h"


class KDBIdentities : public KDBTemplate<Identity*>
{
private:
    int set(const std::string& name, const std::string& lec);

protected:

    // WARNING: 1) A top-level const is a const qualifier that applies to an object itself:
    //             - const int x;       this const applies to x, so it is top-level
    //             - int* const ptr;    this const applies to ptr, so it is top-level
    //          2) In contrast, a low-level const is a const qualifier that applies to the object being referenced 
    //             or pointed to:
    //             - const int& ref;    this const applies to the object being referenced, so it is low-level
    //             - const int* ptr;    this const applies to the object being pointed to, so it is low-level
    //          -> In the method copy_obj(const T original) of the template class KDBTemplate, the const is 'top-level'.
    //          -> In the present class, the const qualifier must be placed AFTER the * so that it remains a 'top-level' const. 
    // see https://stackoverflow.com/a/78058445 and 
    // https://www.learncpp.com/cpp-tutorial/type-deduction-with-pointers-references-and-const/?utm_content=cmp-true
    Identity* copy_obj(Identity* const original) const override;

    Identity* get_unchecked(const std::string& name) const override;

    KDBIdentities(KDBIdentities* kdb, const bool deep_copy, const std::string& pattern) : 
        KDBTemplate(kdb, deep_copy, pattern) {};

public:
    KDBIdentities(const std::string& filepath="") : KDBTemplate(IDENTITIES, filepath) {}

    KDBIdentities* subset(const std::string& pattern, const bool deep_copy=false)
    {
        return new KDBIdentities(this, deep_copy, pattern);
    }

    std::string get_lec(const std::string& name) const;

    bool add(const std::string& name, const std::string& lec);

    bool add(const std::string& name, const Identity& obj);

    void update(const std::string& name, const std::string& lec);

    bool execute_identities(const Period& from, const Period& to, const std::string& identities_list ="", 
                            const std::string& var_files = "", const std::string& scalar_files = "", const bool trace = false);

    bool execute_identities(const std::string& from, const std::string& to, const std::string& identities_list = "", 
                            const std::string& var_files = "", const std::string& scalar_files = "", const bool trace = false);
};


inline std::size_t hash_value(KDBIdentities const& cpp_kdb)
{
    KDB* kdb = cpp_kdb.get_database();
    if(!kdb) return 0;

    char* lec;
    std::size_t seed = 0;
    for(const auto& [name, handle] : kdb->k_objs)
    {
        hash_combine<std::string>(seed, name);  
        // need to wrap with std::string() because hash_value() and
        // hash_combine() only compare pointer addresses when applied 
        // on char* arrays
        lec = (char*) P_get_ptr(SW_getptr(handle), 0);
        hash_combine<std::string>(seed, std::string(lec));
    }
    return seed;
}

inline KDBIdentities Identities;
