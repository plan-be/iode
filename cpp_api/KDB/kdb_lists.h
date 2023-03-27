#pragma once
#include "kdb_template.h"


class KDBLists : public KDBTemplate<List>
{
protected:

    List copy_obj(const List& original) const override;

    List get_unchecked(const int pos) const override;

public:
    KDBLists(std::string& filepath) : KDBTemplate(filepath) {}

    KDBLists(const EnumIodeKDBType kdb_type = KDB_GLOBAL, const std::string& pattern = "") : 
        KDBTemplate(kdb_type, I_LISTS, pattern) {};

    KDBLists(const KDBLists& kdb_to_copy) : KDBTemplate(kdb_to_copy) {}

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
    KDB* kdb = cpp_kdb.get_KDB();
    if(kdb == NULL) return 0;

    std::size_t seed = 0;
    for(int pos=0; pos < kdb->k_nb; pos++)
    {
        boost::hash_combine(seed, kdb->k_objs[pos].o_name);
        boost::hash_combine(seed, KLVAL(kdb, pos));
    }
    return seed;
}
