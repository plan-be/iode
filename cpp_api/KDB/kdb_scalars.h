#pragma once

#include "kdb_abstract.h"


class KDBScalars : public KDBAbstract
{

public:
    KDBScalars() : KDBAbstract(I_SCALARS) {};

    Scalar* get(const int pos) const
    {
        return KSVAL(getKDB(), pos);
    }

    Scalar* get(const std::string name) const
    {
        int pos = getPosition(name);
        return get(pos);
    }

};
