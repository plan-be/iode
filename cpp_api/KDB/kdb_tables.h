#pragma once
#include "objects/table.h"
#include "kdb_template.h"


class KDBTables : public KDBTemplate<Table>
{
protected:

    Table copy_obj(const Table& original) const override;

    Table get_unchecked(const int pos) const override;

    KDBTables(KDBTables* kdb, const bool deep_copy, const std::string& pattern) : 
        KDBTemplate(kdb, deep_copy, pattern) {};

public:
    KDBTables(const std::string& filepath="") : KDBTemplate(TABLES, filepath) {}

    KDBTables* subset(const std::string& pattern, const bool deep_copy=false)
    {
        return new KDBTables(this, deep_copy, pattern);
    }

    std::string get_title(const int pos) const;

    std::string get_title(const std::string& name) const;

    int add(const std::string& name, const Table& obj);

    int add(const std::string& name, const int nb_columns);

    int add(const std::string& name, const int nbColumns, const std::string& def, const std::vector<std::string>& vars, 
        bool mode = false, bool files = false, bool date = false);

    int add(const std::string& name, const int nbColumns, const std::string& def, const std::vector<std::string>& titles, 
        const std::vector<std::string>& lecs, bool mode = false, bool files = false, bool date = false);

    int add(const std::string& name, const int nbColumns, const std::string& def, const std::string& lecs, 
        bool mode = false, bool files = false, bool date = false);
};


inline std::size_t hash_value(KDBTables const& cpp_kdb)
{
    KDB* kdb = cpp_kdb.get_database();
    if(kdb == NULL) return 0;

    std::size_t seed = 0;
    for(int pos=0; pos < kdb->k_nb; pos++)
    {
        char* o_name = kdb->k_objs[pos].o_name;
        hash_combine<std::string_view>(seed, std::string_view(o_name, std::strlen(o_name)));
        hash_combine<TBL>(seed, *KTVAL(kdb, pos));
    }
    return seed;
}

inline KDBTables Tables;
