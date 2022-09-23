#pragma once
#include "kdb_template.h"
#include "objects/scalar.h"


class KDBScalars : public KDBTemplate<Scalar>
{
protected:

    Scalar copy_obj(const Scalar& original) const override;

    Scalar get_unchecked(const int pos) const override;

public:
    KDBScalars(const EnumIodeKDBType kdb_type = KDB_GLOBAL, const std::string& pattern = "") : 
        KDBTemplate(kdb_type, I_SCALARS, pattern) {};

    int add(const std::string& name, const IODE_REAL value, const IODE_REAL relax, const IODE_REAL std=L_NAN);

    void update(const std::string& name, const IODE_REAL value, const IODE_REAL relax, const IODE_REAL std = L_NAN);

    void update(const int pos, const IODE_REAL value, const IODE_REAL relax, const IODE_REAL std = L_NAN);
};
