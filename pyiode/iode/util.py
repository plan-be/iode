import re
from enum import Enum, auto
from pathlib import Path
from textwrap import wrap

from typing import List, Dict, Any

from iode.iode_python import (is_NA, IodeFileType, IodeType, IODE_DEFAULT_DATABASE_FILENAME, 
                              iode_msg_path, print_error_msg)
from .common import IODE_FILE_TYPES, FileType

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


def join_lines(value: str) -> str:
    return ' '.join(line.strip() for line in value.splitlines())


class JUSTIFY(Enum):
    LEFT = auto()
    CENTER = auto()
    RIGHT = auto()


def table2str(columns: Dict[str, List[str]], sep: str = '\t', justify_funcs: Dict[str, JUSTIFY] = None, 
              max_lines: int = -1, edgeitems: int = 5, max_width_col: int = 100, max_width: int = 200, 
              precision: int = 4) -> str:
    r"""
    Return table (dict of columns) as string

    Parameters
    ----------
    columns: dict(str, list(str))
        Pairs (column name, column values) of the table to return as string.
    sep: str, optional 
        Separator between columns. 
        Defaults to '\t'.
    justify_funcs: dict[str, JUSTIFY], optional 
        Justify function to apply on string representation of column values. 
        Defaults to RIGHT for all columns.
    max_lines: int, optional 
        Maximum number of lines to show. 
        Defaults to -1 (show all lines).
    edgeitems : int, optional
        If number of lines to display is greater than max_lines, only the first and 
        last edgeitems lines are displayed. Only active if max_lines is not -1. 
        Defaults to 5.
    max_width_col: int, optional
        Maximum width (= nb characters) for any column.
        Defaults to 100.
    max_width: int, optional 
        Maximum width (= nb characters) for lines to show. 
        A value of -1 means unlimited width.
        Defaults to 200.
    precision: int, optional 
        Precision of the string representation of the float values to show. 
        Defaults to 4.

    Returns
    -------
        str

    Examples
    --------
    >>> from iode.util import table2str, JUSTIFY
    >>> columns = {'category_1': [23.9010175, 18.07965729, 95.38763094, 7.605285, 45.77738378, 31.20329029, 26.24638991, 94.85772881, 17.19156602, 86.97879573], 
    ...            'category_2': [39.52903949, 91.1525069, 83.76037566, 43.83771813, 58.17427187, 80.56636471, 76.80209378, 53.63765832, 15.94267299, 88.42504629], 
    ...            'category_3': [64.36370439, 53.8316863, 49.02940035, 61.91318117, 25.01676303, 7.52817659, 9.18225827, 16.68777191, 46.39337746, 27.30904207], 
    ...            'category_4': [92.90886005, 98.98271845, 16.87014647, 52.23406945, 75.85269188, 68.53109561, 87.69120031, 6.19099948, 57.21632568, 13.4801123], 
    ...            'category_5': [55.99231002, 29.0795402, 6.06556571, 15.85292046, 66.87820451, 61.1155681, 16.17608971, 49.67034014, 29.84330636, 73.399402], 
    ...            'category_6': [84.74263881, 86.74429788, 14.59513443, 21.0325867, 81.49218407, 0.09002046, 6.96791193, 84.88923794, 82.1819903, 1.07949955], 
    ...            'category_7': [80.65553154, 97.73020615, 27.59011683, 47.88146724, 86.22525403, 71.41743477, 67.53744625, 75.27254816, 92.51563273, 87.1841808], 
    ...            'category_8': [81.92785908, 49.47791145, 95.31219267, 78.94364085, 62.62581941, 29.65099761, 23.61973127, 81.37426694, 62.02338724, 73.65105918], 
    ...            'category_9': [18.58065375, 58.86699547, 28.93429997, 40.62139687, 47.13588669, 3.0366871, 70.22586303, 16.8850943, 3.34322589, 8.02234255], 
    ...            'category_10': [31.39205972, 63.99854372, 72.89885168, 76.08958234, 40.29562424, 19.04340988, 24.46528084, 62.13187458, 71.05022862, 58.76206382]}
    >>> s = table2str(columns)
    >>> print(s)        # doctest: +NORMALIZE_WHITESPACE
    category_1  category_2      category_3      category_4      category_5      category_6      category_7      category_8      category_9      category_10
       23.9010     39.5290         64.3637         92.9089         55.9923         84.7426         80.6555         81.9279         18.5807          31.3921
       18.0797     91.1525         53.8317         98.9827         29.0795         86.7443         97.7302         49.4779         58.8670          63.9985
       95.3876     83.7604         49.0294         16.8701          6.0656         14.5951         27.5901         95.3122         28.9343          72.8989
        7.6053     43.8377         61.9132         52.2341         15.8529         21.0326         47.8815         78.9436         40.6214          76.0896
       45.7774     58.1743         25.0168         75.8527         66.8782         81.4922         86.2253         62.6258         47.1359          40.2956
       31.2033     80.5664          7.5282         68.5311         61.1156          0.0900         71.4174         29.6510          3.0367          19.0434
       26.2464     76.8021          9.1823         87.6912         16.1761          6.9679         67.5374         23.6197         70.2259          24.4653
       94.8577     53.6377         16.6878          6.1910         49.6703         84.8892         75.2725         81.3743         16.8851          62.1319
       17.1916     15.9427         46.3934         57.2163         29.8433         82.1820         92.5156         62.0234          3.3432          71.0502
       86.9788     88.4250         27.3090         13.4801         73.3994          1.0795         87.1842         73.6511          8.0223          58.7621
    <BLANKLINE>
    >>> s = table2str(columns, max_lines=6, edgeitems=3)
    >>> print(s)        # doctest: +NORMALIZE_WHITESPACE
    category_1  category_2      category_3      category_4      category_5      category_6      category_7      category_8      category_9      category_10
       23.9010     39.5290         64.3637         92.9089         55.9923         84.7426         80.6555         81.9279         18.5807          31.3921
       18.0797     91.1525         53.8317         98.9827         29.0795         86.7443         97.7302         49.4779         58.8670          63.9985
       95.3876     83.7604         49.0294         16.8701          6.0656         14.5951         27.5901         95.3122         28.9343          72.8989
           ...         ...             ...             ...             ...             ...             ...             ...             ...              ...
       94.8577     53.6377         16.6878          6.1910         49.6703         84.8892         75.2725         81.3743         16.8851          62.1319
       17.1916     15.9427         46.3934         57.2163         29.8433         82.1820         92.5156         62.0234          3.3432          71.0502
       86.9788     88.4250         27.3090         13.4801         73.3994          1.0795         87.1842         73.6511          8.0223          58.7621
    <BLANKLINE>
    >>> s = table2str(columns, sep=' | ')
    >>> print(s)        # doctest: +NORMALIZE_WHITESPACE
    category_1 | category_2 | category_3 | category_4 | category_5 | category_6 | category_7 | category_8 | category_9 | category_10
       23.9010 |    39.5290 |    64.3637 |    92.9089 |    55.9923 |    84.7426 |    80.6555 |    81.9279 |    18.5807 |     31.3921
       18.0797 |    91.1525 |    53.8317 |    98.9827 |    29.0795 |    86.7443 |    97.7302 |    49.4779 |    58.8670 |     63.9985
       95.3876 |    83.7604 |    49.0294 |    16.8701 |     6.0656 |    14.5951 |    27.5901 |    95.3122 |    28.9343 |     72.8989
        7.6053 |    43.8377 |    61.9132 |    52.2341 |    15.8529 |    21.0326 |    47.8815 |    78.9436 |    40.6214 |     76.0896
       45.7774 |    58.1743 |    25.0168 |    75.8527 |    66.8782 |    81.4922 |    86.2253 |    62.6258 |    47.1359 |     40.2956
       31.2033 |    80.5664 |     7.5282 |    68.5311 |    61.1156 |     0.0900 |    71.4174 |    29.6510 |     3.0367 |     19.0434
       26.2464 |    76.8021 |     9.1823 |    87.6912 |    16.1761 |     6.9679 |    67.5374 |    23.6197 |    70.2259 |     24.4653
       94.8577 |    53.6377 |    16.6878 |     6.1910 |    49.6703 |    84.8892 |    75.2725 |    81.3743 |    16.8851 |     62.1319
       17.1916 |    15.9427 |    46.3934 |    57.2163 |    29.8433 |    82.1820 |    92.5156 |    62.0234 |     3.3432 |     71.0502
       86.9788 |    88.4250 |    27.3090 |    13.4801 |    73.3994 |     1.0795 |    87.1842 |    73.6511 |     8.0223 |     58.7621
    <BLANKLINE>
    >>> s = table2str(columns, max_width=100)
    >>> print(s)        # doctest: +NORMALIZE_WHITESPACE
    category_1  category_2      category_3      category_4      ...     category_7      category_8      category_9      category_10
       23.9010     39.5290         64.3637         92.9089      ...        80.6555         81.9279         18.5807          31.3921
       18.0797     91.1525         53.8317         98.9827      ...        97.7302         49.4779         58.8670          63.9985
       95.3876     83.7604         49.0294         16.8701      ...        27.5901         95.3122         28.9343          72.8989
        7.6053     43.8377         61.9132         52.2341      ...        47.8815         78.9436         40.6214          76.0896
       45.7774     58.1743         25.0168         75.8527      ...        86.2253         62.6258         47.1359          40.2956
       31.2033     80.5664          7.5282         68.5311      ...        71.4174         29.6510          3.0367          19.0434
       26.2464     76.8021          9.1823         87.6912      ...        67.5374         23.6197         70.2259          24.4653
       94.8577     53.6377         16.6878          6.1910      ...        75.2725         81.3743         16.8851          62.1319
       17.1916     15.9427         46.3934         57.2163      ...        92.5156         62.0234          3.3432          71.0502
       86.9788     88.4250         27.3090         13.4801      ...        87.1842         73.6511          8.0223          58.7621
    <BLANKLINE>    
    >>> s = table2str(columns, precision=2)
    >>> print(s)        # doctest: +NORMALIZE_WHITESPACE
    category_1  category_2      category_3      category_4      category_5      category_6      category_7      category_8      category_9      category_10
         23.90       39.53           64.36           92.91           55.99           84.74           80.66           81.93           18.58            31.39
         18.08       91.15           53.83           98.98           29.08           86.74           97.73           49.48           58.87            64.00
         95.39       83.76           49.03           16.87            6.07           14.60           27.59           95.31           28.93            72.90
          7.61       43.84           61.91           52.23           15.85           21.03           47.88           78.94           40.62            76.09
         45.78       58.17           25.02           75.85           66.88           81.49           86.23           62.63           47.14            40.30
         31.20       80.57            7.53           68.53           61.12            0.09           71.42           29.65            3.04            19.04
         26.25       76.80            9.18           87.69           16.18            6.97           67.54           23.62           70.23            24.47
         94.86       53.64           16.69            6.19           49.67           84.89           75.27           81.37           16.89            62.13
         17.19       15.94           46.39           57.22           29.84           82.18           92.52           62.02            3.34            71.05
         86.98       88.43           27.31           13.48           73.40            1.08           87.18           73.65            8.02            58.76
    <BLANKLINE>

    >>> columns = {'names': ['Alice', 'Bob', 'Charlie', 'David', 'Eve', 'Frank', 'Grace', 'Hannah', 'Ivy', 'Jack'],
    ...            'values': ['Tempora amet voluptatem sed modi. Tempora est non est. Voluptatem velit eius modi quaerat. Quaerat quaerat dolorem sed adipisci. Numquam neque sit quiquia. Sit tempora quiquia', 
    ...                       'Numquam porro quiquia est sed labore. Sit neque modi dolor voluptatem. Quisquam amet aliquam quiquia quiquia. Voluptatem dolore dolor', 
    ...                       'Ipsum dolorem quaerat tempora ipsum adipisci quiquia. Magnam sit velit ut ut sit dolorem. Etincidunt sit sed velit tempora. Sit velit ipsum sit modi.', 
    ...                       'Numquam adipisci est tempora tempora porro. Sit numquam porro dolore sit adipisci velit est. Sed magnam adipisci voluptatem', 
    ...                       'Numquam dolore aliquam quaerat non dolore magnam non. Eius est sed aliquam dolore voluptatem consectetur. Modi sed labore amet sit sit', 
    ...                       'Quiquia ut consectetur voluptatem est. Quaerat porro consectetur adipisci est modi dolore. Ipsum modi ipsum', 
    ...                       'Etincidunt numquam non sit etincidunt. Eius labore dolore neque quiquia consectetur dolorem sit. Porro labore adipisci labore ut. Dolor magnam aliquam ipsum ipsum ut eius non.', 
    ...                       'Porro quisquam sit quiquia. Dolor ut sed modi magnam voluptatem quisquam. Consectetur', 
    ...                       'Modi sit amet est etincidunt quisquam. Amet amet quaerat eius adipisci quiquia numquam modi. Labore non labore amet ut. Adipisci voluptate', 
    ...                       'Aliquam magnam quiquia amet amet non est. Velit quaerat tempora etincidunt sit magnam dolore']}
    >>> s = table2str(columns, justify_funcs={"names": JUSTIFY.LEFT, "values": JUSTIFY.LEFT})
    >>> print(s)        # doctest: +NORMALIZE_WHITESPACE
     names                                                     values
    Alice       Tempora amet voluptatem sed modi. Tempora est non est. Voluptatem velit eius modi quaerat. Quaerat
                quaerat dolorem sed adipisci. Numquam neque sit quiquia. Sit tempora quiquia
    Bob         Numquam porro quiquia est sed labore. Sit neque modi dolor voluptatem. Quisquam amet aliquam quiquia
                quiquia. Voluptatem dolore dolor
    Charlie     Ipsum dolorem quaerat tempora ipsum adipisci quiquia. Magnam sit velit ut ut sit dolorem. Etincidunt
                sit sed velit tempora. Sit velit ipsum sit modi.
    David       Numquam adipisci est tempora tempora porro. Sit numquam porro dolore sit adipisci velit est. Sed
                magnam adipisci voluptatem
    Eve         Numquam dolore aliquam quaerat non dolore magnam non. Eius est sed aliquam dolore voluptatem
                consectetur. Modi sed labore amet sit sit
    Frank       Quiquia ut consectetur voluptatem est. Quaerat porro consectetur adipisci est modi dolore. Ipsum
                modi ipsum
    Grace       Etincidunt numquam non sit etincidunt. Eius labore dolore neque quiquia consectetur dolorem sit.
                Porro labore adipisci labore ut. Dolor magnam aliquam ipsum ipsum ut eius non.
    Hannah      Porro quisquam sit quiquia. Dolor ut sed modi magnam voluptatem quisquam. Consectetur
    Ivy         Modi sit amet est etincidunt quisquam. Amet amet quaerat eius adipisci quiquia numquam modi. Labore
                non labore amet ut. Adipisci voluptate
    Jack        Aliquam magnam quiquia amet amet non est. Velit quaerat tempora etincidunt sit magnam dolore
    <BLANKLINE>
    """
    def format_value(value: Any) -> str:
        if isinstance(value, float):
            return f"{value:.{precision}f}" if not is_NA(value) else "na"
        else:
            value = str(value).strip()
            return join_lines(value)

    justify_funcs_ = dict()
    if justify_funcs is not None:
        for key, value in justify_funcs.items():
            if value == JUSTIFY.LEFT:
                justify_funcs_[key] = str.ljust
            elif value == JUSTIFY.CENTER:
                justify_funcs_[key] = str.center
            else:
                justify_funcs_[key] = str.rjust

    cont = '...'

    nb_columns = len(columns)
    column_names = list(columns.keys())

    # check that all columns have the same length
    column_length = len(columns[column_names[0]])
    if not all(len(column) == column_length for column in columns.values()):
        raise ValueError("All columns must have the same length. Got columns of lengths (" + 
                         ', '.join(f"{name}: {len(column)}" for name, column in columns.items()) + ")")
    
    # skip lines ?
    if max_lines != -1 and column_length > max_lines:
        columns_ = dict()
        # replace middle lines of the table by '...'.
        # We show only the first and last edgeitems lines.
        for name, column in columns.items():
            columns_[name] = column[:edgeitems] + [cont] + column[-edgeitems:]
    else:
        columns_ = columns

    # prepare table (rows = columns) and format values
    table = []    
    for name, column in columns_.items():
        column = [format_value(value) for value in column]
        table += [[name] + column]

    # transpose table
    table = [list(row) for row in zip(*table)]

    # need to wrap column ?
    # note: see table2str() from larray project (original algorithm from @gdementen)
    rows = []
    for row in table:
        wrapped_row = [wrap(value, max_width_col) for value in row]
        maxlines = max(len(value) for value in wrapped_row)
        newlines = [[] for _ in range(maxlines)]
        for value in wrapped_row:
            for i in range(maxlines):
                chunk = value[i] if i < len(value) else ''
                newlines[i].append(chunk)
        rows.extend(newlines)
    table = rows
    nb_rows = len(table)

    # center column names and align values
    col_widths = []
    for j in range(nb_columns):
        column_name = table[0][j]
        justify_func = justify_funcs_.get(column_name, str.rjust)
        max_col_length = max(len(table[i][j]) for i in range(nb_rows))
        table[0][j] = column_name.center(max_col_length)
        for i in range(nb_rows):
            table[i][j] = justify_func(table[i][j], max_col_length)
        col_widths += [max_col_length]

    # skip columns ?
    # note: see table2str() from larray project (original algorithm from @gdementen)
    sep_width = len(sep) * (len(columns) - 1)
    if max_width != -1 and sum(col_widths) + sep_width > max_width:
        nb_edges = 0
        max_edges = nb_columns // 2
        if max_edges:
            # width of the "continuation" column (i.e. ellipsis column)
            col_w = len(cont)
            nb_cols = 1
            # iterate like this: first_column --> <-- last_column
            for i in range(1, max_edges + 1):
                col_w += col_widths[i-1] + col_widths[-i]
                # + 1 for the "continuation" column (i.e. ellipsis column)
                nb_cols += 2
                sep_w = len(sep) * (nb_cols - 1)
                if col_w + sep_w > max_width:
                    break
                nb_edges = i
        
        if nb_edges:
            table = [row[:nb_edges] + [cont] + row[-nb_edges:] for row in table]

    # return table as string
    s = ''
    for row in table:
        s += sep.join(row) + '\n'
    return s


