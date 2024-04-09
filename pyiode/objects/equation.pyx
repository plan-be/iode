# distutils: language = c++

from collections.abc import Iterable
from typing import Union, Tuple, List, Optional

from cython.operator cimport dereference
from pyiode.objects.equation cimport EnumIodeEquationTest, CEquation
from pyiode.objects.equation cimport hash_value as hash_value_eq


# Equation wrapper class
# see https://cython.readthedocs.io/en/latest/src/userguide/wrapping_CPlusPlus.html#create-cython-wrapper-class 
cdef class Equation:

    cdef CEquation* c_equation

    def __cinit__(self, name: str, lec: str, method: Union[int, str] = "LSQ", from_period: Union[str, Period] = "", 
        to_period: Union[str, Period] = "", comment: str = "", instruments: str = "", block: str = "", 
        tests: Union[List[float], Tuple[float, ...]] = None, date: bool = False) -> Equation:
        """

        Attributes
        ----------
        lec: str
        method: str
        sample: Sample
        comment: str
        instruments: str
        block: str
            block of equations (to estimate) to which the equation belong.
        test_stdev: float
        test_meany: float
        test_ssres: float
        test_stderr: float
        test_fstat: float
        test_r2: float
        test_r2adj: float
        test_dw: float
        test_loglik: float
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
        >>> from iode import Equation
        >>> eq_ACAF = Equation("ACAF", "(ACAF / VAF[-1]) := acaf1 + acaf2 * GOSF[-1] + acaf4 * (TIME=1995)")
        >>> eq_ACAF         # doctest: +NORMALIZE_WHITESPACE
        Equation(lec: (ACAF / VAF[-1]) := acaf1 + acaf2 * GOSF[-1] + acaf4 * (TIME=1995),
            method: LSQ,
            sample: --,
            comment: ,
            block: ,
            instruments: ,
            tests: [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0],
            date: )
        """
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

        self.c_equation = new CEquation(name.encode("utf-8"), lec.encode("utf-8"), 0, from_period.encode("utf-8"), 
                                        to_period.encode("utf-8"), comment.encode("utf-8"), instruments.encode("utf-8"), 
                                        block.encode("utf-8"), <bint>date)
        self.method = method
        if tests is not None:
            if len(tests) != 10:
                raise ValueError(f"'tests': Expected iterable of size 10.\nGot {len(tests)} elements")
            for i, test_val in enumerate(tests): 
                self.c_equation.set_test(<EnumIodeEquationTest>i, test_val)

    def __dealloc__(self):
        del self.c_equation

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
        self.c_equation.set_lec(lec.encode("utf-8"), name.encode("utf-8"))

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
        >>> from iode import Variables, Equation, Sample
        >>> var_db = Variables(f"{SAMPLE_DATA_DIR}/fun.var")
        >>> var_db.sample
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
        self.c_equation.set_sample(from_period.encode("utf-8"), to_period.encode("utf-8"))

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
        >>> eq_ACAF.date
        ''
        >>> # date with specific format
        >>> eq_ACAF.get_date_format("dd/mm/yyyy")
        ''
        """
        if not isinstance(format, str):
            raise TypeError("Expected value of type string.\nGot value of type '" + type(format).__name__ + "'")     
        return self.c_equation.get_date_as_string(format.encode("utf-8")).decode("utf-8")

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
        >>> from iode import Equation, Scalars, Variables
        >>> scl_db = Scalars(f"{SAMPLE_DATA_DIR}/fun.scl")
        >>> var_db = Variables(f"{SAMPLE_DATA_DIR}/fun.var")
        >>> eq_ACAF = Equation("ACAF", "(ACAF / VAF[-1]) := acaf1 + acaf2 * GOSF[-1] + acaf4 * (TIME=1995)")
        >>> eq_ACAF         # doctest: +NORMALIZE_WHITESPACE
        Equation(lec: (ACAF / VAF[-1]) := acaf1 + acaf2 * GOSF[-1] + acaf4 * (TIME=1995),
            method: LSQ,
            sample: 1960Y1:2015Y1,
            comment: ,
            block: ,
            instruments: ,
            tests: [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0],
            date: )
        >>> eq_ACAF.get_coefficients_list()
        ['acaf1', 'acaf2', 'acaf4']
        >>> # clear Scalars database
        >>> scl_db.clear()
        >>> # Do not create scalars in the Scalars database
        >>> eq_ACAF.get_coefficients_list(False)
        ['acaf1', 'acaf2', 'acaf4']
        >>> scl_db.get_names()
        []
        >>> # create scalars on the flight
        >>> eq_ACAF.get_coefficients_list()
        ['acaf1', 'acaf2', 'acaf4']
        >>> # content of the Scalars database
        >>> scl_db.get_names()
        ['acaf1', 'acaf2', 'acaf4']
        """
        return [coeff.decode("utf-8") for coeff in self.c_equation.get_coefficients_list(create_if_not_exit)]

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
        >>> from iode import Equation, Scalars, Variables
        >>> scl_db = Scalars(f"{SAMPLE_DATA_DIR}/fun.scl")
        >>> var_db = Variables(f"{SAMPLE_DATA_DIR}/fun.var")
        >>> eq_ACAF = Equation("ACAF", "(ACAF / VAF[-1]) := acaf1 + acaf2 * GOSF[-1] + acaf4 * (TIME=1995)")
        >>> eq_ACAF         # doctest: +NORMALIZE_WHITESPACE
        Equation(lec: (ACAF / VAF[-1]) := acaf1 + acaf2 * GOSF[-1] + acaf4 * (TIME=1995),
            method: LSQ,
            sample: 1960Y1:2015Y1,
            comment: ,
            block: ,
            instruments: ,
            tests: [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0],
            date: )
        >>> eq_ACAF.get_variables_list()
        ['ACAF', 'VAF', 'GOSF', 'TIME']
        >>> # clear Variables database + reset vars sample
        >>> var_db.clear()
        >>> var_db.sample = "1960Y1:2015Y1"
        >>> var_db.sample
        1960Y1:2015Y1
        >>> # Do not create variables in the Variables database
        >>> eq_ACAF.get_variables_list(False)
        ['ACAF', 'VAF', 'GOSF', 'TIME']
        >>> var_db.get_names()
        []
        >>> # create variables on the flight
        >>> eq_ACAF.get_variables_list()
        ['ACAF', 'VAF', 'GOSF', 'TIME']
        >>> # content of the Variables database
        >>> var_db.get_names()
        ['ACAF', 'GOSF', 'TIME', 'VAF']
        """
        return [var.decode("utf-8") for var in self.c_equation.get_variables_list(create_if_not_exit)]

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
        return lhs.decode("utf-8"), rhs.decode("utf-8") 

    # Attributes access

    @property
    def lec(self) -> str:
        return self.c_equation.get_lec().decode("utf-8")
    
    @property
    def method(self) -> str:
        return self.c_equation.get_method().decode("utf-8")

    @method.setter
    def method(self, value: Union[str, int]):
        if not isinstance(value, (str, int)):
            raise TypeError("Expected value of type string or int.\nGot value of type '" + type(value).__name__ + "'")
        if isinstance(value, str):
            self.c_equation.set_method(<string>value.encode("utf-8"))
        else:
            # Note: <int> casting converts the Python type int to the cython.int type
            self.c_equation.set_method(<int>value)

    @property
    def sample(self) -> Sample:
        str_sample = self.c_equation.get_sample().to_string().decode("utf-8")
        from_period, to_period = str_sample.split(':')
        return Sample(from_period, to_period)

    @sample.setter
    def sample(self, value: Union[str, Sample]):
        if not isinstance(value, (str, Sample)):
            raise TypeError("Expected value of type string or 'Sample'.\nGot value of type '" + type(value).__name__ + "'")
        if isinstance(value, Sample):
            value = str(value)
        from_period, to_period = value.split(':')
        self.c_equation.set_sample(from_period.encode("utf-8"), to_period.encode("utf-8"))

    @property
    def comment(self) -> str:
        return self.c_equation.get_comment().decode("utf-8")

    @comment.setter
    def comment(self, value: str):
        if not isinstance(value, str):
            raise TypeError("Expected value of type string.\nGot value of type '" + type(value).__name__ + "'")
        self.c_equation.set_comment(value.encode("utf-8"))

    @property
    def instruments(self) -> str:
        return self.c_equation.get_instruments().decode("utf-8")

    @instruments.setter
    def instruments(self, value: str):
        if not isinstance(value, str):
            raise TypeError("Expected value of type string.\nGot value of type '" + type(value).__name__ + "'")
        self.c_equation.set_instruments(value.encode("utf-8"))

    @property
    def block(self) -> str:
        return self.c_equation.get_block().decode("utf-8")

    @block.setter
    def block(self, value: str):
        if not isinstance(value, str):
            raise TypeError("Expected value of type string.\nGot value of type '" + type(value).__name__ + "'")
        self.c_equation.set_block(value.encode("utf-8"))

    @property
    def tests(self) -> List[float]:
        return self.c_equation.tests

    @property
    def test_stdev(self) -> float:
        return self.c_equation.get_test_stdev() 

    @property
    def test_meany(self) -> float:
        return self.c_equation.get_test_meany() 

    @property
    def test_ssres(self) -> float:
        return self.c_equation.get_test_ssres() 

    @property
    def test_stderr(self) -> float:
        return self.c_equation.get_test_stderr() 

    @property
    def test_fstat(self) -> float:
        return self.c_equation.get_test_fstat() 

    @property
    def test_r2(self) -> float:
        return self.c_equation.get_test_r2() 

    @property
    def test_r2adj(self) -> float:
        return self.c_equation.get_test_r2adj() 

    @property
    def test_dw(self) -> float:
        return self.c_equation.get_test_dw() 

    @property
    def test_loglik(self) -> float:
        return self.c_equation.get_test_loglik()

    @property
    def date(self) -> str:
        return self.get_date_format()

    # Special methods

    def __eq__(self, other: Equation) -> bool:
        if not isinstance(other, Equation):
            raise TypeError(f"Expected argument of type 'Equation'.\nGot argument of type '{type(other).__name__}'")
        return self.c_equation == other.c_equation

    def __str__(self) -> str:
        return self.c_equation.to_string().decode("utf-8")

    def __repr__(self) -> str:
        return self.c_equation.to_string().decode("utf-8")

    def __hash__(self) -> int:
        return <int>hash_value_eq(dereference(self.c_equation))
