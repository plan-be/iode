#pragma once

#include "kdb_abstract.h"


class KDBIdentities : public KDBAbstract
{

public:
    KDBIdentities() : KDBAbstract(I_IDENTITIES) {};

    std::string getLec(const int pos) const 
    { 
        return std::string(KILEC(getKDB(), pos));
    }

    std::string getLec(const std::string name) const
    {
        int pos = getPosition(name);
        return getLec(pos);
    }

    CLEC* getCLec(const int pos) const
    {
        return KICLEC(getKDB(), pos);
    }

    CLEC* getCLec(const std::string name) const
    {
        int pos = getPosition(name);
        return getCLec(pos);
    }
};
