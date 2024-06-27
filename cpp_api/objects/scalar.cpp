#include "scalar.h"


Scalar::Scalar()
{
    this->val = 0.0;
    this->relax = 1.0;
    this->std = L_NAN;
}

Scalar::Scalar(const int pos, KDB* kdb)
{
    if (!kdb) 
        kdb = K_WS[SCALARS];

    if (pos < 0 || pos > kdb->k_nb)
    {
        IodeExceptionInvalidArguments error("Cannot extract Scalar", "Scalar position must be in range [0, " + 
            std::to_string(kdb->k_nb - 1) + "])");
        error.add_argument("scalar position", std::to_string(pos));
        throw error;
    }

    // Note: KSVAL does not allocate a new pointer SCL*
    SCL* c_scalar = KSVAL(kdb, pos);
    this->val = c_scalar->val;
    this->relax = c_scalar->relax;
    this->std = c_scalar->std;
}

Scalar::Scalar(const std::string& name, KDB* kdb)
{
    if (!kdb) 
        kdb = K_WS[SCALARS];

    int pos = K_find(kdb, to_char_array(name));
    if (pos < 0) 
        throw IodeExceptionFunction("Cannot extract Scalar", "Scalar with name " + name + " does not exist.");
    
    // Note: KSVAL does not allocate a new pointer SCL*
    SCL* c_scalar = KSVAL(kdb, pos);
    this->val = c_scalar->val;
    this->relax = c_scalar->relax;
    this->std = c_scalar->std;
}

Scalar::Scalar(const double value, const double relax, const double std)
{
    this->val = value;
    this->relax = relax;
    this->std = std;
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

std::size_t hash_value(Scalar const& scalar)
{
	std::size_t seed = 0;

	boost::hash_combine(seed, scalar.val);
	boost::hash_combine(seed, scalar.relax);
	boost::hash_combine(seed, scalar.std);

    return seed;
}

std::size_t hash_value(SCL const& scl)
{
	std::size_t seed = 0;

	boost::hash_combine(seed, scl.val);
	boost::hash_combine(seed, scl.relax);
	boost::hash_combine(seed, scl.std);

    return seed;
}
