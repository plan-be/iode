#pragma once

#include "kdb_abstract.h"
#include "../table.h"

class KDBTables : public KDBAbstract
{

public:
    KDBTables() : KDBAbstract(I_TABLES) {};

    Table* get(const int pos) const;

    Table* get(const std::string name) const;

    std::string getTitle(const int pos) const;

    std::string getTitle(const std::string name) const;

    Table* add_table(const std::string name, const int nb_columns);

    Table* add_table(const std::string name, const int nbColumns, const std::string def, std::vector<std::string> vars, bool mode, bool files, bool date);

    Table* copy(const std::string name, const std::string original_table_name);

    void remove_table(const int pos);

    void remove_table(const std::string name);
};
