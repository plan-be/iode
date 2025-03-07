# distutils: language = c++

import warnings
from pathlib import Path
from collections.abc import Iterable
from typing import Union, Tuple, List, Optional, Any
if sys.version_info.minor >= 11:
    from enum import IntEnum, StrEnum
    from typing import Self
else:
    Self = Any
    from enum import Enum, IntEnum
    StrEnum = Enum

cimport cython
from cython.operator cimport dereference
from libc.string cimport memcpy
from libcpp.string cimport string
from libcpp.vector cimport vector
from pyiode.common cimport IODE_NAN, IodeVarMode, IodeLowToHigh, IodeHighToLow, VariablesInitialization
from pyiode.util cimport IODE_IS_A_NUMBER
from pyiode.time.period cimport CPeriod
from pyiode.time.sample cimport CSample
from pyiode.iode_database.cpp_api_database cimport KV_get, KV_set, KV_add, K_add
from pyiode.iode_database.cpp_api_database cimport _c_add_var_from_other, _c_copy_var_content
from pyiode.iode_database.cpp_api_database cimport BinaryOperation as CBinaryOperation
from pyiode.iode_database.cpp_api_database cimport _c_operation_scalar, _c_operation_one_var, _c_operation_one_period
from pyiode.iode_database.cpp_api_database cimport _c_operation_between_two_vars
from pyiode.iode_database.cpp_api_database cimport hash_value
from pyiode.iode_database.cpp_api_database cimport K_CMP_EPS
from pyiode.iode_database.cpp_api_database cimport B_DataCompareEps
from pyiode.iode_database.cpp_api_database cimport KDBVariables as CKDBVariables
from pyiode.iode_database.cpp_api_database cimport Variables as cpp_global_variables
from pyiode.iode_database.cpp_api_database cimport low_to_high as cpp_low_to_high
from pyiode.iode_database.cpp_api_database cimport high_to_low as cpp_high_to_low
from pyiode.iode_database.cpp_api_database cimport KCPTR, KIPTR, KLPTR, KVPTR, KVVAL
from pyiode.iode_database.cpp_api_database cimport B_FileImportVar
from pyiode.iode_database.cpp_api_database cimport EXP_RuleExport
from pyiode.iode_database.cpp_api_database cimport W_flush, W_close

from iode.util import check_filepath, split_list


class BinaryOperation(IntEnum):
    OP_ADD = CBinaryOperation.OP_ADD
    OP_SUB = CBinaryOperation.OP_SUB
    OP_MUL = CBinaryOperation.OP_MUL
    OP_DIV = CBinaryOperation.OP_DIV
    OP_POW = CBinaryOperation.OP_POW


