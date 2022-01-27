#pragma once

#include "kdb_abstract.h"
#include "../period.h"


class KDBVariables : public KDBAbstract
{

public:
    KDBVariables() : KDBAbstract(I_VARIABLES) {};

    Variable get(const int pos, const int t, const int mode) const
    {
        return KV_get(getKDB(), pos, t, mode);
    }

    Variable get(const std::string name, const int t, const int mode) const
    {
        int pos = getPosition(name);
        return get(pos, t, mode);
    }

    int getNbPeriods() const 
    { 
        return KSMPL(getKDB())->s_nb; 
    }

    Period getPeriod(const int t) const
    {
        PERIOD period = KSMPL(getKDB())->s_p1;
        return Period(&period).shift(t);
    }
};
