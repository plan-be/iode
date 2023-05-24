#pragma once
#include "common.h"
#include "utils/utils.h"
#include <boost/functional/hash.hpp>


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


/**
 * @brief compute a hash value for a scalar.
 * 
 * @note see https://www.boost.org/doc/libs/1_55_0/doc/html/hash/custom.html
 *       and https://www.boost.org/doc/libs/1_55_0/doc/html/hash/combine.html
 * 
 * @return std::size_t 
 */
std::size_t hash_value(SCL const& c_scalar);
