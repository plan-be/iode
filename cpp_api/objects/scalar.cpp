#include "scalar.h"


Scalar::Scalar()
{
    c_scalar = nullptr;
    allocated = false;
}

Scalar::Scalar(const int pos, KDB* kdb)
{
    if (!kdb) kdb = K_WS[I_SCALARS];
    if (pos < 0 || pos > kdb->k_nb)
    {
        IodeExceptionInvalidArguments error("Cannot extract Scalar", "Scalar position must be in range [0, " + 
            std::to_string(kdb->k_nb - 1) + "])");
        error.add_argument("equation position", std::to_string(pos));
        throw error;
    }
    // Note: KSVAL does not allocate a new pointer SCL*
    c_scalar = KSVAL(kdb, pos);
    allocated = false;
}

Scalar::Scalar(const std::string& name, KDB* kdb)
{
    if (!kdb) kdb = K_WS[I_SCALARS];
    int pos = K_find(kdb, to_char_array(name));
    if (pos < 0) throw IodeExceptionFunction("Cannot extract Scalar", "Scalar with name " + name + " does not exist.");
    // Note: KSVAL does not allocate a new pointer SCL*
    c_scalar = KSVAL(kdb, pos);
    allocated = false;
}

Scalar::Scalar(const IODE_REAL value, const IODE_REAL relax, const IODE_REAL std)
{
    c_scalar = (SCL*) SW_nalloc(sizeof(SCL));
    c_scalar->val = value;
    c_scalar->relax = relax;
    c_scalar->std = std;
    allocated = true;
}

Scalar::Scalar(const Scalar& scl)
{
    c_scalar = (SCL*) SW_nalloc(sizeof(SCL));
    c_scalar->val = scl.c_scalar->val;
    c_scalar->relax = scl.c_scalar->relax;
    c_scalar->std = scl.c_scalar->std;
    allocated = true;
}

Scalar::~Scalar()
{
    if (allocated) SW_nfree(c_scalar);
}

// required to be used in std::map
Scalar& Scalar::operator=(const Scalar& scl)
{
    c_scalar->val = scl.c_scalar->val;
    c_scalar->relax = scl.c_scalar->relax;
    c_scalar->std = scl.c_scalar->std;
    return *this;
}

bool Scalar::operator==(const Scalar& other) const
{
    bool eq = c_scalar->val == other.c_scalar->val;
    eq &= c_scalar->relax == other.c_scalar->relax;
    eq &= c_scalar->std == other.c_scalar->std;
    return eq;
}
