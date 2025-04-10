import sys
import warnings
from typing import Union, Tuple, List, Optional, Any
if sys.version_info.minor >= 11:
    from typing import Self
else:
    Self = Any

try:
    import larray as la
    Axis = la.Axis
except ImportError:
    la = None
    Axis = Any

from iode.time.period import Period

from iode.iode_cython import Sample as CythonSample
_ALLOWED_TYPES_FOR_PERIOD = {'str', 'float', 'Period'}


class Sample:
    r"""
    A sample represents the series of sub periods attached to the IODE variables or to the estimation process.

    Parameters
    ----------
    start_period: str or Period
    	First period of the sample.
    	If str, it must be a valid period string (e.g. '1960Y1').
    end_period: str or Period
    	Last period of the sample.
        If str, it must be a valid period string (e.g. '2015Y1').

    Attributes
    ----------
    start: str
        First period of the sample.
    end: str
        Last period of the sample.
    nb_periods: int
        Total number of sub periods in the sample. 

    Examples
    --------
    >>> from iode import Sample
    >>> Sample("1982Y1", "2020Y1")
    Sample("1982Y1:2020Y1")
    >>> Sample("1982Y1:2020Y1")
    Sample("1982Y1:2020Y1")
    """
    def __init__(self, start_period: Union[str, Period], end_period: Union[str, Period]=None):
        if not isinstance(start_period, (str, Period)):
            raise TypeError("start_period must be a string or a Period object")
        if end_period is not None and not isinstance(end_period, (str, Period)):
            raise TypeError("end_period must be a string or a Period object")
        
        if isinstance(start_period, Period):
            start_period = str(start_period)
        if end_period is not None and isinstance(end_period, Period):
            end_period = str(end_period)
        
        if end_period is None: 
            if not len(start_period):
                raise ValueError("Both start and end periods must be specified")
            if ':' not in start_period:
                raise ValueError("When only one parameter is passed to Sample(), it is considered as "
                                 "a string representation of the desired sample and must include a colon ':'")
            start_period, end_period = start_period.split(':')
        
        if not isinstance(start_period, str) or not isinstance(end_period, str):
            raise TypeError("start_period and end_period must be strings")
        
        if not start_period or not end_period:
            raise ValueError("Both start and end periods must be specified")
        
        self._cython_instance = CythonSample(start_period, end_period)

    @classmethod
    def get_instance(cls) -> Self:
        instance = cls.__new__(cls)
        instance._cython_instance = CythonSample.__new__(CythonSample)
        return instance

    def index(self, period: Union[str, Period]) -> int:
        r"""
        Position of the 'period' in the sample.

        Returns
        -------
        int

        Raises
        ------
        IndexError
            If the 'period' has not been found in the sample.

        Examples
        --------
        >>> from iode import variables, SAMPLE_DATA_DIR
        >>> variables.load(f"{SAMPLE_DATA_DIR}/fun.var")    # doctest: +ELLIPSIS
        Loading .../fun.var
        394 objects loaded
        >>> variables.sample.index("1982Y1")
        22
        >>> variables.sample.index("2020Y1")
        Traceback (most recent call last):
        ... 
        IndexError: The period '2020Y1' is not in the sample '1960Y1:2015Y1'
        """
        if isinstance(period, Period):
            period = str(period)
        return self._cython_instance.index(period)

    def get_period_list(self, astype: Union[type(Any), str]=str) -> List[Any]:
        rf"""
        List of all periods of the sample.
        Periods are exported as string (default) or as float.

        Parameters
        ----------
        astype: type or str
            Allowed returned type for periods are {list(_ALLOWED_TYPES_FOR_PERIOD)}.
            Default to str.

        Returns
        -------
        list(str) or list(float)

        Examples
        --------
        >>> from iode import variables, SAMPLE_DATA_DIR
        >>> variables.load(f"{{SAMPLE_DATA_DIR}}/fun.var")      # doctest: +ELLIPSIS
        Loading .../fun.var
        394 objects loaded
        >>> variables.sample.get_period_list()                  # doctest: +ELLIPSIS
        ['1960Y1', '1961Y1', ..., '2014Y1', '2015Y1']
        >>> variables.sample.get_period_list(astype=float)      # doctest: +ELLIPSIS
        [1960.0, 1961.0, ..., 2014.0, 2015.0]
        >>> variables.sample.get_period_list(astype=Period)     # doctest: +ELLIPSIS
        [Period("1960Y1"), Period("1961Y1"), ..., Period("2014Y1"), Period("2015Y1")]
        """
        if isinstance(astype, type):
            astype = astype.__name__
        if not isinstance(astype, str):
            raise TypeError(f"Expected 'astype' to be a string or a type. Got {type(astype).__name__} instead.")
        if astype not in _ALLOWED_TYPES_FOR_PERIOD:
            raise ValueError(f"Allowed types for 'astype' are {list(_ALLOWED_TYPES_FOR_PERIOD)}")
                
        if astype == 'Period':
            list_periods = self._cython_instance.get_period_list('str')
            return [Period(p) for p in list_periods]
        else:
            return self._cython_instance.get_period_list(astype)

    def intersection(self, other_sample: Self) -> Self:
        r"""
        Compute the intersection between two samples.

        Returns
        -------
        Sample

        Examples
        --------
        >>> from iode import variables, SAMPLE_DATA_DIR
        >>> variables.load(f"{SAMPLE_DATA_DIR}/fun.var")    # doctest: +ELLIPSIS
        Loading .../fun.var
        394 objects loaded
        >>> variables.sample
        Sample("1960Y1:2015Y1")
        >>> variables_2 = variables.copy()
        >>> variables_2.sample = "2000Y1:2040Y1"
        >>> variables_2.sample
        Sample("2000Y1:2040Y1")
        >>> sample_intersec = variables.sample.intersection(variables_2.sample)
        >>> sample_intersec
        Sample("2000Y1:2015Y1")
        """
        if not isinstance(other_sample, Sample):
            raise TypeError("Expected argument 'other_sample' of intersection to be of type Sample. "
                            f"Got {type(other_sample).__name__} instead.")
        instance = self.get_instance()
        instance._cython_instance = self._cython_instance.intersection(other_sample._cython_instance)
        if instance._cython_instance is None:
            warnings.warn(f"The intersection between '{self}' and '{other_sample}' is empty", UserWarning)
            return None
        return instance

    @property
    def start(self) -> Period:
        period = Period.get_instance()
        period._cython_instance = self._cython_instance.get_start()
        if period._cython_instance is None:
            return None
        return period

    @property
    def end(self) -> Period:
        period = Period.get_instance()
        period._cython_instance = self._cython_instance.get_end()
        if period._cython_instance is None:
            return None
        return period

    @property
    def nb_periods(self) -> int:
        return self._cython_instance.get_nb_periods()

    @property
    def periods(self) -> List[Period]:
        r"""
        Return the list of periods.

        Returns
        -------
        List[Period]

        Examples
        --------
        >>> from iode import variables, SAMPLE_DATA_DIR
        >>> variables.load(f"{SAMPLE_DATA_DIR}/fun.var")    # doctest: +ELLIPSIS
        Loading .../fun.var
        394 objects loaded
        >>> variables.sample.periods    # doctest: +ELLIPSIS
        [Period("1960Y1"), Period("1961Y1"), ..., Period("2014Y1"), Period("2015Y1")]
        """
        return self.get_period_list(astype=Period)

    def __len__(self) -> int:
        return self._cython_instance.__len__()

    def __eq__(self, other: Self) -> bool:
        if not isinstance(other, Sample):
            warnings.warn(f"Comparing '{self}' with '{other}' is not supported", UserWarning)
            return False
        return self._cython_instance.__eq__(other._cython_instance)

    def __str__(self) -> str:
        return self._cython_instance.__str__()

    def __repr__(self) -> str:
        return self._cython_instance.__repr__()
