# distutils: language = c++

from collections.abc import Iterable
from typing import Union, Tuple, List, Dict, Optional, Any
if sys.version_info.minor >= 11:
    from typing import Self
else:
    Self = Any

cimport cython
from pyiode.iode_database.cpp_api_database cimport KDBScalars as CKDBScalars
from pyiode.iode_database.cpp_api_database cimport Scalars as cpp_global_scalars

ScalarInput = Union[int, float, List[float], Tuple[float, float], Dict[str, float], Scalar]


@cython.final
cdef class Scalars(_AbstractDatabase):
    """
    IODE Scalars database. 

    Attributes
    ----------
    filename: str
    description: str

    Parameters
    ----------
    filepath: str, optional
        file containing the IODE scalars to load.

    Returns
    -------
    Scalars

    Examples
    --------
    >>> from iode import scalars, SAMPLE_DATA_DIR
    >>> scalars.load(f"{SAMPLE_DATA_DIR}/fun.scl")
    >>> len(scalars)
    161
    """
    cdef bint ptr_owner
    cdef CKDBScalars* database_ptr

    def __cinit__(self, filepath: str = None) -> Scalars:
        self.database_ptr = NULL
        self.abstract_db_ptr = NULL
        self.ptr_owner = False

    def __init__(self, filepath: str = None):
        # Prevent accidental instantiation from normal Python code
        # since we cannot pass a struct pointer into a Python constructor.
        raise TypeError("This class cannot be instantiated directly.")

    def __dealloc__(self):
        # if self.database_ptr points to the C++ global instance Scalars 
        # which does not need to be manually deleted 
        if self.ptr_owner and self.database_ptr is not NULL:
            del self.database_ptr
            self.database_ptr = NULL

    # see https://cython.readthedocs.io/en/stable/src/userguide/extension_types.html#instantiation-from-existing-c-c-pointers 
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

    # TODO: implement KDBAbstract::load() method (for global KDB only)
    def _load(self, filepath: str):
        cdef CKDBScalars* kdb = new CKDBScalars(filepath.encode())
        del kdb

    def _subset(self, pattern: str, copy: bool) -> Scalars:
        cdef Scalars subset_db = Scalars.__new__(Scalars)
        subset_db.database_ptr = subset_db.abstract_db_ptr = self.database_ptr.subset(pattern.encode(), <bint>copy)
        return subset_db

    def _get_object(self, key):
        if not isinstance(key, str):
            raise TypeError(f"Cannot get object {key}.\nExpected a string value for {key} " + 
                            f"but got value of type {type(key).__name__}")
        key = key.strip()

        cdef CScalar c_scalar = self.database_ptr.get(key.encode())
        py_scalar = Scalar(c_scalar.val, c_scalar.relax)
        py_scalar.c_scalar.std = c_scalar.std
        return py_scalar

    def _set_object(self, key, value):
        if not isinstance(key, str):
            raise TypeError(f"Cannot set object {key}.\nExpected a string value for {key} " + 
                            f"but got value of type {type(key).__name__}")
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

    def from_series(self, s: Series):
        r"""
        Copy the pandas Series `s` into the IODE Scalars database.
        The scalar names to copy are deduced from the index of the Series.

        Parameters
        ----------
        s: Series
            pandas Series containing the scalars values to copy into the Scalars database.
            The 'relax' value of all scalars will be set to 1.

        Notes
        -----
        The index of the passed Series is sorted in alphabetical order before 
        copying to IODE Scalars database.

        See Also
        --------
        Scalars.from_frame
        Scalars.to_frame

        Examples
        --------
        >>> from iode import scalars
        >>> import pandas as pd
        >>> scalars.clear()
        >>> len(scalars)
        0

        >>> # create the pandas Series
        >>> data = {"alpha_0": 0.9, "alpha_1": 0.1, "alpha_": 5.5,
        ...         "beta_0": 0.9, "beta_1": 0.01, "beta_": 3.6}
        >>> s = pd.Series(data, dtype="float64")
        >>> # display the pandas series
        >>> s          # doctest: +NORMALIZE_WHITESPACE
        alpha_0    0.90
        alpha_1    0.10
        alpha_     5.50
        beta_0     0.90
        beta_1     0.01
        beta_      3.60
        dtype: float64

        >>> # load into the IODE Scalars database
        >>> scalars.from_series(s)
        >>> len(scalars)
        6
        
        >>> scalars.get_names()
        ['alpha_', 'alpha_0', 'alpha_1', 'beta_', 'beta_0', 'beta_1']
        >>> scalars["alpha_1"]
        Scalar(0.1, 1, na)
        >>> scalars["beta_"]
        Scalar(3.6, 1, na)
        """
        if pd is None:
            raise RuntimeError("pandas library not found")

        for index, value in s.items():
            self._set_object(index, value)

    def from_frame(self, df: DataFrame):
        r"""
        Copy the pandas DataFrame `df` into the IODE Scalars database.
        The scalar names to copy are deduced from the index of the DataFrame.

        Parameters
        ----------
        df: DataFrame
            pandas DataFrame containing the scalars to copy into the Scalars database.
            The passed DataFrame should have the following structure:
                - index: names of the scalars
                - a column labeled 'value': values of the scalars.
                - a column labeled 'relax' (optional): relaxation parameter values of the scalars. 
                - a column labeled 'std' (optional): standard deviation values resulting from estimation.
            If the DataFrame does not contain a 'relax' column, relax values will be all set to 1.
            If the DataFrame does not contain a 'std' column, std values will be all set to :math:`NA`.

        Notes
        -----
        The index of the passed DataFrame is sorted in alphabetical order before 
        copying to IODE Scalars database.

        See Also
        --------
        Scalars.from_series
        Scalars.to_frame

        Examples
        --------
        >>> from iode import scalars
        >>> import pandas as pd
        >>> import numpy as np
        >>> scalars.clear()
        >>> len(scalars)
        0

        >>> # create the pandas DataFrame
        >>> data = {"alpha_0": [0.9, 1., np.nan],
        ...         "alpha_1": [0.1, 0.8, np.nan],
        ...         "alpha_": [5.5, 0., np.nan],
        ...         "beta_0": [0.9, 1., 0.05246],
        ...         "beta_1": [0.01, 0.8, 0.25687],
        ...         "beta_": [3.6, 0., 0.]}
        >>> df = pd.DataFrame.from_dict(data, orient='index', dtype="float64", 
        ...                             columns=["value", "relax", "std"])
        >>> # display the pandas series
        >>> df          # doctest: +NORMALIZE_WHITESPACE
                value  relax      std
        alpha_0   0.90    1.0      NaN
        alpha_1   0.10    0.8      NaN
        alpha_    5.50    0.0      NaN
        beta_0    0.90    1.0  0.05246
        beta_1    0.01    0.8  0.25687
        beta_     3.60    0.0  0.00000

        >>> # load into the IODE Scalars database
        >>> scalars.from_frame(df)
        >>> len(scalars)
        6
        
        >>> scalars.get_names()
        ['alpha_', 'alpha_0', 'alpha_1', 'beta_', 'beta_0', 'beta_1']
        >>> df.loc["alpha_1"]
        value    0.1
        relax    0.8
        std      NaN
        Name: alpha_1, dtype: float64
        >>> scalars["alpha_1"]
        Scalar(0.1, 0.8, na)
        >>> df.loc["beta_1"]
        value    0.01000
        relax    0.80000
        std      0.25687
        Name: beta_1, dtype: float64
        >>> scalars["beta_1"]
        Scalar(0.01, 0.8, 0.25687)
        """
        if pd is None:
            raise RuntimeError("pandas library not found")

        if 'value' not in df.columns:
            raise ValueError("Expected at least one column with name 'value'. "
                             f"Got dataframe with column names {df.columns.to_list()}")

        if 'relax' not in df.columns:
            self.from_series(df['value'])
        else:
            is_std_values = 'std' in df.columns
            for row in df.itertuples(name='Scalar'):
                scalar = Scalar(row.value, row.relax)
                if is_std_values:
                    scalar.c_scalar.std = row.std
                self._set_object(row[0], scalar)

    def to_frame(self) -> DataFrame:
        r"""
        Create a pandas DataFrame from the current Scalars database.
        The index of the returned DataFrame is build from the Scalars names.

        See Also
        --------
        Scalars.from_series
        Scalars.from_frame

        Examples
        --------
        >>> from iode import SAMPLE_DATA_DIR, scalars
        >>> import pandas as pd
        >>> scalars.load(f"{SAMPLE_DATA_DIR}/fun.scl")
        >>> len(scalars)
        161

        >>> # Export the IODE Scalars database as a pandas DataFrame
        >>> df = scalars.to_frame()
        >>> len(df)
        161

        >>> df.index.to_list()              # doctest: +ELLIPSIS
        ['acaf1', 'acaf2', 'acaf3', ..., 'zkf1', 'zkf2', 'zkf3']
        >>> scalars["acaf1"]                # doctest: +NORMALIZE_WHITESPACE
        Scalar(0.0157684, 1, 0.00136871)
        >>> df.loc["acaf1"]                 # doctest: +NORMALIZE_WHITESPACE
        value    0.015768
        relax    1.000000
        std      0.001369
        Name: acaf1, dtype: float64
        >>> scalars["qc0_"]                 # doctest: +NORMALIZE_WHITESPACE
        Scalar(0.178165, 1, 0.102838)
        >>> df.loc["qc0_"]                  # doctest: +NORMALIZE_WHITESPACE
        value    0.178165
        relax    1.000000
        std      0.102838
        Name: qc0_, dtype: float64

        >>> # Export a subset of the IODE Scalars database as a pandas DataFrame
        >>> df = scalars["a*;*_"].to_frame()
        >>> len(df)
        19

        >>> df.index.to_list()              # doctest: +ELLIPSIS
        ['acaf1', 'acaf2', 'acaf3', ..., 'vs1_', 'vs2_', 'vs3_']
        >>> scalars["acaf1"]                # doctest: +NORMALIZE_WHITESPACE
        Scalar(0.0157684, 1, 0.00136871)
        >>> df.loc["acaf1"]                 # doctest: +NORMALIZE_WHITESPACE
        value    0.015768
        relax    1.000000
        std      0.001369
        Name: acaf1, dtype: float64
        >>> scalars["qc0_"]                 # doctest: +NORMALIZE_WHITESPACE
        Scalar(0.178165, 1, 0.102838)
        >>> df.loc["qc0_"]                  # doctest: +NORMALIZE_WHITESPACE
        value    0.178165
        relax    1.000000
        std      0.102838
        Name: qc0_, dtype: float64
        """
        if pd is None:
            raise RuntimeError("pandas library not found")
        
        data = {name: self._get_object(name)._as_tuple() for name in self.get_names()}
        return pd.DataFrame.from_dict(data, orient='index', dtype="float64", columns=["value", "relax", "std"])


scalars: Scalars = Scalars._from_ptr()