def check_file(filepath: str, file_must_exist: bool = False) -> Path:
    """
    This function checks if the parent directory of the 'filepath' exists and 
    returns its absolute path.
    If 'file_must_exist' is True, then the function checks if the 'filepath' exists.
    """
    # check if empty
    if not filepath:
        raise ValueError("Empty filepath.")

    p_filepath = Path(filepath)

    # convert to absolute path
    p_filepath = p_filepath.resolve()

    # check if parent directory exist
    p_directory = p_filepath.parent
    if not p_directory.exists():
        raise ValueError(f"Directory '{p_directory}' "
                         f"in filepath '{p_filepath}' does not exist")

    if file_must_exist and not p_filepath.exists():
        raise FileNotFoundError(f"The file '{p_filepath}' does not exist")

    return p_filepath


def check_file_exists(filepath: str) -> Path:
    """
    This function checks if the 'filepath' exists and returns its absolute path.
    """
    if not filepath:
        raise ValueError("Empty filepath.")
    
    p_filepath = Path(filepath).resolve()
    if not p_filepath.exists():
        raise FileNotFoundError(f"The file '{p_filepath}' does not exist")
    
    return p_filepath


def check_filepath(filepath: str, expected_file_type: IodeFileType, file_must_exist: bool) -> str:
    r"""
    Check the validity of a filepath and its extension. 
    If the filename does not contain an extension, it is added automatically according to 'expected_file_type'.
    If the (modified) filepath is valid, it is returned. 
    Otherwise, an error is raised.

    filepath: str
        The filepath to check. It can be a relative or an absolute path. 
        If it is a relative path, it is considered to be relative to the current working directory.
    expected_file_type: IodeFileType
        The expected file type. If the filepath does not contain an extension, it is added 
        automatically according to this parameter. 
        If the filepath contains an extension, it is checked against this parameter. 
        If the extension is not valid, an error is raised. 
    file_must_exist: bool
        If True, the file must exist.

    Returns
    -------
    str:
    	The checked filepath. If the filepath did not contain an extension, it is added automatically.

    Examples
    --------
    >>> from pathlib import Path
    >>> from iode import IodeFileType, SAMPLE_DATA_DIR
    >>> from iode.util import check_filepath

    >>> # wrong directory
    >>> cwd = Path.cwd()
    >>> fake_dir = cwd.parent / "fake_dir"
	>>> filepath = str(fake_dir / "fun.cmt")
    >>> check_filepath(filepath, IodeFileType.FILE_COMMENTS, False)    # doctest: +ELLIPSIS
    Traceback (most recent call last):
    ...
    ValueError: Directory '...fake_dir' in filepath '...fun.cmt' does not exist
    
	>>> # wrong extension -> expect a COMMENTS file
	>>> filepath = str(Path(SAMPLE_DATA_DIR) / "fun.eqs")
    >>> check_filepath(filepath, IodeFileType.FILE_COMMENTS, False)
    Traceback (most recent call last):
    ...
    ValueError: The file 'fun.eqs' has a wrong extension '.eqs'
    Expected extensions are: ['.cmt', '.ac']
    
    >>> # wrong extension
	>>> filepath = str(Path(SAMPLE_DATA_DIR) / "fun.docx")
    >>> check_filepath(filepath, IodeFileType.FILE_TXT, False)
    Traceback (most recent call last):
    ...
    ValueError: The file 'fun.docx' has a wrong extension '.docx'
    Expected extensions are: ['.txt']

	>>> # file does not exist
	>>> filepath = str(Path(SAMPLE_DATA_DIR) / "funxxx.cmt")
    >>> check_filepath(filepath, IodeFileType.FILE_COMMENTS, True)     # doctest: +ELLIPSIS
    Traceback (most recent call last):
    ...
    FileNotFoundError: The file '...funxxx.cmt' does not exist

	>>> # file does not exist (no extension given)
	>>> filepath = str(Path(SAMPLE_DATA_DIR) / "funxxx")
	>>> check_filepath(filepath, IodeFileType.FILE_COMMENTS, True)     # doctest: +SKIP

    Traceback (most recent call last):
    ...
    FileNotFoundError: Neither 'funxxx.cmt' nor 'funxxx.ac' could be found in directory '...'

	>>> # No extension but not an IODE objects file
	>>> filepath = str(Path(SAMPLE_DATA_DIR) / "fun")
    >>> check_filepath(filepath, IodeFileType.FILE_TXT, True)          # doctest: +ELLIPSIS
    Traceback (most recent call last):
    ...
    ValueError: You must provide an extension to the file '...fun'

	>>> # extension added automatically
	>>> filepath = str(Path(SAMPLE_DATA_DIR) / "fun")
    >>> checked_filepath = check_filepath(filepath, IodeFileType.FILE_COMMENTS, True)
    >>> Path(checked_filepath).name
    'fun.cmt'
    >>> Path(checked_filepath).parent.name
    'data'
    """
    p_filepath: Path = check_file(filepath, False)

    # check or add extension
    if p_filepath.suffix:
        # check extension
        ext = p_filepath.suffix
        expected_extensions = IODE_FILE_TYPES[int(expected_file_type)].extensions
        if ext not in expected_extensions:
            raise ValueError(f"The file '{p_filepath.name}' has a wrong extension '{ext}'\n"
                             f"Expected extensions are: {expected_extensions}")

        # check if file exist
        if file_must_exist and not p_filepath.exists():
            raise FileNotFoundError(f"The file '{p_filepath}' does not exist")
    else:
        if expected_file_type > IodeFileType.FILE_VARIABLES:
            raise ValueError(f"You must provide an extension to the file '{p_filepath}'")
        database_type: IodeType = IodeType(expected_file_type)

        # set binary format extension
        binary_ext = IODE_FILE_TYPES[int(database_type)].extensions[0]
        p_filepath = p_filepath.with_suffix(binary_ext)

        # check if file exist
        if file_must_exist:
            # check first with binary format extension
            if not p_filepath.exists():
                # switch to ascii format extension and check if file exist
                ascii_ext = IODE_FILE_TYPES[int(database_type)].extensions[1]
                p_filepath = p_filepath.with_suffix(ascii_ext)
                if not p_filepath.exists():
                    p_directory = p_filepath.parent
                    stem = p_filepath.stem
                    raise FileNotFoundError(f"Neither '{stem}{binary_ext}' nor '{stem}{ascii_ext}' "
                                            f"could be found in directory '{p_directory}'")

    return str(p_filepath)


