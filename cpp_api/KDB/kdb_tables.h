#pragma once
#include "../table.h"
#include "kdb_template.h"


class KDBTables : public KDBTemplate<Table>
{
protected:

    Table copy_obj(const Table& original) const override;

    Table get_unchecked(const int pos) const override;

public:
    KDBTables(const std::string& pattern = "", const bool shallow_copy=true) : 
        KDBTemplate(I_TABLES, pattern, shallow_copy) {};

    std::string get_title(const int pos) const;

    std::string get_title(const std::string& name) const;

    int add(const std::string& name, const int nb_columns);

    int add(const std::string& name, const int nbColumns, const std::string& def, std::vector<std::string>& vars, bool mode, bool files, bool date);

    int add(const std::string& name, const int nbColumns, const std::string& def, const std::string& lecs, bool mode, bool files, bool date);
};
