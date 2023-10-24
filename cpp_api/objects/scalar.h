#pragma once
#include "common.h"
#include "utils/utils.h"
#include <boost/functional/hash.hpp>


struct Scalar : public SCL
{
public:
    Scalar();

    Scalar(const int pos, KDB* kdb = nullptr);

    Scalar(const std::string& name, KDB* kdb = nullptr);

    Scalar(const IODE_REAL value, const IODE_REAL relax=1.0, const IODE_REAL std=L_NAN);

    Scalar(const Scalar& scalar);

    std::string to_string() const;

    // required to be used in std::map
    Scalar& operator=(const Scalar& scalar);

    bool operator==(const Scalar& other) const;
};


/**
 * @brief compute a hash value for a scalar.
 * 
 * @note see https://www.boost.org/doc/libs/1_55_0/doc/html/hash/custom.html
 *       and https://www.boost.org/doc/libs/1_55_0/doc/html/hash/combine.html
 * 
 * @return std::size_t 
 */
std::size_t hash_value(Scalar const& scalar);

/**
 * @brief compute a hash value for an object of type SCL (C API).
 * 
 * @note see https://www.boost.org/doc/libs/1_55_0/doc/html/hash/custom.html
 *       and https://www.boost.org/doc/libs/1_55_0/doc/html/hash/combine.html
 * 
 * @return std::size_t 
 */
std::size_t hash_value(SCL const& scl);
