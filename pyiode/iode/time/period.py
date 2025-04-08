import sys
import warnings
from typing import Union, Tuple, List, Any
if sys.version_info.minor >= 11:
    from typing import Self
else:
    Self = Any

from iode.common import PERIODICITY_LIST
from iode.iode_cython import Period as CythonPeriod


class Period:
    def __init__(self, period_or_year: Union[str, int, Self], periodicity: str='Y', step: int=1) -> Self:
        r"""
        An IODE period is defined by a year, a periodicity and by a position in the year.

        Possible values for the periodicity are:

            - Y: yearly
            - S: semestrial
            - Q: quarterly
            - M: monthly
            - W: weekly
            - D: daily

        Parameters
        ----------
        period_or_year: str or int
            If str, it is consided as a string representing the year, the periodicity 
            and the position of the period in the year (step).
            If int, it is considered as the year of the period.
        periodicity: str, optional
                Periodicity of the period. Possible values are 'Y', 'S', 'Q', 'M', 'W' and 'D'.
                Default to 'Y' (yearly).
        step: int, optional
                Position of the period in the year. 
                Default to 1.

        Attributes
        ----------
        year: int
        periodicity: str
        step: int
            position in the year

        Examples
        --------
        >>> from iode import Period
        >>> # passing a string
        >>> period = Period("2000Y1")
        >>> period
        Period("2000Y1")
        >>> # passing year, periodicity and step
        >>> period = Period(2010, 'Q', 1)
        >>> period
        Period("2010Q1")
        >>> # passing only year
        >>> period = Period(2010)
        >>> period
        Period("2010Y1")
        >>> # passing year and periodicity
        >>> period = Period(2010, 'Q')
        >>> period
        Period("2010Q1")
        >>> # copy a period
        >>> period_2 = Period(period)
        >>> period_2
        Period("2010Q1")
        """
        if isinstance(period_or_year, Period):
            period = period_or_year
            self._cython_instance = CythonPeriod(period.year, period.periodicity, period.step)
        elif isinstance(period_or_year, str):
            period = period_or_year
            self._cython_instance = CythonPeriod(period)
        elif isinstance(period_or_year, int):
            year = period_or_year
            if len(periodicity) != 1:
                raise ValueError("'periodicity': Expected string of length 1")
            if periodicity not in PERIODICITY_LIST:
                raise ValueError(f"Wrong periodicity. Valid values for the periodicity are: {', '.join(PERIODICITY_LIST)}")
            self._cython_instance = CythonPeriod(year, periodicity, step)

    @classmethod
    def get_instance(cls) -> Self:
        instance = cls.__new__(cls)
        instance._cython_instance = CythonPeriod.__new__(CythonPeriod)
        return instance

    @property
    def nb_periods_per_year(self) -> int:
        r"""
        Number of periods in a year according to the periodicity.

        Examples
        --------
        >>> from iode import Period
        >>> period = Period(2010, 'Q', 1)
        >>> period.nb_periods_per_year
        4
        """
        return self._cython_instance.get_nb_periods_per_year()

    def difference(self, other: Self) -> int:
        r"""
        Number of sub periods between two periods. 
        The two periods must have the same periodicity.

        Parameters
        ----------
        other: Period

        Returns
        -------
        int

        Examples
        --------
        >>> from iode import Period
        >>> period = Period(2000, 'Q', 1)
        >>> period_2 = Period(2001, 'Q', 3)
        >>> period.difference(period_2)
        -6
        >>> period_2.difference(period)
        6
        """
        return self._cython_instance.difference(other._cython_instance)

    def shift(self, nb_periods: int) -> Self:
        r"""
        Shift the current period by a number of sub periods.
        If the number of sub periods is positive, the shift is time forward.  
        Conversely, if the number of sub periods is negative, the shift is time backward.  

        Parameters
        ----------
        nb_periods: int
            Number of sub periods. 
            The shift is time forward if positive and time backward if negative. 

        Returns
        -------
        shifted_period: Period

        Examples
        --------
        >>> from iode import Period
        >>> period = Period(2000, 'Q', 1)
        >>> period
        Period("2000Q1")
        >>> # shift forward
        >>> shifted_period = period.shift(7)
        >>> shifted_period
        Period("2001Q4")
        >>> # shift backward
        >>> shifted_period = period.shift(-7)
        >>> shifted_period
        Period("1998Q2")
        """
        instance = self.get_instance()
        instance._cython_instance = self._cython_instance.shift(nb_periods)
        return instance

    @property
    def year(self) -> int:
        r"""
        Corresponding year of the period

        Examples
        --------
        >>> from iode import Period
        >>> period = Period(2000, 'Q', 3)
        >>> period.year
        2000
        """
        return self._cython_instance.get_year()

    @property
    def periodicity(self) -> str:
        r"""
        Possible values are:

            - Y: yearly
            - S: semestrial
            - Q: quarterly
            - M: monthly
            - W: weekly
            - D: daily
        
        Examples
        --------
        >>> from iode import Period
        >>> period = Period(2000, 'Q', 3)
        >>> period.periodicity
        'Q'
        """
        return self._cython_instance.get_periodicity()

    @property
    def step(self) -> int:
        r"""
        Position of the period in the year

        Examples
        --------
        >>> from iode import Period
        >>> period = Period(2000, 'Q', 3)
        >>> period.step
        3
        """
        return self._cython_instance.get_step()

    def __eq__(self, other: Self) -> bool:
        if not isinstance(other, Period):
            warnings.warn(f"Comparing '{self}' with '{other}' is not supported", UserWarning)
            return False
        return self._cython_instance == other._cython_instance

    def __float__(self) -> float:
        r"""
        Returns a float representation of the period.

        Returns
        -------
        float

        Examples
        --------
        >>> from iode import Period
        >>> period = Period(2000, 'Q', 1)
        >>> float(period)
        2000.0
        >>> period = Period(2000, 'Q', 3)
        >>> float(period)
        2000.5
        """
        return float(self._cython_instance)

    def __lt__(self, other: Self) -> bool:
        if not isinstance(other, Period):
            warnings.warn(f"Comparing '{self}' with '{other}' is not supported", UserWarning)
            return False
        return float(self) < float(other)

    def __gt__(self, other: Self) -> bool:
        if not isinstance(other, Period):
            warnings.warn(f"Comparing '{self}' with '{other}' is not supported", UserWarning)
            return False
        return float(self) > float(other)

    def __le__(self, other: Self) -> bool:
        if not isinstance(other, Period):
            warnings.warn(f"Comparing '{self}' with '{other}' is not supported", UserWarning)
            return False
        if self == other:
            return True
        else:
            return self < other

    def __ge__(self, other: Self) -> bool:
        if not isinstance(other, Period):
            warnings.warn(f"Comparing '{self}' with '{other}' is not supported", UserWarning)
            return False
        if self == other:
            return True
        else:
            return self > other

    def __str__(self) -> str:
        if self.year == 0:
            return ""
        else:
            return f"{self.year}{self.periodicity}{self.step}"

    def __repr__(self) -> str:
        if self.year == 0:
            return ""
        else:
            return f'Period("{self.year}{self.periodicity}{self.step}")'


