# distutils: language = c++

import warnings
from pathlib import Path
from collections.abc import Iterable
from typing import Union, Tuple, List, Optional, Any
if sys.version_info.minor >= 11:
    from typing import Self
else:
    Self = Any

cimport cython
from libcpp.string cimport string
from libcpp.vector cimport vector
from cython.operator cimport dereference
from pyiode.common cimport IODE_NAN, IodeVarMode, IodeLowToHigh, IodeHighToLow, VariablesInitialization
from pyiode.iode_database.cpp_api_database cimport hash_value
from pyiode.iode_database.cpp_api_database cimport K_CMP_EPS
from pyiode.iode_database.cpp_api_database cimport B_DataCompareEps
from pyiode.iode_database.cpp_api_database cimport IodeGetVector, IodeSetVector, IodeCalcSamplePosition
from pyiode.iode_database.cpp_api_database cimport KDBVariables as CKDBVariables
from pyiode.iode_database.cpp_api_database cimport Variables as cpp_global_variables
from pyiode.iode_database.cpp_api_database cimport low_to_high as cpp_low_to_high
from pyiode.iode_database.cpp_api_database cimport high_to_low as cpp_high_to_low
from pyiode.iode_database.cpp_api_database cimport KCPTR, KIPTR, KLPTR, KVPTR
from pyiode.iode_database.cpp_api_database cimport B_FileImportVar
from pyiode.iode_database.cpp_api_database cimport EXP_RuleExport
from pyiode.iode_database.cpp_api_database cimport W_flush, W_close

from iode.util import check_filepath

KeyName = Union[str, List[str]]
KeyPeriod = Union[None, str, List[str], slice]
KeyVariable = Union[KeyName, Tuple[KeyName, KeyPeriod]]


cdef inline _df_to_ws_pos(df, start_period: str, last_period:str, int* la_pos, int* ws_pos, int* lg):
    '''
    Calls C API fonction IodeCalcSamplePosition() to determine which elements of array to copy into KV_WS and 
    at which position in the KV_WS sample.
    '''
    IodeCalcSamplePosition(_cstr(start_period), _cstr(last_period), la_pos, ws_pos, lg)

# TODO: (ALD) - check in the Cython 3.0 documentation if there is no other way to create and initialize a Numpy array
#             - rewrite this function to be based on the C++ API
cdef inline _iodevar_to_ndarray(char* name, bint copy = True):
    """
    Create an numpy array from the content of an IODE variable (KV_WS[name]). 
    The ndarray data may be either a newly allocated vector, or may point to the IODE memory.
    Be aware that IODE data can be relocated in memory when creating new variables.

    Parameters
    ----------
    name: C char* 
        Name of the IODE variable to read 
    copy: bool, optional
        if True, a pointer to the IODE data is placed in the resulting ndarray.
        if False, the data is allocated.

    Returns
    -------
    out: ndarray 
        1-dim numpy array containing the values of variables[name]
    """  
    cdef cnp.npy_intp shape[1]   # int of the size of a pointer (32 ou 64 bits) See https://numpy.org/doc/stable/reference/c-api/dtype.html
    cdef int lg
    cdef double *data_ptr = IodeGetVector(name, &lg)
    
    shape[0] = <cnp.npy_intp> lg  # vector length
    if data_ptr != NULL:
        # Create an array wrapper around data_ptr 
        # See https://numpy.org/doc/stable/reference/c-api/array.html?highlight=pyarray_simplenewfromdata#c.PyArray_SimpleNewFromData
        vararray = cnp.PyArray_SimpleNewFromData(1, shape, cnp.NPY_DOUBLE, data_ptr) # See https://numpy.org/doc/stable/reference/c-api/array.html?highlight=pyarray_simplenewfromdata#c.PyArray_SimpleNewFromData
        
        if(copy):
            # Duplicates if inmemory == 0 (?) => ch³anges to vararray will NOT modify KV_WS
            vararray = vararray.copy()
        vararray[vararray < -1.0e37] = np.nan
    else:
        vararray = np.ndarray(lg, dtype='double')        
        #vararray.fill(0)        
        vararray.fill(np.nan)
    return vararray

# TODO: (ALD) - check in the Cython 3.0 documentation if there is no other way to create and initialize a Numpy array
#             - rewrite this function to be based on the C++ API
def _numpy_array_to_ws(data, vars_names: Iterable[str], periods_list: Iterable[str]):
    cdef int df_pos
    cdef int ws_pos
    cdef int lg

    nb_periods_ = len(periods_list)
    lg = nb_periods_
    start_period, last_period = periods_list[0], periods_list[-1]

    # calculate:
    #   - df_pos: the position of the first element of data to be copied into WS
    #   - ws_pos: the position in KV_WS sample where to copy df[...pos]
    #   - the nb of values to be copied 
    IodeCalcSamplePosition(_cstr(start_period), _cstr(last_period), &df_pos, &ws_pos, &lg)

    # replace numpy/pandas NaN by IODE NaN
    data = np.nan_to_num(data, nan=NA)

    # copy each line of array into KV_WS on the time intersection of df and KV_WS
    # values = <double*>cnp.PyArray_DATA(df[vars_names[0]].data)
    if np.issubdtype(data.dtype, np.floating):
        if data.data.c_contiguous:
            for name, row_data in zip(vars_names, data):
                # Save the vector of doubles in KV_WS
                values = < double * > cnp.PyArray_DATA(row_data)
                IodeSetVector(_cstr(name), values, df_pos, ws_pos, lg)
        else:
            for name, row_data in zip(vars_names, data):
                row_data = row_data.copy()  # copy if non contiguous
                # Save the vector of doubles in KV_WS
                values = < double * > cnp.PyArray_DATA(row_data)
                IodeSetVector(_cstr(name), values, df_pos, ws_pos, lg)
    else:
        for name, row_data in zip(vars_names, data):
            row_data = row_data.astype("double") # astype creates a copy
            # Save the vector of doubles in KV_WS
            values = <double*>cnp.PyArray_DATA(row_data)
            IodeSetVector(_cstr(name), values, df_pos, ws_pos, lg)

def _ws_to_numpy_array(vars_list: List[str], nb_periods: int) -> np.ndarray:
    # Copy values from the IODE Variables database to a Numpy 2D array
    data = np.empty((len(vars_list), nb_periods), dtype=np.float64)
    for i, name in enumerate(vars_list):
        # cpp_values_ptr = self.database_ptr.get_var_ptr(name.encode())
        # data[i] = [cpp_values_ptr[t] for t in range(0, nb_periods_)]
        data[i] = _iodevar_to_ndarray(_cstr(name), False)
    return data


class VarPositionalIndexer:
    def __init__(self, database):
        self.database = database
    
    def _check_index(self, index: Union[int, Tuple[int, int]]) -> Tuple[int, int]:
        if isinstance(index, int):
            pos, t = index, None
        if isinstance(index, tuple):
            pos, t = index
        if pos < 0:
            pos += len(self.database)
        if not (0 <= pos < len(self.database)):
            raise IndexError(f"Index {pos} out of range")
        if t is not None: 
            if t < 0:
                t += self.database.nb_periods
            if not (0 <= t < self.database.nb_periods):
                raise IndexError(f"Period index {t} out of range")
        return pos, t

    def __getitem__(self, index: Union[int, Tuple[int, int]]):
        pos, t = self._check_index(index)
        return self.database._get_variable(pos, t)
    
    def __setitem__(self, index: Union[int, Tuple[int, int]], value):
        pos, t = self._check_index(index)
        self.database._set_variable(pos, value, t)


