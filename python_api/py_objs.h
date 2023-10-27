#pragma once
#include <string>
#include <vector>
#include <exception>

#include "cpp_api/iode_cpp_api.h"
#include "py_numpy.h"

#include <nanobind/nanobind.h>
#include <nanobind/stl/string.h>
#include <nanobind/stl/array.h>
#include <nanobind/stl/vector.h>
#include <nanobind/stl/pair.h>
#include <nanobind/operators.h>

namespace nb = nanobind;


void init_iode_objs(nb::module_ &m);
