#pragma once

#include "kdb_abstract.h"
#include "kdb_abstract.cpp"
#include "../table.h"

class KDBTables : public KDBAbstract<Table>
{
protected:

    // CRUD (Create - Read - Update - Delete) + Copy methods

    void add_or_update(const std::string& name, const Table& table) override;

    Table copy_obj(const Table& original) const override;

    Table get_unchecked(const int pos) const override;

public:
    KDBTables() : KDBAbstract(I_TABLES) {};

    std::string get_title(const int pos) const;

    std::string get_title(const std::string& name) const;

    Table add(const std::string& name, const int nb_columns);

    Table add(const std::string& name, const int nbColumns, const std::string& def, std::vector<std::string>& vars, bool mode, bool files, bool date);
};
