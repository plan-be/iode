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

    KDBIdentities(KDBIdentities* kdb, const bool deep_copy, const std::string& pattern) : 
        KDBTemplate(kdb, deep_copy, pattern) {};

public:
    KDBIdentities(const std::string& filepath="") : KDBTemplate(IDENTITIES, filepath) {}

    KDBIdentities* subset(const std::string& pattern, const bool deep_copy=false)
    {
        return new KDBIdentities(this, deep_copy, pattern);
    }

    std::string get_lec(const int pos) const;

    std::string get_lec(const std::string& name) const;

    int add(const std::string& name, const std::string& lec);

    int add(const std::string& name, const Identity& obj);

    void update(const std::string& name, const std::string& lec);

    void update(const int pos, const std::string& lec);

    void execute_identities(const Period& from, const Period& to, const std::string& identities_list ="", 
                            const std::string& var_files = "", const std::string& scalar_files = "", const bool trace = false);

    void execute_identities(const std::string& from, const std::string& to, const std::string& identities_list = "", 
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
    KDB* kdb = cpp_kdb.get_database();
    if(kdb == NULL) return 0;

    std::size_t seed = 0;
    for(int pos=0; pos < kdb->k_nb; pos++)
    {
        boost::hash_combine(seed, kdb->k_objs[pos].o_name);
        // need to wrapp with std::string() because hash_value() and
        // hash_combine() only compare pointer addresses when applied 
        // on char* arrays
        boost::hash_combine(seed, std::string(KILEC(kdb, pos)));
    }
    return seed;
}

inline KDBIdentities Identities;
