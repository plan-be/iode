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

    def __init__(self, *args: Union[str, Tuple[int, str, int]]) -> Period:
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
        *args: str or (int, str, int)
            Either a string representing the period or values representing the year, 
            the periodicity and the position of the period in the year (step):

            * period: str
                String representing the period.

            or

            * year: int
                Year of the period.

            * periodicity: {'Y', 'S', 'Q', 'M', 'W', 'D'}, optional
                Periodicity of the period. 
                Default to 'Y' (yearly).

            * step: int, optional
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
        '2000Y1'
        >>> # passing year, periodicity and step
        >>> period = Period(2010, 'Q', 1)
        >>> period
        '2010Q1'
        >>> # passing only year
        >>> period = Period(2010)
        >>> period
        '2010Y1'
        >>> # passing year and periodicity
        >>> period = Period(2010, 'Q')
        >>> period
        '2010Q1'
        >>> # copy a period
        >>> period_2 = Period(period)
        >>> period_2
        '2010Q1'
        """
        cdef int year_ = 0
        cdef char periodicity_ = b'Y'
        cdef int step_ = 1
        cdef str str_period = ''

        def check_periodicity(value: str) -> int:
            if len(value) != 1:
                raise ValueError("'periodicity': Expected string of length 1")
            if value not in PERIODICITY_LIST:
                raise ValueError(f"Wrong periodicity. Valid periodicity are: 'Y', 'S', 'Q', 'M', 'W' or 'D'")
            return value.encode()[0]

        if len(args) == 1:
            arg = args[0]
            if isinstance(arg, int):
                year_ = arg
            elif isinstance(arg, Period):
                year_, periodicity_, step_ = arg.year, arg.periodicity, arg.step
            elif isinstance(arg, str):
                str_period = arg
            else:
                raise ValueError("Expected argument of type int, 'Period' or string.\nGot argument of type '" + type(arg).__name__ + "'")
        elif len(args) == 2:
            year_ = args[0]
            periodicity_ = check_periodicity(args[1])
        elif len(args) == 3:
            year_ = args[0]
            periodicity_ = check_periodicity(args[1])
            step_ = args[2]
        else:
            raise ValueError("Wrong number of arguments to create a new Period")

        if str_period:
            self.c_period = CPeriod(<string>str_period.encode())
        else:
            self.c_period = CPeriod(year_, periodicity_, step_)

    def nb_periods_per_year(self) -> int:
        """
        Number of periods in a year according to the periodicity.

        Examples
        --------
        >>> from iode import Period
        >>> period = Period(2010, 'Q', 1)
        >>> period.nb_periods_per_year()
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
        '2000Q1'
        >>> # shift forward
        >>> shifted_period = period.shift(7)
        >>> shifted_period
        '2001Q4'
        >>> # shift backward
        >>> shifted_period = period.shift(-7)
        >>> shifted_period
        '1998Q2'
        """
        shifted_period = self.c_period.shift(nb_periods)
        return Period(shifted_period.p_y, chr(shifted_period.p_p), shifted_period.p_s)

    def to_float(self) -> float:
        """
        Returns a numerical representation of the period.

        Returns
        -------
        float

        Examples
        --------
        >>> from iode import Period
        >>> period = Period(2000, 'Q', 1)
        >>> period.to_float()
        2000.0
        >>> period = Period(2000, 'Q', 3)
        >>> period.to_float()
        2000.5
        """
        return self.c_period.to_float()

    # Attributes access

    @property
    def year(self) -> int:
        """
        Corresponding year of the period
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
        """
        return self.c_period.p_p

    @property
    def step(self) -> int:
        """
        Position of the period in the year
        """
        return self.c_period.p_s

    # Special methods

    def __eq__(self, other: Period) -> bool:
        return self.c_period == other.c_period

    def __str__(self) -> str:
        return self.c_period.to_string().decode()

    def __repr__(self) -> str:
        return repr(str(self))
