#pragma once

#include "api/constants.h"
#include "api/utils/utils.h"
#include "api/objs/kdb.h"
#include "api/objs/pack.h"


/*----------------------- STRUCTS ----------------------------*/

// Scalar (struct containing a scalar value, and if estimated, 
//         its relaxation parameter and its standard error).
struct Scalar 
{
    double    value;        // Value of the scalar
    double    relax;        // Relaxation parameter. Used only for estimation.
			                // If null, the parameter will not be estimated.
			                // If 0 < relax < 1, the convergence of the estimation will be slowed down. To use when the estimation process diverges.
    double    std;          // Standard error. Result of the estimation.

public:
    Scalar(const double value=0.0, const double relax=1.0, const double std=IODE_NAN)
    {
        this->value = value;
        this->relax = relax;
        this->std = std;
    }

    Scalar(const Scalar& scalar)
    {
        this->value = scalar.value;
        this->relax = scalar.relax;
        this->std = scalar.std;
    }

    // required to be used in std::map
    Scalar& operator=(const Scalar& scalar)
    {
        this->value = scalar.value;
        this->relax = scalar.relax;
        this->std = scalar.std;
        return *this;
    }

    bool operator==(const Scalar& other) const
    {
        bool eq = this->value == other.value;
        eq &= this->relax == other.relax;
        eq &= this->std == other.std;
        return eq;
    }
};


// Custom specialization of std::hash can be injected in namespace std.
template<>
struct std::hash<Scalar>
{
    std::size_t operator()(const Scalar& scalar) const noexcept
    {
		std::size_t seed = 0;

        hash_combine<double>(seed, scalar.value);
        hash_combine<double>(seed, scalar.relax);
        hash_combine<double>(seed, scalar.std);

        return seed;
    }
};

/*----------------------- FUNCTIONS ----------------------------*/

std::size_t hash_value(const Scalar& scalar);


inline Scalar* KSVAL(KDB* kdb, const std::string& name) 
{         
    return (Scalar*) K_optr0(kdb, (char*) name.c_str());
}

inline Scalar* KSVAL(KDB* kdb, SWHDL handle) 
{         
    return (Scalar*) P_get_ptr(SW_getptr(handle), 0); 
}