@cython.final
cdef class Variables(_AbstractDatabase):
    r"""
    IODE Variables database. 

    Attributes
    ----------
    filename: str
    description: str
    mode: int
    sample: Sample
    nb_periods: int
    periods: list(str)
    periods_as_float: list(float)

    Parameters
    ----------
    filepath: str, optional
        file containing the IODE variables to load.

    Returns
    -------
    Variables

    Examples
    --------
    >>> from iode import variables, SAMPLE_DATA_DIR
    >>> variables.load(f"{SAMPLE_DATA_DIR}/fun.var")      # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
    Loading .../fun.var
    394 objects loaded 
    >>> len(variables)
    394
    >>> variables           # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
    Workspace: Variables
    nb variables: 394
    filename: ...\tests\data\fun.var
    description: Modèle fun - Simulation 1
    sample: 1960Y1:2015Y1
    mode: LEVEL
    <BLANKLINE>
     name       1960Y1  1961Y1  1962Y1  1963Y1  1964Y1  1965Y1  ...     2009Y1  2010Y1  2011Y1  2012Y1  2013Y1  2014Y1  2015Y1
    ACAF            na      na      na      na      na      na  ...     -37.46  -37.83  -44.54  -55.56  -68.89  -83.34  -96.41
    ACAG            na      na      na      na      na      na  ...      27.23   28.25   29.28   30.32   31.37   32.42   33.47
    AOUC            na    0.25    0.25    0.26    0.28    0.29  ...       1.29    1.31    1.33    1.36    1.39    1.42    1.46
    AOUC_           na      na      na      na      na      na  ...       1.23    1.25    1.27    1.30    1.34    1.37    1.41
    AQC           0.22    0.22    0.22    0.23    0.24    0.25  ...       1.45    1.46    1.48    1.51    1.56    1.61    1.67
    ...            ...     ...     ...     ...     ...     ...  ...        ...     ...     ...     ...     ...     ...     ...
    ZJ              na      na      na      na      na      na  ...       1.49    1.51    1.53    1.56    1.59    1.63    1.67
    ZKF           0.80    0.81    0.82    0.81    0.83    0.82  ...       0.87    0.87    0.87    0.87    0.87    0.87    0.87
    ZKFO          1.00    1.00    1.00    1.00    1.00    1.00  ...       1.02    1.02    1.02    1.02    1.02    1.02    1.02
    ZX            0.00    0.00    0.00    0.00    0.00    0.00  ...       0.00    0.00    0.00    0.00    0.00    0.00    0.00
    ZZF_          0.69    0.69    0.69    0.69    0.69    0.69  ...       0.69    0.69    0.69    0.69    0.69    0.69    0.69
    <BLANKLINE>
    """
    cdef bint ptr_owner
    cdef CKDBVariables* database_ptr
    cdef IodeVarMode mode_

    def __cinit__(self, filepath: str = None) -> Variables:
        self.database_ptr = NULL
        self.abstract_db_ptr = NULL
        self.ptr_owner = False
        self.mode_ = IodeVarMode.VAR_MODE_LEVEL

    def __init__(self, filepath: str = None):
        # Prevent accidental instantiation from normal Python code
        # since we cannot pass a struct pointer into a Python constructor.
        raise TypeError("This class cannot be instantiated directly.")

    def __dealloc__(self):
        # if self.database_ptr points to the C++ global instance Variables 
        # which does not need to be manually deleted 
        if self.ptr_owner and self.database_ptr is not NULL:
            del self.database_ptr
            self.database_ptr = NULL

    # see https://cython.readthedocs.io/en/stable/src/userguide/extension_types.html#instantiation-from-existing-c-c-pointers 
    @staticmethod
    cdef Variables _from_ptr(CKDBVariables* database_ptr = NULL, bint owner=False):
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
        return wrapper

    # TODO: implement KDBAbstract::load() method (for global KDB only)
    def _load(self, filepath: str):
        cdef CKDBVariables* kdb = new CKDBVariables(filepath.encode())
        del kdb

    def _subset(self, pattern: str, copy: bool) -> Variables:
        cdef Variables subset_db = Variables.__new__(Variables)
        subset_db.database_ptr = subset_db.abstract_db_ptr = self.database_ptr.subset(pattern.encode(), <bint>copy)
        subset_db.mode_ = IodeVarMode.VAR_MODE_LEVEL
        return subset_db

    def _unfold_key(self, key: KeyVariable) -> Tuple[KeyName, Optional[str, Tuple[int, int], List[str]]]:
        cdef CSample* c_sample

        # no selection on periods
        if not isinstance(key, tuple):
            return super()._unfold_key(key), None
        # key = names, periods
        else:
            if len(key) > 2:
                raise ValueError(f"variables[...]: Expected maximum 2 arguments ('names' and 'periods'). "
                                 f"Got {len(key)} arguments")
            names, periods_ = key

            # get selection on Variable name(s)
            names = super()._unfold_key(names) 
            
            # periods_ represents a unique period or a contiguous range of periods
            if isinstance(periods_, str): 
                # periods_ represents a contiguous range of periods
                if ':' in periods_:
                    first_period, last_period = periods_.split(':')
                    if not first_period:
                        first_period = None
                    if not last_period:
                        last_period = None
                    periods_ = slice(first_period, last_period)
                # periods_ represents a unique period
                else:
                    return names, periods_
            
            # check if periods_ is a list of periods
            if isinstance(periods_, Iterable) and all(isinstance(period_, (str, Period)) for period_ in periods_):
                return names, [str(period_) if isinstance(period_, Period) else period_ for period_ in periods_]
            
            # convert slice to a (start, end) tuple or a list of periods if step is not None
            if isinstance(periods_, slice):
                sample = self.sample
                first_period = sample.start if periods_.start is None else periods_.start
                last_period = sample.end if periods_.stop is None else periods_.stop
                if isinstance(first_period, Period):
                    first_period = str(first_period)
                if isinstance(last_period, Period):
                    last_period = str(last_period)
                if periods_.step is not None:
                    return names, self.periods_subset(first_period, last_period)[::periods_.step]
                else:
                    c_sample = self.database_ptr.get_sample()
                    t_first = c_sample.get_period_position(<string>first_period.encode())
                    t_last = c_sample.get_period_position(<string>last_period.encode())
                    return names, (t_first, t_last)
            else:
                # wrong type for periods_
                raise TypeError(f"variables[names, periods]: 'periods' must represent either a period (str), a list of periods "
                                f"or a slice of periods (start_period:last_period). 'periods' is of type {type(periods_).__name__}.")

    @property
    def i(self) -> VarPositionalIndexer:
        """
        Allow to select the ith variable in the database.

        Examples
        --------
        >>> from iode import variables, SAMPLE_DATA_DIR
        >>> variables.load(f"{SAMPLE_DATA_DIR}/fun.var")        # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Loading .../fun.var
        394 objects loaded
        >>> # get the value of the first period of the first variable
        >>> variables.i[0, 0]
        nan
        
        >>> # get the value of the last period of the last variable
        >>> variables.i[-1, -1]
        0.68840039
        >>> # update the value of the first period of the first variable
        >>> variables.i[0, 0] = 3.0
        >>> variables.i[0, 0]
        3.0
        >>> # update the value of the last period of the last variable
        >>> variables.i[-1, -1] = 3.0
        >>> variables.i[-1, -1]
        3.0
        """
        return VarPositionalIndexer(self)

    def _get_variable(self, key: Union[str, int], periods_: Union[str, int, List[str]]) -> Union[float, List[float]]: 
        cdef double cpp_value
        cdef vector[double] cpp_values
        # periods_ represents all periods
        if periods_ is None:
            if isinstance(key, int):
                cpp_values = self.database_ptr.get(<int>key)
            else:
                cpp_values = self.database_ptr.get(<string>(key.encode()))
            return [value if IODE_IS_A_NUMBER(value) else np.nan for value in cpp_values]
        # periods_ represents a unique period 
        elif isinstance(periods_, str):
            if isinstance(key, int):
                raise TypeError("variables[key, period]: 'key' must be of type str when 'period' is of type str.")
            else:
                cpp_value = self.database_ptr.get_var(<string>(key.encode()), <string>periods_.encode(), self.mode_)
                return cpp_value if IODE_IS_A_NUMBER(cpp_value) else np.nan
        # periods_ represents a unique period 
        elif isinstance(periods_, int):
            if isinstance(key, int):
                cpp_value = self.database_ptr.get_var(<int>key, <int>periods_, self.mode_)
            else:
                cpp_value = self.database_ptr.get_var(<string>(key.encode()), <int>periods_, self.mode_)
            return cpp_value if IODE_IS_A_NUMBER(cpp_value) else np.nan
        # periods_ represents a contiguous range of periods
        elif isinstance(periods_, tuple):
            t_first, t_last = periods_
            if isinstance(key, int):
                values = [self.database_ptr.get_var(<int>key, <int>t, self.mode_) for t in range(t_first, t_last+1)]
            else:
                values = [self.database_ptr.get_var(<string>(key.encode()), <int>t, self.mode_) for t in range(t_first, t_last+1)]
            return [value if IODE_IS_A_NUMBER(value) else np.nan for value in values]
        # periods_ represents a range/list of periods
        elif isinstance(periods_, list):
            if isinstance(key, int):
                raise TypeError("variables[key, periods]: 'key' must be of type str when 'periods' is of type list.")
            else:
                values = [self.database_ptr.get_var(<string>(key.encode()), <string>period_.encode(), self.mode_) for period_ in periods_]
                return [value if IODE_IS_A_NUMBER(value) else np.nan for value in values]
        else:
            raise TypeError("Wrong selection of periods. Expected None or value of type str, int, or list(str). "
                            f"Got value of type {type(periods_).__name__} instead.")

    # overriden for Variables
    def __getitem__(self, key) -> Union[float, List[float], Variables]:
        r"""
        Return the (subset of) variable(s) referenced by `key`.

        The `key` can represent a single object name (e.g. "ACAF") or a list of object names ("ACAF;ACAG;AOUC") 
        or a pattern (e.g. "A*") or a list of sub-patterns (e.g. "A*;*_").
        
        If the `key` represents a list of object names or of sub-patterns, each name or sub-pattern is separated 
        by a `separator` character which is either a whitespace ` `, or a comma `,`, or a semi-colon `;`, or a 
        tabulation `\t`, or a newline `\n`.

        A (sub-)`pattern` is a list of characters representing a group of object names. 
        It includes some special characters which have a special meaning:
        
            - `*` : any character sequence, even empty
            - `?` : any character (one and only one)
            - `@` : any alphanumerical char [A-Za-z0-9]
            - `&` : any non alphanumerical char
            - `|` : any alphanumeric character or none at the beginning and end of a string 
            - `!` : any non-alphanumeric character or none at the beginning and end of a string 
            - `\` : escape the next character

        Note that the `key` can contain references to IODE lists which are prefixed with the symbol `$`.

        Parameters
        ----------
        key: str or list(str)
            (the list of) name(s) of the variable(s) to get.
            The list of variables to get can be specified by a pattern or by a list of sub-patterns (e.g. "A*;*_").

        Returns
        -------
        Single variable or a subset of the database.

        Examples
        --------
        >>> from iode import SAMPLE_DATA_DIR
        >>> from iode import variables, NA
        >>> variables.load(f"{SAMPLE_DATA_DIR}/fun.var")        # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Loading .../fun.var
        394 objects loaded
        >>> variables.sample
        Sample("1960Y1:2015Y1")

        >>> # -------- a) get one Variable --------
        >>> # get the variable values for the whole sample
        >>> variables["ACAF"]                       # doctest: +ELLIPSIS 
        [nan, nan, ..., -83.34062511080091, -96.41041982848331]
        >>> # get the variable value for a specific period
        >>> variables["ACAF", "1990Y1"]
        23.771
        >>> # get the variable values for range of periods (using a Python slice)
        >>> variables["ACAF", "1990Y1":"2000Y1"]    # doctest: +ELLIPSIS 
        [23.771, 26.240999, ..., 13.530404919696034, 10.046610792200543]
        >>> # same as above but with the colon ':' inside the periods range string
        >>> variables["ACAF", "1990Y1:2000Y1"]      # doctest: +ELLIPSIS 
        [23.771, 26.240999, ..., 13.530404919696034, 10.046610792200543]

        >>> # b) -------- get a subset of the Variables database using a pattern --------
        >>> variables_subset = variables["A*"]
        >>> variables_subset.names
        ['ACAF', 'ACAG', 'AOUC', 'AOUC_', 'AQC']
        >>> # get the variable values for a specific period
        >>> variables["A*", "1990Y1"]
        [23.771, -28.1721855713507, 1.0, 0.9373591502749314, 1.0]
        >>> # get the variable values for range of periods (using a Python slice)
        >>> variables["A*", "1990Y1":"2000Y1"]      # doctest: +ELLIPSIS 
        [[23.771, 26.240999, 30.159, ..., 1.2031082, 1.3429699656745855, 1.3386028553645442]]

        >>> # c) -------- get a subset of the Variables database using a list of names --------
        >>> variables_subset = variables[["ACAF", "ACAG", "AQC", "BQY", "BVY"]]
        >>> variables_subset.names
        ['ACAF', 'ACAG', 'AQC', 'BQY', 'BVY']
        >>> # get the variable values for a specific period
        >>> variables[["ACAF", "ACAG", "AQC", "BQY", "BVY"], "1990Y1"]
        [23.771, -28.1721855713507, 1.0, -34.099998, -34.099997]
        >>> # get the variable values for range of periods (using a Python slice)
        >>> variables[["ACAF", "ACAG", "AQC", "BQY", "BVY"], "1990Y1":"2000Y1"]      # doctest: +ELLIPSIS 
        [[23.771, 26.240999, 30.159, ..., 140.73978, 144.8587818455608, 150.05335230584103]]
        """
        names, periods_ = self._unfold_key(key)
        # names represents a single Variable
        if len(names) == 1:
            return self._get_variable(names[0], periods_)
        # names represents a selection of Variables
        elif periods_ is None:
            names = ';'.join(names)
            return self._subset(names, copy=False)
        else:
            return [self._get_variable(name, periods_) for name in names]

    def _convert_values(self, values, nb_periods) -> Union[str, float, List[float]]:
        # value is a LEC expression
        if isinstance(values, str):
            return values
        # same value for all periods to be set    
        if isinstance(values, int):
            values = float(values)
        if isinstance(values, float):
            value = NA if np.isnan(values) else values
            return value if nb_periods == 1 else [value] * nb_periods
        # iterable of int/float
        if isinstance(values, Iterable) and all(isinstance(item, (int, float)) for item in values):
            if len(values) == nb_periods:
                return [NA if np.isnan(value) else float(value) for value in values]
            else:
                raise ValueError(f"variables[key] = values: Expected 'values' to be a vector of length {nb_periods} " 
                                 f"but got vector of length {len(values)}.")    
        # wrong type for 'value'
        raise TypeError("variables[key] = value(s): Expected a float or a list of float as 'value(s)'. "
                            f"Got 'value(s)' of type {type(values).__name__}") 

    def _add(self, name: str, values: Union[str, float, List[float]]):
        cdef vector[double] cpp_values

        if not isinstance(name, str):
            raise TypeError(f"'name': Expected value of type string. Got value of type {type(name).__name__}")

        # raise an error if values is a vector and len(values) != self.nb_periods
        values = self._convert_values(values, self.nb_periods)
        # values is a LEC expression
        if isinstance(values, str):
            self.database_ptr.add(<string>(name.encode()), <string>values.encode())
        # values is a vector of float
        else:
            cpp_values = [<double>value_ for value_ in values]
            self.database_ptr.add(<string>(name.encode()), cpp_values)

    def _update(self, key: Union[str, int], values: Union[str, float, List[float]]):
        cdef vector[double] cpp_values

        # raise an error if values is a vector and len(values) != self.nb_periods
        values = self._convert_values(values, self.nb_periods)
        # values is a LEC expression
        if isinstance(values, str):
            if isinstance(key, int):
                self.database_ptr.update(<int>key, <string>values.encode())
            else:
                self.database_ptr.update(<string>(key.encode()), <string>values.encode())
        # values is a vector of float
        else:
            cpp_values = [<double>value_ for value_ in values]
            if isinstance(key, int):
                self.database_ptr.update(<int>key, cpp_values)
            else:
                self.database_ptr.update(<string>(key.encode()), cpp_values)

    def _set_variable(self, key: Union[str, int], values: Any, periods_: Optional[str, int, Tuple[int, int], List[str]]):
        cdef vector[double] cpp_values

        if isinstance(key, str):
            key = key.strip()
        
        # new Variable -> raise an error if values is a vector and len(values) != self.nb_periods
        if isinstance(key, str) and key not in self:
            self._add(key, values)
        # update a Variable
        else:
            # update values for the whole sample
            # raise an error if values is a vector and len(values) != self.nb_periods
            if periods_ is None:
                self._update(key, values)
            # update the value for only one period 
            elif isinstance(periods_, (str, int)):
                if isinstance(key, int) and isinstance(periods_, str):
                    raise TypeError("variables[key, period] = value: Expected 'key' to be a string.")
                if isinstance(key, str) and isinstance(periods_, int):
                    raise TypeError("variables[key, period] = value: Expected 'period' to be a string.")
                value = self._convert_values(values, 1)
                if isinstance(key, int):
                    self.database_ptr.set_var(<int>key, <int>periods_, <double>value, self.mode_)
                else:
                    self.database_ptr.set_var(<string>(key.encode()), <string>periods_.encode(), <double>value, self.mode_)
            # update values for a contiguous range of periods
            elif isinstance(periods_, tuple):
                first_period, last_period = periods_
                nb_periods = last_period - first_period + 1
                # values is a LEC expression
                if isinstance(values, str):
                    if isinstance(key, int):
                        self.database_ptr.update(<int>(key), <string>values.encode(), <int>first_period, <int>last_period) 
                    else:   
                        self.database_ptr.update(<string>(key.encode()), <string>values.encode(), <int>first_period, <int>last_period)
                else:
                    values = self._convert_values(values, nb_periods)
                    cpp_values = [<double>value for value in values]
                    if isinstance(key, int):
                        self.database_ptr.update(<int>(key), cpp_values, <int>first_period, <int>last_period)
                    else:
                        self.database_ptr.update(<string>(key.encode()), cpp_values, <int>first_period, <int>last_period)
            # update values for a list of periods
            else:
                if isinstance(key, int):
                    raise TypeError("variables[key, periods] = lec where 'key' is an integer is not implemented.")
                if isinstance(values, str):
                    raise NotImplementedError(f"Case variables[key, periods] = lec where '{periods_}' does not represents "
                                              "a contiguous range of periods is not implemented")
                else:
                    values = self._convert_values(values, len(periods_))
                    for period, value in zip(periods_, values):
                        self.database_ptr.set_var(<string>(key.encode()), <string>period.encode(), <double>value, self.mode_)

    # overriden for Variables
    def __setitem__(self, key, value):
        r"""
        Update/add a (subset of) variable(s) referenced by `key` from/to the Variables database.

        The `key` can represent a single object name (e.g. "ACAF") or a list of object names ("ACAF;ACAG;AOUC") 
        or a pattern (e.g. "A*") or a list of sub-patterns (e.g. "A*;*_").
        
        If the `key` represents a list of object names or of sub-patterns, each name or sub-pattern is separated 
        by a `separator` character which is either a whitespace ` `, or a comma `,`, or a semi-colon `;`, or a 
        tabulation `\t`, or a newline `\n`.

        A (sub-)`pattern` is a list of characters representing a group of object names. 
        It includes some special characters which have a special meaning:
        
            - `*` : any character sequence, even empty
            - `?` : any character (one and only one)
            - `@` : any alphanumerical char [A-Za-z0-9]
            - `&` : any non alphanumerical char
            - `|` : any alphanumeric character or none at the beginning and end of a string 
            - `!` : any non-alphanumeric character or none at the beginning and end of a string 
            - `\` : escape the next character

        Note that the `key` can contain references to IODE lists which are prefixed with the symbol `$`.

        Parameters
        ----------
        key: str or list(str)
            (the list of) name(s) of the variable(s) to update/add.
            The list of variables to update/add can be specified by a pattern or by a list of sub-patterns 
            (e.g. "A*;*_").
        value: int, float, tuple(int, float), list(int, float), str or list of any of those
            If int, the value is first converted to a float and then used for all periods.
            If float, the value is used for all periods.
            If tuple or list, the number of elements must be equal to the number of periods. 
            Each element is then used for the corresponding period.
            If str, the value is interpreted as a LEC expression and is evaluated for each period.

        Examples
        --------
        >>> import numpy as np
        >>> from iode import SAMPLE_DATA_DIR
        >>> from iode import variables, NA
        >>> variables.load(f"{SAMPLE_DATA_DIR}/fun.var")        # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Loading .../fun.var
        394 objects loaded
        
        >>> # a) -------- add one variable --------
        >>> # 1) same value for all periods
        >>> variables["A0"] = NA
        >>> variables["A0"]                     # doctest: +ELLIPSIS 
        [nan, nan, ..., nan, nan]
        >>> # or equivalently
        >>> variables["A0"] = np.nan
        >>> variables["A0"]                     # doctest: +ELLIPSIS 
        [nan, nan, ..., nan, nan]
        >>> # 2) vector (list) containing a specific value for each period
        >>> variables["A1"] = list(range(variables.nb_periods))
        >>> variables["A1"]                     # doctest: +ELLIPSIS 
        [0.0, 1.0, 2.0, ..., 53.0, 54.0, 55.0]
        >>> # 3) LEC expression
        >>> variables["A2"] = "t + 10"
        >>> variables["A2"]                     # doctest: +ELLIPSIS 
        [10.0, 11.0, 12.0, ..., 63.0, 64.0, 65.0]

        >>> # b) -------- update one variable --------
        >>> # 1) update all values of a Variable
        >>> variables["ACAF"]                   # doctest: +ELLIPSIS 
        [nan, nan, ..., -83.34062511080091, -96.41041982848331]
        >>> # 1.I) same value for all periods
        >>> variables["ACAF"] = NA
        >>> variables["ACAF"]                   # doctest: +ELLIPSIS 
        [nan, nan, ..., nan, nan]
        >>> # or equivalently
        >>> variables["ACAF"] = np.nan
        >>> variables["ACAF"]                   # doctest: +ELLIPSIS 
        [nan, nan, ..., nan, nan]
        >>> # 1.II) vector (list) containing a specific value for each period
        >>> variables["ACAF"] = list(range(variables.nb_periods))
        >>> variables["ACAF"]                   # doctest: +ELLIPSIS 
        [0.0, 1.0, 2.0, ..., 53.0, 54.0, 55.0]
        >>> # 1.III) LEC expression
        >>> variables["ACAF"] = "t + 10"
        >>> variables["ACAF"]                   # doctest: +ELLIPSIS 
        [10.0, 11.0, 12.0, ..., 63.0, 64.0, 65.0]

        >>> # 2) set one value of a Variable for a specific period
        >>> variables["ACAG", "1990Y1"]
        -28.1721855713507
        >>> variables["ACAG", "1990Y1"] = -28.2
        >>> variables["ACAG", "1990Y1"]
        -28.2

        >>> # 3) set the variable values for range of periods 
        >>> # 3.I) using a Python slice
        >>> # 3.I.a) variable(periods) = same value for all periods
        >>> variables["ACAF", "1991Y1":"1995Y1"] = 0.0
        >>> variables["ACAF", "1991Y1":"1995Y1"]
        [0.0, 0.0, 0.0, 0.0, 0.0]
        >>> # 3.I.b) variable(periods) = vector (list) containing a specific value for each period
        >>> variables["ACAF", "1991Y1":"1995Y1"] = [0., 1., 2., 3., 4.]
        >>> variables["ACAF", "1991Y1":"1995Y1"]
        [0.0, 1.0, 2.0, 3.0, 4.0]
        >>> # 3.I.c) variable(periods) = LEC expression
        >>> variables["ACAF", "1991Y1":"1995Y1"] = "t + 10"
        >>> variables["ACAF", "1991Y1":"1995Y1"]
        [41.0, 42.0, 43.0, 44.0, 45.0]

        >>> # 3.II) same as above but with the colon ':' inside the periods range string
        >>> # 3.II.a) variable(periods) = same value for all periods
        >>> variables["ACAF", "1991Y1:1995Y1"] = 0.0
        >>> variables["ACAF", "1991Y1:1995Y1"]
        [0.0, 0.0, 0.0, 0.0, 0.0]
        >>> # 3.II.b) variable(periods) = vector (list) containing a specific value for each period
        >>> variables["ACAF", "1991Y1:1995Y1"] = [0., -1., -2., -3., -4.]
        >>> variables["ACAF", "1991Y1":"1995Y1"]
        [0.0, -1.0, -2.0, -3.0, -4.0]
        >>> # 3.II.c) variable(periods) = LEC expression
        >>> variables["ACAF", "1991Y1:1995Y1"] = "t - 10"
        >>> variables["ACAF", "1991Y1:1995Y1"]
        [21.0, 22.0, 23.0, 24.0, 25.0]

        >>> # c) -------- working on a subset --------
        >>> # 1) get subset
        >>> variables_subset = variables["A*"]
        >>> variables_subset.names
        ['A0', 'A1', 'A2', 'ACAF', 'ACAG', 'AOUC', 'AOUC_', 'AQC']
        >>> # 2) add a variable to the subset
        >>> variables_subset["A3"] = NA
        >>> variables_subset["A3"]              # doctest: +ELLIPSIS 
        [nan, nan, ..., nan, nan]
        >>> # or equivalently
        >>> variables_subset["A3"] = np.nan
        >>> variables_subset["A3"]              # doctest: +ELLIPSIS 
        [nan, nan, ..., nan, nan]
        >>> # --> new variable also appears in the global workspace
        >>> "A3" in variables
        True
        >>> variables["A3"]                     # doctest: +ELLIPSIS 
        [nan, nan, ..., nan, nan]
        >>> # 3) update a variable in the subset
        >>> variables_subset["A3"] = 0.0
        >>> variables_subset["A3"]              # doctest: +ELLIPSIS 
        [0.0, 0.0, ..., 0.0, 0.0]
        >>> # --> variable is also updated in the global workspace
        >>> variables["A3"]                     # doctest: +ELLIPSIS 
        [0.0, 0.0, ..., 0.0, 0.0]
        """
        names, periods_ = self._unfold_key(key)
        # update/add a single Variable
        if len(names) == 1:
            self._set_variable(names[0], value, periods_)
        # update/add several variables
        else:
            # if value is a string (LEC expression) or a numerical value -> set the same value for all Variables
            values = [value] * len(names) if isinstance(value, str) or not isinstance(value, Iterable) else value
            # check list of values has the same length as list of names
            if len(names) != len(values):
                raise ValueError(f"Cannot add/update Variables for the selection key '{key}'.\n"
                                f"{len(values)} values has been passed while the selection key '{key}' "
                                f"represents {len(names)} variables.")
            for name, value in zip(names, values):
                self._set_variable(name, value, periods_) 

    # overriden for Variables
    def __delitem__(self, key):
        """
        Remove the (subset of) variable(s) referenced by `key` from the Variables database.

        Parameters
        ----------
        key: str or list(str)
            (list of) name(s) of the variable(s) to be removed.
            The list of names can be given as a string pattern (e.g. "A*;*_").

        Examples
        --------
        >>> from iode import SAMPLE_DATA_DIR
        >>> from iode import variables
        >>> variables.load(f"{SAMPLE_DATA_DIR}/fun.var")        # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Loading .../fun.var
        394 objects loaded

        >>> # a) delete one variable
        >>> variables.get_names("A*")
        ['ACAF', 'ACAG', 'AOUC', 'AOUC_', 'AQC']
        >>> del variables["ACAF"]
        >>> variables.get_names("A*")
        ['ACAG', 'AOUC', 'AOUC_', 'AQC']

        >>> # b) delete several variables at once using a pattern
        >>> del variables["A*"]
        >>> variables.get_names("A*")
        []

        >>> # c) delete several variables at once using a list of names
        >>> variables.get_names("B*")
        ['BENEF', 'BQY', 'BRUGP', 'BVY']
        >>> del variables[["BENEF", "BQY"]]
        >>> variables.get_names("B*")
        ['BRUGP', 'BVY']

        >>> # delete one variable from a subset of the global database
        >>> variables_subset = variables["D*"]
        >>> variables_subset.names
        ['DEBT', 'DPU', 'DPUF', 'DPUG', 'DPUGO', 'DPUH', 'DPUHO', 'DPUU', 'DTF', 'DTFX', 'DTH', 'DTH1', 'DTH1C', 'DTHX']
        >>> del variables_subset["DPUGO"]
        >>> variables_subset.names
        ['DEBT', 'DPU', 'DPUF', 'DPUG', 'DPUH', 'DPUHO', 'DPUU', 'DTF', 'DTFX', 'DTH', 'DTH1', 'DTH1C', 'DTHX']
        >>> # NOTE: the variable has also been deleted from the global database
        >>> "DPUGO" in variables
        False
        >>> variables.get_names("D*")
        ['DEBT', 'DPU', 'DPUF', 'DPUG', 'DPUH', 'DPUHO', 'DPUU', 'DTF', 'DTFX', 'DTH', 'DTH1', 'DTH1C', 'DTHX']
        """
        names, periods_ = self._unfold_key(key)
        # names represents a single Variable
        if len(names) == 1:
            self.database_ptr.remove(names[0].encode())
        # names represents a selection of Variables
        elif periods_ is None:
            for name in names:
                self.database_ptr.remove(name.encode())
        else:
            raise RuntimeError(f"Cannot select period(s) when deleting (a) variable(s)")

    def from_frame(self, df: DataFrame):
        """
        Copy the pandas DataFrame `df` into the IODE Variables database.
        The variable names to copy are deduced from the index of the DataFrame.
        The column names must match the sub-periods of the current Variables sample.

        Parameters
        ----------
        df: DataFrame
            pandas DataFrame containing the variables to copy into the IODE Variables database.

        Notes
        -----
        The index of the passed DataFrame is sorted in alphabetical order before 
        copying to IODE Variables database.

        Warnings
        --------
        IODE and pandas don't use the same constant to represent NaN values.
        When loading a pandas DataFrame into the Variables database, the pandas 
        NaN values (:math:`nan`) are converted to IODE NaN values (:math:`NA`).

        See Also
        --------
        Variables.to_frame

        Examples
        --------
        >>> from iode import variables
        >>> import numpy as np
        >>> import pandas as pd
        >>> variables.clear()
        >>> len(variables)
        0

        >>> # create the pandas DataFrame
        >>> vars_names = [f"{region}_{code}" for region in ["VLA", "WAL", "BXL"] for code in ["00", "01", "02"]]
        >>> periods_list = [f"{i}Y1" for i in range(1960, 1971)]
        >>> data = np.arange(len(vars_names) * len(periods_list), dtype=float).reshape(len(vars_names), len(periods_list))
        >>> df = pd.DataFrame(index=vars_names, columns=periods_list, data=data)
        >>> # display the dataframe
        >>> df          # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
                1960Y1  1961Y1  1962Y1  1963Y1  ...  1967Y1  1968Y1  1969Y1  1970Y1
        VLA_00     0.0     1.0     2.0     3.0  ...     7.0     8.0     9.0    10.0
        VLA_01    11.0    12.0    13.0    14.0  ...    18.0    19.0    20.0    21.0
        VLA_02    22.0    23.0    24.0    25.0  ...    29.0    30.0    31.0    32.0
        WAL_00    33.0    34.0    35.0    36.0  ...    40.0    41.0    42.0    43.0
        WAL_01    44.0    45.0    46.0    47.0  ...    51.0    52.0    53.0    54.0
        WAL_02    55.0    56.0    57.0    58.0  ...    62.0    63.0    64.0    65.0
        BXL_00    66.0    67.0    68.0    69.0  ...    73.0    74.0    75.0    76.0
        BXL_01    77.0    78.0    79.0    80.0  ...    84.0    85.0    86.0    87.0
        BXL_02    88.0    89.0    90.0    91.0  ...    95.0    96.0    97.0    98.0
        <BLANKLINE>
        [9 rows x 11 columns]

        >>> # load into the IODE Variables database
        >>> variables.from_frame(df)
        >>> len(variables)
        9
        >>> variables.names
        ['BXL_00', 'BXL_01', 'BXL_02', 'VLA_00', 'VLA_01', 'VLA_02', 'WAL_00', 'WAL_01', 'WAL_02']
        >>> variables.sample
        Sample("1960Y1:1970Y1")
        >>> variables["VLA_00"]
        [0.0, 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0, 10.0]
        >>> variables["BXL_02"]
        [88.0, 89.0, 90.0, 91.0, 92.0, 93.0, 94.0, 95.0, 96.0, 97.0, 98.0]
        """
        if pd is None:
            raise RuntimeError("pandas library not found")

        # list of variable names
        vars_names = df.index.to_list()

        # list of periods
        periods_list = df.columns.to_list()

        # override the current sample
        self.sample = f"{periods_list[0]}:{periods_list[-1]}"

        # numpy data
        data = df.to_numpy(copy=False)

        _numpy_array_to_ws(data, vars_names, periods_list)

    def to_frame(self, vars_axis_name: str = 'names', time_axis_name: str = 'time', sample_as_floats: bool = False) -> DataFrame:
        """
        Create a pandas DataFrame from the current Variables database.
        The index of the returned DataFrame is build from the Variables names 
        and the columns from the periods.

        Parameters
        ----------
        vars_axis_name: str, optional
            Name of the axis representing the Variables names.
            Defaults to 'names'.
        time_axis_name: str, optional
            Name of the axis representing the periods.
            Defaults to 'time'.
        sample_as_floats: bool, optional
            Whether or not to export periods as string or float values.
            Defaults to False (periods are exported as string values).

        Warnings
        --------
        IODE and pandas don't use the same constant to represent NaN values.
        When exporting IODE variables as a pandas DataFrame, the IODE NaN values 
        (:math:`NA`) are converted to pandas NaN values (:math:`nan`).

        See Also
        --------
        Variables.from_frame

        Examples
        --------
        >>> from iode import SAMPLE_DATA_DIR
        >>> from iode import variables
        >>> import pandas as pd
        >>> variables.load(f"{SAMPLE_DATA_DIR}/fun.var")        # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Loading .../fun.var
        394 objects loaded
        >>> len(variables)
        394
        >>> variables.sample
        Sample("1960Y1:2015Y1")
        >>> variables.nb_periods
        56

        >>> # Export the IODE Variables database as a pandas DataFrame
        >>> df = variables.to_frame()
        >>> df.shape
        (394, 56)
        >>> df.index.to_list()              # doctest: +ELLIPSIS
        ['ACAF', 'ACAG', 'AOUC', ..., 'ZKFO', 'ZX', 'ZZF_']
        >>> df.columns.to_list()            # doctest: +ELLIPSIS
        ['1960Y1', '1961Y1', ..., '2014Y1', '2015Y1']
        >>> variables["AOUC"]               # doctest: +ELLIPSIS
        [nan, 0.24783191606766575, ..., 1.4237139558484628, 1.4608626117037322]
        >>> df.loc["AOUC"]                  # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        time
        1960Y1         NaN
        1961Y1    0.247832
        ...
        2014Y1    1.423714
        2015Y1    1.460863
        Name: AOUC, dtype: float64    
        >>> variables["ZKFO"]               # doctest: +ELLIPSIS
        [1.0, 1.0, ... 1.0159901, 1.0159901]
        >>> df.loc["ZKFO"]                  # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        time
        1960Y1    1.00000
        1961Y1    1.00000
        ...
        2014Y1    1.01599
        2015Y1    1.01599
        Name: ZKFO, dtype: float64

        >>> # Export a subset of the IODE Variables database as a pandas DataFrame
        >>> df = variables["A*;*_"].to_frame()
        >>> df.shape
        (33, 56)
        >>> df.index.to_list()              # doctest: +ELLIPSIS
        ['ACAF', 'ACAG', 'AOUC', ..., 'WNF_', 'YDH_', 'ZZF_']
        >>> df.columns.to_list()            # doctest: +ELLIPSIS
        ['1960Y1', '1961Y1', ..., '2014Y1', '2015Y1']
        >>> variables["AOUC"]               # doctest: +ELLIPSIS
        [nan, 0.24783191606766575, ..., 1.4237139558484628, 1.4608626117037322]
        >>> df.loc["AOUC"]                  # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        time
        1960Y1         NaN
        1961Y1    0.247832
        ...
        2014Y1    1.423714
        2015Y1    1.460863
        Name: AOUC, dtype: float64    
        >>> variables["ZZF_"]               # doctest: +ELLIPSIS
        [0.68840039, 0.68840039, ..., 0.68840039, 0.68840039]
        >>> df.loc["ZZF_"]                  # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        time
        1960Y1    0.6884
        1961Y1    0.6884
        ...
        2014Y1    0.6884
        2015Y1    0.6884
        Name: ZZF_, dtype: float64
        """
        cdef cnp.ndarray[cnp.double_t, ndim = 2] data
        cdef double* cpp_values_ptr

        if pd is None:
            raise RuntimeError("pandas library not found")
        
        vars_list = self.names
        periods_list = self.periods_as_float if sample_as_floats else self.periods
        data = _ws_to_numpy_array(vars_list, len(periods_list))

        df = DataFrame(index=vars_list, columns=periods_list, data=data)
        df.index.name = vars_axis_name
        df.columns.name = time_axis_name
        return df

    def from_array(self, array: Array, time_axis_name: str = 'time', sep: str = "_"):
        r"""
        Copies the Array array into the IODE Variables database.
        
        If the passed array has more than two dimensions, the non-time axes are grouped 
        (with 'sep' as separator) to form the Variables names.

        Parameters
        ----------
        array: Array
            Input Array object.
        time_axis_name: str, optional
            Name of the time axis in array.
            Assumed to be 'time' by default.
        sep: str, optional
            If the 'array' as more than two axes, the separator 
            'sep' is used to group labels of the non-time axes.

        Warnings
        --------
        IODE and LArray don't use the same constant to represent NaN values.
        When loading a LArray Array into the Variables database, the LArray 
        NaN values (:math:`nan`) are converted to IODE NaN values (:math:`NA`).

        Examples
        --------
        >>> from iode import variables
        >>> import larray as la
        >>> import numpy as np
        >>> variables.clear()
        >>> len(variables)
        0

        >>> regions_axis = la.Axis("region=VLA,WAL,BXL")
        >>> code_axis = la.Axis("code=00..02")
        >>> periods_axis = la.Axis("time=1960Y1..1970Y1")
        >>> array = la.ndtest((regions_axis, code_axis, periods_axis), dtype=float)
        >>> array
        region  code\time  1960Y1  1961Y1  1962Y1  ...  1968Y1  1969Y1  1970Y1
           VLA         00     0.0     1.0     2.0  ...     8.0     9.0    10.0
           VLA         01    11.0    12.0    13.0  ...    19.0    20.0    21.0
           VLA         02    22.0    23.0    24.0  ...    30.0    31.0    32.0
           WAL         00    33.0    34.0    35.0  ...    41.0    42.0    43.0
           WAL         01    44.0    45.0    46.0  ...    52.0    53.0    54.0
           WAL         02    55.0    56.0    57.0  ...    63.0    64.0    65.0
           BXL         00    66.0    67.0    68.0  ...    74.0    75.0    76.0
           BXL         01    77.0    78.0    79.0  ...    85.0    86.0    87.0
           BXL         02    88.0    89.0    90.0  ...    96.0    97.0    98.0
        
        >>> # load the IODE Variables from the Array object
        >>> variables.from_array(array)
        >>> len(variables)
        9
        >>> variables.names
        ['BXL_00', 'BXL_01', 'BXL_02', 'VLA_00', 'VLA_01', 'VLA_02', 'WAL_00', 'WAL_01', 'WAL_02']
        >>> variables.sample
        Sample("1960Y1:1970Y1")
        >>> variables["VLA_00"]
        [0.0, 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0, 10.0]
        >>> variables["BXL_02"]
        [88.0, 89.0, 90.0, 91.0, 92.0, 93.0, 94.0, 95.0, 96.0, 97.0, 98.0]
        """
        if la is None:
            raise RuntimeError("larray library not found")

        # retrieve the time_axis_name. 
        # Raise an error if no time_axis_name present in array.
        if time_axis_name not in array.axes:
            raise RuntimeError(f"Passed Array object must contain an axis named {time_axis_name}.\nGot axes {repr(array.axes)}.")
        time = array.axes[time_axis_name]

        # override the current sample
        self.sample = f"{time.labels[0]}:{time.labels[-1]}"
        
        # push the time axis as last axis and combine all other axes 
        array = array.transpose(..., time_axis_name)
        if array.ndim > 2:
            array = array.combine_axes(array.axes[:-1], sep=sep)
        
        vars_names = array.axes[0].labels
        periods_list = array.axes[1].labels
        _numpy_array_to_ws(array.data, vars_names, periods_list)

    def to_array(self, vars_axis_name: str = 'names', time_axis_name: str = 'time', sample_as_floats: bool = False) -> Array:
        """
        Creates an Array from the current IODE Variables database.
        
        Parameters
        ----------
        vars_axis_name: str, optional
            Name of the axis representing the Variables names.
            Defaults to 'names'.
        time_axis_name: str, optional
            Name of the axis representing the periods.
            Defaults to 'time'.
        sample_as_floats: bool, optional
            Whether or not to convert periods as float values.
            Defaults to False (periods are exported as string values).

        Warnings
        --------
        IODE and LArray don't use the same constant to represent NaN values.
        When exporting IODE variables as a LArray Array, the IODE NaN values 
        (:math:`NA`) are converted to LArray NaN values (:math:`nan`).

        Examples
        --------
        >>> from iode import SAMPLE_DATA_DIR
        >>> from iode import variables
        >>> variables.load(f"{SAMPLE_DATA_DIR}/fun.var")        # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Loading .../fun.var
        394 objects loaded
        >>> len(variables)
        394
        >>> variables.sample
        Sample("1960Y1:2015Y1")
        >>> variables.nb_periods
        56

        >>> # Export the IODE Variables database as an (larray) Array object
        >>> array = variables.to_array()
        >>> array.shape
        (394, 56)
        >>> array.axes.info
        394 x 56
         names [394]: 'ACAF' 'ACAG' 'AOUC' ... 'ZKFO' 'ZX' 'ZZF_'
         time [56]: '1960Y1' '1961Y1' '1962Y1' ... '2013Y1' '2014Y1' '2015Y1'
        >>> variables["AOUC"]               # doctest: +ELLIPSIS
        [nan, 0.24783191606766575, ..., 1.4237139558484628, 1.4608626117037322]
        >>> array["AOUC"]
        time  1960Y1  ...              2014Y1              2015Y1
                 nan  ...  1.4237139558484628  1.4608626117037322
        >>> variables["ZKFO"]               # doctest: +ELLIPSIS
        [1.0, 1.0, ..., 1.0159901, 1.0159901]
        >>> array["ZKFO"]
        time  1960Y1  1961Y1  1962Y1  ...     2012Y1     2013Y1     2014Y1     2015Y1
                 1.0     1.0     1.0  ...  1.0159901  1.0159901  1.0159901  1.0159901

        >>> # Export a subset of the IODE Variables database as an (larray) Array object
        >>> array = variables["A*;*_"].to_array()
        >>> array.shape
        (33, 56)
        >>> array.axes.info
        33 x 56
         names [33]: 'ACAF' 'ACAG' 'AOUC' ... 'WNF_' 'YDH_' 'ZZF_'
         time [56]: '1960Y1' '1961Y1' '1962Y1' ... '2013Y1' '2014Y1' '2015Y1'
        >>> variables["AOUC"]               # doctest: +ELLIPSIS
        [nan, 0.24783191606766575, ..., 1.4237139558484628, 1.4608626117037322]
        >>> array["AOUC"]
        time  1960Y1  ...              2014Y1              2015Y1
                 nan  ...  1.4237139558484628  1.4608626117037322
        >>> variables["ZZF_"]               # doctest: +ELLIPSIS
        [0.68840039, 0.68840039, ..., 0.68840039, 0.68840039]
        >>> array["ZZF_"]
        time      1960Y1      1961Y1  ...      2013Y1      2014Y1      2015Y1
              0.68840039  0.68840039  ...  0.68840039  0.68840039  0.68840039
        """
        if la is None:
            raise RuntimeError("larray library not found")

        vars_list = self.names
        periods_list = self.periods_as_float if sample_as_floats else self.periods
        data = _ws_to_numpy_array(vars_list, len(periods_list))
        
        vars_axis = la.Axis(name=vars_axis_name, labels=vars_list)
        time_axis = la.Axis(name=time_axis_name, labels=periods_list)
        return la.Array(axes=(vars_axis, time_axis), data=data)

    @property
    def mode(self) -> str:
        """
        Current display mode for the IODE Variables values.
        The default mode is 'LEVEL'.

        Parameters
        ----------
        value: VarsMode or str
            New mode value.
            Possible modes are LEVEL, DIFF, GROWTH_RATE, Y0Y_DIFF or Y0Y_GROWTH_RATE.

        Examples
        --------
        >>> from iode import SAMPLE_DATA_DIR, variables, VarsMode
        >>> variables.load(f"{SAMPLE_DATA_DIR}/fun.var")        # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Loading .../fun.var
        394 objects loaded
        >>> variables.mode
        'LEVEL'
        >>> variables["ACAF", "1990Y1"]
        23.771
        
        >>> variables.mode = VarsMode.GROWTH_RATE
        >>> variables.mode
        'GROWTH_RATE'
        >>> variables["ACAF", "1990Y1"]
        38.48528176607737
        >>> variables.mode = "level"
        >>> variables.mode
        'LEVEL'
        >>> variables["ACAF", "1990Y1"]
        23.771
        """
        return VarsMode(<int>self.mode_).name

    @mode.setter
    def mode(self, value: Union[VarsMode, str]):
        if isinstance(value, str):
            value = value.upper()
            value = VarsMode[value]
        value = int(value)
        self.mode_ = <IodeVarMode>value

    @property
    def sample(self) -> Sample:
        """
        Current sample of the IODE Variables database.

        Parameters
        ----------
        value: str or tuple(str, str)
            New sample as string 'start_period:last_period' or as a tuple 'start_period', 'last_period'.
        
        Examples
        --------
        >>> from iode import SAMPLE_DATA_DIR
        >>> from iode import variables
        >>> variables.load(f"{SAMPLE_DATA_DIR}/fun.var")        # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Loading .../fun.var
        394 objects loaded
        >>> variables.sample
        Sample("1960Y1:2015Y1")

        >>> # update sample by passing a string
        >>> variables.sample = '1970Y1:2020Y1'
        >>> variables.sample
        Sample("1970Y1:2020Y1")
        >>> # start or end period are optional
        >>> variables.sample = ':2010Y1'
        >>> variables.sample
        Sample("1970Y1:2010Y1")
        >>> variables.sample = '1980Y1:'
        >>> variables.sample
        Sample("1980Y1:2010Y1")

        >>> # update sample by passing a start period and 
        >>> # an end period separated by a comma
        >>> variables.sample = '1980Y1', '2010Y1'
        >>> variables.sample
        Sample("1980Y1:2010Y1")
        """
        cdef CSample* c_sample = self.database_ptr.get_sample()
        return Sample._from_ptr(c_sample, <bint>False)

    @sample.setter    
    def sample(self, value: Union[str, Tuple[Union[str, Period], Union[str, Period]]]):
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
        
        self.database_ptr.set_sample(from_period.encode(), to_period.encode())

    @property
    def nb_periods(self) -> int:
        """
        Return the number of periods from the current Variables sample.

        Returns
        -------
        int

        Examples
        --------
        >>> from iode import SAMPLE_DATA_DIR
        >>> from iode import variables
        >>> variables.load(f"{SAMPLE_DATA_DIR}/fun.var")        # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Loading .../fun.var
        394 objects loaded
        >>> variables.sample
        Sample("1960Y1:2015Y1")
        >>> variables.nb_periods
        56
        """
        return self.database_ptr.get_nb_periods()
        
    @property
    def periods(self) -> List[str]:
        """
        Return the list of periods from the current Variables sample.

        Returns
        -------
        list(str)

        Examples
        --------
        >>> from iode import SAMPLE_DATA_DIR
        >>> from iode import variables
        >>> variables.load(f"{SAMPLE_DATA_DIR}/fun.var")        # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Loading .../fun.var
        394 objects loaded
        >>> variables.sample
        Sample("1960Y1:2015Y1")
        >>> variables.periods       # doctest: +ELLIPSIS 
        ['1960Y1', '1961Y1', ..., '2014Y1', '2015Y1']
        """
        return [cpp_period.decode() for cpp_period in self.database_ptr.get_list_periods(bytes(), bytes())]

    @property
    def periods_as_float(self) -> List[float]:
        """
        Return the list of periods as float from the current Variables sample.

        Returns
        -------
        list(str)

        Examples
        --------
        >>> from iode import SAMPLE_DATA_DIR
        >>> from iode import variables
        >>> variables.load(f"{SAMPLE_DATA_DIR}/fun.var")        # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Loading .../fun.var
        394 objects loaded
        >>> variables.sample
        Sample("1960Y1:2015Y1")
        >>> variables.periods_as_float          # doctest: +ELLIPSIS 
        [1960.0, 1961.0, ..., 2014.0, 2015.0]
        """
        return self.database_ptr.get_list_periods_as_float(bytes(), bytes())

    @property
    def threshold(self) -> float:
        r"""
        Threshold under which the difference between 2 variables are considered equal.
        
        The comparison test is:
        .. math::

           \begin{cases} 
           \frac{x_1 - x_2}{x_1} < \epsilon, & \text{if } x_1 \neq 0 \\
           |x_2| < \epsilon, & \text{otherwise}
           \end{cases}

        where :math:`x_1` and :math:`x_2` are the values of the variables to compare 
        and :math:`\epsilon` is the threshold value.

        Parameters
        ----------
        value: float
        	New threshold value.
            Default value is 1e-7.

        See Also
        --------
        iode.Variables.compare

        Examples
        --------
        >>> from iode import SAMPLE_DATA_DIR
        >>> from iode import variables
        >>> variables.load(f"{SAMPLE_DATA_DIR}/fun.var")        # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Loading .../fun.var
        394 objects loaded
        >>> variables.threshold
        1e-07
        >>> variables.threshold = 1e-5
        >>> variables.threshold
        1e-05
        """
        return K_CMP_EPS

    @threshold.setter
    def threshold(self, value: float):
        res = B_DataCompareEps(str(value).encode())
        if res != 0:
            raise ValueError(f"threshold: Invalid value '{value}'.")

    @property
    def df(self) -> DataFrame:
        r"""
        Create a pandas DataFrame from the current Variables database.
        The index of the returned DataFrame is build from the Variables names 
        and the columns from the periods.

        Warnings
        --------
        IODE and pandas don't use the same constant to represent NaN values.
        When exporting IODE variables as a pandas DataFrame, the IODE NaN values 
        (:math:`NA`) are converted to pandas NaN values (:math:`nan`).

        See Also
        --------
        Variables.to_frame

        Examples
        --------
        >>> from iode import SAMPLE_DATA_DIR
        >>> from iode import variables
        >>> import pandas as pd
        >>> variables.load(f"{SAMPLE_DATA_DIR}/fun.var")        # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Loading .../fun.var
        394 objects loaded
        >>> len(variables)
        394
        >>> variables.sample
        Sample("1960Y1:2015Y1")
        >>> variables.nb_periods
        56

        >>> # Export the IODE Variables database as a pandas DataFrame
        >>> df = variables.df
        >>> df.shape
        (394, 56)
        >>> df.index.to_list()              # doctest: +ELLIPSIS
        ['ACAF', 'ACAG', 'AOUC', ..., 'ZKFO', 'ZX', 'ZZF_']
        >>> df.columns.to_list()            # doctest: +ELLIPSIS
        ['1960Y1', '1961Y1', ..., '2014Y1', '2015Y1']
        >>> variables["AOUC"]               # doctest: +ELLIPSIS
        [nan, 0.24783191606766575, ..., 1.4237139558484628, 1.4608626117037322]
        >>> df.loc["AOUC"]                  # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        time
        1960Y1         NaN
        1961Y1    0.247832
        ...
        2014Y1    1.423714
        2015Y1    1.460863
        Name: AOUC, dtype: float64    
        >>> variables["ZKFO"]               # doctest: +ELLIPSIS
        [1.0, 1.0, ... 1.0159901, 1.0159901]
        >>> df.loc["ZKFO"]                  # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        time
        1960Y1    1.00000
        1961Y1    1.00000
        ...
        2014Y1    1.01599
        2015Y1    1.01599
        Name: ZKFO, dtype: float64

        >>> # Export a subset of the IODE Variables database as a pandas DataFrame
        >>> df = variables["A*;*_"].df
        >>> df.shape
        (33, 56)
        >>> df.index.to_list()              # doctest: +ELLIPSIS
        ['ACAF', 'ACAG', 'AOUC', ..., 'WNF_', 'YDH_', 'ZZF_']
        >>> df.columns.to_list()            # doctest: +ELLIPSIS
        ['1960Y1', '1961Y1', ..., '2014Y1', '2015Y1']
        >>> variables["AOUC"]               # doctest: +ELLIPSIS
        [nan, 0.24783191606766575, ..., 1.4237139558484628, 1.4608626117037322]
        >>> df.loc["AOUC"]                  # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        time
        1960Y1         NaN
        1961Y1    0.247832
        ...
        2014Y1    1.423714
        2015Y1    1.460863
        Name: AOUC, dtype: float64    
        >>> variables["ZZF_"]               # doctest: +ELLIPSIS
        [0.68840039, 0.68840039, ..., 0.68840039, 0.68840039]
        >>> df.loc["ZZF_"]                  # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        time
        1960Y1    0.6884
        1961Y1    0.6884
        ...
        2014Y1    0.6884
        2015Y1    0.6884
        Name: ZZF_, dtype: float64 
        """
        return self.to_frame()

    def periods_subset(self, from_period: Union[str, Period] = None, to_period: Union[str, Period] = None, 
                       as_float: bool = False):
        """
        Return a subset of the periods from the current Variables sample.

        Parameters
        ----------
        from_period: str or Period, optional
            first period of the subset of periods.
            Defaults to the first period of the current Variables sample.
        to_period: str or Period, optional
            last period of the subset of periods.
            Defaults to the last period of the current Variables sample.
        as_float: bool, optional
            whether or not to return periods as float.
            Defaults to False.

        Returns
        -------
        list(str) or list(float)

        Examples
        --------
        >>> from iode import SAMPLE_DATA_DIR
        >>> from iode import variables
        >>> variables.load(f"{SAMPLE_DATA_DIR}/fun.var")        # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Loading .../fun.var
        394 objects loaded
        >>> variables.sample
        Sample("1960Y1:2015Y1")
        >>> variables.periods_subset("1990Y1", "2000Y1")                    # doctest: +ELLIPSIS
        ['1990Y1', '1991Y1', ..., '1999Y1', '2000Y1']
        >>> variables.periods_subset("1990Y1", "2000Y1", as_float=True)     # doctest: +ELLIPSIS
        [1990.0, 1991.0, ..., 1999.0, 2000.0]
        """
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

        if as_float:
            return self.database_ptr.get_list_periods_as_float(from_period.encode(), to_period.encode())
        else:
            return [cpp_period.decode() for cpp_period in 
                    self.database_ptr.get_list_periods(from_period.encode(), to_period.encode())]
        
    def copy_from(self, input_files: Union[str, List[str]], from_period: Union[str, Period]=None, 
                  to_period: Union[str, Period]=None, names: Union[str, List[str]]='*'):
        """
        Copy (a subset of) variables from the input file(s) 'input_files' into the current database.

        Parameters
        ----------
        input_file: str or list(str)
            file(s) from which the copied variables are read.
        from_period: str or Period, optional
            start period for copying the variables values.
        to_period: str or Period, optional
            end period for copying the variables values.
        names: str or list(str), optional
            list of variables to copy from the input file(s).
            Defaults to load all variables from the input file(s). 
        """
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
        """
        Build series with higher periodicity for *stock data* (Unemployment, Debt, ...) or 
        *flow data* (GNP, Deficit, ...).

        The list of specified series (variables) from the input file are loaded into the current 
        Variables database and the periodicity of these series (variables) is modified simultaneously. 
        The new periodicity is the one currently defined in the current Variables database.

        The loaded series are added to or replace those (for existing names) in the current 
        Variables database.

        This procedure exists for the following cases:

            - annual to monthly 
            - annual to quarterly 
            - quarterly to monthly 
        
        Two types of series are available, one for stocks (STOCK), the other for flows (FLOW).

        Three interpolation methods are available:

            - linear (LINEAR): A[1980Q{1,2,3,4}] = A[1979Y1] + i * (A[1980Y1] - A[1979Y1])/4 i = 1,2,3,4 
            - cubic splines (CUBIC_SPLINES): cubic interpolation 
            - step (STEP) : A[1980Q{1,2,3,4}] = A[1980Y1] 

        Parameters
        ----------
        type_of_series : LowToHighType or str
            Two types of series are considered: 'stock' and 'flow':
                - STOCK: stock data (Unemployment, Debt, ...)
                - FLOW: flow data (GNP, Deficit, ...)

        method : LowToHighMethod or str
            Method to use for transformation. Three methods can be used:
                - LINEAR ('L'): Linear interpolation
                - CUBIC_SPLINES ('C'): Cubic Spliness
                - STEP ('S'): Step

        filepath : str or Path
            Filepath to the source data file.

        var_list : str or list(str)
            List of variables to include in the transformation.

        Returns
        -------
        None

        Examples
        --------
        >>> from iode import SAMPLE_DATA_DIR, variables, LowToHighType, LowToHighMethod
        >>> variables.clear()
        >>> # define a yearly sample
        >>> variables.sample = "2010Q1:2020Q4"
        >>> # input filepath
        >>> filepath = f"{SAMPLE_DATA_DIR}/fun.var"
        
        Linear interpolation / stock
        
        >>> # "stock" -> the result is a linear interpolation of the 2 surrounding source values.
        >>> variables.low_to_high(LowToHighType.STOCK, LowToHighMethod.LINEAR, filepath, ["ACAF", "ACAG"])
        >>> variables["ACAF", "2014Q1":"2014Q4"]
        [-72.50614701966526, -76.11763971671049, -79.7291324137557, -83.34062511080091]
        >>> variables["ACAG", "2014Q1":"2014Q4"]
        [31.63267881532112, 31.895218819946884, 32.15775882457264, 32.4202988291984]

        Linear interpolation / flow
        
        >>> # "flow" -> the result is the source value divided by the nb of sub-periods. 
        >>> variables.low_to_high(LowToHighType.FLOW, LowToHighMethod.LINEAR, filepath, ["ACAF", "ACAG"])
        >>> variables["ACAF", "2014Q1":"2014Q4"]
        [-20.83515627770023, -20.83515627770023, -20.83515627770023, -20.83515627770023]
        >>> variables["ACAG", "2014Q1":"2014Q4"]
        [8.1050747072996, 8.1050747072996, 8.1050747072996, 8.1050747072996]

        Cubic splines / stock
        
        >>> variables.low_to_high(LowToHighType.STOCK, LowToHighMethod.CUBIC_SPLINES, filepath, ["ACAF", "ACAG"])
        >>> variables["ACAF", "2012Q1":"2012Q4"]
        [-47.2984169294621, -50.052041225380975, -52.80566552129986, -55.55928981721873]
        >>> variables["ACAG", "2012Q1":"2012Q4"]
        [29.544440560604077, 29.804280757173238, 30.064120953742403, 30.323961150311572]

        Cubic splines / flow
        
        >>> variables.low_to_high(LowToHighType.FLOW, LowToHighMethod.CUBIC_SPLINES, filepath, ["ACAF", "ACAG"])
        >>> variables["ACAF", "2012Q1":"2012Q4"]
        [-12.748422687629207, -13.436828761608925, -14.270289043196508, -15.103749324784092]
        >>> variables["ACAG", "2012Q1":"2012Q4"]
        [7.483230672890683, 7.548190722032976, 7.6135768258069625, 7.678962929580949]

        Step / stock
        
        >>> # "stock" -> the result has the same value as the source
        >>> variables.low_to_high(LowToHighType.STOCK, LowToHighMethod.STEP, filepath, ["ACAF", "ACAG"])
        >>> variables["ACAF", "2014Q1":"2014Q4"]
        [-83.34062511080091, -83.34062511080091, -83.34062511080091, -83.34062511080091]
        >>> variables["ACAG", "2014Q1":"2014Q4"]
        [32.4202988291984, 32.4202988291984, 32.4202988291984, 32.4202988291984]

        Step / flow
        
        >>> # "flow" -> the result is the source value plus a portion of 
        >>> # the difference between the 2 surrounding values in the source
        >>> variables.low_to_high(LowToHighType.FLOW, LowToHighMethod.STEP, filepath, ["ACAF", "ACAG"])
        >>> variables["ACAF", "2014Q1":"2014Q4"]
        [-20.83515627770023, -20.83515627770023, -20.83515627770023, -20.83515627770023]
        >>> variables["ACAG", "2014Q1":"2014Q4"]
        [8.1050747072996, 8.1050747072996, 8.1050747072996, 8.1050747072996]
        """
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
        """
        Build series of lower periodicity by (summing the / taking the average of the / taking the 
        last observation of) sub-periods.

        The list of specified series (variables) from the input file are loaded into the current 
        Variables database and the periodicity of these series (variables) is modified simultaneously. 
        The new periodicity is the one currently defined in the current Variables database.

        The loaded series are added to or replace those (for existing names) in the current 
        Variables database.

        This procedure exists for the following cases:

            - monthly to annual 
              (annual observation = sum of 12 months / average of 12 months / December value) 
            - quarterly to annual 
              (annual observation = sum of 4 quarters / average of 4 quarters / last quarter value) 
            - monthly to quarterly 
              (quarterly observation = sum of 3 months / average of 3 months / value for the last month of the quarter) 

        Three types of series are available:

            - SIM: addition of sub-periods data
            - MEAN: average of sub-periods data
            - LAST: last observation

        In the case of a non-existent value (:math:`NA`) for one of the sub-periods, the result is :math:`NA`.

        Parameters
        ----------
        type_of_series : HighToLowType or str
            Three types of series are available: 
                - SUM : addition of sub-period data
                - MEAN : average of sub-period data
                - LAST : last observation

        filepath : str or Path
            Filepath to the source data file.

        var_list : str or list(str)
            List of variables to include in the transformation.

        Examples
        --------
        >>> from iode import SAMPLE_DATA_DIR, variables, HighToLowType
        >>> variables.clear()
        >>> # define a yearly sample
        >>> variables.sample = "2000Y1:2020Y1"
        >>> # input filepath
        >>> filepath = f"{SAMPLE_DATA_DIR}/fun_q.var"
        
        Last Obs in year
        
        >>> variables.high_to_low(HighToLowType.LAST, filepath, ["ACAF", "ACAG"])
        >>> variables["ACAF", "2010Y1":"2014Y1"]
        [-37.82742883229439, -44.544792633543224, -55.55928981721873, -68.89465432262006, -83.34062511080091]
        >>> variables["ACAG", "2010Y1":"2014Y1"]
        [7.063482244552621, 7.321150091008727, 7.580990287577893, 7.842534702673841, 8.1050747072996]

        Mean of year
        
        >>> variables.high_to_low(HighToLowType.MEAN, filepath, ["ACAF", "ACAG"])
        >>> variables["ACAF", "2010Y1":"2014Y1"]
        [-37.82742883229439, -44.544792633543224, -55.55928981721873, -68.89465432262006, -83.34062511080091]
        >>> variables["ACAG", "2010Y1":"2014Y1"]
        [7.063482244552621, 7.321150091008727, 7.580990287577893, 7.842534702673841, 8.1050747072996]

        Sum
        
        >>> variables.high_to_low(HighToLowType.SUM, filepath, ["ACAF", "ACAG"])
        >>> variables["ACAF", "2010Y1":"2014Y1"]
        [-151.30971532917755, -178.1791705341729, -222.23715926887493, -275.5786172904802, -333.36250044320366]
        >>> variables["ACAG", "2010Y1":"2014Y1"]
        [28.253928978210485, 29.284600364034908, 30.323961150311572, 31.370138810695362, 32.4202988291984]
        """
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
        """
        Extrapolate variables using one the method described below, based on previous periods.

        The possible methods are as follows:

          - :math:`Y := Y[-1], if Y null or NA` (TM1) : each null or NA endogen at the start takes the value of 
            the previous period,
          - :math:`Y := Y[-1], always` (TM1_A) : each endogen takes the value of the previous period at the start,
          - :math:`Y := extrapolation, if Y null or NA` (EXTRA) : each null or NA endogen takes as value a linear 
            extrapolation of the two previous periods,
          - :math:`Y := extrapolation, always` (EXTRA_A) : each endogen takes as its value a linear extrapolation of 
            the two preceding periods, whether or not it is zero at the start,
          - :math:`Y unchanged` (ASIS): endogenous values are not initialized. They retain their value whether or 
            not they are zero,
          - :math:`Y := Y[-1], if Y = NA` (TM1_NA): each NA value takes the value of the previous period,
          - :math:`Y := extrapolation, if Y = NA` (extra_na): each NA value takes the value of a linear extrapolation of 
            the two previous periods.

        The sample over which the calculation is to be performed must be supplied. 
        This sample must be a sub-sample of the global sample defined in the current Variables database.

        In addition, the list of variables to be extrapolated can be specified. 
        If this list is not given, all variables will be extrapolated.

        Parameters
        ----------
        method: SimulationInitialization or str
            initialization method. Possible values are:
              - TM1: :math:`Y := Y[-1], if Y null or NA`
              - TM1_A: :math:`Y := Y[-1], always`
              - EXTRA: :math:`Y := extrapolation, if Y null or NA`
              - EXTRA_A: :math:`Y := extrapolation, always`
              - ASIS: :math:`Y unchanged`
              - TM1_NA: :math:`Y := Y[-1], if Y = NA`
              - EXTRA_NA: :math:`Y := extrapolation, if Y = NA`
        from_period: str or Period, optional
            starting period to extrapolate variables.
            Defaults to the starting period if the current sample.
        to_period: str or Period, optional
            ending period to extrapolate variables.
            Defaults to the ending period if the current sample.
        variables_list: str or list(str), optional
            list of variables to extrapolate.
            Defaults to None (all variables).

        Examples
        --------
        >>> from iode import variables, NA, SimulationInitialization
        >>> variables.clear()
        >>> variables.sample = "2000Y1:2020Y1"

        >>> def reset_ACAF():
        ...     variables["ACAF"] = "t"
        ...     variables["ACAF", ["2005Y1", "2007Y1"]] = NA
        >>> # create ACAF
        >>> reset_ACAF()
        >>> variables["ACAF", :"2010Y1"]
        [0.0, 1.0, 2.0, 3.0, 4.0, nan, 6.0, nan, 8.0, 9.0, 10.0]

        >>> # "TM1" (Y := Y[-1], if Y null or NA)
        >>> reset_ACAF()
        >>> variables.extrapolate(SimulationInitialization.TM1, "2005Y1", "2010Y1")
        >>> variables["ACAF", "2003Y1":"2009Y1"]
        [3.0, 4.0, 4.0, 6.0, 6.0, 8.0, 9.0]

        >>> # "TM1_A" (Y := Y[-1], always)
        >>> reset_ACAF()
        >>> variables.extrapolate(SimulationInitialization.TM1_A, "2005Y1", "2010Y1")
        >>> variables["ACAF", "2003Y1":"2009Y1"]
        [3.0, 4.0, 4.0, 4.0, 4.0, 4.0, 4.0]

        >>> # "EXTRA" (Y := extrapolation, if Y null or NA)
        >>> reset_ACAF()
        >>> variables.extrapolate(SimulationInitialization.EXTRA, "2005Y1", "2010Y1")
        >>> variables["ACAF", "2003Y1":"2009Y1"]
        [3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0]

        >>> # "EXTRA_A" (Y := extrapolation, always)
        >>> reset_ACAF()
        >>> variables.extrapolate(SimulationInitialization.EXTRA_A, "2005Y1", "2010Y1")
        >>> variables["ACAF", "2003Y1":"2009Y1"]
        [3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0]

        >>> # "ASIS" (Y unchanged)
        >>> reset_ACAF()
        >>> variables.extrapolate(SimulationInitialization.ASIS, "2005Y1", "2010Y1")
        >>> variables["ACAF", "2003Y1":"2009Y1"]
        [3.0, 4.0, nan, 6.0, nan, 8.0, 9.0]

        >>> # "TM1_NA" (Y := Y[-1], if Y = NA)
        >>> reset_ACAF()
        >>> variables.extrapolate(SimulationInitialization.TM1_NA, "2005Y1", "2010Y1")
        >>> variables["ACAF", "2003Y1":"2009Y1"]
        [3.0, 4.0, 4.0, 6.0, 6.0, 8.0, 9.0]

        >>> # "EXTRA_NA" (Y := extrapolation, if Y = NA)
        >>> reset_ACAF()
        >>> variables.extrapolate(SimulationInitialization.EXTRA_NA, "2005Y1", "2010Y1")
        >>> variables["ACAF", "2003Y1":"2009Y1"]
        [3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0]
        """
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
        """
        Eliminate seasonal variations in monthly series (= variables).

        The method constructs deseasonalized series using the *Census XI method*, as well as cyclical 
        and stochastic trend components. Thus, In addition to the deseasonalized series (named after 
        the series in the input file), the result is two other series:

            - the series containing the cyclical trend component named : `_C<name>` where *name* is the original name
            - the series containing the stochastic component called : `_I<name>` where *name* is the original name

        Note that the deseasonalized series is the product of the other two.

        To do this, the list of *series* (= variables) are loaded from the specified file *input_file* 
        into the current IODE Variables database and simultaneously modifies the selected *series* if necessary.

        The new *series* are added to or replace (for existing names) those in the current IODE Variables database.

        In the case of non-existent value (NA) for one of the periods, the result is NA for the whole year.

        Parameters
        ----------
        input_file: str or Path
            Filepath to the input file.
        eps_test: float, optional
            Criterion verifying whether a seasonal influence is present in a series
            Default to 5.0.
        series: str or list(str), optional
            list of series on which to apply the seasonal adjustment.
            Defaults to None (all variables).

        See Also
        --------
        Variables.trend_correction
        """
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
        """
        Implementation of the *Hodrick-Prescott method* for trend series (= variables) construction. 
        The principle is the same as for deseasonalization: series read from a file are imported and transformed simultaneously.

        If the `log` flag is set, the series are log-transformed before calculation and exp-transformed after calculation. 
        The series values must therefore be strictly positive.

        Parameters
        ----------
        input_file: str
            filepath to the input file.
        lambda_: float
            Lambda parameter of the Hodrick-Prescott method.
        series: str or list(str), optional
            list of series on which to apply the trend correction.
            Defaults to None (all variables).
        log: bool, optional
            Whether or not the series are log-transformed before calculation and exp-transformed 
            after calculation. The series values must therefore be strictly positive.
            Defaults to False.

        See Also
        --------
        Variables.seasonal_adjustment
        """
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
        r"""
        Convert an external file representing IODE variables to an IODE variables file (.var). 
        The possible formats for the input file are:
          
          - `Ascii`: IODE-specific Ascii format for objects 
          - `Rotated Ascii`: Ascii format for variables with series in columns 
          - `DIF`: DIF format (Data Interchange Format) 
          - `DIF` Belgostat: (old) exchange format specific to Belgostat 
          - `NIS`: National Institute of Statistics Ascii format (old) 
          - `GEM`: Ascii format of Chronos software 
          - `PRN-Aremos`: Ascii format from Aremos software 
          - `TXT Belgostat`: (old) Belgostat-specific exchange format 

        The rule file is a simple text file contains the rules for: 
        
          - selecting the objects to be imported 
          - determining the objects names.

        Each rule consists of two fields:

          - the selection pattern, containing a description of the names concerned by the rule. 
            This mask is defined in the same way as the :py:meth:`~iode.Comments.search` method.
          - the transcoding algorithm for the names, which can contain : 
            - `+` : indicates that the character must be included in the name 
            - `-` : indicates that the character should be skipped 
            - any other character: included in the name 
        
        Example:

            B* C+-+          -> transforms B1234 into CB2, BCDEF into CBE, etc 
            *X ++++++++++    -> keeps names ending in X unchanged
            * ++++++++++     -> keeps all names unchanged

        Parameters
        ----------
        input_file : str or Path
            The path to the input file to be converted. 
        input_format : str or ImportFormats
            The format of the input file. Possible formats are ASCII, ROT_ASCII (Rotated Ascii), 
            DIF, BISTEL, NIS, GEM, PRN, TXT (TXT Belgostat).
        save_file : str or Path
            The path to the output file where the IODE variables will be saved.
        rule_file : str or Path
            The path to the rule file that defines the selection and transcoding rules. 
        from_period : str or Period
            The first period of the series to be imported. 
        to_period : str or Period
            The last period of the series to be imported.
        debug_file : str or Path, optional
            The path to the debug file where the debug information will be saved. 
            If not provided, the debug information will be printed to the console.

        Examples
        --------
        >>> from pathlib import Path
        >>> from iode import SAMPLE_DATA_DIR, variables, ImportFormats
        >>> input_file = f"{SAMPLE_DATA_DIR}/fun_xode.av.ref"
        >>> input_format = ImportFormats.ASCII
        >>> save_file = "imported_var.var"
        >>> rule_file = f"{SAMPLE_DATA_DIR}/rules.txt"
        >>> debug_file = "debug.log"

        >>> # print rules
        >>> with open(rule_file, "r") as f:         # doctest: +NORMALIZE_WHITESPACE
        ...     print(f.read())
        ...
        AC*  KK_--+++++++++++++
        *U   UU_++++++++++++++++
        >>> # get list of variables with a name starting with 'AC' 
        >>> # and ending with 'U' from the input file
        >>> with open(input_file, "r") as f:         # doctest: +NORMALIZE_WHITESPACE, +ELLIPSIS
        ...     for line in f:
        ...         name = line.split(" ")[0]
        ...         if name.startswith("AC") or name.endswith("U"):
        ...             print(line.strip())
        ...
        ACAF na na ... -83.3406251108009 -96.4104198284833
        ACAG na na ... 32.4202988291984 33.469601344881
        CGU 69.354416 70.728317 ... 2652.457356636 2800.12343205764 
        DPU 56.285999 58.596001 ... 1794.28676968594 1879.1395597413
        DPUU 56.285999 58.596001 ... 1795.98222768555 1880.91519686508
        IFU na na ... 1895.9196231884 1952.4775760035
        IHU na na ... 855.342842036469 904.6210534989
        MU na na ... 0.278260325684654 0.278260325684654
        NAWRU na na ... 0.139645850151953 0.139645850151953
        WBU 256.177 268.75299 ... 8525.33576585068 8986.56510007165
        >>> # import variables from input_file to save_file
        >>> # using the rules defined in rule_file
        >>> variables.convert_file(input_file, input_format, save_file, rule_file, "2000Y1", "2010Y1", debug_file)
        Reading object 1 : KK_AF
        Reading object 2 : KK_AG
        Reading object 3 : UU_CGU
        Reading object 4 : UU_DPU
        Reading object 5 : UU_DPUU
        Reading object 6 : UU_IFU
        Reading object 7 : UU_IHU
        Reading object 8 : UU_MU
        Reading object 9 : UU_NAWRU
        Reading object 10 : UU_WBU
        10 objects saved
        >>> # check content of the saved file
        >>> variables.load(save_file)        # doctest: +ELLIPSIS
        Loading ...\imported_var.var
        10 objects loaded
        >>> variables                        # doctest: +NORMALIZE_WHITESPACE, +ELLIPSIS
        Workspace: Variables
        nb variables: 10
        filename: ...\imported_var.var
        sample: 1960Y1:2015Y1
        mode: LEVEL
        <BLANKLINE>
          name      1960Y1  1961Y1  1962Y1  1963Y1  1964Y1  ...      2010Y1  2011Y1  2012Y1  2013Y1  2014Y1  2015Y1 
        KK_AF           na      na      na      na      na  ...      -37.83  -44.54  -55.56  -68.89  -83.34  -96.41 
        KK_AG           na      na      na      na      na  ...       28.25   29.28   30.32   31.37   32.42   33.47 
        UU_CGU       69.35   70.73   77.93   88.62   95.53  ...     2173.77 2268.92 2381.03 2510.32 2652.46 2800.12 
        UU_DPU       56.29   58.60   61.96   67.21   74.28  ...     1531.90 1584.26 1642.98 1712.98 1794.29 1879.14 
        UU_DPUU      56.29   58.60   61.96   67.21   74.28  ...     1533.35 1585.75 1644.53 1714.60 1795.98 1880.92 
        UU_IFU          na      na      na      na      na  ...     1566.97 1591.35 1676.27 1802.71 1895.92 1952.48 
        UU_IHU          na      na      na      na      na  ...      726.94  755.66  784.87  815.30  855.34  904.62 
        UU_MU           na      na      na      na      na  ...        0.28    0.28    0.28    0.28    0.28    0.28 
        UU_NAWRU        na      na      na      na      na  ...        0.14    0.14    0.14    0.14    0.14    0.14 
        UU_WBU      256.18  268.75  295.29  324.89  366.34  ...     7072.79 7328.34 7664.35 8073.34 8525.34 8986.57 
        <BLANKLINE>
        >>> # content of the debug file
        >>> with open(debug_file, "r") as f:         # doctest: +NORMALIZE_WHITESPACE
        ...     for line in f:
        ...         print(line.strip())
        ...
        ACAF -> KK_AF       (Rule KK_--+++++++++++++)
        ACAG -> KK_AG       (Rule KK_--+++++++++++++)
        CGU -> UU_CGU       (Rule UU_++++++++++++++++)
        DPU -> UU_DPU       (Rule UU_++++++++++++++++)
        DPUU -> UU_DPUU     (Rule UU_++++++++++++++++)
        IFU -> UU_IFU       (Rule UU_++++++++++++++++)
        IHU -> UU_IHU       (Rule UU_++++++++++++++++)
        MU -> UU_MU         (Rule UU_++++++++++++++++)
        NAWRU -> UU_NAWRU   (Rule UU_++++++++++++++++)
        WBU -> UU_WBU       (Rule UU_++++++++++++++++)
        """
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
        r"""
        Convert an IODE Variables file to a format used by some other programs. 
        The possible output formats are:
          
          - `CSV` 
          - `RCSV` 
          - `DIF` 
          - `WKS` 
          - `TSP` 

        If an IODE Comments file is passed, comments of the same name will be associated with variables 
        in the result file when the output format allows it.

        The rule file is a simple text file contains the rules for: 
        
          - selecting the variables (and comments) to be exported 
          - determining the new variables names in the saved file.

        Each rule consists of two fields:

          - the selection pattern, containing a description of the names concerned by the rule. 
            This mask is defined in the same way as the :py:meth:`~iode.Comments.search` method.
          - the transcoding algorithm for the names, which can contain : 
            - `+` : indicates that the character must be included in the name 
            - `-` : indicates that the character should be skipped 
            - any other character: included in the name 
        
        Example:

            B* C+-+          -> transforms B1234 into CB2, BCDEF into CBE, etc 
            *X ++++++++++    -> keeps names ending in X unchanged
            * ++++++++++     -> keeps all names unchanged

        Parameters
        ----------
        variables_file : str or Path
            The path to the input Variables file to be converted. 
        rule_file : str or Path
            The path to the rule file that defines the selection and transcoding rules. 
        save_file : str or Path
            The path to the output file where the IODE variables will be saved.
        export_format : str or ExportFormats
            The format of the output file. Possible formats are CSV, RCSV (rotated CSV), 
            DIF, WKS, TSP.
        from_period : str or Period, optional
            The first period of the series to be exported. 
        to_period : str or Period, optional
            The last period of the series to be exported.        
        comments_file : str or Path, optional
            The path to the input Comments file. 
        nan_value: str, optional
            The value to be used for missing data. 
            Only used for `CSV` and `RCSV` (rotated CSV).
            Default is "#N/A". 
        separator: str, optional
            The character to be used as separator. 
            Only used for `CSV` and `RCSV` (rotated CSV).
            Default is ";".
        debug_file : str or Path, optional
            The path to the debug file where the debug information will be saved. 
            If not provided, the debug information will be printed to the console.

        Examples
        --------
        >>> from pathlib import Path
        >>> from iode import SAMPLE_DATA_DIR, comments, variables, ExportFormats
        >>> variables_file = f"{SAMPLE_DATA_DIR}/fun.av"
        >>> comments_file = f"{SAMPLE_DATA_DIR}/fun.ac"
        >>> rule_file = f"{SAMPLE_DATA_DIR}/rules.txt"
        >>> from_period = "2000Y1"
        >>> to_period = "2010Y1"
        
        >>> comments.load(comments_file)            # doctest: +NORMALIZE_WHITESPACE, +ELLIPSIS
        Loading .../fun.ac
        Reading object 1 : ACAF
        Reading object 2 : ACAG
        ...
        Reading object 316 : ZX
        Reading object 317 : ZZ_
        317 objects loaded 
        >>> variables.load(variables_file)          # doctest: +NORMALIZE_WHITESPACE, +ELLIPSIS
        Loading .../fun.av
        Reading object 1 : ACAF
        Reading object 2 : ACAG
        ...
        Reading object 393 : ZX
        Reading object 394 : ZZF_
        394 objects loaded

        >>> # print file containing rules
        >>> with open(rule_file, "r") as f:         # doctest: +NORMALIZE_WHITESPACE
        ...     print(f.read())
        ...
        AC*  KK_--+++++++++++++
        *U   UU_++++++++++++++++
        >>> # get list of variables with a name starting with 'AC' 
        >>> # and ending with 'U'
        >>> variables.get_names("AC*;*U")
        ['ACAF', 'ACAG', 'CGU', 'DPU', 'DPUU', 'IFU', 'IHU', 'MU', 'NAWRU', 'WBU']
        >>> # get list of comments with a name starting with 'AC' 
        >>> # and ending with 'U'
        >>> comments.get_names("AC*;*U")
        ['ACAF', 'ACAG', 'DPU', 'DPUU', 'IFU', 'IHU', 'WBU']

        >>> # export variables to CSV
        >>> export_format = ExportFormats.CSV
        >>> save_file = "exported_var.csv"
        >>> debug_file = "debug_csv.log"
        >>> variables.export_as_file(variables_file, rule_file, save_file, export_format, 
        ...                          from_period, to_period, comments_file, debug_file=debug_file)      # doctest: +NORMALIZE_WHITESPACE, +ELLIPSIS
        Loading ...\fun.av
        Reading object 1 : ACAF
        Reading object 2 : ACAG
        ...
        Reading object 393 : ZX
        Reading object 394 : ZZF_
        394 objects loaded
        Loading ...\fun.ac
        Reading object 1 : ACAF
        Reading object 2 : ACAG
        ...
        Reading object 316 : ZX
        Reading object 317 : ZZ_
        317 objects loaded     
        >>> # check content of the saved file
        >>> # note: no comment found for variables CGU, MU and NAWRU
        >>> with open(save_file, "r") as f:         # doctest: +NORMALIZE_WHITESPACE, +ELLIPSIS
        ...     print(f.read())
        ...
        code;comment;2000Y1;2001Y1;2002Y1;2003Y1;2004Y1;2005Y1;2006Y1;2007Y1;2008Y1;2009Y1;2010Y1;
        KK_AF; Ondernemingen: ontvangen kapitaaloverdrachten.; 10.046611;...;-37.827429;
        KK_AG; Totale overheid: netto ontvangen kapitaaloverdrachten.; -41.534787;...;28.253929;
        UU_CGU; ; 1383.2586;...;2173.7682;
        UU_DPU; Nominale afschrijvingen op de kapitaalvoorraad.; 953.60012;...;1531.9025;
        UU_DPUU; Nominale afschrijvingen op de kapitaalvoorraad (aangepast:  inkomensoptiek).; 954.5012;...;1533.35;
        UU_IFU; Bruto kapitaalvorming: ondernemingen.; 1076.1795;...;1566.9738;
        UU_IHU; Bruto kapitaalvorming: gezinnen.; 471.00145;...;726.93744;
        UU_MU; ; 0.42001992;...;0.27826033;
        UU_NAWRU; ; 0.14141811;...;0.13964585;
        UU_WBU; Totale loonmassa (inclusief werkgeversbijdragen).; 4922.5664;...;7072.7855;
        >>> # content of the debug file
        >>> with open(debug_file, "r") as f:         # doctest: +NORMALIZE_WHITESPACE
        ...     print(f.read())
        ...
        ACAF -> KK_AF       (Rule KK_--+++++++++++++)
        ACAG -> KK_AG       (Rule KK_--+++++++++++++)
        CGU -> UU_CGU       (Rule UU_++++++++++++++++)
        DPU -> UU_DPU       (Rule UU_++++++++++++++++)
        DPUU -> UU_DPUU     (Rule UU_++++++++++++++++)
        IFU -> UU_IFU       (Rule UU_++++++++++++++++)
        IHU -> UU_IHU       (Rule UU_++++++++++++++++)
        MU -> UU_MU         (Rule UU_++++++++++++++++)
        NAWRU -> UU_NAWRU   (Rule UU_++++++++++++++++)
        WBU -> UU_WBU       (Rule UU_++++++++++++++++)

        >>> # export variables to rotated CSV
        >>> export_format = ExportFormats.RCSV
        >>> save_file = "exported_var.rcsv"
        >>> debug_file = "debug_rcsv.log"
        >>> variables.export_as_file(variables_file, rule_file, save_file, export_format, 
        ...                          from_period, to_period, comments_file, debug_file=debug_file)      # doctest: +NORMALIZE_WHITESPACE, +ELLIPSIS
        Loading ...\fun.av
        Reading object 1 : ACAF
        Reading object 2 : ACAG
        ...
        Reading object 393 : ZX
        Reading object 394 : ZZF_
        394 objects loaded
        Loading ...\fun.ac
        Reading object 1 : ACAF
        Reading object 2 : ACAG
        ...
        Reading object 316 : ZX
        Reading object 317 : ZZ_
        317 objects loaded 
        >>> # check content of the saved file
        >>> # warning: the comments file is not used for rotated CSV
        >>> with open(save_file, "r") as f:         # doctest: +NORMALIZE_WHITESPACE, +ELLIPSIS
        ...     print(f.read())
        ...
        ; KK_AF; KK_AG; UU_CGU; UU_DPU; UU_DPUU; UU_IFU; UU_IHU; UU_MU; UU_NAWRU; UU_WBU;
        2000Y1; 10.046611; -41.534787; ... 0.42001992; 0.14141811; 4922.5664;
        2001Y1; 2.8679227; 18.939801; ... 0.40711156; 0.14138538; 5138.9458;
        2002Y1; -0.92921251; 19.980815; ... 0.39212964; 0.14125761; 5341.3233;
        2003Y1; -6.091565; 21.020502; ... 0.37923534; 0.14106277; 5556.2476;
        2004Y1; -14.582094; 22.066476; ... 0.36772624; 0.14083541; 5696.1652;
        2005Y1; -26.53879; 23.107962; ... 0.35617242; 0.14059196; 5814.7965;
        2006Y1; -28.987288; 24.129637; ... 0.34370718; 0.14034559; 6015.8951;
        2007Y1; -33.378426; 25.160909; ... 0.32978662; 0.1401158; 6295.5108;
        2008Y1; -38.409518; 26.192111; ... 0.31416594; 0.13991636; 6650.3069;
        2009Y1; -37.46351; 27.229955; ... 0.29691377; 0.13975922; 6861.5824;
        2010Y1; -37.827429; 28.253929; ... 0.27826033; 0.13964585; 7072.7855;

        >>> # export variables to TSP
        >>> export_format = ExportFormats.TSP
        >>> save_file = "exported_var.tsp"
        >>> debug_file = "debug_tsp.log"
        >>> variables.export_as_file(variables_file, rule_file, save_file, export_format, 
        ...                          from_period, to_period, comments_file, debug_file=debug_file)      # doctest: +NORMALIZE_WHITESPACE, +ELLIPSIS
        Loading ...\fun.av
        Reading object 1 : ACAF
        Reading object 2 : ACAG
        ...
        Reading object 393 : ZX
        Reading object 394 : ZZF_
        394 objects loaded
        Loading ...\fun.ac
        Reading object 1 : ACAF
        Reading object 2 : ACAG
        ...
        Reading object 316 : ZX
        Reading object 317 : ZZ_
        317 objects loaded 
        >>> # check content of the saved file
        >>> with open(save_file, "r") as f:         # doctest: +NORMALIZE_WHITESPACE, +ELLIPSIS
        ...     print(f.read())
        ...
        FREQ A;
        SMPL 2000 2010 ;
        LOAD KK_AF ;
        <BLANKLINE>
        ? Ondernemingen: ontvangen kapitaaloverdrachten.
        10.046611 2.8679227 -0.92921251 -6.091565 -14.582094 -26.53879 -28.987288
        -33.378426 -38.409518 -37.46351 -37.827429
        ;
        LOAD KK_AG ;
        <BLANKLINE>
        ? Totale overheid: netto ontvangen kapitaaloverdrachten.
        -41.534787 18.939801 19.980815 21.020502 22.066476 23.107962 24.129637
        25.160909 26.192111 27.229955 28.253929
        ;
        LOAD UU_CGU ;
        <BLANKLINE>
        1383.2586 1463.8679 1539.7198 1615.7986 1672.9449 1723.928 1787.6062 1873.3473
        1987.2756 2083.4747 2173.7682
        ;
        LOAD UU_DPU ;
        <BLANKLINE>
        ? Nominale afschrijvingen op de kapitaalvoorraad.
        953.60012 1007.4142 1056.9933 1101.323 1143.7441 1195.2886 1252.6131 1316.011
        1402.6103 1471.9715 1531.9025
        ;
        LOAD UU_DPUU ;
        <BLANKLINE>
        ? Nominale afschrijvingen op de kapitaalvoorraad (aangepast:
        ? inkomensoptiek).
        954.5012 1008.3662 1057.9921 1102.3636 1144.8248 1196.418 1253.7967 1317.2545
        1403.9357 1473.3624 1533.35
        ;
        LOAD UU_IFU ;
        <BLANKLINE>
        ? Bruto kapitaalvorming: ondernemingen.
        1076.1795 1136.1372 1150.8846 1197.4509 1277.7354 1399.6986 1490.8074 1562.3181
        1617.3543 1602.6872 1566.9738
        ;
        LOAD UU_IHU ;
        <BLANKLINE>
        ? Bruto kapitaalvorming: gezinnen.
        471.00145 486.53108 514.2291 552.55878 600.62646 634.63051 655.61486 671.66908
        665.98197 697.87295 726.93744
        ;
        LOAD UU_MU ;
        <BLANKLINE>
        0.42001992 0.40711156 0.39212964 0.37923534 0.36772624 0.35617242 0.34370718
        0.32978662 0.31416594 0.29691377 0.27826033
        ;
        LOAD UU_NAWRU ;
        <BLANKLINE>
        0.14141811 0.14138538 0.14125761 0.14106277 0.14083541 0.14059196 0.14034559
        0.1401158 0.13991636 0.13975922 0.13964585
        ;
        LOAD UU_WBU ;
        <BLANKLINE>
        ? Totale loonmassa (inclusief werkgeversbijdragen).
        4922.5664 5138.9458 5341.3233 5556.2476 5696.1652 5814.7965 6015.8951 6295.5108
        6650.3069 6861.5824 7072.7855
        ;
        """
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
        columns = {"name": names}
        for t, period in enumerate(self.periods):
            columns[period] = [self.database_ptr.get_var(<string>(name.encode()), <int>t, self.mode_) for name in names]
        return table2str(columns, max_lines=10, max_width=100, precision=2, justify_funcs={"name": JUSTIFY.LEFT})

    def __hash__(self) -> int:
        """
        Return a hash value for the current Variables database.

        Examples
        --------
        >>> from iode import SAMPLE_DATA_DIR, variables
        >>> variables.load(f"{SAMPLE_DATA_DIR}/fun.var")        # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Loading .../fun.var
        394 objects loaded
        >>> len(variables)
        394
        >>> original_hash = hash(variables)
        
        >>> # rename 1 variable
        >>> variables.rename("ACAF", "ACAF_")
        >>> original_hash == hash(variables)
        False
        >>> variables.rename("ACAF_", "ACAF")  # revert the change
        >>> original_hash == hash(variables)
        True

        >>> # modify one variable
        >>> original_variable = variables["ACAF"]
        >>> variables["ACAF"] = 0.0
        >>> original_hash == hash(variables)
        False
        >>> variables["ACAF"] = original_variable  # revert the change
        >>> original_hash == hash(variables)
        True

        >>> # delete a variable
        >>> original_variable = variables["ACAF"]
        >>> del variables["ACAF"]
        >>> original_hash == hash(variables)
        False
        >>> variables["ACAF"] = original_variable
        >>> original_hash == hash(variables)
        True

        >>> # add a variable
        >>> variables["NEW"] = 0.0
        >>> original_hash == hash(variables)
        False
        >>> del variables["NEW"]
        >>> original_hash == hash(variables)
        True
        """
        return hash_value(dereference(self.database_ptr))


variables: Variables = Variables._from_ptr()
