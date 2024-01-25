#pragma once
#include "kdb_template.h"


class KDBLists : public KDBTemplate<List>
{
protected:

    List copy_obj(const List& original) const override;

    List get_unchecked(const int pos) const override;

    KDBLists(const bool deep_copy, const std::string& pattern) : 
        KDBTemplate(I_LISTS, deep_copy, pattern) {};

public:
    KDBLists(const std::string& filepath="") : KDBTemplate(I_LISTS, filepath) {}

    KDBLists* subset(const std::string& pattern, const bool deep_copy=false)
    {
        return new KDBLists(deep_copy, pattern);
    }

    int add(const std::string& name, const List& list);

    void update(const std::string& name, const List& list);

    void update(const int pos, const List& list);
};

/**
 * @brief compute a hash value for the database.
 * 
 * @note see https://www.boost.org/doc/libs/1_55_0/doc/html/hash/custom.html
 *       and https://www.boost.org/doc/libs/1_55_0/doc/html/hash/combine.html
 * 
 * @return std::size_t 
 */
inline std::size_t hash_value(KDBLists const& cpp_kdb)
{
    KDB* kdb = cpp_kdb.get_database();
    if(kdb == NULL) return 0;

    std::size_t seed = 0;
    for(int pos=0; pos < kdb->k_nb; pos++)
    {
        boost::hash_combine(seed, kdb->k_objs[pos].o_name);
        // need to wrapp with std::string() because hash_value() and
        // hash_combine() only compare pointer addresses when applied 
        // on char* arrays
        boost::hash_combine(seed, std::string(KLVAL(kdb, pos)));
    }
    return seed;
}

inline KDBLists Lists;
