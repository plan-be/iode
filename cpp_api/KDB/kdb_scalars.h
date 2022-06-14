#pragma once

#include "kdb_abstract.h"
#include "kdb_abstract.cpp"


class KDBScalars : public KDBAbstract<Scalar>
{
protected:

    // CRUD (Create - Read - Update - Delete) + Copy methods

    int add_or_update(const std::string& name, const Scalar& Scalar) override;

    Scalar copy_obj(const Scalar& original) const override;

    Scalar get_unchecked(const int pos) const override;

public:
    KDBScalars(const std::string& pattern = "") : KDBAbstract(I_SCALARS, pattern) {};

    void add(const std::string& name, const IODE_REAL value, const IODE_REAL relax, const IODE_REAL std=L_NAN);

    void update(const std::string& name, const IODE_REAL value, const IODE_REAL relax, const IODE_REAL std = L_NAN);
};
