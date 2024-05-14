import re

_list_separator = r",;\s"


def split_list(list_txt: str):
    """
    Split an IODE list written as a string and return a Python list.
    By default, the delimiters are ``, ;`` and any ``whitespace``.

    Parameters
    ----------
    list_txt: str
        IODE list written as a string.

    Returns
    -------
    list(str)

    Examples
    --------
    >>> from iode import split_list
    >>> well_written_list = "A;B;C;D;E;F;G;H"
    >>> split_list(well_written_list)
    ['A', 'B', 'C', 'D', 'E', 'F', 'G', 'H']
    >>> badly_written_list = "  A,B C;D  E,,,F;, G;H  "
    >>> split_list(badly_written_list)
    ['A', 'B', 'C', 'D', 'E', 'F', 'G', 'H']
    """
    if not isinstance(list_txt, str):
        raise TypeError(f"Expected value of type str. Got value of type {type(list_txt).__name__} instead.")
    return re.split(f"[{_list_separator}]+", list_txt.strip())
