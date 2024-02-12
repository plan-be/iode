# distutils: language = c++

from typing import Union, Tuple
from libcpp.string cimport string
from pyiode_period cimport Period as CPeriod


periodicities = ["Y", "S", "Q", "M", "W", "D"]


# Period wrapper class
# see https://cython.readthedocs.io/en/latest/src/userguide/wrapping_CPlusPlus.html#create-cython-wrapper-class 
cdef class Period:

    cdef CPeriod c_period

    def __init__(self, *args: Union[str, Tuple[int, str, int]]) -> Period:
        f"""
        An IODE period is defined by a year, a periodicity ({", ".join(periodicities)}) and by a position 
        in the year.

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
                Periodicity of the period. Defaults to 'Y'.

            * step: int, optional
                Position of the period in the year. Defaults to 1.

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
        2000Y1
        >>> # passing year, periodicity and step
        >>> period = Period(2010, 'Q', 1)
        >>> period
        2010Q1
        >>> # passing only year
        >>> period = Period(2010)
        >>> period
        2010Y1
        >>> # passing year and periodicity
        >>> period = Period(2010, 'Q')
        >>> period
        2010Q1
        >>> # copy a period
        >>> period_2 = Period(period)
        >>> period_2
        2010Q1
        """
        cdef int year_ = 0
        cdef char periodicity_ = b'Y'
        cdef int step_ = 1
        cdef str str_period = ''

        def check_year(value: int) -> int:
            if not isinstance(value, int):
                raise TypeError("Expected an integer for the argument 'year'.\nGot argument of type '" + type(value).__name__ + "'")
            return value

        def check_periodicity(value: str) -> int:
            if not isinstance(value, str):
                raise TypeError("Expected a string for the argument 'periodicity_'.\nGot type '" + type(value).__name__ + "'")
            if len(value) > 1:
                raise ValueError("Expected a string a length 1 for the argument 'periodicity_'")
            if value not in periodicities:
                raise ValueError(f"Wrong periodicity. Valid periodicity are: {periodicities}")
            return value.encode("utf-8")[0]

        def check_step(value: int) -> int:
            if not isinstance(value, int):
                raise TypeError("Expected an integer for the argument 'step'.\nGot argument of type '" + type(value).__name__ + "'")
            return value

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
            year_ = check_year(args[0])
            periodicity_ = check_periodicity(args[1])
        elif len(args) == 3:
            year_ = check_year(args[0])
            periodicity_ = check_periodicity(args[1])
            step_ = check_step(args[2])
        else:
            raise ValueError("Wrong number of arguments to create a new Period")

        if str_period:
            self.c_period = CPeriod(<string>str_period.encode("utf-8"))
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
        if not isinstance(other, Period):
            raise TypeError(f"Expected argument of type 'Period'.\nGot argument of type '{type(other)}'")
        return self.c_period.difference(other.c_period)

    def shift(self, nb_periods: int) -> Period:
        """
        Shift the period by a number of sub periods. 
        The number of sub period may be negative.

        Parameters
        ----------
        nb_periods: int
            Number of sub period. May be negative.

        Returns
        -------
        shifted_period: Period

        Examples
        --------
        >>> from iode import Period
        >>> period = Period(2000, 'Q', 1)
        >>> shifted_period = period.shift(7)
        >>> shifted_period
        2001Q4
        """
        if not isinstance(nb_periods, int):
            raise TypeError(f"Expected argument of type int.\nGot argument of type '{type(nb_periods)}'")
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
        return self.c_period.p_y

    @property
    def periodicity(self) -> str:
        return self.c_period.p_p

    @property
    def step(self) -> int:
        return self.c_period.p_s

    # Special methods

    def __eq__(self, other: Period) -> bool:
        if not isinstance(other, Period):
            raise TypeError(f"Expected argument of type 'Period'.\nGot argument of type '{type(other)}'")
        return self.c_period == other.c_period

    def __str__(self) -> str:
        return self.c_period.to_string().decode("utf-8")

    def __repr__(self) -> str:
        return self.c_period.to_string().decode("utf-8")
