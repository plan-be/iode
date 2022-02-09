#pragma once

#include "kdb_abstract.h"
#include "../table.h"

class KDBTables : public KDBAbstract
{

public:
    KDBTables() : KDBAbstract(I_TABLES) {};

    Table get(const int pos) const
    {
        TBL* c_table = KTVAL(getKDB(), pos);
        return Table(c_table);
    }

    Table get(const std::string name) const
    {
        int pos = getPosition(name);
        return get(pos);
    }

    std::string getTitle(const int pos) const
    {
        TBL* c_table = KTVAL(getKDB(), pos);
        std::string title_oem = std::string((char*) T_get_title(c_table));
        std::string title = convert_oem_to_utf8(title_oem);
        T_free(c_table);
        return title;
    }

    std::string getTitle(const std::string name) const
    {
        int pos = getPosition(name);
        return getTitle(pos);
    }
};
