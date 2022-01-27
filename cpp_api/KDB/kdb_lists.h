#pragma once

#include "kdb_abstract.h"


class KDBLists : public KDBAbstract
{

public:
    KDBLists() : KDBAbstract(I_LISTS) {};

    List get(const int pos) const
    {
        return std::string(KLVAL(getKDB(), pos));
    }

    List get(const std::string name) const
    {
        int pos = getPosition(name);
        return get(pos);
    }

};