def get_iode_file_type(filepath: str) -> IodeFileType:
    r"""
    Return the IODE file type of a filepath. The filepath can be a relative or an absolute path.

    Parameters
    ----------
    filepath: str
       The filepath to check. It can be a relative or an absolute path.

    Returns
    -------
    IodeFileType
      The IODE file type of the filepath.

    Examples
    --------
    >>> from pathlib import Path
    >>> from iode import IodeFileType, SAMPLE_DATA_DIR
    >>> from iode.util import get_iode_file_type

    >>> filename = ""
    >>> get_iode_file_type(filename)
    <IodeFileType.FILE_ANY: 32>
    >>> filename = "ws"
    >>> get_iode_file_type(filename)
    <IodeFileType.FILE_ANY: 32>

    >>> get_iode_file_type(SAMPLE_DATA_DIR)
    <IodeFileType.DIRECTORY: 33>

    >>> filename = "fun.cmt"
    >>> get_iode_file_type(filename)
    <IodeFileType.FILE_COMMENTS: 0>
    >>> filename = "fun.ac"
    >>> get_iode_file_type(filename)
    <IodeFileType.FILE_COMMENTS: 0>

    >>> filename = "fun.eqs"
    >>> get_iode_file_type(filename)
    <IodeFileType.FILE_EQUATIONS: 1>
    >>> filename = "fun.ae"
    >>> get_iode_file_type(filename)
    <IodeFileType.FILE_EQUATIONS: 1>

    >>> filename = "fun.idt"
    >>> get_iode_file_type(filename)
    <IodeFileType.FILE_IDENTITIES: 2>
    >>> filename = "fun.ai"
    >>> get_iode_file_type(filename)
    <IodeFileType.FILE_IDENTITIES: 2>

    >>> filename = "fun.lst"
    >>> get_iode_file_type(filename)
    <IodeFileType.FILE_LISTS: 3>
    >>> filename = "fun.al"
    >>> get_iode_file_type(filename)
    <IodeFileType.FILE_LISTS: 3>

    >>> filename = "fun.scl"
    >>> get_iode_file_type(filename)
    <IodeFileType.FILE_SCALARS: 4>
    >>> filename = "fun.as"
    >>> get_iode_file_type(filename)
    <IodeFileType.FILE_SCALARS: 4>

    >>> filename = "fun.tbl"
    >>> get_iode_file_type(filename)
    <IodeFileType.FILE_TABLES: 5>
    >>> filename = "fun.at"
    >>> get_iode_file_type(filename)
    <IodeFileType.FILE_TABLES: 5>

    >>> filename = "fun.var"
    >>> get_iode_file_type(filename)
    <IodeFileType.FILE_VARIABLES: 6>
    >>> filename = "fun.av"
    >>> get_iode_file_type(filename)
    <IodeFileType.FILE_VARIABLES: 6>

    >>> filename = "fun.rep"
    >>> get_iode_file_type(filename)
    <IodeFileType.FILE_REP: 16>
    
    >>> filename = "fun.log"
    >>> get_iode_file_type(filename)
    <IodeFileType.FILE_LOG: 30>

    >>> filename = "fun.ini"
    >>> get_iode_file_type(filename)
    <IodeFileType.FILE_SETTINGS: 31>

    >>> filename = "fun.txt"
    >>> get_iode_file_type(filename)
    <IodeFileType.FILE_TXT: 25>

    >>> filename = "fun.a2m"
    >>> get_iode_file_type(filename)
    <IodeFileType.FILE_A2M: 17>

    >>> filename = "fun.agl"
    >>> get_iode_file_type(filename)
    <IodeFileType.FILE_AGL: 18>

    >>> filename = "fun.prf"
    >>> get_iode_file_type(filename)
    <IodeFileType.FILE_PRF: 19>

    >>> filename = "fun.dif"
    >>> get_iode_file_type(filename)
    <IodeFileType.FILE_DIF: 20>

    >>> filename = "fun.mif"
    >>> get_iode_file_type(filename)
    <IodeFileType.FILE_MIF: 21>

    >>> filename = "fun.rtf"
    >>> get_iode_file_type(filename)
    <IodeFileType.FILE_RTF: 22>

    >>> filename = "fun.asc"
    >>> get_iode_file_type(filename)
    <IodeFileType.FILE_AAS: 24>

    >>> filename = "fun.ref"
    >>> get_iode_file_type(filename)
    <IodeFileType.FILE_REF: 29>
    """
    if not filepath:
        return IodeFileType.FILE_ANY

    p_filepath = Path(filepath)

    if p_filepath.is_dir():
        return IodeFileType.DIRECTORY

    if not p_filepath.suffix:
        return IodeFileType.FILE_ANY

    ext = p_filepath.suffix

    for i, file_type in enumerate(IODE_FILE_TYPES):
        if ext in file_type.extensions:
            return IodeFileType(i)

    return IodeFileType.FILE_ANY
