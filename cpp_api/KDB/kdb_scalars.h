#pragma once
#include "kdb_template.h"


class KDBScalars : public KDBTemplate<Scalar>
{
protected:

    Scalar copy_obj(const Scalar& original) const override;

    Scalar get_unchecked(const int pos) const override;

public:
    KDBScalars(const std::string& pattern = "", const bool shallow_copy=true) : 
        KDBTemplate(I_SCALARS, pattern, shallow_copy) {};

    int add(const std::string& name, const IODE_REAL value, const IODE_REAL relax, const IODE_REAL std=L_NAN);

    void update(const std::string& name, const IODE_REAL value, const IODE_REAL relax, const IODE_REAL std = L_NAN);

    void update(const int pos, const IODE_REAL value, const IODE_REAL relax, const IODE_REAL std = L_NAN);
};
