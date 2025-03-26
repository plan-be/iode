from collections.abc import Iterable
from typing import Union, Tuple, List, Dict, Optional, Any
if sys.version_info.minor >= 11:
    from typing import Self
else:
    Self = Any

cimport cython
from cython.operator cimport dereference
from pyiode.iode_database.cpp_api_database cimport hash_value
from pyiode.iode_database.cpp_api_database cimport KDBScalars as CKDBScalars
from pyiode.iode_database.cpp_api_database cimport Scalars as cpp_global_scalars
from pyiode.iode_database.cpp_api_database cimport KCPTR, KIPTR, KLPTR, KVPTR

import pandas as pd


cdef class Scalars(IodeDatabase):
    cdef bint ptr_owner
    cdef CKDBScalars* database_ptr

    def __cinit__(self, filepath: str=None) -> Scalars:
        self.database_ptr = NULL
        self.abstract_db_ptr = NULL
        self.ptr_owner = False

    def __dealloc__(self):
        if self.ptr_owner and self.database_ptr is not NULL:
            del self.database_ptr
            self.database_ptr = NULL

    @staticmethod
    cdef Scalars _from_ptr(CKDBScalars* database_ptr = NULL, bint owner=False):
        # call to __new__() that bypasses the __init__() constructor.
        cdef Scalars wrapper = Scalars.__new__(Scalars)
        if database_ptr is not NULL:
            wrapper.ptr_owner = owner
            wrapper.database_ptr = database_ptr
            wrapper.abstract_db_ptr = database_ptr
        else:
            wrapper.ptr_owner = False
            wrapper.database_ptr = &cpp_global_scalars
            wrapper.abstract_db_ptr = &cpp_global_scalars
        return wrapper

    @staticmethod
    def __init_instance(instance: Scalars) -> Self:
        instance.ptr_owner = False
        instance.database_ptr = &cpp_global_scalars
        instance.abstract_db_ptr = &cpp_global_scalars
        return instance

    def _load(self, filepath: str):
        cdef CKDBScalars* kdb = new CKDBScalars(filepath.encode())
        del kdb

    def _subset(self, pattern: str, copy: bool) -> Scalars:
        subset_db: Scalars = self._new_instance()
        subset_db.database_ptr = subset_db.abstract_db_ptr = self.database_ptr.subset(pattern.encode(), <bint>copy)
        return subset_db

    def _get_object(self, key: Union[str, int]) -> Scalar:
        cdef CScalar* c_scalar
        if isinstance(key, int):
            c_scalar = self.database_ptr.get(<int>key)
        else:
            key = key.strip()
            c_scalar = self.database_ptr.get(<string>(key.encode()))
        # self.database_ptr.get() does not allocate a new C++ Scalar instance
        py_scalar = Scalar._from_ptr(c_scalar, <bint>False) 
        return py_scalar

    def _set_object(self, key: Union[str, int], value):
        if isinstance(key, str):
            key = key.strip()
        
        if isinstance(value, int):
            value = float(value) 

        if isinstance(value, pd.Series):
            value = value.to_dict()

        # update a scalar
        if isinstance(key, int) or self.database_ptr.contains(key.encode()):
            if isinstance(value, float):
                scalar = self._get_object(key)
                scalar.value = value
            elif isinstance(value, Scalar):
                scalar = value
            elif isinstance(value, (tuple, list)) and all(isinstance(elem, (int, float)) for elem in value):
                if len(value) > 2:
                    raise ValueError(f"Cannot update scalar '{key}': Expected input to be a tuple or list of length 2. "
                                    f"Got {type(value).__name__} of length {len(value)}")
                scalar = self._get_object(key)
                scalar.value = float(value[0])
                scalar.relax = float(value[1])
            elif isinstance(value, dict):
                scalar = self._get_object(key)
                scalar.value = value.pop('value', scalar.value)
                scalar.relax = value.pop('relax', scalar.relax)
                if len(value):
                    raise ValueError(f"Cannot update scalar '{key}': only 'value' and 'relax' keys are accepted. "
                                     f"Got unknown key(s): {';'.join(value.keys())}")
            else:
                raise TypeError(f"Cannot update scalar '{key}': Expected input to be of type float or tuple(float, float) "
                                f"or list(float, float) or dict(str, float) or Scalar. Got value of type {type(value).__name__}")

            if isinstance(key, int):
                self.database_ptr.update(<int>key, <double>scalar.value, <double>scalar.relax, <double>scalar.std)
            else:    
                self.database_ptr.update(<string>(key.encode()), <double>scalar.value, <double>scalar.relax, <double>scalar.std)
        # add a new scalar
        else:
            if isinstance(value, float):
                scalar = Scalar(value)
            elif isinstance(value, Scalar):
                scalar = value
            elif isinstance(value, (tuple, list)) and all(isinstance(elem, (int, float)) for elem in value):
                if len(value) > 2:
                    raise ValueError(f"Cannot add scalar '{key}': Expected input to be a tuple or list of length 2. "
                                    f"Got {type(value).__name__} of length {len(value)}")
                scalar = Scalar(float(value[0]), float(value[1]))
            elif isinstance(value, dict):
                scalar = Scalar(**value)
            else:
                raise TypeError(f"Cannot add scalar '{key}': Expected input to be of type float or tuple(float, float) "
                                f"or list(float, float) or dict(str, float) or Scalar. Got value of type {type(value).__name__}")

            self.database_ptr.add(key.encode(), scalar.value, scalar.relax, scalar.std)

    def copy_from(self, input_files: Union[str, List[str]], names: Union[str, List[str]]='*'):
        if not (self.is_global_workspace or self.is_detached):
            raise RuntimeError("Cannot call 'copy_from' method on a subset of a workspace")
        input_files, names = self._copy_from(input_files, names)
        self.database_ptr.copy_from(input_files.encode(), names.encode())

    def __hash__(self) -> int:
        return hash_value(dereference(self.database_ptr))


