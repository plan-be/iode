# distutils: language = c++

from collections.abc import Iterable
from typing import Union, Tuple, List, Dict, Optional

# Import necessary C++ types/classes
from libcpp.string cimport string
from libcpp.vector cimport vector

from pyiode.common cimport EnumIodeAdjustmentMethod
from pyiode.iode_database.cpp_api_database cimport KDBScalars as CKDBScalars


cdef extern from "cpp_api/compute/estimation.h":
    string dynamic_adjustment(const EnumIodeAdjustmentMethod method, const string& eqs, const string& c1, const string& c2)
    CKDBScalars* dickey_fuller_test(const string& lec, bint drift, bint trend, int order)

