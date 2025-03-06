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

KeyName = Union[str, List[str], slice]
KeyPeriod = Union[None, str, int, Tuple[str, str], Tuple[int, int], List[str], slice]
KeyVariable = Union[KeyName, Tuple[KeyName, KeyPeriod]]

class BinaryOperation(IntEnum):
    OP_ADD = CBinaryOperation.OP_ADD
    OP_SUB = CBinaryOperation.OP_SUB
    OP_MUL = CBinaryOperation.OP_MUL
    OP_DIV = CBinaryOperation.OP_DIV
    OP_POW = CBinaryOperation.OP_POW


def _check_same_periods(left_periods: List[str], right_periods: List[str], check_contiguous: bool=True, 
    right_hand_side_obj_type:str = None):
    left_periods_set = set(left_periods)
    right_periods_set = set(right_periods)
    missing_periods = left_periods_set - right_periods_set
    if len(missing_periods):
        missing_periods = sorted(list(missing_periods))
        raise KeyError(f"Missing value for the periods: '{', '.join(missing_periods)}'")
    extra_periods = right_periods_set - left_periods_set
    if len(extra_periods):
        extra_periods = sorted(list(extra_periods))
        raise KeyError(f"Unexpected periods in the right-hand side: '{', '.join(extra_periods)}'")
    if check_contiguous:
        # check if left-hand side 'periods' represents contiguous periods
        if len(left_periods) > 1:
            sample = Sample(left_periods[0], left_periods[-1])
            if left_periods != sample.periods:
                raise ValueError(f"Expected contiguous periods in the left-hand side.")
        # check if right-hand side 'periods' represents contiguous periods
        if len(right_periods) > 1:
            sample = Sample(right_periods[0], right_periods[-1])
            if right_periods != sample.periods:
                suffix = f" {right_hand_side_obj_type} object" if right_hand_side_obj_type else ""
                raise ValueError(f"Expected contiguous periods in the right-hand side{suffix}.")


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

    def _convert_period(self, t: int) -> Period:
        return self.database.sample.start.shift(t) if t is not None else None

    def __getitem__(self, index: Union[int, Tuple[int, int]]):
        pos, t = self._check_index(index)
        period = self._convert_period(t)
        return self.database._get_variable(pos, period)
    
    def __setitem__(self, index: Union[int, Tuple[int, int]], value):
        pos, t = self._check_index(index)
        period = self._convert_period(t)
        self.database._set_variable(pos, value, period)


