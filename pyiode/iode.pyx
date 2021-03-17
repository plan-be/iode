import sys

import numpy as np
cimport numpy as np
import cython
import larray as la

from libc.stdlib cimport free, malloc
from libc.stdio cimport printf
from cpython cimport PyObject, Py_INCREF

# Numpy must be initialized before we can use it safely.
np.import_array()

cdef extern from "iode.h":
    cdef char* IODE_VERSION

cdef extern from "iodeapi.h":
    cdef void IodeInit()
    cdef int IodeLoad(char *filename, int filetype)
    cdef int IodeSave(char *filename, int filetype)
    cdef int B_ReportLine(char *repline)
    cdef int B_EqsEstimate(char *arg)
    cdef double *IodeGetVector(char *name, int *lg)
    cdef int IodeGetMaxt()
    cdef double *IodeGetScls(char *name) 
    cdef double *IodeGetSampleVector(int *lg)
    cdef char   *IodeGetSampleString()
    cdef int    IodeGetSamplePeriodicity()
    cdef char   *IodeGetLst(char *name)
    cdef char   *IodeGetCmt(char *name)
    cdef char   **IodeContents(char *pattern, int objtype)
    cdef char   *IodeVersion()

cdef iodevar_to_ndarray(char * name, int inmemory):
    cdef np.npy_intp shape[1]
    cdef int lg
    cdef double *data_ptr = IodeGetVector(name, &lg)
    
    shape[0] = <np.npy_intp> lg
    if data_ptr != NULL:
        vararray = np.PyArray_SimpleNewFromData(1, shape, np.NPY_DOUBLE, data_ptr)
        if(not inmemory):
            vararray = vararray.copy()
        vararray[vararray < -1.0e37] = np.nan
    else:
        vararray = np.ndarray(lg, dtype='double')        
        vararray.fill(0)        
    return vararray

cdef iodesample_to_ndarray():
    cdef np.npy_intp shape[1]
    cdef int lg
    cdef double *data_ptr = IodeGetSampleVector(&lg)
    
    shape[0] = <np.npy_intp> lg
    if data_ptr != NULL:
        vararray = np.PyArray_SimpleNewFromData(1, shape, np.NPY_DOUBLE, data_ptr)
        vararray = vararray.copy()
        vararray[vararray < -1.0e37] = np.nan
    else:
        vararray = np.ndarray(lg, dtype='double')        
        vararray.fill(0)        
    return vararray

cdef iodescl_to_ndarray(char * name):
    cdef np.npy_intp shape[1]
    cdef int lg = 3
    cdef double *data_ptr = IodeGetScls(name)
    
    shape[0] = <np.npy_intp> lg

    if data_ptr != NULL:
        vararray = np.PyArray_SimpleNewFromData(1, shape, np.NPY_DOUBLE, data_ptr)
        vararray = vararray.copy()
        vararray[vararray < -1.0e37] = np.nan
    else:
        vararray = np.ndarray(lg, dtype='double')        
        vararray.fill(0)        
    return vararray

IodeInit()

def cstr(pystr):
    return pystr.encode("cp1252")

def pystr(cstr):
    return cstr.decode("cp1252")
	
def version():
    "Return the Iode version"
    return pystr(IODE_VERSION)

def samplelength():
    return IodeGetMaxt()

def samplelist():
    vararray = iodesample_to_ndarray()
    return(vararray)

def wsload(filename, filetype):
    "Load iode file"
    return IodeLoad(cstr(filename), filetype)    

def wsloadvar(filename):
    "Load iode variable file"
    return wsload(filename,6)
    
def wsloadlst(filename):
    "Load iode list file"
    return wsload(filename,3)
   
def wssave(filename, filetype):
    return IodeSave(cstr(filename), filetype)    
    
def wssavevar(filename):
    return wssave(filename,6)  
    
def eqsestimate(arg):
    return B_EqsEstimate(cstr(arg))  
   
#report functions

def report(filename):
    fi = open(filename)
    for line in fi:
        reportline(line)
    fi.close() # not really needed

def reportline(repline):
   return(B_ReportLine(cstr(repline)))


#object handling
def getvector(name, inmemory = 0):
    vararray = iodevar_to_ndarray(cstr(name), inmemory)
    return(vararray)
       
def getscl(name):
    vararray = iodescl_to_ndarray(cstr(name))
    return vararray

def getlst(name):
    str = IodeGetLst(cstr(name))
    return pystr(str)

def getcmt(name):
    str = IodeGetCmt(cstr(name))
    return str.decode("cp850")

def samplestr():
    cdef char* c_str = IodeGetSampleString()
    try:
        py_str = pystr(c_str)
    finally:
        free(c_str)
    return py_str
    
    
# def contents(pattern, objtype):
    # cdef char **cnt = IodeContents(cstr(pattern), objtype)
    # cdef int i
    # res = []
    # if cnt != NULL:
        # i = 0
        # while cnt[i] != NULL:
            # s = bytes(cnt[i])
            # res.append(pystr(s))
            # i = i + 1
    # return res

def contents(pattern, objtype):
    cdef char **cnt = IodeContents(cstr(pattern), objtype)
    cdef int nb
    
    res = []
    if cnt != NULL:
        nb = 0
        while cnt[nb] != NULL:
            nb = nb + 1
        res = [None] * nb
        
        nb = 0
        while cnt[nb] != NULL:
            s = bytes(cnt[nb])
            res[nb] = pystr(s)
            nb = nb + 1
            
    return res

    
def wsloadvarpy(filename, pattern = '*', varsaxis = 'vars', timeaxis = 'time'):
    rc = wsloadvar(filename)
    vars = contents(pattern, 6)
    var = la.Axis(vars, varsaxis)
    time = la.Axis(samplelist(), timeaxis)
    #var = la.Axis('vars', vars)
    #time = la.Axis('time', samplelist())

    la_res = la.zeros([var, time])
    for v in vars:
        la_res[v] = getvector(v)
    return la_res


class scl:
    def __init__(self, name):
        self.name = name
        iodescl = iodescl_to_ndarray(cstr(name))
        self.value = iodescl[0]
        self.stderr = iodescl[1]
        self.relax = iodescl[2]

    def __str__(self):
        return 'Scalar (%s, %s, %s, %s)' % (self.name, self.value, self.stderr, self.relax)

class cmt:
    def __init__(self, name):
        self.name = name
        self.cmt = getcmt(name)

    def __str__(self):
        return 'Comment (%s, %s)' % (self.name, self.cmt)