cdef class CythonVariables(IodeDatabase):
    cdef bint ptr_owner
    cdef CKDBVariables* database_ptr
    cdef IodeVarMode mode_
    cdef Period first_period_subset
    cdef Period last_period_subset

    def __cinit__(self, filepath: str = None) -> Variables:
        self.database_ptr = NULL
        self.abstract_db_ptr = NULL
        self.ptr_owner = False
        self.mode_ = IodeVarMode.VAR_MODE_LEVEL
        self.first_period_subset = None
        self.last_period_subset = None

    def __dealloc__(self):
        # if self.database_ptr points to the C++ global instance Variables 
        # which does not need to be manually deleted 
        if self.ptr_owner and self.database_ptr is not NULL:
            del self.database_ptr
            self.database_ptr = NULL

    # see https://cython.readthedocs.io/en/stable/src/userguide/extension_types.html#instantiation-from-existing-c-c-pointers 
    @staticmethod
    cdef Variables _from_ptr(CKDBVariables* database_ptr = NULL, bint owner=False):
        cdef CSample* c_sample
        # call to __new__() that bypasses the __init__() constructor.
        cdef Variables wrapper = Variables.__new__(Variables)
        if database_ptr is not NULL:
            wrapper.ptr_owner = owner
            wrapper.database_ptr = database_ptr
            wrapper.abstract_db_ptr = database_ptr
        else:
            wrapper.ptr_owner = False
            wrapper.database_ptr = &cpp_global_variables
            wrapper.abstract_db_ptr = &cpp_global_variables
        wrapper.mode_ = IodeVarMode.VAR_MODE_LEVEL
        wrapper.first_period_subset = None
        wrapper.last_period_subset = None
        return wrapper

    # TODO: implement KDBAbstract::load() method (for global KDB only)
    def _load(self, filepath: str):
        cdef CKDBVariables* kdb = new CKDBVariables(filepath.encode())
        del kdb
        self.mode_ = IodeVarMode.VAR_MODE_LEVEL
        self.first_period_subset: Period = None
        self.last_period_subset: Period = None

    def _subset_(self, pattern: str, copy: bool) -> Self:
        cdef CSample* c_sample
        cdef Variables subset_db = Variables.__new__(Variables)
        subset_db.database_ptr = subset_db.abstract_db_ptr = self.database_ptr.subset(pattern.encode(), <bint>copy)
        subset_db.ptr_owner = True
        subset_db.mode_ = IodeVarMode.VAR_MODE_LEVEL
        return subset_db

    def _get_whole_sample(self) -> Sample:
        """
        If the current instance is a subset of a Variables database, 
        returns the sample of the original Variables database.
        """
        cdef CSample* c_sample 
        if self.is_detached:
            c_sample = self.database_ptr.get_sample()
        else:
            c_sample = cpp_global_variables.get_sample()
        return Sample._from_ptr(c_sample, <bint>False)

    cpdef int c_get_real_period_position(self, CPeriod c_period):
        """
        Get the position of a period in the Variables database sample (not the subset).
        """
        cdef CSample* c_sample
        if self.is_detached:
            c_sample = self.database_ptr.get_sample()
        else:
            c_sample = cpp_global_variables.get_sample()
        return c_sample.get_period_position(<CPeriod>c_period)

    def _get_variable_unique_period(self, pos: int, t: int) -> float:
        cdef cpp_value
        cpp_value = self.database_ptr.get_var(pos, t, self.mode_)
        return cpp_value if IODE_IS_A_NUMBER(cpp_value) else np.nan

    def _get_variable_list_periods(self, pos: int, key_periods: List[int]) -> List[float]:
        values = [self.database_ptr.get_var(pos, <int>p, self.mode_) for p in key_periods]
        values = [value if IODE_IS_A_NUMBER(value) else np.nan for value in values]
        return values

    # NOTE: needed to create a dedicated method since Cython seems to have some 
    #       difficulties with Union[..., Variables].
    #       Without a dedicated method, the following error is raised:
    #       ""Storing unsafe C derivative of temporary Python reference"
    def __add_var(self, name: str, value: Variables):
        cdef string c_name = name.encode()
        t_first, t_last = value._get_periods_bounds()
        # NOTE: 'value' can contains more than one variable as long as the variable named 'name' is present
        _c_add_var_from_other(c_name, self.database_ptr, value.database_ptr, t_first, t_last)

    def _add_(self, name: str, values: Union[str, int, float, np.ndarray, Iterable[float], Variables]):
        cdef vector[double] cpp_values
        cdef int c_nb_periods
        cdef char* c_name
        cdef bytes b_name
        cdef double[::1] numpy_data_memview
        cdef KDB* c_db_ptr = NULL

        if not isinstance(name, str):
            raise TypeError(f"Cannot add a new IODE variable.\nExpected value for the 'name' argument of type string. "
                            f"Got value of type {type(name).__name__}")

        if isinstance(values, int):
            values = float(values)
        
        # values is a LEC expression
        if isinstance(values, str):
            self.database_ptr.add(<string>(name.encode()), <string>values.encode())
        # values is a vector of float
        elif isinstance(values, float):
            cpp_values = [values] * self.nb_periods
            self.database_ptr.add(<string>(name.encode()), cpp_values)
        elif isinstance(values, np.ndarray):
            if values.ndim != 1:
                raise ValueError(f"Cannot add the IODE variable '{name}'.\n"
                                 f"Expected a 1-dimensional numpy array.\n" 
                                 f"Got a {values.ndim}-dimensional numpy array instead")
            if len(values) != self.nb_periods:
                raise ValueError(f"Cannot add the IODE variable '{name}'.\n"
                                 f"Expected a numpy array of {self.nb_periods} values (number of periods).\n"
                                 f"Got {len(values)} values instead")
            # NOTE: do not call np.ascontiguousarray by default as it makes a copy of the data
            if not values.flags['C_CONTIGUOUS']:
                values = np.ascontiguousarray(values)
            numpy_data_memview = values
            b_name = name.encode()
            c_name = b_name
            c_nb_periods = self.nb_periods
            c_db_ptr = self.database_ptr.get_database()
            K_add(c_db_ptr, c_name, &numpy_data_memview[0], &c_nb_periods)
        # values is a Variables object
        # NOTE: 'values' can contains more than one variable as long as the variable
        #       named 'name' is present
        elif isinstance(values, Variables):
            if values.sample != self.sample:
                raise ValueError(f"Cannot add the IODE variable '{name}': Incompatible periods.\n"
                                f"Expected right-hand side Variables object to have sample {self.sample}.\n"
                                f"Got Variables object with sample {values.sample} instead.")
            self.__add_var(name, values)
        # values is an iterable of float
        elif isinstance(values, Iterable):
            if len(values) != self.nb_periods:
                raise ValueError(f"Cannot add the IODE variable '{name}'.\n"
                                 f"Expected a iterable of {self.nb_periods} values.\n"
                                 f"Got {len(values)} values instead")
            cpp_values = values
            self.database_ptr.add(<string>(name.encode()), cpp_values)
        else:
            raise TypeError(f"Cannot add the IODE variable '{name}'.\n"
                            f"Expected value of type str, int, float, numpy array, iterable of float or Variables. "
                            f"Got value of type {type(values).__name__} instead")

    # NOTE: needed to create a dedicated method since Cython seems to have some 
    #       difficulties with Union[..., Variables].
    #       Without a dedicated method, the following error is raised:
    #       ""Storing unsafe C derivative of temporary Python reference"
    def __copy_var(self, source_name: str, dest_name: str, t_first: int, t_last: int, value: Variables):
        cdef string c_source_name = source_name.encode()
        cdef string c_dest_name = dest_name.encode()
        value_t_first, value_t_last = value._get_periods_bounds()
        # NOTE: 'value' can contains more than one variable as long as the variable named 'name' is present
        _c_copy_var_content(c_dest_name, self.database_ptr, t_first, t_last, 
                            c_source_name, value.database_ptr, value_t_first, value_t_last)

    def _set_variable(self, key_name, values, key_periods):
        """
        Update/add a variable referenced by `key_name` from/to the current Variables database.

        Parameters
        ----------
        key_name: str, int
            The name of the variable to update/add.
            If `key_name` is an int, it is interpreted as the position of the variable in 
            the Variables database.
        values: str or int or float or numpy array or iterable(float) or Variables
            The new values of the variable.
            If `values` is a str, it is interpreted as a LEC expression.
        key_periods: str or Period or tuple(str, str) or list(str), optional
            The periods to update/add. 
            If `key_periods` is None, values for the whole sample is updated/added.
        """
        cdef int t
        cdef int t_first
        cdef int t_last
        cdef int pos
        cdef KDB* c_db_ptr = NULL
        cdef vector[double] cpp_values
        cdef double* var_ptr = NULL
        cdef double[::1] numpy_data_memview 

        if isinstance(key_name, str):
            key_name = key_name.strip()

        if isinstance(values, int):
            values = float(values)
        
        if values is None:
            raise ValueError(f"Cannot add or update the IODE variable '{key_name}'.\n"
                             f"Got None as value.")

        if key_periods is not None:
            if isinstance(key_periods, str):
                key_periods = Period(key_periods)
            elif isinstance(key_periods, Period):
                pass
            elif isinstance(key_periods, tuple):
                key_periods = Period(key_periods[0]), Period(key_periods[1])
            elif isinstance(key_periods, list):
                key_periods = [Period(p) for p in key_periods]
            else:
                raise TypeError(f"Cannot add or update the IODE variable '{key_name}'.\n"
                                f"The periods selection must be either omitted or specified as a single period, "
                                f"a sample 'start:end', or a list of periods.\nGot periods selection of type "
                                f"{type(key_periods).__name__} instead")

        c_db_ptr = self.database_ptr.get_database()

        # new Variable -> raises an error if key_periods is not None or does not represent the full sample
        #              -> only allowed when the current database is not a subset over the whole Variables sample
        if isinstance(key_name, str) and key_name not in self:
            if not (key_periods is None or isinstance(key_periods, tuple)):
                raise RuntimeError(f"Cannot add the IODE variable '{key_name}'.\nThe syntax 'variables['{key_name}'] = new_variable' "
                                   f"should be used instead of 'variables['{key_name}', <periods>] = new_variable'")
            if self.first_period_subset is not None or self.last_period_subset is not None:
                raise RuntimeError(f"Cannot add the IODE variable '{key_name}' when the subset does not cover the "
                                   f"whole sample of the IODE Variables workspace.\n")
            if isinstance(key_periods, tuple) and key_periods != (self.sample.start, self.sample.end):
                raise RuntimeError(f"Cannot add the IODE variable '{key_name}'.\n"
                                   f"When adding a new variable, the periods selection must be omitted or "
                                   f"represent the whole Variables sample {self.sample}.\nGot periods selection {key_periods} instead.")
            # NOTE: if 'values' is a Variables object, it can contains more than one variable as long as the variable
            #       named 'name' is present
            self._add_(key_name, values)
        
        # update a Variable
        else:
            pos = self.get_position(key_name) if isinstance(key_name, str) else key_name
            name = self.get_name(pos) if isinstance(key_name, int) else key_name
            # update values for the whole (subset) sample
            if key_periods is None:
                sample = self.sample
                key_periods = sample.start, sample.end
            # update the value for only one period 
            if isinstance(key_periods, Period):
                t = self._get_real_period_position(key_periods)
                if isinstance(values, float):
                    KV_set(c_db_ptr, pos, t, self.mode_, <double>values)
                # values is a LEC expression
                elif isinstance(values, str):
                    self.database_ptr.update(pos, <string>values.encode(), t, t) 
                elif isinstance(values, Variables):
                    self.__copy_var(name, name, t, t, values)
                else:
                    raise TypeError(f"Cannot update the IODE variable '{name}'.\n"
                                    f"When updating values for a single period, the right-hand side must be of type "
                                    f"int, float, str or interable of float.\nGot input of type {type(values).__name__} instead")
            # update values for a contiguous range of periods
            elif isinstance(key_periods, tuple):
                first_period, last_period = key_periods
                t_first = self._get_real_period_position(first_period)
                t_last = self._get_real_period_position(last_period)
                # set same value for all periods in the range
                if isinstance(values, float):
                    for t in range(t_first, t_last + 1): 
                        KV_set(c_db_ptr, pos, t, self.mode_, <double>values)  
                # values is a LEC expression
                elif isinstance(values, str):
                    self.database_ptr.update(pos, <string>values.encode(), t_first, t_last) 
                # values is of type Variables
                elif isinstance(values, Variables):
                    sample: Sample = Sample(first_period, last_period)
                    if values.sample != sample:
                        raise ValueError(f"Cannot update the IODE variable '{name}': Incompatible periods.\n"
                                         f"Expected right-hand side Variables object to have sample {sample}.\n"
                                         f"Got Variables object with sample {values.sample} instead.")
                    # NOTE: 'values' can contains more than one variable as long as the variable
                    #       named 'name' is present
                    source_name = name if len(values) > 1 else values.names[0]
                    self.__copy_var(source_name, name, t_first, t_last, values)
                # values is an iterable
                elif isinstance(values, Iterable):
                    nb_periods = t_last - t_first + 1
                    if len(values) != nb_periods:
                        raise ValueError(f"Cannot update the IODE variable '{name}'.\n"
                                         f"Expected {nb_periods} values.\nGot {len(values)} values instead")
                    if isinstance(values, np.ndarray) and self.mode_ == IodeVarMode.VAR_MODE_LEVEL:
                        var_ptr = KVVAL(c_db_ptr, pos, t_first)
                        # NOTE: do not call np.ascontiguousarray by default as it makes a copy of the data
                        if not values.flags['C_CONTIGUOUS']:
                            values = np.ascontiguousarray(values)
                        numpy_data_memview = values
                        memcpy(var_ptr, &numpy_data_memview[0], nb_periods * sizeof(double))
                    for i, t in enumerate(range(t_first, t_last + 1)):
                        KV_set(c_db_ptr, pos, t, self.mode_, <double>(values[i]))
                else:
                    raise TypeError(f"Cannot update the IODE variable '{name}'.\nExpected 'value' of type str, int, "
                                    f"float, numpy array, pandas Series or Variables.\nGot 'value' of type "
                                    f"{type(values).__name__} instead")
            # update values for a list of periods
            elif isinstance(key_periods, list):              
                # set the same value for all periods in the list
                if isinstance(values, float):
                    values = [values] * len(key_periods)
                # values is a iterable of float containing a specific value for each period
                elif isinstance(values, Iterable):
                    if len(values) != len(key_periods):
                        raise ValueError(f"Cannot update the IODE variable '{name}'.\n"
                                         f"Expected a {type(values).__name__} of {len(key_periods)} values.\n"
                                         f"Got {len(values)} values instead")
                    if not all(isinstance(v, float) for v in values):
                        raise ValueError(f"Cannot update the IODE variable '{name}'.\n"
                                         f"Not all items of {type(values).__name__} are of type float:\n{values}")   
                else:
                    raise TypeError(f"Cannot update the IODE variable '{name}'.\n"
                                    f"When updating values for non-contiguous periods, the right-hand side must be "
                                    f"a float or an iterable of float.\nGot input of type {type(values).__name__} instead")
                for p, v in zip(key_periods, values):
                    t = self._get_real_period_position(p)
                    KV_set(c_db_ptr, pos, t, self.mode_, <double>v)

    # NOTE: needed to create a dedicated method since Cython seems to have some 
    #       difficulties with Union[..., Variables].
    #       Without a dedicated method, the following error is raised:
    #       ""Storing unsafe C derivative of temporary Python reference"
    def __binary_op_variables__(self, other: Variables, op: BinaryOperation) -> Variables:
        cdef int i_op = int(op)
        cdef CKDBVariables* cpp_self_db = self.database_ptr
        cdef CKDBVariables* cpp_other_db = self.database_ptr
        if len(self) != len(other):
            raise ValueError(f"Cannot perform arithmetic operation between two Variables with different number of variables.\n"
                             f"Left operand has {len(self)} variables.\nRight operand has {len(other)} variables")
        if self.sample != other.sample:
            raise ValueError(f"Cannot perform arithmetic operation between two Variables with different samples.\n"
                             f"Left operand sample: {self.sample}\nRight operand sample: {other.sample}")
        t_first, t_last = self._get_periods_bounds()
        names = self.names
        if len(names) == 1:
            name = names[0]
            other_name = other.names[0]
            _c_operation_between_two_vars(i_op, cpp_self_db, <string>name.encode(), <int>t_first, <int>t_last, 
                                       cpp_other_db, <string>other_name.encode(), <int>t_first, <int>t_last)
        else:
            self._check_same_names(names, other.names)
            for name in names:
                _c_operation_between_two_vars(i_op, cpp_self_db, <string>name.encode(), <int>t_first, <int>t_last, 
                                           cpp_other_db, <string>name.encode(), <int>t_first, <int>t_last)
        return self

    def __binary_op__(self, other: Union[int, float, np.ndarray, pd.Series, pd.DataFrame, Array, Variables], 
                      op: BinaryOperation, copy_self: bool):
        cdef int i, t_first, t_last
        cdef int i_op = int(op)
        cdef double c_value
        cdef CKDBVariables* c_database = NULL
        cdef double[::1] numpy_data_memview 

        py_database: Variables = self.copy() if copy_self else self

        if isinstance(other, Variables):
            return py_database.__binary_op_variables__(other, op)

        t_first, t_last = py_database._get_periods_bounds()
        nb_periods = t_last - t_first + 1

        other = py_database._convert_values(other)
        c_database = py_database.database_ptr

        if isinstance(other, float):
            c_value = other
            _c_operation_scalar(i_op, c_database, t_first, t_last, c_value)
            return py_database

        key_names = py_database.names
        key_periods = py_database.periods
           
        if isinstance(other, np.ndarray):
            data = other
            # see https://cython.readthedocs.io/en/stable/src/userguide/memoryviews.html#pass-data-from-a-c-function-via-pointer
            if not data.flags['C_CONTIGUOUS']:
                data = np.ascontiguousarray(data)
            if len(key_names) == 1:
                if data.ndim != 1:
                    raise ValueError("Expected a 1D numpy array for the right-hand side operand as the left-hand side "
                                     "represents a single variable.")
                if len(data) != nb_periods:
                    raise ValueError(f"Cannot perform arithmetic operation between a left-hand side representing {nb_periods} "
                                     f"periods and a numpy ndarray with {len(data)} elements")
                name = key_names[0]
                numpy_data_memview = data
                _c_operation_one_var(i_op, c_database, <string>name.encode(), t_first, t_last, &numpy_data_memview[0])
            elif len(key_periods) == 1:
                if data.ndim != 1:
                    raise ValueError("Expected a 1D numpy array for the right-hand side operand as the left-hand side "
                                     "represents a single period.")
                if len(data) != len(key_names):
                    raise ValueError(f"Cannot perform arithmetic operation between a left-hand side representing {len(key_names)} "
                                     f"variables and a numpy ndarray with {len(data)} elements")
                numpy_data_memview = data
                _c_operation_one_period(i_op, c_database, t_first, &numpy_data_memview[0], len(data))
            else:
                if data.shape[0] != len(key_names):
                    raise ValueError(f"Cannot perform arithmetic operation between a left-hand side representing {len(key_names)} "
                                    f"variables and a numpy ndarray with {data.shape[0]} rows")
                if data.shape[-1] != nb_periods:
                    raise ValueError(f"Cannot perform arithmetic operation between a left-hand side representing {nb_periods} "
                                    f"periods and a numpy ndarray with {data.shape[-1]} columns") 
                for name, _data in zip(key_names, data):
                    numpy_data_memview = _data
                    _c_operation_one_var(i_op, c_database, <string>name.encode(), t_first, t_last, &numpy_data_memview[0])
            return py_database

        if isinstance(other, pd.DataFrame):
            other = self._check_pandas_dataframe(other, key_names, key_periods)
            if isinstance(other, pd.DataFrame):
                data = other.to_numpy(copy=False)
                # see https://cython.readthedocs.io/en/stable/src/userguide/memoryviews.html#pass-data-from-a-c-function-via-pointer
                if not data.flags['C_CONTIGUOUS']:
                    data = np.ascontiguousarray(data)
                for name, _data in zip(key_names, data):
                    numpy_data_memview = _data
                    _c_operation_one_var(i_op, c_database, <string>name.encode(), t_first, t_last, &numpy_data_memview[0])
            return py_database

        if isinstance(other, pd.Series):
            other = self._check_pandas_series(other, key_names, key_periods)
            data = other.to_numpy(copy=False)
            # see https://cython.readthedocs.io/en/stable/src/userguide/memoryviews.html#pass-data-from-a-c-function-via-pointer
            if not data.flags['C_CONTIGUOUS']:
                data = np.ascontiguousarray(data)
            numpy_data_memview = data
            if len(key_names) == 1:
                if len(data) != nb_periods:
                    raise ValueError(f"Cannot perform arithmetic operation between a left-hand side representing {nb_periods} "
                                     f"periods and a pandas Series with {len(data)} elements")
                name = key_names[0]
                _c_operation_one_var(i_op, c_database, <string>name.encode(), t_first, t_last, &numpy_data_memview[0])
            else:
                if len(data) != len(key_names):
                    raise ValueError(f"Cannot perform arithmetic operation between a left-hand side representing {len(key_names)} "
                                     f"variables and a pandas Series with {len(data)} elements")
                if len(key_periods) != 1:
                    raise ValueError("Cannot perform arithmetic operation between a left-hand side representing multiple variables "
                                     "and periods and a pandas Series")
                _c_operation_one_period(i_op, c_database, t_first, &numpy_data_memview[0], len(data))
            return py_database
            
        if isinstance(other, Array):
            other = self._check_larray_array(other, key_names, key_periods)
            data = other.data
            # see https://cython.readthedocs.io/en/stable/src/userguide/memoryviews.html#pass-data-from-a-c-function-via-pointer
            if not data.flags['C_CONTIGUOUS']:
                data = np.ascontiguousarray(data)
            if len(key_names) == 1:
                name = key_names[0]
                numpy_data_memview = data.flatten()
                _c_operation_one_var(i_op, c_database, <string>name.encode(), t_first, t_last, &numpy_data_memview[0])
            else:
                for name, _data in zip(key_names, data):
                    numpy_data_memview = _data
                    _c_operation_one_var(i_op, c_database, <string>name.encode(), t_first, t_last, &numpy_data_memview[0])
            return py_database

        raise TypeError(f"unsupported operand type for {op.name}.\nAccepted types are: "
                        f"'int, float, numpy ndarray, pandas Series, pandas DataFrame, larray Array "
                        f"or iode Variables'.\nGot operand type {type(other).__name__} instead.")

    # self + other
    def __add__(self, other):
        return self.__binary_op__(other, BinaryOperation.OP_ADD, True)

    # other + self
    def __radd__(self, other):
        return self.__binary_op__(other, BinaryOperation.OP_ADD, True)

    # self += other
    def __iadd__(self, other):
        self.__binary_op__(other, BinaryOperation.OP_ADD, False)
        return self

    # self - other
    def __sub__(self, other):
        return self.__binary_op__(other, BinaryOperation.OP_SUB, True)

    # other - self
    def __rsub__(self, other):
        if isinstance(other, Variables):
            return other.__binary_op__(self, BinaryOperation.OP_SUB, True)
        else:
            raise TypeError(f"unsupported operand type for 'X' in the arithmetic operation 'X - Y'.\n"
                            f"The only accepted type for 'X' is 'Variables'.\nGot 'X' of type {type(other).__name__} instead")

    # self -= other
    def __isub__(self, other):
        self.__binary_op__(other, BinaryOperation.OP_SUB, False)
        return self

    # self * other
    def __mul__(self, other):
        return self.__binary_op__(other, BinaryOperation.OP_MUL, True)
    
    # other * self
    def __rmul__(self, other):
        return self.__binary_op__(other, BinaryOperation.OP_MUL, True)

    # self *= other
    def __imul__(self, other):
        self.__binary_op__(other, BinaryOperation.OP_MUL, False)
        return self

    # self / other
    def __truediv__(self, other):
        if isinstance(other, (int, float)) and other == 0:
            raise ZeroDivisionError("division by zero")
        return self.__binary_op__(other, BinaryOperation.OP_DIV, True)

    # other / self
    def __rtruediv__(self, other):
        if isinstance(other, Variables):
            return other.__binary_op__(self, BinaryOperation.OP_DIV, True)
        else:
            raise TypeError(f"unsupported operand type for 'X' in the arithmetic operation 'X / Y'.\n"
                            f"The only accepted type for 'X' is 'Variables'.\nGot 'X' of type {type(other).__name__} instead")

    # self /= other
    def __itruediv__(self, other):
        if isinstance(other, (int, float)) and other == 0:
            raise ZeroDivisionError("division by zero")
        self.__binary_op__(other, BinaryOperation.OP_DIV, False)
        return self

    # self ** other
    def __pow__(self, other):
        return self.__binary_op__(other, BinaryOperation.OP_POW, True)

    def from_numpy(self, data: np.ndarray, vars_names: Union[str, List[str]]=None, 
        first_period: Union[str, Period]=None, last_period: Union[str, Period]=None):
        cdef var_pos
        cdef bytes b_name
        cdef char* c_name
        cdef double value
        cdef double* var_ptr = NULL
        cdef int i, j, t
        cdef int mode = <int>self.mode_
        cdef int t_first_period
        cdef int t_last_period
        cdef int nb_periods
        cdef KDB* db_ptr = self.database_ptr.get_database()
        if db_ptr is NULL:
            raise RuntimeError("The IODE Variables workspace has not been initialized")

        self_names = self.names

        self_first_period, self_last_period = self._get_periods_bounds()
        self_nb_periods = self_last_period - self_first_period + 1

        if self_nb_periods <= 0:
            raise RuntimeError("The sample of the Variables database to export is empty")

        if vars_names is None:
            vars_names = self_names
        if isinstance(vars_names, str):
            vars_names = split_list(vars_names)

        # check that all names in the pandas object are present in the current subset 
        if self.is_detached:
            self._check_same_names(self_names, vars_names)

        # value for argument 'first_period' represents a sample (range of periods)
        if isinstance(first_period, str) and ':' in first_period:
            first_period, last_period = first_period.split(':')

        self_first_period = self.first_period
        if first_period is None:
            first_period = self_first_period
        else:
            first_period = Period(first_period)

        if first_period < self_first_period:
            raise ValueError(f"The first period {first_period} is before the first period of "
                             f"the current Variables database {self_first_period}")

        self_last_period = self.last_period
        if last_period is None:
            last_period = self_last_period
        else:
            last_period = Period(last_period)

        if last_period > self_last_period:
            raise ValueError(f"The last period {last_period} is after the last period of "
                             f"the current Variables database {self_last_period}")
        
        t_first_period = self._get_real_period_position(first_period)
        t_last_period = self._get_real_period_position(last_period)
        nb_periods = t_last_period - t_first_period + 1

        # If the ndarray is a 1D array, either `var_names` must represent 
        # a single variable or `first_period` must be equal to `last_period`
        if data.ndim == 1:
            # data = single variable
            if len(vars_names) == 1:
                data = data.reshape(1, -1)
            # data = single period
            elif nb_periods == 1:
                data = data.reshape(-1, 1)
            else:
                raise ValueError("When the passed numpy ndarray is 1D, either the argument "
                                 "'vars_names' must represent a single variable or the arguments "
                                 "'first_period' and 'last_period' must be equal")
        
        if len(vars_names) != data.shape[0]:
            raise ValueError(f"The number of variables ({len(vars_names)}) to update is different "
                             f"from the number of rows of the numpy ndarray ({data.shape[0]}).\n"
                             f"Variables to updated are: {vars_names}")

        if nb_periods != data.shape[1]:
            raise ValueError(f"The number of periods ({nb_periods}) to update is different "
                             f"from the number of columns of the numpy ndarray ({data.shape[1]}).\n"
                             f"Periods to updated are: {first_period}:{last_period}")

        # add new variables if they do not exist
        new_vars = set(vars_names) - set(self_names)
        for name in new_vars:
            # NOTE: Cython cannot directly convert a Python string to a C string, 
            #       so we need to use the encode() method to convert it to a bytes object first, 
            #       and then convert the bytes object to a C string using the syntax char_obj = bytes_obj.
            b_name = name.encode()
            c_name = b_name
            # add a new variable with all values set to IODE_NAN
            KV_add(db_ptr, c_name)

        if not data.flags['C_CONTIGUOUS']:
            # make sure the array is C-contiguous
            data = np.ascontiguousarray(data)

        # astype(nb.float64) + np.nan -> NA
        data = self._convert_values(data)

        # declaring a C-contiguous array of 2D double
        # see https://cython.readthedocs.io/en/latest/src/userguide/numpy_tutorial.html#declaring-the-numpy-arrays-as-contiguous 
        cdef double[:, ::1] data_view = data

        # copy the values
        if self.mode_ == IodeVarMode.VAR_MODE_LEVEL:
            for i, name in enumerate(vars_names):
                # NOTE: Cython cannot directly convert a Python string to a C string, 
                #       so we need to use the encode() method to convert it to a bytes object first, 
                #       and then convert the bytes object to a C string using the syntax char_obj = bytes_obj.
                b_name = name.encode()
                c_name = b_name
                var_pos = K_find(db_ptr, c_name)
                var_ptr = KVVAL(db_ptr, var_pos, t_first_period)
                memcpy(var_ptr, &data_view[i, 0], nb_periods * sizeof(double))
        else:
            for i, name in enumerate(vars_names):
                b_name = name.encode()
                c_name = b_name
                var_pos = K_find(db_ptr, c_name)
                for j, t in enumerate(range(t_first_period, t_last_period + 1)):
                    value = data_view[i, j]
                    KV_set(db_ptr, var_pos, t, mode, value)

    def to_numpy(self) -> np.ndarray:
        cdef int i, t
        cdef double value
        cdef double* var_ptr = NULL
        cdef int mode = <int>self.mode_
        cdef int t_first_period
        cdef int t_last_period
        cdef int nb_periods
        cdef KDB* db_ptr = self.database_ptr.get_database()
        if db_ptr is NULL:
            raise RuntimeError("The IODE Variables workspace is empty")

        if not len(self):
            raise RuntimeError("The IODE Variables workspace is empty")

        t_first_period, t_last_period = self._get_periods_bounds()
        nb_periods = t_last_period - t_first_period + 1

        if nb_periods <= 0:
            raise RuntimeError("The sample of the Variables database to export is empty")

        # Copy values from the IODE Variables database to a Numpy 2D array
        data = np.empty((len(self), nb_periods), dtype=np.float64)

        # declaring a C-contiguous array of 2D double
        # see https://cython.readthedocs.io/en/latest/src/userguide/numpy_tutorial.html#declaring-the-numpy-arrays-as-contiguous 
        cdef double[:, ::1] data_view = data
        if self.mode_ == IodeVarMode.VAR_MODE_LEVEL:
            for i in range(len(self)):
                var_ptr = KVVAL(db_ptr, i, t_first_period)
                memcpy(&data_view[i, 0], var_ptr, nb_periods * sizeof(double))
        else:
            for i in range(len(self)):
                for t in range(t_first_period, t_last_period + 1):
                    value = KV_get(db_ptr, i, t, mode)
                    data_view[i, t - t_first_period] = value
        
        if len(self) == 1:
            data = data.reshape((nb_periods,))

        _NA = 0.9999 * NA
        data[data < _NA] = np.nan
        return data

    def __array__(self, dtype=None):
        data = self.to_numpy()
        if dtype is not None:
            data = data.astype(dtype)
        return data

    def from_frame(self, df: DataFrame):
        if pd is None:
            raise RuntimeError("pandas library not found")

        # list of variable names
        vars_names = df.index.to_list()

        # list of periods
        periods_list = df.columns.to_list()
        first_period, last_period = periods_list[0], periods_list[-1]

        if not (self.is_global_workspace or self.is_detached):
            # check that all names in the pandas object are present in the current subset 
            self._check_same_names(self.names, vars_names)

        if self.first_period_subset is not None or self.last_period_subset is not None:
            raise RuntimeError("Cannot call 'from_frame' method on a subset of the sample "
                               "of the variables workspace")

        # override the current sample
        self.sample = f"{first_period}:{last_period}"

        # numpy data
        data = df.to_numpy(copy=False)

        self.from_numpy(data, vars_names, first_period, last_period)

    def to_frame(self, vars_axis_name: str = 'names', time_axis_name: str = 'time', sample_as_floats: bool = False) -> DataFrame:
        if pd is None:
            raise RuntimeError("pandas library not found")
        
        vars_list = self.names
        periods_list = self.periods_as_float if sample_as_floats else self.periods
        data = self.to_numpy()

        df = DataFrame(index=vars_list, columns=periods_list, data=data)
        df.index.name = vars_axis_name
        df.columns.name = time_axis_name
        return df

    def from_array(self, array: Array, time_axis_name: str = 'time', sep: str = "_"):
        if la is None:
            raise RuntimeError("larray library not found")  

        if self.first_period_subset is not None or self.last_period_subset is not None:
            raise RuntimeError("Cannot call 'from_array' method on a subset of a workspace")

        # retrieve the time_axis_name. 
        # Raise an error if no time_axis_name is present in the array.
        if time_axis_name not in array.axes:
            raise ValueError(f"Passed Array object must contain an axis named {time_axis_name}.\nGot axes {repr(array.axes)}.")
        time = array.axes[time_axis_name]
        first_period, last_period = time.labels[0], time.labels[-1]

        # override the current sample
        self.sample = f"{first_period}:{last_period}"
        
        # push the time axis as last axis and combine all other axes 
        array = array.transpose(..., time_axis_name)
        if array.ndim > 2:
            array = array.combine_axes(array.axes[:-1], sep=sep)
        vars_names = array.axes[0].labels

        if not (self.is_global_workspace or self.is_detached):
            # check that all names in the pandas object are present in the current subset 
            self._check_same_names(self.names, vars_names) 

        self.from_numpy(array.data, vars_names, first_period, last_period)

    def to_array(self, vars_axis_name: str = 'names', time_axis_name: str = 'time', sample_as_floats: bool = False) -> Array:
        if la is None:
            raise RuntimeError("larray library not found")

        vars_list = self.names
        periods_list = self.periods_as_float if sample_as_floats else self.periods
        data = self.to_numpy()

        vars_axis = la.Axis(name=vars_axis_name, labels=vars_list)
        time_axis = la.Axis(name=time_axis_name, labels=periods_list)
        return la.Array(axes=(vars_axis, time_axis), data=data)

    @property
    def mode(self) -> str:
        return VarsMode(<int>self.mode_).name

    @mode.setter
    def mode(self, value: Union[VarsMode, str]):
        if isinstance(value, str):
            value = value.upper()
            value = VarsMode[value]
        value = int(value)
        self.mode_ = <IodeVarMode>value

    @property
    def first_period(self) -> Period:
        cdef CSample* c_sample
        if self.first_period_subset is not None:
            return self.first_period_subset 
        else:
            whole_db_sample: Sample = self._get_whole_sample()
            return whole_db_sample.start
    
    @property
    def last_period(self) -> Period:
        cdef CSample* c_sample
        if self.last_period_subset is not None:
            return self.last_period_subset 
        else:
            whole_db_sample: Sample = self._get_whole_sample()
            return whole_db_sample.end

    @property
    def sample(self) -> Sample:
        whole_db_sample: Sample = self._get_whole_sample()
        if self.first_period_subset is None and self.last_period_subset is None:
            return whole_db_sample

        self._maybe_update_subset_sample()

        if self.first_period_subset is not None:
            first_period = self.first_period_subset
        else: 
            first_period = whole_db_sample.start

        if self.last_period_subset is not None:
            last_period = self.last_period_subset
        else:
            last_period = whole_db_sample.end
        
        return Sample(first_period, last_period)

    @sample.setter    
    def sample(self, value: Union[str, Tuple[Union[str, Period], Union[str, Period]]]):
        if self.first_period_subset is not None or self.last_period_subset is not None:
            raise RuntimeError("Changing the sample on a subset of the Variables workspace is not allowed.") 

        if isinstance(value, str):
            if ':' not in value:
                raise ValueError("sample: Missing colon ':' in the definition of the new sample. "
                                 f"Got value '{value}'.")
            from_period, to_period = value.split(':')
        elif isinstance(value, tuple):
            if not len(value) == 2:
                raise ValueError("'sample': Expected two values: from_period, to_period. "
                                 f"Got {len(value)} values.")
            from_period, to_period = value
            if isinstance(from_period, Period):
                from_period = str(from_period)
            if isinstance(to_period, Period):
                to_period = str(to_period)
        
        if self.is_detached:
            self.database_ptr.set_sample(from_period.encode(), to_period.encode())
        else:
            cpp_global_variables.set_sample(from_period.encode(), to_period.encode())

    @property
    def nb_periods(self) -> int:
        # self.sample calls self._maybe_update_subset_sample()
        return self.sample.nb_periods
        
    @property
    def periods(self) -> List[str]:
        # self.sample calls self._maybe_update_subset_sample()
        return self.sample.get_period_list()

    @property
    def periods_as_float(self) -> List[float]:
        # self.sample calls self._maybe_update_subset_sample()
        return self.sample.get_period_list(astype=float)

    @property
    def threshold(self) -> float:
        return K_CMP_EPS

    @threshold.setter
    def threshold(self, value: float):
        res = B_DataCompareEps(str(value).encode())
        if res != 0:
            raise ValueError(f"threshold: Invalid value '{value}'.")

    @property
    def df(self) -> DataFrame:
        return self.to_frame()

    def periods_subset(self, from_period: Union[str, Period] = None, to_period: Union[str, Period] = None, 
                       as_float: bool = False):
        # self.sample calls self._maybe_update_subset_sample()
        sample = self.sample

        if from_period is None or to_period is None:
            if from_period is None:
                from_period = sample.start
            if to_period is None:
                to_period = sample.end
        
        if isinstance(from_period, Period):
            from_period = str(from_period)
        if isinstance(to_period, Period):
            to_period = str(to_period)

        if as_float:
            return self.database_ptr.get_list_periods_as_float(from_period.encode(), to_period.encode())
        else:
            return [cpp_period.decode() for cpp_period in 
                    self.database_ptr.get_list_periods(from_period.encode(), to_period.encode())]
        
    def copy_from(self, input_files: Union[str, List[str]], from_period: Union[str, Period]=None, 
                  to_period: Union[str, Period]=None, names: Union[str, List[str]]='*'):
        if not (self.is_global_workspace or self.is_detached):
            raise RuntimeError("Cannot call 'copy_from' method on a subset of a workspace")
        
        input_files, names = self._copy_from(input_files, names)

        if from_period is None or to_period is None:
            sample = self.sample
            if from_period is None:
                from_period = sample.start
            if to_period is None:
                to_period = sample.end
        
        if isinstance(from_period, Period):
            from_period = str(from_period)
        if isinstance(to_period, Period):
            to_period = str(to_period)

        self.database_ptr.copy_from(input_files.encode(), from_period.encode(), to_period.encode(), 
                                    names.encode())

    def low_to_high(self, type_of_series: Union[LowToHighType, str], method: Union[LowToHighMethod, str], 
                    filepath: Union[str, Path], var_list: Union[str, List[str]]):
        if isinstance(filepath, str):
            filepath = Path(filepath)
        if not filepath.exists():
            raise ValueError(f"file '{str(filepath)}' not found.")
        filepath = str(filepath)

        if isinstance(type_of_series, str):
            type_of_series = type_of_series.upper()
            type_of_series = LowToHighType[type_of_series]
        type_of_series = int(type_of_series)

        if isinstance(method, str):
            method = method.upper()
            if len(method) > 1:
                method = LowToHighMethod[method]
        else:
            method = method.value

        if not isinstance(var_list, str) and isinstance(var_list, Iterable) and \
            all(isinstance(item, str) for item in var_list):
            var_list = ';'.join(var_list)

        cpp_low_to_high(<IodeLowToHigh>type_of_series, <char>ord(method), filepath.encode(), var_list.encode())

    def high_to_low(self, type_of_series: Union[HighToLowType, str], filepath: Union[str, Path], var_list: Union[str, List[str]]):
        if isinstance(filepath, str):
            filepath = Path(filepath)
        if not filepath.exists():
            raise ValueError(f"file '{str(filepath)}' not found.")
        filepath = str(filepath)

        if isinstance(type_of_series, str):
            type_of_series = type_of_series.upper()
            type_of_series = LowToHighType[type_of_series]
        type_of_series = int(type_of_series)

        if not isinstance(var_list, str) and isinstance(var_list, Iterable) and \
            all(isinstance(item, str) for item in var_list):
            var_list = ';'.join(var_list)

        cpp_high_to_low(<IodeHighToLow>type_of_series, filepath.encode(), var_list.encode())

    def extrapolate(self, method: Union[SimulationInitialization, str], from_period: Union[str, Period] = None, 
                    to_period: Union[str, Period] = None, variables_list: Union[str, List[str]] = None):
        if isinstance(method, str):
            method = method.upper()
            method = SimulationInitialization[method]
        method = int(method)

        if from_period is None or to_period is None:
            sample = self.sample
            if from_period is None:
                from_period = sample.start
            if to_period is None:
                to_period = sample.end

        if isinstance(from_period, Period):
            from_period = str(from_period)

        if isinstance(to_period, Period):
            to_period = str(to_period)

        if variables_list is None:
            variables_list = ''
        if not isinstance(variables_list, str) and isinstance(variables_list, Iterable) and \
            all(isinstance(name, str) for name in variables_list):
            variables_list = ';'.join(variables_list)

        self.database_ptr.extrapolate(<VariablesInitialization>method, from_period.encode(), to_period.encode(), 
                                      variables_list.encode())

    # TODO : add doctests (ask Geert Bryon)
    def seasonal_adjustment(self, input_file: str, eps_test: float=5.0, series: Union[str, List[str]] = None):
        if isinstance(input_file, str):
            input_file = Path(input_file)
        if not input_file.exists():
            raise ValueError(f"file '{str(input_file)}' not found.")
        input_file = str(input_file)

        if series is None:
            series = ''
        if not isinstance(series, str) and isinstance(series, Iterable) and \
            all(isinstance(name, str) for name in series):
            series = ';'.join(series)

        self.database_ptr.seasonal_adjustment(input_file.encode(), series.encode(), eps_test)

    # TODO : add doctests (ask Geert Bryon)
    def trend_correction(self, input_file: str, lambda_: float, series: Union[str, List[str]] = None, log: bool = False):
        if isinstance(input_file, str):
            input_file = Path(input_file)
        if not input_file.exists():
            raise ValueError(f"file '{str(input_file)}' not found.")
        input_file = str(input_file)

        if series is None:
            series = ''
        if not isinstance(series, str) and isinstance(series, Iterable) and \
            all(isinstance(name, str) for name in series):
            series = ';'.join(series)

        self.database_ptr.trend_correction(input_file.encode(), lambda_, series.encode(), <bint>log)

    @classmethod
    def convert_file(cls, input_file: Union[str, Path], input_format: Union[str, ImportFormats], 
                     save_file: Union[str, Path], rule_file: Union[str, Path], 
                     from_period: Union[str, Period], to_period: Union[str, Period],
                     debug_file: Union[str, Path]=None) -> None:
        # $FileImportCmt format rule_file input_file language [debug_file]
        input_file = check_filepath(input_file, IodeFileType.FILE_ANY, file_must_exist=True)

        _c_import_formats: str = ''.join([item.name[0] for item in list(ImportFormats)])
        if isinstance(input_format, ImportFormats):
            input_format = input_format.name[0]
        if input_format not in _c_import_formats:
            raise ValueError(f"Invalid input format '{input_format}'. "
                             f"Possible values are: {_c_import_formats}")
        
        save_file = check_filepath(save_file, IodeFileType.FILE_VARIABLES, file_must_exist=False)
        rule_file = check_filepath(rule_file, IodeFileType.FILE_ANY, file_must_exist=True)

        if isinstance(from_period, Period):
            from_period = str(from_period)
        if isinstance(to_period, Period):
            to_period = str(to_period)

        # $FileImportVar format rule infile outfile from to  [trace]
        args = f"{input_format} {rule_file} {input_file} {save_file} {from_period} {to_period}"
        
        if debug_file:
            debug_file = check_filepath(debug_file, IodeFileType.FILE_LOG, file_must_exist=False)
            args += " " + debug_file

        res = B_FileImportVar(args.encode('utf-8'))
        if res < 0:
            raise RuntimeError(f"Cannot import variables from file '{input_file}'")

    @classmethod
    def export_as_file(cls, variables_file: Union[str, Path], rule_file: Union[str, Path], 
                       save_file: Union[str, Path], export_format: Union[str, ExportFormats], 
                       from_period: Union[str, Period], to_period: Union[str, Period],
                       comments_file: Union[str, Path], nan_value: str="#N/A", separator: str=";",
                       debug_file: Union[str, Path]=None) -> None:
        variables_file = check_filepath(variables_file, IodeFileType.FILE_VARIABLES, file_must_exist=True)
        rule_file = check_filepath(rule_file, IodeFileType.FILE_ANY, file_must_exist=True)
        save_file = check_filepath(save_file, IodeFileType.FILE_ANY, file_must_exist=False)

        if isinstance(export_format, str):
            export_format = ExportFormats[export_format.upper()]
        export_format = int(export_format)

        if isinstance(from_period, Period):
            from_period = str(from_period)
        if isinstance(to_period, Period):
            to_period = str(to_period)

        if comments_file:
            comments_file = check_filepath(comments_file, IodeFileType.FILE_COMMENTS, file_must_exist=True)
        else:
            comments_file = ""
        
        if debug_file:
            debug_file = check_filepath(debug_file, IodeFileType.FILE_LOG, file_must_exist=False)
        else:
            debug_file = ""

        res = EXP_RuleExport(debug_file.encode('utf-8'), rule_file.encode('utf-8'), save_file.encode('utf-8'), 
                             variables_file.encode('utf-8'), comments_file.encode('utf-8'), 
                             from_period.encode('utf-8'), to_period.encode('utf-8'), nan_value.encode('utf-8'), 
                             separator.encode('utf-8'), export_format)

        # -- make sure the debug file is written at the end of the function --
        # note: W_flush() and W_close() are not called in the C function EXP_RuleExport
        if debug_file:
            W_flush()
            W_close()

        if res < 0:
            raise RuntimeError(f"Cannot export the variables file '{variables_file}'")

    def _str_header(self) -> str:
        s = super()._str_header() 
        s += f"sample: {self.sample}\n"
        s += f"mode: {self.mode}\n"
        return s

    def _str_table(self, names: List[str]) -> str:
        cdef t
        columns = {"name": names}
        names_pos: List[int] = [self.get_position(name) for name in names] 
        for str_period in self.periods:
            period = Period(str_period)
            t = self._get_real_period_position(period)
            columns[str_period] = [self.database_ptr.get_var(<int>pos, t, self.mode_) for pos in names_pos]
        return table2str(columns, max_lines=10, max_width=100, precision=2, justify_funcs={"name": JUSTIFY.LEFT})

    def print_to_file(self, filepath: Union[str, Path], names: Union[str, List[str]]=None, format: str=None) -> None:
        self._print_to_file(filepath, names, format)

    def __hash__(self) -> int:
        return hash_value(dereference(self.database_ptr))
