import warnings
from pathlib import Path
from collections.abc import Iterable
from typing import Union, Tuple, List, Set, Optional, Any
from enum import IntEnum

import numpy as np
import pandas as pd
try:
    import larray as la
    Array = la.Array
except ImportError:
    la = None
    Array = Any

cimport cython
from cython.operator cimport dereference
from libc.string cimport memcpy
from libcpp.string cimport string
from libcpp.vector cimport vector
from pyiode.common cimport IODE_NAN, IodeVarMode, IodeLowToHigh, IodeHighToLow, VariablesInitialization
from pyiode.util cimport IODE_IS_A_NUMBER
from pyiode.time.period cimport CPeriod
from pyiode.time.sample cimport CSample
from pyiode.iode_database.cpp_api_database cimport KV_get, KV_set, KV_add
from pyiode.iode_database.cpp_api_database cimport _c_add_var_from_other, _c_copy_var_content
from pyiode.iode_database.cpp_api_database cimport BinaryOperation as CBinaryOperation
from pyiode.iode_database.cpp_api_database cimport _c_operation_scalar, _c_operation_one_var, _c_operation_one_period
from pyiode.iode_database.cpp_api_database cimport _c_operation_between_two_vars
from pyiode.iode_database.cpp_api_database cimport hash_value
from pyiode.iode_database.cpp_api_database cimport K_CMP_EPS
from pyiode.iode_database.cpp_api_database cimport B_DataCompareEps
from pyiode.iode_database.cpp_api_database cimport CKDBVariables
from pyiode.iode_database.cpp_api_database cimport KDBVariables as CppKDBVariables
from pyiode.iode_database.cpp_api_database cimport Variables as cpp_global_variables
from pyiode.iode_database.cpp_api_database cimport low_to_high as cpp_low_to_high
from pyiode.iode_database.cpp_api_database cimport high_to_low as cpp_high_to_low
from pyiode.iode_database.cpp_api_database cimport KVVAL
from pyiode.iode_database.cpp_api_database cimport B_FileImportVar
from pyiode.iode_database.cpp_api_database cimport EXP_RuleExport
from pyiode.iode_database.cpp_api_database cimport RasExecute
from pyiode.iode_database.cpp_api_database cimport W_flush, W_close


class BinaryOperation(IntEnum):
    OP_ADD = CBinaryOperation.OP_ADD
    OP_SUB = CBinaryOperation.OP_SUB
    OP_MUL = CBinaryOperation.OP_MUL
    OP_DIV = CBinaryOperation.OP_DIV
    OP_POW = CBinaryOperation.OP_POW


