#pragma once
#include "kdb_template.h"


class KDBComments : public KDBTemplate<Comment>
{
protected:

    Comment copy_obj(const Comment& original) const override;

    Comment get_unchecked(const int pos) const override;

public:
    KDBComments(const EnumIodeKDBType kdb_type = KDB_GLOBAL, const std::string& pattern = "") : 
        KDBTemplate(kdb_type, I_COMMENTS, pattern) {};

    KDBComments(const KDBComments& kdb_to_copy) : KDBTemplate(kdb_to_copy) {}

    int add(const std::string& name, const Comment& comment);

    void update(const std::string& name, const Comment& comment);

    void update(const int pos, const Comment& comment);
};


/**
 * @brief compute a hash value for the database.
 * 
 * @note see https://www.boost.org/doc/libs/1_55_0/doc/html/hash/custom.html
 *       and https://www.boost.org/doc/libs/1_55_0/doc/html/hash/combine.html
 * 
 * @return std::size_t 
 */
inline std::size_t hash_value(KDBComments const& cpp_kdb)
{
    KDB* kdb = cpp_kdb.get_KDB();
    if(kdb == NULL) return 0;

    std::size_t seed = 0;
    for(int pos=0; pos < kdb->k_nb; pos++)
    {
        boost::hash_combine(seed, kdb->k_objs[pos].o_name);
        // need to wrapp with std::string() because hash_value() and
        // hash_combine() only compare pointer addresses when applied 
        // on char* arrays
        boost::hash_combine(seed, std::string(KCVAL(kdb, pos)));
    }
    return seed;
}

inline KDBComments Comments;
