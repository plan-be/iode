# distutils: language = c++

from typing import Union, Tuple
from libcpp.string cimport string
from time.period cimport CPeriod

from typing import List


PERIODICITY_LIST: List[str] = ["Y", "S", "Q", "M", "W", "D"]


# Period wrapper class
# see https://cython.readthedocs.io/en/latest/src/userguide/wrapping_CPlusPlus.html#create-cython-wrapper-class 
cdef class Period:

    cdef CPeriod c_period

    def __init__(self, period_or_year: Union[str, int, Period], periodicity: str='Y', step: int=1) -> Period:
        """
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
        cdef char ch_periodicity

        if isinstance(period_or_year, Period):
            period_or_year, periodicity, step = period_or_year.year, period_or_year.periodicity, period_or_year.step
        
        if isinstance(period_or_year, str):
            self.c_period = CPeriod(<string>period_or_year.encode())
        else:
            if len(periodicity) != 1:
                raise ValueError("'periodicity': Expected string of length 1")
            if periodicity not in PERIODICITY_LIST:
                raise ValueError(f"Wrong periodicity. Valid values for the periodicity are: {', '.join(PERIODICITY_LIST)}")
            ch_periodicity = periodicity.encode()[0]
            self.c_period = CPeriod(<int>period_or_year, ch_periodicity, <int>step)

    @property
    def nb_periods_per_year(self) -> int:
        """
        Number of periods in a year according to the periodicity.

        Examples
        --------
        >>> from iode import Period
        >>> period = Period(2010, 'Q', 1)
        >>> period.nb_periods_per_year
        4
        """
        return self.c_period.nb_periods_per_year()

    def difference(self, other: Period) -> int:
        """
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
        return self.c_period.difference(other.c_period)

    def shift(self, nb_periods: int) -> Period:
        """
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
        shifted_period = self.c_period.shift(nb_periods)
        return Period(shifted_period.p_y, chr(shifted_period.p_p), shifted_period.p_s)

    # Attributes access

    @property
    def year(self) -> int:
        """
        Corresponding year of the period

        Examples
        --------
        >>> from iode import Period
        >>> period = Period(2000, 'Q', 3)
        >>> period.year
        2000
        """
        return self.c_period.p_y

    @property
    def periodicity(self) -> str:
        """
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
        return chr(self.c_period.p_p)

    @property
    def step(self) -> int:
        """
        Position of the period in the year

        Examples
        --------
        >>> from iode import Period
        >>> period = Period(2000, 'Q', 3)
        >>> period.step
        3
        """
        return self.c_period.p_s

    # Special methods

    def __eq__(self, other: Period) -> bool:
        return self.c_period == other.c_period

    def __float__(self) -> float:
        """
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
        return self.c_period.to_float()

    def __lt__(self, other: Period) -> bool:
        return float(self) < float(other)

    def __gt__(self, other: Period) -> bool:
        return float(self) > float(other)

    def __le__(self, other: Period) -> bool:
        if self == other:
            return True
        else:
            return self < other

    def __ge__(self, other: Period) -> bool:
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
