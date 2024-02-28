from libcpp.string cimport string
from libcpp.vector cimport vector


cdef extern from "iode.h":
    cdef double L_NAN
    cdef int I_NB_TYPES


cdef extern from "common.h":
    cdef enum EnumIodeType:
        I_COMMENTS,
        I_EQUATIONS,
        I_IDENTITIES,
        I_LISTS,
        I_SCALARS,
        I_TABLES,
        I_VARIABLES

    cdef vector[string] vIodeTypes
