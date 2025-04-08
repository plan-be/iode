from libcpp.string cimport string

from typing import Union, Tuple, List
from pyiode.time.period cimport CPeriod


cdef class Period:
    cdef CPeriod c_period

    def __init__(self, period_or_year: Union[str, int], periodicity: str=None, step: int=None) -> Period:
        cdef char ch_periodicity
        
        if isinstance(period_or_year, str):
            self.c_period = CPeriod(<string>period_or_year.encode())
        else:
            ch_periodicity = periodicity.encode()[0]
            self.c_period = CPeriod(<int>period_or_year, ch_periodicity, <int>step)

    def get_nb_periods_per_year(self) -> int:
        return self.c_period.nb_periods_per_year()

    def difference(self, other: Period) -> int:
        return self.c_period.difference(other.c_period)

    def shift(self, nb_periods: int) -> Period:
        shifted_period = self.c_period.shift(nb_periods)
        return Period(shifted_period.p_y, chr(shifted_period.p_p), shifted_period.p_s)

    def get_year(self) -> int:
        return self.c_period.p_y

    def get_periodicity(self) -> str:
        return chr(self.c_period.p_p)

    def get_step(self) -> int:
        return self.c_period.p_s

    def __eq__(self, other: Period) -> bool:
        if self.c_period.p_y != other.c_period.p_y:
            return False
        if self.c_period.p_p != other.c_period.p_p:
            return False 
        if self.c_period.p_s != other.c_period.p_s:
            return False
        return True

    def __float__(self) -> float:
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
        if self.get_year() == 0:
            return ""
        else:
            return f"{self.get_year()}{self.get_periodicity()}{self.get_step()}"
