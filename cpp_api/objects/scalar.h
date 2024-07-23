#pragma once
#include "cpp_api/common.h"
#include "cpp_api/utils/utils.h"


struct Scalar : public SCL
{
public:
    Scalar();

    Scalar(const double value, const double relax=1.0, const double std=IODE_NAN);

    Scalar(const SCL* c_scalar);
    Scalar(const Scalar& scalar);

    // required to be used in std::map
    Scalar& operator=(const Scalar& scalar);

    bool operator==(const Scalar& other) const;
};


// Custom specialization of std::hash can be injected in namespace std.
template<>
struct std::hash<SCL>
{
    std::size_t operator()(const SCL& scalar) const noexcept
    {
		std::size_t seed = 0;

        hash_combine<double>(seed, scalar.val);
        hash_combine<double>(seed, scalar.relax);
        hash_combine<double>(seed, scalar.std);

        return seed;
    }
};

std::size_t hash_value(const Scalar& scalar);
