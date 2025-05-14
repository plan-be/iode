from cython.operator cimport dereference

from typing import Union, Tuple, List, Optional, Any
try:
    import larray as la
    Axis = la.Axis
except ImportError:
    la = None
    Axis = Any

from pyiode.time.sample cimport CSample


cdef class Sample:
    cdef bint ptr_owner
    cdef CSample* c_sample

    def __init__(self, start_period: str, end_period: str):
        self.ptr_owner = True
        if not start_period:
            raise ValueError("String value of 'start_period' is empty")
        if not end_period:
            raise ValueError("String value of 'end_period' is empty")
        self.c_sample = new CSample(start_period.encode(), end_period.encode())

    def __cinit__(self):
        self.ptr_owner = False
        self.c_sample = NULL

    def __dealloc__(self):
        if self.ptr_owner and self.c_sample is not NULL:
            del self.c_sample
            self.c_sample = NULL

    @staticmethod
    cdef Sample _from_ptr(CSample* ptr, bint owner=False):
        """
        Factory function to create Sample objects from a given CSample pointer.
        """
        # Fast call to __new__() that bypasses the __init__() constructor.
        cdef Sample wrapper = Sample.__new__(Sample)
        wrapper.c_sample = ptr
        wrapper.ptr_owner = owner
        return wrapper

    def is_undefined(self) -> bool:
        cdef CPeriod c_period
        if self.c_sample is NULL:
            return True
        c_period = self.c_sample.start_period()
        if c_period.p_y == 0:
            return True
        c_period = self.c_sample.end_period()
        if c_period.p_y ==0:
            return True
        return False

    def index(self, period: str) -> int:
        if self.is_undefined():
            raise RuntimeError("'sample' is not defined")
        
        cdef string str_period = period.encode()
        return self.c_sample.get_period_position(str_period)

    def get_period_list(self, astype: str) -> Union[List[str], List[float]]:
        if self.is_undefined():
            warnings.warn("'sample' is not defined")
            return []       
        
        if astype == 'float':
            return self.c_sample.get_list_periods_as_float()
        elif astype == 'str':
            return [period.decode() for period in self.c_sample.get_list_periods()]
        else:
            raise ValueError(f"'astype': type {astype} is not allowed.")

    def intersection(self, other_sample: Sample) -> Sample:
        if self.is_undefined():
            raise RuntimeError("'sample' is not defined")
        if other_sample.is_undefined():
            raise RuntimeError("Passed sample to 'intersection(sample)' represents an empty sample")
        
        cdef CSample c_sample_inter = self.c_sample.intersection(dereference(other_sample.c_sample))
        return Sample._from_ptr(new CSample(c_sample_inter), <bint>True)

    def get_start(self) -> Period:
        if self.is_undefined():
            warnings.warn("'sample' is not defined")
            return None
        
        c_period = self.c_sample.start_period()
        return Period(c_period.to_string().decode())

    def get_end(self) -> Period:
        if self.is_undefined():
            warnings.warn("'sample' is not defined")
            return None
        
        c_period = self.c_sample.end_period()
        return Period(c_period.to_string().decode())

    def get_nb_periods(self) -> int:
        if self.is_undefined():
            warnings.warn("'sample' is not defined")
            return 0
        
        return self.c_sample.nb_periods()

    def __len__(self) -> int:
        if self.is_undefined():
            warnings.warn("'sample' is not defined")
            return 0
        
        return self.c_sample.nb_periods()

    def __eq__(self, other: Sample) -> bool:
        if self.is_undefined():
            warnings.warn("'sample' in 'sample == other' is not defined")
            return False
        elif other.is_undefined():
            warnings.warn("other sample in 'sample == other' is not defined")
            return False
        else:
            return str(self) == str(other)

    def __str__(self) -> str:
        if self.is_undefined():
            warnings.warn("'sample' is not defined")
            return str(None)
        else:
            return str(self.get_start()) + ":" + str(self.get_end())

    def __repr__(self) -> str:
        if self.is_undefined():
            warnings.warn("'sample' is not defined")
            return str(None)
        else:
            return f'Sample("{self.get_start()}:{self.get_end()}")'
