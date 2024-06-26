#pragma once
#include "kdb_template.h"
#include "objects/scalar.h"


class KDBScalars : public KDBTemplate<Scalar>
{
protected:

    Scalar copy_obj(const Scalar& original) const override;

    Scalar get_unchecked(const int pos) const override;

    KDBScalars(KDBScalars* kdb, const bool deep_copy, const std::string& pattern) : 
        KDBTemplate(kdb, deep_copy, pattern) {};

public:
    KDBScalars(const std::string& filepath="") : KDBTemplate(SCALARS, filepath) {}

    KDBScalars* subset(const std::string& pattern, const bool deep_copy=false)
    {
        return new KDBScalars(this, deep_copy, pattern);
    }

    int add(const std::string& name, const Scalar& obj);

    int add(const std::string& name, const double value, const double relax, const double std=IODE_NAN);

    void update(const std::string& name, const Scalar& obj);

    void update(const std::string& name, const double value, const double relax, const double std = IODE_NAN);

    void update(const int pos, const double value, const double relax, const double std = IODE_NAN);
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
    KDB* kdb = cpp_kdb.get_database();
    if(kdb == NULL) return 0;

    std::size_t seed = 0;
    for(int pos=0; pos < kdb->k_nb; pos++)
    {
        boost::hash_combine(seed, kdb->k_objs[pos].o_name);
        boost::hash_combine(seed, *KSVAL(kdb, pos));
    }
    return seed;
}

inline KDBScalars Scalars;
