# distutils: language = c++

from collections.abc import Iterable
from typing import Union, Tuple, List, Optional, Any

from pyiode.common cimport EnumIodeLtoH, EnumIodeHtoL
from pyiode.iode_database.cpp_api_database cimport low_to_high as cpp_low_to_high 
from pyiode.iode_database.cpp_api_database cimport high_to_low as cpp_high_to_low


def low_to_high(type_of_series: str, method: str, filepath: str, var_list: Union[str, List[str]]):
    """
    Transform low periodicity series to high periodicity series (i.e. variables). 

    Parameters
    ----------
    type_of_series : str
        Two types of series are considered: 'stock' and 'flow':
            - 'S' (LTOH_STOCK) : the interpolated values are of the same order of magnitude as the original values
            - 'F' (LTOH_FLOW) : the values of the sub-periods are additive over a period

    method : str
        Method to use for transformation. Three methods can be used:
            - 'L' (LTOH_LINEAR) : Linear interpolation
            - 'C' (LTOH_CUBIC_SPLINESS) : Cubic Spliness
            - 'S' (LTOH_STEP) : Steps

    filepath : str
        Filepath to the source data file.

    var_list : str or list(str)
        List of variables to include in the transformation.

    Returns
    -------
    None

    Examples
    --------
    >>> from iode import SAMPLE_DATA_DIR, LTOH_STOCK, LTOH_FLOW
    >>> from iode import LTOH_CUBIC_SPLINESS, LTOH_LINEAR, LTOH_STEP
    >>> from iode import variables, low_to_high
    >>> variables.clear()
    >>> # define a yearly sample
    >>> variables.sample = "2010Q1:2020Q4"
    >>> # input filepath
    >>> filepath = f"{SAMPLE_DATA_DIR}/fun.var"
    
    Linear interpolation / stock
    
    >>> # "stock" -> the result is a linear interpolation of the 2 surrounding source values.
    >>> low_to_high(LTOH_STOCK, LTOH_LINEAR, filepath, ["ACAF", "ACAG"])
    >>> variables["ACAF", "2014Q1":"2014Q4"]
    [-72.50614701966526, -76.11763971671049, -79.7291324137557, -83.34062511080091]
    >>> variables["ACAG", "2014Q1":"2014Q4"]
    [31.63267881532112, 31.895218819946884, 32.15775882457264, 32.4202988291984]

    Linear interpolation / flow
    
    >>> # "flow" -> the result is the source value divided by the nb of sub-periods. 
    >>> low_to_high(LTOH_FLOW, LTOH_LINEAR, filepath, ["ACAF", "ACAG"])
    >>> variables["ACAF", "2014Q1":"2014Q4"]
    [-20.83515627770023, -20.83515627770023, -20.83515627770023, -20.83515627770023]
    >>> variables["ACAG", "2014Q1":"2014Q4"]
    [8.1050747072996, 8.1050747072996, 8.1050747072996, 8.1050747072996]

    Cubic splines / stock
    
    >>> low_to_high(LTOH_STOCK, LTOH_CUBIC_SPLINESS, filepath, ["ACAF", "ACAG"])
    >>> variables["ACAF", "2012Q1":"2012Q4"]
    [-47.2984169294621, -50.052041225380975, -52.80566552129986, -55.55928981721873]
    >>> variables["ACAG", "2012Q1":"2012Q4"]
    [29.544440560604077, 29.804280757173238, 30.064120953742403, 30.323961150311572]

    Cubic splines / flow
    
    >>> low_to_high(LTOH_FLOW, LTOH_CUBIC_SPLINESS, filepath, ["ACAF", "ACAG"])
    >>> variables["ACAF", "2012Q1":"2012Q4"]
    [-12.748422687629207, -13.436828761608925, -14.270289043196508, -15.103749324784092]
    >>> variables["ACAG", "2012Q1":"2012Q4"]
    [7.483230672890683, 7.548190722032976, 7.6135768258069625, 7.678962929580949]

    Step / stock
    
    >>> # "stock" -> the result has the same value as the source
    >>> low_to_high(LTOH_STOCK, LTOH_STEP, filepath, ["ACAF", "ACAG"])
    >>> variables["ACAF", "2014Q1":"2014Q4"]
    [-83.34062511080091, -83.34062511080091, -83.34062511080091, -83.34062511080091]
    >>> variables["ACAG", "2014Q1":"2014Q4"]
    [32.4202988291984, 32.4202988291984, 32.4202988291984, 32.4202988291984]

    Step / flow
    
    >>> # "flow" -> the result is the source value plus a portion of 
    >>> # the difference between the 2 surrounding values in the source
    >>> low_to_high(LTOH_FLOW, LTOH_STEP, filepath, ["ACAF", "ACAG"])
    >>> variables["ACAF", "2014Q1":"2014Q4"]
    [-20.83515627770023, -20.83515627770023, -20.83515627770023, -20.83515627770023]
    >>> variables["ACAG", "2014Q1":"2014Q4"]
    [8.1050747072996, 8.1050747072996, 8.1050747072996, 8.1050747072996]
    """
    if not isinstance(type_of_series, str):
        raise TypeError(f"'type_of_series': Expected value of type str. Got value of type {type(type_of_series).__name__} instead")

    type_of_series = type_of_series.upper()
    if type_of_series not in "SF":
        raise ValueError(f"'type_of_series': possible values are 'S' (LTOH_STOCK) or 'F' (LTOH_FLOW). "
                         f"Got value {type_of_series} instead")

    if not isinstance(method, str):
        raise TypeError(f"'method': Expected value of type str of one character. Got value of type {type(method).__name__} instead")

    if method not in "LCS":
        raise ValueError(f"'method': possible values are 'L' (LTOH_LINEAR), 'C' (LTOH_CUBIC_SPLINESS) or 'S' (LTOH_STEP). " 
                         f"Got value {method} instead")

    if not isinstance(filepath, str):
        raise TypeError(f"'filepath': Expected value of type str. Got value of type {type(filepath).__name__} instead")

    if isinstance(var_list, Iterable) and all(isinstance(item, str) for item in var_list):
        var_list = ';'.join(var_list)

    if not isinstance(var_list, str):
        raise TypeError(f"'filepath': Expected value of type str or list of str. Got value of type {type(filepath).__name__} instead")

    i_type_of_series = LTOH_SERIES_TYPES_DICT[type_of_series]
    cpp_low_to_high(<EnumIodeLtoH>i_type_of_series, <char>ord(method), filepath.encode(), var_list.encode())


