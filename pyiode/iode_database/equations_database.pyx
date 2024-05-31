# distutils: language = c++

from collections.abc import Iterable
from typing import Union, Tuple, List, Dict, Optional, Any
if sys.version_info.minor >= 11:
    from typing import Self
else:
    Self = Any

cimport cython
from cython.operator cimport dereference
from pyiode.objects.equation cimport CEquation
from pyiode.iode_database.cpp_api_database cimport KDBEquations as CKDBEquations
from pyiode.iode_database.cpp_api_database cimport Equations as cpp_global_equations
from pyiode.iode_database.cpp_api_database cimport Variables as cpp_global_variables
from pyiode.iode_database.cpp_api_database cimport eqs_estimate as cpp_eqs_estimate

EquationInput = Union[str, Dict[str, Any], Equation]


@cython.final
cdef class Equations(_AbstractDatabase):
    """
    IODE Equations database. 

    Attributes
    ----------
    filename: str
    description: str

    Parameters
    ----------
    filepath: str, optional
        file containing the IODE equations to load.

    Returns
    -------
    Equations

    Examples
    --------
    >>> from iode import equations, SAMPLE_DATA_DIR
    >>> equations.load(f"{SAMPLE_DATA_DIR}/fun.eqs")
    >>> len(equations)
    274
    """
    cdef bint ptr_owner
    cdef CKDBEquations* database_ptr

    def __cinit__(self, filepath: str = None) -> Equations:
        self.database_ptr = NULL
        self.abstract_db_ptr = NULL
        self.ptr_owner = False

    def __init__(self, filepath: str = None):
        # Prevent accidental instantiation from normal Python code
        # since we cannot pass a struct pointer into a Python constructor.
        raise TypeError("This class cannot be instantiated directly.")

    def __dealloc__(self):
        # if self.database_ptr points to the C++ global instance Equations 
        # which does not need to be manually deleted 
        if self.ptr_owner and self.database_ptr is not NULL:
            del self.database_ptr
            self.database_ptr = NULL

    # see https://cython.readthedocs.io/en/stable/src/userguide/extension_types.html#instantiation-from-existing-c-c-pointers 
    @staticmethod
    cdef Equations _from_ptr(CKDBEquations* database_ptr = NULL):
        # call to __new__() that bypasses the __init__() constructor.
        cdef Equations wrapper = Equations.__new__(Equations)
        if database_ptr is not NULL:
            wrapper.ptr_owner = True
            wrapper.database_ptr = database_ptr
            wrapper.abstract_db_ptr = database_ptr
        else:
            wrapper.ptr_owner = False
            wrapper.database_ptr = &cpp_global_equations
            wrapper.abstract_db_ptr = &cpp_global_equations
        return wrapper

    # TODO: implement KDBAbstract::load() method (for global KDB only)
    def _load(self, filepath: str):
        cdef CKDBEquations* kdb = new CKDBEquations(filepath.encode())
        del kdb

    def _subset(self, pattern: str, copy: bool) -> Equations:
        # call to __new__() that bypasses the __init__() constructor.
        cdef Equations subset_ = Equations.__new__(Equations)
        subset_.database_ptr = subset_.abstract_db_ptr = self.database_ptr.subset(pattern.encode(), <bint>copy)
        return subset_

    def _get_object(self, key):
        if not isinstance(key, str):
            raise TypeError(f"Cannot get object {key}.\nExpected a string value for {key} " + 
                            f"but got value of type {type(key).__name__}")
        key = key.strip()

        c_eq = self.database_ptr.get(key.encode())
        str_sample = c_eq.get_sample().to_string().decode()
        from_period, to_period = str_sample.split(':')
        eq = Equation(key, c_eq.get_lec().decode(), c_eq.get_method().decode(), 
                      from_period, to_period, c_eq.get_comment().decode(), 
                      c_eq.get_instruments().decode(), c_eq.get_block().decode())
        eq.c_equation.date = c_eq.date
        for i, test_val in enumerate(c_eq.tests): 
            eq.c_equation.set_test(<EnumIodeEquationTest>i, test_val)
        return eq

    def _set_object(self, key, value):
        cdef CEquation* c_equation
        if not isinstance(key, str):
            raise TypeError(f"Cannot set object {key}.\nExpected a string value for {key} " + 
                            f"but got value of type {type(key).__name__}")
        key = key.strip()

        if self.database_ptr.contains(key.encode()):
            # update existing equation
            if isinstance(value, str):
                equation = self._get_object(key)
                equation.set_lec(value.strip(), key)
            elif isinstance(value, Equation):
                equation = value
            elif isinstance(value, dict):
                equation = self._get_object(key)
                if 'lec' in value:
                    equation.set_lec(value.pop('lec').strip(), key)
                if 'method' in value:
                    equation.method = value.pop('method')
                if 'sample' in value:
                    equation.sample = value.pop('sample')
                if 'comment' in value:
                    equation.comment = value.pop('comment')
                if 'instruments' in value:
                    equation.instruments = value.pop('instruments')
                if 'block' in value:
                    equation.block = value.pop('block')
                if 'tests' in value:
                    warnings.warn(f"Update equation '{key}': 'tests' cannot be updated manually. Skipped new values.")
                    del value['tests']
                if 'date' in value:
                    warnings.warn(f"Update equation '{key}': 'date' cannot be updated manually. Skipped new value.")
                    del value['date']
                if len(value):
                    raise ValueError(f"Update equation '{key}': only 'lec', 'method', 'sample', 'comment', "
                                     f"'instruments' and 'block' keys are accepted. "
                                     f"Got unknown key(s): {';'.join(value.keys())}")
            else:
                raise TypeError(f"Update equation '{key}': Expected input to be of type str or dict or Equation. "
                                f"Got value of type {type(value).__name__}")
            c_equation = (<Equation>equation).c_equation
            self.database_ptr.update(key.encode(), dereference(c_equation))
        else:
            # add a new equation
            if isinstance(value, str):
                equation = Equation(name=key, lec=value.strip())
            elif isinstance(value, Equation):
                equation = value
            elif isinstance(value, (tuple, list)):
                value = [key] + list(value)
                equation = Equation(*value)
            elif isinstance(value, dict):
                value['name'] = key
                equation = Equation(**value)
            else:
                raise TypeError(f"New equation '{key}': Expected input to be of type str or tuple or list or "
                                f"dict or Equation. Got value of type {type(value).__name__}")
            c_equation = (<Equation>equation).c_equation
            self.database_ptr.add(key.encode(), dereference(c_equation))

    def estimate(self, from_period: Union[str, Period]=None, to_period: Union[str, Period]=None, list_eqs: Union[str, List[str]]=None):
        r"""
        Estimate an equation or a block of equations.

        At the end of the estimation process, certain variables and scalars are automatically created 
        if the process has converged. These variables and scalars can be used for computational purposes and, 
        as they are part of the global workspace, can be saved for future use.

        The tests resulting from the last estimation are saved as scalars. The same applies to residuals, 
        left-hand and right-hand members of equations.

        Saved tests (as scalars) have the following names (`e<ith>_*` for the <ith> equation of the block):

            - `e<ith>_n`  : number of sample periods 
            - `e<ith>_k`  : number of estimated coefficients 
            - `e<ith>_stdev` : std dev of residuals 
            - `e<ith>_meany` : mean of Y 
            - `e<ith>_ssres` : sum of squares of residuals 
            - `e<ith>_stderr` : std error 
            - `e<ith>_stderrp` : std error percent (in %) 
            - `e<ith>_fstat` : F-Stat 
            - `e<ith>_r2`  : R square 
            - `e<ith>_r2adj`  : adjusted R-squared 
            - `e<ith>_dw` : Durbin-Watson 
            - `e<ith>_loglik`  : Log Likelihood 

        Calculated series are saved in special variables:

            - `_YCALC<i>` : right-hand side of the <ith> equation in the block
            - `_YOBS<i>` : left-hand side of the <ith> equation in the block 
            - `_YRES<i>` : residuals of the <ith> equation of the block 

        Outside the estimation sample, the series values are :math:`NA`.

        Parameters
        ----------
        from_period : str or Period, optional
            The starting period of the execution range. 
            Defaults to the starting period of the current Variables sample.
        to_period : str or Period, optional
            The ending period of the execution range.
            Defaults to the ending period of the current Variables sample.
        list_eqs: str or list(str), optional
            List of equations to be estimated. 
            If not provided, all equations of the present Equations database will be estimated.
            Default to None (all equations).

        Warnings
        --------
        If some equations to be estimated belongs to a same block, the *block* (:meth:`Equation.block`), 
        *method* (:meth:`Equation.method`), and *instruments* (':meth:`Equation.instruments`) of each of 
        them must have been updated before.

        Examples
        --------
        >>> from iode import SAMPLE_DATA_DIR, equations, scalars, variables
        >>> equations.load(f"{SAMPLE_DATA_DIR}/fun.eqs")
        >>> variables.load(f"{SAMPLE_DATA_DIR}/fun.var")

        >>> equations["ACAF"].lec
        '(ACAF/VAF[-1]) :=acaf1+acaf2*GOSF[-1]+\nacaf4*(TIME=1995)'
        >>> equations["DPUH"].lec
        'dln (DPUH/DPUHO):=dpuh_1+dpuh_2*dln(IHU/PI5)+dln PC'

        >>> # create scalars
        >>> scalars["acaf1"] = 0., 1.
        >>> scalars["acaf2"] = 0., 1.
        >>> scalars["acaf4"] = 0., 1.
        >>> scalars["dpuh_1"] = 0., 1.
        >>> scalars["dpuh_2"] = 0., 1.

        >>> # estimate an equation
        >>> equations.estimate("1980Y1", "1996Y1", "ACAF")
        >>> scalars["acaf1"]
        Scalar(0.0157705, 1, 0.00136949)
        >>> scalars["acaf2"]
        Scalar(-7.96505e-06, 1, 1.48247e-06)
        >>> scalars["acaf4"]
        Scalar(-0.0085027, 1, 0.00208257)
        >>> scalars["dpuh_1"]
        Scalar(0, 1, nan)
        >>> scalars["dpuh_2"]
        Scalar(0, 1, nan)

        >>> # estimate a block
        >>> # prepare equations (same block and method)
        >>> block = "ACAF;DPUH"
        >>> for name in block.split(";"):
        ...     equations[name] = {"block": block, "method": "LSQ"}
        >>> equations.estimate("1980Y1", "1996Y1", block)
        >>> scalars["acaf1"]
        Scalar(0.0157705, 1, 0.00136079)
        >>> scalars["acaf2"]
        Scalar(-7.96505e-06, 1, 1.47188e-06)
        >>> scalars["acaf4"]
        Scalar(-0.0085027, 1, 0.00206603)
        >>> scalars["dpuh_1"]
        Scalar(0.0109855, 1, 0.00481857)
        >>> scalars["dpuh_2"]
        Scalar(0.0574893, 1, 0.0368951)
        """
        if from_period is None or to_period is None:
            c_sample = cpp_global_variables.get_sample()
            if from_period is None:
                from_period = c_sample.start_period().to_string().decode()
            if to_period is None:
                to_period = c_sample.end_period().to_string().decode()
        
        if isinstance(from_period, Period):
            from_period = str(from_period)

        if isinstance(to_period, Period):
            to_period = str(to_period)

        if list_eqs is None:
            list_eqs = ';'.join(self.get_names())

        if not isinstance(list_eqs, str) and isinstance(list_eqs, Iterable) and \
            all(isinstance(item, str) for item in list_eqs):
            list_eqs = ';'.join(list_eqs)
        
        cpp_eqs_estimate(list_eqs.encode(), from_period.encode(), to_period.encode())


equations: Equations = Equations._from_ptr()
