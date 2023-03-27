#pragma once
#include "kdb_template.h"
#include "objects/scalar.h"


class KDBScalars : public KDBTemplate<Scalar>
{
protected:

    Scalar copy_obj(const Scalar& original) const override;

    Scalar get_unchecked(const int pos) const override;

public:
    KDBScalars(std::string& filepath) : KDBTemplate(filepath) {}

    KDBScalars(const EnumIodeKDBType kdb_type = KDB_GLOBAL, const std::string& pattern = "") : 
        KDBTemplate(kdb_type, I_SCALARS, pattern) {};

    KDBScalars(const KDBScalars& kdb_to_copy) : KDBTemplate(kdb_to_copy) {}

    int add(const std::string& name, const IODE_REAL value, const IODE_REAL relax, const IODE_REAL std=L_NAN);

    void update(const std::string& name, const IODE_REAL value, const IODE_REAL relax, const IODE_REAL std = L_NAN);

    void update(const int pos, const IODE_REAL value, const IODE_REAL relax, const IODE_REAL std = L_NAN);
};


/**
 * @brief compute a hash value for the database.
 * 
 * @note see https://www.boost.org/doc/libs/1_55_0/doc/html/hash/custom.html
 *       and https://www.boost.org/doc/libs/1_55_0/doc/html/hash/combine.html
 * 
 * @return std::size_t 
 */
inline std::size_t hash_value(KDBScalars const& cpp_kdb)
{
    KDB* kdb = cpp_kdb.get_KDB();
    if(kdb == NULL) return 0;

    std::size_t seed = 0;
    for(int pos=0; pos < kdb->k_nb; pos++)
    {
        boost::hash_combine(seed, kdb->k_objs[pos].o_name);
        boost::hash_combine(seed, KSVAL(kdb, pos));
    }
    return seed;
}