def high_to_low(type_of_series: int, filepath: str, var_list: Union[str, List[str]]):
    """
    Transform high periodicity to low periodicity series (i.e. variables).

    Parameters
    ----------
    type_of_series : int
        Three types of series are considered : 
            - HTOL_LAST : last sub-period value
            - HTOL_MEAN : average of sub-period data
            - HTOL_SUM : addition of sub-period data

    filepath : str
        Filepath to the source data file.

    var_list : str or list(str)
        List of variables to include in the transformation.

    Examples
    --------
    >>> from iode import SAMPLE_DATA_DIR, HTOL_LAST, HTOL_MEAN, HTOL_SUM
    >>> from iode import variables, high_to_low
    >>> variables.clear()
    >>> # define a yearly sample
    >>> variables.sample = "2000Y1:2020Y1"
    >>> # input filepath
    >>> filepath = f"{SAMPLE_DATA_DIR}/fun_q.var"
    
    Last Obs in year
    
    >>> high_to_low(HTOL_LAST, filepath, ["ACAF", "ACAG"])
    >>> variables["ACAF", "2010Y1":"2014Y1"]
    [-37.82742883229439, -44.544792633543224, -55.55928981721873, -68.89465432262006, -83.34062511080091]
    >>> variables["ACAG", "2010Y1":"2014Y1"]
    [7.063482244552621, 7.321150091008727, 7.580990287577893, 7.842534702673841, 8.1050747072996]

    Mean of year
    
    >>> high_to_low(HTOL_MEAN, filepath, ["ACAF", "ACAG"])
    >>> variables["ACAF", "2010Y1":"2014Y1"]
    [-37.82742883229439, -44.544792633543224, -55.55928981721873, -68.89465432262006, -83.34062511080091]
    >>> variables["ACAG", "2010Y1":"2014Y1"]
    [7.063482244552621, 7.321150091008727, 7.580990287577893, 7.842534702673841, 8.1050747072996]

    Sum
    
    >>> high_to_low(HTOL_SUM, filepath, ["ACAF", "ACAG"])
    >>> variables["ACAF", "2010Y1":"2014Y1"]
    [-151.30971532917755, -178.1791705341729, -222.23715926887493, -275.5786172904802, -333.36250044320366]
    >>> variables["ACAG", "2010Y1":"2014Y1"]
    [28.253928978210485, 29.284600364034908, 30.323961150311572, 31.370138810695362, 32.4202988291984]
    """
    if not isinstance(type_of_series, int):
        raise TypeError(f"'type_of_series': Expected value of type int. Got value of type {type(type_of_series).__name__} instead")

    if type_of_series not in [HTOL_LAST, HTOL_MEAN, HTOL_SUM]:
        raise ValueError(f"'type_of_series': possible values are HTOL_LAST, HTOL_MEAN or HTOL_SUM. Got value {type_of_series} instead")

    if not isinstance(filepath, str):
        raise TypeError(f"'filepath': Expected value of type str. Got value of type {type(filepath).__name__} instead")

    if isinstance(var_list, Iterable) and all(isinstance(item, str) for item in var_list):
        var_list = ';'.join(var_list)

    if not isinstance(var_list, str):
        raise TypeError(f"'filepath': Expected value of type str or list of str. Got value of type {type(filepath).__name__} instead")

    cpp_high_to_low(<EnumIodeHtoL>type_of_series, filepath.encode(), var_list.encode())
