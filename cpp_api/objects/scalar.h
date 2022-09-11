#pragma once
#include "common.h"
#include "utils/utils.h"


struct Scalar
{
    SCL* c_scalar;
    bool allocated;

public:
    Scalar();

    Scalar(const int pos, KDB* kdb = nullptr);

    Scalar(const std::string& name, KDB* kdb = nullptr);

    Scalar(const IODE_REAL value, const IODE_REAL relax=1.0, const IODE_REAL std=L_NAN);

    Scalar(const Scalar& scl);

    ~Scalar();

    // required to be used in std::map
    Scalar& operator=(const Scalar& scl);

    bool operator==(const Scalar& other) const;

    IODE_REAL& value() { return c_scalar->val; }

    IODE_REAL& relax() { return c_scalar->relax; }

    IODE_REAL& std() { return c_scalar->std; }
};
