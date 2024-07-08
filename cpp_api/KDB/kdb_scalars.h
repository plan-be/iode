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


inline std::size_t hash_value(KDBScalars const& cpp_kdb)
{
    KDB* kdb = cpp_kdb.get_database();
    if(kdb == NULL) return 0;

    std::size_t seed = 0;
    for(int pos=0; pos < kdb->k_nb; pos++)
    {
        char* o_name = kdb->k_objs[pos].o_name;
        hash_combine<std::string_view>(seed, std::string_view(o_name, std::strlen(o_name)));
        hash_combine<SCL>(seed, *KSVAL(kdb, pos));
    }
    return seed;
}

inline KDBScalars Scalars;
