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
from numpy.math cimport NAN as C_NAN

from libc.stdlib cimport malloc, free
from libcpp.string cimport string
from libcpp.vector cimport vector
from cython.operator cimport dereference

from pyiode.common cimport IODE_NAN, IodeVarMode, IodeLowToHigh, IodeHighToLow, VariablesInitialization
from pyiode.time.sample cimport CSample
from pyiode.iode_database.cpp_api_database cimport hash_value
from pyiode.iode_database.cpp_api_database cimport IODE_NAN
from pyiode.iode_database.cpp_api_database cimport IodeGetVector, IodeSetVector, IodeCalcSamplePosition
from pyiode.iode_database.cpp_api_database cimport KDBVariables as CKDBVariables
from pyiode.iode_database.cpp_api_database cimport Variables as cpp_global_variables
from pyiode.iode_database.cpp_api_database cimport low_to_high as cpp_low_to_high
from pyiode.iode_database.cpp_api_database cimport high_to_low as cpp_high_to_low

KeyName = Union[str, List[str]]
KeyPeriod = Union[None, str, List[str], slice]
KeyVariable = Union[KeyName, Tuple[KeyName, KeyPeriod]]


cdef inline _create_numpy_array(double* c_array, int size):
    """Create a Numpy array from a C array of floats."""
    cdef double NA = IODE_NAN * (1.0 - 1.e-10)
    cdef cnp.ndarray[cnp.float64_t, ndim=1] numpy_array
    cdef cnp.npy_intp dimensions[1]
    dimensions[0] = <cnp.npy_intp> size

    # Create a Numpy array from the C array
    numpy_array = cnp.PyArray_SimpleNewFromData(1, dimensions, cnp.NPY_FLOAT64, <void*> c_array)

    # Set the base object of the Numpy array to NULL to prevent the Python garbage collector
    # from trying to free the memory allocated by malloc.
    cnp.PyArray_SetBaseObject(numpy_array, None)

    # Replace IODE NA by Numpy NaN
    for i in range(size):
        if numpy_array[i] <= NA:
            numpy_array[i] = C_NAN

    return numpy_array

