#pragma once
#include "objects/table.h"
#include "kdb_template.h"


class KDBTables : public KDBTemplate<Table>
{
protected:

    Table copy_obj(const Table& original) const override;

    Table get_unchecked(const int pos) const override;

public:
    KDBTables(std::string& filepath) : KDBTemplate(filepath) {}

    KDBTables(const EnumIodeKDBType kdb_type = KDB_GLOBAL, const std::string& pattern = "") : 
        KDBTemplate(kdb_type, I_TABLES, pattern) {};

    KDBTables(const KDBTables& kdb_to_copy) : KDBTemplate(kdb_to_copy) {}

    std::string get_title(const int pos) const;

    std::string get_title(const std::string& name) const;

    int add(const std::string& name, const int nb_columns);

    int add(const std::string& name, const int nbColumns, const std::string& def, std::vector<std::string>& vars, bool mode, bool files, bool date);

    int add(const std::string& name, const int nbColumns, const std::string& def, const std::string& lecs, bool mode, bool files, bool date);
};


/**
 * @brief compute a hash value for the database.
 * 
 * @note see https://www.boost.org/doc/libs/1_55_0/doc/html/hash/custom.html
 *       and https://www.boost.org/doc/libs/1_55_0/doc/html/hash/combine.html
 * 
 * @return std::size_t 
 */
inline std::size_t hash_value(KDBTables const& cpp_kdb)
{
    KDB* kdb = cpp_kdb.get_KDB();
    if(kdb == NULL) return 0;

    std::size_t seed = 0;
    for(int pos=0; pos < kdb->k_nb; pos++)
    {
        boost::hash_combine(seed, kdb->k_objs[pos].o_name);
        boost::hash_combine(seed, KTVAL(kdb, pos));
    }
    return seed;
}
