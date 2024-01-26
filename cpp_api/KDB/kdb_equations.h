#pragma once
#include "time/period.h"
#include "time/sample.h"
#include "objects/equation.h"
#include "kdb_template.h"


class KDBEquations : public KDBTemplate<Equation>
{
protected:
    Equation copy_obj(const Equation& original) const override;

    Equation get_unchecked(const int pos) const override;

    KDBEquations(KDBEquations* kdb, const bool deep_copy, const std::string& pattern) : 
        KDBTemplate(kdb, deep_copy, pattern) {};

public:
    KDBEquations(const std::string& filepath="") : KDBTemplate(I_EQUATIONS, filepath) {}

    KDBEquations* subset(const std::string& pattern, const bool deep_copy=false)
    {
        return new KDBEquations(this, deep_copy, pattern);
    }

    std::string get_lec(const int pos) const;

    std::string get_lec(const std::string& name) const;

    int add(const std::string& name, const Equation& obj);

    int add(const std::string& name, const std::string& lec, const std::string& method = "LSQ", const std::string& from = "", const std::string& to = "", 
        const std::string& comment = "", const std::string& instruments = "", const std::string& block = "", const bool date = true);

    void update(const std::string& name, const Equation& obj);

    void update(const std::string& name, const std::string& lec, const std::string& method = "LSQ", const std::string& from = "", const std::string& to = "", 
        const std::string& comment = "", const std::string& instruments = "", const std::string& block = "", const bool date = false);

    void update(const int pos, const std::string& lec, const std::string& method = "LSQ", const std::string& from = "", const std::string& to = "", 
        const std::string& comment = "", const std::string& instruments = "", const std::string& block = "", const bool date = false);
};


/**
 * @brief compute a hash value for the database.
 * 
 * @note see https://www.boost.org/doc/libs/1_55_0/doc/html/hash/custom.html
 *       and https://www.boost.org/doc/libs/1_55_0/doc/html/hash/combine.html
 * 
 * @return std::size_t 
 */
inline std::size_t hash_value(KDBEquations const& cpp_kdb)
{
    KDB* kdb = cpp_kdb.get_database();
    if(kdb == NULL) return 0;

    std::size_t seed = 0;
    for(int pos=0; pos < kdb->k_nb; pos++)
    {
        boost::hash_combine(seed, kdb->k_objs[pos].o_name);
        boost::hash_combine(seed, *KEVAL(kdb, pos));
    }
    return seed;
}

inline KDBEquations Equations;
