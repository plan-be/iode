#pragma once

#include "cpp_api/iode_cpp_api.h"

#include <nanobind/nanobind.h>
#include <nanobind/stl/string.h>
#include <nanobind/ndarray.h>

namespace nb = nanobind;


/*
 * nanobind supports zero-copy exchange using two protocols:
 * - The classic buffer protocol.
 * - DLPack, a GPU-compatible generalization of the buffer protocol.
 * 
 * see https://nanobind.readthedocs.io/en/latest/ndarray.html for documentation
 */

using numpy_var = nb::ndarray<nb::numpy, IODE_REAL, nb::ndim<1>>;

/** Get an IODE variable in a numpy ndarray
 * see https://nanobind.readthedocs.io/en/latest/ndarray.html#binding-functions-that-return-arrays 
 *     https://nanobind.readthedocs.io/en/latest/ndarray.html#constraint-types 
 *     https://nanobind.readthedocs.io/en/latest/api_extra.html#array-annotations 
 */
inline numpy_var get_var_as_ndarray(const std::string& name)
{
    size_t nb_obs = (size_t) Variables.get_nb_periods();
    if(nb_obs == 0)
        return numpy_var(NULL, { 0 });

    IODE_REAL* var_ptr = Variables.get_var_ptr(name);
    return numpy_var(var_ptr, { nb_obs });
}
