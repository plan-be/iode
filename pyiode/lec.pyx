# distutils: language = c++

from lec cimport execute_lec as cpp_execute_lec


def execute_lec(lec: str, period: Union[str, int, Period]=None) -> Union[float, List[float]]:
    """
    Compute a LEC formula using the current Variables and Scalars databases.
    The formula may be evaluate at a specific period or on the whole sample 
    (no value for *period* is passed).
    
    Parameters
    ----------
    lec: str
        LEC expression to be evaluated.
    period: str or int or Period, optional
        Period at which the LEC formula is evaluated.
        If the passed value is of type int, then it represents 
        the position of the period in the current sample.
        If the passed value is None, empty string or -1, the LEC expression 
        is evaluated over the whole sample.
        Default to None.
    
    Returns
    -------
    float or list(float)

    Examples
    --------
    >>> from iode import SAMPLE_DATA_DIR
    >>> from iode import execute_lec, equations, scalars, variables
    >>> equations.load(f"{SAMPLE_DATA_DIR}/fun.eqs")
    >>> scalars.load(f"{SAMPLE_DATA_DIR}/fun.scl")
    >>> variables.load(f"{SAMPLE_DATA_DIR}/fun.var")

    >>> equations["ACAF"].lec       # doctest: +NORMALIZE_WHITESPACE
    '(ACAF/VAF[-1]) := acaf1+acaf2*GOSF[-1]+\\n                                 acaf4*(TIME=1995)'
    >>> variables["ACAF", "2000Y1"]
    10.046610792200543
    >>> lec = "(acaf1 + acaf2 * GOSF[-1] + acaf4*(TIME=1995)) * VAF[-1]"
    >>> # pass a period value as string
    >>> execute_lec(lec, "2000Y1")
    10.046610792200543
    >>> # pass a period position in the sample
    >>> t = variables.sample.index("2000Y1")
    >>> t
    40
    >>> execute_lec(lec, t)
    10.046610792200543
    >>> # compute the LEC formula over the whole sample
    >>> execute_lec(lec)        # doctest: +ELLIPSIS
    [-2e+37, 4.2884154594335815, 4.532163174288473, ..., -83.34062511080091, -96.41041982848331]
    >>> variables["ACAF"]       # doctest: +ELLIPSIS
    [-2e+37, -2e+37, -2e+37, ..., -83.34062511080091, -96.41041982848331]
    """
    # evaluate LEC expression over the whole sample
    if period is None:
        return cpp_execute_lec(lec.encode())

    if isinstance(period, int):
        if period >= 0:
            return cpp_execute_lec(<string>lec.encode(), <int>period)
        # evaluate LEC expression over the whole sample
        else:
            return cpp_execute_lec(lec.encode())

    if isinstance(period, Period):
        period = str(period)

    if isinstance(period, str):
        if len(period):
            return cpp_execute_lec(<string>lec.encode(), <string>period.encode())
        # evaluate LEC expression over the whole sample
        else:
           return cpp_execute_lec(lec.encode())
