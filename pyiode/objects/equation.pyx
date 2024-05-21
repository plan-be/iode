# distutils: language = c++

from collections.abc import Iterable
from typing import Union, Tuple, List, Dict, Optional

from libcpp.map cimport map
from libcpp.string cimport string
from cython.operator cimport dereference
from pyiode.common cimport EnumIodeEquationTest
from pyiode.objects.equation cimport CEquation
from pyiode.objects.equation cimport hash_value as hash_value_eq


# Equation wrapper class
# see https://cython.readthedocs.io/en/latest/src/userguide/wrapping_CPlusPlus.html#create-cython-wrapper-class 
cdef class Equation:
    """
    IODE equation.

    Attributes
    ----------
    lec: str
    method: str
    sample: Sample
    comment: str
    instruments: str
    block: str
        block of equations (to estimate) to which the equation belong.
    tests: dict(str, float)
    date: str
        last time the equation has been estimated.

    Parameters
    ----------
    name: str
        Endogenous variable name.
    lec: str
        LEC expression of the equation.
    method: int or {'LSQ', 'ZELLNER', 'INSTRUMENTAL', 'GLS', 'MAX_LIKELIHOOD'}, optional
        Method used to estimate the coefficients of the equation.
        Defaults to 'LSQ' (0).
    from_period: str or Period, optional
        Starting period for the estimation.
        Defaults to the starting period of the sample associated with the IODE Variables database.
    to_period: str or Period, optional
        Ending period for the estimation.
        Defaults to the ending period of the sample associated with the IODE Variables database.
    comment: str, optional
        Defaults to empty.
    instruments: str, optional
        Defaults to empty.
    block: str, optional
        block of equations (to estimate) to which the equation belong.
        Defaults to empty.

    Examples
    --------
    >>> from iode import Equation, variables
    >>> variables.sample = "1960Y1:2015Y1"
    >>> eq_ACAF = Equation("ACAF", "(ACAF / VAF[-1]) := acaf1 + acaf2 * GOSF[-1] + acaf4 * (TIME=1995)")
    >>> eq_ACAF         # doctest: +NORMALIZE_WHITESPACE
    # GDM> the __repr__ should be more Python-code like, ideally eval(repr_str) should be valid
    #      (ie replace ":" by "=" and display "repr()" of comment, block and instruments, and MAYBE avoid showing
    #       values which are the same as default values)
    Equation(lec: (ACAF / VAF[-1]) := acaf1 + acaf2 * GOSF[-1] + acaf4 * (TIME=1995),
        method: LSQ,
        sample: 1960Y1:2015Y1,
        comment: ,
        block: ,
        instruments: ,
        tests:
                corr: 0
                dw: 0
                fstat: 0
                loglik: 0
                meany: 0
                r2: 0
                r2adj: 0
                ssres: 0
                stderr: 0
                stderrp: 0
                stdev: 0
        date: )
    """

    cdef CEquation* c_equation

    def __cinit__(self, name: str, lec: str, method: Union[int, str] = "LSQ", from_period: Union[str, Period] = "", 
        to_period: Union[str, Period] = "", comment: str = "", instruments: str = "", block: str = "") -> Equation:
        if not isinstance(name, str):
            raise TypeError("'name': Expected value of type string.\nGot value of type '" + type(name).__name__ + "'")

        if not isinstance(lec, str):
            raise TypeError("'lec': Expected value of type string.\nGot value of type '" + type(lec).__name__ + "'")

        if not isinstance(method, (str, int)):
            raise TypeError("'method': Expected value of type string or int.\nGot value of type '" + type(method).__name__ + "'")

        if not isinstance(from_period, (str, Period)):
            raise TypeError("'from_period': Expected value of type string or 'Period'.\n" +
                            "Got value of type '" + type(from_period).__name__ + "'")
        if isinstance(from_period, Period):
            from_period = str(from_period)

        if not isinstance(to_period, (str, Period)):
            raise TypeError("'to_period': Expected value of type string or 'Period'.\n" + 
                            "Got value of type '" + type(to_period).__name__ + "'")
        if isinstance(to_period, Period):
            to_period = str(to_period)

        if not isinstance(comment, str):
            raise TypeError("'comment': Expected value of type string.\nGot value of type '" + type(comment).__name__ + "'")

        if not isinstance(instruments, str):
            raise TypeError("'instruments': Expected value of type string.\nGot value of type '" + type(instruments).__name__ + "'")

        if not isinstance(block, str):
            raise TypeError("'block': Expected value of type string.\nGot value of type '" + type(block).__name__ + "'")

        self.c_equation = new CEquation(name.encode(), lec.encode(), 0, from_period.encode(), 
                                        to_period.encode(), comment.encode(), instruments.encode(), 
                                        block.encode(), <bint>False)
        self.method = method

    def __dealloc__(self):
        del self.c_equation

    # GDM> what is the point of the name argument if you cannot change the name (or maybe I misunderstand the doctest)?
    #      Besides, if you remove the name argument, you could make this a property setter, which would be nicer IMO
    def set_lec(self, lec: str, name: str):
        """
        Update LEC expression of the current equation.

        Examples
        --------
        >>> from iode import Equation
        >>> eq_ACAF = Equation("ACAF", "(ACAF / VAF[-1]) := acaf1 + acaf2 * GOSF[-1] + acaf4 * (TIME=1995)")
        >>> eq_ACAF.lec
        '(ACAF / VAF[-1]) := acaf1 + acaf2 * GOSF[-1] + acaf4 * (TIME=1995)'
        >>> # remove acaf1 from the LEC expression of the ACAF equation
        >>> eq_ACAF.set_lec("(ACAF / VAF[-1]) := acaf2 * GOSF[-1] + acaf4 * (TIME=1995)", "ACAF")
        >>> eq_ACAF.lec
        '(ACAF / VAF[-1]) := acaf2 * GOSF[-1] + acaf4 * (TIME=1995)'
        >>> # wrong endogenous name
        >>> eq_ACAF.set_lec("(ACAF / VAF[-1]) := acaf2 * GOSF[-1] + acaf4 * (TIME=1995)", "ACAG")
        Traceback (most recent call last):
        ... 
        ValueError: Cannot set LEC '(ACAF / VAF[-1]) := acaf2 * GOSF[-1] + acaf4 * (TIME=1995)' to the equation named 'ACAG'
        """
        if not isinstance(lec, str):
            raise TypeError("'lec': Expected value of type string.\nGot value of type '" + type(lec).__name__ + "'")
        if not isinstance(name, str):
            raise TypeError("'name': Expected value of type string.\nGot value of type '" + type(name).__name__ + "'")
        self.c_equation.set_lec(lec.encode(), name.encode())

    # GDM> I think the property is enough. If you want to make setting only one bound easier,
    #      I think adding from_period and to_period properties might be a better idea. If you
    #      keep this method, I think the property way of doing should be mentioned and/or shown in the docstring/test.
    def set_sample(self, from_period: Union[str, Period] = "", to_period: Union[str, Period] = ""):
        """
        Set the sample for the estimation of coefficients.

        Parameters
        ----------
        from_period: str or Period, optional
            starting period for the estimation.
            Defaults to the starting period of the sample associated with the IODE Variables database.
        to_period: str or Period, optional
            ending period for the estimation.
            Defaults to the ending period of the sample associated with the IODE Variables database.

        Examples
        --------
        >>> from iode import SAMPLE_DATA_DIR
        >>> from iode import variables, Equation, Sample
        >>> variables.load(f"{SAMPLE_DATA_DIR}/fun.var")
        >>> variables.sample
        1960Y1:2015Y1
        >>> eq_ACAF = Equation("ACAF", "(ACAF / VAF[-1]) := acaf1 + acaf2 * GOSF[-1] + acaf4 * (TIME=1995)")
        >>> eq_ACAF.set_sample("1980Y1", "2010Y1")
        >>> eq_ACAF.sample
        1980Y1:2010Y1
        >>> # change only the starting period
        >>> eq_ACAF.set_sample(from_period="1970Y1")
        >>> eq_ACAF.sample
        1970Y1:2015Y1
        >>> # change only the ending period
        >>> eq_ACAF.set_sample(to_period="2000Y1")
        >>> eq_ACAF.sample
        1960Y1:2000Y1
        """
        if not isinstance(from_period, (str, Period)):
            raise TypeError("'from_period': Expected value of type string or 'Period'.\n" +
                            "Got value of type '" + type(from_period).__name__ + "'")
        if not isinstance(to_period, (str, Period)):
            raise TypeError("'to_period': Expected value of type string or 'Period'.\n" + 
                            "Got value of type '" + type(to_period).__name__ + "'")
        if isinstance(from_period, Period):
            from_period = str(from_period)
        if isinstance(to_period, Period):
            to_period = str(to_period)
        self.c_equation.set_sample(from_period.encode(), to_period.encode())

    # GDM: the name of the method is confusing (I thought it would return the *format*, not the date).
    #      call this get_date() instead?
    def get_date_format(self, format: str = "dd-mm-yyyy") -> str:
        """
        Return the date of last estimation in a given format.

        Returns
        -------
        str

        Examples
        --------
        >>> from iode import SAMPLE_DATA_DIR
        >>> from iode import Equation
        >>> eq_ACAF = Equation("ACAF", "(ACAF / VAF[-1]) := acaf1 + acaf2 * GOSF[-1] + acaf4 * (TIME=1995)")
        >>> # date as default format "dd-mm-yyyy"
        # GDM> this example is not very interesting. Should have an example with an actual date.
        >>> eq_ACAF.date
        ''
        >>> # date with specific format
        >>> eq_ACAF.get_date_format("dd/mm/yyyy")
        ''
        """
        if not isinstance(format, str):
            raise TypeError("Expected value of type string.\nGot value of type '" + type(format).__name__ + "'")     
        return self.c_equation.get_date_as_string(format.encode()).decode()

    # GDM> rename to get_coefficients? or even a .coefficients (readonly) property and move the "create missing scalars
    #      feature" to a dedicated method? I don't understand exactly in which circumstances it can happen
    #      that there are equations with missing scalars, but the conditional side effect seems odd to me.
    #      From my uninformed point of view, it would seem more logical if the missing scalars were created in the
    #      Equation init method or even in a dedicated create_equation_and_scalars() function
    def get_coefficients_list(self, create_if_not_exit: bool = True) -> List[str]:
        """
        Return the list of coefficients present in the equation.

        Parameters
        ----------
        create_if_not_exit: bool, optional
            Whether or not to create an entry in the Scalars database for each coefficient not found in it. 
            Defaults to True.

        Returns
        -------
        list(str)

        Examples
        --------
        >>> from iode import SAMPLE_DATA_DIR
        >>> from iode import Equation, scalars, variables
        >>> scalars.load(f"{SAMPLE_DATA_DIR}/fun.scl")
        >>> variables.load(f"{SAMPLE_DATA_DIR}/fun.var")
        >>> eq_ACAF = Equation("ACAF", "(ACAF / VAF[-1]) := acaf1 + acaf2 * GOSF[-1] + acaf4 * (TIME=1995)")
        >>> eq_ACAF         # doctest: +NORMALIZE_WHITESPACE
        Equation(lec: (ACAF / VAF[-1]) := acaf1 + acaf2 * GOSF[-1] + acaf4 * (TIME=1995),
            method: LSQ,
            sample: 1960Y1:2015Y1,
            comment: ,
            block: ,
            instruments: ,
            tests:
                corr: 0
                dw: 0
                fstat: 0
                loglik: 0
                meany: 0
                r2: 0
                r2adj: 0
                ssres: 0
                stderr: 0
                stderrp: 0
                stdev: 0
            date: )
        >>> eq_ACAF.get_coefficients_list()
        ['acaf1', 'acaf2', 'acaf4']
        >>> # clear Scalars database
        >>> scalars.clear()
        >>> # Do not create scalars in the Scalars database
        >>> eq_ACAF.get_coefficients_list(False)
        ['acaf1', 'acaf2', 'acaf4']
        >>> scalars.get_names()
        []
        >>> # create scalars on the flight
        >>> eq_ACAF.get_coefficients_list()
        ['acaf1', 'acaf2', 'acaf4']
        >>> # content of the Scalars database
        >>> scalars.get_names()
        ['acaf1', 'acaf2', 'acaf4']
        """
        return [coeff.decode() for coeff in self.c_equation.get_coefficients_list(create_if_not_exit)]

    # same comments as for coefficients
    def get_variables_list(self, create_if_not_exit: bool = True) -> List[str]:
        """
        Return the list of variables present in the equation.

        Parameters
        ----------
        create_if_not_exit: bool, optional
            Whether or not to create an entry in the Variables database for each variable not found in it. 
            Defaults to True.

        Returns
        -------
        list(str)

        Examples
        --------
        >>> from iode import SAMPLE_DATA_DIR
        >>> from iode import Equation, scalars, variables
        >>> scalars.load(f"{SAMPLE_DATA_DIR}/fun.scl")
        >>> variables.load(f"{SAMPLE_DATA_DIR}/fun.var")
        >>> eq_ACAF = Equation("ACAF", "(ACAF / VAF[-1]) := acaf1 + acaf2 * GOSF[-1] + acaf4 * (TIME=1995)")
        >>> eq_ACAF         # doctest: +NORMALIZE_WHITESPACE
        Equation(lec: (ACAF / VAF[-1]) := acaf1 + acaf2 * GOSF[-1] + acaf4 * (TIME=1995),
            method: LSQ,
            sample: 1960Y1:2015Y1,
            comment: ,
            block: ,
            instruments: ,
            tests:
                corr: 0
                dw: 0
                fstat: 0
                loglik: 0
                meany: 0
                r2: 0
                r2adj: 0
                ssres: 0
                stderr: 0
                stderrp: 0
                stdev: 0
            date: )
        >>> eq_ACAF.get_variables_list()
        ['ACAF', 'VAF', 'GOSF', 'TIME']
        >>> # clear Variables database + reset vars sample
        >>> variables.clear()
        >>> variables.sample = "1960Y1:2015Y1"
        >>> variables.sample
        1960Y1:2015Y1
        >>> # Do not create variables in the Variables database
        >>> eq_ACAF.get_variables_list(False)
        ['ACAF', 'VAF', 'GOSF', 'TIME']
        >>> variables.get_names()
        []
        >>> # create variables on the flight
        >>> eq_ACAF.get_variables_list()
        ['ACAF', 'VAF', 'GOSF', 'TIME']
        >>> # content of the Variables database
        >>> variables.get_names()
        ['ACAF', 'GOSF', 'TIME', 'VAF']
        """
        return [var.decode() for var in self.c_equation.get_variables_list(create_if_not_exit)]

    def split_equation(self) -> Tuple[str, str]:
        """
        Split an equation into its left-hand side and its right-hand side.

        Returns
        -------
        tuple(str, str)

        Examples
        --------
        >>> from iode import SAMPLE_DATA_DIR
        >>> from iode import Equation
        >>> eq_ACAF = Equation("ACAF", "(ACAF / VAF[-1]) := acaf1 + acaf2 * GOSF[-1] + acaf4 * (TIME=1995)")
        >>> lhs, rhs = eq_ACAF.split_equation()
        >>> # left-hand side
        >>> lhs
        '(ACAF / VAF[-1])'
        >>> # right-hand side
        >>> rhs
        'acaf1 + acaf2 * GOSF[-1] + acaf4 * (TIME=1995)'
        """
        lhs, rhs = self.c_equation.split_equation()
        return lhs.decode(), rhs.decode() 

    # Attributes access

    @property
    def lec(self) -> str:
        return self.c_equation.get_lec().decode()
    
    @property
    def method(self) -> str:
        return self.c_equation.get_method().decode()

    @method.setter
    def method(self, value: Union[str, int]):
        if not isinstance(value, (str, int)):
            raise TypeError("Expected value of type string or int.\nGot value of type '" + type(value).__name__ + "'")
        if isinstance(value, str):
            self.c_equation.set_method(<string>value.encode())
        else:
            # Note: <int> casting converts the Python type int to the cython.int type
            self.c_equation.set_method(<int>value)

    @property
    def sample(self) -> Sample:
        str_sample = self.c_equation.get_sample().to_string().decode()
        from_period, to_period = str_sample.split(':')
        return Sample(from_period, to_period)

    @sample.setter
    def sample(self, value: Union[str, Sample]):
        if not isinstance(value, (str, Sample)):
            raise TypeError("Expected value of type string or 'Sample'.\nGot value of type '" + type(value).__name__ + "'")
        if isinstance(value, Sample):
            value = str(value)
        from_period, to_period = value.split(':')
        self.c_equation.set_sample(from_period.encode(), to_period.encode())

    @property
    def comment(self) -> str:
        return self.c_equation.get_comment().decode()

    @comment.setter
    def comment(self, value: str):
        if not isinstance(value, str):
            raise TypeError("Expected value of type string.\nGot value of type '" + type(value).__name__ + "'")
        self.c_equation.set_comment(value.encode())

    @property
    def instruments(self) -> str:
        return self.c_equation.get_instruments().decode()

    @instruments.setter
    def instruments(self, value: str):
        if not isinstance(value, str):
            raise TypeError("Expected value of type string.\nGot value of type '" + type(value).__name__ + "'")
        self.c_equation.set_instruments(value.encode())

    @property
    def block(self) -> str:
        return self.c_equation.get_block().decode()

    @block.setter
    def block(self, value: str):
        if not isinstance(value, str):
            raise TypeError("Expected value of type string.\nGot value of type '" + type(value).__name__ + "'")
        self.c_equation.set_block(value.encode())

    @property
    def tests(self) -> Dict[str, float]:
        cdef map[string, float] cpp_tests = self.c_equation.get_tests_as_map()
        return {item.first.decode(): item.second for item in cpp_tests}

    @property
    def date(self) -> str:
        return self.get_date_format()

    # Special methods

    def __eq__(self, other: Equation) -> bool:
        if not isinstance(other, Equation):
            raise TypeError(f"Expected argument of type 'Equation'.\nGot argument of type '{type(other).__name__}'")
        return self.c_equation == other.c_equation

    def __str__(self) -> str:
        return self.c_equation.to_string().decode()

    def __repr__(self) -> str:
        return self.c_equation.to_string().decode()

    def __hash__(self) -> int:
        return <int>hash_value_eq(dereference(self.c_equation))
