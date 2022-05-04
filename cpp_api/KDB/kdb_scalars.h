#pragma once

#include "kdb_abstract.h"
#include "kdb_abstract.cpp"


class KDBScalars : public KDBAbstract<Scalar>
{
protected:

    // CRUD (Create - Read - Update - Delete) + Copy methods

    void add_or_update(const std::string& name, const Scalar& Scalar) override;

    Scalar copy_obj(const Scalar& original) const override;

    Scalar get_unchecked(const int pos) const override;

public:
    KDBScalars() : KDBAbstract(I_SCALARS) {};

    void add(const std::string& name, const IODE_REAL value, const IODE_REAL relax);

    void update(const std::string& name, const IODE_REAL value, const IODE_REAL relax);
};
