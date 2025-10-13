#pragma once
#include "kdb_template.h"


class KDBLists : public KDBTemplate<List>
{
protected:

    List copy_obj(const List original) const override { return original; }

    List get_unchecked(const int pos) const override;

    KDBLists(KDBLists* kdb, const bool deep_copy, const std::string& pattern) : 
        KDBTemplate(kdb, deep_copy, pattern) {};

public:
    KDBLists(const std::string& filepath="") : KDBTemplate(LISTS, filepath) {}

    KDBLists* subset(const std::string& pattern, const bool deep_copy=false)
    {
        return new KDBLists(this, deep_copy, pattern);
    }

    int add(const std::string& name, const List& list);

    void update(const std::string& name, const List& list);

    void update(const int pos, const List& list);
};


inline std::size_t hash_value(KDBLists const& cpp_kdb)
{
    KDB* kdb = cpp_kdb.get_database();
    if(kdb == NULL) return 0;

    std::size_t seed = 0;
    for(int pos=0; pos < kdb->size(); pos++)
    {
        char* o_name = kdb->k_objs[pos].o_name;
        hash_combine<std::string_view>(seed, std::string_view(o_name, strlen(o_name)));
        // need to wrap with std::string() because hash_value() and
        // hash_combine() only compare pointer addresses when applied 
        // on char* arrays
        hash_combine<std::string>(seed, std::string(KLVAL(kdb, pos)));
    }
    return seed;
}

inline KDBLists Lists;