@cython.final
cdef class Variables(IodeDatabase):
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
    cdef Period first_period_subset
    cdef Period last_period_subset

    def __cinit__(self, filepath: str = None) -> Variables:
        self.database_ptr = NULL
        self.abstract_db_ptr = NULL
        self.ptr_owner = False
        self.mode_ = IodeVarMode.VAR_MODE_LEVEL
        self.first_period_subset = None
        self.last_period_subset = None

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
            t_last_period: int = self.nb_periods - 1
        else:
            t_last_period: int = self._get_real_period_position(self.last_period_subset)

        return t_first_period, t_last_period

    # TODO: implement KDBAbstract::load() method (for global KDB only)
    def _load(self, filepath: str):
        cdef CKDBVariables* kdb = new CKDBVariables(filepath.encode())
        del kdb
        self.mode_ = IodeVarMode.VAR_MODE_LEVEL
        self.first_period_subset: Period = None
        self.last_period_subset: Period = None

    def _subset(self, pattern: str, copy: bool, first_period: Union[str, Period]=None, 
                last_period: Union[str, Period]=None) -> Variables:
        cdef CSample* c_sample
        cdef Variables subset_db = Variables.__new__(Variables)
        subset_db.database_ptr = subset_db.abstract_db_ptr = self.database_ptr.subset(pattern.encode(), <bint>copy)
        subset_db.ptr_owner = True
        subset_db.mode_ = IodeVarMode.VAR_MODE_LEVEL

        if isinstance(first_period, str):
            first_period = Period(first_period)
        
        if isinstance(last_period, str):
            last_period = Period(last_period)

        if first_period is None:
            first_period = self.first_period_subset
        
        if last_period is None:
            last_period = self.last_period_subset
        
        # check that first period subset < last period subset
        if first_period is not None and last_period is not None and first_period > last_period:
            raise ValueError(f"subset: first period of the subset ('{first_period}') must be " 
                             f"<= last period of the subset ('{last_period}')")

        # get the sample of the real database (not self.sample if self is already a subset)
        whole_db_sample: Sample = self._get_whole_sample()
        
        # check that first period of the subset is inside the real Variables sample 
        if first_period is not None and (first_period < whole_db_sample.start or first_period > whole_db_sample.end):
            raise ValueError(f"subset: first period of the subset '{first_period}' is not inside the Variables sample '{whole_db_sample}'")
        
        # check that last period of the subset is inside the real Variables sample 
        if last_period is not None and (last_period < whole_db_sample.start or last_period > whole_db_sample.end):
            raise ValueError(f"subset: last period of the subset '{last_period}' is not inside the Variables sample '{whole_db_sample}'")

        subset_db.first_period_subset = first_period
        subset_db.last_period_subset = last_period
        return subset_db

    def _unfold_key(self, key: KeyVariable) -> Tuple[KeyName, Optional[Period, Tuple[Period, Period], List[Period]]]:
        """
        split the key into a tuple (names, periods):

           - names: str or list of str
           - periods: None or Period or tuple of Period or list of Period

        Examples
        --------
        >>> from iode import Period, Sample, variables, SAMPLE_DATA_DIR
        >>> variables.load(f"{SAMPLE_DATA_DIR}/fun.var")        # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Loading .../fun.var
        394 objects loaded
        
        >>> # select all periods
        >>> variables._unfold_key("AC*")  # doctest: +NORMALIZE_WHITESPACE
        (['ACAF', 'ACAG'], None)
        
        >>> # cannot pass more than 2 arguments
        >>> variables._unfold_key(("AC*", '2000Y1:2010Y1', '2015Y1'))  # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Traceback (most recent call last):
           ...
        ValueError: variables[...]: Expected maximum 2 arguments ('names' and 'periods'). Got 3 arguments
        
        >>> # select 1 period
        >>> variables._unfold_key(("AC*", '2000Y1'))                # doctest: +NORMALIZE_WHITESPACE
        (['ACAF', 'ACAG'], Period("2000Y1"))
        >>> variables._unfold_key(("AC*", Period('2000Y1')))
        (['ACAF', 'ACAG'], Period("2000Y1"))

        >>> # select a range of periods (contiguous)
        >>> variables._unfold_key(("AC*", '2000Y1:2010Y1'))         # doctest: +NORMALIZE_WHITESPACE
        (['ACAF', 'ACAG'], (Period("2000Y1"), Period("2010Y1")))

        >>> # select a list of years
        >>> variables._unfold_key(("AC*", ['2000Y1', '2005Y1', '2010Y1']))  # doctest: +NORMALIZE_WHITESPACE
        (['ACAF', 'ACAG'], [Period("2000Y1"), Period("2005Y1"), Period("2010Y1")])

        >>> # select a range of contiguous periods using a Sample object
        >>> variables._unfold_key(("AC*", Sample('2000Y1:2010Y1')))  # doctest: +NORMALIZE_WHITESPACE
        (['ACAF', 'ACAG'], (Period("2000Y1"), Period("2010Y1")))

        >>> # select a range of contiguous periods using a slice
        >>> periods_range = slice('2000Y1', '2010Y1')               # doctest: +NORMALIZE_WHITESPACE
        >>> variables._unfold_key(("AC*", periods_range))           # doctest: +NORMALIZE_WHITESPACE
        (['ACAF', 'ACAG'], (Period("2000Y1"), Period("2010Y1")))
        >>> periods_range = slice(Period('2000Y1'), Period('2010Y1'))
        >>> variables._unfold_key(("AC*", periods_range))           # doctest: +NORMALIZE_WHITESPACE
        (['ACAF', 'ACAG'], (Period("2000Y1"), Period("2010Y1")))
        
        >>> # select a range of non-contiguous periods using a slice with a step
        >>> periods_range = slice('2006Y1', '2010Y1', 2)             # doctest: +NORMALIZE_WHITESPACE
        >>> variables._unfold_key(("AC*", periods_range))            # doctest: +NORMALIZE_WHITESPACE
        (['ACAF', 'ACAG'], [Period("2006Y1"), Period("2008Y1"), Period("2010Y1")])
        """
        cdef CSample* c_sample

        # no selection on periods
        if not isinstance(key, tuple):
            key = key, None
        
        if len(key) > 2:
            raise ValueError(f"variables[...]: Expected maximum 2 arguments ('names' and 'periods'). "
                                f"Got {len(key)} arguments")
        names, _periods = key

        # get selection on Variable name(s)
        names = super()._unfold_key(names) 
        
        # _periods represents the whole sample
        if _periods is None:
            pass
        # _periods represents a unique period
        elif isinstance(_periods, Period):
            pass
        # _periods represents a unique period or a contiguous range of periods
        elif isinstance(_periods, str): 
            # _periods represents a contiguous range of periods
            if ':' in _periods:
                first_period, last_period = _periods.split(':')
                first_period = Period(first_period) if first_period else None
                last_period = Period(last_period) if last_period else None
                _periods = first_period, last_period
            # _periods represents a unique period
            else:
                _periods = Period(_periods)
        
        # _periods is a list of periods
        elif isinstance(_periods, Iterable):
            if not all(isinstance(period, (str, Period)) for period in _periods):
                raise TypeError("variables[names, periods]: 'periods' must be a list of str or Period objects.")
            _periods = [Period(period) if isinstance(period, str) else period for period in _periods]
        
        # _periods is a Sample object
        elif isinstance(_periods, Sample):
            _periods = _periods.start, _periods.end

        # convert slice to a (start, end) tuple or a list of periods if step is not None
        elif isinstance(_periods, slice):
            sample = self.sample
            first_period = sample.start if _periods.start is None else _periods.start
            last_period = sample.end if _periods.stop is None else _periods.stop
            if _periods.step is not None:
                first_period, last_period = str(first_period), str(last_period)
                _periods = self.periods_subset(first_period, last_period)[::_periods.step]
                _periods = [Period(period) for period in _periods]
            else:
                if isinstance(first_period, str):
                    first_period = Period(first_period)
                if isinstance(last_period, str):
                    last_period = Period(last_period)
                _periods = first_period, last_period
        else:
            # wrong type for _periods
            raise TypeError(f"variables[names, periods]: 'periods' must be of type str, Period, Sample, "
                            f"list of str or Period, or a slice(str or Period, str or Period, int). "
                            f"'periods' is of type {type(_periods).__name__}.")

        return names, _periods

    # TODO: add tests for the case of a subset over the periods
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

    def _maybe_update_subset_sample(self):
        """
        Update the subset sample if the database sample has changed.

        Examples
        --------
        >>> from iode import variables, SAMPLE_DATA_DIR, Sample
        >>> variables.load(f"{SAMPLE_DATA_DIR}/fun.var")        # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Loading .../fun.var
        394 objects loaded
        >>> variables.sample
        Sample("1960Y1:2015Y1")
        >>> vars_subset = variables["A*", "2000Y1:2010Y1"]
        >>> vars_subset.sample
        Sample("2000Y1:2010Y1")
        >>> variables.sample = "1960Y1:2005Y1"
        >>> variables.sample
        Sample("1960Y1:2005Y1")
        >>> # automatic update of the subset sample
        >>> vars_subset.sample
        Sample("2000Y1:2005Y1")
        """ 
        if self.first_period_subset is None and self.last_period_subset is None:
            return 
    
        whole_db_sample: Sample = self._get_whole_sample()

        if self.first_period_subset is not None:
            if self.first_period_subset < whole_db_sample.start:
                self.first_period_subset = whole_db_sample.start
            if self.first_period_subset > whole_db_sample.end:
                self.first_period_subset = None
                self.last_period_subset = None

        if self.last_period_subset is not None:
            if self.last_period_subset > whole_db_sample.end:
                self.last_period_subset = whole_db_sample.end
            if self.last_period_subset < whole_db_sample.start:
                self.first_period_subset = None
                self.last_period_subset = None

    cdef int c_get_real_period_position(self, CPeriod c_period):
        """
        Get the position of a period in the Variables database sample (not the subset).
        """
        cdef CSample* c_sample
        if self.is_detached:
            c_sample = self.database_ptr.get_sample()
        else:
            c_sample = cpp_global_variables.get_sample()
        return c_sample.get_period_position(<CPeriod>c_period)

    def _get_real_period_position(self, period: Period):
        """
        Check if 'period' is inside the current (subset) sample.
        Get the position of a period in the Variables database sample (not the subset).
        """
        sample: Sample = self.sample
        if period < sample.start or period > sample.end:
            raise IndexError(f"The period '{period}' is outside the sample '{sample}'")
        return self.c_get_real_period_position(period.c_period)

    def _get_variable(self, key_name: Union[str, int], key_periods: Union[None, Period, List[Period]]) -> Union[float, pd.Series, Variables]: 
        """
        Get the value(s) of the variable `key_name` for the period(s) `key_periods`.

        Each period given is converted to its 'absolute' position in the Variables 
        database sample (not relative to the subset sample for instance).
        """
        cdef int t
        cdef int t_first
        cdef int t_last
        cdef int pos
        cdef double cpp_value
        cdef vector[double] cpp_values

        # key_periods represents all periods (of the current subset) -> return a Variables object
        if key_periods is None:
            if isinstance(key_name, int):
                key_name = self.get_name(key_name)
            db_subset = self._subset(key_name, copy=False)
            return db_subset
        # key_periods represents a unique period -> return a float 
        elif isinstance(key_periods, Period):
            pos = self.get_position(key_name) if isinstance(key_name, str) else key_name
            t = self._get_real_period_position(key_periods)
            cpp_value = self.database_ptr.get_var(pos, t, self.mode_)
            return cpp_value if IODE_IS_A_NUMBER(cpp_value) else np.nan
        # key_periods represents a contiguous range of periods -> return a Variables object
        elif isinstance(key_periods, tuple):
            first_period, last_period = key_periods
            if not isinstance(first_period, Period):
                raise TypeError(f"Expected value of type 'Period' for the first period. "
                                f"Got value of type {type(first_period).__name__} instead.")
            if not isinstance(last_period, Period):
                raise TypeError(f"Expected value of type 'Period' for the last period. "
                                f"Got value of type {type(last_period).__name__} instead.")
            if isinstance(key_name, int):
                key_name = self.get_name(key_name)
            db_subset = self._subset(key_name, copy=False, first_period=first_period, last_period=last_period)
            return db_subset
        # key_periods represents a list of non-contiguous periods -> return a pandas Series
        elif isinstance(key_periods, list):
            if not all(isinstance(period, Period) for period in key_periods):
                raise TypeError("Expected a list of periods each of type 'Period'")
            pos = self.get_position(key_name) if isinstance(key_name, str) else key_name
            period_names = [str(period) for period in key_periods]
            key_periods: List[int] = [self._get_real_period_position(period) for period in key_periods]
            values = [self.database_ptr.get_var(pos, <int>p, self.mode_) for p in key_periods]
            data = [value if IODE_IS_A_NUMBER(value) else np.nan for value in values]
            series = pd.Series(data, index=period_names)
            series.index.name = "time"
            series.name = key_name if isinstance(key_name, str) else self.get_name(key_name)
            return series
        else:
            raise TypeError("Wrong selection of periods.\nExpected None or value of type Period, "
                            f"tuple(Period, Period) or list(Period).\nGot value of type "
                            f"{type(key_periods).__name__} instead.")

    # overriden for Variables
    def __getitem__(self, key) -> Union[float, Variables]:
        r"""
        Return the (subset of) variable(s) referenced by `key`.

        The `key` represents a selection on the Variables names and optionally on the periods.

        The selection on names can be: 
        
            - a single Variable name (e.g. "ACAF") 
            - a list of Variable names (e.g. "ACAF;ACAG;AOUC") 
            - a pattern (e.g. "A*;*_").
        
        If the selection on names represents a list of names or of sub-patterns, each name or sub-pattern must be 
        separated by a `separator` character which is either a whitespace ` `, or a comma `,`, or a semi-colon `;`, 
        or a tabulation `\t`, or a newline `\n`.

        A (sub-)`pattern` is a list of characters representing a group of object names. 
        It includes some special characters which have a special meaning:
        
            - `*` : any character sequence, even empty
            - `?` : any character (one and only one)
            - `@` : any alphanumerical char [A-Za-z0-9]
            - `&` : any non alphanumerical char
            - `|` : any alphanumeric character or none at the beginning and end of a string 
            - `!` : any non-alphanumeric character or none at the beginning and end of a string 
            - `\` : escape the next character

        Note that the selection on names can contain references to IODE lists which are prefixed with the symbol `$`.

        The selection on periods can be:

           - the whole sample (e.g. None) 
           - a single period (e.g. "1990Y1") 
           - a range of periods (e.g. "1990Y1:2000Y1") 
           - a list of periods (e.g. ["1990Y1", "1995Y1", "2000Y1"]) 

        Parameters
        ----------
        key: str or list(str) or tuple(str, str) or tuple(str, list(str)) or tuple(str, str:str)
            The key is split into two parts: the selection on names and the selection on periods.
            The selection on names can be a single name, a list of names, or a pattern.
            The selection on periods (optional) can be a single period, a list of periods, or a range of periods.

        Returns
        -------
        float or Variables
            If `key` represents a single name and a single period, the returned value is a float.
            If `key` represents a list of non-contiguous periods, the returned object is a pandas DataFrame.
            Otherwise, a subset of the Variables workspace is returned.

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
        >>> variables["ACAF"]                       # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Workspace: Variables
        nb variables: 1
        filename: ...fun.var
        description: Modèle fun - Simulation 1
        sample: 1960Y1:2015Y1
        mode: LEVEL
        <BLANKLINE>
        name        1960Y1  1961Y1  ...  2014Y1   2015Y1
        ACAF            na      na  ...  -83.34   -96.41
        <BLANKLINE>
        >>> # get the variable value for a specific period
        >>> variables["ACAF", "1990Y1"]
        23.771
        >>> # get the variable values for range of periods (using a Python slice)
        >>> variables["ACAF", "1990Y1":"2000Y1"]    # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Workspace: Variables
        nb variables: 1
        filename: ...fun.var
        description: Modèle fun - Simulation 1
        sample: 1990Y1:2000Y1
        mode: LEVEL
        <BLANKLINE>
        name        1990Y1  1991Y1  1992Y1  1993Y1  1994Y1  1995Y1  1996Y1  1997Y1  1998Y1  1999Y1  2000Y1
        ACAF         23.77   26.24   30.16   34.66    8.16  -13.13   32.17   39.94   29.65   13.53   10.05
        <BLANKLINE>
        >>> # same as above but with the colon ':' inside the periods range string
        >>> variables["ACAF", "1990Y1:2000Y1"]      # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Workspace: Variables
        nb variables: 1
        filename: ...fun.var
        description: Modèle fun - Simulation 1
        sample: 1990Y1:2000Y1
        mode: LEVEL
        <BLANKLINE>
        name        1990Y1  1991Y1  1992Y1  1993Y1  1994Y1  1995Y1  1996Y1  1997Y1  1998Y1  1999Y1  2000Y1
        ACAF         23.77   26.24   30.16   34.66    8.16  -13.13   32.17   39.94   29.65   13.53   10.05
        <BLANKLINE>

        >>> # b) -------- get the values for a single period --------
        >>> variables[:, "1990Y1"]                  # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Workspace: Variables
        nb variables: 394
        filename: ...fun.var
        description: Modèle fun - Simulation 1
        sample: 1990Y1:1990Y1
        mode: LEVEL
        <BLANKLINE>
         name       1990Y1
        ACAF         23.77
        ACAG        -28.17
        AOUC          1.00
        ...            ...
        ZKFO          1.00
        ZX            0.00
        ZZF_          0.69
        <BLANKLINE> 
        >>> # or equivalently:
        >>> variables["*", "1990Y1"]                # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Workspace: Variables
        nb variables: 394
        filename: ...fun.var
        description: Modèle fun - Simulation 1
        sample: 1990Y1:1990Y1
        mode: LEVEL
        <BLANKLINE>
         name       1990Y1
        ACAF         23.77
        ACAG        -28.17
        AOUC          1.00
        AOUC_         0.94
        AQC           1.00
        ...            ...
        ZJ            1.09
        ZKF           0.81
        ZKFO          1.00
        ZX            0.00
        ZZF_          0.69
        <BLANKLINE>

        >>> # c) -------- get a subset of the Variables database using a pattern --------
        >>> vars_subset = variables["A*"]
        >>> vars_subset.names
        ['ACAF', 'ACAG', 'AOUC', 'AOUC_', 'AQC']
        >>> # get the variable values for a specific period -> Worspace subset
        >>> variables["A*", "1990Y1"]       # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Workspace: Variables
        nb variables: 5
        filename: ...fun.var
        description: Modèle fun - Simulation 1
        sample: 1990Y1:1990Y1
        mode: LEVEL
        <BLANKLINE>
        name       1990Y1
        ACAF         23.77
        ACAG        -28.17
        AOUC          1.00
        AOUC_         0.94
        AQC           1.00
        <BLANKLINE>
        >>> # get the variable values for range of periods -> Workspace subset
        >>> variables["A*", "1990Y1:2000Y1"]      # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE 
        Workspace: Variables
        nb variables: 5
        filename: ...fun.var
        description: Modèle fun - Simulation 1
        sample: 1990Y1:2000Y1
        mode: LEVEL
        <BLANKLINE>
         name       1990Y1  1991Y1  1992Y1  1993Y1  1994Y1  1995Y1  1996Y1  1997Y1  1998Y1  1999Y1  2000Y1
        ACAF         23.77   26.24   30.16   34.66    8.16  -13.13   32.17   39.94   29.65   13.53   10.05
        ACAG        -28.17  -30.93  -40.29  -43.16  -16.03  -41.85  -40.24  -32.93  -38.35  -39.86  -41.53
        AOUC          1.00    1.02    1.03    1.03    1.05    1.05    1.05    1.08    1.09    1.11    1.12
        AOUC_         0.94    0.96    0.97    0.98    0.99    1.00    1.00    1.03    1.04    1.08    1.10
        AQC           1.00    1.06    1.11    1.15    1.16    1.16    1.16    1.20    1.20    1.34    1.34
        <BLANKLINE>

        >>> # d) -------- get a subset of the Variables database using a list of names --------
        >>> vars_subset = variables[["ACAF", "ACAG", "AQC", "BQY", "BVY"]]
        >>> vars_subset.names
        ['ACAF', 'ACAG', 'AQC', 'BQY', 'BVY']
        >>> # get the variable values for a specific period -> pandas Series
        >>> variables[["ACAF", "ACAG", "AQC", "BQY", "BVY"], "1990Y1"]              # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Workspace: Variables
        nb variables: 5
        filename: ...fun.var
        description: Modèle fun - Simulation 1
        sample: 1990Y1:1990Y1
        mode: LEVEL
        <BLANKLINE>
        name        1990Y1
        ACAF         23.77
        ACAG        -28.17
        AQC           1.00
        BQY         -34.10
        BVY         -34.10
        <BLANKLINE>
        >>> # get the variable values for a list of non-contiguous periods -> pandas DataFrame
        >>> variables[["ACAF", "ACAG", "AQC", "BQY", "BVY"], ["1990Y1", "1995Y1", "2000Y1"]]  # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        time          1990Y1      1995Y1      2000Y1
        variables
        ACAF       23.771000  -13.130997   10.046611
        ACAG      -28.172186  -41.845993  -41.534787
        AQC         1.000000    1.161687    1.338603
        BQY       -34.099998   91.089355  119.955090
        BVY       -34.099997  105.500000  150.053352       
        >>> # get the variable values for range of periods -> Workspace subset
        >>> variables[["ACAF", "ACAG", "AQC", "BQY", "BVY"], "1990Y1:2000Y1"]      # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE 
        Workspace: Variables
        nb variables: 5
        filename: ...fun.var
        description: Modèle fun - Simulation 1
        sample: 1990Y1:2000Y1
        mode: LEVEL
        <BLANKLINE>
        name        1990Y1  1991Y1  1992Y1  1993Y1  1994Y1  1995Y1  1996Y1  1997Y1  1998Y1  1999Y1  2000Y1
        ACAF         23.77   26.24   30.16   34.66    8.16  -13.13   32.17   39.94   29.65   13.53   10.05
        ACAG        -28.17  -30.93  -40.29  -43.16  -16.03  -41.85  -40.24  -32.93  -38.35  -39.86  -41.53
        AQC           1.00    1.06    1.11    1.15    1.16    1.16    1.16    1.20    1.20    1.34    1.34
        BQY         -34.10   -1.26  -13.75   52.16   66.63   91.09  104.68  113.52  116.19  117.91  119.96
        BVY         -34.10   -1.30  -14.70   58.10   75.90  105.50  123.20  135.62  140.74  144.86  150.05
        <BLANKLINE>
        """
        names, key_periods = self._unfold_key(key)

        # names represents a single IODE Variable
        if len(names) == 1:
            return self._get_variable(names[0], key_periods)
        # names represents a selection of IODE Variables
        else:
            # periods = list of non-contiguous periods -> return a pandas DataFrame
            if isinstance(key_periods, list):
                data: List[pd.Series] = [self._get_variable(name, key_periods) for name in names]
                columns = [str(period) for period in key_periods]
                df = pd.concat(data, axis='columns').transpose()
                df.index.name = "variables"
                return df
            # periods = single period or range of periods or the whole sample
            # -> return a Variables object (Workspace subset)
            else:
                pattern = ';'.join(names)
                if isinstance(key_periods, Period):
                    first_period = last_period = key_periods
                elif isinstance(key_periods, tuple):
                    first_period, last_period = key_periods
                else:
                    first_period = last_period = None
                db_subset = self._subset(pattern, copy=False, first_period=first_period, 
                                         last_period=last_period)
                return db_subset

    def _expand_key_periods(self, key_periods: Union[str, Period, List[str], Tuple[str, str], Sample]) -> List[str]:
        if key_periods is None:
            return self.periods
        
        if isinstance(key_periods, (str, Period)):
            key_periods = [key_periods]
        
        if isinstance(key_periods, list):
            key_periods = [str(p) for p in key_periods]
        elif isinstance(key_periods, tuple):
            key_periods = Sample(key_periods[0], key_periods[1]).periods 
        elif isinstance(key_periods, Sample):
            key_periods = key_periods.periods
        else:
            raise TypeError(f"Expected periods to be of type str, Period, list, tuple or Sample.\n"
                            f"Got periods of type {type(key_periods).__name__} instead")

        return key_periods

    def _convert_values(self, values: Union[int, float, str, Iterable[Union[int, float]], \
        Dict[str, Any], np.ndarray, pd.Series, pd.DataFrame, Array, Variables]) \
        -> Union[str, float, List[float], Variables]:
        """
        Check the type of 'values' and convert np.nan to IODE NA (if needed).
        """
        # value is a LEC expression
        if isinstance(values, str):
            return values
        # same value for all periods to be set
        elif isinstance(values, (int, float)):
            return NA if np.isnan(values) else float(values)
        # Variables object
        elif isinstance(values, Variables):
            return values
        # dictionary
        elif isinstance(values, dict):
            return {name: self._convert_values(value) for name, value in values.items()}
        # numpy array
        elif isinstance(values, np.ndarray):
            if values.dtype != np.float64:
                values = values.astype(np.float64)
            values = np.nan_to_num(values, nan=NA)
            return values
        # pandas Series or DataFrame
        elif isinstance(values, (pd.Series, pd.DataFrame)):
            values = values.astype(np.float64)
            return values.fillna(NA)
        # larray Array
        elif la is not None and isinstance(values, Array):
            values = values.astype(np.float64)
            values.data = np.nan_to_num(values.data, nan=NA)
            return values
        # list of float
        elif isinstance(values, Iterable) and all(isinstance(value, (int, float)) for value in values):
            return [NA if np.isnan(value) else float(value) for value in values]
        # wrong type for 'value'
        else:
            raise TypeError("variables[key] = value: Expected 'value' of type int, float, "
                            "str, list of int/float, numpy ndarray, pandas Series, "
                            f"pandas DataFrame or Variables.\nGot 'value(s)' of type {type(values).__name__}") 

    # NOTE: needed to create a dedicated method since Cython seems to have some 
    #       difficulties with Union[..., Variables].
    #       Without a dedicated method, the following error is raised:
    #       ""Storing unsafe C derivative of temporary Python reference"
    def __add_var(self, name: str, value: Variables):
        cdef string c_name = name.encode()
        t_first, t_last = value._get_periods_bounds()
        # NOTE: 'value' can contains more than one variable as long as the variable named 'name' is present
        _c_add_var_from_other(c_name, self.database_ptr, value.database_ptr, t_first, t_last)

    def _add(self, name: str, values: Union[str, int, float, np.ndarray, Iterable[float], Variables]):
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

    def _set_variable(self, key_name: Union[str, int], values: Union[str, int, float, np.ndarray, Iterable[float], Variables], 
        key_periods: Optional[str, Period, Tuple[Any], List[Any]]):
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
            self._add(key_name, values)
        
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
                elif isinstance(values, Variables):
                    self.__copy_var(name, name, t, t, values)
                else:
                    raise TypeError(f"Cannot update the IODE variable '{name}'.\n"
                                    f"When updating values for a single period, the right-hand side must be "
                                    f"a float or an interable of float.\nGot input of type {type(values).__name__} instead")
            # update values for a contiguous range of periods
            elif isinstance(key_periods, tuple):
                first_period, last_period = key_periods
                t_first = self._get_real_period_position(first_period)
                t_last = self._get_real_period_position(last_period)
                # values is a LEC expression
                if isinstance(values, str):
                    self.database_ptr.update(pos, <string>values.encode(), t_first, t_last) 
                # set same value for all periods in the range
                elif isinstance(values, float):
                    for t in range(t_first, t_last + 1): 
                        KV_set(c_db_ptr, pos, t, self.mode_, <double>values)  
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
                # values is a list of float containing a specific value for each period
                elif isinstance(values, list):
                    if len(values) != len(key_periods):
                        raise ValueError(f"Cannot update the IODE variable '{name}'.\n"
                                         f"Expected a list of {len(key_periods)} values.\n"
                                         f"Got {len(values)} values instead")   
                else:
                    raise TypeError(f"Cannot update the IODE variable '{name}'.\n"
                                    f"When updating values for non-contiguous periods, the right-hand side must be "
                                    f"a float or a list of float.\nGot input of type {type(values).__name__} instead")
                for p, v in zip(key_periods, values):
                    t = self._get_real_period_position(p)
                    KV_set(c_db_ptr, pos, t, self.mode_, <double>v)

    def _check_pandas_series(self, value: pd.Series, key_names: List[str], key_periods: List[str]) -> pd.Series:
        if isinstance(value.index, MultiIndex):
            raise ValueError(f"Expected pandas Series with a single-level index.\n") 
        if len(key_names) > 1:
            if len(key_periods) > 1:
                raise ValueError("Cannot set or update the value of several variables with a pandas Series "
                                "when the selection key represents more than one period.")
            # check that names in the selection key are present in the Series object
            series_names = value.index.to_list()
            self._check_same_names(key_names, series_names)
        else:
            # check that periods in the selection key are present in the Series object
            series_periods = value.index.to_list()
            _check_same_periods(key_periods, series_periods, True, "pandas Series")
        return value

    def _check_pandas_dataframe(self, value: pd.DataFrame, key_names: List[str], key_periods: List[str]) \
        -> Union[pd.Series, pd.DataFrame]:
        if isinstance(value.index, MultiIndex):
            raise ValueError(f"Expected pandas DataFrame with a single-level index.\n")
        # check that periods in the selection key are present in the DataFrame object
        df_periods = value.columns.to_list()
        _check_same_periods(key_periods, df_periods, True, "pandas DataFrame")
        
        df_names = value.index.to_list()
        if len(key_names) == 1:
            if len(df_names) > 1:
                raise ValueError(f"Expected DataFrame with a single index.\n")
            # transform the DataFrame to a Series
            value = value.squeeze()
        else:
            # check that names in the selection key are present in the DataFrame object
            self._check_same_names(key_names, df_names)   
        return value

    def _check_larray_array(self, value: Array, key_names: List[str], key_periods: List[str]) -> Array:
        # Raise an error if no 'time' axis is present in the passed array.
        if 'time' not in value.axes:
            raise ValueError(f"Passed Array object must contain an axis named 'time'.\n"
                                f"Got axes {repr(value.axes)}.")
        time = value.axes['time']
        # push the time axis as last axis and combine all other axes 
        value = value.transpose(..., time)
        if value.ndim > 2:
            value = value.combine_axes(value.axes[:-1], sep='_')
        
        # check that periods in the selection key are present in the Array object
        array_periods = list(time.labels)
        _check_same_periods(key_periods, array_periods, True, "Array")
        
        if len(key_names) == 1:
            if value.ndim == 2:
                if value.shape[0] > 1:
                    raise ValueError(f"Expected Array object to represent a single variable.\n")
                value = value.i[0, :]
        else:   
            # check that names in the selection key are present in the Array object
            array_names = list(value.axes[0].labels)
            self._check_same_names(key_names, array_names)
        return value

    # overriden for Variables
    def __setitem__(self, key, value):
        r"""
        Update/add a (subset of) variable(s) referenced by `key` from/to the Variables database.

        The `key` represents a selection on the Variables names and optionally on the periods.

        The selection on names can be: 
        
            - a single Variable name (e.g. "ACAF") 
            - a list of Variable names (e.g. "ACAF;ACAG;AOUC") 
            - a pattern (e.g. "A*;*_").
        
        If the selection on names represents a list of names or of sub-patterns, each name or sub-pattern must be 
        separated by a `separator` character which is either a whitespace ` `, or a comma `,`, or a semi-colon `;`, 
        or a tabulation `\t`, or a newline `\n`.

        A (sub-)`pattern` is a list of characters representing a group of object names. 
        It includes some special characters which have a special meaning:
        
            - `*` : any character sequence, even empty
            - `?` : any character (one and only one)
            - `@` : any alphanumerical char [A-Za-z0-9]
            - `&` : any non alphanumerical char
            - `|` : any alphanumeric character or none at the beginning and end of a string 
            - `!` : any non-alphanumeric character or none at the beginning and end of a string 
            - `\` : escape the next character

        Note that the selection on names can contain references to IODE lists which are prefixed with the symbol `$`.

        The selection on periods can be:

           - the whole sample (e.g. None) 
           - a single period (e.g. "1990Y1") 
           - a range of periods (e.g. "1990Y1:2000Y1") 
           - a list of periods (e.g. ["1990Y1", "1995Y1", "2000Y1"]) 

        Parameters
        ----------
        key: str or list(str) or tuple(str, str) or tuple(str, list(str)) or tuple(str, str:str)
            The key is split into two parts: the selection on names and the selection on periods.
            The selection on names can be a single name, a list of names, or a pattern.
            The selection on periods (optional) can be a single period, a list of periods, or a range of periods.

        value: str or int or float or dict(str, ...) or numpy array or pandas Series or pandas DataFrame or Variables
            If str, the value is interpreted as a LEC expression and is evaluated for each period.
            If int, the value is first converted to a float and then used for all periods.
            If float, the value is used for all periods.
            If dict, the keys represents the names of the variables to be modified. 
            If numpy array or pandas Series/DataFrame, there must be a value for each variable and period to be set.
            If Variables, names and periods must match.

        Examples
        --------
        >>> import numpy as np
        >>> import pandas as pd
        >>> import larray as la
        >>> from iode import SAMPLE_DATA_DIR
        >>> from iode import variables, NA, Sample
        >>> variables.load(f"{SAMPLE_DATA_DIR}/fun.var")        # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Loading .../fun.var
        394 objects loaded
        
        >>> # a) -------- add one variable --------
        >>> # 1) same value for all periods
        >>> variables["A0"] = np.nan
        >>> variables["A0"]                     # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE 
        Workspace: Variables
        nb variables: 1
        filename: ...fun.var
        description: Modèle fun - Simulation 1
        sample: 1960Y1:2015Y1
        mode: LEVEL
        <BLANKLINE>
        name        1960Y1  1961Y1  ...  2014Y1   2015Y1
        A0              na      na  ...      na       na
        <BLANKLINE>
        >>> # or equivalently
        >>> variables["A0"] = NA
        >>> variables["A0"]                     # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Workspace: Variables
        nb variables: 1
        filename: ...fun.var
        description: Modèle fun - Simulation 1
        sample: 1960Y1:2015Y1
        mode: LEVEL
        <BLANKLINE>
        name        1960Y1  1961Y1  ...  2014Y1   2015Y1
        A0              na      na  ...      na       na
        <BLANKLINE>
        >>> # 2) LEC expression
        >>> variables["A1"] = "t + 10"
        >>> variables["A1"]                     # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Workspace: Variables
        nb variables: 1
        filename: ...fun.var
        description: Modèle fun - Simulation 1
        sample: 1960Y1:2015Y1
        mode: LEVEL
        <BLANKLINE>
        name        1960Y1  1961Y1  ...  2014Y1   2015Y1
        A1           10.00   11.00  ...   64.00    65.00
        <BLANKLINE>
        >>> # 3) list of values for each period
        >>> values = list(range(variables.nb_periods))
        >>> values[0] = NA
        >>> values[-1] = np.nan
        >>> variables["A2"] = values
        >>> variables["A2"]                     # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Workspace: Variables
        nb variables: 1
        filename: ...fun.var
        description: Modèle fun - Simulation 1
        sample: 1960Y1:2015Y1
        mode: LEVEL
        <BLANKLINE>
        name        1960Y1  1961Y1  1962Y1  ...  2013Y1  2014Y1  2015Y1
        A2              na    1.00    2.00  ...   53.00   54.00      na
        <BLANKLINE>
        >>> # 4) numpy ndarray
        >>> values = np.asarray(values)
        >>> variables["A3"] = values
        >>> variables["A3"]                     # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Workspace: Variables
        nb variables: 1
        filename: ...fun.var
        description: Modèle fun - Simulation 1
        sample: 1960Y1:2015Y1
        mode: LEVEL
        <BLANKLINE>
        name        1960Y1  1961Y1  1962Y1  ...  2013Y1  2014Y1  2015Y1
        A3              na    1.00    2.00  ...   53.00   54.00      na
        <BLANKLINE>
        >>> # 5) pandas Series
        >>> values = pd.Series(values, index=variables.periods)
        >>> variables["A4"] = values
        >>> variables["A4"]                     # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Workspace: Variables
        nb variables: 1
        filename: ...fun.var
        description: Modèle fun - Simulation 1
        sample: 1960Y1:2015Y1
        mode: LEVEL
        <BLANKLINE>
        name        1960Y1  1961Y1  1962Y1  ...  2013Y1  2014Y1  2015Y1
        A4              na    1.00    2.00  ...   53.00   54.00      na
        <BLANKLINE>
        >>> # 6) Variables object
        >>> variables["A5"] = variables["ACAF"]
        >>> variables["A5"]                     # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Workspace: Variables
        nb variables: 1
        filename: ...fun.var
        description: Modèle fun - Simulation 1
        sample: 1960Y1:2015Y1
        mode: LEVEL
        <BLANKLINE>
        name        1960Y1  1961Y1  ...  2013Y1  2014Y1  2015Y1
        A5              na      na  ...  -68.89  -83.34  -96.41
        <BLANKLINE> 

        >>> # b) -------- update one variable --------
        >>> # 1) set one value of a variable for a specific period
        >>> variables["ACAG", "1990Y1"]
        -28.1721855713507
        >>> variables["ACAG", "1990Y1"] = -28.2
        >>> variables["ACAG", "1990Y1"]
        -28.2

        >>> # 2) update all values of a variable
        >>> variables["ACAF"]                   # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Workspace: Variables
        nb variables: 1
        filename: ...fun.var
        description: Modèle fun - Simulation 1
        sample: 1960Y1:2015Y1
        mode: LEVEL
        <BLANKLINE>
        name        1960Y1  1961Y1  ...  2014Y1   2015Y1
        ACAF            na      na  ...  -83.34   -96.41
        <BLANKLINE>
        >>> # 2.1) same value for all periods
        >>> variables["ACAF"] = np.nan
        >>> variables["ACAF"]                   # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Workspace: Variables
        nb variables: 1
        filename: ...fun.var
        description: Modèle fun - Simulation 1
        sample: 1960Y1:2015Y1
        mode: LEVEL
        <BLANKLINE>
        name        1960Y1  1961Y1  ...  2014Y1   2015Y1
        ACAF            na      na  ...      na       na
        <BLANKLINE>
        >>> # or equivalently
        >>> variables["ACAF"] = NA
        >>> variables["ACAF"]                   # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Workspace: Variables
        nb variables: 1
        filename: ...fun.var
        description: Modèle fun - Simulation 1
        sample: 1960Y1:2015Y1
        mode: LEVEL
        <BLANKLINE>
        name        1960Y1  1961Y1  ...  2014Y1   2015Y1
        ACAF            na      na  ...      na       na
        <BLANKLINE>
        >>> # 2.2) LEC expression
        >>> variables["ACAF"] = "t + 10"
        >>> variables["ACAF"]                   # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Workspace: Variables
        nb variables: 1
        filename: ...fun.var
        description: Modèle fun - Simulation 1
        sample: 1960Y1:2015Y1
        mode: LEVEL
        <BLANKLINE>
        name        1960Y1  1961Y1  ...  2014Y1   2015Y1
        ACAF         10.00   11.00  ...   64.00    65.00
        <BLANKLINE>
        >>> # 2.3) list of values for each period
        >>> values = list(range(variables.nb_periods))
        >>> values[0] = NA
        >>> values[-1] = np.nan
        >>> variables["ACAF"] = values
        >>> variables["ACAF"]                   # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Workspace: Variables
        nb variables: 1
        filename: ...fun.var
        description: Modèle fun - Simulation 1
        sample: 1960Y1:2015Y1
        mode: LEVEL
        <BLANKLINE>
        name        1960Y1  1961Y1  1962Y1  ...  2013Y1  2014Y1  2015Y1
        ACAF            na    1.00    2.00  ...   53.00   54.00      na
        <BLANKLINE>
        >>> # 2.4) numpy array
        >>> values = np.asarray(values)
        >>> variables["ACAG"] = values
        >>> variables["ACAG"]                   # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Workspace: Variables
        nb variables: 1
        filename: ...fun.var
        description: Modèle fun - Simulation 1
        sample: 1960Y1:2015Y1
        mode: LEVEL
        <BLANKLINE>
        name        1960Y1  1961Y1  1962Y1  ...  2013Y1  2014Y1  2015Y1
        ACAG            na    1.00    2.00  ...   53.00   54.00      na
        <BLANKLINE>
        >>> # 2.5) pandas Series
        >>> values = pd.Series(values, index=variables.periods)
        >>> variables["AOUC"] = values
        >>> variables["AOUC"]                   # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Workspace: Variables
        nb variables: 1
        filename: ...fun.var
        description: Modèle fun - Simulation 1
        sample: 1960Y1:2015Y1
        mode: LEVEL
        <BLANKLINE>
        name        1960Y1  1961Y1  1962Y1  ...  2013Y1  2014Y1  2015Y1
        AOUC            na    1.00    2.00  ...   53.00   54.00      na
        <BLANKLINE>
        >>> # 2.6) Variables object
        >>> variables["AQC"]                    # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Workspace: Variables
        nb variables: 1
        filename: ...fun.var
        description: Modèle fun - Simulation 1
        sample: 1960Y1:2015Y1
        mode: LEVEL
        <BLANKLINE>
        name        1960Y1  1961Y1  1962Y1  ...  2013Y1  2014Y1  2015Y1
        AQC           0.22    0.22    0.22  ...    1.56    1.61    1.67
        <BLANKLINE>
        >>> variables["ACAF"] = variables["AQC"]
        >>> variables["ACAF"]                   # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Workspace: Variables
        nb variables: 1
        filename: ...fun.var
        description: Modèle fun - Simulation 1
        sample: 1960Y1:2015Y1
        mode: LEVEL
        <BLANKLINE>
        name        1960Y1  1961Y1  1962Y1  ...  2013Y1  2014Y1  2015Y1
        ACAF          0.22    0.22    0.22  ...    1.56    1.61    1.67
        <BLANKLINE>

        >>> # 3) set the values for range of (contiguous) periods 
        >>> # 3.1) variable[t:t+x] = same value for all periods
        >>> variables["ACAF", "1991Y1:1995Y1"] = 0.0
        >>> variables["ACAF", "1991Y1:1995Y1"]            # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE        
        Workspace: Variables
        nb variables: 1
        filename: ...fun.var
        description: Modèle fun - Simulation 1
        sample: 1991Y1:1995Y1
        mode: LEVEL
        <BLANKLINE>
        name        1991Y1  1992Y1  1993Y1  1994Y1  1995Y1
        ACAF          0.00    0.00    0.00    0.00    0.00
        <BLANKLINE>
        >>> # 3.2) variable[t:t+x] = LEC expression
        >>> variables["ACAF", "1991Y1:1995Y1"] = "t + 10"
        >>> variables["ACAF", "1991Y1:1995Y1"]        # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Workspace: Variables
        nb variables: 1
        filename: ...fun.var
        description: Modèle fun - Simulation 1
        sample: 1991Y1:1995Y1
        mode: LEVEL
        <BLANKLINE>
        name        1991Y1  1992Y1  1993Y1  1994Y1  1995Y1
        ACAF         41.00   42.00   43.00   44.00   45.00
        <BLANKLINE>
        >>> # 3.3) variable[t:t+x] = list of values for each period
        >>> values = [1.0, NA, 3.0, np.nan, 5.0]
        >>> variables["ACAF", "1991Y1:1995Y1"] = values
        >>> variables["ACAF", "1991Y1:1995Y1"]          # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Workspace: Variables
        nb variables: 1
        filename: ...fun.var
        description: Modèle fun - Simulation 1
        sample: 1991Y1:1995Y1
        mode: LEVEL
        <BLANKLINE>
        name        1991Y1  1992Y1  1993Y1  1994Y1  1995Y1
        ACAF          1.00      na    3.00      na    5.00
        <BLANKLINE>
        >>> # 3.4) variable[t:t+x] = numpy array
        >>> values = np.asarray(values)
        >>> variables["ACAG", "1991Y1:1995Y1"] = values
        >>> variables["ACAG", "1991Y1:1995Y1"]          # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Workspace: Variables
        nb variables: 1
        filename: ...fun.var
        description: Modèle fun - Simulation 1
        sample: 1991Y1:1995Y1
        mode: LEVEL
        <BLANKLINE>
        name        1991Y1  1992Y1  1993Y1  1994Y1  1995Y1
        ACAG          1.00      na    3.00      na    5.00
        <BLANKLINE>
        >>> # 3.5) variable[t:t+x] = pandas Series
        >>> periods = Sample("1991Y1:1995Y1").periods
        >>> variables["AOUC", "1991Y1:1995Y1"] = pd.Series(values, index=periods)
        >>> variables["AOUC", "1991Y1:1995Y1"]          # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Workspace: Variables
        nb variables: 1
        filename: ...fun.var
        description: Modèle fun - Simulation 1
        sample: 1991Y1:1995Y1
        mode: LEVEL
        <BLANKLINE>
        name        1991Y1  1992Y1  1993Y1  1994Y1  1995Y1
        AOUC          1.00      na    3.00      na    5.00
        <BLANKLINE>
        >>> # 3.6) variable[t:t+x] = Variables object
        >>> variables["AQC", "1991Y1:1995Y1"]           # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Workspace: Variables
        nb variables: 1
        filename: ...fun.var
        description: Modèle fun - Simulation 1
        sample: 1991Y1:1995Y1
        mode: LEVEL
        <BLANKLINE>
        name        1991Y1  1992Y1  1993Y1  1994Y1  1995Y1
        AQC           1.06    1.11    1.15    1.16    1.16
        <BLANKLINE>
        >>> variables["ACAF", "1991Y1:1995Y1"] = variables["AQC", "1991Y1:1995Y1"]
        >>> variables["ACAF", "1991Y1:1995Y1"]        # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Workspace: Variables
        nb variables: 1
        filename: ...fun.var
        description: Modèle fun - Simulation 1
        sample: 1991Y1:1995Y1
        mode: LEVEL
        <BLANKLINE>
        name        1991Y1  1992Y1  1993Y1  1994Y1  1995Y1
        ACAF          1.06    1.11    1.15    1.16    1.16
        <BLANKLINE>
        
        >>> # 4) set the values for a list of non-contiguous periods
        >>> values = [1.0, 3.0, 5.0]
        >>> variables["ACAF", ["1991Y1", "1993Y1", "1995Y1"]] = values
        >>> variables["ACAF", ["1991Y1", "1993Y1", "1995Y1"]]        # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        time
        1991Y1    1.0
        1993Y1    3.0
        1995Y1    5.0
        Name: ACAF, dtype: float64

        >>> # c) -------- update several variables at once --------
        >>> # 1) using a string
        >>> variables["ACAF, ACAG, AOUC", "1991Y1:1995Y1"] = "t + 1"
        >>> variables["ACAF, ACAG, AOUC", "1991Y1:1995Y1"]          # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Workspace: Variables
        nb variables: 3
        filename: ...fun.var
        description: Modèle fun - Simulation 1                         
        sample: 1991Y1:1995Y1
        mode: LEVEL
        <BLANKLINE>
        name	1991Y1	1992Y1	1993Y1	1994Y1	1995Y1
        ACAF	 32.00	 33.00	 34.00	 35.00	 36.00
        ACAG	 32.00	 33.00	 34.00	 35.00	 36.00
        AOUC	 32.00	 33.00	 34.00	 35.00	 36.00
        <BLANKLINE>
        >>> # 2) using a dict of values
        >>> periods = ["1991Y1", "1992Y1", "1993Y1", "1994Y1", "1995Y1"]
        >>> values = {"ACAF": "ACAF * 1.05", 
        ...           "ACAG": [np.nan, -39.96, -42.88, -16.33, -41.16], 
        ...           "AOUC": pd.Series([1.023, np.nan, 1.046, np.nan, 1.064], index=periods)}
        >>> variables["ACAF, ACAG, AOUC", "1991Y1:1995Y1"] = values
        >>> variables["ACAF, ACAG, AOUC", "1991Y1:1995Y1"]          # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Workspace: Variables
        nb variables: 3
        filename: ...fun.var
        description: Modèle fun - Simulation 1                         
        sample: 1991Y1:1995Y1
        mode: LEVEL
        <BLANKLINE>
        name	1991Y1	1992Y1	1993Y1	1994Y1	1995Y1
        ACAF	 33.60	 34.65	 35.70	 36.75	 37.80
        ACAG	    na	-39.96	-42.88	-16.33	-41.16
        AOUC	  1.02	    na	  1.05	    na	  1.06
        <BLANKLINE>
        >>> # 3) using a numpy array
        >>> data = [[28.89, 31.90, 36.66, 42.13, 9.92],
        ...         [np.nan, -39.96, -42.88, -16.33, -41.16], 
        ...         [1.023, np.nan, 1.046, np.nan, 1.064]]
        >>> data = np.asarray(data)
        >>> data                                                    # doctest: +NORMALIZE_WHITESPACE
        array([[ 28.89 ,  31.9  ,  36.66 ,  42.13 ,   9.92 ],
               [    nan, -39.96 , -42.88 , -16.33 , -41.16 ],
               [  1.023,     nan,   1.046,     nan,   1.064]])
        >>> variables["ACAF, ACAG, AOUC", "1991Y1:1995Y1"] = data
        >>> variables["ACAF, ACAG, AOUC", "1991Y1:1995Y1"]          # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Workspace: Variables
        nb variables: 3
        filename: ...fun.var
        description: Modèle fun - Simulation 1                         
        sample: 1991Y1:1995Y1
        mode: LEVEL
        <BLANKLINE>
        name	1991Y1	1992Y1	1993Y1	1994Y1	1995Y1
        ACAF	 28.89	 31.90	 36.66	 42.13	  9.92
        ACAG	    na	-39.96	-42.88	-16.33	-41.16
        AOUC	  1.02	    na	  1.05	    na	  1.06
        <BLANKLINE>
        >>> # 4) using a pandas DataFrame
        >>> data += 2.0
        >>> df = pd.DataFrame(data, index=["ACAF", "ACAG", "AOUC"], columns=periods)
        >>> df                                                      # doctest: +NORMALIZE_WHITESPACE
        1991Y1	1992Y1	1993Y1	1994Y1	1995Y1
        ACAF	30.890	33.90	38.660	44.13	11.920
        ACAG	NaN	-37.96	-40.880	-14.33	-39.160
        AOUC	3.023	NaN	3.046	NaN	3.064
        >>> variables["ACAF, ACAG, AOUC", "1991Y1:1995Y1"] = df
        >>> variables["ACAF, ACAG, AOUC", "1991Y1:1995Y1"]         # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Workspace: Variables
        nb variables: 3
        filename: ...fun.var
        description: Modèle fun - Simulation 1                         
        sample: 1991Y1:1995Y1
        mode: LEVEL
        <BLANKLINE>
        name	1991Y1	1992Y1	1993Y1	1994Y1	1995Y1
        ACAF	 30.89	 33.90	 38.66	 44.13	 11.92
        ACAG	    na	-37.96	-40.88	-14.33	-39.16
        AOUC	  3.02	    na	  3.05	    na	  3.06
        <BLANKLINE>
        >>> # 5) using an Array object (from the larray library)
        >>> data += 2.0
        >>> names_axis = la.Axis(name="names", labels=["ACAF", "ACAG", "AOUC"])
        >>> time_axis = la.Axis(name="time", labels=periods)
        >>> array = la.Array(data, axes=[names_axis, time_axis])
        >>> array                                                   # doctest: +NORMALIZE_WHITESPACE
        names\time  1991Y1  1992Y1  1993Y1               1994Y1  1995Y1
              ACAF   32.89    35.9   40.66                46.13   13.92
              ACAG     nan  -35.96  -38.88  -12.329999999999998  -37.16
              AOUC   5.023     nan   5.046                  nan   5.064
        >>> variables["ACAF, ACAG, AOUC", "1991Y1:1995Y1"] = array
        >>> variables["ACAF, ACAG, AOUC", "1991Y1:1995Y1"]          # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Workspace: Variables
        nb variables: 3
        filename: ...fun.var
        description: Modèle fun - Simulation 1                         
        sample: 1991Y1:1995Y1
        mode: LEVEL
        <BLANKLINE>
        name	1991Y1	1992Y1	1993Y1	1994Y1	1995Y1
        ACAF	 32.89	 35.90	 40.66	 46.13	 13.92
        ACAG	    na	-35.96	-38.88	-12.33	-37.16
        AOUC	  5.02	    na	  5.05	    na	  5.06
        <BLANKLINE>
        >>> # 6) using another variables database (subset)
        >>> variables_subset = variables["ACAF, ACAG, AOUC", "1991Y1:1995Y1"].copy()
        >>> variables_subset["ACAF"] = [1991, 1992, 1993, 1994, 1995]
        >>> variables_subset["ACAG"] = [1996, 1997, 1998, 1999, 2000]
        >>> variables_subset["AOUC"] = [2001, 2002, 2003, 2004, 2005]
        >>> variables["ACAF, ACAG, AOUC", "1991Y1:1995Y1"] = variables_subset
        >>> variables["ACAF, ACAG, AOUC", "1991Y1:1995Y1"]          # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Workspace: Variables
        nb variables: 3
        filename: ...fun.var
        description: Modèle fun - Simulation 1                         
        sample: 1991Y1:1995Y1
        mode: LEVEL
        <BLANKLINE>
        name	 1991Y1	 1992Y1	 1993Y1	 1994Y1	 1995Y1
        ACAF	1991.00	1992.00	1993.00	1994.00	1995.00
        ACAG	1996.00 1997.00 1998.00 1999.00 2000.00
        AOUC	2001.00 2002.00 2003.00 2004.00 2005.00
        <BLANKLINE>

        >>> # d) -------- working on a subset (whole sample) --------
        >>> # reset variables database to initial state
        >>> variables.load(f"{SAMPLE_DATA_DIR}/fun.var")        # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Loading .../fun.var
        394 objects loaded
        >>> # 1) get subset
        >>> vars_subset = variables["A*"]
        >>> vars_subset.names
        ['ACAF', 'ACAG', 'AOUC', 'AOUC_', 'AQC']
        >>> # 2) add a variable to the subset
        >>> vars_subset["A0"] = np.nan
        >>> vars_subset["A0"]              # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Workspace: Variables
        nb variables: 1
        filename: ...fun.var
        description: Modèle fun - Simulation 1
        sample: 1960Y1:2015Y1
        mode: LEVEL
        <BLANKLINE>
        name        1960Y1  1961Y1  ...  2014Y1   2015Y1
        A0              na      na  ...      na       na
        <BLANKLINE>
        >>> # --> new variable also appears in the global workspace
        >>> "A0" in variables
        True
        >>> variables["A0"]                     # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Workspace: Variables
        nb variables: 1
        filename: ...fun.var
        description: Modèle fun - Simulation 1
        sample: 1960Y1:2015Y1
        mode: LEVEL
        <BLANKLINE>
        name        1960Y1  1961Y1  ...  2014Y1   2015Y1
        A0              na      na  ...      na       na
        <BLANKLINE>
        >>> # 3) update a variable in the subset
        >>> vars_subset["A0"] = 0.0
        >>> vars_subset["A0"]              # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Workspace: Variables
        nb variables: 1
        filename: ...fun.var
        description: Modèle fun - Simulation 1
        sample: 1960Y1:2015Y1
        mode: LEVEL
        <BLANKLINE>
        name        1960Y1  1961Y1  ...  2014Y1   2015Y1
        A0            0.00    0.00  ...    0.00     0.00
        <BLANKLINE>
        >>> # --> variable is also updated in the global workspace
        >>> variables["A0"]                     # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Workspace: Variables
        nb variables: 1
        filename: ...fun.var
        description: Modèle fun - Simulation 1
        sample: 1960Y1:2015Y1
        mode: LEVEL
        <BLANKLINE>
        name        1960Y1  1961Y1  ...  2014Y1   2015Y1
        A0            0.00    0.00  ...    0.00     0.00
        <BLANKLINE>
        >>> # 4) delete a variable in the subset
        >>> del vars_subset["A0"]
        >>> "A0" in vars_subset
        False
        >>> # --> variable is also deleted in the global workspace
        >>> "A0" in variables
        False

        >>> # e) -------- working on a subset (names + periods) --------
        >>> # 1) get subset
        >>> vars_subset = variables["A*", "1991Y1:1995Y1"]
        >>> vars_subset.names
        ['ACAF', 'ACAG', 'AOUC', 'AOUC_', 'AQC']
        >>> vars_subset                     # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Workspace: Variables
        nb variables: 5
        filename: ...fun.var
        description: Modèle fun - Simulation 1
        sample: 1991Y1:1995Y1
        mode: LEVEL
        <BLANKLINE>
         name       1991Y1  1992Y1  1993Y1  1994Y1  1995Y1
        ACAF         26.24   30.16   34.66    8.16  -13.13
        ACAG        -30.93  -40.29  -43.16  -16.03  -41.85
        AOUC          1.02    1.03    1.03    1.05    1.05
        AOUC_         0.96    0.97    0.98    0.99    1.00
        AQC           1.06    1.11    1.15    1.16    1.16
        <BLANKLINE>
        >>> # 2) add a new variable in the subset -> Forbidden !
        >>> vars_subset["A0"] = 0.0         # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Traceback (most recent call last):
           ...
        RuntimeError: Cannot add the IODE variable 'A0' when the subset does not cover the whole sample of the IODE Variables workspace.
        >>> # 3) update a variable in the subset
        >>> vars_subset["ACAF"] = 1.0
        >>> vars_subset["ACAF"]             # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Workspace: Variables
        nb variables: 1
        filename: ...fun.var
        description: Modèle fun - Simulation 1
        sample: 1991Y1:1995Y1
        mode: LEVEL
        <BLANKLINE>
        name        1991Y1  1992Y1  1993Y1  1994Y1  1995Y1
        ACAF          1.00    1.00    1.00    1.00    1.00
        <BLANKLINE>
        >>> # --> variable is also updated in the global workspace
        >>> variables["ACAF", "1991Y1"]
        1.0
        >>> variables["ACAF", "1995Y1"]
        1.0
        >>> # 4) delete a variable in the subset -> Forbidden !
        >>> del vars_subset["ACAF"]           # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Traceback (most recent call last):
          ...
        RuntimeError: Cannot delete variable(s) 'ACAF' when the subset does not cover the whole sample of the IODE Variables workspace
        """
        cdef int t_first
        cdef int t_last
        cdef double c_value
        cdef IodeVarMode level_mode = IodeVarMode.VAR_MODE_LEVEL

        names, key_periods = self._unfold_key(key)

        # check type of passed 'value' and convert np.nan to IODE NA
        value = self._convert_values(value)

        # if value is a string (LEC expression) or a numerical value 
        # -> set the same value for all variables and periods
        if isinstance(value, str) or not isinstance(value, Iterable):
            for name in names:
                self._set_variable(name, value, key_periods)
            return
        
        # if value is a list or a tuple
        if isinstance(value, (list, tuple)):
            if len(names) == 1:
                self._set_variable(names[0], value, key_periods)
            else:
                if len(value) != len(names):
                    raise ValueError(f"Expected {len(names)} values to set the variables.\n"
                                    f"Got {len(value)} values instead.")
                for name, _value in zip(names, value):
                    self._set_variable(name, _value, key_periods)
            return

        # if value is a dict
        if isinstance(value, dict):
            # check that all names in the selection key are present in the dict
            self._check_same_names(names, value.keys())
            for name, _value in value.items():
                self._set_variable(name, _value, key_periods)
            return

        # if value is an Variables object
        if isinstance(value, Variables):
            if len(names) > 1:
                # check that names in the selection key are present in the Variables object
                self._check_same_names(names, value.names)
            for name in names:
                # NOTE: _set_variable() will extract the data for the given name.
                #       No need to pass value[name] here.
                self._set_variable(name, value, key_periods)
            return

        key_periods: List[str] = self._expand_key_periods(key_periods)
        
        # if value is a numpy array
        if isinstance(value, np.ndarray):
            self.from_numpy(value, names, key_periods[0], key_periods[-1])
            return

        key_period_bounds = key_periods[0], key_periods[-1]

        # if value is a pandas DataFrame
        if isinstance(value, pd.DataFrame):
            value = self._check_pandas_dataframe(value, names, key_periods)
            if isinstance(value, pd.DataFrame):
                data = value.to_numpy(copy=False)
                # see https://cython.readthedocs.io/en/stable/src/userguide/memoryviews.html#pass-data-from-a-c-function-via-pointer
                if not data.flags['C_CONTIGUOUS']:
                    data = np.ascontiguousarray(data)
                for name, _data in zip(names, data):
                    self._set_variable(name, _data, key_period_bounds)
                return

        # if value is pandas Series
        if isinstance(value, pd.Series):
            value = self._check_pandas_series(value, names, key_periods)
            if len(names) > 1:
                for name in names:
                    self._set_variable(name, value[name], key_period_bounds)
            else:
                data = value.to_numpy(copy=False)
                # see https://cython.readthedocs.io/en/stable/src/userguide/memoryviews.html#pass-data-from-a-c-function-via-pointer
                if not data.flags['C_CONTIGUOUS']:
                    data = np.ascontiguousarray(data)
                self._set_variable(names[0], data, key_period_bounds)
            return
        
        if la is not None and isinstance(value, Array):
            value = self._check_larray_array(value, names, key_periods)
            data = value.data
            # see https://cython.readthedocs.io/en/stable/src/userguide/memoryviews.html#pass-data-from-a-c-function-via-pointer
            if not data.flags['C_CONTIGUOUS']:
                data = np.ascontiguousarray(data)
            if len(names) == 1:
                data = data.flatten()
                self._set_variable(names[0], data, key_period_bounds)
            else:
                for name, _data in zip(names, data):
                    self._set_variable(name, _data, key_period_bounds)
            return

        raise TypeError(f"Invalid type for the right hand side value when trying to set variables.\n"
                        f"Expected value of type str, int, float, list(int|float), tuple(int|float), dict(str, ...), "
                        f"numpy array, pandas Series, pandas DataFrame or Variables.\n"
                        f"Got value of type {type(value).__name__} instead.")

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

        >>> # delete one variable from a subset of the global workspace
        >>> vars_subset = variables["D*"]
        >>> vars_subset.names
        ['DEBT', 'DPU', 'DPUF', 'DPUG', 'DPUGO', 'DPUH', 'DPUHO', 'DPUU', 'DTF', 'DTFX', 'DTH', 'DTH1', 'DTH1C', 'DTHX']
        >>> del vars_subset["DPUGO"]
        >>> vars_subset.names
        ['DEBT', 'DPU', 'DPUF', 'DPUG', 'DPUH', 'DPUHO', 'DPUU', 'DTF', 'DTFX', 'DTH', 'DTH1', 'DTH1C', 'DTHX']
        >>> # NOTE: the variable has also been deleted from the global workspace
        >>> "DPUGO" in variables
        False
        >>> variables.get_names("D*")
        ['DEBT', 'DPU', 'DPUF', 'DPUG', 'DPUH', 'DPUHO', 'DPUU', 'DTF', 'DTFX', 'DTH', 'DTH1', 'DTH1C', 'DTHX']
        >>> # WARNING: when deleting (one) variable(s), the period(s) cannot be specified
        >>> del variables["DPUG", "1990Y1:1995Y1"]          # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Traceback (most recent call last):
            ...
        RuntimeError: Cannot delete variable(s) 'DPUG'.
        The syntax 'del variables['DPUG']' must be used instead of 'del variables['DPUG', <periods>]'
        
        >>> # subset over names and periods
        >>> vars_subset = variables["D*", "1990Y1:1995Y1"]
        >>> vars_subset.names
        ['DEBT', 'DPU', 'DPUF', 'DPUG', 'DPUH', 'DPUHO', 'DPUU', 'DTF', 'DTFX', 'DTH', 'DTH1', 'DTH1C', 'DTHX']
        >>> del vars_subset["DPUG"]                         # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Traceback (most recent call last):
            ...
        RuntimeError: Cannot delete variable(s) 'DPUG' when the subset does not cover the whole sample of the IODE Variables workspace
        """
        names, key_periods = self._unfold_key(key)
        if key_periods is not None:
            names = f"'{names[0]}'" if len(names) == 1 else names
            raise RuntimeError(f"Cannot delete variable(s) {names}.\nThe syntax 'del variables[{names}]' "
                               f"must be used instead of 'del variables[{names}, <periods>]'")
        if self.first_period_subset is not None or self.last_period_subset is not None:
            names = f"'{names[0]}'" if len(names) == 1 else names
            raise RuntimeError(f"Cannot delete variable(s) {names} when the subset does not cover the "
                               f"whole sample of the IODE Variables workspace")

        # names represents a single Variable
        if len(names) == 1:
            self.database_ptr.remove(names[0].encode())
        # names represents a selection of Variables
        elif key_periods is None:
            for name in names:
                self.database_ptr.remove(name.encode())
        else:
            raise RuntimeError(f"Cannot select period(s) when deleting (a) variable(s)")

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
        r"""
        Add `other` to the current (subset of) Variables object.

        Parameters
        ----------
        other: int, float, numpy ndarray, pandas Series, pandas DataFrame, larray Array or iode Variables
            If `other` is an int or a float, add the scalar to all values of the current (subset of) Variables object.
            If `other` is a numpy ndarray, the shape of the ndarray must be compatible with the current (subset of) 
            Variables object. Specifically, the number of rows must be equal to the number of variables and the number of 
            columns must be equal to the number of periods.
            If `other` is a pandas Series, it must represent either a single variable or a single period.
            If `other` is a pandas DataFrame, it must represent the same variables names and periods 
            as the current (subset of) Variables object. Specifically, the index of the DataFrame must be equal to the 
            variables names and the columns of the DataFrame must be equal to the periods.
            If `other` is an larray Array, its last axis must be equal to the periods and be named 'time'. 
            If the Array has more than two axes, the first n-1 axes are combined to form the variables names. 
            The first (combined) axis must be equal to the variables names.
            If `other` is an iode Variables object, add the two Variables objects.
            `self` and `other` must share the same sample and represent the same set of variables names.
        
        Returns
        -------
        Variables

        Warnings
        --------
        Adding a numpy ndarray to a Variables object is not recommended as there is no compatibility check 
        between for the names and periods. The result is not guaranteed to be the one you expected. 
        This possibility is provided for speed reasons (when the database or the subset is large).

        Examples
        --------
        >>> import numpy as np
        >>> import pandas as pd
        >>> import larray as la
        >>> from iode import SAMPLE_DATA_DIR
        >>> from iode import variables, NA, Sample
        >>> variables.load(f"{SAMPLE_DATA_DIR}/fun.var")        # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Loading .../fun.var
        394 objects loaded
        >>> vars_subset = variables["A*", "1991Y1:1995Y1"]
        >>> vars_subset.names
        ['ACAF', 'ACAG', 'AOUC', 'AOUC_', 'AQC']
        >>> vars_subset                                         # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Workspace: Variables
        nb variables: 5
        filename: ...fun.var
        description: Modèle fun - Simulation 1
        sample: 1991Y1:1995Y1
        mode: LEVEL
        <BLANKLINE>
         name       1991Y1  1992Y1  1993Y1  1994Y1  1995Y1
        ACAF         26.24   30.16   34.66    8.16  -13.13
        ACAG        -30.93  -40.29  -43.16  -16.03  -41.85
        AOUC          1.02    1.03    1.03    1.05    1.05
        AOUC_         0.96    0.97    0.98    0.99    1.00
        AQC           1.06    1.11    1.15    1.16    1.16
        <BLANKLINE>

        >>> # add a scalar to all values of a subset of a Variables object
        >>> new_vars_subset = vars_subset + 2.0
        >>> new_vars_subset                                     # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Workspace: Variables
        nb variables: 5
        filename: ...fun.var
        description: Modèle fun - Simulation 1
        sample: 1991Y1:1995Y1
        mode: LEVEL
        <BLANKLINE>
         name       1991Y1  1992Y1  1993Y1  1994Y1  1995Y1
        ACAF         28.24   32.16   36.66   10.16  -11.13
        ACAG        -28.93  -38.29  -41.16  -14.03  -39.85
        AOUC          3.02    3.03    3.03    3.05    3.05
        AOUC_         2.96    2.97    2.98    2.99    3.00
        AQC           3.06    3.11    3.15    3.16    3.16
        <BLANKLINE>

        >>> # add two (subsets of) a Variables object
        >>> new_vars_subset = vars_subset + vars_subset
        >>> new_vars_subset                                 # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Workspace: Variables
        nb variables: 5
        filename: ...fun.var
        description: Modèle fun - Simulation 1
        sample: 1991Y1:1995Y1
        mode: LEVEL
        <BLANKLINE>
         name       1991Y1  1992Y1  1993Y1  1994Y1  1995Y1
        ACAF         52.48   60.32   69.32   16.32  -26.26
        ACAG        -61.87  -80.57  -86.32  -32.06  -83.69
        AOUC          2.05    2.06    2.06    2.09    2.10
        AOUC_         1.93    1.95    1.96    1.98    1.99
        AQC           2.13    2.22    2.31    2.31    2.32
        <BLANKLINE>

        >>> # add a pandas Series to a single variable
        >>> series = pd.Series([1.0, 2.0, 3.0, 4.0, 5.0], index=vars_subset.periods)
        >>> series                                              # doctest: +NORMALIZE_WHITESPACE
        1991Y1    1.0
        1992Y1    2.0
        1993Y1    3.0
        1994Y1    4.0
        1995Y1    5.0
        dtype: float64
        >>> updated_ACAF = vars_subset["ACAF"] + series
        >>> updated_ACAF                                        # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Workspace: Variables
        nb variables: 1
        filename: ...fun.var
        description: Modèle fun - Simulation 1
        sample: 1991Y1:1995Y1
        mode: LEVEL
        <BLANKLINE>
         name       1991Y1  1992Y1  1993Y1  1994Y1  1995Y1
        ACAF         27.24   32.16   37.66   12.16   -8.13
        <BLANKLINE>
        
        >>> # add a pandas Series to a subset corresponding to a single period
        >>> series = pd.Series([1.0, 2.0, 3.0, 4.0, 5.0], index=vars_subset.names)
        >>> series                                          # doctest: +NORMALIZE_WHITESPACE
        ACAF     1.0
        ACAG     2.0
        AOUC     3.0
        AOUC_    4.0
        AQC      5.0
        dtype: float64
        >>> vars_subset_1995Y1 = vars_subset[:, "1995Y1"] + series
        >>> vars_subset_1995Y1                              # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Workspace: Variables
        nb variables: 5
        filename: ...fun.var
        description: Modèle fun - Simulation 1
        sample: 1995Y1:1995Y1
        mode: LEVEL
        <BLANKLINE>
         name       1995Y1
        ACAF        -12.13
        ACAG        -39.85
        AOUC          4.05
        AOUC_         5.00
        AQC           6.16
        <BLANKLINE>

        >>> # add a pandas DataFrame to the subset of a Variables object  
        >>> data = np.array([[1.0, 2.0, 3.0, 4.0, 5.0], 
        ...                  [6.0, 7.0, 8.0, 9.0, 10.0], 
        ...                  [11.0, 12.0, 13.0, 14.0, 15.0], 
        ...                  [16.0, 17.0, 18.0, 19.0, 20.0], 
        ...                  [21.0, 22.0, 23.0, 24.0, 25.0]],)
        >>> df = pd.DataFrame(data, index=vars_subset.names, columns=vars_subset.periods) 
        >>> df                                              # doctest: +NORMALIZE_WHITESPACE
               1991Y1  1992Y1  1993Y1  1994Y1  1995Y1
        ACAF      1.0     2.0     3.0     4.0     5.0
        ACAG      6.0     7.0     8.0     9.0    10.0
        AOUC     11.0    12.0    13.0    14.0    15.0
        AOUC_    16.0    17.0    18.0    19.0    20.0
        AQC      21.0    22.0    23.0    24.0    25.0
        >>> new_vars_subset = vars_subset + df
        >>> new_vars_subset                                 # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Workspace: Variables
        nb variables: 5
        filename: ...fun.var
        description: Modèle fun - Simulation 1
        sample: 1991Y1:1995Y1
        mode: LEVEL
        <BLANKLINE>
         name       1991Y1  1992Y1  1993Y1  1994Y1  1995Y1
        ACAF         27.24   32.16   37.66   12.16   -8.13
        ACAG        -24.93  -33.29  -35.16   -7.03  -31.85
        AOUC         12.02   13.03   14.03   15.05   16.05
        AOUC_        16.96   17.97   18.98   19.99   21.00
        AQC          22.06   23.11   24.15   25.16   26.16
        <BLANKLINE>

        >>> # add an larray Array to a subset of a Variables object
        >>> axis_names = la.Axis(name="names", labels=vars_subset.names)
        >>> axis_time = la.Axis(name="time", labels=vars_subset.periods)
        >>> array = la.Array(data, axes=(axis_names, axis_time))
        >>> array                                           # doctest: +NORMALIZE_WHITESPACE
        names\time  1991Y1  1992Y1  1993Y1  1994Y1  1995Y1
              ACAF     1.0     2.0     3.0     4.0     5.0
              ACAG     6.0     7.0     8.0     9.0    10.0
              AOUC    11.0    12.0    13.0    14.0    15.0
             AOUC_    16.0    17.0    18.0    19.0    20.0
               AQC    21.0    22.0    23.0    24.0    25.0
        >>> new_vars_subset = vars_subset + array
        >>> new_vars_subset                                 # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Workspace: Variables
        nb variables: 5
        filename: ...fun.var
        description: Modèle fun - Simulation 1
        sample: 1991Y1:1995Y1
        mode: LEVEL
        <BLANKLINE>
         name       1991Y1  1992Y1  1993Y1  1994Y1  1995Y1
        ACAF         27.24   32.16   37.66   12.16   -8.13
        ACAG        -24.93  -33.29  -35.16   -7.03  -31.85
        AOUC         12.02   13.03   14.03   15.05   16.05
        AOUC_        16.96   17.97   18.98   19.99   21.00
        AQC          22.06   23.11   24.15   25.16   26.16
        <BLANKLINE>        

        >>> # WARNING: adding a numpy ndarray to a (subset of a) Variables object is not recommended 
        >>> #          as there is no compatibility check between for the names and periods.
        >>> #          The result is not guaranteed to be the one you expected.
        >>> #          This possibility is provided for speed reasons 
        >>> #          (when dealing with large subsets/databases).
        >>> # add a numpy 1D ndarray to a single variable
        >>> data = np.array([1.0, 2.0, 3.0, 4.0, 5.0])
        >>> updated_ACAF = vars_subset["ACAF"] + data
        >>> updated_ACAF                                    # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Workspace: Variables
        nb variables: 1
        filename: ...fun.var
        description: Modèle fun - Simulation 1
        sample: 1991Y1:1995Y1
        mode: LEVEL
        <BLANKLINE>
         name       1991Y1  1992Y1  1993Y1  1994Y1  1995Y1
        ACAF         27.24   32.16   37.66   12.16   -8.13
        <BLANKLINE>
        >>> # add a numpy 1D ndarray to the subset corresponding to a single period
        >>> vars_subset_1995Y1 = vars_subset[:, "1995Y1"] + data
        >>> vars_subset_1995Y1                              # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Workspace: Variables
        nb variables: 5
        filename: ...fun.var
        description: Modèle fun - Simulation 1
        sample: 1995Y1:1995Y1
        mode: LEVEL
        <BLANKLINE>
         name       1995Y1
        ACAF        -12.13
        ACAG        -39.85
        AOUC          4.05
        AOUC_         5.00
        AQC           6.16
        <BLANKLINE>        
        >>> # add a numpy 2D ndarray to a (subset of a) Variables object
        >>> data = np.array([[1.0, 2.0, 3.0, 4.0, 5.0], 
        ...                  [6.0, 7.0, 8.0, 9.0, 10.0], 
        ...                  [11.0, 12.0, 13.0, 14.0, 15.0], 
        ...                  [16.0, 17.0, 18.0, 19.0, 20.0], 
        ...                  [21.0, 22.0, 23.0, 24.0, 25.0]])
        >>> new_vars_subset = vars_subset + data
        >>> new_vars_subset                                 # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Workspace: Variables
        nb variables: 5
        filename: ...fun.var
        description: Modèle fun - Simulation 1
        sample: 1991Y1:1995Y1
        mode: LEVEL
        <BLANKLINE>
         name       1991Y1  1992Y1  1993Y1  1994Y1  1995Y1
        ACAF         27.24   32.16   37.66   12.16   -8.13
        ACAG        -24.93  -33.29  -35.16   -7.03  -31.85
        AOUC         12.02   13.03   14.03   15.05   16.05
        AOUC_        16.96   17.97   18.98   19.99   21.00
        AQC          22.06   23.11   24.15   25.16   26.16
        <BLANKLINE>
        """
        return self.__binary_op__(other, BinaryOperation.OP_ADD, True)

    # other + self
    def __radd__(self, other):
        r"""
        Add `other` to the current (subset of) Variables object.

        Parameters
        ----------
        other: int, float, numpy ndarray, larray Array or iode Variables
            If `other` is an int or a float, add the scalar to all values of the current (subset of) Variables object.
            If `other` is a numpy ndarray, the shape of the ndarray must be compatible with the current (subset of) 
            Variables object. Specifically, the number of rows must be equal to the number of variables and the number of 
            columns must be equal to the number of periods.
            If `other` is an larray Array, its last axis must be equal to the periods and be named 'time'. 
            If the Array has more than two axes, the first n-1 axes are combined to form the variables names. 
            The first (combined) axis must be equal to the variables names.
            If `other` is an iode Variables object, add the two Variables objects.
            `self` and `other` must share the same sample and represent the same set of variables names.
        
        Returns
        -------
        Variables

        Warnings
        --------
        Adding a numpy ndarray to a Variables object is not recommended as there is no compatibility check 
        between for the names and periods. The result is not guaranteed to be the one you expected. 
        This possibility is provided for speed reasons (when the database or the subset is large).

        Examples
        --------
        >>> import numpy as np
        >>> import pandas as pd
        >>> import larray as la
        >>> from iode import SAMPLE_DATA_DIR
        >>> from iode import variables, NA, Sample
        >>> variables.load(f"{SAMPLE_DATA_DIR}/fun.var")        # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Loading .../fun.var
        394 objects loaded
        >>> vars_subset = variables["A*", "1991Y1:1995Y1"]
        >>> vars_subset.names
        ['ACAF', 'ACAG', 'AOUC', 'AOUC_', 'AQC']
        >>> vars_subset                                         # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Workspace: Variables
        nb variables: 5
        filename: ...fun.var
        description: Modèle fun - Simulation 1
        sample: 1991Y1:1995Y1
        mode: LEVEL
        <BLANKLINE>
         name       1991Y1  1992Y1  1993Y1  1994Y1  1995Y1
        ACAF         26.24   30.16   34.66    8.16  -13.13
        ACAG        -30.93  -40.29  -43.16  -16.03  -41.85
        AOUC          1.02    1.03    1.03    1.05    1.05
        AOUC_         0.96    0.97    0.98    0.99    1.00
        AQC           1.06    1.11    1.15    1.16    1.16
        <BLANKLINE>

        >>> # add a scalar to all values of a subset of a Variables object
        >>> new_vars_subset = 2.0 + vars_subset
        >>> new_vars_subset                                     # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Workspace: Variables
        nb variables: 5
        filename: ...fun.var
        description: Modèle fun - Simulation 1
        sample: 1991Y1:1995Y1
        mode: LEVEL
        <BLANKLINE>
         name       1991Y1  1992Y1  1993Y1  1994Y1  1995Y1
        ACAF         28.24   32.16   36.66   10.16  -11.13
        ACAG        -28.93  -38.29  -41.16  -14.03  -39.85
        AOUC          3.02    3.03    3.03    3.05    3.05
        AOUC_         2.96    2.97    2.98    2.99    3.00
        AQC           3.06    3.11    3.15    3.16    3.16
        <BLANKLINE>

        >>> # add two (subsets of) a Variables object
        >>> new_vars_subset = vars_subset + vars_subset
        >>> new_vars_subset                                 # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Workspace: Variables
        nb variables: 5
        filename: ...fun.var
        description: Modèle fun - Simulation 1
        sample: 1991Y1:1995Y1
        mode: LEVEL
        <BLANKLINE>
         name       1991Y1  1992Y1  1993Y1  1994Y1  1995Y1
        ACAF         52.48   60.32   69.32   16.32  -26.26
        ACAG        -61.87  -80.57  -86.32  -32.06  -83.69
        AOUC          2.05    2.06    2.06    2.09    2.10
        AOUC_         1.93    1.95    1.96    1.98    1.99
        AQC           2.13    2.22    2.31    2.31    2.32
        <BLANKLINE>

        >>> # add an larray Array to a subset of a Variables object
        >>> data = np.array([[1.0, 2.0, 3.0, 4.0, 5.0], 
        ...                  [6.0, 7.0, 8.0, 9.0, 10.0], 
        ...                  [11.0, 12.0, 13.0, 14.0, 15.0], 
        ...                  [16.0, 17.0, 18.0, 19.0, 20.0], 
        ...                  [21.0, 22.0, 23.0, 24.0, 25.0]])
        >>> axis_names = la.Axis(name="names", labels=vars_subset.names)
        >>> axis_time = la.Axis(name="time", labels=vars_subset.periods)
        >>> array = la.Array(data, axes=(axis_names, axis_time))
        >>> array                                           # doctest: +NORMALIZE_WHITESPACE
        names\time  1991Y1  1992Y1  1993Y1  1994Y1  1995Y1
              ACAF     1.0     2.0     3.0     4.0     5.0
              ACAG     6.0     7.0     8.0     9.0    10.0
              AOUC    11.0    12.0    13.0    14.0    15.0
             AOUC_    16.0    17.0    18.0    19.0    20.0
               AQC    21.0    22.0    23.0    24.0    25.0
        >>> new_vars_subset = array + vars_subset
        >>> new_vars_subset                                 # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Workspace: Variables
        nb variables: 5
        filename: ...fun.var
        description: Modèle fun - Simulation 1
        sample: 1991Y1:1995Y1
        mode: LEVEL
        <BLANKLINE>
         name       1991Y1  1992Y1  1993Y1  1994Y1  1995Y1
        ACAF         27.24   32.16   37.66   12.16   -8.13
        ACAG        -24.93  -33.29  -35.16   -7.03  -31.85
        AOUC         12.02   13.03   14.03   15.05   16.05
        AOUC_        16.96   17.97   18.98   19.99   21.00
        AQC          22.06   23.11   24.15   25.16   26.16
        <BLANKLINE>        

        >>> # WARNING: adding a numpy ndarray to a (subset of a) Variables object is not recommended 
        >>> #          as there is no compatibility check between for the names and periods.
        >>> #          The result is not guaranteed to be the one you expected.
        >>> #          This possibility is provided for speed reasons 
        >>> #          (when dealing with large subsets/databases).
        >>> # add a numpy 1D ndarray to a single variable
        >>> data = np.array([1.0, 2.0, 3.0, 4.0, 5.0])
        >>> updated_ACAF = data + vars_subset["ACAF"]
        >>> updated_ACAF                                    # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        array([ 27.240999 ,  32.159    ,  37.661999 ,  12.1610022,  -8.130997 ])
        >>> # add a numpy 1D ndarray to the subset corresponding to a single period
        >>> vars_subset_1995Y1 = data + vars_subset[:, "1995Y1"]
        >>> vars_subset_1995Y1                              # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        array([[-12.130997  , -11.130997  , -10.130997  ,  -9.130997  ,
                 -8.130997  ],
               [-40.845993  , -39.845993  , -38.845993  , -37.845993  ,
                -36.845993  ],
               [  2.0498914 ,   3.0498914 ,   4.0498914 ,   5.0498914 ,
                  6.0498914 ],
               [  1.99526324,   2.99526324,   3.99526324,   4.99526324,
                  5.99526324],
               [  2.1616869 ,   3.1616869 ,   4.1616869 ,   5.1616869 ,
                  6.1616869 ]])        
        >>> # add a numpy 2D ndarray to a (subset of a) Variables object
        >>> data = np.array([[1.0, 2.0, 3.0, 4.0, 5.0], 
        ...                  [6.0, 7.0, 8.0, 9.0, 10.0], 
        ...                  [11.0, 12.0, 13.0, 14.0, 15.0], 
        ...                  [16.0, 17.0, 18.0, 19.0, 20.0], 
        ...                  [21.0, 22.0, 23.0, 24.0, 25.0]])
        >>> new_vars_subset = data + vars_subset
        >>> new_vars_subset                                 # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        array([[ 27.240999  ,  32.159     ,  37.661999  ,  12.1610022 ,
                 -8.130997  ],
               [-24.934     , -33.285999  , -35.157997  ,  -7.029003  ,
                -31.845993  ],
               [ 12.02443339,  13.0314501 ,  14.03091768,  15.04628419,
                 16.0498914 ],
               [ 16.96466659,  17.97403904,  18.97881286,  19.98955638,
                 20.99526324],
               [ 22.0628064 ,  23.1102825 ,  24.1532652 ,  25.1571276 ,
                 26.1616869 ]])
        """
        return self.__binary_op__(other, BinaryOperation.OP_ADD, True)

    # self += other
    def __iadd__(self, other):
        r"""
        Add `other` to the current (subset of) Variables object.

        Parameters
        ----------
        other: int, float, numpy ndarray, pandas Series, pandas DataFrame, larray Array or iode Variables
            If `other` is an int or a float, add the scalar to all values of the current (subset of) Variables object.
            If `other` is a numpy ndarray, the shape of the ndarray must be compatible with the current (subset of) 
            Variables object. Specifically, the number of rows must be equal to the number of variables and the number of 
            columns must be equal to the number of periods.
            If `other` is a pandas Series, it must represent either a single variable or a single period.
            If `other` is a pandas DataFrame, it must represent the same variables names and periods 
            as the current (subset of) Variables object. Specifically, the index of the DataFrame must be equal to the 
            variables names and the columns of the DataFrame must be equal to the periods.
            If `other` is an larray Array, its last axis must be equal to the periods and be named 'time'. 
            If the Array has more than two axes, the first n-1 axes are combined to form the variables names. 
            The first (combined) axis must be equal to the variables names.
            If `other` is an iode Variables object, add the two Variables objects.
            `self` and `other` must share the same sample and represent the same set of variables names.

        Warnings
        --------
        Adding a numpy ndarray to a Variables object is not recommended as there is no compatibility check 
        between for the names and periods. The result is not guaranteed to be the one you expected. 
        This possibility is provided for speed reasons (when the database or the subset is large).

        Examples
        --------
        >>> import numpy as np
        >>> import pandas as pd
        >>> import larray as la
        >>> from iode import SAMPLE_DATA_DIR
        >>> from iode import variables, NA, Sample
        >>> variables.load(f"{SAMPLE_DATA_DIR}/fun.var")        # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Loading .../fun.var
        394 objects loaded
        >>> vars_subset = variables["A*", "1991Y1:1995Y1"]
        >>> vars_subset.names
        ['ACAF', 'ACAG', 'AOUC', 'AOUC_', 'AQC']
        >>> vars_subset                                         # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Workspace: Variables
        nb variables: 5
        filename: ...fun.var
        description: Modèle fun - Simulation 1
        sample: 1991Y1:1995Y1
        mode: LEVEL
        <BLANKLINE>
         name       1991Y1  1992Y1  1993Y1  1994Y1  1995Y1
        ACAF         26.24   30.16   34.66    8.16  -13.13
        ACAG        -30.93  -40.29  -43.16  -16.03  -41.85
        AOUC          1.02    1.03    1.03    1.05    1.05
        AOUC_         0.96    0.97    0.98    0.99    1.00
        AQC           1.06    1.11    1.15    1.16    1.16
        <BLANKLINE>

        >>> # add a scalar to all values of the current subset of a Variables object
        >>> vars_subset += 2.0
        >>> vars_subset                                         # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Workspace: Variables
        nb variables: 5
        filename: ...fun.var
        description: Modèle fun - Simulation 1
        sample: 1991Y1:1995Y1
        mode: LEVEL
        <BLANKLINE>
         name       1991Y1  1992Y1  1993Y1  1994Y1  1995Y1
        ACAF         28.24   32.16   36.66   10.16  -11.13
        ACAG        -28.93  -38.29  -41.16  -14.03  -39.85
        AOUC          3.02    3.03    3.03    3.05    3.05
        AOUC_         2.96    2.97    2.98    2.99    3.00
        AQC           3.06    3.11    3.15    3.16    3.16
        <BLANKLINE>

        >>> # add a (subsets of) a Variables object to the current (subset of) Variables object
        >>> vars_subset += vars_subset
        >>> vars_subset                                 # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Workspace: Variables
        nb variables: 5
        filename: ...fun.var
        description: Modèle fun - Simulation 1
        sample: 1991Y1:1995Y1
        mode: LEVEL
        <BLANKLINE>
         name       1991Y1  1992Y1  1993Y1  1994Y1  1995Y1
        ACAF         56.48   64.32   73.32   20.32  -22.26
        ACAG        -57.87  -76.57  -82.32  -28.06  -79.69
        AOUC          6.05    6.06    6.06    6.09    6.10
        AOUC_         5.93    5.95    5.96    5.98    5.99
        AQC           6.13    6.22    6.31    6.31    6.32
        <BLANKLINE>

        >>> # add a pandas Series to a single variable
        >>> series = pd.Series([1.0, 2.0, 3.0, 4.0, 5.0], index=vars_subset.periods)
        >>> series                                              # doctest: +NORMALIZE_WHITESPACE
        1991Y1    1.0
        1992Y1    2.0
        1993Y1    3.0
        1994Y1    4.0
        1995Y1    5.0
        dtype: float64
        >>> vars_subset["ACAF"] += series
        >>> vars_subset["ACAF"]                                 # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Workspace: Variables
        nb variables: 1
        filename: ...fun.var
        description: Modèle fun - Simulation 1
        sample: 1991Y1:1995Y1
        mode: LEVEL
        <BLANKLINE>
         name       1991Y1  1992Y1  1993Y1  1994Y1  1995Y1
        ACAF         57.48   66.32   76.32   24.32  -17.26
        <BLANKLINE>
        
        >>> # add a pandas Series to the subset corresponding to a single period
        >>> series = pd.Series([1.0, 2.0, 3.0, 4.0, 5.0], index=vars_subset.names)
        >>> series                                          # doctest: +NORMALIZE_WHITESPACE
        ACAF     1.0
        ACAG     2.0
        AOUC     3.0
        AOUC_    4.0
        AQC      5.0
        dtype: float64
        >>> vars_subset[:, "1995Y1"] += series
        >>> vars_subset[:, "1995Y1"]                        # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Workspace: Variables
        nb variables: 5
        filename: ...fun.var
        description: Modèle fun - Simulation 1
        sample: 1995Y1:1995Y1
        mode: LEVEL
        <BLANKLINE>
         name       1995Y1
        ACAF        -16.26
        ACAG        -77.69
        AOUC          9.10
        AOUC_         9.99
        AQC          11.32
        <BLANKLINE>

        >>> # add a pandas DataFrame to the current subset of the Variables object  
        >>> data = np.array([[1.0, 2.0, 3.0, 4.0, 5.0], 
        ...                  [6.0, 7.0, 8.0, 9.0, 10.0], 
        ...                  [11.0, 12.0, 13.0, 14.0, 15.0], 
        ...                  [16.0, 17.0, 18.0, 19.0, 20.0], 
        ...                  [21.0, 22.0, 23.0, 24.0, 25.0]],)
        >>> df = pd.DataFrame(data, index=vars_subset.names, columns=vars_subset.periods) 
        >>> df                                              # doctest: +NORMALIZE_WHITESPACE
               1991Y1  1992Y1  1993Y1  1994Y1  1995Y1
        ACAF      1.0     2.0     3.0     4.0     5.0
        ACAG      6.0     7.0     8.0     9.0    10.0
        AOUC     11.0    12.0    13.0    14.0    15.0
        AOUC_    16.0    17.0    18.0    19.0    20.0
        AQC      21.0    22.0    23.0    24.0    25.0
        >>> vars_subset += df
        >>> vars_subset                                     # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Workspace: Variables
        nb variables: 5
        filename: ...fun.var
        description: Modèle fun - Simulation 1
        sample: 1991Y1:1995Y1
        mode: LEVEL
        <BLANKLINE>
         name       1991Y1  1992Y1  1993Y1  1994Y1  1995Y1
        ACAF         58.48   68.32   79.32   28.32  -11.26
        ACAG        -51.87  -69.57  -74.32  -19.06  -67.69
        AOUC         17.05   18.06   19.06   20.09   24.10
        AOUC_        21.93   22.95   23.96   24.98   29.99
        AQC          27.13   28.22   29.31   30.31   36.32
        <BLANKLINE>

        >>> # add an larray Array to the current subset of the Variables object
        >>> axis_names = la.Axis(name="names", labels=vars_subset.names)
        >>> axis_time = la.Axis(name="time", labels=vars_subset.periods)
        >>> array = la.Array(data, axes=(axis_names, axis_time))
        >>> array                                           # doctest: +NORMALIZE_WHITESPACE
        names\time  1991Y1  1992Y1  1993Y1  1994Y1  1995Y1
              ACAF     1.0     2.0     3.0     4.0     5.0
              ACAG     6.0     7.0     8.0     9.0    10.0
              AOUC    11.0    12.0    13.0    14.0    15.0
             AOUC_    16.0    17.0    18.0    19.0    20.0
               AQC    21.0    22.0    23.0    24.0    25.0
        >>> vars_subset += array
        >>> vars_subset                                     # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Workspace: Variables
        nb variables: 5
        filename: ...fun.var
        description: Modèle fun - Simulation 1
        sample: 1991Y1:1995Y1
        mode: LEVEL
        <BLANKLINE>
         name       1991Y1  1992Y1  1993Y1  1994Y1  1995Y1
        ACAF         59.48   70.32   82.32   32.32   -6.26
        ACAG        -45.87  -62.57  -66.32  -10.06  -57.69
        AOUC         28.05   30.06   32.06   34.09   39.10
        AOUC_        37.93   39.95   41.96   43.98   49.99
        AQC          48.13   50.22   52.31   54.31   61.32
        <BLANKLINE>        

        >>> # WARNING: adding a numpy ndarray to a (subset of a) Variables object is not recommended 
        >>> #          as there is no compatibility check between for the names and periods.
        >>> #          The result is not guaranteed to be the one you expected.
        >>> #          This possibility is provided for speed reasons 
        >>> #          (when dealing with large subsets/databases).
        >>> # add a numpy 1D ndarray to a single variable
        >>> data = np.array([1.0, 2.0, 3.0, 4.0, 5.0])
        >>> vars_subset["ACAF"] += data
        >>> vars_subset["ACAF"]                             # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Workspace: Variables
        nb variables: 1
        filename: ...fun.var
        description: Modèle fun - Simulation 1
        sample: 1991Y1:1995Y1
        mode: LEVEL
        <BLANKLINE>
         name       1991Y1  1992Y1  1993Y1  1994Y1  1995Y1
        ACAF         60.48   72.32   85.32   36.32   -1.26
        <BLANKLINE>
        >>> # add a numpy 1D ndarray to the subset corresponding to a single period
        >>> vars_subset[:, "1995Y1"] += data
        >>> vars_subset[:, "1995Y1"]                        # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Workspace: Variables
        nb variables: 5
        filename: ...fun.var
        description: Modèle fun - Simulation 1
        sample: 1995Y1:1995Y1
        mode: LEVEL
        <BLANKLINE>
         name       1995Y1
        ACAF         -0.26
        ACAG        -55.69
        AOUC         42.10
        AOUC_        53.99
        AQC          66.32
        <BLANKLINE>        
        >>> # add a numpy 2D ndarray to the current (subset of the) Variables object
        >>> data = np.array([[1.0, 2.0, 3.0, 4.0, 5.0], 
        ...                  [6.0, 7.0, 8.0, 9.0, 10.0], 
        ...                  [11.0, 12.0, 13.0, 14.0, 15.0], 
        ...                  [16.0, 17.0, 18.0, 19.0, 20.0], 
        ...                  [21.0, 22.0, 23.0, 24.0, 25.0]])
        >>> vars_subset += data
        >>> vars_subset                                     # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Workspace: Variables
        nb variables: 5
        filename: ...fun.var
        description: Modèle fun - Simulation 1
        sample: 1991Y1:1995Y1
        mode: LEVEL
        <BLANKLINE>
         name       1991Y1  1992Y1  1993Y1  1994Y1  1995Y1
        ACAF         61.48   74.32   88.32   40.32    4.74
        ACAG        -39.87  -55.57  -58.32   -1.06  -45.69
        AOUC         39.05   42.06   45.06   48.09   57.10
        AOUC_        53.93   56.95   59.96   62.98   73.99
        AQC          69.13   72.22   75.31   78.31   91.32
        <BLANKLINE>
        """
        self.__binary_op__(other, BinaryOperation.OP_ADD, False)
        return self

    # self - other
    def __sub__(self, other):
        r"""
        Subtract `other` from the current (subset of) Variables object.

        Parameters
        ----------
        other: int, float, numpy ndarray, pandas Series, pandas DataFrame, larray Array or iode Variables
            If `other` is an int or a float, subtract the scalar from all values of the current (subset of) Variables object.
            If `other` is a numpy ndarray, the shape of the ndarray must be compatible with the current (subset of) 
            Variables object. Specifically, the number of rows must be equal to the number of variables and the number of 
            columns must be equal to the number of periods.
            If `other` is a pandas Series, it must represent either a single variable or a single period.
            If `other` is a pandas DataFrame, it must represent the same variables names and periods 
            as the current (subset of) Variables object. Specifically, the index of the DataFrame must be equal to the 
            variables names and the columns of the DataFrame must be equal to the periods.
            If `other` is an larray Array, its last axis must be equal to the periods and be named 'time'. 
            If the Array has more than two axes, the first n-1 axes are combined to form the variables names. 
            The first (combined) axis must be equal to the variables names.
            If `other` is an iode Variables object, it must share the same sample and represent the same 
            set of variables names as `self`.
        
        Returns
        -------
        Variables

        Warnings
        --------
        Subtracting a numpy ndarray to a Variables object is not recommended as there is no compatibility check 
        between for the names and periods. The result is not guaranteed to be the one you expected. 
        This possibility is provided for speed reasons (when the database or the subset is large).

        Examples
        --------
        >>> import numpy as np
        >>> import pandas as pd
        >>> import larray as la
        >>> from iode import SAMPLE_DATA_DIR
        >>> from iode import variables, NA, Sample
        >>> variables.load(f"{SAMPLE_DATA_DIR}/fun.var")        # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Loading .../fun.var
        394 objects loaded
        >>> vars_subset = variables["A*", "1991Y1:1995Y1"]
        >>> vars_subset.names
        ['ACAF', 'ACAG', 'AOUC', 'AOUC_', 'AQC']
        >>> vars_subset                                         # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Workspace: Variables
        nb variables: 5
        filename: ...fun.var
        description: Modèle fun - Simulation 1
        sample: 1991Y1:1995Y1
        mode: LEVEL
        <BLANKLINE>
         name       1991Y1  1992Y1  1993Y1  1994Y1  1995Y1
        ACAF         26.24   30.16   34.66    8.16  -13.13
        ACAG        -30.93  -40.29  -43.16  -16.03  -41.85
        AOUC          1.02    1.03    1.03    1.05    1.05
        AOUC_         0.96    0.97    0.98    0.99    1.00
        AQC           1.06    1.11    1.15    1.16    1.16
        <BLANKLINE>

        >>> # subtract a scalar from all values of a subset of a Variables object
        >>> new_vars_subset = vars_subset - 2.0
        >>> new_vars_subset                                     # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Workspace: Variables
        nb variables: 5
        filename: ...fun.var
        description: Modèle fun - Simulation 1
        sample: 1991Y1:1995Y1
        mode: LEVEL
        <BLANKLINE>
         name       1991Y1  1992Y1  1993Y1  1994Y1  1995Y1
        ACAF     	 24.24	 28.16	 32.66	  6.16	-15.13
        ACAG     	-32.93	-42.29	-45.16	-18.03	-43.85
        AOUC    	 -0.98	 -0.97	 -0.97	 -0.95	 -0.95
        AOUC_   	 -1.04	 -1.03	 -1.02	 -1.01	 -1.00
        AQC  	     -0.94	 -0.89	 -0.85	 -0.84	 -0.84
        <BLANKLINE>

        >>> # subtract (a subset of) a Variables object from another
        >>> new_vars_subset = vars_subset - vars_subset
        >>> new_vars_subset                                 # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Workspace: Variables
        nb variables: 5
        filename: ...fun.var
        description: Modèle fun - Simulation 1
        sample: 1991Y1:1995Y1
        mode: LEVEL
        <BLANKLINE>
         name       1991Y1  1992Y1  1993Y1  1994Y1  1995Y1
        ACAF 	      0.00	  0.00	  0.00	  0.00	  0.00
        ACAG 	      0.00	  0.00	  0.00	  0.00	  0.00
        AOUC 	      0.00	  0.00	  0.00	  0.00	  0.00
        AOUC_	      0.00	  0.00	  0.00	  0.00	  0.00
        AQC  	      0.00	  0.00	  0.00	  0.00	  0.00
        <BLANKLINE>

        >>> # subtract a pandas Series from a single variable
        >>> series = pd.Series([1.0, 2.0, 3.0, 4.0, 5.0], index=vars_subset.periods)
        >>> series                                              # doctest: +NORMALIZE_WHITESPACE
        1991Y1    1.0
        1992Y1    2.0
        1993Y1    3.0
        1994Y1    4.0
        1995Y1    5.0
        dtype: float64
        >>> updated_ACAF = vars_subset["ACAF"] - series
        >>> updated_ACAF                                        # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Workspace: Variables
        nb variables: 1
        filename: ...fun.var
        description: Modèle fun - Simulation 1
        sample: 1991Y1:1995Y1
        mode: LEVEL
        <BLANKLINE>
         name       1991Y1  1992Y1  1993Y1  1994Y1  1995Y1
        ACAF	     25.24	 28.16	 31.66	  4.16	-18.13
        <BLANKLINE>
        
        >>> # subtract a pandas Series from a subset corresponding to a single period
        >>> series = pd.Series([1.0, 2.0, 3.0, 4.0, 5.0], index=vars_subset.names)
        >>> series                                          # doctest: +NORMALIZE_WHITESPACE
        ACAF     1.0
        ACAG     2.0
        AOUC     3.0
        AOUC_    4.0
        AQC      5.0
        dtype: float64
        >>> vars_subset_1995Y1 = vars_subset[:, "1995Y1"] - series
        >>> vars_subset_1995Y1                              # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Workspace: Variables
        nb variables: 5
        filename: ...fun.var
        description: Modèle fun - Simulation 1
        sample: 1995Y1:1995Y1
        mode: LEVEL
        <BLANKLINE>
         name       1995Y1
        ACAF 	    -14.13
        ACAG     	-43.85
        AOUC 	     -1.95
        AOUC_	     -3.00
        AQC  	     -3.84
        <BLANKLINE>

        >>> # subtract a pandas DataFrame from the subset of a Variables object  
        >>> data = np.array([[1.0, 2.0, 3.0, 4.0, 5.0], 
        ...                  [6.0, 7.0, 8.0, 9.0, 10.0], 
        ...                  [11.0, 12.0, 13.0, 14.0, 15.0], 
        ...                  [16.0, 17.0, 18.0, 19.0, 20.0], 
        ...                  [21.0, 22.0, 23.0, 24.0, 25.0]],)
        >>> df = pd.DataFrame(data, index=vars_subset.names, columns=vars_subset.periods) 
        >>> df                                              # doctest: +NORMALIZE_WHITESPACE
               1991Y1  1992Y1  1993Y1  1994Y1  1995Y1
        ACAF      1.0     2.0     3.0     4.0     5.0
        ACAG      6.0     7.0     8.0     9.0    10.0
        AOUC     11.0    12.0    13.0    14.0    15.0
        AOUC_    16.0    17.0    18.0    19.0    20.0
        AQC      21.0    22.0    23.0    24.0    25.0
        >>> new_vars_subset = vars_subset - df
        >>> new_vars_subset                                 # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Workspace: Variables
        nb variables: 5
        filename: ...fun.var
        description: Modèle fun - Simulation 1
        sample: 1991Y1:1995Y1
        mode: LEVEL
        <BLANKLINE>
         name       1991Y1  1992Y1  1993Y1  1994Y1  1995Y1
        ACAF    	 25.24	 28.16	 31.66	  4.16	-18.13
        ACAG     	-36.93	-47.29	-51.16	-25.03	-51.85
        AOUC     	 -9.98	-10.97	-11.97	-12.95	-13.95
        AOUC_    	-15.04	-16.03	-17.02	-18.01	-19.00
        AQC     	-19.94	-20.89	-21.85	-22.84	-23.84
        <BLANKLINE>

        >>> # subtract an larray Array from a subset of a Variables object
        >>> axis_names = la.Axis(name="names", labels=vars_subset.names)
        >>> axis_time = la.Axis(name="time", labels=vars_subset.periods)
        >>> array = la.Array(data, axes=(axis_names, axis_time))
        >>> array                                           # doctest: +NORMALIZE_WHITESPACE
        names\time  1991Y1  1992Y1  1993Y1  1994Y1  1995Y1
              ACAF     1.0     2.0     3.0     4.0     5.0
              ACAG     6.0     7.0     8.0     9.0    10.0
              AOUC    11.0    12.0    13.0    14.0    15.0
             AOUC_    16.0    17.0    18.0    19.0    20.0
               AQC    21.0    22.0    23.0    24.0    25.0
        >>> new_vars_subset = vars_subset - array
        >>> new_vars_subset                                 # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Workspace: Variables
        nb variables: 5
        filename: ...fun.var
        description: Modèle fun - Simulation 1
        sample: 1991Y1:1995Y1
        mode: LEVEL
        <BLANKLINE>
         name       1991Y1  1992Y1  1993Y1  1994Y1  1995Y1
        ACAF     	 25.24	 28.16	 31.66	  4.16	-18.13
        ACAG     	-36.93	-47.29	-51.16	-25.03	-51.85
        AOUC     	 -9.98	-10.97	-11.97	-12.95	-13.95
        AOUC_   	-15.04	-16.03	-17.02	-18.01	-19.00
        AQC      	-19.94	-20.89	-21.85	-22.84	-23.84
        <BLANKLINE>        

        >>> # WARNING: subtracting a numpy ndarray from a (subset of a) Variables object is not recommended 
        >>> #          as there is no compatibility check between for the names and periods.
        >>> #          The result is not guaranteed to be the one you expected.
        >>> #          This possibility is provided for speed reasons 
        >>> #          (when dealing with large subsets/databases).
        >>> # subtract a numpy 1D ndarray from a single variable
        >>> data = np.array([1.0, 2.0, 3.0, 4.0, 5.0])
        >>> updated_ACAF = vars_subset["ACAF"] - data
        >>> updated_ACAF                                    # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Workspace: Variables
        nb variables: 1
        filename: ...fun.var
        description: Modèle fun - Simulation 1
        sample: 1991Y1:1995Y1
        mode: LEVEL
        <BLANKLINE>
         name       1991Y1  1992Y1  1993Y1  1994Y1  1995Y1
        ACAF	     25.24	 28.16	 31.66	  4.16	-18.13
        <BLANKLINE>
        >>> # subtract a numpy 1D ndarray from the subset corresponding to a single period
        >>> vars_subset_1995Y1 = vars_subset[:, "1995Y1"] - data
        >>> vars_subset_1995Y1                              # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Workspace: Variables
        nb variables: 5
        filename: ...fun.var
        description: Modèle fun - Simulation 1
        sample: 1995Y1:1995Y1
        mode: LEVEL
        <BLANKLINE>
         name       1995Y1
        ACAF 	    -14.13
        ACAG     	-43.85
        AOUC     	 -1.95
        AOUC_   	 -3.00
        AQC      	 -3.84
        <BLANKLINE>        
        >>> # subtract a numpy 2D ndarray from a (subset of a) Variables object
        >>> data = np.array([[1.0, 2.0, 3.0, 4.0, 5.0], 
        ...                  [6.0, 7.0, 8.0, 9.0, 10.0], 
        ...                  [11.0, 12.0, 13.0, 14.0, 15.0], 
        ...                  [16.0, 17.0, 18.0, 19.0, 20.0], 
        ...                  [21.0, 22.0, 23.0, 24.0, 25.0]])
        >>> new_vars_subset = vars_subset - data
        >>> new_vars_subset                                 # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Workspace: Variables
        nb variables: 5
        filename: ...fun.var
        description: Modèle fun - Simulation 1
        sample: 1991Y1:1995Y1
        mode: LEVEL
        <BLANKLINE>
         name       1991Y1  1992Y1  1993Y1  1994Y1  1995Y1
        ACAF    	 25.24	 28.16	 31.66	  4.16	-18.13
        ACAG    	-36.93	-47.29	-51.16	-25.03	-51.85
        AOUC    	 -9.98	-10.97	-11.97	-12.95	-13.95
        AOUC_    	-15.04	-16.03	-17.02	-18.01	-19.00
        AQC  	    -19.94	-20.89	-21.85	-22.84	-23.84
        <BLANKLINE>
        """
        return self.__binary_op__(other, BinaryOperation.OP_SUB, True)

    # other - self
    def __rsub__(self, other):
        r"""
        subtract `self` from `other`.

        Parameters
        ----------
        other: iode Variables
            `self` and `other` must share the same sample and represent 
            the same set of variables names.
        
        Returns
        -------
        Variables

        Examples
        --------
        >>> import numpy as np
        >>> import pandas as pd
        >>> import larray as la
        >>> from iode import SAMPLE_DATA_DIR
        >>> from iode import variables, NA, Sample
        >>> variables.load(f"{SAMPLE_DATA_DIR}/fun.var")        # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Loading .../fun.var
        394 objects loaded
        >>> vars_subset = variables["A*", "1991Y1:1995Y1"]
        >>> vars_subset.names
        ['ACAF', 'ACAG', 'AOUC', 'AOUC_', 'AQC']
        >>> vars_subset                                         # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Workspace: Variables
        nb variables: 5
        filename: ...fun.var
        description: Modèle fun - Simulation 1
        sample: 1991Y1:1995Y1
        mode: LEVEL
        <BLANKLINE>
         name       1991Y1  1992Y1  1993Y1  1994Y1  1995Y1
        ACAF         26.24   30.16   34.66    8.16  -13.13
        ACAG        -30.93  -40.29  -43.16  -16.03  -41.85
        AOUC          1.02    1.03    1.03    1.05    1.05
        AOUC_         0.96    0.97    0.98    0.99    1.00
        AQC           1.06    1.11    1.15    1.16    1.16
        <BLANKLINE>

        >>> # subtract a (subset of) Variables object from another
        >>> new_vars_subset = vars_subset - vars_subset
        >>> new_vars_subset                                 # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Workspace: Variables
        nb variables: 5
        filename: ...fun.var
        description: Modèle fun - Simulation 1
        sample: 1991Y1:1995Y1
        mode: LEVEL
        <BLANKLINE>
         name       1991Y1  1992Y1  1993Y1  1994Y1  1995Y1
        ACAF 	      0.00	  0.00	  0.00	  0.00	  0.00
        ACAG     	  0.00	  0.00	  0.00	  0.00	  0.00
        AOUC     	  0.00	  0.00	  0.00	  0.00	  0.00
        AOUC_	      0.00	  0.00	  0.00	  0.00	  0.00
        AQC  	      0.00	  0.00	  0.00	  0.00	  0.00
        <BLANKLINE>
        """
        if isinstance(other, Variables):
            return other.__binary_op__(self, BinaryOperation.OP_SUB, True)
        else:
            raise TypeError(f"unsupported operand type for 'X' in the arithmetic operation 'X - Y'.\n"
                            f"The only accepted type for 'X' is 'Variables'.\nGot 'X' of type {type(other).__name__} instead")

    # self -= other
    def __isub__(self, other):
        r"""
        subtract `other` from the current (subset of) Variables object.

        Parameters
        ----------
        other: int, float, numpy ndarray, pandas Series, pandas DataFrame, larray Array or iode Variables
            If `other` is an int or a float, subtract the scalar from all values of the current (subset of) Variables object.
            If `other` is a numpy ndarray, the shape of the ndarray must be compatible with the current (subset of) 
            Variables object. Specifically, the number of rows must be equal to the number of variables and the number of 
            columns must be equal to the number of periods.
            If `other` is a pandas Series, it must represent either a single variable or a single period.
            If `other` is a pandas DataFrame, it must represent the same variables names and periods 
            as the current (subset of) Variables object. Specifically, the index of the DataFrame must be equal to the 
            variables names and the columns of the DataFrame must be equal to the periods.
            If `other` is an larray Array, its last axis must be equal to the periods and be named 'time'. 
            If the Array has more than two axes, the first n-1 axes are combined to form the variables names. 
            The first (combined) axis must be equal to the variables names.
            If `other` is an iode Variables object, it must share the same sample and represent the same 
            set of variables names as `self`.
        Warnings
        --------
        Subtracting a numpy ndarray to a Variables object is not recommended as there is no compatibility check 
        between for the names and periods. The result is not guaranteed to be the one you expected. 
        This possibility is provided for speed reasons (when the database or the subset is large).

        Examples
        --------
        >>> import numpy as np
        >>> import pandas as pd
        >>> import larray as la
        >>> from iode import SAMPLE_DATA_DIR
        >>> from iode import variables, NA, Sample
        >>> variables.load(f"{SAMPLE_DATA_DIR}/fun.var")        # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Loading .../fun.var
        394 objects loaded
        >>> vars_subset = variables["A*", "1991Y1:1995Y1"]
        >>> vars_subset.names
        ['ACAF', 'ACAG', 'AOUC', 'AOUC_', 'AQC']
        >>> vars_subset                                         # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Workspace: Variables
        nb variables: 5
        filename: ...fun.var
        description: Modèle fun - Simulation 1
        sample: 1991Y1:1995Y1
        mode: LEVEL
        <BLANKLINE>
         name       1991Y1  1992Y1  1993Y1  1994Y1  1995Y1
        ACAF         26.24   30.16   34.66    8.16  -13.13
        ACAG        -30.93  -40.29  -43.16  -16.03  -41.85
        AOUC          1.02    1.03    1.03    1.05    1.05
        AOUC_         0.96    0.97    0.98    0.99    1.00
        AQC           1.06    1.11    1.15    1.16    1.16
        <BLANKLINE>

        >>> # subtract a scalar from all values of the current subset of a Variables object
        >>> vars_subset -= 2.0
        >>> vars_subset                                         # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Workspace: Variables
        nb variables: 5
        filename: ...fun.var
        description: Modèle fun - Simulation 1
        sample: 1991Y1:1995Y1
        mode: LEVEL
        <BLANKLINE>
         name       1991Y1  1992Y1  1993Y1  1994Y1  1995Y1
        ACAF     	 24.24	 28.16	 32.66	  6.16	-15.13
        ACAG     	-32.93	-42.29	-45.16	-18.03	-43.85
        AOUC    	 -0.98	 -0.97	 -0.97	 -0.95	 -0.95
        AOUC_   	 -1.04	 -1.03	 -1.02	 -1.01	 -1.00
        AQC  	     -0.94	 -0.89	 -0.85	 -0.84	 -0.84
        <BLANKLINE>

        >>> # subtract a (subsets of) a Variables object from the current one
        >>> vars_subset -= vars_subset
        >>> vars_subset                                 # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Workspace: Variables
        nb variables: 5
        filename: ...fun.var
        description: Modèle fun - Simulation 1
        sample: 1991Y1:1995Y1
        mode: LEVEL
        <BLANKLINE>
         name       1991Y1  1992Y1  1993Y1  1994Y1  1995Y1
        ACAF 	      0.00	  0.00	  0.00	  0.00	  0.00
        ACAG 	      0.00	  0.00	  0.00	  0.00	  0.00
        AOUC 	      0.00	  0.00	  0.00	  0.00	  0.00
        AOUC_	      0.00	  0.00	  0.00	  0.00	  0.00
        AQC  	      0.00	  0.00	  0.00	  0.00	  0.00
        <BLANKLINE>

        >>> # subtract a pandas Series from a single variable
        >>> series = pd.Series([1.0, 2.0, 3.0, 4.0, 5.0], index=vars_subset.periods)
        >>> series                                              # doctest: +NORMALIZE_WHITESPACE
        1991Y1    1.0
        1992Y1    2.0
        1993Y1    3.0
        1994Y1    4.0
        1995Y1    5.0
        dtype: float64
        >>> vars_subset["ACAF"] -= series
        >>> vars_subset["ACAF"]                                 # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Workspace: Variables
        nb variables: 1
        filename: ...fun.var
        description: Modèle fun - Simulation 1
        sample: 1991Y1:1995Y1
        mode: LEVEL
        <BLANKLINE>
        name        1991Y1  1992Y1  1993Y1  1994Y1  1995Y1
        ACAF         -1.00   -2.00   -3.00   -4.00   -5.00
        <BLANKLINE>
        
        >>> # subtract a pandas Series from the subset corresponding to a single period
        >>> series = pd.Series([1.0, 2.0, 3.0, 4.0, 5.0], index=vars_subset.names)
        >>> series                                          # doctest: +NORMALIZE_WHITESPACE
        ACAF     1.0
        ACAG     2.0
        AOUC     3.0
        AOUC_    4.0
        AQC      5.0
        dtype: float64
        >>> vars_subset[:, "1995Y1"] -= series
        >>> vars_subset[:, "1995Y1"]                        # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Workspace: Variables
        nb variables: 5
        filename: ...fun.var
        description: Modèle fun - Simulation 1
        sample: 1995Y1:1995Y1
        mode: LEVEL
        <BLANKLINE>
         name       1995Y1
        ACAF         -6.00
        ACAG         -2.00
        AOUC         -3.00
        AOUC_        -4.00
        AQC          -5.00
        <BLANKLINE>

        >>> # subtract a pandas DataFrame from the current subset of the Variables object  
        >>> data = np.array([[1.0, 2.0, 3.0, 4.0, 5.0], 
        ...                  [6.0, 7.0, 8.0, 9.0, 10.0], 
        ...                  [11.0, 12.0, 13.0, 14.0, 15.0], 
        ...                  [16.0, 17.0, 18.0, 19.0, 20.0], 
        ...                  [21.0, 22.0, 23.0, 24.0, 25.0]],)
        >>> df = pd.DataFrame(data, index=vars_subset.names, columns=vars_subset.periods) 
        >>> df                                              # doctest: +NORMALIZE_WHITESPACE
               1991Y1  1992Y1  1993Y1  1994Y1  1995Y1
        ACAF      1.0     2.0     3.0     4.0     5.0
        ACAG      6.0     7.0     8.0     9.0    10.0
        AOUC     11.0    12.0    13.0    14.0    15.0
        AOUC_    16.0    17.0    18.0    19.0    20.0
        AQC      21.0    22.0    23.0    24.0    25.0
        >>> vars_subset -= df
        >>> vars_subset                                     # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Workspace: Variables
        nb variables: 5
        filename: ...fun.var
        description: Modèle fun - Simulation 1
        sample: 1991Y1:1995Y1
        mode: LEVEL
        <BLANKLINE>
         name       1991Y1  1992Y1  1993Y1  1994Y1  1995Y1
        ACAF         -2.00   -4.00   -6.00   -8.00  -11.00
        ACAG         -6.00   -7.00   -8.00   -9.00  -12.00
        AOUC        -11.00  -12.00  -13.00  -14.00  -18.00
        AOUC_       -16.00  -17.00  -18.00  -19.00  -24.00
        AQC         -21.00  -22.00  -23.00  -24.00  -30.00
        <BLANKLINE>

        >>> # subtract an larray Array from the current subset of the Variables object
        >>> axis_names = la.Axis(name="names", labels=vars_subset.names)
        >>> axis_time = la.Axis(name="time", labels=vars_subset.periods)
        >>> array = la.Array(data, axes=(axis_names, axis_time))
        >>> array                                           # doctest: +NORMALIZE_WHITESPACE
        names\time  1991Y1  1992Y1  1993Y1  1994Y1  1995Y1
              ACAF     1.0     2.0     3.0     4.0     5.0
              ACAG     6.0     7.0     8.0     9.0    10.0
              AOUC    11.0    12.0    13.0    14.0    15.0
             AOUC_    16.0    17.0    18.0    19.0    20.0
               AQC    21.0    22.0    23.0    24.0    25.0
        >>> vars_subset -= array
        >>> vars_subset                                     # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Workspace: Variables
        nb variables: 5
        filename: ...fun.var
        description: Modèle fun - Simulation 1
        sample: 1991Y1:1995Y1
        mode: LEVEL
        <BLANKLINE>
         name       1991Y1  1992Y1  1993Y1  1994Y1  1995Y1
        ACAF         -3.00   -6.00   -9.00  -12.00  -16.00
        ACAG        -12.00  -14.00  -16.00  -18.00  -22.00
        AOUC        -22.00  -24.00  -26.00  -28.00  -33.00
        AOUC_       -32.00  -34.00  -36.00  -38.00  -44.00
        AQC         -42.00  -44.00  -46.00  -48.00  -55.00
        <BLANKLINE>   

        >>> # WARNING: subtracting a numpy ndarray to a (subset of a) Variables object is not recommended 
        >>> #          as there is no compatibility check between for the names and periods.
        >>> #          The result is not guaranteed to be the one you expected.
        >>> #          This possibility is provided for speed reasons 
        >>> #          (when dealing with large subsets/databases).
        >>> # subtract a numpy 1D ndarray from a single variable
        >>> data = np.array([1.0, 2.0, 3.0, 4.0, 5.0])
        >>> vars_subset["ACAF"] -= data
        >>> vars_subset["ACAF"]                             # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Workspace: Variables
        nb variables: 1
        filename: ...fun.var
        description: Modèle fun - Simulation 1
        sample: 1991Y1:1995Y1
        mode: LEVEL
        <BLANKLINE>
        name        1991Y1  1992Y1  1993Y1  1994Y1  1995Y1
        ACAF         -4.00   -8.00  -12.00  -16.00  -21.00
        <BLANKLINE>
        >>> # subtract a numpy 1D ndarray from the subset corresponding to a single period
        >>> vars_subset[:, "1995Y1"] -= data
        >>> vars_subset[:, "1995Y1"]                        # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Workspace: Variables
        nb variables: 5
        filename: ...fun.var
        description: Modèle fun - Simulation 1
        sample: 1995Y1:1995Y1
        mode: LEVEL
        <BLANKLINE>
         name       1995Y1
        ACAF        -22.00
        ACAG        -24.00
        AOUC        -36.00
        AOUC_       -48.00
        AQC         -60.00
        <BLANKLINE>        
        >>> # subtract a numpy 2D ndarray from the current (subset of the) Variables object
        >>> data = np.array([[1.0, 2.0, 3.0, 4.0, 5.0], 
        ...                  [6.0, 7.0, 8.0, 9.0, 10.0], 
        ...                  [11.0, 12.0, 13.0, 14.0, 15.0], 
        ...                  [16.0, 17.0, 18.0, 19.0, 20.0], 
        ...                  [21.0, 22.0, 23.0, 24.0, 25.0]])
        >>> vars_subset -= data
        >>> vars_subset                                     # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Workspace: Variables
        nb variables: 5
        filename: ...fun.var
        description: Modèle fun - Simulation 1
        sample: 1991Y1:1995Y1
        mode: LEVEL
        <BLANKLINE>
         name       1991Y1  1992Y1  1993Y1  1994Y1  1995Y1
        ACAF         -5.00  -10.00  -15.00  -20.00  -27.00
        ACAG        -18.00  -21.00  -24.00  -27.00  -34.00
        AOUC        -33.00  -36.00  -39.00  -42.00  -51.00
        AOUC_       -48.00  -51.00  -54.00  -57.00  -68.00
        AQC         -63.00  -66.00  -69.00  -72.00  -85.00
        <BLANKLINE>
        """
        self.__binary_op__(other, BinaryOperation.OP_SUB, False)
        return self

    # self * other
    def __mul__(self, other):
        r"""
        multiply the current (subset of) Variables object by `other`.

        Parameters
        ----------
        other: int, float, numpy ndarray, pandas Series, pandas DataFrame, larray Array or iode Variables
            If `other` is an int or a float, multiply all values of the current (subset of) Variables object the scalar.
            If `other` is a numpy ndarray, the shape of the ndarray must be compatible with the current (subset of) 
            Variables object. Specifically, the number of rows must be equal to the number of variables and the number of 
            columns must be equal to the number of periods.
            If `other` is a pandas Series, it must represent either a single variable or a single period.
            If `other` is a pandas DataFrame, it must represent the same variables names and periods 
            as the current (subset of) Variables object. Specifically, the index of the DataFrame must be equal to the 
            variables names and the columns of the DataFrame must be equal to the periods.
            If `other` is an larray Array, its last axis must be equal to the periods and be named 'time'. 
            If the Array has more than two axes, the first n-1 axes are combined to form the variables names. 
            The first (combined) axis must be equal to the variables names.
            If `other` is an iode Variables object, it must share the same sample and represent the same 
            set of variables names as `self`.        
        Returns
        -------
        Variables

        Warnings
        --------
        Multiplying a numpy ndarray to a Variables object is not recommended as there is no compatibility check 
        between for the names and periods. The result is not guaranteed to be the one you expected. 
        This possibility is provided for speed reasons (when the database or the subset is large).

        Examples
        --------
        >>> import numpy as np
        >>> import pandas as pd
        >>> import larray as la
        >>> from iode import SAMPLE_DATA_DIR
        >>> from iode import variables, NA, Sample
        >>> variables.load(f"{SAMPLE_DATA_DIR}/fun.var")        # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Loading .../fun.var
        394 objects loaded
        >>> vars_subset = variables["A*", "1991Y1:1995Y1"]
        >>> vars_subset.names
        ['ACAF', 'ACAG', 'AOUC', 'AOUC_', 'AQC']
        >>> vars_subset                                         # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Workspace: Variables
        nb variables: 5
        filename: ...fun.var
        description: Modèle fun - Simulation 1
        sample: 1991Y1:1995Y1
        mode: LEVEL
        <BLANKLINE>
         name       1991Y1  1992Y1  1993Y1  1994Y1  1995Y1
        ACAF         26.24   30.16   34.66    8.16  -13.13
        ACAG        -30.93  -40.29  -43.16  -16.03  -41.85
        AOUC          1.02    1.03    1.03    1.05    1.05
        AOUC_         0.96    0.97    0.98    0.99    1.00
        AQC           1.06    1.11    1.15    1.16    1.16
        <BLANKLINE>

        >>> # multiply all values of a subset of a Variables object by a scalar
        >>> new_vars_subset = vars_subset * 2.0
        >>> new_vars_subset                                     # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Workspace: Variables
        nb variables: 5
        filename: ...fun.var
        description: Modèle fun - Simulation 1
        sample: 1991Y1:1995Y1
        mode: LEVEL
        <BLANKLINE>
         name       1991Y1  1992Y1  1993Y1  1994Y1  1995Y1
        ACAF    	 52.48	 60.32	 69.32	 16.32	-26.26
        ACAG     	-61.87	-80.57	-86.32	-32.06	-83.69
        AOUC 	      2.05	  2.06	  2.06	  2.09	  2.10
        AOUC_	      1.93	  1.95	  1.96	  1.98	  1.99
        AQC  	      2.13	  2.22	  2.31	  2.31	  2.32
        <BLANKLINE>

        >>> # multiply two (subsets of) a Variables object
        >>> new_vars_subset = vars_subset * vars_subset
        >>> new_vars_subset                                 # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Workspace: Variables
        nb variables: 5
        filename: ...fun.var
        description: Modèle fun - Simulation 1
        sample: 1991Y1:1995Y1
        mode: LEVEL
        <BLANKLINE>
         name       1991Y1   1992Y1   1993Y1  1994Y1   1995Y1
        ACAF     	688.59	 909.57	 1201.45   66.60   172.42
        ACAG     	956.91	1622.96	 1862.61  256.93  1751.09
        AOUC 	      1.05	   1.06	    1.06	1.09	 1.10
        AOUC_	      0.93	   0.95	    0.96	0.98	 0.99
        AQC  	      1.13	   1.23	    1.33	1.34	 1.35
        <BLANKLINE>

        >>> # multiply a single variable by a pandas Series
        >>> series = pd.Series([1.0, 2.0, 3.0, 4.0, 5.0], index=vars_subset.periods)
        >>> series                                              # doctest: +NORMALIZE_WHITESPACE
        1991Y1    1.0
        1992Y1    2.0
        1993Y1    3.0
        1994Y1    4.0
        1995Y1    5.0
        dtype: float64
        >>> updated_ACAF = vars_subset["ACAF"] * series
        >>> updated_ACAF                                        # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Workspace: Variables
        nb variables: 1
        filename: ...fun.var
        description: Modèle fun - Simulation 1
        sample: 1991Y1:1995Y1
        mode: LEVEL
        <BLANKLINE>
         name       1991Y1  1992Y1  1993Y1  1994Y1  1995Y1
        ACAF	     26.24	 60.32	103.99	 32.64	-65.65
        <BLANKLINE>
        
        >>> # multiply a subset corresponding to a single period by a pandas Series
        >>> series = pd.Series([1.0, 2.0, 3.0, 4.0, 5.0], index=vars_subset.names)
        >>> series                                          # doctest: +NORMALIZE_WHITESPACE
        ACAF     1.0
        ACAG     2.0
        AOUC     3.0
        AOUC_    4.0
        AQC      5.0
        dtype: float64
        >>> vars_subset_1995Y1 = vars_subset[:, "1995Y1"] * series
        >>> vars_subset_1995Y1                              # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Workspace: Variables
        nb variables: 5
        filename: ...fun.var
        description: Modèle fun - Simulation 1
        sample: 1995Y1:1995Y1
        mode: LEVEL
        <BLANKLINE>
         name    1995Y1
        ACAF 	 -13.13
        ACAG 	 -83.69
        AOUC 	   3.15
        AOUC_	   3.98
        AQC  	   5.81
        <BLANKLINE>

        >>> # multiply a subset of a Variables object by a pandas DataFrame  
        >>> data = np.array([[1.0, 2.0, 3.0, 4.0, 5.0], 
        ...                  [6.0, 7.0, 8.0, 9.0, 10.0], 
        ...                  [11.0, 12.0, 13.0, 14.0, 15.0], 
        ...                  [16.0, 17.0, 18.0, 19.0, 20.0], 
        ...                  [21.0, 22.0, 23.0, 24.0, 25.0]],)
        >>> df = pd.DataFrame(data, index=vars_subset.names, columns=vars_subset.periods) 
        >>> df                                              # doctest: +NORMALIZE_WHITESPACE
               1991Y1  1992Y1  1993Y1  1994Y1  1995Y1
        ACAF      1.0     2.0     3.0     4.0     5.0
        ACAG      6.0     7.0     8.0     9.0    10.0
        AOUC     11.0    12.0    13.0    14.0    15.0
        AOUC_    16.0    17.0    18.0    19.0    20.0
        AQC      21.0    22.0    23.0    24.0    25.0
        >>> new_vars_subset = vars_subset * df
        >>> new_vars_subset                                 # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Workspace: Variables
        nb variables: 5
        filename: ...fun.var
        description: Modèle fun - Simulation 1
        sample: 1991Y1:1995Y1
        mode: LEVEL
        <BLANKLINE>
         name        1991Y1  1992Y1  1993Y1  1994Y1  1995Y1
        ACAF          26.24   60.32  103.99   32.64  -65.65
        ACAG        -185.60 -282.00 -345.26 -144.26 -418.46
        AOUC          11.27   12.38   13.40   14.65   15.75
        AOUC_         15.43   16.56   17.62   18.80   19.91
        AQC           22.32   24.43   26.53   27.77   29.04
        <BLANKLINE>

        >>> # multiply a subset of a Variables object by an larray Array
        >>> axis_names = la.Axis(name="names", labels=vars_subset.names)
        >>> axis_time = la.Axis(name="time", labels=vars_subset.periods)
        >>> array = la.Array(data, axes=(axis_names, axis_time))
        >>> array                                           # doctest: +NORMALIZE_WHITESPACE
        names\time  1991Y1  1992Y1  1993Y1  1994Y1  1995Y1
              ACAF     1.0     2.0     3.0     4.0     5.0
              ACAG     6.0     7.0     8.0     9.0    10.0
              AOUC    11.0    12.0    13.0    14.0    15.0
             AOUC_    16.0    17.0    18.0    19.0    20.0
               AQC    21.0    22.0    23.0    24.0    25.0
        >>> new_vars_subset = vars_subset * array
        >>> new_vars_subset                                 # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Workspace: Variables
        nb variables: 5
        filename: ...fun.var
        description: Modèle fun - Simulation 1
        sample: 1991Y1:1995Y1
        mode: LEVEL
        <BLANKLINE>
         name        1991Y1  1992Y1  1993Y1  1994Y1  1995Y1
        ACAF          26.24   60.32  103.99   32.64  -65.65
        ACAG        -185.60 -282.00 -345.26 -144.26 -418.46
        AOUC          11.27   12.38   13.40   14.65   15.75
        AOUC_         15.43   16.56   17.62   18.80   19.91
        AQC           22.32   24.43   26.53   27.77   29.04
        <BLANKLINE>     

        >>> # WARNING: multiplying a numpy ndarray to a (subset of a) Variables object is not recommended 
        >>> #          as there is no compatibility check between for the names and periods.
        >>> #          The result is not guaranteed to be the one you expected.
        >>> #          This possibility is provided for speed reasons 
        >>> #          (when dealing with large subsets/databases).
        >>> # multiply a single variable by a numpy 1D ndarray
        >>> data = np.array([1.0, 2.0, 3.0, 4.0, 5.0])
        >>> updated_ACAF = vars_subset["ACAF"] * data
        >>> updated_ACAF                                    # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Workspace: Variables
        nb variables: 1
        filename: ...fun.var
        description: Modèle fun - Simulation 1
        sample: 1991Y1:1995Y1
        mode: LEVEL
        <BLANKLINE>
         name       1991Y1  1992Y1  1993Y1  1994Y1  1995Y1
        ACAF	     26.24	 60.32	103.99	 32.64	-65.65
        <BLANKLINE>
        >>> # multiply the subset corresponding to a single period by a numpy 1D ndarray
        >>> vars_subset_1995Y1 = vars_subset[:, "1995Y1"] * data
        >>> vars_subset_1995Y1                              # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Workspace: Variables
        nb variables: 5
        filename: ...fun.var
        description: Modèle fun - Simulation 1
        sample: 1995Y1:1995Y1
        mode: LEVEL
        <BLANKLINE>
         name   1995Y1
        ACAF 	-13.13
        ACAG 	-83.69
        AOUC 	  3.15
        AOUC_	  3.98
        AQC  	  5.81
        <BLANKLINE>       
        >>> # multiply a (subset of a) Variables object by a numpy 2D ndarray
        >>> data = np.array([[1.0, 2.0, 3.0, 4.0, 5.0], 
        ...                  [6.0, 7.0, 8.0, 9.0, 10.0], 
        ...                  [11.0, 12.0, 13.0, 14.0, 15.0], 
        ...                  [16.0, 17.0, 18.0, 19.0, 20.0], 
        ...                  [21.0, 22.0, 23.0, 24.0, 25.0]])
        >>> new_vars_subset = vars_subset * data
        >>> new_vars_subset                                 # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Workspace: Variables
        nb variables: 5
        filename: ...fun.var
        description: Modèle fun - Simulation 1
        sample: 1991Y1:1995Y1
        mode: LEVEL
        <BLANKLINE>
         name        1991Y1  1992Y1  1993Y1  1994Y1  1995Y1
        ACAF          26.24   60.32  103.99   32.64  -65.65
        ACAG        -185.60 -282.00 -345.26 -144.26 -418.46
        AOUC          11.27   12.38   13.40   14.65   15.75
        AOUC_         15.43   16.56   17.62   18.80   19.91
        AQC           22.32   24.43   26.53   27.77   29.04
        <BLANKLINE>
        """
        return self.__binary_op__(other, BinaryOperation.OP_MUL, True)
    
    # other * self
    def __rmul__(self, other):
        r"""
        multiply `other` by the current (subset of) Variables object.

        Parameters
        ----------
        other: int, float, numpy ndarray, larray Array or iode Variables
            If `other` is an int or a float, multiply all values of the current (subset of) Variables object by the scalar.
            If `other` is a numpy ndarray, the shape of the ndarray must be compatible with the current (subset of) 
            Variables object. Specifically, the number of rows must be equal to the number of variables and the number of 
            columns must be equal to the number of periods.
            If `other` is an larray Array, its last axis must be equal to the periods and be named 'time'. 
            If the Array has more than two axes, the first n-1 axes are combined to form the variables names. 
            The first (combined) axis must be equal to the variables names.
            If `other` is an iode Variables object, it must share the same sample and represent the same 
            set of variables names as `self`.        
        Returns
        -------
        Variables

        Warnings
        --------
        Multiplying a numpy ndarray to a Variables object is not recommended as there is no compatibility check 
        between for the names and periods. The result is not guaranteed to be the one you expected. 
        This possibility is provided for speed reasons (when the database or the subset is large).

        Examples
        --------
        >>> import numpy as np
        >>> import pandas as pd
        >>> import larray as la
        >>> from iode import SAMPLE_DATA_DIR
        >>> from iode import variables, NA, Sample
        >>> variables.load(f"{SAMPLE_DATA_DIR}/fun.var")        # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Loading .../fun.var
        394 objects loaded
        >>> vars_subset = variables["A*", "1991Y1:1995Y1"]
        >>> vars_subset.names
        ['ACAF', 'ACAG', 'AOUC', 'AOUC_', 'AQC']
        >>> vars_subset                                         # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Workspace: Variables
        nb variables: 5
        filename: ...fun.var
        description: Modèle fun - Simulation 1
        sample: 1991Y1:1995Y1
        mode: LEVEL
        <BLANKLINE>
         name       1991Y1  1992Y1  1993Y1  1994Y1  1995Y1
        ACAF         26.24   30.16   34.66    8.16  -13.13
        ACAG        -30.93  -40.29  -43.16  -16.03  -41.85
        AOUC          1.02    1.03    1.03    1.05    1.05
        AOUC_         0.96    0.97    0.98    0.99    1.00
        AQC           1.06    1.11    1.15    1.16    1.16
        <BLANKLINE>

        >>> # multiply all values of a subset of a Variables object by a scalar
        >>> new_vars_subset = 2.0 * vars_subset
        >>> new_vars_subset                                     # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Workspace: Variables
        nb variables: 5
        filename: ...fun.var
        description: Modèle fun - Simulation 1
        sample: 1991Y1:1995Y1
        mode: LEVEL
        <BLANKLINE>
         name       1991Y1  1992Y1  1993Y1  1994Y1  1995Y1
        ACAF    	 52.48	 60.32	 69.32	 16.32	-26.26
        ACAG     	-61.87	-80.57	-86.32	-32.06	-83.69
        AOUC 	      2.05	  2.06	  2.06	  2.09	  2.10
        AOUC_	      1.93	  1.95	  1.96	  1.98	  1.99
        AQC  	      2.13	  2.22	  2.31	  2.31	  2.32
        <BLANKLINE>

        >>> # multiply two (subsets of) a Variables object
        >>> new_vars_subset = vars_subset * vars_subset
        >>> new_vars_subset                                 # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Workspace: Variables
        nb variables: 5
        filename: ...fun.var
        description: Modèle fun - Simulation 1
        sample: 1991Y1:1995Y1
        mode: LEVEL
        <BLANKLINE>
         name       1991Y1  1992Y1  1993Y1  1994Y1  1995Y1
        ACAF     	688.59	 909.57	 1201.45   66.60   172.42
        ACAG     	956.91	1622.96	 1862.61  256.93  1751.09
        AOUC 	      1.05	   1.06	    1.06	1.09	 1.10
        AOUC_	      0.93	   0.95	    0.96	0.98	 0.99
        AQC  	      1.13	   1.23	    1.33	1.34	 1.35
        <BLANKLINE>

        >>> # multiply a subset of a Variables object by an larray Array
        >>> data = np.array([[1.0, 2.0, 3.0, 4.0, 5.0], 
        ...                  [6.0, 7.0, 8.0, 9.0, 10.0], 
        ...                  [11.0, 12.0, 13.0, 14.0, 15.0], 
        ...                  [16.0, 17.0, 18.0, 19.0, 20.0], 
        ...                  [21.0, 22.0, 23.0, 24.0, 25.0]])
        >>> axis_names = la.Axis(name="names", labels=vars_subset.names)
        >>> axis_time = la.Axis(name="time", labels=vars_subset.periods)
        >>> array = la.Array(data, axes=(axis_names, axis_time))
        >>> array                                           # doctest: +NORMALIZE_WHITESPACE
        names\time  1991Y1  1992Y1  1993Y1  1994Y1  1995Y1
              ACAF     1.0     2.0     3.0     4.0     5.0
              ACAG     6.0     7.0     8.0     9.0    10.0
              AOUC    11.0    12.0    13.0    14.0    15.0
             AOUC_    16.0    17.0    18.0    19.0    20.0
               AQC    21.0    22.0    23.0    24.0    25.0
        >>> new_vars_subset = array * vars_subset
        >>> new_vars_subset                                 # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Workspace: Variables
        nb variables: 5
        filename: ...fun.var
        description: Modèle fun - Simulation 1
        sample: 1991Y1:1995Y1
        mode: LEVEL
        <BLANKLINE>
         name        1991Y1  1992Y1  1993Y1  1994Y1  1995Y1
        ACAF          26.24   60.32  103.99   32.64  -65.65
        ACAG        -185.60 -282.00 -345.26 -144.26 -418.46
        AOUC          11.27   12.38   13.40   14.65   15.75
        AOUC_         15.43   16.56   17.62   18.80   19.91
        AQC           22.32   24.43   26.53   27.77   29.04
        <BLANKLINE>   

        >>> # WARNING: multiplying a numpy ndarray to a (subset of a) Variables object is not recommended 
        >>> #          as there is no compatibility check between for the names and periods.
        >>> #          The result is not guaranteed to be the one you expected.
        >>> #          This possibility is provided for speed reasons 
        >>> #          (when dealing with large subsets/databases).
        >>> # multiply a single variable by a numpy 1D ndarray
        >>> data = np.array([1.0, 2.0, 3.0, 4.0, 5.0])
        >>> result = data + vars_subset["ACAF"]
        >>> result                                      # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        array([ 27.240999 ,  32.159    ,  37.661999 ,  12.1610022,  -8.130997 ])
        >>> # multiply the subset corresponding to a single period by a numpy 1D ndarray
        >>> vars_subset_1995Y1 = data * vars_subset[:, "1995Y1"]
        >>> vars_subset_1995Y1                              # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        array([[ -13.130997  ,  -26.261994  ,  -39.392991  ,  -52.523988  ,
                -65.654985  ],
               [ -41.845993  ,  -83.691986  , -125.537979  , -167.383972  ,
                -209.229965  ],
               [   1.0498914 ,    2.0997828 ,    3.1496742 ,    4.1995656 ,
                   5.24945701],
               [   0.99526324,    1.99052648,    2.98578972,    3.98105296,
                   4.9763162 ],
               [   1.1616869 ,    2.3233738 ,    3.4850607 ,    4.6467476 ,
                   5.8084345 ]])       
        >>> # multiply a (subset of a) Variables object by a numpy 2D ndarray
        >>> data = np.array([[1.0, 2.0, 3.0, 4.0, 5.0], 
        ...                  [6.0, 7.0, 8.0, 9.0, 10.0], 
        ...                  [11.0, 12.0, 13.0, 14.0, 15.0], 
        ...                  [16.0, 17.0, 18.0, 19.0, 20.0], 
        ...                  [21.0, 22.0, 23.0, 24.0, 25.0]])
        >>> result = data * vars_subset
        >>> result                                  # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        array([[  26.240999  ,   60.318     ,  103.985997  ,   32.6440088 ,
                  -65.654985  ],
                [-185.604     , -282.001993  , -345.263976  , -144.261027  ,
                 -418.45993   ],
                [  11.26876733,   12.37740123,   13.40192989,   14.6479787 ,
                   15.74837102],
                [  15.4346655 ,   16.55866374,   17.61863151,   18.80157119,
                   19.90526479],
                [  22.3189344 ,   24.426215  ,   26.5250996 ,   27.7710624 ,
                   29.0421725 ]])
        """
        return self.__binary_op__(other, BinaryOperation.OP_MUL, True)

    # self *= other
    def __imul__(self, other):
        r"""
        multiply the current (subset of) Variables object by `other`.

        Parameters
        ----------
        other: int, float, numpy ndarray, pandas Series, pandas DataFrame, larray Array or iode Variables
            If `other` is an int or a float, multiply all values of the current (subset of) Variables object by the scalar.
            If `other` is a numpy ndarray, the shape of the ndarray must be compatible with the current (subset of) 
            Variables object. Specifically, the number of rows must be equal to the number of variables and the number of 
            columns must be equal to the number of periods.
            If `other` is a pandas Series, it must represent either a single variable or a single period.
            If `other` is a pandas DataFrame, it must represent the same variables names and periods 
            as the current (subset of) Variables object. Specifically, the index of the DataFrame must be equal to the 
            variables names and the columns of the DataFrame must be equal to the periods.
            If `other` is an larray Array, its last axis must be equal to the periods and be named 'time'. 
            If the Array has more than two axes, the first n-1 axes are combined to form the variables names. 
            The first (combined) axis must be equal to the variables names.
            If `other` is an iode Variables object, it must share the same sample and represent the same 
            set of variables names as `self`.

        Warnings
        --------
        Multiplying a numpy ndarray to a Variables object is not recommended as there is no compatibility check 
        between for the names and periods. The result is not guaranteed to be the one you expected. 
        This possibility is provided for speed reasons (when the database or the subset is large).

        Examples
        --------
        >>> import numpy as np
        >>> import pandas as pd
        >>> import larray as la
        >>> from iode import SAMPLE_DATA_DIR
        >>> from iode import variables, NA, Sample
        >>> variables.load(f"{SAMPLE_DATA_DIR}/fun.var")        # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Loading .../fun.var
        394 objects loaded
        >>> vars_subset = variables["A*", "1991Y1:1995Y1"]
        >>> vars_subset.names
        ['ACAF', 'ACAG', 'AOUC', 'AOUC_', 'AQC']
        >>> vars_subset                                         # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Workspace: Variables
        nb variables: 5
        filename: ...fun.var
        description: Modèle fun - Simulation 1
        sample: 1991Y1:1995Y1
        mode: LEVEL
        <BLANKLINE>
         name       1991Y1  1992Y1  1993Y1  1994Y1  1995Y1
        ACAF         26.24   30.16   34.66    8.16  -13.13
        ACAG        -30.93  -40.29  -43.16  -16.03  -41.85
        AOUC          1.02    1.03    1.03    1.05    1.05
        AOUC_         0.96    0.97    0.98    0.99    1.00
        AQC           1.06    1.11    1.15    1.16    1.16
        <BLANKLINE>

        >>> # multiply all values of a subset of a Variables object by a scalar
        >>> vars_subset *= 2.0
        >>> vars_subset                                         # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Workspace: Variables
        nb variables: 5
        filename: ...fun.var
        description: Modèle fun - Simulation 1
        sample: 1991Y1:1995Y1
        mode: LEVEL
        <BLANKLINE>
         name       1991Y1  1992Y1  1993Y1  1994Y1  1995Y1
        ACAF    	 52.48	 60.32	 69.32	 16.32	-26.26
        ACAG     	-61.87	-80.57	-86.32	-32.06	-83.69
        AOUC 	      2.05	  2.06	  2.06	  2.09	  2.10
        AOUC_	      1.93	  1.95	  1.96	  1.98	  1.99
        AQC  	      2.13	  2.22	  2.31	  2.31	  2.32
        <BLANKLINE>

        >>> # multiply two (subsets of) a Variables object
        >>> vars_subset *= vars_subset
        >>> vars_subset                                 # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Workspace: Variables
        nb variables: 5
        filename: ...fun.var
        description: Modèle fun - Simulation 1
        sample: 1991Y1:1995Y1
        mode: LEVEL
        <BLANKLINE>
         name        1991Y1  1992Y1  1993Y1  1994Y1  1995Y1
        ACAF        2754.36 3638.26 4805.82  266.41  689.69
        ACAG        3827.65 6491.85 7450.45 1027.72 7004.35
        AOUC           4.20    4.26    4.25    4.38    4.41
        AOUC_          3.72    3.80    3.83    3.92    3.96
        AQC            4.52    4.93    5.32    5.36    5.40
        <BLANKLINE>

        >>> # multiply a single variable by a pandas Series
        >>> series = pd.Series([1.0, 2.0, 3.0, 4.0, 5.0], index=vars_subset.periods)
        >>> series                                              # doctest: +NORMALIZE_WHITESPACE
        1991Y1    1.0
        1992Y1    2.0
        1993Y1    3.0
        1994Y1    4.0
        1995Y1    5.0
        dtype: float64
        >>> vars_subset["ACAF"] *= series
        >>> vars_subset["ACAF"]                                 # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Workspace: Variables
        nb variables: 1
        filename: ...fun.var
        description: Modèle fun - Simulation 1
        sample: 1991Y1:1995Y1
        mode: LEVEL
        <BLANKLINE>
        name         1991Y1   1992Y1    1993Y1   1994Y1   1995Y1
        ACAF        2754.36  7276.52  14417.45  1065.63  3448.46
        <BLANKLINE>
        
        >>> # multiply a subset corresponding to a single period by a pandas Series
        >>> series = pd.Series([1.0, 2.0, 3.0, 4.0, 5.0], index=vars_subset.names)
        >>> series                                          # doctest: +NORMALIZE_WHITESPACE
        ACAF     1.0
        ACAG     2.0
        AOUC     3.0
        AOUC_    4.0
        AQC      5.0
        dtype: float64
        >>> vars_subset[:, "1995Y1"] *= series
        >>> vars_subset[:, "1995Y1"]                        # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Workspace: Variables
        nb variables: 5
        filename: ...fun.var
        description: Modèle fun - Simulation 1
        sample: 1995Y1:1995Y1
        mode: LEVEL
        <BLANKLINE>
         name         1995Y1
        ACAF         3448.46
        ACAG        14008.70
        AOUC           13.23
        AOUC_          15.85
        AQC            26.99
        <BLANKLINE>

        >>> # multiply a subset of a Variables object by a pandas DataFrame  
        >>> data = np.array([[1.0, 2.0, 3.0, 4.0, 5.0], 
        ...                  [6.0, 7.0, 8.0, 9.0, 10.0], 
        ...                  [11.0, 12.0, 13.0, 14.0, 15.0], 
        ...                  [16.0, 17.0, 18.0, 19.0, 20.0], 
        ...                  [21.0, 22.0, 23.0, 24.0, 25.0]],)
        >>> df = pd.DataFrame(data, index=vars_subset.names, columns=vars_subset.periods) 
        >>> df                                              # doctest: +NORMALIZE_WHITESPACE
               1991Y1  1992Y1  1993Y1  1994Y1  1995Y1
        ACAF      1.0     2.0     3.0     4.0     5.0
        ACAG      6.0     7.0     8.0     9.0    10.0
        AOUC     11.0    12.0    13.0    14.0    15.0
        AOUC_    16.0    17.0    18.0    19.0    20.0
        AQC      21.0    22.0    23.0    24.0    25.0
        >>> vars_subset *= df
        >>> vars_subset                                     # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Workspace: Variables
        nb variables: 5
        filename: ...fun.var
        description: Modèle fun - Simulation 1
        sample: 1991Y1:1995Y1
        mode: LEVEL
        <BLANKLINE>
         name         1991Y1          1992Y1          1993Y1         1994Y1    1995Y1
        ACAF         2754.36        14553.04        43252.35        4262.53   17242.31
        ACAG        22965.90        45442.93        59603.61        9249.44  140086.97
        AOUC           46.18           51.07           55.27          61.30     198.41
        AOUC_          59.56           64.52           68.98          74.42     316.98
        AQC            94.88          108.48          122.36         128.54     674.76
        <BLANKLINE>

        >>> # multiply a subset of a Variables object by an larray Array
        >>> axis_names = la.Axis(name="names", labels=vars_subset.names)
        >>> axis_time = la.Axis(name="time", labels=vars_subset.periods)
        >>> array = la.Array(data, axes=(axis_names, axis_time))
        >>> array                                           # doctest: +NORMALIZE_WHITESPACE
        names\time  1991Y1  1992Y1  1993Y1  1994Y1  1995Y1
              ACAF     1.0     2.0     3.0     4.0     5.0
              ACAG     6.0     7.0     8.0     9.0    10.0
              AOUC    11.0    12.0    13.0    14.0    15.0
             AOUC_    16.0    17.0    18.0    19.0    20.0
               AQC    21.0    22.0    23.0    24.0    25.0
        >>> vars_subset *= array
        >>> vars_subset                                     # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Workspace: Variables
        nb variables: 5
        filename: ...fun.var
        description: Modèle fun - Simulation 1
        sample: 1991Y1:1995Y1
        mode: LEVEL
        <BLANKLINE>
         name          1991Y1          1992Y1          1993Y1         1994Y1            1995Y1
        ACAF          2754.36        29106.09       129757.05       17050.10          86211.54
        ACAG        137795.38       318100.50       476828.85       83244.98        1400869.70
        AOUC           507.94          612.80          718.45         858.25           2976.13
        AOUC_          952.92         1096.76         1241.66        1414.00           6339.51
        AQC           1992.54         2386.56         2814.32        3084.93          16868.96
        <BLANKLINE>    

        >>> # WARNING: multiplying a numpy ndarray to a (subset of a) Variables object is not recommended 
        >>> #          as there is no compatibility check between for the names and periods.
        >>> #          The result is not guaranteed to be the one you expected.
        >>> #          This possibility is provided for speed reasons 
        >>> #          (when dealing with large subsets/databases).
        >>> # multiply a single variable by a numpy 1D ndarray
        >>> data = np.array([1.0, 2.0, 3.0, 4.0, 5.0])
        >>> vars_subset["ACAF"] *= data
        >>> vars_subset["ACAF"]                             # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Workspace: Variables
        nb variables: 1
        filename: ...fun.var
        description: Modèle fun - Simulation 1
        sample: 1991Y1:1995Y1
        mode: LEVEL
        <BLANKLINE>
        name         1991Y1    1992Y1     1993Y1    1994Y1     1995Y1
        ACAF        2754.36  58212.18  389271.15  68200.40  431057.71
        <BLANKLINE>
        >>> # multiply the subset corresponding to a single period by a numpy 1D ndarray
        >>> vars_subset[:, "1995Y1"] *= data
        >>> vars_subset[:, "1995Y1"]                        # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Workspace: Variables
        nb variables: 5
        filename: ...fun.var
        description: Modèle fun - Simulation 1
        sample: 1995Y1:1995Y1
        mode: LEVEL
        <BLANKLINE>
         name           1995Y1
        ACAF         431057.71
        ACAG        2801739.41
        AOUC           8928.40
        AOUC_         25358.05
        AQC           84344.78
        <BLANKLINE>      
        >>> # multiply a (subset of a) Variables object by a numpy 2D ndarray
        >>> data = np.array([[1.0, 2.0, 3.0, 4.0, 5.0], 
        ...                  [6.0, 7.0, 8.0, 9.0, 10.0], 
        ...                  [11.0, 12.0, 13.0, 14.0, 15.0], 
        ...                  [16.0, 17.0, 18.0, 19.0, 20.0], 
        ...                  [21.0, 22.0, 23.0, 24.0, 25.0]])
        >>> vars_subset *= data
        >>> vars_subset                                     # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Workspace: Variables
        nb variables: 5
        filename: ...fun.var
        description: Modèle fun - Simulation 1
        sample: 1991Y1:1995Y1
        mode: LEVEL
        <BLANKLINE>
         name          1991Y1          1992Y1          1993Y1          1994Y1            1995Y1
        ACAF          2754.36        116424.36      1167813.46      272801.62        2155288.53
        ACAG        826772.28       2226703.47      3814630.82      749204.78       28017394.08
        AOUC          5587.35          7353.60         9339.81       12015.54         133926.04
        AOUC_        15246.65         18644.88        22349.96       26865.93         507161.04
        AQC          41843.33         52504.32        64729.44       74038.26        2108619.46
        <BLANKLINE>
        """
        self.__binary_op__(other, BinaryOperation.OP_MUL, False)
        return self

    # self / other
    def __truediv__(self, other):
        r"""
        divide the current (subset of) Variables object by `other`.

        Parameters
        ----------
        other: int, float, numpy ndarray, pandas Series, pandas DataFrame, larray Array or iode Variables
            If `other` is an int or a float, divide all values of the current (subset of) Variables object by the scalar.
            If `other` is a numpy ndarray, the shape of the ndarray must be compatible with the current (subset of) 
            Variables object. Specifically, the number of rows must be equal to the number of variables and the number of 
            columns must be equal to the number of periods.
            If `other` is a pandas Series, it must represent either a single variable or a single period.
            If `other` is a pandas DataFrame, it must represent the same variables names and periods 
            as the current (subset of) Variables object. Specifically, the index of the DataFrame must be equal to the 
            variables names and the columns of the DataFrame must be equal to the periods.
            If `other` is an larray Array, its last axis must be equal to the periods and be named 'time'. 
            If the Array has more than two axes, the first n-1 axes are combined to form the variables names. 
            The first (combined) axis must be equal to the variables names.
            If `other` is an iode Variables object, it must share the same sample and represent the same 
            set of variables names as `self`.
                
        Returns
        -------
        Variables

        Warnings
        --------
        Dividing a numpy ndarray to a Variables object is not recommended as there is no compatibility check 
        between for the names and periods. The result is not guaranteed to be the one you expected. 
        This possibility is provided for speed reasons (when the database or the subset is large).

        Examples
        --------
        >>> import numpy as np
        >>> import pandas as pd
        >>> import larray as la
        >>> from iode import SAMPLE_DATA_DIR
        >>> from iode import variables, NA, Sample
        >>> variables.load(f"{SAMPLE_DATA_DIR}/fun.var")        # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Loading .../fun.var
        394 objects loaded
        >>> vars_subset = variables["A*", "1991Y1:1995Y1"]
        >>> vars_subset.names
        ['ACAF', 'ACAG', 'AOUC', 'AOUC_', 'AQC']
        >>> vars_subset                                         # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Workspace: Variables
        nb variables: 5
        filename: ...fun.var
        description: Modèle fun - Simulation 1
        sample: 1991Y1:1995Y1
        mode: LEVEL
        <BLANKLINE>
         name       1991Y1  1992Y1  1993Y1  1994Y1  1995Y1
        ACAF         26.24   30.16   34.66    8.16  -13.13
        ACAG        -30.93  -40.29  -43.16  -16.03  -41.85
        AOUC          1.02    1.03    1.03    1.05    1.05
        AOUC_         0.96    0.97    0.98    0.99    1.00
        AQC           1.06    1.11    1.15    1.16    1.16
        <BLANKLINE>

        >>> # divide all values of a subset of a Variables object by a scalar
        >>> new_vars_subset = vars_subset / 2.0
        >>> new_vars_subset                                     # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Workspace: Variables
        nb variables: 5
        filename: ...fun.var
        description: Modèle fun - Simulation 1
        sample: 1991Y1:1995Y1
        mode: LEVEL
        <BLANKLINE>
         name	1991Y1	1992Y1	1993Y1	1994Y1	1995Y1
        ACAF 	 13.12	 15.08	 17.33	  4.08	 -6.57
        ACAG 	-15.47	-20.14	-21.58	 -8.01	-20.92
        AOUC 	  0.51	  0.52	  0.52	  0.52	  0.52
        AOUC_	  0.48	  0.49	  0.49	  0.49	  0.50
        AQC  	  0.53	  0.56	  0.58	  0.58	  0.58
        <BLANKLINE>

        >>> # divide (a subset of) a Variables object by another
        >>> new_vars_subset = vars_subset / vars_subset
        >>> new_vars_subset                                 # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Workspace: Variables
        nb variables: 5
        filename: ...fun.var
        description: Modèle fun - Simulation 1
        sample: 1991Y1:1995Y1
        mode: LEVEL
        <BLANKLINE>
         name	1991Y1	1992Y1	1993Y1	1994Y1	1995Y1
        ACAF 	  1.00	  1.00	  1.00	  1.00	  1.00
        ACAG 	  1.00	  1.00	  1.00	  1.00	  1.00
        AOUC 	  1.00	  1.00	  1.00	  1.00	  1.00
        AOUC_	  1.00	  1.00	  1.00	  1.00	  1.00
        AQC  	  1.00	  1.00	  1.00	  1.00	  1.00
        <BLANKLINE>

        >>> # divide a a single variable by a pandas Series
        >>> series = pd.Series([1.0, 2.0, 3.0, 4.0, 5.0], index=vars_subset.periods)
        >>> series                                              # doctest: +NORMALIZE_WHITESPACE
        1991Y1    1.0
        1992Y1    2.0
        1993Y1    3.0
        1994Y1    4.0
        1995Y1    5.0
        dtype: float64
        >>> updated_ACAF = vars_subset["ACAF"] / series
        >>> updated_ACAF                                        # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Workspace: Variables
        nb variables: 1
        filename: ...fun.var
        description: Modèle fun - Simulation 1
        sample: 1991Y1:1995Y1
        mode: LEVEL
        <BLANKLINE>
        name	1991Y1	1992Y1	1993Y1	1994Y1	1995Y1
        ACAF	 26.24	 15.08	 11.55	  2.04	 -2.63
        <BLANKLINE>
        
        >>> # divide a pandas Series to the subset corresponding to a single period
        >>> series = pd.Series([1.0, 2.0, 3.0, 4.0, 5.0], index=vars_subset.names)
        >>> series                                          # doctest: +NORMALIZE_WHITESPACE
        ACAF     1.0
        ACAG     2.0
        AOUC     3.0
        AOUC_    4.0
        AQC      5.0
        dtype: float64
        >>> vars_subset_1995Y1 = vars_subset[:, "1995Y1"] / series
        >>> vars_subset_1995Y1                              # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Workspace: Variables
        nb variables: 5
        filename: ...fun.var
        description: Modèle fun - Simulation 1
        sample: 1995Y1:1995Y1
        mode: LEVEL
        <BLANKLINE>
         name	1995Y1
        ACAF 	-13.13
        ACAG 	-20.92
        AOUC 	  0.35
        AOUC_	  0.25
        AQC  	  0.23
        <BLANKLINE>

        >>> # divide a (subset of a) Variables object by a pandas DataFrame  
        >>> data = np.array([[1.0, 2.0, 3.0, 4.0, 5.0], 
        ...                  [6.0, 7.0, 8.0, 9.0, 10.0], 
        ...                  [11.0, 12.0, 13.0, 14.0, 15.0], 
        ...                  [16.0, 17.0, 18.0, 19.0, 20.0], 
        ...                  [21.0, 22.0, 23.0, 24.0, 25.0]],)
        >>> df = pd.DataFrame(data, index=vars_subset.names, columns=vars_subset.periods) 
        >>> df                                              # doctest: +NORMALIZE_WHITESPACE
               1991Y1  1992Y1  1993Y1  1994Y1  1995Y1
        ACAF      1.0     2.0     3.0     4.0     5.0
        ACAG      6.0     7.0     8.0     9.0    10.0
        AOUC     11.0    12.0    13.0    14.0    15.0
        AOUC_    16.0    17.0    18.0    19.0    20.0
        AQC      21.0    22.0    23.0    24.0    25.0
        >>> new_vars_subset = vars_subset / df
        >>> new_vars_subset                                 # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Workspace: Variables
        nb variables: 5
        filename: ...fun.var
        description: Modèle fun - Simulation 1
        sample: 1991Y1:1995Y1
        mode: LEVEL
        <BLANKLINE>
         name	1991Y1	1992Y1	1993Y1	1994Y1	1995Y1
        ACAF 	 26.24	 15.08	 11.55	  2.04	 -2.63
        ACAG 	 -5.16	 -5.76	 -5.39	 -1.78	 -4.18
        AOUC 	  0.09	  0.09	  0.08	  0.07	  0.07
        AOUC_	  0.06	  0.06	  0.05	  0.05	  0.05
        AQC  	  0.05	  0.05	  0.05	  0.05	  0.05
        <BLANKLINE>

        >>> # divide (a subset of) a Variables object by an larray Array
        >>> axis_names = la.Axis(name="names", labels=vars_subset.names)
        >>> axis_time = la.Axis(name="time", labels=vars_subset.periods)
        >>> array = la.Array(data, axes=(axis_names, axis_time))
        >>> array                                           # doctest: +NORMALIZE_WHITESPACE
        names\time  1991Y1  1992Y1  1993Y1  1994Y1  1995Y1
              ACAF     1.0     2.0     3.0     4.0     5.0
              ACAG     6.0     7.0     8.0     9.0    10.0
              AOUC    11.0    12.0    13.0    14.0    15.0
             AOUC_    16.0    17.0    18.0    19.0    20.0
               AQC    21.0    22.0    23.0    24.0    25.0
        >>> new_vars_subset = vars_subset / array
        >>> new_vars_subset                                 # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Workspace: Variables
        nb variables: 5
        filename: ...fun.var
        description: Modèle fun - Simulation 1
        sample: 1991Y1:1995Y1
        mode: LEVEL
        <BLANKLINE>
         name	1991Y1	1992Y1	1993Y1	1994Y1	1995Y1
        ACAF 	 26.24	 15.08	 11.55	  2.04	 -2.63
        ACAG 	 -5.16	 -5.76	 -5.39	 -1.78	 -4.18
        AOUC 	  0.09	  0.09	  0.08	  0.07	  0.07
        AOUC_	  0.06	  0.06	  0.05	  0.05	  0.05
        AQC  	  0.05	  0.05	  0.05	  0.05	  0.05
        <BLANKLINE>        

        >>> # WARNING: dividing a numpy ndarray to a (subset of a) Variables object is not recommended 
        >>> #          as there is no compatibility check between for the names and periods.
        >>> #          The result is not guaranteed to be the one you expected.
        >>> #          This possibility is provided for speed reasons 
        >>> #          (when dealing with large subsets/databases).
        >>> # divide a single variable by a numpy 1D ndarray
        >>> data = np.array([1.0, 2.0, 3.0, 4.0, 5.0])
        >>> updated_ACAF = vars_subset["ACAF"] / data
        >>> updated_ACAF                                    # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Workspace: Variables
        nb variables: 1
        filename: ...fun.var
        description: Modèle fun - Simulation 1
        sample: 1991Y1:1995Y1
        mode: LEVEL
        <BLANKLINE>
        name	1991Y1	1992Y1	1993Y1	1994Y1	1995Y1
        ACAF	 26.24	 15.08	 11.55	  2.04	 -2.63
        <BLANKLINE>
        >>> # divide the subset corresponding to a single period by a numpy 1D ndarray
        >>> vars_subset_1995Y1 = vars_subset[:, "1995Y1"] / data
        >>> vars_subset_1995Y1                              # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Workspace: Variables
        nb variables: 5
        filename: ...fun.var
        description: Modèle fun - Simulation 1
        sample: 1995Y1:1995Y1
        mode: LEVEL
        <BLANKLINE>
         name	1995Y1
        ACAF 	-13.13
        ACAG 	-20.92
        AOUC 	  0.35
        AOUC_	  0.25
        AQC  	  0.23
        <BLANKLINE>
        >>> # divide a (subset of a) Variables object by a numpy 2D ndarray
        >>> data = np.array([[1.0, 2.0, 3.0, 4.0, 5.0], 
        ...                  [6.0, 7.0, 8.0, 9.0, 10.0], 
        ...                  [11.0, 12.0, 13.0, 14.0, 15.0], 
        ...                  [16.0, 17.0, 18.0, 19.0, 20.0], 
        ...                  [21.0, 22.0, 23.0, 24.0, 25.0]])
        >>> new_vars_subset = vars_subset / data
        >>> new_vars_subset                                 # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Workspace: Variables
        nb variables: 5
        filename: ...fun.var
        description: Modèle fun - Simulation 1
        sample: 1991Y1:1995Y1
        mode: LEVEL
        <BLANKLINE>
         name	1991Y1	1992Y1	1993Y1	1994Y1	1995Y1
        ACAF 	 26.24	 15.08	 11.55	  2.04	 -2.63
        ACAG 	 -5.16	 -5.76	 -5.39	 -1.78	 -4.18
        AOUC 	  0.09	  0.09	  0.08	  0.07	  0.07
        AOUC_	  0.06	  0.06	  0.05	  0.05	  0.05
        AQC  	  0.05	  0.05	  0.05	  0.05	  0.05
        <BLANKLINE>
        """
        if isinstance(other, (int, float)) and other == 0:
            raise ZeroDivisionError("division by zero")
        return self.__binary_op__(other, BinaryOperation.OP_DIV, True)

    # other / self
    def __rtruediv__(self, other):
        r"""
        divide `other` by the current (subset of) Variables object.

        Parameters
        ----------
        other: iode Variables
            `self` and `other` must share the same sample and represent 
            the same set of variables names.
        
        Returns
        -------
        Variables

        Examples
        --------
        >>> import numpy as np
        >>> import pandas as pd
        >>> import larray as la
        >>> from iode import SAMPLE_DATA_DIR
        >>> from iode import variables, NA, Sample
        >>> variables.load(f"{SAMPLE_DATA_DIR}/fun.var")        # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Loading .../fun.var
        394 objects loaded
        >>> vars_subset = variables["A*", "1991Y1:1995Y1"]
        >>> vars_subset.names
        ['ACAF', 'ACAG', 'AOUC', 'AOUC_', 'AQC']
        >>> vars_subset                                         # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Workspace: Variables
        nb variables: 5
        filename: ...fun.var
        description: Modèle fun - Simulation 1
        sample: 1991Y1:1995Y1
        mode: LEVEL
        <BLANKLINE>
         name       1991Y1  1992Y1  1993Y1  1994Y1  1995Y1
        ACAF         26.24   30.16   34.66    8.16  -13.13
        ACAG        -30.93  -40.29  -43.16  -16.03  -41.85
        AOUC          1.02    1.03    1.03    1.05    1.05
        AOUC_         0.96    0.97    0.98    0.99    1.00
        AQC           1.06    1.11    1.15    1.16    1.16
        <BLANKLINE>

        >>> # divide (a subset of) a Variables object by another
        >>> new_vars_subset = vars_subset / vars_subset
        >>> new_vars_subset                                 # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Workspace: Variables
        nb variables: 5
        filename: ...fun.var
        description: Modèle fun - Simulation 1
        sample: 1991Y1:1995Y1
        mode: LEVEL
        <BLANKLINE>
         name	1991Y1	1992Y1	1993Y1	1994Y1	1995Y1
        ACAF 	  1.00	  1.00	  1.00	  1.00	  1.00
        ACAG 	  1.00	  1.00	  1.00	  1.00	  1.00
        AOUC 	  1.00	  1.00	  1.00	  1.00	  1.00
        AOUC_	  1.00	  1.00	  1.00	  1.00	  1.00
        AQC  	  1.00	  1.00	  1.00	  1.00	  1.00
        <BLANKLINE>
        """
        if isinstance(other, Variables):
            return other.__binary_op__(self, BinaryOperation.OP_DIV, True)
        else:
            raise TypeError(f"unsupported operand type for 'X' in the arithmetic operation 'X / Y'.\n"
                            f"The only accepted type for 'X' is 'Variables'.\nGot 'X' of type {type(other).__name__} instead")

    # self /= other
    def __itruediv__(self, other):
        r"""
        divide the current (subset of) Variables object by `other`.

        Parameters
        ----------
        other: int, float, numpy ndarray, pandas Series, pandas DataFrame, larray Array or iode Variables
            If `other` is an int or a float, divide all values of the current (subset of) Variables object by the scalar.
            If `other` is a numpy ndarray, the shape of the ndarray must be compatible with the current (subset of) 
            Variables object. Specifically, the number of rows must be equal to the number of variables and the number of 
            columns must be equal to the number of periods.
            If `other` is a pandas Series, it must represent either a single variable or a single period.
            If `other` is a pandas DataFrame, it must represent the same variables names and periods 
            as the current (subset of) Variables object. Specifically, the index of the DataFrame must be equal to the 
            variables names and the columns of the DataFrame must be equal to the periods.
            If `other` is an larray Array, its last axis must be equal to the periods and be named 'time'. 
            If the Array has more than two axes, the first n-1 axes are combined to form the variables names. 
            The first (combined) axis must be equal to the variables names.
            If `other` is an iode Variables object, it must share the same sample and represent the same 
            set of variables names as `self`.
        
        Warnings
        --------
        Dividing a numpy ndarray to a Variables object is not recommended as there is no compatibility check 
        between for the names and periods. The result is not guaranteed to be the one you expected. 
        This possibility is provided for speed reasons (when the database or the subset is large).

        Examples
        --------
        >>> import numpy as np
        >>> import pandas as pd
        >>> import larray as la
        >>> from iode import SAMPLE_DATA_DIR
        >>> from iode import variables, NA, Sample
        >>> variables.load(f"{SAMPLE_DATA_DIR}/fun.var")        # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Loading .../fun.var
        394 objects loaded
        >>> vars_subset = variables["A*", "1991Y1:1995Y1"]
        >>> vars_subset.names
        ['ACAF', 'ACAG', 'AOUC', 'AOUC_', 'AQC']
        >>> vars_subset                                         # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Workspace: Variables
        nb variables: 5
        filename: ...fun.var
        description: Modèle fun - Simulation 1
        sample: 1991Y1:1995Y1
        mode: LEVEL
        <BLANKLINE>
         name       1991Y1  1992Y1  1993Y1  1994Y1  1995Y1
        ACAF         26.24   30.16   34.66    8.16  -13.13
        ACAG        -30.93  -40.29  -43.16  -16.03  -41.85
        AOUC          1.02    1.03    1.03    1.05    1.05
        AOUC_         0.96    0.97    0.98    0.99    1.00
        AQC           1.06    1.11    1.15    1.16    1.16
        <BLANKLINE>

        >>> # divide all values of a subset of a Variables object by a scalar
        >>> vars_subset /= 2.0
        >>> vars_subset                                         # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Workspace: Variables
        nb variables: 5
        filename: ...fun.var
        description: Modèle fun - Simulation 1
        sample: 1991Y1:1995Y1
        mode: LEVEL
        <BLANKLINE>
         name	1991Y1	1992Y1	1993Y1	1994Y1	1995Y1
        ACAF 	 13.12	 15.08	 17.33	  4.08	 -6.57
        ACAG 	-15.47	-20.14	-21.58	 -8.01	-20.92
        AOUC 	  0.51	  0.52	  0.52	  0.52	  0.52
        AOUC_	  0.48	  0.49	  0.49	  0.49	  0.50
        AQC  	  0.53	  0.56	  0.58	  0.58	  0.58
        <BLANKLINE>

        >>> # divide (a subset of) a Variables object by another
        >>> vars_subset /= vars_subset
        >>> vars_subset                                 # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Workspace: Variables
        nb variables: 5
        filename: ...fun.var
        description: Modèle fun - Simulation 1
        sample: 1991Y1:1995Y1
        mode: LEVEL
        <BLANKLINE>
         name	1991Y1	1992Y1	1993Y1	1994Y1	1995Y1
        ACAF 	  1.00	  1.00	  1.00	  1.00	  1.00
        ACAG 	  1.00	  1.00	  1.00	  1.00	  1.00
        AOUC 	  1.00	  1.00	  1.00	  1.00	  1.00
        AOUC_	  1.00	  1.00	  1.00	  1.00	  1.00
        AQC  	  1.00	  1.00	  1.00	  1.00	  1.00
        <BLANKLINE>

        >>> # divide a a single variable by a pandas Series
        >>> series = pd.Series([1.0, 0.5, 0.25, 0.2, 0.1], index=vars_subset.periods)
        >>> series                                              # doctest: +NORMALIZE_WHITESPACE
        1991Y1    1.00
        1992Y1    0.50
        1993Y1    0.25
        1994Y1    0.20
        1995Y1    0.10
        dtype: float64
        >>> vars_subset["ACAF"] /= series
        >>> vars_subset["ACAF"]                                 # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Workspace: Variables
        nb variables: 1
        filename: ...fun.var
        description: Modèle fun - Simulation 1
        sample: 1991Y1:1995Y1
        mode: LEVEL
        <BLANKLINE>
        name        1991Y1  1992Y1  1993Y1  1994Y1  1995Y1
        ACAF          1.00    2.00    4.00    5.00   10.00
        <BLANKLINE>
        
        >>> # divide a pandas Series to the subset corresponding to a single period
        >>> series = pd.Series([1.0, 0.5, 0.25, 0.2, 0.1], index=vars_subset.names)
        >>> series                                          # doctest: +NORMALIZE_WHITESPACE
        ACAF     1.00
        ACAG     0.50
        AOUC     0.25
        AOUC_    0.20
        AQC      0.10
        dtype: float64
        >>> vars_subset[:, "1995Y1"] /= series
        >>> vars_subset[:, "1995Y1"]                        # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Workspace: Variables
        nb variables: 5
        filename: ...fun.var
        description: Modèle fun - Simulation 1
        sample: 1995Y1:1995Y1
        mode: LEVEL
        <BLANKLINE>
         name       1995Y1
        ACAF         10.00
        ACAG          2.00
        AOUC          4.00
        AOUC_         5.00
        AQC          10.00
        <BLANKLINE>

        >>> # divide a (subset of a) Variables object by a pandas DataFrame  
        >>> data = np.array([[0.1, 0.2, 0.25, 0.5, 1.0], 
        ...                  [1.0, 0.5, 0.25, 0.2, 0.1], 
        ...                  [1.0, 0.5, 0.25, 0.2, 0.1], 
        ...                  [1.0, 0.5, 0.25, 0.2, 0.1], 
        ...                  [1.0, 0.5, 0.25, 0.2, 0.1]])
        >>> df = pd.DataFrame(data, index=vars_subset.names, columns=vars_subset.periods) 
        >>> df                                              # doctest: +NORMALIZE_WHITESPACE
               1991Y1  1992Y1  1993Y1  1994Y1  1995Y1
        ACAF      0.1     0.2    0.25     0.5     1.0
        ACAG      1.0     0.5    0.25     0.2     0.1
        AOUC      1.0     0.5    0.25     0.2     0.1
        AOUC_     1.0     0.5    0.25     0.2     0.1
        AQC       1.0     0.5    0.25     0.2     0.1
        >>> vars_subset /= df
        >>> vars_subset                                     # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Workspace: Variables
        nb variables: 5
        filename: ...fun.var
        description: Modèle fun - Simulation 1
        sample: 1991Y1:1995Y1
        mode: LEVEL
        <BLANKLINE>
         name       1991Y1  1992Y1  1993Y1  1994Y1  1995Y1
        ACAF         10.00   10.00   16.00   10.00   10.00
        ACAG          1.00    2.00    4.00    5.00   20.00
        AOUC          1.00    2.00    4.00    5.00   40.00
        AOUC_         1.00    2.00    4.00    5.00   50.00
        AQC           1.00    2.00    4.00    5.00  100.00
        <BLANKLINE>

        >>> # divide (a subset of) a Variables object by an larray Array
        >>> axis_names = la.Axis(name="names", labels=vars_subset.names)
        >>> axis_time = la.Axis(name="time", labels=vars_subset.periods)
        >>> array = la.Array(data, axes=(axis_names, axis_time))
        >>> array                                           # doctest: +NORMALIZE_WHITESPACE
        names\time  1991Y1  1992Y1  1993Y1  1994Y1  1995Y1
            ACAF       0.1     0.2    0.25     0.5     1.0
            ACAG       1.0     0.5    0.25     0.2     0.1
            AOUC       1.0     0.5    0.25     0.2     0.1
            AOUC_      1.0     0.5    0.25     0.2     0.1
            AQC        1.0     0.5    0.25     0.2     0.1
        >>> vars_subset /= array
        >>> vars_subset                                     # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Workspace: Variables
        nb variables: 5
        filename: ...fun.var
        description: Modèle fun - Simulation 1
        sample: 1991Y1:1995Y1
        mode: LEVEL
        <BLANKLINE>
         name       1991Y1  1992Y1  1993Y1  1994Y1   1995Y1
        ACAF        100.00   50.00   64.00   20.00    10.00
        ACAG          1.00    4.00   16.00   25.00   200.00
        AOUC          1.00    4.00   16.00   25.00   400.00
        AOUC_         1.00    4.00   16.00   25.00   500.00
        AQC           1.00    4.00   16.00   25.00  1000.00
        <BLANKLINE>      

        >>> # WARNING: dividing a numpy ndarray to a (subset of a) Variables object is not recommended 
        >>> #          as there is no compatibility check between for the names and periods.
        >>> #          The result is not guaranteed to be the one you expected.
        >>> #          This possibility is provided for speed reasons 
        >>> #          (when dealing with large subsets/databases).
        >>> # divide a single variable by a numpy 1D ndarray
        >>> data = np.array([1.0, 0.5, 1.0, 0.5, 1.0])
        >>> vars_subset["ACAF"] /= data
        >>> vars_subset["ACAF"]                             # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Workspace: Variables
        nb variables: 1
        filename: ...fun.var
        description: Modèle fun - Simulation 1
        sample: 1991Y1:1995Y1
        mode: LEVEL
        <BLANKLINE>
        name        1991Y1  1992Y1  1993Y1  1994Y1  1995Y1
        ACAF        100.00  100.00   64.00   40.00   10.00
        <BLANKLINE>
        >>> # divide the subset corresponding to a single period by a numpy 1D ndarray
        >>> vars_subset[:, "1995Y1"] /= data
        >>> vars_subset[:, "1995Y1"]                        # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Workspace: Variables
        nb variables: 5
        filename: ...fun.var
        description: Modèle fun - Simulation 1
        sample: 1995Y1:1995Y1
        mode: LEVEL
        <BLANKLINE>
         name        1995Y1
        ACAF          10.00
        ACAG         400.00
        AOUC         400.00
        AOUC_       1000.00
        AQC         1000.00
        <BLANKLINE>
        >>> # divide a (subset of a) Variables object by a numpy 2D ndarray
        >>> data = np.array([[0.1, 0.2, 0.25, 0.5, 1.0], 
        ...                  [1.0, 0.5, 0.25, 0.2, 0.1], 
        ...                  [1.0, 0.5, 0.25, 0.2, 0.1], 
        ...                  [1.0, 0.5, 0.25, 0.2, 0.1], 
        ...                  [1.0, 0.5, 0.25, 0.2, 0.1]])
        >>> vars_subset /= data
        >>> vars_subset                                     # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Workspace: Variables
        nb variables: 5
        filename: ...fun.var
        description: Modèle fun - Simulation 1
        sample: 1991Y1:1995Y1
        mode: LEVEL
        <BLANKLINE>
         name        1991Y1  1992Y1  1993Y1  1994Y1    1995Y1
        ACAF        1000.00  500.00  256.00   80.00     10.00
        ACAG           1.00    8.00   64.00  125.00   4000.00
        AOUC           1.00    8.00   64.00  125.00   4000.00
        AOUC_          1.00    8.00   64.00  125.00  10000.00
        AQC            1.00    8.00   64.00  125.00  10000.00
        <BLANKLINE>
        """
        if isinstance(other, (int, float)) and other == 0:
            raise ZeroDivisionError("division by zero")
        self.__binary_op__(other, BinaryOperation.OP_DIV, False)
        return self

    # self ** other
    def __pow__(self, other):
        r"""
        Compute the expression :math:`self^{other}` ( self \*\* other ).

        Parameters
        ----------
        other: int, float, numpy ndarray, pandas Series, pandas DataFrame, larray Array or iode Variables
            If `other` is an int or a float, compute 'value \*\* other' for all values of the current (subset of) 
            Variables object.
            If `other` is a numpy ndarray, the shape of the ndarray must be compatible with the current (subset of) 
            Variables object. Specifically, the number of rows must be equal to the number of variables and the number of 
            columns must be equal to the number of periods.
            If `other` is a pandas Series, it must represent either a single variable or a single period.
            If `other` is a pandas DataFrame, it must represent the same variables names and periods 
            as the current (subset of) Variables object. Specifically, the index of the DataFrame must be equal to the 
            variables names and the columns of the DataFrame must be equal to the periods.
            If `other` is an larray Array, its last axis must be equal to the periods and be named 'time'. 
            If the Array has more than two axes, the first n-1 axes are combined to form the variables names. 
            The first (combined) axis must be equal to the variables names.
            If `other` is an iode Variables object, it must share the same sample and represent the same 
            set of variables names as `self`.
                
        Returns
        -------
        Variables

        Warnings
        --------
        Using a numpy ndarray is not recommended as there is no compatibility check 
        between for the names and periods. The result is not guaranteed to be the one you expected. 
        This possibility is provided for speed reasons (when the database or the subset is large).

        Examples
        --------
        >>> import numpy as np
        >>> import pandas as pd
        >>> import larray as la
        >>> from iode import SAMPLE_DATA_DIR
        >>> from iode import variables, NA, Sample
        >>> variables.load(f"{SAMPLE_DATA_DIR}/fun.var")        # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Loading .../fun.var
        394 objects loaded
        >>> vars_subset = variables["A*", "1991Y1:1995Y1"]
        >>> vars_subset.names
        ['ACAF', 'ACAG', 'AOUC', 'AOUC_', 'AQC']
        >>> vars_subset                                         # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Workspace: Variables
        nb variables: 5
        filename: ...fun.var
        description: Modèle fun - Simulation 1
        sample: 1991Y1:1995Y1
        mode: LEVEL
        <BLANKLINE>
         name       1991Y1  1992Y1  1993Y1  1994Y1  1995Y1
        ACAF         26.24   30.16   34.66    8.16  -13.13
        ACAG        -30.93  -40.29  -43.16  -16.03  -41.85
        AOUC          1.02    1.03    1.03    1.05    1.05
        AOUC_         0.96    0.97    0.98    0.99    1.00
        AQC           1.06    1.11    1.15    1.16    1.16
        <BLANKLINE>

        >>> # compute 'value ** other' for all values of the current 
        >>> # (subset of) Variables object.
        >>> new_vars_subset = vars_subset ** 2
        >>> new_vars_subset                                     # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Workspace: Variables
        nb variables: 5
        filename: ...fun.var
        description: Modèle fun - Simulation 1
        sample: 1991Y1:1995Y1
        mode: LEVEL
        <BLANKLINE>
         name       1991Y1   1992Y1   1993Y1  1994Y1   1995Y1
        ACAF 	    688.59	 909.57	 1201.45   66.60   172.42
        ACAG     	956.91	1622.96	 1862.61  256.93  1751.09
        AOUC 	      1.05	   1.06	    1.06	1.09	 1.10
        AOUC_    	  0.93	   0.95	    0.96	0.98	 0.99
        AQC  	      1.13	   1.23	    1.33	1.34	 1.35
        <BLANKLINE>

        >>> # compute 'V[name, period] ** W[name, period]' for each name and period
        >>> # for all names and periods
        >>> other = vars_subset.copy()
        >>> other = 2.0
        >>> new_vars_subset = vars_subset ** other
        >>> new_vars_subset                                 # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Workspace: Variables
        nb variables: 5
        filename: ...fun.var
        description: Modèle fun - Simulation 1
        sample: 1991Y1:1995Y1
        mode: LEVEL
        <BLANKLINE>
         name       1991Y1   1992Y1   1993Y1  1994Y1   1995Y1
        ACAF 	    688.59	 909.57	 1201.45   66.60   172.42
        ACAG     	956.91	1622.96	 1862.61  256.93  1751.09
        AOUC 	      1.05	   1.06	    1.06	1.09	 1.10
        AOUC_    	  0.93	   0.95	    0.96	0.98	 0.99
        AQC  	      1.13	   1.23	    1.33	1.34	 1.35
        <BLANKLINE>

        >>> # compute 'iode_var[period] ** series[period]' for each period
        >>> series = pd.Series([1.0, 2.0, 0.5, 1./4., 2.0], index=vars_subset.periods)
        >>> series                                              # doctest: +NORMALIZE_WHITESPACE
        1991Y1    1.00
        1992Y1    2.00
        1993Y1    0.50
        1994Y1    0.25
        1995Y1    2.00
        dtype: float64
        >>> updated_ACAF = vars_subset["ACAF"] ** series
        >>> updated_ACAF                                        # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Workspace: Variables
        nb variables: 1
        filename: ...fun.var
        description: Modèle fun - Simulation 1
        sample: 1991Y1:1995Y1
        mode: LEVEL
        <BLANKLINE>
         name       1991Y1  1992Y1  1993Y1  1994Y1  1995Y1
        ACAF	     26.24	909.57	  5.89	  1.69	172.42
        <BLANKLINE>
        
        >>> # compute 'single_period_subset[name] ** series[name]' for each name
        >>> series = pd.Series([1.0, 2.0, 0.5, 1./4., 2.0], index=vars_subset.names)
        >>> series                                          # doctest: +NORMALIZE_WHITESPACE
        ACAF     1.00
        ACAG     2.00
        AOUC     0.50
        AOUC_    0.25
        AQC      2.00
        dtype: float64
        >>> vars_subset_1995Y1 = vars_subset[:, "1995Y1"] ** series
        >>> vars_subset_1995Y1                              # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Workspace: Variables
        nb variables: 5
        filename: ...fun.var
        description: Modèle fun - Simulation 1
        sample: 1995Y1:1995Y1
        mode: LEVEL
        <BLANKLINE>
         name	 1995Y1
        ACAF 	 -13.13
        ACAG 	1751.09
        AOUC 	   1.02
        AOUC_	   1.00
        AQC  	   1.35
        <BLANKLINE>

        >>> # compute 'iode_var[name, period] ** df[name, period]' for each name and period
        >>> data = np.array([[1.0, 2.0, 0.5, 1./4., 2.0],
        ...                  [2.0, -1.0, 2.0, -1.0, 2.0], 
        ...                  [1./4., 2.0, 1.0, 2.0, 0.5],
        ...                  [0.5, 1./4., 2.0, 1.0, 2.0],
        ...                  [2.0, 0.5, 1./4., 2.0, 1.0]])
        >>> df = pd.DataFrame(data, index=vars_subset.names, columns=vars_subset.periods) 
        >>> df                                              # doctest: +NORMALIZE_WHITESPACE
              1991Y1  1992Y1  1993Y1  1994Y1  1995Y1
        ACAF	1.00	2.00	0.50	0.25	 2.0
        ACAG	2.00   -1.00	2.00   -1.00	 2.0
        AOUC	0.25	2.00	1.00	2.00	 0.5
        AOUC_	0.50	0.25	2.00	1.00	 2.0
        AQC	    2.00	0.50	0.25	2.00	 1.0
        >>> new_vars_subset = vars_subset ** df
        >>> new_vars_subset                                 # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Workspace: Variables
        nb variables: 5
        filename: ...fun.var
        description: Modèle fun - Simulation 1
        sample: 1991Y1:1995Y1
        mode: LEVEL
        <BLANKLINE>
         name       1991Y1  1992Y1   1993Y1  1994Y1   1995Y1
        ACAF 	     26.24	909.57	   5.89	   1.69	  172.42
        ACAG     	956.91	 -0.02	1862.61	  -0.06	 1751.09
        AOUC 	      1.01	  1.06	   1.03	   1.09	    1.02
        AOUC_	      0.98	  0.99	   0.96	   0.99	    0.99
        AQC  	      1.13	  1.05	   1.04	   1.34	    1.16
        <BLANKLINE>

        >>> # compute 'iode_var[name, period] ** array[name, period]' for each name and period
        >>> axis_names = la.Axis(name="names", labels=vars_subset.names)
        >>> axis_time = la.Axis(name="time", labels=vars_subset.periods)
        >>> array = la.Array(data, axes=(axis_names, axis_time))
        >>> array                                           # doctest: +NORMALIZE_WHITESPACE
        names\time  1991Y1  1992Y1  1993Y1  1994Y1  1995Y1
              ACAF     1.0     2.0     0.5    0.25     2.0
              ACAG     2.0    -1.0     2.0    -1.0     2.0
              AOUC    0.25     2.0     1.0     2.0     0.5
             AOUC_     0.5    0.25     2.0     1.0     2.0
               AQC     2.0     0.5    0.25     2.0     1.0
        >>> new_vars_subset = vars_subset ** array
        >>> new_vars_subset                                 # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Workspace: Variables
        nb variables: 5
        filename: ...fun.var
        description: Modèle fun - Simulation 1
        sample: 1991Y1:1995Y1
        mode: LEVEL
        <BLANKLINE>
         name       1991Y1  1992Y1   1993Y1  1994Y1  1995Y1
        ACAF 	     26.24	909.57	   5.89	  1.69	 172.42
        ACAG 	    956.91	 -0.02	1862.61	 -0.06	1751.09
        AOUC 	      1.01	  1.06	   1.03	  1.09	   1.02
        AOUC_	      0.98	  0.99	   0.96	  0.99	   0.99
        AQC  	      1.13	  1.05	   1.04	  1.34	   1.16
        <BLANKLINE>        

        >>> # WARNING: using a numpy ndarray to a (subset of a) Variables object is not recommended 
        >>> #          as there is no compatibility check between for the names and periods.
        >>> #          The result is not guaranteed to be the one you expected.
        >>> #          This possibility is provided for speed reasons 
        >>> #          (when dealing with large subsets/databases).
        >>> # compute 'iode_var[period] ** data[t]' for each period
        >>> data = np.array([1.0, 2.0, 0.5, 1./4., 2.0])
        >>> updated_ACAF = vars_subset["ACAF"] ** data
        >>> updated_ACAF                                    # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Workspace: Variables
        nb variables: 1
        filename: ...fun.var
        description: Modèle fun - Simulation 1
        sample: 1991Y1:1995Y1
        mode: LEVEL
        <BLANKLINE>
         name       1991Y1  1992Y1  1993Y1  1994Y1  1995Y1
        ACAF	     26.24	909.57	  5.89	  1.69	172.42
        <BLANKLINE>
        >>> # compute 'single_period_subset[name] ** data[i]' for each name
        >>> vars_subset_1995Y1 = vars_subset[:, "1995Y1"] ** data
        >>> vars_subset_1995Y1                              # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Workspace: Variables
        nb variables: 5
        filename: ...fun.var
        description: Modèle fun - Simulation 1
        sample: 1995Y1:1995Y1
        mode: LEVEL
        <BLANKLINE>
         name       1995Y1
        ACAF 	    -13.13
        ACAG       1751.09
        AOUC 	      1.02
        AOUC_	      1.00
        AQC  	      1.35
        <BLANKLINE>        
        >>> # compute 'iode_var[name, period] ** data[i, t]' for each name and period
        >>> data = np.array([[1.0, 2.0, 0.5, 1./4., 2.0],
        ...                  [2.0, -1.0, 2.0, -1.0, 2.0], 
        ...                  [1./4., 2.0, 1.0, 2.0, 0.5],
        ...                  [0.5, 1./4., 2.0, 1.0, 2.0],
        ...                  [2.0, 0.5, 1./4., 2.0, 1.0]])
        >>> new_vars_subset = vars_subset ** data
        >>> new_vars_subset                                 # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Workspace: Variables
        nb variables: 5
        filename: ...fun.var
        description: Modèle fun - Simulation 1
        sample: 1991Y1:1995Y1
        mode: LEVEL
        <BLANKLINE>
         name       1991Y1  1992Y1   1993Y1  1994Y1   1995Y1
        ACAF    	 26.24	909.57	   5.89	   1.69	  172.42
        ACAG    	956.91	 -0.02	1862.61	  -0.06	 1751.09
        AOUC 	      1.01	  1.06	   1.03	   1.09	    1.02
        AOUC_	      0.98	  0.99	   0.96	   0.99	    0.99
        AQC  	      1.13	  1.05	   1.04	   1.34	    1.16
        <BLANKLINE>
        """
        return self.__binary_op__(other, BinaryOperation.OP_POW, True)

    def from_numpy(self, data: np.ndarray, vars_names: Union[str, List[str]]=None, 
        first_period: Union[str, Period]=None, last_period: Union[str, Period]=None):
        """
        Copy the numpy ndarray `array` into the IODE Variables database.
        A row of the ndarray represents a variable.
        A column of the ndarray represents a period.

        Parameters
        ----------
        data: numpy ndarray
            Numpy ndarray containing the variables values to copy into the IODE Variables database.
        vars_names: str or list of str, optional
            Names of the variables to copy into the IODE Variables database.
            Default to all variables names found in the present database.
        first_period: str or Period, optional
            First period of the values to copy into the IODE Variables database. 
            Default to the first period of the present database.
        last_period: str or Period, optional
            Last period of the values to copy into the IODE Variables database. 
            Default to the last period of the present database.

        Warnings
        --------
        IODE and pandas don't use the same constant to represent NaN values.
        When loading a pandas DataFrame into the Variables database, the pandas 
        NaN values (:math:`nan`) are converted to IODE NaN values (:math:`NA`).

        See Also
        --------
        Variables.from_frame
        Variables.from_array

        Examples
        --------
        >>> from iode import variables, SAMPLE_DATA_DIR, Sample
        >>> import numpy as np
        >>> variables.load(f"{SAMPLE_DATA_DIR}/fun.var")        # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Loading .../fun.var
        394 objects loaded  
        >>> # define the list of Variables to override, the first and last periods to copy
        >>> vars_names = variables.get_names("A*")
        >>> vars_names
        ['ACAF', 'ACAG', 'AOUC', 'AOUC_', 'AQC']
        >>> first_period = "2000Y1"
        >>> last_period = "2010Y1"
        >>> sample = Sample(first_period, last_period)
        >>> nb_periods = sample.nb_periods
        >>> nb_periods
        11
        >>> # save original values to restore them later
        >>> original_values = variables["A*", "2000Y1:2010Y1"].to_numpy()
        >>> # create the numpy ndarray containing the values to copy into the Variables database
        >>> data = np.zeros((len(vars_names), nb_periods), dtype=float)
        >>> for i in range(len(vars_names)):
        ...     for j in range(nb_periods):
        ...         data[i, j] = i * nb_periods + j
        >>> data
        array([[  0.,   1.,   2.,   3.,   4.,   5.,   6.,   7.,   8.,   9.,  10.],
               [ 11.,  12.,  13.,  14.,  15.,  16.,  17.,  18.,  19.,  20.,  21.],
               [ 22.,  23.,  24.,  25.,  26.,  27.,  28.,  29.,  30.,  31.,  32.],
               [ 33.,  34.,  35.,  36.,  37.,  38.,  39.,  40.,  41.,  42.,  43.],
               [ 44.,  45.,  46.,  47.,  48.,  49.,  50.,  51.,  52.,  53.,  54.]])
        
        >>> variables["A*", "2000Y1:2010Y1"]                # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Workspace: Variables
        nb variables: 5
        filename: ...fun.var
        description: Modèle fun - Simulation 1
        sample: 2000Y1:2010Y1
        mode: LEVEL
        <BLANKLINE>
         name       2000Y1  2001Y1  2002Y1  2003Y1  2004Y1  2005Y1  2006Y1  2007Y1  2008Y1  2009Y1  2010Y1
        ACAF         10.05    2.87   -0.93   -6.09  -14.58  -26.54  -28.99  -33.38  -38.41  -37.46  -37.83
        ACAG        -41.53   18.94   19.98   21.02   22.07   23.11   24.13   25.16   26.19   27.23   28.25
        AOUC          1.12    1.14    1.16    1.17    1.17    1.18    1.20    1.22    1.26    1.29    1.31
        AOUC_         1.10    1.14    1.15    1.16    1.15    1.16    1.19    1.20    1.21    1.23    1.25
        AQC           1.34    1.38    1.41    1.42    1.40    1.40    1.40    1.41    1.43    1.45    1.46
        <BLANKLINE>
        >>> # copy the numpy ndarray into the Variables database (overriding the existing values)
        >>> variables.from_numpy(data, vars_names, first_period, last_period)
        >>> variables["A*", "2000Y1:2010Y1"]                # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Workspace: Variables
        nb variables: 5
        filename: ...fun.var
        description: Modèle fun - Simulation 1
        sample: 2000Y1:2010Y1
        mode: LEVEL
        <BLANKLINE>
         name       2000Y1  2001Y1  2002Y1  2003Y1  2004Y1  2005Y1  2006Y1  2007Y1  2008Y1  2009Y1  2010Y1
        ACAF          0.00    1.00    2.00    3.00    4.00    5.00    6.00    7.00    8.00    9.00   10.00
        ACAG         11.00   12.00   13.00   14.00   15.00   16.00   17.00   18.00   19.00   20.00   21.00
        AOUC         22.00   23.00   24.00   25.00   26.00   27.00   28.00   29.00   30.00   31.00   32.00
        AOUC_        33.00   34.00   35.00   36.00   37.00   38.00   39.00   40.00   41.00   42.00   43.00
        AQC          44.00   45.00   46.00   47.00   48.00   49.00   50.00   51.00   52.00   53.00   54.00
        <BLANKLINE>

        >>> # if a subset represents all values to be updated, the values for the arguments 
        >>> # vars_names, first_period and last_period can be omitted
        >>> vars_subset = variables["A*", "2000Y1:2010Y1"]
        >>> vars_subset.from_numpy(original_values)
        >>> vars_subset                # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Workspace: Variables
        nb variables: 5
        filename: ...fun.var
        description: Modèle fun - Simulation 1
        sample: 2000Y1:2010Y1
        mode: LEVEL
        <BLANKLINE>
         name       2000Y1  2001Y1  2002Y1  2003Y1  2004Y1  2005Y1  2006Y1  2007Y1  2008Y1  2009Y1  2010Y1
        ACAF         10.05    2.87   -0.93   -6.09  -14.58  -26.54  -28.99  -33.38  -38.41  -37.46  -37.83
        ACAG        -41.53   18.94   19.98   21.02   22.07   23.11   24.13   25.16   26.19   27.23   28.25
        AOUC          1.12    1.14    1.16    1.17    1.17    1.18    1.20    1.22    1.26    1.29    1.31
        AOUC_         1.10    1.14    1.15    1.16    1.15    1.16    1.19    1.20    1.21    1.23    1.25
        AQC           1.34    1.38    1.41    1.42    1.40    1.40    1.40    1.41    1.43    1.45    1.46
        <BLANKLINE>
        """
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

        if data.ndim == 1:
            # if the data is a 1D array, we assume that it is a single variable to update
            data = data.reshape(1, -1)
        
        if len(vars_names) != data.shape[0]:
            raise ValueError(f"The number of variables ({len(vars_names)}) to update is different "
                             f"from the number of rows of the numpy ndarray ({data.shape[0]}).\n"
                             f"Variables to updated are: {vars_names}")

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
        """
        Create a Numpy ndarray from the current Variables database.

        Returns
        -------
        np.ndarray
            Numpy ndarray containing the variables values of the current Variables database or subset.

        Examples
        --------
        >>> from iode import variables, SAMPLE_DATA_DIR
        >>> variables.load(f"{SAMPLE_DATA_DIR}/fun.var")        # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Loading .../fun.var
        394 objects loaded
        >>> len(variables)
        394
        >>> variables.sample
        Sample("1960Y1:2015Y1")
        >>> variables.nb_periods
        56

        >>> # export the whole Variables workspace to a numpy ndarray (394 variables x 56 periods)
        >>> data = variables.to_numpy()
        >>> data.shape
        (394, 56)
        >>> data[5, 40]
        442.26441085858613
        >>> variables.i[5, 40]
        442.26441085858613

        >>> # export a subset of names
        >>> vars_subset = variables["A*"]
        >>> vars_subset.names
        ['ACAF', 'ACAG', 'AOUC', 'AOUC_', 'AQC']
        >>> vars_subset                 # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Workspace: Variables
        nb variables: 5
        filename: ...fun.var
        description: Modèle fun - Simulation 1
        sample: 1960Y1:2015Y1
        mode: LEVEL
        <BLANKLINE>
         name       1960Y1  1961Y1  1962Y1  1963Y1  ...  2013Y1  2014Y1  2015Y1
        ACAF            na      na      na      na  ...  -68.89  -83.34  -96.41
        ACAG            na      na      na      na  ...   31.37   32.42   33.47
        AOUC            na    0.25    0.25    0.26  ...    1.39    1.42    1.46
        AOUC_           na      na      na      na  ...    1.34    1.37    1.41
        AQC           0.22    0.22    0.22    0.23  ...    1.56    1.61    1.67
        <BLANKLINE>
        >>> data = vars_subset.to_numpy()
        >>> data.shape
        (5, 56)
        >>> data                        # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        array([[            nan,          nan,          nan,          nan,
                                          ...
                   -55.55928982, -68.89465432, -83.34062511, -96.41041983],
                  [         nan,          nan,          nan,          nan,
                                          ...
                    30.32396115,  31.37013881,  32.42029883,  33.46960134],
                  [         nan,   0.24783192,   0.25456766,   0.26379573,
                                          ...
                     1.35553983,   1.38777697,   1.42371396,   1.46086261],
                  [         nan,          nan,          nan,          nan,
                                          ...
                     1.30459041,   1.33808573,   1.37301015,   1.4075568 ],
                  [  0.21753037,   0.21544869,   0.22228125,   0.22953896,
                                          ...
                     1.51366598,   1.55803879,   1.61318117,   1.67429058]])

        >>> # export a subset of names and periods
        >>> vars_subset = variables["A*", "2000Y1:2010Y1"]
        >>> vars_subset                 # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Workspace: Variables
        nb variables: 5
        filename: ...fun.var
        description: Modèle fun - Simulation 1
        sample: 2000Y1:2010Y1
        mode: LEVEL
        <BLANKLINE>
         name       2000Y1  2001Y1  2002Y1  2003Y1  ...  2007Y1  2008Y1  2009Y1  2010Y1
        ACAF         10.05    2.87   -0.93   -6.09  ...  -33.38  -38.41  -37.46  -37.83
        ACAG        -41.53   18.94   19.98   21.02  ...   25.16   26.19   27.23   28.25
        AOUC          1.12    1.14    1.16    1.17  ...    1.22    1.26    1.29    1.31
        AOUC_         1.10    1.14    1.15    1.16  ...    1.20    1.21    1.23    1.25
        AQC           1.34    1.38    1.41    1.42  ...    1.41    1.43    1.45    1.46
        <BLANKLINE>
        >>> data = vars_subset.to_numpy()
        >>> data.shape
        (5, 11)
        >>> data                        # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        array([[ 10.04661079,   2.86792274,  -0.92921251,  -6.09156499,
                -14.58209446, -26.53878957, -28.98728798, -33.37842578,
                -38.40951778, -37.46350964, -37.82742883],
               [-41.53478657,  18.93980114,  19.98081488,  21.02050218,
                 22.06647552,  23.10796216,  24.12963715,  25.16090905,
                 26.19211148,  27.22995512,  28.25392898],
               [  1.11623762,   1.14047639,   1.15716928,   1.17048954,
                  1.16767464,   1.1815207 ,   1.19946163,   1.21933288,
                  1.26280574,   1.28713178,   1.3071099 ],
               [  1.1019572 ,   1.13624426,   1.15021519,   1.16082895,
                  1.14802147,   1.16412337,   1.18589708,   1.19516611,
                  1.21383423,   1.23185399,   1.25016433],
               [  1.33860286,   1.37918825,   1.40881647,   1.41970458,
                  1.40065206,   1.39697298,   1.39806354,   1.40791334,
                  1.42564488,   1.44633167,   1.46286837]])
        """
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
        """
        Return a Numpy ndarray from the current Variables database.

        Returns
        -------
        np.ndarray
            Numpy ndarray containing the variables values of the current Variables database or subset.

        Examples
        --------
        >>> import numpy as np
        >>> from iode import variables, SAMPLE_DATA_DIR
        >>> variables.load(f"{SAMPLE_DATA_DIR}/fun.var")        # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Loading .../fun.var
        394 objects loaded
        >>> len(variables)
        394
        >>> variables.sample
        Sample("1960Y1:2015Y1")
        >>> variables.nb_periods
        56

        >>> # export the whole Variables workspace to a numpy ndarray (394 variables x 56 periods)
        >>> data = np.asarray(variables)
        >>> data.shape
        (394, 56)
        >>> data[5, 40]
        442.26441085858613
        >>> variables.i[5, 40]
        442.26441085858613

        >>> # export a subset of names
        >>> vars_subset = variables["A*"]
        >>> vars_subset.names
        ['ACAF', 'ACAG', 'AOUC', 'AOUC_', 'AQC']
        >>> vars_subset                 # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Workspace: Variables
        nb variables: 5
        filename: ...fun.var
        description: Modèle fun - Simulation 1
        sample: 1960Y1:2015Y1
        mode: LEVEL
        <BLANKLINE>
         name       1960Y1  1961Y1  1962Y1  1963Y1  ...  2013Y1  2014Y1  2015Y1
        ACAF            na      na      na      na  ...  -68.89  -83.34  -96.41
        ACAG            na      na      na      na  ...   31.37   32.42   33.47
        AOUC            na    0.25    0.25    0.26  ...    1.39    1.42    1.46
        AOUC_           na      na      na      na  ...    1.34    1.37    1.41
        AQC           0.22    0.22    0.22    0.23  ...    1.56    1.61    1.67
        <BLANKLINE>
        >>> data = np.asarray(vars_subset)
        >>> data.shape
        (5, 56)
        >>> data                        # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        array([[            nan,          nan,          nan,          nan,
                                          ...
                   -55.55928982, -68.89465432, -83.34062511, -96.41041983],
                  [         nan,          nan,          nan,          nan,
                                          ...
                    30.32396115,  31.37013881,  32.42029883,  33.46960134],
                  [         nan,   0.24783192,   0.25456766,   0.26379573,
                                          ...
                     1.35553983,   1.38777697,   1.42371396,   1.46086261],
                  [         nan,          nan,          nan,          nan,
                                          ...
                     1.30459041,   1.33808573,   1.37301015,   1.4075568 ],
                  [  0.21753037,   0.21544869,   0.22228125,   0.22953896,
                                          ...
                     1.51366598,   1.55803879,   1.61318117,   1.67429058]])

        >>> # export a subset of names and periods
        >>> vars_subset = variables["A*", "2000Y1:2010Y1"]
        >>> vars_subset                 # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Workspace: Variables
        nb variables: 5
        filename: ...fun.var
        description: Modèle fun - Simulation 1
        sample: 2000Y1:2010Y1
        mode: LEVEL
        <BLANKLINE>
         name       2000Y1  2001Y1  2002Y1  2003Y1  ...  2007Y1  2008Y1  2009Y1  2010Y1
        ACAF         10.05    2.87   -0.93   -6.09  ...  -33.38  -38.41  -37.46  -37.83
        ACAG        -41.53   18.94   19.98   21.02  ...   25.16   26.19   27.23   28.25
        AOUC          1.12    1.14    1.16    1.17  ...    1.22    1.26    1.29    1.31
        AOUC_         1.10    1.14    1.15    1.16  ...    1.20    1.21    1.23    1.25
        AQC           1.34    1.38    1.41    1.42  ...    1.41    1.43    1.45    1.46
        <BLANKLINE>
        >>> data = np.asarray(vars_subset)
        >>> data.shape
        (5, 11)
        >>> data                        # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        array([[ 10.04661079,   2.86792274,  -0.92921251,  -6.09156499,
                -14.58209446, -26.53878957, -28.98728798, -33.37842578,
                -38.40951778, -37.46350964, -37.82742883],
               [-41.53478657,  18.93980114,  19.98081488,  21.02050218,
                 22.06647552,  23.10796216,  24.12963715,  25.16090905,
                 26.19211148,  27.22995512,  28.25392898],
               [  1.11623762,   1.14047639,   1.15716928,   1.17048954,
                  1.16767464,   1.1815207 ,   1.19946163,   1.21933288,
                  1.26280574,   1.28713178,   1.3071099 ],
               [  1.1019572 ,   1.13624426,   1.15021519,   1.16082895,
                  1.14802147,   1.16412337,   1.18589708,   1.19516611,
                  1.21383423,   1.23185399,   1.25016433],
               [  1.33860286,   1.37918825,   1.40881647,   1.41970458,
                  1.40065206,   1.39697298,   1.39806354,   1.40791334,
                  1.42564488,   1.44633167,   1.46286837]])
        """
        data = self.to_numpy()
        if dtype is not None:
            data = data.astype(dtype)
        return data

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
        >>> nb_periods = len(periods_list)
        >>> data = np.zeros((len(vars_names), nb_periods), dtype=float)
        >>> for i in range(len(vars_names)):
        ...     for j in range(nb_periods):
        ...         data[i, j] = i * nb_periods + j
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
        >>> variables["VLA_00"]         # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Workspace: Variables
        nb variables: 1
        filename: ws
        sample: 1960Y1:1970Y1
        mode: LEVEL
        <BLANKLINE>
         name       1960Y1  1961Y1  1962Y1  ...  1968Y1  1969Y1  1970Y1
        VLA_00        0.00    1.00    2.00  ...    8.00    9.00   10.00
        <BLANKLINE>
        >>> variables["BXL_02"]         # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Workspace: Variables
        nb variables: 1
        filename: ws
        sample: 1960Y1:1970Y1
        mode: LEVEL
        <BLANKLINE>
         name       1960Y1  1961Y1  1962Y1  ...  1968Y1  1969Y1  1970Y1
        BXL_02       88.00   89.00   90.00  ...   96.00   97.00   98.00
        <BLANKLINE>
        """
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

        >>> # Export the IODE Variables database as a pandas DataFrame
        >>> variables                       # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Workspace: Variables
        nb variables: 394
        filename: ...fun.var
        description: Modèle fun - Simulation 1
        sample: 1960Y1:2015Y1
        mode: LEVEL
        <BLANKLINE>
         name       1960Y1  1961Y1  1962Y1  1963Y1  ...  2013Y1  2014Y1  2015Y1
        ACAF            na      na      na      na  ...  -68.89  -83.34  -96.41
        ACAG            na      na      na      na  ...   31.37   32.42   33.47
        AOUC            na    0.25    0.25    0.26  ...    1.39    1.42    1.46
        AOUC_           na      na      na      na  ...    1.34    1.37    1.41
        AQC           0.22    0.22    0.22    0.23  ...    1.56    1.61    1.67
        ...            ...     ...     ...     ...  ...     ...     ...     ...
        ZJ              na      na      na      na  ...    1.59    1.63    1.67
        ZKF           0.80    0.81    0.82    0.81  ...    0.87    0.87    0.87
        ZKFO          1.00    1.00    1.00    1.00  ...    1.02    1.02    1.02
        ZX            0.00    0.00    0.00    0.00  ...    0.00    0.00    0.00
        ZZF_          0.69    0.69    0.69    0.69  ...    0.69    0.69    0.69
        <BLANKLINE>
        >>> df = variables.to_frame()
        >>> df.shape
        (394, 56)
        >>> df.index.to_list()              # doctest: +ELLIPSIS
        ['ACAF', 'ACAG', 'AOUC', ..., 'ZKFO', 'ZX', 'ZZF_']
        >>> df.columns.to_list()            # doctest: +ELLIPSIS
        ['1960Y1', '1961Y1', ..., '2014Y1', '2015Y1']
        >>> df                              # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        time     1960Y1    1961Y1    1962Y1  ...     2013Y1     2014Y1     2015Y1
        names                                ...
        ACAF        NaN       NaN       NaN  ... -68.894654 -83.340625 -96.410420
        ACAG        NaN       NaN       NaN  ...  31.370139  32.420299  33.469601
        AOUC        NaN  0.247832  0.254568  ...   1.387777   1.423714   1.460863
        AOUC_       NaN       NaN       NaN  ...   1.338086   1.373010   1.407557
        AQC    0.217530  0.215449  0.222281  ...   1.558039   1.613181   1.674291
        ...         ...       ...       ...  ...        ...        ...        ...
        ZJ          NaN       NaN       NaN  ...   1.591981   1.630309   1.667971
        ZKF    0.802574  0.812873  0.819252  ...   0.874883   0.874351   0.873593
        ZKFO   1.000000  1.000000  1.000000  ...   1.015990   1.015990   1.015990
        ZX     0.000000  0.000000  0.000000  ...   0.000000   0.000000   0.000000
        ZZF_   0.688400  0.688400  0.688400  ...   0.688400   0.688400   0.688400
        <BLANKLINE>
        [394 rows x 56 columns]

        >>> # Export a subset of the IODE Variables database as a pandas DataFrame
        >>> vars_subset = variables["A*;*_", "2000Y1:2010Y1"]
        >>> vars_subset                     # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Workspace: Variables
        nb variables: 33
        filename: ...fun.var
        description: Modèle fun - Simulation 1
        sample: 2000Y1:2010Y1
        mode: LEVEL
        <BLANKLINE>
         name        2000Y1  2001Y1  2002Y1  2003Y1 ...  2007Y1   2008Y1   2009Y1   2010Y1
        ACAF          10.05    2.87   -0.93   -6.09 ...  -33.38   -38.41   -37.46   -37.83
        ACAG         -41.53   18.94   19.98   21.02 ...   25.16    26.19    27.23    28.25
        AOUC           1.12    1.14    1.16    1.17 ...    1.22     1.26     1.29     1.31
        AOUC_          1.10    1.14    1.15    1.16 ...    1.20     1.21     1.23     1.25
        AQC            1.34    1.38    1.41    1.42 ...    1.41     1.43     1.45     1.46
        ...             ...     ...     ...     ... ...     ...      ...      ...      ...
        WCF_        3716.45 3863.90 3999.57 4147.95 ... 4665.91  4916.65  5042.74  5170.60
        WIND_       1000.14 1035.22 1070.93 1102.91 ... 1178.12  1231.49  1268.86  1301.03
        WNF_        2334.76 2427.49 2512.87 2606.28 ... 2932.23  3089.99  3169.32  3249.75
        YDH_        7276.61 7635.91 7958.39 8331.07 ... 9685.61 10228.84 10630.74 10995.83
        ZZF_           0.69    0.69    0.69    0.69 ...    0.69     0.69     0.69     0.69
        <BLANKLINE>
        >>> df = vars_subset.to_frame()
        >>> df.shape
        (33, 11)
        >>> df.index.to_list()              # doctest: +ELLIPSIS
        ['ACAF', 'ACAG', 'AOUC', ..., 'WNF_', 'YDH_', 'ZZF_']
        >>> df.columns.to_list()            # doctest: +ELLIPSIS
        ['2000Y1', '2001Y1', '2002Y1', ..., '2008Y1', '2009Y1', '2010Y1']
        >>> df                              # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        time           2000Y1       2001Y1  ...        2009Y1        2010Y1
        names                               ...
        ACAF        10.046611     2.867923  ...    -37.463510    -37.827429
        ACAG       -41.534787    18.939801  ...     27.229955     28.253929
        AOUC         1.116238     1.140476  ...      1.287132      1.307110
        AOUC_        1.101957     1.136244  ...      1.231854      1.250164
        AQC          1.338603     1.379188  ...      1.446332      1.462868
        ...               ...          ...  ...           ...           ...
        WCF_      3716.447509  3863.897550  ...   5042.743118   5170.600010
        WIND_     1000.144577  1035.218800  ...   1268.861647   1301.025126
        WNF_      2334.763628  2427.492334  ...   3169.316544   3249.751702
        YDH_      7276.607740  7635.905667  ...  10630.736896  10995.831393
        ZZF_         0.688400     0.688400  ...      0.688400      0.688400
        <BLANKLINE>
        [33 rows x 11 columns]
        """
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
        >>> variables["VLA_00"]         # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Workspace: Variables
        nb variables: 1
        filename: ws
        sample: 1960Y1:1970Y1
        mode: LEVEL
        <BLANKLINE>
         name       1960Y1  1961Y1  1962Y1  ...  1968Y1  1969Y1  1970Y1
        VLA_00        0.00    1.00    2.00  ...    8.00    9.00   10.00
        <BLANKLINE>
        >>> variables["BXL_02"]         # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Workspace: Variables
        nb variables: 1
        filename: ws
        sample: 1960Y1:1970Y1
        mode: LEVEL
        <BLANKLINE>
         name       1960Y1  1961Y1  1962Y1  ...  1968Y1  1969Y1  1970Y1
        BXL_02       88.00   89.00   90.00  ...   96.00   97.00   98.00
        <BLANKLINE>
        """
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

        >>> # Export the IODE Variables database as an (larray) Array object
        >>> variables                       # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Workspace: Variables
        nb variables: 394
        filename: ...fun.var
        description: Modèle fun - Simulation 1
        sample: 1960Y1:2015Y1
        mode: LEVEL
        <BLANKLINE>
         name       1960Y1  1961Y1  1962Y1  1963Y1  ...  2013Y1  2014Y1   2015Y1
        ACAF            na      na      na      na  ...  -68.89  -83.34   -96.41
        ACAG            na      na      na      na  ...   31.37   32.42    33.47
        AOUC            na    0.25    0.25    0.26  ...    1.39    1.42     1.46
        AOUC_           na      na      na      na  ...    1.34    1.37     1.41
        AQC           0.22    0.22    0.22    0.23  ...    1.56    1.61     1.67
        ...            ...     ...     ...     ...  ...     ...     ...      ...
        ZJ              na      na      na      na  ...    1.59    1.63     1.67
        ZKF           0.80    0.81    0.82    0.81  ...    0.87    0.87     0.87
        ZKFO          1.00    1.00    1.00    1.00  ...    1.02    1.02     1.02
        ZX            0.00    0.00    0.00    0.00  ...    0.00    0.00     0.00
        ZZF_          0.69    0.69    0.69    0.69  ...    0.69    0.69     0.69
        <BLANKLINE>
        >>> array = variables.to_array()
        >>> array.shape
        (394, 56)
        >>> array.axes.info
        394 x 56
         names [394]: 'ACAF' 'ACAG' 'AOUC' ... 'ZKFO' 'ZX' 'ZZF_'
         time [56]: '1960Y1' '1961Y1' '1962Y1' ... '2013Y1' '2014Y1' '2015Y1'
        >>> array                           # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        names\\time      1960Y1  ...              2015Y1
               ACAF         nan  ...  -96.41041982848331
               ACAG         nan  ...   33.46960134488098
               AOUC         nan  ...  1.4608626117037322
              AOUC_         nan  ...  1.4075567973393608
                AQC  0.21753037  ...  1.6742905757021305
                ...         ...  ...                 ...
                 ZJ         nan  ...  1.6679707618363606
                ZKF  0.80257398  ...  0.8735925955073036
               ZKFO         1.0  ...           1.0159901
                 ZX         0.0  ...                 0.0
               ZZF_  0.68840039  ...          0.68840039

        >>> # Export a subset of the IODE Variables database as an (larray) Array object
        >>> vars_subset = variables["A*;*_", "2000Y1:2010Y1"]
        >>> vars_subset                     # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Workspace: Variables
        nb variables: 33
        filename: ...fun.var
        description: Modèle fun - Simulation 1
        sample: 2000Y1:2010Y1
        mode: LEVEL
        <BLANKLINE>
         name        2000Y1  2001Y1  2002Y1  2003Y1 ...  2007Y1   2008Y1   2009Y1   2010Y1
        ACAF          10.05    2.87   -0.93   -6.09 ...  -33.38   -38.41   -37.46   -37.83
        ACAG         -41.53   18.94   19.98   21.02 ...   25.16    26.19    27.23    28.25
        AOUC           1.12    1.14    1.16    1.17 ...    1.22     1.26     1.29     1.31
        AOUC_          1.10    1.14    1.15    1.16 ...    1.20     1.21     1.23     1.25
        AQC            1.34    1.38    1.41    1.42 ...    1.41     1.43     1.45     1.46
        ...             ...     ...     ...     ... ...     ...      ...      ...      ...
        WCF_        3716.45 3863.90 3999.57 4147.95 ... 4665.91  4916.65  5042.74  5170.60
        WIND_       1000.14 1035.22 1070.93 1102.91 ... 1178.12  1231.49  1268.86  1301.03
        WNF_        2334.76 2427.49 2512.87 2606.28 ... 2932.23  3089.99  3169.32  3249.75
        YDH_        7276.61 7635.91 7958.39 8331.07 ... 9685.61 10228.84 10630.74 10995.83
        ZZF_           0.69    0.69    0.69    0.69 ...    0.69     0.69     0.69     0.69
        <BLANKLINE>
        >>> array = vars_subset.to_array()
        >>> array.shape
        (33, 11)
        >>> array.axes.info
        33 x 11
         names [33]: 'ACAF' 'ACAG' 'AOUC' ... 'WNF_' 'YDH_' 'ZZF_'
         time [11]: '2000Y1' '2001Y1' '2002Y1' ... '2008Y1' '2009Y1' '2010Y1'
        >>> array                           # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        names\\time               2000Y1  ...               2010Y1
               ACAF   10.046610792200543  ...   -37.82742883229439
               ACAG   -41.53478656734795  ...   28.253928978210485
               AOUC   1.1162376230972206  ...   1.3071099004906368
              AOUC_     1.10195719812178  ...   1.2501643331956398
                AQC   1.3386028553645442  ...   1.4628683697450802
                ...                 ...  ...                 ...    
               WCF_   3716.4475089520292  ...    5170.600010384268
              WIND_   1000.1445769794319  ...    1301.025126372868
               WNF_   2334.7636275081923  ...   3249.7517024908175
               YDH_    7276.607740221424  ...   10995.831392939246
               ZZF_           0.68840039  ...           0.68840039
        """
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
    def first_period(self) -> Period:
        """
        First period of the current Variables database.
        """
        cdef CSample* c_sample
        if self.first_period_subset is not None:
            return self.first_period_subset 
        else:
            whole_db_sample: Sample = self._get_whole_sample()
            return whole_db_sample.start
    
    @property
    def last_period(self) -> Period:
        """
        Last period of the current Variables database.
        """
        cdef CSample* c_sample
        if self.last_period_subset is not None:
            return self.last_period_subset 
        else:
            whole_db_sample: Sample = self._get_whole_sample()
            return whole_db_sample.end

    @property
    def sample(self) -> Sample:
        """
        Current (or new) sample of the IODE Variables database.

        If a new sample is given, two cases are possible:
          
          - the sample is shorter than the current sample: the data beyond the new sample is destroyed,
          - the sample is longer than the current sample: the value `NA` (not available) is set for the added periods.

        Parameters
        ----------
        value: str or tuple(str, str)
            New sample as string 'start_period:last_period' or as a tuple 'start_period', 'last_period'.
        
        Warnings
        --------
        Changing the sample on a subset of the Variables workspace is not allowed unless the 
        :py:meth:`~iode.Variables.copy` method has been used (in that case, any change made on the subset 
        will not be reflected in the global Variables workspace).

        Examples
        --------
        >>> from iode import SAMPLE_DATA_DIR
        >>> from iode import variables
        >>> variables.load(f"{SAMPLE_DATA_DIR}/fun.var")        # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Loading .../fun.var
        394 objects loaded
        
        >>> variables.sample
        Sample("1960Y1:2015Y1")
        >>> variables['ACAF']                                   # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Workspace: Variables
        nb variables: 1
        filename: ...fun.var
        description: Modèle fun - Simulation 1
        sample: 1960Y1:2015Y1
        mode: LEVEL
        <BLANKLINE>
        name        1960Y1  1961Y1  ...  2014Y1   2015Y1
        ACAF            na      na  ...  -83.34   -96.41
        <BLANKLINE>

        >>> # -- update sample by passing a string
        >>> # case 1: new sample is shorter than the current sample
        >>> #         the data beyond the new sample is destroyed
        >>> variables.sample = '1970Y1:2010Y1'
        >>> variables.sample
        Sample("1970Y1:2010Y1")
        >>> variables['ACAF']                                   # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Workspace: Variables
        nb variables: 1
        filename: ...fun.var
        description: Modèle fun - Simulation 1
        sample: 1970Y1:2010Y1
        mode: LEVEL
        <BLANKLINE>
        name        1970Y1  1971Y1  ...  2009Y1   2010Y1
        ACAF          1.21    5.20  ...  -37.46   -37.83
        <BLANKLINE>
        >>> # case 2: new sample is longer than the current sample
        >>> #         the value NA (not available) is set for the added periods
        >>> variables.sample = '1968Y1:2012Y1'
        >>> variables.sample
        Sample("1968Y1:2012Y1")
        >>> variables['ACAF']                                   # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Workspace: Variables
        nb variables: 1
        filename: ...fun.var
        description: Modèle fun - Simulation 1
        sample: 1968Y1:2012Y1
        mode: LEVEL
        <BLANKLINE>
        name        1968Y1  1969Y1  1970Y1  ...  2010Y1 2011Y1   2012Y1
        ACAF            na      na    1.21  ...  -37.83     na       na
        <BLANKLINE>
        
        >>> # -- start period is optional -> the start period is kept as it is
        >>> variables.sample = ':2010Y1'
        >>> variables.sample
        Sample("1968Y1:2010Y1")
        >>> variables['ACAF']                                   # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Workspace: Variables
        nb variables: 1
        filename: ...fun.var
        description: Modèle fun - Simulation 1
        sample: 1968Y1:2010Y1
        mode: LEVEL
        <BLANKLINE>
        name        1968Y1  1969Y1  1970Y1  ...  2009Y1   2010Y1
        ACAF            na      na    1.21  ...  -37.46   -37.83
        <BLANKLINE>
        >>> # -- end period is optional -> the end period is kept as it is
        >>> variables.sample = '1970Y1:'
        >>> variables.sample
        Sample("1970Y1:2010Y1")
        >>> variables['ACAF']                                   # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Workspace: Variables
        nb variables: 1
        filename: ...fun.var
        description: Modèle fun - Simulation 1
        sample: 1970Y1:2010Y1
        mode: LEVEL
        <BLANKLINE>
        name        1970Y1  1971Y1  ...  2009Y1   2010Y1
        ACAF          1.21    5.20  ...  -37.46   -37.83
        <BLANKLINE>

        >>> # update sample by passing a start period and 
        >>> # an end period separated by a comma
        >>> variables.sample = '1968Y1', '2012Y1'
        >>> variables.sample
        Sample("1968Y1:2012Y1")
        >>> variables['ACAF']                                   # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Workspace: Variables
        nb variables: 1
        filename: ...fun.var
        description: Modèle fun - Simulation 1
        sample: 1968Y1:2012Y1
        mode: LEVEL
        <BLANKLINE>
        name        1968Y1  1969Y1  1970Y1  ...  2010Y1 2011Y1   2012Y1
        ACAF            na      na    1.21  ...  -37.83     na       na
        <BLANKLINE>
        """
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
        # self.sample calls self._maybe_update_subset_sample()
        return self.sample.nb_periods
        
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
        # self.sample calls self._maybe_update_subset_sample()
        return self.sample.get_period_list()

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
        # self.sample calls self._maybe_update_subset_sample()
        return self.sample.get_period_list(astype=float)

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
        >>> variables["AOUC"]               # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Workspace: Variables
        nb variables: 1
        filename: ...fun.var
        description: Modèle fun - Simulation 1
        sample: 1960Y1:2015Y1
        mode: LEVEL
        <BLANKLINE>
        name        1960Y1  1961Y1  ...  2014Y1   2015Y1
        AOUC            na    0.25  ...    1.42     1.46
        <BLANKLINE>
        >>> df.loc["AOUC"]                  # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        time
        1960Y1         NaN
        1961Y1    0.247832
        ...
        2014Y1    1.423714
        2015Y1    1.460863
        Name: AOUC, dtype: float64    
        >>> variables["ZKFO"]               # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Workspace: Variables
        nb variables: 1
        filename: ...fun.var
        description: Modèle fun - Simulation 1
        sample: 1960Y1:2015Y1
        mode: LEVEL
        <BLANKLINE>
        name        1960Y1  1961Y1  ...  2014Y1   2015Y1
        ZKFO          1.00    1.00  ...    1.02     1.02
        <BLANKLINE>
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
        >>> variables["AOUC"]               # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Workspace: Variables
        nb variables: 1
        filename: ...fun.var
        description: Modèle fun - Simulation 1
        sample: 1960Y1:2015Y1
        mode: LEVEL
        <BLANKLINE>
        name        1960Y1  1961Y1  ...  2014Y1   2015Y1
        AOUC            na    0.25  ...    1.42     1.46
        <BLANKLINE>
        >>> df.loc["AOUC"]                  # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        time
        1960Y1         NaN
        1961Y1    0.247832
        ...
        2014Y1    1.423714
        2015Y1    1.460863
        Name: AOUC, dtype: float64    
        >>> variables["ZZF_"]               # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Workspace: Variables
        nb variables: 1
        filename: ...fun.var
        description: Modèle fun - Simulation 1
        sample: 1960Y1:2015Y1
        mode: LEVEL
        <BLANKLINE>
        name        1960Y1  1961Y1  ...  2014Y1   2015Y1
        ZZF_          0.69    0.69  ...    0.69     0.69
        <BLANKLINE>
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
        >>> variables["ACAF", "2014Q1":"2014Q4"]    # doctest: +NORMALIZE_WHITESPACE
        Workspace: Variables
        nb variables: 1
        filename: ws
        sample: 2014Q1:2014Q4
        mode: LEVEL
        <BLANKLINE>
        name        2014Q1  2014Q2  2014Q3  2014Q4
        ACAF        -72.51  -76.12  -79.73  -83.34
        <BLANKLINE>
        >>> variables["ACAG", "2014Q1":"2014Q4"]    # doctest: +NORMALIZE_WHITESPACE
        Workspace: Variables
        nb variables: 1
        filename: ws
        sample: 2014Q1:2014Q4
        mode: LEVEL
        <BLANKLINE>
        name        2014Q1  2014Q2  2014Q3  2014Q4
        ACAG         31.63   31.90   32.16   32.42
        <BLANKLINE>

        Linear interpolation / flow
        
        >>> # "flow" -> the result is the source value divided by the nb of sub-periods. 
        >>> variables.low_to_high(LowToHighType.FLOW, LowToHighMethod.LINEAR, filepath, ["ACAF", "ACAG"])
        >>> variables["ACAF", "2014Q1":"2014Q4"]    # doctest: +NORMALIZE_WHITESPACE
        Workspace: Variables
        nb variables: 1
        filename: ws
        sample: 2014Q1:2014Q4
        mode: LEVEL
        <BLANKLINE>
        name        2014Q1  2014Q2  2014Q3  2014Q4
        ACAF        -20.84  -20.84  -20.84  -20.84
        <BLANKLINE>
        >>> variables["ACAG", "2014Q1":"2014Q4"]    # doctest: +NORMALIZE_WHITESPACE
        Workspace: Variables
        nb variables: 1
        filename: ws
        sample: 2014Q1:2014Q4
        mode: LEVEL
        <BLANKLINE>
        name        2014Q1  2014Q2  2014Q3  2014Q4
        ACAG          8.11    8.11    8.11    8.11
        <BLANKLINE>

        Cubic splines / stock
        
        >>> variables.low_to_high(LowToHighType.STOCK, LowToHighMethod.CUBIC_SPLINES, filepath, ["ACAF", "ACAG"])
        >>> variables["ACAF", "2012Q1":"2012Q4"]    # doctest: +NORMALIZE_WHITESPACE
        Workspace: Variables
        nb variables: 1
        filename: ws
        sample: 2012Q1:2012Q4
        mode: LEVEL
        <BLANKLINE>
        name        2012Q1  2012Q2  2012Q3  2012Q4
        ACAF        -47.30  -50.05  -52.81  -55.56
        <BLANKLINE>
        >>> variables["ACAG", "2012Q1":"2012Q4"]    # doctest: +NORMALIZE_WHITESPACE
        Workspace: Variables
        nb variables: 1
        filename: ws
        sample: 2012Q1:2012Q4
        mode: LEVEL
        <BLANKLINE>
        name        2012Q1  2012Q2  2012Q3  2012Q4
        ACAG         29.54   29.80   30.06   30.32
        <BLANKLINE>

        Cubic splines / flow
        
        >>> variables.low_to_high(LowToHighType.FLOW, LowToHighMethod.CUBIC_SPLINES, filepath, ["ACAF", "ACAG"])
        >>> variables["ACAF", "2012Q1":"2012Q4"]    # doctest: +NORMALIZE_WHITESPACE
        Workspace: Variables
        nb variables: 1
        filename: ws
        sample: 2012Q1:2012Q4
        mode: LEVEL
        <BLANKLINE>
        name        2012Q1  2012Q2  2012Q3  2012Q4
        ACAF        -12.75  -13.44  -14.27  -15.10
        <BLANKLINE>
        >>> variables["ACAG", "2012Q1":"2012Q4"]    # doctest: +NORMALIZE_WHITESPACE
        Workspace: Variables
        nb variables: 1
        filename: ws
        sample: 2012Q1:2012Q4
        mode: LEVEL
        <BLANKLINE>
        name        2012Q1  2012Q2  2012Q3  2012Q4
        ACAG          7.48    7.55    7.61    7.68
        <BLANKLINE>

        Step / stock
        
        >>> # "stock" -> the result has the same value as the source
        >>> variables.low_to_high(LowToHighType.STOCK, LowToHighMethod.STEP, filepath, ["ACAF", "ACAG"])
        >>> variables["ACAF", "2014Q1":"2014Q4"]    # doctest: +NORMALIZE_WHITESPACE
        Workspace: Variables
        nb variables: 1
        filename: ws
        sample: 2014Q1:2014Q4
        mode: LEVEL
        <BLANKLINE>
        name        2014Q1  2014Q2  2014Q3  2014Q4
        ACAF        -83.34  -83.34  -83.34  -83.34
        <BLANKLINE>
        >>> variables["ACAG", "2014Q1":"2014Q4"]    # doctest: +NORMALIZE_WHITESPACE
        Workspace: Variables
        nb variables: 1
        filename: ws
        sample: 2014Q1:2014Q4
        mode: LEVEL
        <BLANKLINE>
        name        2014Q1  2014Q2  2014Q3  2014Q4
        ACAG         32.42   32.42   32.42   32.42
        <BLANKLINE>

        Step / flow
        
        >>> # "flow" -> the result is the source value plus a portion of 
        >>> # the difference between the 2 surrounding values in the source
        >>> variables.low_to_high(LowToHighType.FLOW, LowToHighMethod.STEP, filepath, ["ACAF", "ACAG"])
        >>> variables["ACAF", "2014Q1":"2014Q4"]    # doctest: +NORMALIZE_WHITESPACE
        Workspace: Variables
        nb variables: 1
        filename: ws
        sample: 2014Q1:2014Q4
        mode: LEVEL
        <BLANKLINE>
        name        2014Q1  2014Q2  2014Q3  2014Q4
        ACAF        -20.84  -20.84  -20.84  -20.84
        <BLANKLINE>
        >>> variables["ACAG", "2014Q1":"2014Q4"]    # doctest: +NORMALIZE_WHITESPACE
        Workspace: Variables
        nb variables: 1
        filename: ws
        sample: 2014Q1:2014Q4
        mode: LEVEL
        <BLANKLINE>
        name        2014Q1  2014Q2  2014Q3  2014Q4
        ACAG          8.11    8.11    8.11    8.11
        <BLANKLINE>
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
        >>> variables["ACAF", "2010Y1":"2014Y1"]    # doctest: +NORMALIZE_WHITESPACE
        Workspace: Variables
        nb variables: 1
        filename: ws
        sample: 2010Y1:2014Y1
        mode: LEVEL
        <BLANKLINE>
        name        2010Y1  2011Y1  2012Y1  2013Y1  2014Y1
        ACAF        -37.83  -44.54  -55.56  -68.89  -83.34
        <BLANKLINE>
        >>> variables["ACAG", "2010Y1":"2014Y1"]    # doctest: +NORMALIZE_WHITESPACE
        Workspace: Variables
        nb variables: 1
        filename: ws
        sample: 2010Y1:2014Y1
        mode: LEVEL
        <BLANKLINE>
        name        2010Y1  2011Y1  2012Y1  2013Y1  2014Y1
        ACAG          7.06    7.32    7.58    7.84    8.11
        <BLANKLINE>

        Mean of year
        
        >>> variables.high_to_low(HighToLowType.MEAN, filepath, ["ACAF", "ACAG"])
        >>> variables["ACAF", "2010Y1":"2014Y1"]    # doctest: +NORMALIZE_WHITESPACE
        Workspace: Variables
        nb variables: 1
        filename: ws
        sample: 2010Y1:2014Y1
        mode: LEVEL
        <BLANKLINE>
        name        2010Y1  2011Y1  2012Y1  2013Y1  2014Y1
        ACAF        -37.83  -44.54  -55.56  -68.89  -83.34
        <BLANKLINE>
        >>> variables["ACAG", "2010Y1":"2014Y1"]    # doctest: +NORMALIZE_WHITESPACE
        Workspace: Variables
        nb variables: 1
        filename: ws
        sample: 2010Y1:2014Y1
        mode: LEVEL
        <BLANKLINE>
        name        2010Y1  2011Y1  2012Y1  2013Y1  2014Y1
        ACAG          7.06    7.32    7.58    7.84    8.11
        <BLANKLINE>

        Sum
        
        >>> variables.high_to_low(HighToLowType.SUM, filepath, ["ACAF", "ACAG"])
        >>> variables["ACAF", "2010Y1":"2014Y1"]    # doctest: +NORMALIZE_WHITESPACE
        Workspace: Variables
        nb variables: 1
        filename: ws
        sample: 2010Y1:2014Y1
        mode: LEVEL
        <BLANKLINE>
        name         2010Y1  2011Y1  2012Y1  2013Y1  2014Y1
        ACAF        -151.31 -178.18 -222.24 -275.58 -333.36
        <BLANKLINE>
        >>> variables["ACAG", "2010Y1":"2014Y1"]    # doctest: +NORMALIZE_WHITESPACE
        Workspace: Variables
        nb variables: 1
        filename: ws
        sample: 2010Y1:2014Y1
        mode: LEVEL
        <BLANKLINE>
        name        2010Y1  2011Y1  2012Y1  2013Y1  2014Y1
        ACAG         28.25   29.28   30.32   31.37   32.42
        <BLANKLINE>
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
        >>> variables["ACAF", :"2010Y1"]                # doctest: +NORMALIZE_WHITESPACE
        Workspace: Variables
        nb variables: 1
        filename: ws
        sample: 2000Y1:2010Y1
        mode: LEVEL
        <BLANKLINE>
        name        2000Y1  2001Y1  2002Y1  2003Y1  2004Y1  2005Y1  2006Y1  2007Y1  2008Y1  2009Y1  2010Y1
        ACAF          0.00    1.00    2.00    3.00    4.00      na    6.00      na    8.00    9.00   10.00
        <BLANKLINE>

        >>> # "TM1" (Y := Y[-1], if Y null or NA)
        >>> reset_ACAF()
        >>> variables.extrapolate(SimulationInitialization.TM1, "2005Y1", "2010Y1")
        >>> variables["ACAF", "2003Y1":"2009Y1"]        # doctest: +NORMALIZE_WHITESPACE
        Workspace: Variables
        nb variables: 1
        filename: ws
        sample: 2003Y1:2009Y1
        mode: LEVEL
        <BLANKLINE>
        name        2003Y1  2004Y1  2005Y1  2006Y1  2007Y1  2008Y1  2009Y1
        ACAF          3.00    4.00    4.00    6.00    6.00    8.00    9.00
        <BLANKLINE>

        >>> # "TM1_A" (Y := Y[-1], always)
        >>> reset_ACAF()
        >>> variables.extrapolate(SimulationInitialization.TM1_A, "2005Y1", "2010Y1")
        >>> variables["ACAF", "2003Y1":"2009Y1"]        # doctest: +NORMALIZE_WHITESPACE
        Workspace: Variables
        nb variables: 1
        filename: ws
        sample: 2003Y1:2009Y1
        mode: LEVEL
        <BLANKLINE>
        name        2003Y1  2004Y1  2005Y1  2006Y1  2007Y1  2008Y1  2009Y1
        ACAF          3.00    4.00    4.00    4.00    4.00    4.00    4.00
        <BLANKLINE>

        >>> # "EXTRA" (Y := extrapolation, if Y null or NA)
        >>> reset_ACAF()
        >>> variables.extrapolate(SimulationInitialization.EXTRA, "2005Y1", "2010Y1")
        >>> variables["ACAF", "2003Y1":"2009Y1"]        # doctest: +NORMALIZE_WHITESPACE
        Workspace: Variables
        nb variables: 1
        filename: ws
        sample: 2003Y1:2009Y1
        mode: LEVEL
        <BLANKLINE>
        name        2003Y1  2004Y1  2005Y1  2006Y1  2007Y1  2008Y1  2009Y1
        ACAF          3.00    4.00    5.00    6.00    7.00    8.00    9.00
        <BLANKLINE>

        >>> # "EXTRA_A" (Y := extrapolation, always)
        >>> reset_ACAF()
        >>> variables.extrapolate(SimulationInitialization.EXTRA_A, "2005Y1", "2010Y1")
        >>> variables["ACAF", "2003Y1":"2009Y1"]        # doctest: +NORMALIZE_WHITESPACE
        Workspace: Variables
        nb variables: 1
        filename: ws
        sample: 2003Y1:2009Y1
        mode: LEVEL
        <BLANKLINE>
        name        2003Y1  2004Y1  2005Y1  2006Y1  2007Y1  2008Y1  2009Y1
        ACAF          3.00    4.00    5.00    6.00    7.00    8.00    9.00
        <BLANKLINE>

        >>> # "ASIS" (Y unchanged)
        >>> reset_ACAF()
        >>> variables.extrapolate(SimulationInitialization.ASIS, "2005Y1", "2010Y1")
        >>> variables["ACAF", "2003Y1":"2009Y1"]        # doctest: +NORMALIZE_WHITESPACE
        Workspace: Variables
        nb variables: 1
        filename: ws
        sample: 2003Y1:2009Y1
        mode: LEVEL
        <BLANKLINE>
        name        2003Y1  2004Y1  2005Y1  2006Y1  2007Y1  2008Y1  2009Y1
        ACAF          3.00    4.00      na    6.00      na    8.00    9.00
        <BLANKLINE>

        >>> # "TM1_NA" (Y := Y[-1], if Y = NA)
        >>> reset_ACAF()
        >>> variables.extrapolate(SimulationInitialization.TM1_NA, "2005Y1", "2010Y1")
        >>> variables["ACAF", "2003Y1":"2009Y1"]        # doctest: +NORMALIZE_WHITESPACE
        Workspace: Variables
        nb variables: 1
        filename: ws
        sample: 2003Y1:2009Y1
        mode: LEVEL
        <BLANKLINE>
        name        2003Y1  2004Y1  2005Y1  2006Y1  2007Y1  2008Y1  2009Y1
        ACAF          3.00    4.00    4.00    6.00    6.00    8.00    9.00
        <BLANKLINE>

        >>> # "EXTRA_NA" (Y := extrapolation, if Y = NA)
        >>> reset_ACAF()
        >>> variables.extrapolate(SimulationInitialization.EXTRA_NA, "2005Y1", "2010Y1")
        >>> variables["ACAF", "2003Y1":"2009Y1"]        # doctest: +NORMALIZE_WHITESPACE
        Workspace: Variables
        nb variables: 1
        filename: ws
        sample: 2003Y1:2009Y1
        mode: LEVEL
        <BLANKLINE>
        name        2003Y1  2004Y1  2005Y1  2006Y1  2007Y1  2008Y1  2009Y1
        ACAF          3.00    4.00    5.00    6.00    7.00    8.00    9.00
        <BLANKLINE>
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
        >>> save_file = str(output_dir / "imported_var.var")
        >>> rule_file = f"{SAMPLE_DATA_DIR}/rules.txt"
        >>> debug_file = str(output_dir / "debug.log")

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
        >>> save_file = str(output_dir / "exported_var.csv")
        >>> debug_file = str(output_dir / "debug_csv.log")
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
        >>> save_file = str(output_dir / "exported_var.rcsv")
        >>> debug_file = str(output_dir / "debug_rcsv.log")
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
        >>> save_file = str(output_dir / "exported_var.tsp")
        >>> debug_file = str(output_dir / "debug_tsp.log")
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
        cdef t
        columns = {"name": names}
        names_pos: List[int] = [self.get_position(name) for name in names] 
        for str_period in self.periods:
            period = Period(str_period)
            t = self._get_real_period_position(period)
            columns[str_period] = [self.database_ptr.get_var(<int>pos, t, self.mode_) for pos in names_pos]
        return table2str(columns, max_lines=10, max_width=100, precision=2, justify_funcs={"name": JUSTIFY.LEFT})

    def print_to_file(self, filepath: Union[str, Path], names: Union[str, List[str]]=None, format: str=None) -> None:
        """
        Print the list variables defined by `names` to the file `filepath` using the format `format`.

        Argument `format` must be in the list:
        - 'H' (HTML file)
        - 'M' (MIF file)
        - 'R' (RTF file)
        - 'C' (CSV file)

        If argument `format` is null (default), the *A2M* format will be used to print the output.

        If the filename does not contain an extension, it is automatically added based on 
        the value of `format`.

        If `names` is a string, it is considered as a *pattern* and the function will print 
        all variables matching the pattern. The following characters in *pattern* have a 
        special meaning:
        
            - `*` : any character sequence, even empty
            - `?` : any character (one and only one)
            - `@` : any alphanumerical char [A-Za-z0-9]
            - `&` : any non alphanumerical char
            - `|` : any alphanumeric character or none at the beginning and end of a string 
            - `!` : any non-alphanumeric character or none at the beginning and end of a string 
            - `\` : escape the next character

        If `names` is None, print all variables of the (subset of the) current database.

        Parameters
        ----------
        filepath: str or Path
            path to the file to print.
            If the filename does not contain an extension, it is automatically 
            added based on the value of the format argument.
        names: str or list of str, optional
            pattern or list of names of the variables to print.
            If None, print all variables of the (subset of the) current database.
            Defaults to None.
        format: str, optional
            format of the output file. Possible values are: 'H' (HTML file), 
            'M' (MIF file), 'R' (RTF file) or 'C' (CSV file).
            Defaults to None meaning that the variables will be dumped in the *A2M* format.

        Examples
        --------
        >>> from iode import variables, SAMPLE_DATA_DIR
        >>> variables.load(f"{SAMPLE_DATA_DIR}/fun.var")         # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Loading .../fun.var
        394 objects loaded
        >>> variables.print_to_file(output_dir / "variables.csv", ["ACAF", "ACAG"])       # doctest: +ELLIPSIS
        Printing IODE objects definition to file '...variables.csv'...
        Printing ACAF ...
        Printing ACAG ...
        Print done
        >>> with open(output_dir / "variables.csv") as f:                     # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        ...     print(f.read())
        ...
        <BLANKLINE>
        "Name","1960Y1","1961Y1","1962Y1","1963Y1",...,"2012Y1","2013Y1","2014Y1","2015Y1",
        <BLANKLINE>
        "ACAF","#N/A","#N/A","#N/A","#N/A",...,"-55.55929","-68.894654","-83.340625","-96.41042",
        "ACAG","#N/A","#N/A","#N/A","#N/A",...,"30.323961","31.370139","32.420299","33.469601",
        <BLANKLINE>
        """
        self._print_to_file(filepath, names, format)

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
        >>> original_variable = variables["ACAF"].copy()
        >>> variables["ACAF"] = 0.0
        >>> original_hash == hash(variables)
        False
        >>> variables["ACAF"] = original_variable  # revert the change
        >>> original_hash == hash(variables)
        True

        >>> # delete a variable
        >>> original_variable = variables["ACAF"].copy()
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
