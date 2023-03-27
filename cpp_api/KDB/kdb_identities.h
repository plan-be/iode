#pragma once
#include "kdb_template.h"
#include "objects/identity.h"


class KDBIdentities : public KDBTemplate<Identity>
{
private:
    int add_or_update(const std::string& name, const std::string& lec);

protected:

    Identity copy_obj(const Identity& original) const override;

    Identity get_unchecked(const int pos) const override;

public:
    KDBIdentities(std::string& filepath) : KDBTemplate(filepath) {}

    KDBIdentities(const EnumIodeKDBType kdb_type = KDB_GLOBAL, const std::string& pattern = "") : 
        KDBTemplate(kdb_type, I_IDENTITIES, pattern) {};
    
    KDBIdentities(const KDBIdentities& kdb_to_copy) : KDBTemplate(kdb_to_copy) {}

    std::string get_lec(const int pos) const;

    std::string get_lec(const std::string& name) const;

    int add(const std::string& name, const std::string& lec);

    void update(const std::string& name, const std::string& lec);

    void update(const int pos, const std::string& lec);

    void execute_identities(const Period& from, const Period& to, const std::string& identities_list, 
                            const std::string& var_files = "", const std::string& scalar_files = "", const bool trace = false);

    void execute_identities(const std::string& from, const std::string& to, const std::string& identities_list, 
                            const std::string& var_files = "", const std::string& scalar_files = "", const bool trace = false);
};


/**
 * @brief compute a hash value for the database.
 * 
 * @note see https://www.boost.org/doc/libs/1_55_0/doc/html/hash/custom.html
 *       and https://www.boost.org/doc/libs/1_55_0/doc/html/hash/combine.html
 * 
 * @return std::size_t 
 */
inline std::size_t hash_value(KDBIdentities const& cpp_kdb)
{
    KDB* kdb = cpp_kdb.get_KDB();
    if(kdb == NULL) return 0;

    std::size_t seed = 0;
    for(int pos=0; pos < kdb->k_nb; pos++)
    {
        boost::hash_combine(seed, kdb->k_objs[pos].o_name);
        boost::hash_combine(seed, KILEC(kdb, pos));
    }
    return seed;
}