cdef inline _df_to_ws_pos(df, start_period: str, last_period:str, int* la_pos, int* ws_pos, int* lg):
    '''
    Calls C API fonction IodeCalcSamplePosition() to determine which elements of array to copy into KV_WS and 
    at which position in the KV_WS sample.
    '''
    IodeCalcSamplePosition(_cstr(start_period), _cstr(last_period), la_pos, ws_pos, lg)

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
    >>> variables.load(f"{SAMPLE_DATA_DIR}/fun.var")
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
        >>> variables.load(f"{SAMPLE_DATA_DIR}/fun.var")
        >>> # get the value of the first period of the first variable
        >>> variables.i[0, 0]
        -2e+37
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

    def _get_variable(self, key: Union[str, int], periods_: Union[str, int, Tuple[int, int], List[str]]) -> Union[float, np.ndarray]: 
        cdef double* c_array = NULL
        cdef var_pos
        cdef period_pos
        cdef int nb_periods = self.database_ptr.get_nb_periods()
        cdef CSample* c_sample = self.database_ptr.get_sample()

        if c_sample is NULL:
            raise RuntimeError("The database has not been initialized yet. Empty sample.")

        if isinstance(key, int):
            var_pos = key
        else:
            key = key.strip()
            var_pos = self.database_ptr.get_position(key.encode())

        # periods_ represents all periods
        if periods_ is None:
            size = nb_periods
            c_array = <double*> malloc(sizeof(double) * nb_periods) 
            for t in range(nb_periods):
                c_array[t] = self.database_ptr.get_var(var_pos, t, self.mode_) 
        # periods_ represents a unique period 
        elif isinstance(periods_, (str, int)):
            if isinstance(periods_, str):
                period_pos = c_sample.get_period_position(<string>periods_.encode())
            else:
                period_pos = periods_
            if not 0 <= period_pos < nb_periods:
                raise ValueError(f"Invalid period '{periods_}'")
            return self.database_ptr.get_var(var_pos, period_pos, self.mode_)
        # periods_ represents a contiguous range of periods
        elif isinstance(periods_, tuple):
            start_period, end_period = periods_
            if end_period == 0 or end_period >= nb_periods:
                end_period = nb_periods - 1
            size = end_period - start_period + 1
            if size <= 0:
                str_start_period = self.database_ptr.get_period(<int>start_period).decode()
                str_end_period = self.database_ptr.get_period(<int>end_period).decode()
                raise ValueError(f"Invalid period range '{str_start_period}:{str_end_period}'")
            c_array = <double*> malloc(sizeof(double) * size) 
            for t in range(start_period, end_period+1):
                c_array[t - start_period] = self.database_ptr.get_var(var_pos, t, self.mode_) 
        # periods_ represents a range/list of periods
        elif isinstance(periods_, list):
            if isinstance(key, int):
                raise TypeError("variables[key, periods]: 'key' must be of type str when 'periods' is of type list.")
            else:
                size = len(periods_)
                c_array = <double*> malloc(sizeof(double) * size) 
                for i, period in enumerate(periods_):
                    period_pos = c_sample.get_period_position(<string>period.encode())
                    c_array[i] = self.database_ptr.get_var(var_pos, period_pos, self.mode_)
        else:
            raise TypeError("Wrong selection of periods. Expected None or value of type str, int, or list(str). "
                            f"Got value of type {type(periods_).__name__} instead.")
        np_var_array = _create_numpy_array(c_array, size)
        if c_array is not NULL:
            free(c_array)
        return np_var_array

    def _ws_to_numpy_array(self, vars_list: List[str], nb_periods: int) -> np.ndarray:
        # Copy values from the IODE Variables database to a Numpy 2D array
        data = np.empty((len(vars_list), nb_periods), dtype=np.float64)
        for i, name in enumerate(vars_list):
            data[i] = self._get_variable(name, None)
        return data

    # overriden for Variables
    def __getitem__(self, key) -> Union[float, np.ndarray, Variables]:
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
        >>> variables.load(f"{SAMPLE_DATA_DIR}/fun.var")
        >>> variables.sample
        Sample("1960Y1:2015Y1")

        >>> # -------- a) get one Variable --------
        >>> # get the variable values for the whole sample
        >>> variables["ACAF"]                       # doctest: +ELLIPSIS 
        array([         nan,          nan,          nan,          nan,
                        nan,          nan,          nan,          nan,
                        nan,          nan,   1.2130001 ,   5.2020001 ,
                 9.184     ,   8.0790005 ,  11.332     ,  13.518001  ,
                15.784     ,  16.544001  ,  21.489     ,  20.281     ,
                21.277     ,  32.417999  ,  24.446999  ,  27.025002  ,
                24.504     ,  27.560999  ,  25.542     ,  27.499001  ,
                25.353001  ,  17.165001  ,  23.771     ,  26.240999  ,
                30.159     ,  34.661999  ,   8.1610022 , -13.130997  ,
                32.171001  ,  39.935799  ,  29.645657  ,  13.53040492,
                10.04661079,   2.86792274,  -0.92921251,  -6.09156499,
               -14.58209446, -26.53878957, -28.98728798, -33.37842578,
               -38.40951778, -37.46350964, -37.82742883, -44.54479263,
               -55.55928982, -68.89465432, -83.34062511, -96.41041983])
        >>> # get the variable value for a specific period
        >>> variables["ACAF", "1990Y1"]
        23.771
        >>> # get the variable values for range of periods (using a Python slice)
        >>> variables["ACAF", "1990Y1":"2000Y1"]    # doctest: +ELLIPSIS 
        array([ 23.771     ,  26.240999  ,  30.159     ,  34.661999  ,
                 8.1610022 , -13.130997  ,  32.171001  ,  39.935799  ,
                29.645657  ,  13.53040492,  10.04661079])
        >>> # same as above but with the colon ':' inside the periods range string
        >>> variables["ACAF", "1990Y1:2000Y1"]      # doctest: +ELLIPSIS 
        array([ 23.771     ,  26.240999  ,  30.159     ,  34.661999  ,
                 8.1610022 , -13.130997  ,  32.171001  ,  39.935799  ,
                29.645657  ,  13.53040492,  10.04661079])

        >>> # b) -------- get a subset of the Variables database using a pattern --------
        >>> variables_subset = variables["A*"]
        >>> variables_subset.names
        ['ACAF', 'ACAG', 'AOUC', 'AOUC_', 'AQC']
        >>> # get the variable values for a specific period
        >>> variables["A*", "1990Y1"]
        array([ 23.771     , -28.17218557,   1.        ,   0.93735915,   1.        ])
        >>> # get the variable values for range of periods (using a Python slice)
        >>> variables["A*", "1990Y1":"2000Y1"]      # doctest: +ELLIPSIS 
        array([[ 23.771     ,  26.240999  ,  30.159     ,  34.661999  ,
                  8.1610022 , -13.130997  ,  32.171001  ,  39.935799  ,
                 29.645657  ,  13.53040492,  10.04661079],
               [-28.17218557, -30.934     , -40.285999  , -43.157997  ,
                -16.029003  , -41.845993  , -40.237     , -32.93      ,
                -38.345695  , -39.85817413, -41.53478657],
               [  1.        ,   1.02443339,   1.0314501 ,   1.03091768,
                  1.04628419,   1.0498914 ,   1.05368175,   1.0798151 ,
                  1.08976062,   1.10803581,   1.11623762],
               [  0.93735915,   0.96466659,   0.97403904,   0.97881286,
                  0.98955638,   0.99526324,   1.00320369,   1.02825881,
                  1.03849292,   1.0809661 ,   1.1019572 ],
               [  1.        ,   1.0628064 ,   1.1102825 ,   1.1532652 ,
                  1.1571276 ,   1.1616869 ,   1.1580297 ,   1.201328  ,
                  1.2031082 ,   1.34296997,   1.33860286]])

        >>> # c) -------- get a subset of the Variables database using a list of names --------
        >>> variables_subset = variables[["ACAF", "ACAG", "AQC", "BQY", "BVY"]]
        >>> variables_subset.names
        ['ACAF', 'ACAG', 'AQC', 'BQY', 'BVY']
        >>> # get the variable values for a specific period
        >>> variables[["ACAF", "ACAG", "AQC", "BQY", "BVY"], "1990Y1"]
        array([ 23.771     , -28.17218557,   1.        , -34.099998  , -34.099997  ])
        >>> # get the variable values for range of periods (using a Python slice)
        >>> variables[["ACAF", "ACAG", "AQC", "BQY", "BVY"], "1990Y1":"2000Y1"]      # doctest: +ELLIPSIS 
        array([[  23.771     ,   26.240999  ,   30.159     ,   34.661999  ,
                   8.1610022 ,  -13.130997  ,   32.171001  ,   39.935799  ,
                  29.645657  ,   13.53040492,   10.04661079],
               [ -28.17218557,  -30.934     ,  -40.285999  ,  -43.157997  ,
                 -16.029003  ,  -41.845993  ,  -40.237     ,  -32.93      ,
                 -38.345695  ,  -39.85817413,  -41.53478657],
               [   1.        ,    1.0628064 ,    1.1102825 ,    1.1532652 ,
                   1.1571276 ,    1.1616869 ,    1.1580297 ,    1.201328  ,
                   1.2031082 ,    1.34296997,    1.33860286],
               [ -34.099998  ,   -1.2597286 ,  -13.746386  ,   52.161541  ,
                  66.625153  ,   91.089355  ,  104.67634   ,  113.51928   ,
                 116.18705   ,  117.90844709,  119.95508985],
               [ -34.099997  ,   -1.3000031 ,  -14.699997  ,   58.100002  ,
                  75.900002  ,  105.5       ,  123.2       ,  135.6192    ,
                 140.73978   ,  144.85878185,  150.05335231]])
        """
        names, periods_ = self._unfold_key(key)
        # names represents a single Variable
        if len(names) == 1:
            return self._get_variable(names[0], periods_)
        # names represents a selection of Variables
        else:
            # periods_ represents all periods
            if periods_ is None:
                names = ';'.join(names)
                return self._subset(names, copy=False)
            # periods_ represents a unique period 
            elif isinstance(periods_, (str, int)):
                vars_array = np.empty(len(names), dtype=np.float64)
                for i, name in enumerate(names):
                    vars_array[i] = self._get_variable(name, periods_)
                return vars_array
            # periods_ represents a range of periods (slice or list)
            else:
                first_var = self._get_variable(names[0], periods_)
                vars_array = np.empty((len(names), first_var.size), dtype=np.float64)
                vars_array[0, :] = first_var
                for i, name in enumerate(names[1:], start=1):
                    vars_array[i, :] = self._get_variable(name, periods_)
                return vars_array

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

        # values is a LEC expression
        if isinstance(values, str):
            self.database_ptr.add(<string>(name.encode()), <string>values.encode())
        # values is a vector of float
        else:
            cpp_values = [<double>value_ for value_ in values]
            self.database_ptr.add(<string>(name.encode()), cpp_values)

    def _update(self, key: int, values: Union[str, float, List[float]]):
        cdef vector[double] cpp_values

        # values is a LEC expression
        if isinstance(values, str):
            self.database_ptr.update(<int>key, <string>values.encode())
        # values is a vector of float
        else:
            cpp_values = [<double>value_ for value_ in values]
            self.database_ptr.update(<int>key, cpp_values)

    def _set_variable(self, key: Union[str, int], values: Any, periods_: Optional[str, int, Tuple[int, int], List[str]]):
        cdef vector[double] cpp_values
        cdef int var_pos
        cdef int period_pos
        cdef int nb_periods = self.database_ptr.get_nb_periods()
        cdef CSample* c_sample = self.database_ptr.get_sample()

        if c_sample is NULL:
            raise RuntimeError("The database has not been initialized yet. Empty sample.")

        if isinstance(key, str):
            key = key.strip()
        
        # new Variable -> raise an error if values is a vector and len(values) != self.nb_periods
        if isinstance(key, str) and key not in self:
            values = self._convert_values(values, self.nb_periods)
            self._add(key, values)
        # update a Variable
        else:
            if isinstance(key, int):
                var_pos = key
            else:
                var_pos = self.database_ptr.get_position(key.encode())
            
            # update values for the whole sample
            # raise an error if values is a vector and len(values) != self.nb_periods
            if periods_ is None:
                values = self._convert_values(values, self.nb_periods)
                self._update(var_pos, values)
            # update the value for only one period 
            elif isinstance(periods_, (str, int)):
                if isinstance(periods_, str):
                    period_pos = c_sample.get_period_position(<string>periods_.encode())
                else:
                    period_pos = periods_
                if not 0 <= period_pos < nb_periods:    
                    raise ValueError(f"'period' {periods_} must be a valid period name or a valid period index.") 
                value = self._convert_values(values, 1)
                self.database_ptr.set_var(var_pos, period_pos, <double>value, self.mode_)
            # update values for a contiguous range of periods
            elif isinstance(periods_, tuple):
                first_period, last_period = periods_
                if not isinstance(first_period, int) or not isinstance(last_period, int):
                    raise TypeError("Expected int values for 'first_period' and 'last_period'")
                if first_period > last_period:
                    raise ValueError("'first_period' must be lower or equal to 'last_period'.")
                nb_periods = last_period - first_period + 1
                # values is a LEC expression
                if isinstance(values, str):
                    self.database_ptr.update(var_pos, <string>values.encode(), <int>first_period, <int>last_period) 
                else:
                    values = self._convert_values(values, nb_periods)
                    cpp_values = [<double>value for value in values]
                    self.database_ptr.update(var_pos, cpp_values, <int>first_period, <int>last_period)
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
                        period_pos = c_sample.get_period_position(<string>period.encode())
                        self.database_ptr.set_var(var_pos, period_pos, <double>value, self.mode_)

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
        >>> from iode import SAMPLE_DATA_DIR
        >>> from iode import variables, NA
        >>> variables.load(f"{SAMPLE_DATA_DIR}/fun.var")
        
        >>> # a) -------- add one variable --------
        >>> # 1) same value for all periods
        >>> variables["A0"] = NA
        >>> variables["A0"]                     # doctest: +ELLIPSIS 
        array([ nan,  nan,  nan,  nan,  nan,  nan,  nan,  nan,  nan,  nan,  nan,
                nan,  nan,  nan,  nan,  nan,  nan,  nan,  nan,  nan,  nan,  nan,
                nan,  nan,  nan,  nan,  nan,  nan,  nan,  nan,  nan,  nan,  nan,
                nan,  nan,  nan,  nan,  nan,  nan,  nan,  nan,  nan,  nan,  nan,
                nan,  nan,  nan,  nan,  nan,  nan,  nan,  nan,  nan,  nan,  nan,
                nan])
        >>> # 2) vector (list) containing a specific value for each period
        >>> variables["A1"] = list(range(variables.nb_periods))
        >>> variables["A1"]                     # doctest: +ELLIPSIS 
        array([  0.,   1.,   2.,   3.,   4.,   5.,   6.,   7.,   8.,   9.,  10.,
                11.,  12.,  13.,  14.,  15.,  16.,  17.,  18.,  19.,  20.,  21.,
                22.,  23.,  24.,  25.,  26.,  27.,  28.,  29.,  30.,  31.,  32.,
                33.,  34.,  35.,  36.,  37.,  38.,  39.,  40.,  41.,  42.,  43.,
                44.,  45.,  46.,  47.,  48.,  49.,  50.,  51.,  52.,  53.,  54.,
                55.])
        >>> # 3) LEC expression
        >>> variables["A2"] = "t + 10"
        >>> variables["A2"]                     # doctest: +ELLIPSIS 
        array([ 10.,  11.,  12.,  13.,  14.,  15.,  16.,  17.,  18.,  19.,  20.,
                21.,  22.,  23.,  24.,  25.,  26.,  27.,  28.,  29.,  30.,  31.,
                32.,  33.,  34.,  35.,  36.,  37.,  38.,  39.,  40.,  41.,  42.,
                43.,  44.,  45.,  46.,  47.,  48.,  49.,  50.,  51.,  52.,  53.,
                54.,  55.,  56.,  57.,  58.,  59.,  60.,  61.,  62.,  63.,  64.,
                65.])

        >>> # b) -------- update one variable --------
        >>> # 1) update all values of a Variable
        >>> variables["ACAF"]                   # doctest: +ELLIPSIS 
        array([         nan,          nan,          nan,          nan,
                        nan,          nan,          nan,          nan,
                        nan,          nan,   1.2130001 ,   5.2020001 ,
                 9.184     ,   8.0790005 ,  11.332     ,  13.518001  ,
                15.784     ,  16.544001  ,  21.489     ,  20.281     ,
                21.277     ,  32.417999  ,  24.446999  ,  27.025002  ,
                24.504     ,  27.560999  ,  25.542     ,  27.499001  ,
                25.353001  ,  17.165001  ,  23.771     ,  26.240999  ,
                30.159     ,  34.661999  ,   8.1610022 , -13.130997  ,
                32.171001  ,  39.935799  ,  29.645657  ,  13.53040492,
                10.04661079,   2.86792274,  -0.92921251,  -6.09156499,
               -14.58209446, -26.53878957, -28.98728798, -33.37842578,
               -38.40951778, -37.46350964, -37.82742883, -44.54479263,
               -55.55928982, -68.89465432, -83.34062511, -96.41041983])
        >>> # 1.I) same value for all periods
        >>> variables["ACAF"] = NA
        >>> variables["ACAF"]                   # doctest: +ELLIPSIS 
        array([ nan,  nan,  nan,  nan,  nan,  nan,  nan,  nan,  nan,  nan,  nan,
                nan,  nan,  nan,  nan,  nan,  nan,  nan,  nan,  nan,  nan,  nan,
                nan,  nan,  nan,  nan,  nan,  nan,  nan,  nan,  nan,  nan,  nan,
                nan,  nan,  nan,  nan,  nan,  nan,  nan,  nan,  nan,  nan,  nan,
                nan,  nan,  nan,  nan,  nan,  nan,  nan,  nan,  nan,  nan,  nan,
                nan])
        >>> # 1.II) vector (list) containing a specific value for each period
        >>> variables["ACAF"] = list(range(variables.nb_periods))
        >>> variables["ACAF"]                   # doctest: +ELLIPSIS 
        array([  0.,   1.,   2.,   3.,   4.,   5.,   6.,   7.,   8.,   9.,  10.,
                11.,  12.,  13.,  14.,  15.,  16.,  17.,  18.,  19.,  20.,  21.,
                22.,  23.,  24.,  25.,  26.,  27.,  28.,  29.,  30.,  31.,  32.,
                33.,  34.,  35.,  36.,  37.,  38.,  39.,  40.,  41.,  42.,  43.,
                44.,  45.,  46.,  47.,  48.,  49.,  50.,  51.,  52.,  53.,  54.,
                55.])
        >>> # 1.III) LEC expression
        >>> variables["ACAF"] = "t + 10"
        >>> variables["ACAF"]                   # doctest: +ELLIPSIS 
        array([ 10.,  11.,  12.,  13.,  14.,  15.,  16.,  17.,  18.,  19.,  20.,
                21.,  22.,  23.,  24.,  25.,  26.,  27.,  28.,  29.,  30.,  31.,
                32.,  33.,  34.,  35.,  36.,  37.,  38.,  39.,  40.,  41.,  42.,
                43.,  44.,  45.,  46.,  47.,  48.,  49.,  50.,  51.,  52.,  53.,
                54.,  55.,  56.,  57.,  58.,  59.,  60.,  61.,  62.,  63.,  64.,
                65.])

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
        array([ 0.,  0.,  0.,  0.,  0.])
        >>> # 3.I.b) variable(periods) = vector (list) containing a specific value for each period
        >>> variables["ACAF", "1991Y1":"1995Y1"] = [0., 1., 2., 3., 4.]
        >>> variables["ACAF", "1991Y1":"1995Y1"]
        array([ 0.,  1.,  2.,  3.,  4.])
        >>> # 3.I.c) variable(periods) = LEC expression
        >>> variables["ACAF", "1991Y1":"1995Y1"] = "t + 10"
        >>> variables["ACAF", "1991Y1":"1995Y1"]
        array([ 41.,  42.,  43.,  44.,  45.])

        >>> # 3.II) same as above but with the colon ':' inside the periods range string
        >>> # 3.II.a) variable(periods) = same value for all periods
        >>> variables["ACAF", "1991Y1:1995Y1"] = 0.0
        >>> variables["ACAF", "1991Y1:1995Y1"]
        array([ 0.,  0.,  0.,  0.,  0.])
        >>> # 3.II.b) variable(periods) = vector (list) containing a specific value for each period
        >>> variables["ACAF", "1991Y1:1995Y1"] = [0., -1., -2., -3., -4.]
        >>> variables["ACAF", "1991Y1":"1995Y1"]
        array([ 0., -1., -2., -3., -4.])
        >>> # 3.II.c) variable(periods) = LEC expression
        >>> variables["ACAF", "1991Y1:1995Y1"] = "t - 10"
        >>> variables["ACAF", "1991Y1:1995Y1"]
        array([ 21.,  22.,  23.,  24.,  25.])

        >>> # c) -------- working on a subset --------
        >>> # 1) get subset
        >>> variables_subset = variables["A*"]
        >>> variables_subset.names
        ['A0', 'A1', 'A2', 'ACAF', 'ACAG', 'AOUC', 'AOUC_', 'AQC']
        >>> # 2) add a variable to the subset
        >>> variables_subset["A3"] = NA
        >>> variables_subset["A3"]              # doctest: +ELLIPSIS 
        array([ nan,  nan,  nan,  nan,  nan,  nan,  nan,  nan,  nan,  nan,  nan,
                nan,  nan,  nan,  nan,  nan,  nan,  nan,  nan,  nan,  nan,  nan,
                nan,  nan,  nan,  nan,  nan,  nan,  nan,  nan,  nan,  nan,  nan,
                nan,  nan,  nan,  nan,  nan,  nan,  nan,  nan,  nan,  nan,  nan,
                nan,  nan,  nan,  nan,  nan,  nan,  nan,  nan,  nan,  nan,  nan,
                nan])
        >>> # --> new variable also appears in the global workspace
        >>> "A3" in variables
        True
        >>> variables["A3"]                     # doctest: +ELLIPSIS 
        array([ nan,  nan,  nan,  nan,  nan,  nan,  nan,  nan,  nan,  nan,  nan,
                nan,  nan,  nan,  nan,  nan,  nan,  nan,  nan,  nan,  nan,  nan,
                nan,  nan,  nan,  nan,  nan,  nan,  nan,  nan,  nan,  nan,  nan,
                nan,  nan,  nan,  nan,  nan,  nan,  nan,  nan,  nan,  nan,  nan,
                nan,  nan,  nan,  nan,  nan,  nan,  nan,  nan,  nan,  nan,  nan,
                nan])
        >>> # 3) update a variable in the subset
        >>> variables_subset["A3"] = 0.0
        >>> variables_subset["A3"]              # doctest: +ELLIPSIS 
        array([ 0.,  0.,  0.,  0.,  0.,  0.,  0.,  0.,  0.,  0.,  0.,  0.,  0.,
                0.,  0.,  0.,  0.,  0.,  0.,  0.,  0.,  0.,  0.,  0.,  0.,  0.,
                0.,  0.,  0.,  0.,  0.,  0.,  0.,  0.,  0.,  0.,  0.,  0.,  0.,
                0.,  0.,  0.,  0.,  0.,  0.,  0.,  0.,  0.,  0.,  0.,  0.,  0.,
                0.,  0.,  0.,  0.])
        >>> # --> variable is also updated in the global workspace
        >>> variables["A3"]                     # doctest: +ELLIPSIS 
        array([ 0.,  0.,  0.,  0.,  0.,  0.,  0.,  0.,  0.,  0.,  0.,  0.,  0.,
                0.,  0.,  0.,  0.,  0.,  0.,  0.,  0.,  0.,  0.,  0.,  0.,  0.,
                0.,  0.,  0.,  0.,  0.,  0.,  0.,  0.,  0.,  0.,  0.,  0.,  0.,
                0.,  0.,  0.,  0.,  0.,  0.,  0.,  0.,  0.,  0.,  0.,  0.,  0.,
                0.,  0.,  0.,  0.])
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
        >>> variables.load(f"{SAMPLE_DATA_DIR}/fun.var")

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
        array([  0.,   1.,   2.,   3.,   4.,   5.,   6.,   7.,   8.,   9.,  10.])
        >>> variables["BXL_02"]
        array([ 88.,  89.,  90.,  91.,  92.,  93.,  94.,  95.,  96.,  97.,  98.])
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
        >>> variables.load(f"{SAMPLE_DATA_DIR}/fun.var")
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
        array([        nan,  0.24783192,  0.25456766,  0.26379573,  0.27624285,
                0.28580592,  0.29721309,  0.30634627,  0.30797943,  0.31703778,
                0.33336823,  0.35592318,  0.37846615,  0.41292134,  0.4946085 ,
                0.54713711,  0.60206004,  0.62716076,  0.64032766,  0.67017027,
                0.73722462,  0.79401212,  0.86332326,  0.91624877,  0.97916968,
                1.0021859 ,  0.94999215,  0.9340988 ,  0.94613933,  0.9787904 ,
                1.        ,  1.02443339,  1.0314501 ,  1.03091768,  1.04628419,
                1.0498914 ,  1.05368175,  1.0798151 ,  1.08976062,  1.10803581,
                1.11623762,  1.14047639,  1.15716928,  1.17048954,  1.16767464,
                1.1815207 ,  1.19946163,  1.21933288,  1.26280574,  1.28713178,
                1.3071099 ,  1.32861022,  1.35553983,  1.38777697,  1.42371396,
                1.46086261])
        >>> df.loc["AOUC"]                  # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        time
        1960Y1         NaN
        1961Y1    0.247832
        ...
        2014Y1    1.423714
        2015Y1    1.460863
        Name: AOUC, dtype: float64    
        >>> variables["ZKFO"]               # doctest: +ELLIPSIS
        array([ 1.       ,  1.       ,  1.       ,  1.       ,  1.       ,
                1.       ,  1.       ,  1.       ,  1.       ,  1.       ,
                1.       ,  1.       ,  1.       ,  1.       ,  1.       ,
                1.       ,  1.       ,  1.       ,  1.       ,  1.       ,
                1.       ,  1.       ,  1.       ,  1.       ,  1.       ,
                1.       ,  1.       ,  1.       ,  1.       ,  1.       ,
                1.       ,  1.       ,  1.       ,  1.       ,  1.       ,
                1.       ,  1.       ,  1.0159901,  1.0159901,  1.0159901,
                1.0159901,  1.0159901,  1.0159901,  1.0159901,  1.0159901,
                1.0159901,  1.0159901,  1.0159901,  1.0159901,  1.0159901,
                1.0159901,  1.0159901,  1.0159901,  1.0159901,  1.0159901,
                1.0159901])
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
        array([        nan,  0.24783192,  0.25456766,  0.26379573,  0.27624285,
                0.28580592,  0.29721309,  0.30634627,  0.30797943,  0.31703778,
                0.33336823,  0.35592318,  0.37846615,  0.41292134,  0.4946085 ,
                0.54713711,  0.60206004,  0.62716076,  0.64032766,  0.67017027,
                0.73722462,  0.79401212,  0.86332326,  0.91624877,  0.97916968,
                1.0021859 ,  0.94999215,  0.9340988 ,  0.94613933,  0.9787904 ,
                1.        ,  1.02443339,  1.0314501 ,  1.03091768,  1.04628419,
                1.0498914 ,  1.05368175,  1.0798151 ,  1.08976062,  1.10803581,
                1.11623762,  1.14047639,  1.15716928,  1.17048954,  1.16767464,
                1.1815207 ,  1.19946163,  1.21933288,  1.26280574,  1.28713178,
                1.3071099 ,  1.32861022,  1.35553983,  1.38777697,  1.42371396,
                1.46086261])
        >>> df.loc["AOUC"]                  # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        time
        1960Y1         NaN
        1961Y1    0.247832
        ...
        2014Y1    1.423714
        2015Y1    1.460863
        Name: AOUC, dtype: float64    
        >>> variables["ZZF_"]               # doctest: +ELLIPSIS
        array([ 0.68840039,  0.68840039,  0.68840039,  0.68840039,  0.68840039,
                0.68840039,  0.68840039,  0.68840039,  0.68840039,  0.68840039,
                0.68840039,  0.68840039,  0.68840039,  0.68840039,  0.68840039,
                0.68840039,  0.68840039,  0.68840039,  0.68840039,  0.68840039,
                0.68840039,  0.68840039,  0.68840039,  0.68840039,  0.68840039,
                0.68840039,  0.68840039,  0.68840039,  0.68840039,  0.68840039,
                0.68840039,  0.68840039,  0.68840039,  0.68840039,  0.68840039,
                0.68840039,  0.68840039,  0.68840039,  0.68840039,  0.68840039,
                0.68840039,  0.68840039,  0.68840039,  0.68840039,  0.68840039,
                0.68840039,  0.68840039,  0.68840039,  0.68840039,  0.68840039,
                0.68840039,  0.68840039,  0.68840039,  0.68840039,  0.68840039,
                0.68840039])
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
        data = self._ws_to_numpy_array(vars_list, len(periods_list))

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
        array([  0.,   1.,   2.,   3.,   4.,   5.,   6.,   7.,   8.,   9.,  10.])
        >>> variables["BXL_02"]
        array([ 88.,  89.,  90.,  91.,  92.,  93.,  94.,  95.,  96.,  97.,  98.])
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
        >>> variables.load(f"{SAMPLE_DATA_DIR}/fun.var")
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
        array([        nan,  0.24783192,  0.25456766,  0.26379573,  0.27624285,
                0.28580592,  0.29721309,  0.30634627,  0.30797943,  0.31703778,
                0.33336823,  0.35592318,  0.37846615,  0.41292134,  0.4946085 ,
                0.54713711,  0.60206004,  0.62716076,  0.64032766,  0.67017027,
                0.73722462,  0.79401212,  0.86332326,  0.91624877,  0.97916968,
                1.0021859 ,  0.94999215,  0.9340988 ,  0.94613933,  0.9787904 ,
                1.        ,  1.02443339,  1.0314501 ,  1.03091768,  1.04628419,
                1.0498914 ,  1.05368175,  1.0798151 ,  1.08976062,  1.10803581,
                1.11623762,  1.14047639,  1.15716928,  1.17048954,  1.16767464,
                1.1815207 ,  1.19946163,  1.21933288,  1.26280574,  1.28713178,
                1.3071099 ,  1.32861022,  1.35553983,  1.38777697,  1.42371396,
                1.46086261])
        >>> array["AOUC"]
        time  1960Y1  ...              2014Y1              2015Y1
                 nan  ...  1.4237139558484628  1.4608626117037322
        >>> variables["ZKFO"]               # doctest: +ELLIPSIS
        array([ 1.       ,  1.       ,  1.       ,  1.       ,  1.       ,
                1.       ,  1.       ,  1.       ,  1.       ,  1.       ,
                1.       ,  1.       ,  1.       ,  1.       ,  1.       ,
                1.       ,  1.       ,  1.       ,  1.       ,  1.       ,
                1.       ,  1.       ,  1.       ,  1.       ,  1.       ,
                1.       ,  1.       ,  1.       ,  1.       ,  1.       ,
                1.       ,  1.       ,  1.       ,  1.       ,  1.       ,
                1.       ,  1.       ,  1.0159901,  1.0159901,  1.0159901,
                1.0159901,  1.0159901,  1.0159901,  1.0159901,  1.0159901,
                1.0159901,  1.0159901,  1.0159901,  1.0159901,  1.0159901,
                1.0159901,  1.0159901,  1.0159901,  1.0159901,  1.0159901,
                1.0159901])
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
        array([        nan,  0.24783192,  0.25456766,  0.26379573,  0.27624285,
                0.28580592,  0.29721309,  0.30634627,  0.30797943,  0.31703778,
                0.33336823,  0.35592318,  0.37846615,  0.41292134,  0.4946085 ,
                0.54713711,  0.60206004,  0.62716076,  0.64032766,  0.67017027,
                0.73722462,  0.79401212,  0.86332326,  0.91624877,  0.97916968,
                1.0021859 ,  0.94999215,  0.9340988 ,  0.94613933,  0.9787904 ,
                1.        ,  1.02443339,  1.0314501 ,  1.03091768,  1.04628419,
                1.0498914 ,  1.05368175,  1.0798151 ,  1.08976062,  1.10803581,
                1.11623762,  1.14047639,  1.15716928,  1.17048954,  1.16767464,
                1.1815207 ,  1.19946163,  1.21933288,  1.26280574,  1.28713178,
                1.3071099 ,  1.32861022,  1.35553983,  1.38777697,  1.42371396,
                1.46086261])
        >>> array["AOUC"]
        time  1960Y1  ...              2014Y1              2015Y1
                 nan  ...  1.4237139558484628  1.4608626117037322
        >>> variables["ZZF_"]               # doctest: +ELLIPSIS
        array([ 0.68840039,  0.68840039,  0.68840039,  0.68840039,  0.68840039,
                0.68840039,  0.68840039,  0.68840039,  0.68840039,  0.68840039,
                0.68840039,  0.68840039,  0.68840039,  0.68840039,  0.68840039,
                0.68840039,  0.68840039,  0.68840039,  0.68840039,  0.68840039,
                0.68840039,  0.68840039,  0.68840039,  0.68840039,  0.68840039,
                0.68840039,  0.68840039,  0.68840039,  0.68840039,  0.68840039,
                0.68840039,  0.68840039,  0.68840039,  0.68840039,  0.68840039,
                0.68840039,  0.68840039,  0.68840039,  0.68840039,  0.68840039,
                0.68840039,  0.68840039,  0.68840039,  0.68840039,  0.68840039,
                0.68840039,  0.68840039,  0.68840039,  0.68840039,  0.68840039,
                0.68840039,  0.68840039,  0.68840039,  0.68840039,  0.68840039,
                0.68840039])
        >>> array["ZZF_"]
        time      1960Y1      1961Y1  ...      2013Y1      2014Y1      2015Y1
              0.68840039  0.68840039  ...  0.68840039  0.68840039  0.68840039
        """
        if la is None:
            raise RuntimeError("larray library not found")

        vars_list = self.names
        periods_list = self.periods_as_float if sample_as_floats else self.periods
        data = self._ws_to_numpy_array(vars_list, len(periods_list))
        
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
        >>> variables.load(f"{SAMPLE_DATA_DIR}/fun.var")
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
        >>> variables.load(f"{SAMPLE_DATA_DIR}/fun.var")
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
        >>> variables.load(f"{SAMPLE_DATA_DIR}/fun.var")
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
        >>> variables.load(f"{SAMPLE_DATA_DIR}/fun.var")
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
        >>> variables.load(f"{SAMPLE_DATA_DIR}/fun.var")
        >>> variables.sample
        Sample("1960Y1:2015Y1")
        >>> variables.periods_as_float          # doctest: +ELLIPSIS 
        [1960.0, 1961.0, ..., 2014.0, 2015.0]
        """
        return self.database_ptr.get_list_periods_as_float(bytes(), bytes())

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
        >>> variables.load(f"{SAMPLE_DATA_DIR}/fun.var")
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
        array([        nan,  0.24783192,  0.25456766,  0.26379573,  0.27624285,
                0.28580592,  0.29721309,  0.30634627,  0.30797943,  0.31703778,
                0.33336823,  0.35592318,  0.37846615,  0.41292134,  0.4946085 ,
                0.54713711,  0.60206004,  0.62716076,  0.64032766,  0.67017027,
                0.73722462,  0.79401212,  0.86332326,  0.91624877,  0.97916968,
                1.0021859 ,  0.94999215,  0.9340988 ,  0.94613933,  0.9787904 ,
                1.        ,  1.02443339,  1.0314501 ,  1.03091768,  1.04628419,
                1.0498914 ,  1.05368175,  1.0798151 ,  1.08976062,  1.10803581,
                1.11623762,  1.14047639,  1.15716928,  1.17048954,  1.16767464,
                1.1815207 ,  1.19946163,  1.21933288,  1.26280574,  1.28713178,
                1.3071099 ,  1.32861022,  1.35553983,  1.38777697,  1.42371396,
                1.46086261])
        >>> df.loc["AOUC"]                  # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        time
        1960Y1         NaN
        1961Y1    0.247832
        1962Y1    0.254568
        ...
        2014Y1    1.423714
        2015Y1    1.460863
        Name: AOUC, dtype: float64   
        >>> variables["ZKFO"]               # doctest: +ELLIPSIS
        array([ 1.       ,  1.       ,  1.       ,  1.       ,  1.       ,
                1.       ,  1.       ,  1.       ,  1.       ,  1.       ,
                1.       ,  1.       ,  1.       ,  1.       ,  1.       ,
                1.       ,  1.       ,  1.       ,  1.       ,  1.       ,
                1.       ,  1.       ,  1.       ,  1.       ,  1.       ,
                1.       ,  1.       ,  1.       ,  1.       ,  1.       ,
                1.       ,  1.       ,  1.       ,  1.       ,  1.       ,
                1.       ,  1.       ,  1.0159901,  1.0159901,  1.0159901,
                1.0159901,  1.0159901,  1.0159901,  1.0159901,  1.0159901,
                1.0159901,  1.0159901,  1.0159901,  1.0159901,  1.0159901,
                1.0159901,  1.0159901,  1.0159901,  1.0159901,  1.0159901,
                1.0159901])
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
        array([        nan,  0.24783192,  0.25456766,  0.26379573,  0.27624285,
                0.28580592,  0.29721309,  0.30634627,  0.30797943,  0.31703778,
                0.33336823,  0.35592318,  0.37846615,  0.41292134,  0.4946085 ,
                0.54713711,  0.60206004,  0.62716076,  0.64032766,  0.67017027,
                0.73722462,  0.79401212,  0.86332326,  0.91624877,  0.97916968,
                1.0021859 ,  0.94999215,  0.9340988 ,  0.94613933,  0.9787904 ,
                1.        ,  1.02443339,  1.0314501 ,  1.03091768,  1.04628419,
                1.0498914 ,  1.05368175,  1.0798151 ,  1.08976062,  1.10803581,
                1.11623762,  1.14047639,  1.15716928,  1.17048954,  1.16767464,
                1.1815207 ,  1.19946163,  1.21933288,  1.26280574,  1.28713178,
                1.3071099 ,  1.32861022,  1.35553983,  1.38777697,  1.42371396,
                1.46086261])
        >>> df.loc["AOUC"]                  # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        time
        1960Y1         NaN
        1961Y1    0.247832
        1962Y1    0.254568
        ...
        2014Y1    1.423714
        2015Y1    1.460863
        Name: AOUC, dtype: float64   
        >>> variables["ZZF_"]               # doctest: +ELLIPSIS
        array([ 0.68840039,  0.68840039,  0.68840039,  0.68840039,  0.68840039,
                0.68840039,  0.68840039,  0.68840039,  0.68840039,  0.68840039,
                0.68840039,  0.68840039,  0.68840039,  0.68840039,  0.68840039,
                0.68840039,  0.68840039,  0.68840039,  0.68840039,  0.68840039,
                0.68840039,  0.68840039,  0.68840039,  0.68840039,  0.68840039,
                0.68840039,  0.68840039,  0.68840039,  0.68840039,  0.68840039,
                0.68840039,  0.68840039,  0.68840039,  0.68840039,  0.68840039,
                0.68840039,  0.68840039,  0.68840039,  0.68840039,  0.68840039,
                0.68840039,  0.68840039,  0.68840039,  0.68840039,  0.68840039,
                0.68840039,  0.68840039,  0.68840039,  0.68840039,  0.68840039,
                0.68840039,  0.68840039,  0.68840039,  0.68840039,  0.68840039,
                0.68840039])
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
        >>> variables.load(f"{SAMPLE_DATA_DIR}/fun.var")
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
        array([-72.50614702, -76.11763972, -79.72913241, -83.34062511])
        >>> variables["ACAG", "2014Q1":"2014Q4"]
        array([ 31.63267882,  31.89521882,  32.15775882,  32.42029883])

        Linear interpolation / flow
        
        >>> # "flow" -> the result is the source value divided by the nb of sub-periods. 
        >>> variables.low_to_high(LowToHighType.FLOW, LowToHighMethod.LINEAR, filepath, ["ACAF", "ACAG"])
        >>> variables["ACAF", "2014Q1":"2014Q4"]
        array([-20.83515628, -20.83515628, -20.83515628, -20.83515628])
        >>> variables["ACAG", "2014Q1":"2014Q4"]
        array([ 8.10507471,  8.10507471,  8.10507471,  8.10507471])

        Cubic splines / stock
        
        >>> variables.low_to_high(LowToHighType.STOCK, LowToHighMethod.CUBIC_SPLINES, filepath, ["ACAF", "ACAG"])
        >>> variables["ACAF", "2012Q1":"2012Q4"]
        array([-47.29841693, -50.05204123, -52.80566552, -55.55928982])
        >>> variables["ACAG", "2012Q1":"2012Q4"]
        array([ 29.54444056,  29.80428076,  30.06412095,  30.32396115])

        Cubic splines / flow
        
        >>> variables.low_to_high(LowToHighType.FLOW, LowToHighMethod.CUBIC_SPLINES, filepath, ["ACAF", "ACAG"])
        >>> variables["ACAF", "2012Q1":"2012Q4"]
        array([-12.74842269, -13.43682876, -14.27028904, -15.10374932])
        >>> variables["ACAG", "2012Q1":"2012Q4"]
        array([ 7.48323067,  7.54819072,  7.61357683,  7.67896293])

        Step / stock
        
        >>> # "stock" -> the result has the same value as the source
        >>> variables.low_to_high(LowToHighType.STOCK, LowToHighMethod.STEP, filepath, ["ACAF", "ACAG"])
        >>> variables["ACAF", "2014Q1":"2014Q4"]
        array([-83.34062511, -83.34062511, -83.34062511, -83.34062511])
        >>> variables["ACAG", "2014Q1":"2014Q4"]
        array([ 32.42029883,  32.42029883,  32.42029883,  32.42029883])

        Step / flow
        
        >>> # "flow" -> the result is the source value plus a portion of 
        >>> # the difference between the 2 surrounding values in the source
        >>> variables.low_to_high(LowToHighType.FLOW, LowToHighMethod.STEP, filepath, ["ACAF", "ACAG"])
        >>> variables["ACAF", "2014Q1":"2014Q4"]
        array([-20.83515628, -20.83515628, -20.83515628, -20.83515628])
        >>> variables["ACAG", "2014Q1":"2014Q4"]
        array([ 8.10507471,  8.10507471,  8.10507471,  8.10507471])
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
        array([-37.82742883, -44.54479263, -55.55928982, -68.89465432, -83.34062511])
        >>> variables["ACAG", "2010Y1":"2014Y1"]
        array([ 7.06348224,  7.32115009,  7.58099029,  7.8425347 ,  8.10507471])

        Mean of year
        
        >>> variables.high_to_low(HighToLowType.MEAN, filepath, ["ACAF", "ACAG"])
        >>> variables["ACAF", "2010Y1":"2014Y1"]
        array([-37.82742883, -44.54479263, -55.55928982, -68.89465432, -83.34062511])
        >>> variables["ACAG", "2010Y1":"2014Y1"]
        array([ 7.06348224,  7.32115009,  7.58099029,  7.8425347 ,  8.10507471])

        Sum
        
        >>> variables.high_to_low(HighToLowType.SUM, filepath, ["ACAF", "ACAG"])
        >>> variables["ACAF", "2010Y1":"2014Y1"]
        array([-151.30971533, -178.17917053, -222.23715927, -275.57861729,
               -333.36250044])
        >>> variables["ACAG", "2010Y1":"2014Y1"]
        array([ 28.25392898,  29.28460036,  30.32396115,  31.37013881,  32.42029883])
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
        array([  0.,   1.,   2.,   3.,   4.,  nan,   6.,  nan,   8.,   9.,  10.])

        >>> # "TM1" (Y := Y[-1], if Y null or NA)
        >>> reset_ACAF()
        >>> variables.extrapolate(SimulationInitialization.TM1, "2005Y1", "2010Y1")
        >>> variables["ACAF", "2003Y1":"2009Y1"]
        array([ 3.,  4.,  4.,  6.,  6.,  8.,  9.])

        >>> # "TM1_A" (Y := Y[-1], always)
        >>> reset_ACAF()
        >>> variables.extrapolate(SimulationInitialization.TM1_A, "2005Y1", "2010Y1")
        >>> variables["ACAF", "2003Y1":"2009Y1"]
        array([ 3.,  4.,  4.,  4.,  4.,  4.,  4.])

        >>> # "EXTRA" (Y := extrapolation, if Y null or NA)
        >>> reset_ACAF()
        >>> variables.extrapolate(SimulationInitialization.EXTRA, "2005Y1", "2010Y1")
        >>> variables["ACAF", "2003Y1":"2009Y1"]
        array([ 3.,  4.,  5.,  6.,  7.,  8.,  9.])

        >>> # "EXTRA_A" (Y := extrapolation, always)
        >>> reset_ACAF()
        >>> variables.extrapolate(SimulationInitialization.EXTRA_A, "2005Y1", "2010Y1")
        >>> variables["ACAF", "2003Y1":"2009Y1"]
        array([ 3.,  4.,  5.,  6.,  7.,  8.,  9.])

        >>> # "ASIS" (Y unchanged)
        >>> reset_ACAF()
        >>> variables.extrapolate(SimulationInitialization.ASIS, "2005Y1", "2010Y1")
        >>> variables["ACAF", "2003Y1":"2009Y1"]
        array([  3.,   4.,  nan,   6.,  nan,   8.,   9.])

        >>> # "TM1_NA" (Y := Y[-1], if Y = NA)
        >>> reset_ACAF()
        >>> variables.extrapolate(SimulationInitialization.TM1_NA, "2005Y1", "2010Y1")
        >>> variables["ACAF", "2003Y1":"2009Y1"]
        array([ 3.,  4.,  4.,  6.,  6.,  8.,  9.])

        >>> # "EXTRA_NA" (Y := extrapolation, if Y = NA)
        >>> reset_ACAF()
        >>> variables.extrapolate(SimulationInitialization.EXTRA_NA, "2005Y1", "2010Y1")
        >>> variables["ACAF", "2003Y1":"2009Y1"]
        array([ 3.,  4.,  5.,  6.,  7.,  8.,  9.])
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

    def _str_header(self) -> str:
        s = super()._str_header() 
        s += f"sample: {self.sample}\n"
        s += f"mode: {self.mode}\n"
        return s

    def _str_table(self, names: List[str]) -> str:
        columns = {"name": names}
        vars_pos = [self.database_ptr.get_position(<string>(name.encode())) for name in names]
        for t, period in enumerate(self.periods):
            columns[period] = [self.database_ptr.get_var(var_pos, t, self.mode_) for var_pos in vars_pos]
        return table2str(columns, max_lines=10, max_width=100, precision=2, justify_funcs={"name": JUSTIFY.LEFT})

    def __hash__(self) -> int:
        """
        Return a hash value for the current Variables database.

        Examples
        --------
        >>> from iode import SAMPLE_DATA_DIR, variables
        >>> variables.load(f"{SAMPLE_DATA_DIR}/fun.var")
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
