#pragma once
#include "kdb_template.h"


class KDBLists : public KDBTemplate<CKDBLists, List>
{
protected:

    List copy_obj(const List original) const override { return original; }

    List get_unchecked(const std::string& name) const override;

public:
    // global or standalone database
    KDBLists(const bool is_global, const std::string& filepath="") 
        : KDBTemplate(LISTS, is_global, filepath) {}
    
    // shallow copy database
    KDBLists(KDBLists* kdb, const std::string& pattern) : KDBTemplate(kdb, pattern) {};

    KDBLists* subset(const std::string& pattern, const bool deep_copy=false)
    {
        return template_subset<KDB, KDBLists>(this, pattern, deep_copy);
    }

    bool add(const std::string& name, const List& list);

    void update(const std::string& name, const List& list);
};


inline std::size_t hash_value(KDBLists const& cpp_kdb)
{
    KDB* kdb = cpp_kdb.get_database();
    if(!kdb) return 0;

    char* list;
    std::size_t seed = 0;
    for(const auto& [name, handle] : kdb->k_objs)
    {
        hash_combine<std::string>(seed, name); 
        // need to wrap with std::string() because hash_value() and
        // hash_combine() only compare pointer addresses when applied 
        // on char* arrays
        list = (char*) P_get_ptr(SW_getptr(handle), 0);
        hash_combine<std::string>(seed, std::string(list));
    }
    return seed;
}

inline KDBLists Lists(true);
