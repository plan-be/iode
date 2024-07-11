#include "scalar.h"


Scalar::Scalar()
{
    this->val = 0.0;
    this->relax = 1.0;
    this->std = IODE_NAN;
}

Scalar::Scalar(const double value, const double relax, const double std)
{
    this->val = value;
    this->relax = relax;
    this->std = std;
}

Scalar::Scalar(const SCL* c_scalar)
{
    this->val = c_scalar->val;
    this->relax = c_scalar->relax;
    this->std = c_scalar->std;
}

Scalar::Scalar(const Scalar& scalar)
{
    this->val = scalar.val;
    this->relax = scalar.relax;
    this->std = scalar.std;
}

// required to be used in std::map
Scalar& Scalar::operator=(const Scalar& scalar)
{
    val = scalar.val;
    relax = scalar.relax;
    std = scalar.std;
    return *this;
}

bool Scalar::operator==(const Scalar& other) const
{
    bool eq = val == other.val;
    eq &= relax == other.relax;
    eq &= std == other.std;
    return eq;
}

std::size_t hash_value(const Scalar& scalar)
{
    std::hash<SCL> scl_hash;
    return scl_hash(scalar);
}
