import sys
from pathlib import Path
from typing import Union, Tuple, List, Dict, Optional, Any
if sys.version_info.minor >= 11:
    from typing import Self
else:
    Self = Any

import pandas as pd
from iode.util import table2str, JUSTIFY
from iode.objects.scalar import Scalar
from iode.iode_cython import PositionalIndexer
from iode.iode_cython import Scalars as CythonScalars

ScalarInput = Union[int, float, List[float], Tuple[float, float], Dict[str, float], Scalar]


class Scalars(CythonScalars):
    r"""
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
    >>> scalars.load(f"{SAMPLE_DATA_DIR}/fun.scl")      # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
    Loading .../fun.scl
    161 objects loaded 
    >>> len(scalars)
    161
    >>> scalars                                         # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
    Workspace: Scalars
    nb scalars: 161
    filename: ...\tests\data\fun.scl
    <BLANKLINE>
     name        value  relax    std
    acaf1        0.0158 1.0000  0.0014
    acaf2       -0.0000 1.0000  0.0000
    acaf3        2.5026 1.0000  0.8730
    acaf4       -0.0085 1.0000  0.0021
    dlnpaf       0.9000 1.0000      na
    ...             ...    ...     ...
    y6           0.0000 0.0000  0.0000
    y7          -0.1550 1.0000  0.3239
    zkf1         0.2011 1.0000  0.3757
    zkf2         0.7921 1.0000  0.1812
    zkf3        -7.2712 1.0000  2.6764
    <BLANKLINE>
    """
    def __init__(self, filepath: str=None):
        raise TypeError("This class cannot be instantiated directly.")

    @staticmethod
    def __init_instance(instance: Self) -> Self:
        return CythonScalars.__init_instance(instance)

    @classmethod
    def get_instance(cls) -> Self:
        instance = cls.__new__(cls)
        return cls.__init_instance(instance)

    @classmethod
    def _new_instance(cls) -> Self:
        instance = cls.__new__(cls)
        return instance

    def _load(self, filepath: str):
        CythonScalars._load(self, filepath)

    def _subset(self, pattern: str, copy: bool) -> Self:
        return CythonScalars._subset(self, pattern, copy)

    @property
    def i(self) -> PositionalIndexer:
        r"""
        Allow to select the ith scalar in the database.

        Examples
        --------
        >>> from iode import scalars, SAMPLE_DATA_DIR
        >>> scalars.load(f"{SAMPLE_DATA_DIR}/fun.scl")       # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Loading .../fun.scl
        161 objects loaded 
        >>> # get the first scalar
        >>> scalars.i[0]
        Scalar(0.0157684, 1, 0.00136871)
        >>> # get the last scalar
        >>> scalars.i[-1]
        Scalar(-7.27124, 1, 2.6764)
        >>> # update first scalar
        >>> scalars.i[0] = 0.02
        >>> scalars.i[0]
        Scalar(0.02, 1, na)
        >>> # update last scalar
        >>> scalars.i[-1] = -7.3, 0.5
        >>> scalars.i[-1]
        Scalar(-7.3, 0.5, na)
        """
        return PositionalIndexer(self)

    def _get_object(self, key: Union[str, int]) -> Scalar:
        scalar = Scalar._new_instance()
        return CythonScalars._get_object(self, key, scalar)

    def _set_object(self, key: Union[str, int], value):
        CythonScalars._set_object(self, key, value)

    def __getitem__(self, key: Union[str, List[str]]) -> Union[Scalar, Self]:
        r"""
        Return the (subset of) scalar(s) referenced by `key`.

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
            (the list of) name(s) of the scalar(s) to get.
            The list of scalars to get can be specified by a pattern or by a list of sub-patterns (e.g. "A*;*_").

        Returns
        -------
        Single scalar or a subset of the database.

        Examples
        --------
        >>> from iode import SAMPLE_DATA_DIR
        >>> from iode import scalars
        >>> scalars.load(f"{SAMPLE_DATA_DIR}/fun.scl")       # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Loading .../fun.scl
        161 objects loaded 

        >>> # a) get one scalar
        >>> acaf1 = scalars["acaf1"]
        >>> acaf1
        Scalar(0.0157684, 1, 0.00136871)
        >>> acaf1.value
        0.01576840691268444
        >>> acaf1.relax
        1.0
        >>> acaf1.std
        0.0013687137980014086

        >>> # b) get a subset of the Scalars database using a pattern
        >>> scalars_subset = scalars["a*"]
        >>> scalars_subset.names
        ['acaf1', 'acaf2', 'acaf3', 'acaf4']

        >>> # c) get a subset of the Scalars database using a list of names
        >>> scalars_subset = scalars[["acaf1", "acaf4", "dpuh_1", "dpuh_2"]]
        >>> scalars_subset.names
        ['acaf1', 'acaf4', 'dpuh_1', 'dpuh_2']
        """
        return super().__getitem__(key)

    def __setitem__(self, key: Union[str, List[str]], value: Union[float, Tuple[float, float], Scalar, Dict[str, Any], List[Union[float, Tuple[float, float], Scalar, Dict[str, Any]]]]):
        r"""
        Update/add a (subset of) scalar(s) referenced by `key` from/to the Scalars database.

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
            (the list of) name(s) of the scalar(s) to update/add.
            The list of scalars to update/add can be specified by a pattern or by a list of sub-patterns 
            (e.g. "A*;*_").
        value: float or tuple(float, float) or Scalar or dict(str, ...) or pd.Series or pd.DataFrame or Scalars
            If float, then it is interpreted as the value of the scalar.
            If tuple of two float, then it is interpreted as the value and relax of the scalar.
            The standard deviation is computed during the estimation process and cannot be modified manually.
            If Scalar, then it is used to update an existing scalar or to create a new scalar if it does not exist yet.
            If pandas Series, then it is interpreted as a list of couples (scalar name, scalar value).
            If pandas DataFrame, then only the columns names 'value' and 'relax' are allowed.

        See Also
        --------
        iode.Scalar

        Examples
        --------
        >>> import pandas as pd
        >>> from iode import SAMPLE_DATA_DIR
        >>> from iode import scalars, Scalar
        >>> scalars.load(f"{SAMPLE_DATA_DIR}/fun.scl")       # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Loading .../fun.scl
        161 objects loaded 
        
        >>> # a) -------- add one scalar -------- 
        >>> # 1. default relax to 1.0
        >>> scalars["a0"] = 0.1
        >>> scalars["a0"]
        Scalar(0.1, 1, na)
        >>> # 2. value + relax
        >>> scalars["a1"] = 0.1, 0.9
        >>> scalars["a1"]
        Scalar(0.1, 0.9, na)

        >>> # b) -------- update one scalar --------
        >>> scalars["acaf1"]
        Scalar(0.0157684, 1, 0.00136871)
        >>> # only update the value
        >>> scalars["acaf1"] = 0.8
        >>> scalars["acaf1"]
        Scalar(0.8, 1, na)
        >>> # update value and relax (tuple)
        >>> scalars["acaf2"] = 0.8, 0.9
        >>> scalars["acaf2"]
        Scalar(0.8, 0.9, na)
        >>> # update value and relax (list)
        >>> scalars["acaf2"] = (0.7, 0.8)
        >>> scalars["acaf2"]
        Scalar(0.7, 0.8, na)
        >>> # update value and relax (dictionary)
        >>> scalars["acaf3"] = {"relax": 0.9, "value": 0.8}
        >>> scalars["acaf3"]
        Scalar(0.8, 0.9, na)
        >>> # update value and/or relax (Scalar object)
        >>> # NOTE: the standard deviation (std) cannot be changed manually
        >>> scalars["acaf4"]
        Scalar(-0.00850518, 1, 0.0020833)
        >>> scalars["acaf4"].value = 0.8
        >>> scalars["acaf4"].relax = 0.9
        >>> scalars["acaf4"]
        Scalar(0.8, 0.9, na)

        >>> # c) add/update multiple scalars at once
        >>> # 1) using a dict of values
        >>> values = {"acaf1": 0.016, "acaf2": (-8.e-04, 0.9), "acaf3": Scalar(2.5)}
        >>> scalars["acaf1, acaf2, acaf3"] = values
        >>> scalars["acaf1, acaf2, acaf3"]          # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Workspace: Scalars
        nb scalars: 3
        filename: ...fun.scl
        <BLANKLINE>
         name	 value 	relax   std  
        acaf1	 0.0160	1.0000	 na
        acaf2	-0.0008	0.9000	 na
        acaf3	 2.5000	1.0000	 na
        <BLANKLINE>

        >>> # 2) using a pandas series (only scalar's values)
        >>> data = [0.015, -9.e-04, 2.8]
        >>> series = pd.Series(data, index=["acaf1", "acaf2", "acaf3"])
        >>> scalars["acaf1, acaf2, acaf3"] = series
        >>> scalars["acaf1, acaf2, acaf3"]          # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Workspace: Scalars
        nb scalars: 3
        filename: ...fun.scl
        <BLANKLINE>
         name	 value 	relax 	std  
        acaf1	 0.0150	1.0000	 na
        acaf2	-0.0009	0.9000	 na
        acaf3	 2.8000	1.0000	 na
        <BLANKLINE>

        >>> # 3) using a pandas DataFrame (value + relax)
        >>> data = [(0.014, 0.98), (-7.e-04, 0.95), (2.3, 0.92)]
        >>> df = pd.DataFrame(data, index=["acaf1", "acaf2", "acaf3"], columns=["value", "relax"])
        >>> scalars["acaf1, acaf2, acaf3"] = df
        >>> scalars["acaf1, acaf2, acaf3"]          # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Workspace: Scalars
        nb scalars: 3
        filename: ...fun.scl
        <BLANKLINE>
         name	 value 	relax 	std  
        acaf1	 0.0140	0.9800	 na
        acaf2	-0.0007	0.9500	 na
        acaf3	 2.3000	0.9200	 na
        <BLANKLINE>

        >>> # 4) using another Scalars database (subset)
        >>> scalars_subset = scalars["acaf1, acaf2, acaf3"].copy()
        >>> scalars_subset["acaf1"] = 0.02
        >>> scalars_subset["acaf2"] = (-5.e-04, 0.94)
        >>> scalars_subset["acaf3"] = Scalar(2.9)
        >>> scalars["acaf1, acaf2, acaf3"] = scalars_subset
        >>> scalars["acaf1, acaf2, acaf3"]          # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Workspace: Scalars
        nb scalars: 3
        filename: ...fun.scl
        <BLANKLINE>
         name	 value 	relax 	std  
        acaf1    0.0200 0.9800   na
        acaf2   -0.0005 0.9400   na
        acaf3    2.9000 1.0000   na
        <BLANKLINE>

        >>> # d) working on a subset
        >>> # 1) get subset
        >>> scalars_subset = scalars["a*"]
        >>> scalars_subset.names
        ['a0', 'a1', 'acaf1', 'acaf2', 'acaf3', 'acaf4']
        >>> # 2) add a scalar to the subset 
        >>> scalars_subset["acaf0"] = 1.0, 1.0
        >>> scalars_subset["acaf0"]
        Scalar(1, 1, na)
        >>> # --> new scalar also appears in the global workspace
        >>> "acaf0" in scalars
        True
        >>> scalars["acaf0"]
        Scalar(1, 1, na)
        >>> # 3) update a scalar in the subset
        >>> scalars_subset["acaf0"] = 0.1
        >>> scalars_subset["acaf0"]
        Scalar(0.1, 1, na)
        >>> # --> scalar is also updated in the global workspace
        >>> scalars["acaf0"]
        Scalar(0.1, 1, na)
        """
        super().__setitem__(key, value)

    def __delitem__(self, key):
        r"""
        Remove the (subset of) scalar(s) referenced by `key` from the Scalars database.

        Parameters
        ----------
        key: str or list(str)
            (list of) name(s) of the scalar(s) to be removed.
            The list of names can be given as a string pattern (e.g. "A*;*_").

        Examples
        --------
        >>> from iode import SAMPLE_DATA_DIR
        >>> from iode import scalars
        >>> scalars.load(f"{SAMPLE_DATA_DIR}/fun.scl")       # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Loading .../fun.scl
        161 objects loaded 

        >>> # a) delete one scalar
        >>> scalars.get_names("a*")
        ['acaf1', 'acaf2', 'acaf3', 'acaf4']
        >>> del scalars["acaf4"]
        >>> scalars.get_names("a*")
        ['acaf1', 'acaf2', 'acaf3']

        >>> # b) delete several scalars at once using a pattern
        >>> del scalars["a*"]
        >>> scalars.get_names("a*")
        []

        >>> # c) delete several scalars at once using a list of names
        >>> scalars.get_names("g*")
        ['gamma0', 'gamma1', 'gamma2', 'gamma3', 'gamma4', 'gamma5', 'gamma_']
        >>> del scalars[["gamma3", "gamma_"]]
        >>> scalars.get_names("g*")
        ['gamma0', 'gamma1', 'gamma2', 'gamma4', 'gamma5']

        >>> # delete one scalar from a subset of the global database
        >>> scalars_subset = scalars["z*"]
        >>> scalars_subset.names
        ['zkf1', 'zkf2', 'zkf3']
        >>> del scalars_subset["zkf2"]
        >>> scalars_subset.names
        ['zkf1', 'zkf3']
        >>> # NOTE: the scalar has also been deleted from the global database
        >>> "zkf2" in scalars
        False
        >>> scalars.get_names("z*")
        ['zkf1', 'zkf3']
        """
        super().__delitem__(key)

    def copy_from(self, input_files: Union[str, List[str]], names: Union[str, List[str]]='*'):
        r"""
        Copy (a subset of) scalars from the input file(s) 'input_files' into the current database.

        Parameters
        ----------
        input_file: str or list(str)
            file(s) from which the copied scalars are read.
        names: str or list(str)
            list of scalars to copy from the input file(s).
            Defaults to load all scalars from the input file(s). 
        """
        CythonScalars.copy_from(self, input_files, names)

    def from_series(self, s: pd.Series):
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
        
        >>> scalars.names
        ['alpha_', 'alpha_0', 'alpha_1', 'beta_', 'beta_0', 'beta_1']
        >>> scalars["alpha_1"]
        Scalar(0.1, 1, na)
        >>> scalars["beta_"]
        Scalar(3.6, 1, na)
        """
        if not (self.is_global_workspace or self.is_detached):
            # check that all names in the pandas object are present in the current subset 
            self._check_same_names(self.names, s.index.tolist())

        for index, value in s.items():
            self._set_object(index, value)

    def from_frame(self, df: pd.DataFrame):
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
        
        >>> scalars.names
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
        if not (self.is_global_workspace or self.is_detached):
            # check that all names in the pandas object are present in the current subset 
            self._check_same_names(self.names, df.index.to_list())

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
                    scalar._set_std(row.std)
                self._set_object(row[0], scalar)

    def to_frame(self) -> pd.DataFrame:
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
        >>> scalars.load(f"{SAMPLE_DATA_DIR}/fun.scl")       # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Loading .../fun.scl
        161 objects loaded 
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
        data = {name: self._get_object(name)._as_tuple() for name in self.names}
        return pd.DataFrame.from_dict(data, orient='index', dtype="float64", columns=["value", "relax", "std"])

    @property
    def df(self) -> pd.DataFrame:
        r"""
        Create a pandas DataFrame from the current Scalars database.
        The index of the returned DataFrame is build from the Scalars names.

        See Also
        --------
        Scalars.to_frame

        Examples
        --------
        >>> from iode import SAMPLE_DATA_DIR, scalars
        >>> import pandas as pd
        >>> scalars.load(f"{SAMPLE_DATA_DIR}/fun.scl")       # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Loading .../fun.scl
        161 objects loaded 
        >>> len(scalars)
        161

        >>> # Export the IODE Scalars database as a pandas DataFrame
        >>> df = scalars.df
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
        >>> df = scalars["a*;*_"].df
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
        return self.to_frame()

    def _str_table(self, names: List[str]) -> str:
        data = [[name] + list(self._get_object(name)._as_tuple()) for name in names]
        # transpose data
        data = [list(row) for row in zip(*data)]
        columns = {"name": data[0], "value": data[1], "relax": data[2], "std": data[3]}
        return table2str(columns, max_lines=10, precision=4, justify_funcs={"name": JUSTIFY.LEFT})

    @property
    def print_nb_decimals(self) -> int:
        r"""
        Number of decimals to print.

        Parameters
        ----------
        value: int
            number of decimals to print (equations coeffs, scalars and computed tables values).

        Examples
        --------
        >>> from iode import scalars
        >>> scalars.print_nb_decimals = 4
        >>> scalars.print_nb_decimals
        4
        """
        return self._get_print_nb_decimals()

    @print_nb_decimals.setter
    def print_nb_decimals(self, value: int):
        self._set_print_nb_decimals(value)

    def print_to_file(self, filepath: Union[str, Path], names: Union[str, List[str]]=None, format: str=None) -> None:
        r"""
        Print the list scalars defined by `names` to the file `filepath` using the format `format`.

        Argument `format` must be in the list:
        - 'H' (HTML file)
        - 'M' (MIF file)
        - 'R' (RTF file)
        - 'C' (CSV file)

        If argument `format` is null (default), the *A2M* format will be used to print the output.

        If the filename does not contain an extension, it is automatically added based on 
        the value of `format`.

        If `names` is a string, it is considered as a *pattern* and the function will print 
        all scalars matching the pattern. The following characters in *pattern* have a 
        special meaning:
        
            - `*` : any character sequence, even empty
            - `?` : any character (one and only one)
            - `@` : any alphanumerical char [A-Za-z0-9]
            - `&` : any non alphanumerical char
            - `|` : any alphanumeric character or none at the beginning and end of a string 
            - `!` : any non-alphanumeric character or none at the beginning and end of a string 
            - `\` : escape the next character

        If `names` is None, print all scalars of the (subset of the) current database.

        Parameters
        ----------
        filepath: str or Path
            path to the file to print.
            If the filename does not contain an extension, it is automatically 
            added based on the value of the format argument.
        names: str or list of str, optional
            pattern or list of names of the scalars to print.
            If None, print all scalars of the (subset of the) current database.
            Defaults to None.
        format: str, optional
            format of the output file. Possible values are: 'H' (HTML file), 
            'M' (MIF file), 'R' (RTF file) or 'C' (CSV file).
            Defaults to None meaning that the scalars will be dumped in the *A2M* format.

        Examples
        --------
        >>> from iode import scalars, SAMPLE_DATA_DIR
        >>> scalars.load(f"{SAMPLE_DATA_DIR}/fun.scl")         # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Loading .../fun.scl
        161 objects loaded
        >>> scalars.print_nb_decimals = 4

        >>> scalars.print_to_file(output_dir / "scalars.csv", "a*")         # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Printing IODE objects definition to file '...scalars.csv'...
        Printing acaf1 ...
        Printing acaf2 ...
        Printing acaf3 ...
        Printing acaf4 ...
        Print done
        >>> with open(output_dir / "scalars.csv") as f:                     # doctest: +NORMALIZE_WHITESPACE
        ...     print(f.read())
        ...
        " - acaf1 : 0.0158 (1, 0.0014, 11.5206)"
        " - acaf2 : -0.0000 (1, 0.0000, -5.3691)"
        " - acaf3 : 2.5026 (1, 0.8730, 2.8666)"
        " - acaf4 : -0.0085 (1, 0.0021, -4.0825)"
        <BLANKLINE>
        """
        self._print_to_file(filepath, names, format)

    def __hash__(self) -> int:
        r"""
        Return a hash value for the current Scalars database.

        Examples
        --------
        >>> from iode import SAMPLE_DATA_DIR, scalars
        >>> scalars.load(f"{SAMPLE_DATA_DIR}/fun.scl")       # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Loading .../fun.scl
        161 objects loaded 
        >>> len(scalars)
        161
        >>> original_hash = hash(scalars)
        
        >>> # rename 1 scalar
        >>> scalars.rename("acaf1", "acaf1_")
        >>> original_hash == hash(scalars)
        False
        >>> scalars.rename("acaf1_", "acaf1")  # revert the change
        >>> original_hash == hash(scalars)
        True

        >>> # modify one scalar
        >>> saved_scalar = scalars["acaf1"].copy()
        >>> scalars["acaf1"]
        Scalar(0.0157684, 1, 0.00136871)
        >>> scalars["acaf1"] = 0.02
        >>> scalars["acaf1"]
        Scalar(0.02, 1, na)
        >>> original_hash == hash(scalars)      # modified scalar
        False
        >>> scalars["acaf1"] = saved_scalar     # revert the change
        >>> scalars["acaf1"] 
        Scalar(0.0157684, 1, 0.00136871)
        >>> original_hash == hash(scalars)
        True

        >>> # delete a scalar
        >>> saved_scalar = scalars["acaf1"]
        >>> del scalars["acaf1"]
        >>> original_hash == hash(scalars)      # deleted scalar
        False
        >>> scalars["acaf1"] = saved_scalar
        >>> original_hash == hash(scalars)
        True

        >>> # add a scalar
        >>> scalars["new_scalar"] = (0.9, 1.0)
        >>> original_hash == hash(scalars)      # added scalar
        False
        >>> del scalars["new_scalar"]
        >>> original_hash == hash(scalars)
        True
        """
        return CythonScalars.__hash__(self)


scalars: Scalars = Scalars.get_instance()
