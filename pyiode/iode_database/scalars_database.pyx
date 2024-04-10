# distutils: language = c++

from collections.abc import Iterable
from typing import Union, Tuple, List, Dict, Optional, Any
if sys.version_info.minor >= 11:
    from typing import Self
else:
    Self = Any

cimport cython
from pyiode.iode_database.database cimport KDBScalars as CKDBScalars
from pyiode.iode_database.database cimport Scalars as cpp_global_scalars

ScalarInput = Union[int, float, List[float], Tuple[float, float], Dict[str, float], Scalar]


@cython.final
cdef class Scalars(_AbstractDatabase):
    cdef CKDBScalars* database_ptr

    def __cinit__(self, filepath: str = None) -> Scalars:
        """
        Get an instance of the IODE Scalars database. 
        Load the IODE scalars from 'filepath' if given.

        Parameters
        ----------
        filepath: str, optional
            file containing the IODE scalars to load.

        Returns
        -------
        Scalars

        Examples
        --------
        >>> from iode import Scalars, SAMPLE_DATA_DIR
        >>> scl_db = Scalars(f"{SAMPLE_DATA_DIR}/fun.scl")
        >>> len(scl_db)
        161
        """
        self.database_ptr = self.abstract_db_ptr = &cpp_global_scalars
        if filepath is not None:
            self.load(filepath)

    def __dealloc__(self):
        # self.database_ptr points to the C++ global instance Scalars 
        # which does not need to be manually deleted 
        pass

    # TODO: implement KDBAbstract::load() method (for global KDB only)
    def _load(self, filepath: str):
        cdef CKDBScalars* kdb = new CKDBScalars(filepath.encode())
        del kdb

    def subset(self, pattern: str, copy: bool = False) -> Scalars:
        subset_ = Scalars()
        subset_.database_ptr = subset_.abstract_db_ptr = self.database_ptr.subset(pattern.encode(), <bint>copy)
        return subset_

    def _get_object(self, key):
        if not isinstance(key, str):
            raise TypeError(f"Cannot get object {key}.\nExpected a string value for {key} " + 
                "but got value of type {type(filepath).__name__}")
        key = key.strip()

        cdef CScalar c_scalar = self.database_ptr.get(key.encode())
        py_scalar = Scalar(c_scalar.val, c_scalar.relax)
        py_scalar.c_scalar.std = c_scalar.std
        return py_scalar

    def _set_object(self, key, value):
        if not isinstance(key, str):
            raise TypeError(f"Cannot set object {key}.\nExpected a string value for {key} " + 
                "but got value of type {type(filepath).__name__}")
        key = key.strip()
        
        if isinstance(value, int):
            value = float(value) 

        if self.database_ptr.contains(key.encode()):
            # update exisiting scalar
            if isinstance(value, float):
                scalar = self._get_object(key)
                scalar.value = value
            elif isinstance(value, Scalar):
                scalar = value
            elif isinstance(value, (tuple, list)) and all(isinstance(elem, (int, float)) for elem in value):
                if len(value) > 2:
                    raise ValueError(f"Update scalar '{key}': Expected input to be a tuple or list of length 2. "
                                     f"Got {type(value).__name__} of length {len(value)}")
                scalar = Scalar(float(value[0]), float(value[1]))
            elif isinstance(value, dict):
                scalar = self._get_object(key)
                scalar.value = value.pop('value', scalar.value)
                scalar.relax = value.pop('relax', scalar.relax)
                if len(value):
                    raise ValueError(f"Update scalar '{key}': only 'value' and 'relax' keys are accepted. "
                                     f"Got unknown key(s): {';'.join(value.keys())}")
            else:
                raise TypeError(f"Update scalar '{key}': Expected input to be of type float or tuple(float, float) "
                                f"or list(float, float) or dict(str, float) or Scalar. Got value of type {type(value).__name__}")
            self.database_ptr.update(key.encode(), scalar.value, scalar.relax, scalar.std)
        else:
            # add a new scalar
            if isinstance(value, float):
                scalar = Scalar(value)
            elif isinstance(value, Scalar):
                scalar = value
            elif isinstance(value, (tuple, list)) and all(isinstance(elem, (int, float)) for elem in value):
                if len(value) > 2:
                    raise ValueError(f"New scalar '{key}': Expected input to be a tuple or list of length 2. "
                                     f"Got {type(value).__name__} of length {len(value)}")
                scalar = Scalar(float(value[0]), float(value[1]))
            elif isinstance(value, dict):
                scalar = Scalar(**value)
            else:
                raise TypeError(f"New scalar '{key}': Expected input to be of type float or tuple(float, float) "
                                f"or list(float, float) or dict(str, float) or Scalar. Got value of type {type(value).__name__}")
            self.database_ptr.add(key.encode(), scalar.value, scalar.relax, scalar.std)
