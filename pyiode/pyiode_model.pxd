
cdef extern from "iode.h":
    cdef int    IodeModelSimulate()
    cdef int    IodeModelCalcSCC()
    cdef int    IodeModelSimulateSCC()
    cdef float  IodeModelSimNorm()
    cdef int    IodeModelSimNIter()
    cdef int    IodeModelSimCpu()
    cdef int    IodeModelCpuSort()
    cdef int    IodeModelCpuSCC()
