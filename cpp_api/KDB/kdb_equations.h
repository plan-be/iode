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

public:
    KDBEquations(const std::string& filepath) : KDBTemplate(filepath) {}

    KDBEquations(const EnumIodeKDBType kdb_type = KDB_GLOBAL, const std::string& pattern = "") : 
        KDBTemplate(kdb_type, I_EQUATIONS, pattern) {};

    KDBEquations(const KDBEquations& kdb_to_copy) : KDBTemplate(kdb_to_copy) {}

    std::string get_lec(const int pos) const;

    std::string get_lec(const std::string& name) const;

    int add(const std::string& name, const Equation& obj);

    int add(const std::string& name, const std::string& lec, const std::string& comment = "", const std::string& method = "",
        Sample* sample = nullptr, const std::string& instruments = "", const std::string& block = "", const std::array<float, EQS_NBTESTS>& tests = {0.}, const bool date = true);

    void update(const std::string& name, const std::string& lec = "", const std::string& comment = "", const std::string& method = "",
        Sample* sample = nullptr, const std::string& instruments = "", const std::string& block = "", const std::array<float, EQS_NBTESTS>* tests = nullptr, const bool date = false);

    void update(const int pos, const std::string& lec = "", const std::string& comment = "", const std::string& method = "",
        Sample* sample = nullptr, const std::string& instruments = "", const std::string& block = "", const std::array<float, EQS_NBTESTS>* tests = nullptr, const bool date = false);
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
    KDB* kdb = cpp_kdb.get_KDB();
    if(kdb == NULL) return 0;

    std::size_t seed = 0;
    for(int pos=0; pos < kdb->k_nb; pos++)
    {
        boost::hash_combine(seed, kdb->k_objs[pos].o_name);
        boost::hash_combine(seed, *KEVAL(kdb, pos));
    }
    return seed;
}