cdef class Variables(CythonIodeDatabase):
    cdef bint ptr_owner
    cdef CppKDBVariables* database_ptr
    cdef IodeVarMode mode_
    cdef Period first_period_subset
    cdef Period last_period_subset

    def __cinit__(self, filepath: str=None) -> Variables:
        self.ptr_owner = False
        self.database_ptr = &cpp_global_variables
        self.abstract_db_ptr = &cpp_global_variables
        self.mode_ = IodeVarMode.VAR_MODE_LEVEL
        self.first_period_subset = None
        self.last_period_subset = None

    def __dealloc__(self):
        if self.ptr_owner and self.database_ptr is not NULL:
            del self.database_ptr
            self.database_ptr = NULL

    @staticmethod
    cdef Variables _from_ptr(CppKDBVariables* database_ptr = NULL, bint owner=False):
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

    def _get_periods_bounds(self) -> Tuple[int, int]:
        if self.first_period_subset is None:
            t_first_period: int = 0  
        else: 
            t_first_period: int = self._get_real_period_position(self.first_period_subset)
        
        if self.last_period_subset is None:
            t_last_period: int = self.get_sample().get_nb_periods() - 1
        else:
            t_last_period: int = self._get_real_period_position(self.last_period_subset)

        return t_first_period, t_last_period

    def _load(self, filepath: str):
        if self.database_ptr is not NULL:
            self.database_ptr.load(filepath.encode())
            self.mode_ = IodeVarMode.VAR_MODE_LEVEL
            self.first_period_subset: Period = None
            self.last_period_subset: Period = None

    def initialize_subset(self, cython_instance: Variables, pattern: str, copy: bool, 
                          first_period: Optional[Period], last_period: Optional[Period]) -> Variables:
        cdef CSample* c_sample
        cython_instance.database_ptr = cython_instance.abstract_db_ptr = self.database_ptr.subset(pattern.encode(), <bint>copy)
        cython_instance.ptr_owner = True
        cython_instance.mode_ = IodeVarMode.VAR_MODE_LEVEL
        cython_instance.first_period_subset = first_period
        cython_instance.last_period_subset = last_period
        return cython_instance

    def _get_whole_sample(self) -> Sample:
        cdef CSample* c_sample 
        if self.get_is_detached():
            c_sample = self.database_ptr.get_sample()
        else:
            c_sample = cpp_global_variables.get_sample()
        return Sample._from_ptr(c_sample, <bint>False)

    def _maybe_update_subset_sample(self):
        if self.first_period_subset is None and self.last_period_subset is None:
            return 
    
        whole_db_sample: Sample = self._get_whole_sample()
        global_first_period = whole_db_sample.get_start()
        global_last_period = whole_db_sample.get_end()

        if self.first_period_subset is not None and global_first_period is not None:
            if self.first_period_subset < global_first_period:
                self.first_period_subset = global_first_period
            if self.first_period_subset > global_last_period:
                self.first_period_subset = None
                self.last_period_subset = None

        if self.last_period_subset is not None and global_last_period is not None:
            if self.last_period_subset > global_last_period:
                self.last_period_subset = global_last_period
            if self.last_period_subset < global_first_period:
                self.first_period_subset = None
                self.last_period_subset = None

    cdef int c_get_real_period_position(self, CPeriod c_period):
        """
        Get the position of a period in the Variables database sample (not the subset).
        """
        cdef CSample* c_sample
        if self.get_is_detached():
            c_sample = self.database_ptr.get_sample()
        else:
            c_sample = cpp_global_variables.get_sample()
        return c_sample.get_period_position(<CPeriod>c_period)

    def _get_real_period_position(self, period: Period) -> int:
        sample: Sample = self.get_sample()
        if sample.is_undefined():
            raise RuntimeError("The sample of the IODE Variables workspace is not defined")
        if period < sample.get_start() or period > sample.get_end():
            raise IndexError(f"The period '{period}' is outside the sample '{sample}'")
        return self.c_get_real_period_position(period.c_period)

    def _get_variable(self, name: str, key_periods: Union[Period, List[Period]]) -> Union[float, List[float]]:
        cdef int t
        cdef string c_name = name.encode('utf-8')
        cdef double cpp_value
        cdef vector[double] cpp_values

        # key_periods represents a unique period
        if isinstance(key_periods, Period):
            t = self._get_real_period_position(key_periods)
            cpp_value = self.database_ptr.get_var(c_name, t, self.mode_)
            return cpp_value if IODE_IS_A_NUMBER(cpp_value) else np.nan
        # key_periods represents a list of non-contiguous periods -> return a pandas Series
        if isinstance(key_periods, list):
            if not all(isinstance(p, Period) for p in key_periods):
                raise TypeError("key_periods must be a list of Cython Period objects")
            # convert Period objects to their corresponding integer positions
            key_periods: List[int] = [self._get_real_period_position(period) for period in key_periods]
            values = [self.database_ptr.get_var(c_name, <int>p, self.mode_) for p in key_periods]
            return [value if IODE_IS_A_NUMBER(value) else np.nan for value in values]

    def __add_var(self, name: str, value: Variables):
        cdef string c_name = name.encode()
        t_first, t_last = value._get_periods_bounds()
        # NOTE: 'value' can contains more than one variable as long as the variable named 'name' is present
        _c_add_var_from_other(c_name, self.database_ptr, value.database_ptr, t_first, t_last)

    def _add(self, name: str, values: Union[str, float, np.ndarray, Iterable[float], Variables]):
        cdef bint success
        cdef vector[double] cpp_values
        cdef int c_nb_periods
        cdef bytes b_name
        cdef string s_name
        cdef double[::1] numpy_data_memview
        cdef CKDBVariables* c_db_ptr = NULL
        
        # values is a LEC expression
        if isinstance(values, str):
            self.database_ptr.add(<string>(name.encode()), <string>values.encode())
        # values is a vector of float
        elif isinstance(values, float):
            cpp_values = [values] * self.get_sample().get_nb_periods()
            self.database_ptr.add(<string>(name.encode()), cpp_values)
        elif isinstance(values, np.ndarray):
            numpy_data_memview = values
            b_name = name.encode()
            s_name = b_name
            c_db_ptr = self.database_ptr.get_database()
            success = c_db_ptr.set_obj(s_name, &numpy_data_memview[0])
            if not success:
                raise RuntimeError(f"Cannot add variable '{name}' to the IODE Variables database")
        # values is a Variables object
        # NOTE: 'values' can contains more than one variable as long as the variable named 'name' is present
        elif isinstance(values, Variables):
            self.__add_var(name, values)
        # values is an iterable of float
        elif isinstance(values, Iterable):
            cpp_values = values
            self.database_ptr.add(<string>(name.encode()), cpp_values)

    def __copy_var(self, source_name: str, dest_name: str, t_first: int, t_last: int, value: Variables):
        cdef string c_source_name = source_name.encode()
        cdef string c_dest_name = dest_name.encode()
        value_t_first, value_t_last = value._get_periods_bounds()
        # NOTE: 'value' can contains more than one variable as long as the variable named 'name' is present
        _c_copy_var_content(c_dest_name, self.database_ptr, t_first, t_last, 
                            c_source_name, value.database_ptr, value_t_first, value_t_last)

    def _update_variable(self, name: str, pos: int, values, key_periods: Union[Period, tuple, list]):
        cdef int t
        cdef int t_first
        cdef int t_last
        cdef string c_name = name.encode('utf-8')
        cdef CKDBVariables* c_db_ptr = NULL
        cdef vector[double] cpp_values
        cdef double* var_ptr = NULL
        cdef double[::1] numpy_data_memview 

        c_db_ptr = self.database_ptr.get_database()

        if not self.contains(name):
            raise KeyError(f"Variable '{name}' not found in the IODE Variables database")

        # update the value for only one period 
        if isinstance(key_periods, Period):
            t = self._get_real_period_position(key_periods)
            if isinstance(values, float):
                KV_set(c_db_ptr, c_name, t, self.mode_, <double>values)
            # values is a LEC expression
            elif isinstance(values, str):
                self.database_ptr.update(c_name, <string>values.encode(), t, t) 
            elif isinstance(values, Variables):
                self.__copy_var(name, name, t, t, values)
        # update values for a contiguous range of periods
        elif isinstance(key_periods, tuple):
            if len(key_periods) != 2:
                raise TypeError("key_periods must be a tuple of 2 Period objects")
            first_period, last_period = key_periods
            if not isinstance(first_period, Period):
                raise TypeError("key_periods must be a tuple of 2 Cython Period objects.\n"
                                f"Got first_period of type {type(first_period)} instead.")
            if not isinstance(first_period, Period) or not isinstance(last_period, Period):
                raise TypeError("key_periods must be a tuple of 2 Cython Period objects.\n"
                                f"Got last_period of type {type(last_period)} instead.")
            t_first = self._get_real_period_position(first_period)
            t_last = self._get_real_period_position(last_period)
            # set same value for all periods in the range
            if isinstance(values, float):
                for t in range(t_first, t_last + 1): 
                    KV_set(c_db_ptr, c_name, t, self.mode_, <double>values)  
            # values is a LEC expression
            elif isinstance(values, str):
                self.database_ptr.update(c_name, <string>values.encode(), t_first, t_last) 
            # values is of type Variables
            elif isinstance(values, Variables):
                # NOTE: 'values' can contains more than one variable as long as the variable named 'name' is present
                source_name = name if values.size() > 1 else values.property_names()[0]
                self.__copy_var(source_name, name, t_first, t_last, values)
            # values is a numpy array AND the mode is VAR_MODE_LEVEL
            elif isinstance(values, np.ndarray) and self.mode_ == IodeVarMode.VAR_MODE_LEVEL:
                    nb_periods = t_last - t_first + 1
                    var_ptr = KVVAL(c_db_ptr, c_name, t_first)
                    numpy_data_memview = values
                    memcpy(var_ptr, &numpy_data_memview[0], nb_periods * sizeof(double))
            # assume values is an iterable
            else:
                for i, t in enumerate(range(t_first, t_last + 1)):
                    KV_set(c_db_ptr, c_name, t, self.mode_, <double>(values[i]))
        # update values for a list of periods
        elif isinstance(key_periods, list):
            if not all(isinstance(p, Period) for p in key_periods):
                raise TypeError("key_periods must be a list of Cython Period objects")             
            for p, v in zip(key_periods, values):
                t = self._get_real_period_position(p)
                KV_set(c_db_ptr, c_name, t, self.mode_, <double>v)

    def binary_op_scalar(self, other: float, op: BinaryOperation, copy_self: bool) -> Variables:
        cdef int i_op = int(op)
        cdef double c_value = other
        cdef CppKDBVariables* cpp_self_db = self.database_ptr

        t_first, t_last = self._get_periods_bounds()
        _c_operation_scalar(i_op, cpp_self_db, t_first, t_last, c_value)
        return self

    def binary_op_numpy(self, data: np.ndarray, op: BinaryOperation, names: List[str], nb_periods: int, copy_self: bool) -> Variables:
        cdef int i, t_first, t_last
        cdef int i_op = int(op)
        cdef double c_value
        cdef CppKDBVariables* c_database = NULL
        cdef double[::1] numpy_data_memview

        t_first, t_last = self._get_periods_bounds()
        nb_periods = t_last - t_first + 1
        c_database = self.database_ptr

        if len(names) == 1:
            name = names[0]
            numpy_data_memview = data
            _c_operation_one_var(i_op, c_database, <string>name.encode(), t_first, t_last, &numpy_data_memview[0])
        elif nb_periods == 1:
            numpy_data_memview = data
            _c_operation_one_period(i_op, c_database, t_first, &numpy_data_memview[0], len(data))
        else:
            for name, _data in zip(names, data):
                numpy_data_memview = _data
                _c_operation_one_var(i_op, c_database, <string>name.encode(), t_first, t_last, &numpy_data_memview[0])
        return self

    def binary_op_variables(self, cython_other: Variables, op: BinaryOperation, names: List[str], copy_self: bool) -> Variables:
        cdef int i_op = int(op)
        cdef CppKDBVariables* cpp_self_db = self.database_ptr
        cdef CppKDBVariables* cpp_other_db = cython_other.database_ptr

        t_first, t_last = self._get_periods_bounds()
        if len(names) == 1:
            name = names[0]
            other_name = cython_other.property_names()[0]
            _c_operation_between_two_vars(i_op, cpp_self_db, <string>name.encode(), <int>t_first, <int>t_last, 
                                          cpp_other_db, <string>other_name.encode(), <int>t_first, <int>t_last)
        else:
            for name in names:
                _c_operation_between_two_vars(i_op, cpp_self_db, <string>name.encode(), <int>t_first, <int>t_last, 
                                              cpp_other_db, <string>name.encode(), <int>t_first, <int>t_last)
        return self

    def from_numpy(self, data: np.ndarray, vars_names: List[str], new_vars: Set[str], t_first_period: int, t_last_period: int):
        cdef int var_pos
        cdef bytes b_name
        cdef char* c_name
        cdef double value
        cdef double* var_ptr = NULL
        cdef int i, j, t
        cdef int mode = <int>self.mode_
        cdef int nb_periods
        cdef CKDBVariables* db_ptr = self.database_ptr.get_database()
        
        if db_ptr is NULL:
            raise RuntimeError("The IODE Variables workspace has not been initialized")

        vars_t_first, vars_t_last = self._get_periods_bounds()
        if t_first_period < vars_t_first or t_last_period > vars_t_last:
            raise ValueError(f"Periods {t_first_period} to {t_last_period} are outside the sample bounds "
                             f"{vars_t_first} to {vars_t_last} of the Variables database")

        # add new variables if they do not exist
        for name in new_vars:
            # NOTE: Cython cannot directly convert a Python string to a C string, 
            #       so we need to use the encode() method to convert it to a bytes object first, 
            #       and then convert the bytes object to a C string using the syntax char_obj = bytes_obj.
            b_name = name.encode()
            c_name = b_name
            # add a new variable with all values set to IODE_NAN
            var_pos = KV_add(db_ptr, c_name)
            if var_pos < 0:
                raise RuntimeError(f"Cannot add variable '{name}' to the IODE Variables database")

        # declaring a C-contiguous array of 2D double
        # see https://cython.readthedocs.io/en/latest/src/userguide/numpy_tutorial.html#declaring-the-numpy-arrays-as-contiguous 
        cdef double[:, ::1] data_view = data

        # copy the values
        if self.mode_ == IodeVarMode.VAR_MODE_LEVEL:
            nb_periods = t_last_period - t_first_period + 1
            for i, name in enumerate(vars_names):
                # NOTE: Cython cannot directly convert a Python string to a C string, 
                #       so we need to use the encode() method to convert it to a bytes object first, 
                #       and then convert the bytes object to a C string using the syntax char_obj = bytes_obj.
                b_name = name.encode()
                if not self.database_ptr.contains(<string>b_name):
                    raise RuntimeError(f"Variable '{name}' not found in the IODE Variables database")
                var_ptr = KVVAL(db_ptr, <string>b_name, t_first_period)
                memcpy(var_ptr, &data_view[i, 0], nb_periods * sizeof(double))
        else:
            for i, name in enumerate(vars_names):
                b_name = name.encode()
                if not self.database_ptr.contains(<string>b_name):
                    raise RuntimeError(f"Variable '{name}' not found in the IODE Variables database")
                for j, t in enumerate(range(t_first_period, t_last_period + 1)):
                    value = data_view[i, j]
                    KV_set(db_ptr, <string>b_name, t, mode, value)

    def to_numpy(self) -> np.ndarray:
        cdef int i, t
        cdef double value
        cdef double* var_ptr = NULL
        cdef int mode = <int>self.mode_
        cdef int t_first_period
        cdef int t_last_period
        cdef int nb_periods
        cdef CKDBVariables* db_ptr = self.database_ptr.get_database()
        if db_ptr is NULL:
            raise RuntimeError("The IODE Variables workspace is empty")

        if not self.size():
            raise RuntimeError("The IODE Variables workspace is empty")

        t_first_period, t_last_period = self._get_periods_bounds()
        nb_periods = t_last_period - t_first_period + 1

        if nb_periods <= 0:
            raise RuntimeError("The sample of the Variables database to export is empty")

        # Copy values from the IODE Variables database to a Numpy 2D array
        data = np.empty((self.size(), nb_periods), dtype=np.float64)

        # declaring a C-contiguous array of 2D double
        # see https://cython.readthedocs.io/en/latest/src/userguide/numpy_tutorial.html#declaring-the-numpy-arrays-as-contiguous 
        cdef double[:, ::1] data_view = data
        
        cdef string c_name
        cdef vector[string] cpp_names = self.database_ptr.get_names()
        if self.mode_ == IodeVarMode.VAR_MODE_LEVEL:
            for i, c_name in enumerate(cpp_names):
                var_ptr = KVVAL(db_ptr, c_name, t_first_period)
                memcpy(&data_view[i, 0], var_ptr, nb_periods * sizeof(double))
        else:
            for i, c_name in enumerate(cpp_names):
                for t in range(t_first_period, t_last_period + 1):
                    value = KV_get(db_ptr, c_name, t, mode)
                    data_view[i, t - t_first_period] = value
        
        if self.size() == 1:
            data = data.reshape((nb_periods,))

        _NA = 0.9999 * NA
        data[data < _NA] = np.nan
        return data

    def get_mode(self) -> str:
        return VarsMode(<int>self.mode_).name

    def set_mode(self, value: int):
        self.mode_ = <IodeVarMode>value

    def get_first_period(self) -> Period:
        cdef CSample* c_sample
        if self.first_period_subset is not None:
            return self.first_period_subset 
        else:
            whole_db_sample: Sample = self._get_whole_sample()
            return whole_db_sample.get_start()

    def get_last_period(self) -> Period:
        cdef CSample* c_sample
        if self.last_period_subset is not None:
            return self.last_period_subset 
        else:
            whole_db_sample: Sample = self._get_whole_sample()
            return whole_db_sample.get_end()

    def get_is_subset_over_periods(self) -> bool:
        if self.first_period_subset is None and self.last_period_subset is None:
            return False
        
        whole_db_sample: Sample = self._get_whole_sample()
        global_first_period = whole_db_sample.get_start()
        global_last_period = whole_db_sample.get_end()
        if self.first_period_subset is not None: 
            if global_first_period is None:
                return True 
            return self.first_period_subset != global_first_period
        if self.last_period_subset is not None:
            if global_last_period is None:
                return True
            return self.last_period_subset != global_last_period
        
        return False

    def get_sample(self) -> Sample:
        whole_db_sample: Sample = self._get_whole_sample()
        if self.first_period_subset is None and self.last_period_subset is None:
            return whole_db_sample

        self._maybe_update_subset_sample()

        if self.first_period_subset is not None:
            first_period = self.first_period_subset
        else: 
            first_period = whole_db_sample.get_start()

        if self.last_period_subset is not None:
            last_period = self.last_period_subset
        else:
            last_period = whole_db_sample.get_end()

        if first_period is None or last_period is None:
            return Sample._from_ptr(NULL, <bint>True)
        else:        
            return Sample(str(first_period), str(last_period))

    def set_sample(self, from_period: str, to_period: str):
        if self.get_is_detached():
            self.database_ptr.set_sample(from_period.encode(), to_period.encode())
        else:
            cpp_global_variables.set_sample(from_period.encode(), to_period.encode())

    def get_threshold(self) -> float:
        return K_CMP_EPS

    def set_threshold(self, value: float) -> bool:
        res = B_DataCompareEps(str(value).encode())
        return res == 0            

    def periods_subset(self, from_period: str, to_period: str, as_float: bool) -> List[Union[str, float]]:
        cdef string s_from_period = from_period.encode('utf-8')
        cdef string s_to_period = to_period.encode('utf-8')
        if as_float:
            return self.database_ptr.get_list_periods_as_float(s_from_period, s_to_period)
        else:
            return [cpp_period.decode() for cpp_period in 
                    self.database_ptr.get_list_periods(s_from_period, s_to_period)]

    def copy_from(self, input_files: str, from_period: str, to_period: str, names: str):
        cdef string s_input_files = input_files.encode('utf-8')
        cdef string s_from_period = from_period.encode('utf-8')
        cdef string s_to_period = to_period.encode('utf-8')
        cdef string s_names = names.encode('utf-8')
        self.database_ptr.copy_from(s_input_files, s_from_period, s_to_period, s_names)

    def low_to_high(self, type_of_series: int, method: str, filepath: str, var_list: str):
        cpp_low_to_high(<IodeLowToHigh>type_of_series, <char>ord(method), filepath.encode(), var_list.encode())

    def high_to_low(self, type_of_series: int, filepath: str, var_list: str):
        cpp_high_to_low(<IodeHighToLow>type_of_series, filepath.encode(), var_list.encode())

    def extrapolate(self, method: int, from_period: str, to_period: str, variables_list: str):
        self.database_ptr.extrapolate(<VariablesInitialization>method, from_period.encode(), to_period.encode(), 
                                      variables_list.encode())

    def seasonal_adjustment(self, input_file: str, eps_test: float, series: str):
        self.database_ptr.seasonal_adjustment(input_file.encode(), series.encode(), eps_test)

    def trend_correction(self, input_file: str, lambda_: float, series: str, log: bool):
        self.database_ptr.trend_correction(input_file.encode(), lambda_, series.encode(), <bint>log)

    def execute_RAS(self, pattern: str, xdim: str, ydim: str, ref_period: str, sum_period: str, 
                    max_nb_iterations: int, epsilon: float) -> bool:
        cdef bytes b_pattern = pattern.encode('utf-8')
        cdef bytes b_xdim = xdim.encode('utf-8')
        cdef bytes b_ydim = ydim.encode('utf-8')
        cdef char* c_pattern = b_pattern
        cdef char* c_xdim = b_xdim
        cdef char* c_ydim = b_ydim
        cdef bytes b_ref_period = ref_period.encode('utf-8')
        cdef bytes b_sum_period = sum_period.encode('utf-8')
        cdef CPeriod* c_ref_period = new CPeriod(<string>b_ref_period)
        cdef CPeriod* c_sum_period = new CPeriod(<string>b_sum_period)
        
        # Note: c_ref_period and c_sum_period are freed at the end of RasExecute
        res: int = RasExecute(c_pattern, c_xdim, c_ydim, c_ref_period, c_sum_period, 
                              max_nb_iterations, epsilon)
        del c_ref_period
        del c_sum_period
        return res == 0

    @classmethod
    def convert_file(cls, args: str) -> int:
        return B_FileImportVar(args.encode('utf-8'))

    @classmethod
    def export_as_file(cls, variables_file: str, rule_file: str, save_file: str, export_format: int, from_period: str, to_period: str, 
                       comments_file: str, nan_value: str, separator: str, debug_file: str) -> int:
        if debug_file is None:
            debug_file = ''
        res = EXP_RuleExport(debug_file.encode('utf-8'), rule_file.encode('utf-8'), save_file.encode('utf-8'), 
                             variables_file.encode('utf-8'), comments_file.encode('utf-8'), 
                             from_period.encode('utf-8'), to_period.encode('utf-8'), nan_value.encode('utf-8'), 
                             separator.encode('utf-8'), export_format)
        # -- make sure the debug file is written at the end of the function --
        # note: W_flush() and W_close() are not called in the C function EXP_RuleExport
        if debug_file:
            W_flush()
            W_close()
        return res

    def _str_table(self, names: List[str], periods: List[str]) -> Dict[str, List[float]]:
        cdef t
        columns = {"name": names}
        for str_period in periods:
            period = Period(str_period)
            t = self._get_real_period_position(period)
            columns[str_period] = [self.database_ptr.get_var(<str>name.encode('utf-8'), <int>t, self.mode_) 
                                   for name in names]
        return columns

    def __hash__(self) -> int:
        return hash_value(dereference(self.database_ptr))
