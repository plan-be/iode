# distutils: language = c++

from typing import List

from libcpp.string cimport string
from libcpp.vector cimport vector

from pyiode.common cimport L_NAN
from pyiode.common cimport I_NB_TYPES
from pyiode.common cimport vIodeTypes


def cpp_string_vector_to_py(vector[string] vec) -> List[str]:
    return [value.decode() for value in vec]


# IODE CONSTANTS
# --------------
nan: float = L_NAN
NB_TYPES: int = I_NB_TYPES

IODE_TYPES_LIST = cpp_string_vector_to_py(vIodeTypes)
