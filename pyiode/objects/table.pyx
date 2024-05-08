# distutils: language = c++

import warnings
from collections.abc import Iterable
from typing import Union, Tuple, List, Dict, Optional

from libcpp.string cimport string
from cython.operator cimport dereference
from pyiode.common cimport (EnumCellType, EnumCellAlign, EnumCellFont, EnumLineType, 
                            EnumGraphAlign, EnumGraphAxis, EnumGraphGrid, EnumGraphType)
from pyiode.objects.table cimport CTableCell, CTableLine, CTable
from pyiode.objects.table cimport hash_value as hash_value_tbl
from pyiode.iode_database.database cimport load_reference_kdb


# TableCell wrapper class
# see https://cython.readthedocs.io/en/latest/src/userguide/wrapping_CPlusPlus.html#create-cython-wrapper-class 
# see also https://cython.readthedocs.io/en/stable/src/userguide/extension_types.html#instantiation-from-existing-c-c-pointers 
cdef class TableCell:
    """
    IODE Table cell.

    Attributes
    ----------
    cell_type: int
        Type of cell content. Possible value is 'STRING' or 'LEC'.
    align: int
        Alignment of the text in the cell. Possible values are 'center', 'left', 'right', 'decimal' or 
        CELL_ALIGN_CENTER, CELL_ALIGN_LEFT, CELL_ALIGN_RIGHT, CELL_ALIGN_DECIMAL.
    bold: bool
    italic: bool
    underline: bool

    Examples
    --------
    >>> from iode import SAMPLE_DATA_DIR
    >>> from iode import tables, Table
    >>> tables.load(f"{SAMPLE_DATA_DIR}/fun.tbl")
    >>> table = tables["ANAPRIX"]
    >>> table           # doctest: +NORMALIZE_WHITESPACE
    DIVIS | 1                            |
    TITLE |                        "Analyse des prix"
    ----- | ---------------------------------------------------------------
    CELL  | ""                           |               "#s"
    ----- | ---------------------------------------------------------------
    CELL  | "GAP_"                       |                             GAP_
    CELL  | "dln (PC/(1+ITCR))-dln AOUC" | 100*(dln (PC/(1+ITCR))-dln AOUC)
    <BLANKLINE>
    nb lines: 6
    nb columns: 2
    language: 'English'
    gridx: 'major'
    gridy: 'major'
    graph_axis: 'values'
    graph_alignment: 'left'
    <BLANKLINE>

    >>> table[4]
    ('"GAP_"', 'GAP_')
    >>> # first cell
    >>> table[4][0]
    "GAP_"
    >>> table[4][0].cell_type
    'STRING'
    >>> # second cell
    >>> table[4][1]
    GAP_
    >>> table[4][1].cell_type
    'CELL'
    """
    cdef CTableCell* c_cell
    cdef int nb_columns

    def __init__(self):
        # Prevent accidental instantiation from normal Python code
        # since we cannot pass a struct pointer into a Python constructor.
        raise TypeError("This class cannot be instantiated directly.")

    def __cinit__(self):
        self.c_cell = NULL
        self.nb_columns = 0

    def __dealloc__(self):
        pass

    # see https://cython.readthedocs.io/en/stable/src/userguide/extension_types.html#instantiation-from-existing-c-c-pointers 
    @staticmethod
    cdef TableCell from_ptr(CTableCell* c_cell_ptr, int nb_columns):
        """
        Factory function to create TableCell objects from given CTableCell pointer.
        """
        # Fast call to __new__() that bypasses the __init__() constructor.
        cdef TableCell wrapper = TableCell.__new__(TableCell)
        wrapper.c_cell = c_cell_ptr
        wrapper.nb_columns = nb_columns
        return wrapper

    @property
    def cell_type(self) -> str:
        """
        Type of the content of the cell. Possible value is 'STRING' or 'LEC'.

        Examples
        --------
        >>> from iode import SAMPLE_DATA_DIR
        >>> from iode import tables, Table
        >>> tables.load(f"{SAMPLE_DATA_DIR}/fun.tbl")
        >>> table = tables["ANAPRIX"]
        >>> table[4]
        ('"GAP_"', 'GAP_')
        >>> # first cell
        >>> table[4][0]
        "GAP_"
        >>> table[4][0].cell_type
        'STRING'
        >>> # second cell
        >>> table[4][1]
        GAP_
        >>> table[4][1].cell_type
        'LEC'
        """
        return CELL_TYPE_DICT[<int>(self.c_cell.get_type())] if self.c_cell is not NULL else None

    @property
    def align(self) -> str:
        """
        Alignment of the text in the cell.

        Parameters
        ----------
        align : str or int
            The alignment to set for the cell.
            Possible values are 'center', 'left', 'right', 'decimal' or 
            CELL_ALIGN_CENTER, CELL_ALIGN_LEFT, CELL_ALIGN_RIGHT, CELL_ALIGN_DECIMAL.

        Examples
        --------
        >>> from iode import SAMPLE_DATA_DIR, CELL_ALIGN_CENTER, CELL_ALIGN_LEFT, CELL_ALIGN_RIGHT, CELL_ALIGN_DECIMAL
        >>> from iode import tables, Table
        >>> tables.load(f"{SAMPLE_DATA_DIR}/fun.tbl")
        >>> table = tables["ANAPRIX"]
        >>> table[4][0].align
        'left'
        >>> table[4][0].align = 'center'
        >>> table[4][0].align
        'center'
        >>> table[4][0].align = CELL_ALIGN_RIGHT
        >>> table[4][0].align
        'right'
        """
        return CELL_ALIGN_DICT[<int>self.c_cell.get_align()] if self.c_cell is not NULL else None

    @align.setter
    def align(self, value: Union[int, str]):
        if not isinstance(value, (int, str)):
            raise TypeError(f"Expected value of type int or str. Got value of type {type(value).__name__} instead.")
        if isinstance(value, str):
            value = value.lower()
            value = CELL_ALIGN_REV_DICT[value] if value in CELL_ALIGN_REV_DICT else -1
        if value not in CELL_ALIGN_DICT:
            raise ValueError(f"Possible values are 'center', 'left', 'right', 'decimal' or CELL_ALIGN_CENTER, "
                             f"CELL_ALIGN_LEFT, CELL_ALIGN_RIGHT, CELL_ALIGN_DECIMAL.\nGot value {value} instead.") 
        self.c_cell.set_align(<EnumCellAlign>value)

    @property
    def bold(self) -> bool:
        """
        Whether or not the cell text is bold.

        Parameters
        ----------
        value : bool
            True to set the cell as bold, False otherwise.

        Examples
        --------
        >>> from iode import SAMPLE_DATA_DIR
        >>> from iode import tables, Table
        >>> tables.load(f"{SAMPLE_DATA_DIR}/fun.tbl")
        >>> table = tables["ANAPRIX"]
        >>> table[4][0].bold
        False
        >>> table[4][0].bold = True
        >>> table[4][0].bold
        True
        """
        return self.c_cell.is_bold() if self.c_cell is not NULL else None

    @bold.setter
    def bold(self, value: bool):
        if self.c_cell is not NULL:
            self.c_cell.set_bold(<bint>value)

    @property
    def italic(self) -> bool:
        """
        Whether or not the cell text is italic.

        Parameters
        ----------
        value : bool
            True to set the cell as italic, False otherwise.

        Examples
        --------
        >>> from iode import SAMPLE_DATA_DIR
        >>> from iode import tables, Table
        >>> tables.load(f"{SAMPLE_DATA_DIR}/fun.tbl")
        >>> table = tables["ANAPRIX"]
        >>> table[4][0].italic
        False
        >>> table[4][0].italic = True
        >>> table[4][0].italic
        True
        """
        return self.c_cell.is_italic() if self.c_cell is not NULL else None

    @italic.setter
    def italic(self, value: bool):
        if self.c_cell is not NULL:
            self.c_cell.set_italic(<bint>value)

    @property
    def underline(self) -> bool:
        """
        Whether or not the cell text is underline.

        Parameters
        ----------
        value : bool
            True to set the cell as underlined, False otherwise.

        Examples
        --------
        >>> from iode import SAMPLE_DATA_DIR
        >>> from iode import tables, Table
        >>> tables.load(f"{SAMPLE_DATA_DIR}/fun.tbl")
        >>> table = tables["ANAPRIX"]
        >>> table[4][0].underline
        False
        >>> table[4][0].underline = True
        >>> table[4][0].underline
        True
        """
        return self.c_cell.is_underline() if self.c_cell is not NULL else None

    @underline.setter
    def underline(self, value: bool):
        if self.c_cell is not NULL:
            self.c_cell.set_underline(<bint>value)

    def get_variables_from_lec(self) -> List[str]:
        """
        Get the list of variables associated with the LEC expression of a cell of type 'LEC'.

        Returns
        -------
        list(str)

        Examples
        --------
        >>> from iode import SAMPLE_DATA_DIR
        >>> from iode import variables, tables
        >>> variables.load(f"{SAMPLE_DATA_DIR}/fun.var")
        >>> tables.load(f"{SAMPLE_DATA_DIR}/fun.tbl")
        >>> table = tables["ANAPRIX"]    
        >>> table[5]
        ('"dln (PC/(1+ITCR))-dln AOUC"', '100*(dln (PC/(1+ITCR))-dln AOUC)')
        >>> table[5][1].get_variables_from_lec()
        ['PC', 'ITCR', 'AOUC']
        """      
        if self.c_cell is NULL:
            return
        if self.cell_type != 'LEC':
            warnings.warn("Cannot get list of variables from a cell which is not of type 'LEC'")
            return [] 
        return [item.decode() for item in self.c_cell.get_variables_from_lec()]

    def __str__(self) -> str:
        if self.c_cell is NULL:
            return ''
        quotes = <int>(self.c_cell.get_type()) == CELL_TYPE_STRING
        content = self.c_cell.get_content(<bint>quotes).decode()
        # remove newline characters
        return ''.join(content.splitlines())
    
    def __repr__(self) -> str:
        return self.__str__()


# TableLine wrapper class
# see https://cython.readthedocs.io/en/latest/src/userguide/wrapping_CPlusPlus.html#create-cython-wrapper-class 
# see also https://cython.readthedocs.io/en/stable/src/userguide/extension_types.html#instantiation-from-existing-c-c-pointers 
cdef class TableLine:
    """
    IODE Table line.

    Attributes
    ----------
    line_type: int
        Type of the table line. Possible values are 'TITLE', 'CELL', 'LINE', 'MODE', 'FILES', 'DATE' or 
        LINE_TYPE_TITLE, LINE_TYPE_CELL, LINE_TYPE_LINE, LINE_TYPE_MODE, LINE_TYPE_FILES, LINE_TYPE_DATE.

    graph_type: int
        Graph type associated with the table line. Possible values are {', '.join(GRAPH_TYPE_DICT.values())} 
        or GRAPH_TYPE_LINE, GRAPH_TYPE_SCATTER, GRAPH_TYPE_BAR.

    axis_left: bool
        Whether or not the values of the present line correspond to the Y axis 
        displayed on the left of the graph. Defaults to True.

    Examples
    --------
    >>> from iode import SAMPLE_DATA_DIR
    >>> from iode import tables, Table
    >>> tables.load(f"{SAMPLE_DATA_DIR}/fun.tbl")
    >>> table = tables["ANAPRIX"]
    >>> table           # doctest: +NORMALIZE_WHITESPACE
    DIVIS | 1                            |
    TITLE |                        "Analyse des prix"
    ----- | ---------------------------------------------------------------
    CELL  | ""                           |               "#s"
    ----- | ---------------------------------------------------------------
    CELL  | "GAP_"                       |                             GAP_
    CELL  | "dln (PC/(1+ITCR))-dln AOUC" | 100*(dln (PC/(1+ITCR))-dln AOUC)
    <BLANKLINE>
    nb lines: 6
    nb columns: 2
    language: 'English'
    gridx: 'major'
    gridy: 'major'
    graph_axis: 'values'
    graph_alignment: 'left'
    <BLANKLINE>

    >>> table[0]
    Table example
    >>> table[0].line_type
    'TITLE'
    >>> table[0].graph_type
    'line'
    >>> table[0].axis_left
    True
    """
    cdef CTableLine* c_line
    cdef int nb_columns

    def __init__(self):
        # Prevent accidental instantiation from normal Python code
        # since we cannot pass a struct pointer into a Python constructor.
        raise TypeError("This class cannot be instantiated directly.")

    def __cinit__(self):
        self.c_line = NULL
        self.nb_columns = 0

    def __dealloc__(self):
        pass

    # see https://cython.readthedocs.io/en/stable/src/userguide/extension_types.html#instantiation-from-existing-c-c-pointers 
    @staticmethod
    cdef TableLine from_ptr(CTableLine* c_line_ptr, int nb_columns):
        """
        Factory function to create TableLine objects from given CTableLine pointer.
        """
        # Fast call to __new__() that bypasses the __init__() constructor.
        cdef TableLine wrapper = TableLine.__new__(TableLine)
        wrapper.c_line = c_line_ptr
        wrapper.nb_columns = nb_columns
        return wrapper

    @property
    def line_type(self) -> str:
        """
        Type of the table line

        Returns
        -------
        str

        Examples
        --------
        >>> from iode import SAMPLE_DATA_DIR
        >>> from iode import tables, Table
        >>> tables.load(f"{SAMPLE_DATA_DIR}/fun.tbl")
        >>> table = tables["ANAPRIX"]
        >>> table           # doctest: +NORMALIZE_WHITESPACE
        DIVIS | 1                            |
        TITLE |                        "Analyse des prix"
        ----- | ---------------------------------------------------------------
        CELL  | ""                           |               "#s"
        ----- | ---------------------------------------------------------------
        CELL  | "GAP_"                       |                             GAP_
        CELL  | "dln (PC/(1+ITCR))-dln AOUC" | 100*(dln (PC/(1+ITCR))-dln AOUC)
        <BLANKLINE>
        nb lines: 6
        nb columns: 2
        language: 'English'
        gridx: 'major'
        gridy: 'major'
        graph_axis: 'values'
        graph_alignment: 'left'
        <BLANKLINE>

        >>> table[0]
        Analyse des prix
        >>> table[0].line_type
        'TITLE'

        >>> table[4]
        ('"GAP_"', 'GAP_')
        >>> table[4].line_type
        'CELL'
        """
        return LINE_TYPE_DICT[<int>(self.c_line.get_line_type())] if self.c_line is not NULL else None
    
    @property
    def graph_type(self) -> str:
        """
        Graph type associated with the table line

        Parameters
        ----------
        value: int
            Possible values are 'line', 'scatter', 'bar' or  
            GRAPH_TYPE_LINE, GRAPH_TYPE_SCATTER, GRAPH_TYPE_BAR.

        Examples
        --------
        >>> from iode import SAMPLE_DATA_DIR, GRAPH_TYPE_LINE, GRAPH_TYPE_SCATTER, GRAPH_TYPE_BAR
        >>> from iode import tables, Table
        >>> tables.load(f"{SAMPLE_DATA_DIR}/fun.tbl")
        >>> table = tables["ANAPRIX"]
        >>> table           # doctest: +NORMALIZE_WHITESPACE
        DIVIS | 1                            |
        TITLE |                        "Analyse des prix"
        ----- | ---------------------------------------------------------------
        CELL  | ""                           |               "#s"
        ----- | ---------------------------------------------------------------
        CELL  | "GAP_"                       |                             GAP_
        CELL  | "dln (PC/(1+ITCR))-dln AOUC" | 100*(dln (PC/(1+ITCR))-dln AOUC)
        <BLANKLINE>
        nb lines: 6
        nb columns: 2
        language: 'English'
        gridx: 'major'
        gridy: 'major'
        graph_axis: 'values'
        graph_alignment: 'left'
        <BLANKLINE>

        >>> table[4]
        ('"GAP_"', 'GAP_')
        >>> table[4].graph_type
        'line'
        >>> table[4].graph_type = "scatter"
        >>> table[4].graph_type
        'scatter'
        >>> table[4].graph_type = GRAPH_TYPE_BAR
        >>> table[4].graph_type
        'bar'
        """
        return GRAPH_TYPE_DICT[<int>(self.c_line.get_line_graph())] if self.c_line is not NULL else None

    @graph_type.setter
    def graph_type(self, value: Union[int, str]):
        if self.c_line is NULL:
            return
        if not isinstance(value, (int, str)):
            raise TypeError(f"Expected value of type int or str. Got value of type {type(value).__name__} instead")
        if isinstance(value, str):
            value = value.lower()
            value = GRAPH_TYPE_REV_DICT[value] if value in GRAPH_TYPE_REV_DICT else -1
        if value not in GRAPH_TYPE_DICT:
            raise ValueError(f"Possible values are {GRAPH_TYPE_DICT.values()} or [GRAPH_TYPE_LINE, "
                             f"GRAPH_TYPE_SCATTER, GRAPH_TYPE_BAR].\nGot value {value} instead.")
        self.c_line.set_line_graph(<EnumGraphType>value)

    @property
    def axis_left(self) -> bool:
        """
        Whether or not the values of the present line correspond to the Y axis displayed on the left of the graph.

        Parameters
        ----------
        value: bool

        Examples
        --------
        >>> from iode import SAMPLE_DATA_DIR
        >>> from iode import tables, Table
        >>> tables.load(f"{SAMPLE_DATA_DIR}/fun.tbl")
        >>> table = tables["ANAPRIX"]
        >>> table           # doctest: +NORMALIZE_WHITESPACE
        DIVIS | 1                            |
        TITLE |                        "Analyse des prix"
        ----- | ---------------------------------------------------------------
        CELL  | ""                           |               "#s"
        ----- | ---------------------------------------------------------------
        CELL  | "GAP_"                       |                             GAP_
        CELL  | "dln (PC/(1+ITCR))-dln AOUC" | 100*(dln (PC/(1+ITCR))-dln AOUC)
        <BLANKLINE>
        nb lines: 6
        nb columns: 2
        language: 'English'
        gridx: 'major'
        gridy: 'major'
        graph_axis: 'values'
        graph_alignment: 'left'
        <BLANKLINE>

        >>> table[4]
        ('"GAP_"', 'GAP_')
        >>> table[4].axis_left
        True
        >>> table[4].axis_left = False
        >>> table[4].axis_left
        False
        """
        return self.c_line.is_left_axis() if self.c_line is not NULL else None

    @axis_left.setter
    def axis_left(self, value: bool):
        if self.c_line is NULL:
            return
        if not isinstance(value, bool):
            raise TypeError(f"Expected value of type bool. Got value of type {type(value).__name__} instead")
        self.c_line.set_line_axis(<bint>value)

    def __len__(self) -> int:
        return self.nb_columns

    def _get_row_from_index(self, index: int) -> int:
        if not isinstance(index, int):
            raise TypeError(f"The index of the cell must be of type int. "
                            f"Got an index value of type {type(index).__name__} instead.")
        if not (-self.nb_columns < index < self.nb_columns):
            raise ValueError(f"The index of the cell must be in range [{-self.nb_columns + 1, self.nb_columns - 1}].\n"
                             f"Got value {index} instead.")
        if index < 0:
            index = self.nb_columns + index
        return index

    def __getitem__(self, index) -> TableCell:
        """
        Get the cell of a line.

        Parameters
        ----------
        index: int
            index of the cell.

        Returns
        -------
        TableCell

        Examples
        --------
        >>> from iode import SAMPLE_DATA_DIR
        >>> from iode import tables, Table
        >>> tables.load(f"{SAMPLE_DATA_DIR}/fun.tbl")
        >>> table = tables["ANAPRIX"]
        >>> table           # doctest: +NORMALIZE_WHITESPACE
        DIVIS | 1                            |
        TITLE |                        "Analyse des prix"
        ----- | ---------------------------------------------------------------
        CELL  | ""                           |               "#s"
        ----- | ---------------------------------------------------------------
        CELL  | "GAP_"                       |                             GAP_
        CELL  | "dln (PC/(1+ITCR))-dln AOUC" | 100*(dln (PC/(1+ITCR))-dln AOUC)
        <BLANKLINE>
        nb lines: 6
        nb columns: 2
        language: 'English'
        gridx: 'major'
        gridy: 'major'
        graph_axis: 'values'
        graph_alignment: 'left'
        <BLANKLINE>

        >>> table[4][0]
        "GAP_"
        >>> table[4][1]
        GAP_
        """
        cdef CTableCell* c_cell

        if self.c_line is NULL:
            return

        row = self._get_row_from_index(index)
        c_cell = self.c_line.get_cell(row, self.nb_columns)
        return TableCell.from_ptr(c_cell, self.nb_columns)

    def __setitem__(self, index, value):
        r"""
        Update the content of a cell of the line.

        Parameters
        ----------
        index: int
            index of the cell.
        value: str or TableCell
            new content of the cell.

        Examples
        --------
        >>> from iode import SAMPLE_DATA_DIR
        >>> from iode import tables, Table
        >>> tables.load(f"{SAMPLE_DATA_DIR}/fun.tbl")
        >>> table = tables["ANAPRIX"]
        >>> table           # doctest: +NORMALIZE_WHITESPACE
        DIVIS | 1                            |
        TITLE |                        "Analyse des prix"
        ----- | ---------------------------------------------------------------
        CELL  | ""                           |               "#s"
        ----- | ---------------------------------------------------------------
        CELL  | "GAP_"                       |                             GAP_
        CELL  | "dln (PC/(1+ITCR))-dln AOUC" | 100*(dln (PC/(1+ITCR))-dln AOUC)
        <BLANKLINE>
        nb lines: 6
        nb columns: 2
        language: 'English'
        gridx: 'major'
        gridy: 'major'
        graph_axis: 'values'
        graph_alignment: 'left'
        <BLANKLINE>

        >>> # warning: content of cells of type 'STRING' must be surrounded by double quotes
        >>> table[5][0] = '"dln(PC/ITCR) - dln AOUC"'
        >>> table[5][0]
        "dln(PC/ITCR) - dln AOUC"
        >>> # warning: if the new content is not surrounded by double quotes, 
        >>> #          it is considerer as 'LEC' cell
        >>> table[5][1] = '100*(dln(PC / ITCR) - dln AOUC)'
        >>> table[5][1]
        100*(dln(PC / ITCR) - dln AOUC)
        """
        if self.c_line is NULL:
            return
        
        row = self._get_row_from_index(index)
        if isinstance(value, TableCell):
            value = str(value)
        if not isinstance(value, str):
            raise TypeError(f"Expected value of type str or TableCell. Got value of type {type(value).__name__} instead.")
        c_cell = self.c_line.get_cell(row, self.nb_columns)
        c_cell.set_content(value.encode())
        
    def __delitem__(self, index):
        raise RuntimeError("A Table cell cannot be deleted")

    def __str__(self) -> str:
        cdef CTableCell* c_cell

        if self.c_line is NULL:
            return ''

        line_type = <int>(self.c_line.get_line_type())
        nb_columns = self.nb_columns
        if line_type == LINE_TYPE_TITLE:
            content = self.c_line.get_cell(0, nb_columns).get_content(<bint>False).decode()
            # remove newline characters
            return ''.join(content.splitlines())
        elif line_type == LINE_TYPE_CELL:
            cells_content = []
            for j in range(nb_columns):
                c_cell = self.c_line.get_cell(j, nb_columns)
                quotes = <int>(c_cell.get_type()) == CELL_TYPE_STRING
                content = c_cell.get_content(<bint>quotes).decode()
                # remove newline characters
                cells_content += [''.join(content.splitlines())]
            return str(tuple(cells_content))
        elif line_type == LINE_TYPE_LINE:
            return '---'
        else:
            return f"<{self.line_type}>"

    def __repr__(self) -> str:
        return self.__str__()


# Table wrapper class
# see https://cython.readthedocs.io/en/latest/src/userguide/wrapping_CPlusPlus.html#create-cython-wrapper-class 
cdef class Table:
    """
    IODE Table.

    Parameters
    ----------
    nb_columns : int
        The number of columns in the IODE table.
        Defaults to 2.
    table_title : str, optional
        The title of the table or the name of the IODE Comment that must become the title.
        Defaults to empty.
    lecs_or_vars : str or List[str], optional
        The LEC formulas of the lines or the list of variable names.
        If it represents a list of variables, the associated line title for each variable 
        will be the IODE comment with the same name if it exists, the name of the variable otherwise.
    lines_titles: list(str), optional
        Titles of the lines. If None, the arguments 'lecs_or_vars' is assumed to represents 
        a list of variables.
    mode : bool, optional
        Whether or not to add a 'MODE' line to the table. 
        Defaults to False.
    files : bool, optional
        Whether or not to add a 'FILES' line to the table. 
        This line contains the files used to display the values of the computed table.
        Defaults to False.
    date : bool, optional
        Whether or not to add a 'DATE' line to the table.
        This line contains the date when the table has been computed.
        Defaults to False.

    Returns
    -------
    Table

    Examples
    --------
    >>> from iode import SAMPLE_DATA_DIR
    >>> from iode import Table, comments, lists, variables
    >>> comments.load(f"{SAMPLE_DATA_DIR}/fun.cmt")
    >>> lists.load(f"{SAMPLE_DATA_DIR}/fun.lst")
    >>> variables.load(f"{SAMPLE_DATA_DIR}/fun.var")

    >>> # empty table
    >>> table = Table()
    >>> table           # doctest: +NORMALIZE_WHITESPACE
    DIVIS | 1 |
    TITLE |
    ----- | --------
    CELL  |   | "#S"
    ----- | --------
    <BLANKLINE>
    nb lines: 4
    nb columns: 2
    language: 'English'
    gridx: 'major'
    gridy: 'major'
    graph_axis: 'values'
    graph_alignment: 'left'
    <BLANKLINE>

    >>> # the first cell represents a line title and the second a LEC expression
    >>> table_title = "Table example with titles on the left and LEC expressions on the right"
    >>> lines_titles = ["GOSG:", "YDTG:", "DTH:", "DTF:", "IT:", "YSSG+COTRES:", "RIDG:", "OCUG:"]
    >>> lines_lecs = ["GOSG", "YDTG", "DTH", "DTF", "IT", "YSSG+COTRES", "RIDG", "OCUG"]
    >>> table = Table(2, table_title, lines_lecs, lines_titles, True, True, True)
    >>> table           # doctest: +NORMALIZE_WHITESPACE
    DIVIS | 1              |
    TITLE | "Table example with titles on the left and LEC expressions on the right"
    ----- | ------------------------------------------------------------------------
    CELL  |                |     "#S"
    ----- | ------------------------------------------------------------------------
    CELL  | "GOSG:"        |        GOSG
    CELL  | "YDTG:"        |        YDTG
    CELL  | "DTH:"         |         DTH
    CELL  | "DTF:"         |         DTF
    CELL  | "IT:"          |          IT
    CELL  | "YSSG+COTRES:" | YSSG+COTRES
    CELL  | "RIDG:"        |        RIDG
    CELL  | "OCUG:"        |        OCUG
    ----- | ------------------------------------------------------------------------
    MODE  |
    FILES |
    DATE  |
    <BLANKLINE>
    nb lines: 16
    nb columns: 2
    language: 'English'
    gridx: 'major'
    gridy: 'major'
    graph_axis: 'values'
    graph_alignment: 'left'
    <BLANKLINE>

    >>> # if the argument 'lines_titles' is not used, the argument 'lecs_or_vars' represents 
    >>> # a list of variables.
    >>> table_title = "Table example with variables only"
    >>> # the element '$ENVI' of 'lines_vars' below represents the IODE list 'ENVI' and will 
    >>> # be automatically expanded to its contained variables when passed to the Table constructor 
    >>> lines_vars = ["GOSG", "YDTG", "DTH", "DTF", "IT", "YSSG", "COTRES", "RIDG", "OCUG", "$ENVI"]
    >>> table = Table(2, table_title, lines_vars, mode=True, files=True, date=True)
    >>> table           # doctest: +NORMALIZE_WHITESPACE
    DIVIS | 1                                                                    |
    TITLE |                      "Table example with variables only"
    ----- | ------------------------------------------------------------------------------
    CELL  |                                                                      |   "#S"
    ----- | ------------------------------------------------------------------------------
    CELL  | "Bruto exploitatie-overschot: overheid (= afschrijvingen)."          |    GOSG
    CELL  | "Overheid: geïnde indirecte belastingen."                            |    YDTG
    CELL  | "Totale overheid: directe belasting van de gezinnen."                |     DTH
    CELL  | "Totale overheid: directe vennootschapsbelasting."                   |     DTF
    CELL  | "Totale indirecte belastingen."                                      |      IT
    CELL  | "Globale overheid: ontvangen sociale zekerheidsbijdragen."           |    YSSG
    CELL  | "Cotisation de responsabilisation."                                  |  COTRES
    CELL  | "Overheid: inkomen uit vermogen."                                    |    RIDG
    CELL  | "Globale overheid: saldo van de ontvangen lopendeoverdrachten."      |    OCUG
    CELL  | "Wisselkoers van de USD t.o.v. de BEF (jaargemiddelde)."             |      EX
    CELL  | "Index wereldprijs - invoer van niet-energieprodukten, inUSD."       |   PWMAB
    CELL  | "Index wereldprijs - invoer van diensten, in USD."                   |    PWMS
    CELL  | "Index wereldprijs - uitvoer van niet-energieprodukten, inUSD."      |   PWXAB
    CELL  | "Index wereldprijs - uitvoer van diensten, in USD."                  |    PWXS
    CELL  | "Indicator van het volume van de wereldvraag naar goederen,1985=1."  |   QWXAB
    CELL  | "Indicator van het volume van de wereldvraag naar diensten,1985=1."  |    QWXS
    CELL  | "Brent olieprijs (USD per barrel)."                                  |    POIL
    CELL  | "Totale beroepsbevolking (jaargemiddelde)."                          |    NATY
    CELL  | "TFPFHP_"                                                            | TFPFHP_
    ----- | ------------------------------------------------------------------------------
    MODE  |
    FILES |
    DATE  |
    <BLANKLINE>
    nb lines: 27
    nb columns: 2
    language: 'English'
    gridx: 'major'
    gridy: 'major'
    graph_axis: 'values'
    graph_alignment: 'left'
    <BLANKLINE>

    >>> # variables or LEC expressions can be also passed as a single string
    >>> lines_vars = "GOSG;YDTG;DTH;DTF;IT;YSSG;COTRES;RIDG;OCUG;$ENVI"
    >>> table_title = "Table example with all variables passed as a single string"
    >>> table = Table(2, table_title, lines_vars, mode=True, files=True, date=True)
    >>> table           # doctest: +NORMALIZE_WHITESPACE
    DIVIS | 1                                                                    |
    TITLE |          "Table example with all variables passed as a single string"
    ----- | ------------------------------------------------------------------------------
    CELL  |                                                                      |   "#S"
    ----- | ------------------------------------------------------------------------------
    CELL  | "Bruto exploitatie-overschot: overheid (= afschrijvingen)."          |    GOSG
    CELL  | "Overheid: geïnde indirecte belastingen."                            |    YDTG
    CELL  | "Totale overheid: directe belasting van de gezinnen."                |     DTH
    CELL  | "Totale overheid: directe vennootschapsbelasting."                   |     DTF
    CELL  | "Totale indirecte belastingen."                                      |      IT
    CELL  | "Globale overheid: ontvangen sociale zekerheidsbijdragen."           |    YSSG
    CELL  | "Cotisation de responsabilisation."                                  |  COTRES
    CELL  | "Overheid: inkomen uit vermogen."                                    |    RIDG
    CELL  | "Globale overheid: saldo van de ontvangen lopendeoverdrachten."      |    OCUG
    CELL  | "Wisselkoers van de USD t.o.v. de BEF (jaargemiddelde)."             |      EX
    CELL  | "Index wereldprijs - invoer van niet-energieprodukten, inUSD."       |   PWMAB
    CELL  | "Index wereldprijs - invoer van diensten, in USD."                   |    PWMS
    CELL  | "Index wereldprijs - uitvoer van niet-energieprodukten, inUSD."      |   PWXAB
    CELL  | "Index wereldprijs - uitvoer van diensten, in USD."                  |    PWXS
    CELL  | "Indicator van het volume van de wereldvraag naar goederen,1985=1."  |   QWXAB
    CELL  | "Indicator van het volume van de wereldvraag naar diensten,1985=1."  |    QWXS
    CELL  | "Brent olieprijs (USD per barrel)."                                  |    POIL
    CELL  | "Totale beroepsbevolking (jaargemiddelde)."                          |    NATY
    CELL  | "TFPFHP_"                                                            | TFPFHP_
    ----- | ------------------------------------------------------------------------------
    MODE  |
    FILES |
    DATE  |
    <BLANKLINE>
    nb lines: 27
    nb columns: 2
    language: 'English'
    gridx: 'major'
    gridy: 'major'
    graph_axis: 'values'
    graph_alignment: 'left'
    <BLANKLINE>
    """

    cdef CTable* c_table

    def __cinit__(self, nb_columns: int = 2, table_title: str = "", lecs_or_vars: Union[str, List[str]] = None, 
                  lines_titles: List[str] = None, mode: bool = False, files: bool = False, date: bool = False) -> Table:
        cdef vector[string] cpp_variables = []
        cdef vector[string] cpp_lines_titles = []
        cdef vector[string] cpp_lecs = []

        if not isinstance(nb_columns, int):
            raise TypeError(f"'nb_columns': Expected value of type int. "
                            f"Got value of type {type(nb_columns).__name__} instead")
        if nb_columns < 1:
            raise ValueError(f"'nb_columns': Expected value greater than 0. Got value '{nb_columns}'.")

        if not isinstance(table_title, str):
            raise TypeError(f"'table_title': Expected value of type str. "
                            f"Got value of type {type(table_title).__name__} instead")

        if lecs_or_vars is None:
            lecs_or_vars = ""

        if not isinstance(mode, bool):
            raise TypeError(f"'mode': Expected value of type bool. "
                            f"Got value of type {type(mode).__name__} instead")
        
        if not isinstance(files, bool):
            raise TypeError(f"'files': Expected value of type bool. "
                            f"Got value of type {type(files).__name__} instead")
        
        if not isinstance(date, bool):
            raise TypeError(f"'date': Expected value of type bool. "
                            f"Got value of type {type(date).__name__} instead")

        if lines_titles is None:
            variables = lecs_or_vars
            if isinstance(variables, str):
                self.c_table = new CTable(<int>nb_columns, <string>table_title.encode(), <string>variables.encode(), <bint>mode, <bint>files, <bint>date)
            elif isinstance(variables, Iterable) and all(isinstance(var, str) for var in variables):
                for variable in variables:
                    cpp_variables.push_back(variable.encode())
                self.c_table = new CTable(<int>nb_columns, <string>table_title.encode(), cpp_variables, <bint>mode, <bint>files, <bint>date)
            else:
                raise TypeError(f"'lecs_or_vars': Expected value of type str or list of str. "
                                f"Got value of type {type(lecs_or_vars).__name__} instead")
        else:
            if isinstance(lines_titles, str):
                lines_titles = [lines_titles]
            if not (isinstance(lines_titles, Iterable) and all(isinstance(line_title, str) for line_title in lines_titles)):
                raise TypeError(f"'lines_titles': Expected a value of type list of str. "
                                f"Got value of type {type(lines_titles).__name__} instead")

            lecs = lecs_or_vars
            if isinstance(lecs, str):
                lecs = [lecs]
            if not (isinstance(lecs, Iterable) and all(isinstance(lec, str) for lec in lecs)):
                raise TypeError(f"'lecs_or_vars': Expected a value of type str or list of str. "
                                f"Got value of type {type(lines_titles).__name__} instead")
            
            if len(lines_titles) != len(lecs):
                raise ValueError(f"'lecs_or_vars' and 'lines_titles': The list of LEC expressions ({len(lecs)}) " 
                                 f"and the list of line titles ({len(lines_titles)}) must have the same lenght")
            
            for line_title in lines_titles:
                cpp_lines_titles.push_back(line_title.encode())

            for lec in lecs:
                cpp_lecs.push_back(lec.encode())

            self.c_table = new CTable(nb_columns, <string>table_title.encode(), cpp_lines_titles, cpp_lecs, <bint>mode, <bint>files, <bint>date)

    def __dealloc__(self):
        del self.c_table

    @property
    def nb_lines(self) -> int:
        return <int>(self.c_table.nb_lines())

    @property
    def nb_columns(self) -> int:
        return <int>(self.c_table.nb_columns())

    @property
    def language(self) -> str:
        """
        Language.

        Parameters
        ----------
        lang : int
            Possible values are 'English' (LANG_ENGLISH), 'Dutch' (LANG_DUTCH) and 'French' (LANG_FRENCH).

        Examples
        --------
        >>> from iode import Table, LANG_ENGLISH, LANG_DUTCH, LANG_FRENCH
        >>> table = Table()
        >>> table.language
        'English'
        >>> table.language = LANG_DUTCH
        >>> table.language
        'Dutch'
        """
        return self.c_table.get_language().decode()

    @language.setter
    def language(self, lang: int):
        if not isinstance(lang, int):
            raise TypeError(f"Expected value of type int. Got value of type {type(lang).__name__} instead")
        if lang not in [LANG_ENGLISH, LANG_DUTCH, LANG_FRENCH]:
            raise ValueError(f"The value for 'language' must be either {LANG_ENGLISH} (LANG_ENGLISH), "
                             f"{LANG_DUTCH} (LANG_DUTCH) or {LANG_FRENCH} (LANG_FRENCH).\n"
                             f"Got value {lang} instead.")
        self.c_table.set_language(<EnumLang>lang)

    @property
    def gridx(self) -> str:
        """
        The gridx value of the table.

        Parameters
        ----------
        gridx: str or int
            Possible values are 'major'/GRAPH_GRID_MAJOR, 
            'none'/GRAPH_GRID_NONE and 'minor'/GRAPH_GRID_MINOR.

        Examples
        --------
        >>> from iode import Table, GRAPH_GRID_MAJOR, GRAPH_GRID_NONE, GRAPH_GRID_MINOR
        >>> table = Table()
        >>> table.gridx
        'major'
        >>> table.gridx = GRAPH_GRID_NONE
        >>> table.gridx
        'none'
        >>> table.gridx = 'minor'
        >>> table.gridx
        'minor'
        """
        return GRAPH_GRID_DICT[<int>(self.c_table.get_gridx())]

    @gridx.setter
    def gridx(self, gridx: Union[str, int]):
        if not isinstance(gridx, (str, int)):
            raise TypeError(f"Expected value of type str or int. Got value of type {type(gridx).__name__} instead")
        if isinstance(gridx, str):
            gridx = GRAPH_GRID_REV_DICT[gridx] if gridx in GRAPH_GRID_REV_DICT else -1
        if gridx not in GRAPH_GRID_DICT:
            raise ValueError(f"Possible values are {list(GRAPH_GRID_DICT.values())} or "
                             f"[GRAPH_GRID_MAJOR, GRAPH_GRID_NONE, GRAPH_GRID_MINOR].\n"
                             f"Got value {gridx} instead.")
        self.c_table.set_gridx(<EnumGraphGrid>gridx)

    @property
    def gridy(self) -> str:
        """
        The gridy value of the table.

        Parameters
        ----------
        gridy: str or int
            Possible values are 'major'/GRAPH_GRID_MAJOR, 
            'none'/GRAPH_GRID_NONE and 'minor'/GRAPH_GRID_MINOR.

        Examples
        --------
        >>> from iode import Table, GRAPH_GRID_MAJOR, GRAPH_GRID_NONE, GRAPH_GRID_MINOR
        >>> table = Table()
        >>> table.gridy
        'major'
        >>> table.gridy = GRAPH_GRID_NONE
        >>> table.gridy
        'none'
        >>> table.gridy = 'minor'
        >>> table.gridy
        'minor'
        """
        return GRAPH_GRID_DICT[<int>(self.c_table.get_gridy())]

    @gridy.setter
    def gridy(self, gridy: Union[str, int]):
        if not isinstance(gridy, (str, int)):
            raise TypeError(f"Expected value of type str or int. Got value of type {type(gridy).__name__} instead")
        if isinstance(gridy, str):
            gridy = GRAPH_GRID_REV_DICT[gridy] if gridy in GRAPH_GRID_REV_DICT else -1
        if gridy not in GRAPH_GRID_DICT:
            raise ValueError(f"Possible values are {list(GRAPH_GRID_DICT.values())} or "
                             f"[GRAPH_GRID_MAJOR, GRAPH_GRID_NONE, GRAPH_GRID_MINOR].\n"
                             f"Got value {gridy} instead.")
        self.c_table.set_gridy(<EnumGraphGrid>gridy)

    @property
    def graph_axis(self) -> str:
        """
        Graph axis of the table.

        Parameters
        ----------
        axis : str or int
            Possible values are 'values'/GRAPH_AXIS_VALUES, 'log'/GRAPH_AXIS_LOG, 
            'semilog'/GRAPH_AXIS_SEMILOG, 'percent'/GRAPH_AXIS_PERCENT

        Examples
        --------
        >>> from iode import Table, GRAPH_AXIS_VALUES, GRAPH_AXIS_LOG, GRAPH_AXIS_SEMILOG, GRAPH_AXIS_PERCENT
        >>> table = Table()
        >>> table.graph_axis
        'values'
        >>> table.graph_axis = GRAPH_AXIS_LOG
        >>> table.graph_axis
        'log'
        >>> table.graph_axis = 'percent'
        >>> table.graph_axis
        'percent'
        """
        return GRAPH_AXIS_DICT[self.c_table.get_graph_axis()]

    @graph_axis.setter
    def graph_axis(self, axis: Union[str, int]):
        if not isinstance(axis, (str, int)):
            raise TypeError(f"Expected value of type str or int. Got value of type {type(axis).__name__} instead")
        if isinstance(axis, str):
            axis = GRAPH_AXIS_REV_DICT[axis] if axis in GRAPH_AXIS_REV_DICT else -1
        if axis not in GRAPH_AXIS_DICT:
            raise ValueError(f"Possible values are {list(GRAPH_AXIS_DICT.values())} or "
                             f"[GRAPH_AXIS_VALUES, GRAPH_AXIS_LOG, GRAPH_AXIS_SEMILOG, GRAPH_AXIS_PERCENT].\n"
                             f"Got value {axis} instead.")
        self.c_table.set_graph_axis(<EnumGraphAxis>axis)

    @property
    def graph_alignment(self) -> str:
        """
        Graph alignment of the table.

        Parameters
        ----------
        align : str or int
            Possible values are 'left'/GRAPH_ALIGN_LEFT, 'center'/GRAPH_ALIGN_CENTER, 
            'right'/GRAPH_ALIGN_RIGHT.

        Examples
        --------
        >>> from iode import Table, GRAPH_ALIGN_LEFT, GRAPH_ALIGN_CENTER, GRAPH_ALIGN_RIGHT
        >>> table = Table()
        >>> table.graph_alignment
        'left'
        >>> table.graph_alignment = GRAPH_ALIGN_CENTER
        >>> table.graph_alignment
        'center'
        >>> table.graph_alignment = 'right'
        >>> table.graph_alignment
        'right'
        """
        return GRAPH_ALIGN_DICT[<int>(self.c_table.get_graph_alignment())]

    @graph_alignment.setter
    def graph_alignment(self, align: Union[str, int]):
        if not isinstance(align, (str, int)):
            raise TypeError(f"Expected value of type str or int. Got value of type {type(align).__name__} instead")
        if isinstance(align, str):
            align = GRAPH_ALIGN_REV_DICT[align] if align in GRAPH_ALIGN_REV_DICT else -1
        if align not in GRAPH_ALIGN_DICT:
            raise ValueError(f"Possible values are {list(GRAPH_ALIGN_DICT.values())} or "
                             f"[GRAPH_ALIGN_LEFT, GRAPH_ALIGN_CENTER, GRAPH_ALIGN_RIGHT].\n"
                             f"Got value {align} instead.")
        self.c_table.set_graph_alignment(<EnumGraphAlign>align)

    def _get_row_from_index(self, index: int) -> int:
        if not isinstance(index, int):
            raise TypeError(f"The index of the line must be of type int. "
                            f"Got an index value of type {type(index).__name__} instead")
        if not (-len(self) < index < len(self)):
            raise ValueError(f"The index of the line must be in range [{-len(self)+1, len(self)-1}].\n"
                             f"Got value {index} instead.")
        if index < 0:
            index = self.nb_lines + index
        return index

    def index(self, key: str) -> int:
        """
        Return the index of the line containing the string `key`.

        Parameters
        ----------
        key: str
            Text to be retrieved either as a title or as a cell content.

        Returns
        -------
        int

        Examples
        --------
        >>> from iode import SAMPLE_DATA_DIR, LINE_TYPE_LINE
        >>> from iode import Table, comments, lists, variables
        >>> comments.load(f"{SAMPLE_DATA_DIR}/fun.cmt")
        >>> lists.load(f"{SAMPLE_DATA_DIR}/fun.lst")
        >>> variables.load(f"{SAMPLE_DATA_DIR}/fun.var")
        >>> table_title = "Table example"
        >>> lines_titles = ["GOSG:", "YSSG+COTRES:", "OCUG:"]
        >>> lines_lecs = ["GOSG", "YSSG+COTRES", "OCUG"]
        >>> table = Table(2, table_title, lines_lecs, lines_titles, True, True, True)
        >>> table           # doctest: +NORMALIZE_WHITESPACE
        DIVIS | 1              |
        TITLE |       "Table example"
        ----- | ----------------------------
        CELL  |                |     "#S"
        ----- | ----------------------------
        CELL  | "GOSG:"        |        GOSG
        CELL  | "YSSG+COTRES:" | YSSG+COTRES
        CELL  | "OCUG:"        |        OCUG
        ----- | ----------------------------
        MODE  |
        FILES |
        DATE  |
        <BLANKLINE>
        nb lines: 11
        nb columns: 2
        language: 'English'
        gridx: 'major'
        gridy: 'major'
        graph_axis: 'values'
        graph_alignment: 'left'
        <BLANKLINE>        

        >>> table.index("Table example")
        0
        >>> table.index("GOSG:")
        4
        >>> table.index("OCUG")
        6
        """
        cdef CTableLine* c_line
        cdef CTableCell* c_cell

        if not isinstance(key, str):
            raise TypeError(f"Expected value of type str. Got value of type {type(key).__name__} instead.")
        key = key.replace('"', '').strip()

        nb_columns = self.nb_columns
        for i in range(len(self)):
            c_line = self.c_table.get_line(i)
            line_type = <int>(c_line.get_line_type())
            if line_type == LINE_TYPE_TITLE:
                content = c_line.get_cell(0, nb_columns).get_content(<bint>False).decode().strip()
                if content == key:
                    return i
            elif line_type == LINE_TYPE_CELL:
                for j in range(nb_columns):
                    c_cell = c_line.get_cell(j, nb_columns)
                    content = c_cell.get_content(<bint>False).decode().strip()
                    if content == key:
                        return i
            else:
                pass
        
        raise ValueError(f"Content '{key}' not found in table")

    def insert(self, index: int, value: Union[int, str, List[str], Tuple[str], TableLine], after: bool = True):
        r"""
        Insert a new line in the table.

        Parameters
        ----------
        index: int
            index where to insert a line.
        value: int or str or list(str) or tuple(str) or TableLine
            value of the new line.\n
            If int, 'value' represents the type of the new line: LINE_TYPE_FILES, LINE_TYPE_MODE, 
            LINE_TYPE_DATE, LINE_TYPE_LINE.\n
            If str, 'value' represents either a separator line if it only contains characters '-' 
            or a title line.
            If an iterable of str, 'value' represents the content of the cells of the new line.\n
        after: bool, optional
            If True, insert the new line after the line at the specified index. 
            If False, insert the new line before the line at the specified index. 
            Defaults to True.

        Examples
        --------
        >>> from iode import SAMPLE_DATA_DIR, LINE_TYPE_LINE
        >>> from iode import Table, comments, lists, variables
        >>> comments.load(f"{SAMPLE_DATA_DIR}/fun.cmt")
        >>> lists.load(f"{SAMPLE_DATA_DIR}/fun.lst")
        >>> variables.load(f"{SAMPLE_DATA_DIR}/fun.var")
        >>> table_title = "Table example"
        >>> lines_titles = ["GOSG:", "YSSG+COTRES:", "OCUG:"]
        >>> lines_lecs = ["GOSG", "YSSG+COTRES", "OCUG"]
        >>> table = Table(2, table_title, lines_lecs, lines_titles, True, True, True)
        >>> table           # doctest: +NORMALIZE_WHITESPACE
        DIVIS | 1              |
        TITLE |       "Table example"
        ----- | ----------------------------
        CELL  |                |     "#S"
        ----- | ----------------------------
        CELL  | "GOSG:"        |        GOSG
        CELL  | "YSSG+COTRES:" | YSSG+COTRES
        CELL  | "OCUG:"        |        OCUG
        ----- | ----------------------------
        MODE  |
        FILES |
        DATE  |
        <BLANKLINE>
        nb lines: 11
        nb columns: 2
        language: 'English'
        gridx: 'major'
        gridy: 'major'
        graph_axis: 'values'
        graph_alignment: 'left'
        <BLANKLINE>

        >>> index = table.index("YSSG+COTRES:")
        >>> index
        5

        >>> # insert new separator line
        >>> table.insert(index, '-')
        >>> index += 1

        >>> # insert new title
        >>> table.insert(index, "New Title")
        >>> index += 1

        >>> # insert new separator line 
        >>> table.insert(index, LINE_TYPE_LINE)
        >>> index += 1

        >>> # insert new line with cells
        >>> # "    -> STRING cell
        >>> # no " -> LEC cell
        >>> table.insert(index, ['"RIDG:', 'RIDG'])

        >>> table           # doctest: +NORMALIZE_WHITESPACE
        DIVIS | 1              |
        TITLE |       "Table example"
        ----- | ----------------------------
        CELL  |                |     "#S"
        ----- | ----------------------------
        CELL  | "GOSG:"        |        GOSG
        CELL  | "YSSG+COTRES:" | YSSG+COTRES
        ----- | ----------------------------
        TITLE |         "New Title"
        ----- | ----------------------------
        CELL  | "RIDG:"        |        RIDG
        CELL  | "OCUG:"        |        OCUG
        ----- | ----------------------------
        MODE  |
        FILES |
        DATE  |
        <BLANKLINE>
        nb lines: 15
        nb columns: 2
        language: 'English'
        gridx: 'major'
        gridy: 'major'
        graph_axis: 'values'
        graph_alignment: 'left'
        <BLANKLINE>
        """
        cdef CTableLine* c_line

        row = self._get_row_from_index(index)
        if isinstance(value, int):
            if value == LINE_TYPE_FILES:
                self.c_table.insert_line_files(row, <bint>after)
            elif value == LINE_TYPE_MODE: 
                self.c_table.insert_line_mode(row, <bint>after)
            elif value == LINE_TYPE_DATE: 
                self.c_table.insert_line_date(row, <bint>after)
            elif value == LINE_TYPE_LINE: 
                self.c_table.insert_line_separator(row, <bint>after)
            else:
                raise ValueError(f"The value of 'value' must be either LINE_TYPE_FILES, LINE_TYPE_MODE, "
                                 f"LINE_TYPE_DATE or LINE_TYPE_LINE.\nGot value {value} instead.")
        elif isinstance(value, str):
            if all(character == '-' for character in value):
                self.c_table.insert_line_separator(row, <bint>after)
            elif '|' in value:
                self.insert(row, value.split('|'))
            else:
                self.c_table.insert_title(row, value.encode(), <bint>after)
        elif isinstance(value, Iterable) and all(isinstance(item, str) for item in value):
            c_line = self.c_table.insert_line_with_cells(row, <bint>after)
            nb_columns = self.nb_columns
            if len(value) != nb_columns:
                raise ValueError(f"The length of 'value' {len(value)} must be equal to the number of columns {nb_columns}")
            for j in range(nb_columns):
                c_line.get_cell(j, nb_columns).set_content(value[j].encode())
        elif isinstance(value, TableLine):
            line_type = <int>(value.c_line.get_line_type())
            if line_type == LINE_TYPE_TITLE or line_type == LINE_TYPE_CELL:
                self.insert(row, str(value))
            else:
                self.insert(row, line_type)
        else:
            raise TypeError(f"The value of 'value' must be of type int, str, list(str) or tuple(str).\n"
                            f"Got value of type {type(value).__name__} instead.")

    def compute(self, generalized_sample: str, extra_files: Union[str, List[str]] = None, nb_decimals: int = 2) -> ComputedTable:
        """
        Compute the values corresponding to LEC expressions in cells.
        
        The values are calculated for given a *generalized sample*. 
        This sample contains the following information:

          - the sampling of the periods to take into account 
          - the operations to be performed on the periods
          - the list of files involved in the computation of the table
          - the operations to be performed between files
          - the repetition factor

        The syntax of the *generalized sample* follows the rules described below.

        The syntax of a period: 
        
          - a period is indicated as in LEC: 'yyPpp' or 'yyyyPpp' where yyyy indicates the year, 
            P the periodicity and pp the sub-period (1990Y1) 
          - a period can be shifted n periods to the left or right using the operators <n and >n 
          - when used with a null argument, the shift oprerators have a special meaning: 
              - <0 means "first period of the year" 
              - >0 means "last period of the year" 
          - the special periods 'BOS', 'EOS' and 'NOW' can be used to represent the beginning 
            or end of the current sample or the current period (PC clock)
          - the special periods 'BOS1', 'EOS1' and 'NOW1' are equivalent to the previous ones, 
            except that they are moved to the first sub-period of the year of 'BOS', 'EOS' and 
            'NOW' respectively (if NOW = 2012M5, NOW1 = 2012M1)
          - each period is separated from the next by a semicolon 
          - a period or group of periods can be repeated: simply place the colon character (:) after 
            the definition of the column or group of columns, followed by the desired number of repetitions. 
            Repetitions are made with an increment of one period, unless followed by an asterisk and a value. 
            This value is then the repeat increment. It can be negative, in which case the periods are 
            presented in decreasing order
          - the repeat, increment and shift can be the words PER (or P) or SUB (or S), which respectively indicate 
            the number of periods in a year of the current sample and the current sub-period
          - the file definition is optional and is enclosed in square brackets. 
            It applies to all preceding period definitions

        The following file operations are possible:

          - absolute value: [1] 
          - difference: [1-2] 
          - difference in percent: [1/2] 
          - sum: [1+2] 
          - average: [1~2] or [1^2]. 

        The following period operations are possible:

          - value: (75) 
          - growth rate over one or more periods: (75/74, 75/70) 
          - average growth rate: (75//70) 
          - difference: (75-74, 75-70) 
          - average difference: (75--70) 
          - average: (75~74) or (75^74) 
          - sum of consecutive periods: (70Q1+70Q4) 
          - index or base value: (76=70) 

        Repetition can be performed with an increment greater than 1 or less than 0: 
        simply place a star followed by the step after the number of repetitions (70:3*5 = 70, 75, 80).

        Examples::

            70; 75; 80:6 = 70:3*5; 81:5 = 70; 75; 80; 81; 82; 83; 84; 85
            70/69:2 = 70/69; 71/70
            (70; 70-69):2 = 70; 70-69; 71; 71-70;
            70[1,2]:2*5 = 70[1]; 70[2]; 75[1]; 75[2]
            (70;75)[1,2-1] = 70[1]; 75[1]; 70[2-1]; 75[2-1]
            (70;75;(80; 80/79):2)[1,2] = 70[1]; 70[2]; 75[1]; 75[2]; 80[1]; 80[2]; 80/79[1]; 80/79[2] 81[1]; 81[2]; 81/80[1]; 81/80[2]
            2000Y1>5 = 2005Y1
            1999M1>12 = 2000M1
            EOS<1 = 2019Y1                         (if EOS == 2020Y1)
            BOS<1 = 1959Y1                         (if BOS == 1960Y1)
            EOS<4:5*-1 =2016;2017;2018;2019;2020   (if EOS = 2020Y1)

        Parameters
        ----------
        generalized_sample: str
            Generalized sample (see above).
        extra_files: str or list(str)
            (List of) extra file(s) referenced in the generalized sample.
        nb_decimals: int
            The number of decimals to display.

        Returns
        -------
        ComputedTable

        Examples
        --------
        >>> from iode import SAMPLE_DATA_DIR
        >>> from iode import Table, tables, variables
        >>> tables.load(f"{SAMPLE_DATA_DIR}/fun.tbl")
        >>> variables.load(f"{SAMPLE_DATA_DIR}/fun.var")
        >>> tables["C8_1"]          # doctest: +NORMALIZE_WHITESPACE
        DIVIS | 1                                  |
        TITLE |      "Déterminants de l'output potentiel"
        ----- | ---------------------------------------------
        CELL  | ""                                 |   "#s"
        ----- | ---------------------------------------------
        CELL  | "Output potentiel"                 |  Q_F+Q_I
        CELL  | "Stock de capital"                 | KNFF[-1]
        CELL  | "Intensité de capital"             |    KLFHP
        CELL  | "Productivité totale des facteurs" |  TFPFHP_
        <BLANKLINE>
        nb lines: 8
        nb columns: 2
        language: 'English'
        gridx: 'major'
        gridy: 'major'
        graph_axis: 'values'
        graph_alignment: 'left'
        <BLANKLINE>
        >>> # simple time series (current workspace) - 6 observations - 4 decimals
        >>> computed_table = tables["C8_1"].compute("2000:6", nb_decimals=4)
        >>> computed_table              # doctest: +NORMALIZE_WHITESPACE
           line title \ period[file]     |     00    |     01    |     02    |     03    |     04    |     05
        ---------------------------------------------------------------------------------------------------------
        Output potentiel                 | 5495.2128 | 5627.8589 | 5748.7804 | 5857.9529 | 5966.1999 | 6103.6318
        Stock de capital                 | 8083.5517 | 8359.8908 | 8647.9354 | 8910.3393 | 9175.8106 | 9468.8865
        Intensité de capital             |    0.5032 |    0.4896 |    0.4758 |    0.4623 |    0.4481 |    0.4349
        Productivité totale des facteurs |    0.9938 |    1.0037 |    1.0137 |    1.0239 |    1.0341 |    1.0445
        <BLANKLINE>        
        >>> # two time series (current workspace) - 5 observations - 2 decimals (default)
        >>> computed_table = tables["C8_1"].compute("(2010;2010/2009):5")
        >>> computed_table              # doctest: +NORMALIZE_WHITESPACE
           line title \ period[file]     |    10    | 10/09 |    11    | 11/10 |    12    | 12/11 |    13    | 13/12 |    14    | 14/13
        --------------------------------------------------------------------------------------------------------------------------------
        Output potentiel                 |  6936.11 |  1.74 |  7045.34 |  1.57 |  7161.54 |  1.65 |  7302.29 |  1.97 |  7460.12 |  2.16
        Stock de capital                 | 11293.85 |  2.82 | 11525.01 |  2.05 | 11736.78 |  1.84 | 11975.49 |  2.03 | 12263.95 |  2.41
        Intensité de capital             |     0.39 | -2.17 |     0.38 | -2.05 |     0.37 | -1.91 |     0.36 | -1.86 |     0.36 |  -1.9
        Productivité totale des facteurs |      1.1 |   1.0 |     1.11 |   1.0 |     1.12 |   1.0 |     1.13 |   1.0 |     1.14 |   1.0
        <BLANKLINE>
        >>> # simple time series (current workspace + one extra file) - 5 observations - 2 decimals (default)
        >>> computed_table = tables["C8_1"].compute("2010[1;2]:5", extra_files=f"{SAMPLE_DATA_DIR}/ref.av")
        >>> computed_table              # doctest: +NORMALIZE_WHITESPACE
           line title \ period[file]     |  10[1]   |  10[2]   |  11[1]   |  11[2]   |  12[1]   |  12[2]   |  13[1]   |  13[2]   |  14[1]   |  14[2]    
        ----------------------------------------------------------------------------------------------------------------------------------------------- 
        Output potentiel                 |  6936.11 |  6797.39 |  7045.34 |  6904.44 |  7161.54 |  7018.31 |  7302.29 |  7156.24 |  7460.12 |  7310.91  
        Stock de capital                 | 11293.85 | 11067.97 | 11525.01 | 11294.51 | 11736.78 | 11502.05 | 11975.49 | 11735.98 | 12263.95 | 12018.67  
        Intensité de capital             |     0.39 |     0.38 |     0.38 |     0.37 |     0.37 |     0.36 |     0.36 |     0.36 |     0.36 |     0.35  
        Productivité totale des facteurs |      1.1 |     1.08 |     1.11 |     1.09 |     1.12 |      1.1 |     1.13 |     1.11 |     1.14 |     1.12  
        <BLANKLINE>
        """
        if not isinstance(generalized_sample, str):
            raise TypeError(f"'generalized_sample': Expected value of type str. "
                            f"Got value of type {type(generalized_sample).__name__} instead")
        if not generalized_sample:
            raise ValueError("'generalized_sample' must not be empty")

        if extra_files is not None:
            if isinstance(extra_files, str):
                extra_files = [extra_file.strip() for extra_file in extra_files.split(';')]
            for i, extra_file in enumerate(extra_files):
                if not isinstance(extra_file, str):
                    raise TypeError(f"'extra_files': Expected value of type str or list of str. "
                                    f"Got value of type {type(extra_files).__name__} instead")
                load_reference_kdb(i + 2, FILE_VARIABLES, extra_file.encode())

        if not isinstance(nb_decimals, int):
            raise TypeError(f"'nb_decimals': Expected value of type int. "
                            f"Got value of type {type(nb_decimals).__name__} instead")
        return ComputedTable.initialize(self.c_table, generalized_sample.encode(), nb_decimals)

    def __len__(self) -> int:
        """
        Number of lines of the table.

        Equivalent to :py:meth:`Table.nb_lines`.

        Notes
        -----
        The special divider line is not taken into account if the returned number of lines.

        Examples
        --------
        >>> from iode import Table
        >>> table = Table()
        >>> table           # doctest: +NORMALIZE_WHITESPACE
        DIVIS | 1 |
        TITLE |
        ----- | --------
        CELL  |   | "#S"
        ----- | --------
        <BLANKLINE>
        nb lines: 4
        nb columns: 2
        language: 'English'
        gridx: 'major'
        gridy: 'major'
        graph_axis: 'values'
        graph_alignment: 'left'
        <BLANKLINE>

        >>> len(table)
        4
        >>> # equivalent to nb_lines
        >>> table.nb_lines
        4
        """
        return self.nb_lines

    def __getitem__(self, index: int) -> TableLine:
        """
        Get a line of the table.

        Parameters
        ----------
        index: int
            index of the line.

        Returns
        -------
        TableLine

        Examples
        --------
        >>> from iode import SAMPLE_DATA_DIR
        >>> from iode import Table, comments, lists, variables
        >>> comments.load(f"{SAMPLE_DATA_DIR}/fun.cmt")
        >>> lists.load(f"{SAMPLE_DATA_DIR}/fun.lst")
        >>> variables.load(f"{SAMPLE_DATA_DIR}/fun.var")
        >>> table_title = "Table example"
        >>> lines_titles = ["GOSG:", "YSSG+COTRES:", "OCUG:"]
        >>> lines_lecs = ["GOSG", "YSSG+COTRES", "OCUG"]
        >>> table = Table(2, table_title, lines_lecs, lines_titles, True, True, True)
        >>> table           # doctest: +NORMALIZE_WHITESPACE
        DIVIS | 1              |
        TITLE |       "Table example"
        ----- | ----------------------------
        CELL  |                |     "#S"
        ----- | ----------------------------
        CELL  | "GOSG:"        |        GOSG
        CELL  | "YSSG+COTRES:" | YSSG+COTRES
        CELL  | "OCUG:"        |        OCUG
        ----- | ----------------------------
        MODE  |
        FILES |
        DATE  |
        <BLANKLINE>
        nb lines: 11
        nb columns: 2
        language: 'English'
        gridx: 'major'
        gridy: 'major'
        graph_axis: 'values'
        graph_alignment: 'left'
        <BLANKLINE>

        >>> table[0]
        Table example
        >>> table[1]
        ---
        >>> table[4]
        ('"GOSG:"', 'GOSG')
        >>> table[-3]
        <MODE>
        >>> table[-2]
        <FILES>
        >>> table[-1]
        <DATE>
        """
        cdef CTableLine* c_line

        row = self._get_row_from_index(index)
        c_line = self.c_table.get_line(row)
        return TableLine.from_ptr(c_line, self.c_table.nb_columns())
    
    def __setitem__(self, index: int, value: Union[str, List[str], Tuple[str], TableLine]):
        """
        Update a line of the table.

        Parameters
        ----------
        index: int
            index of the line to be updated.
        value: str or list(str) or tuple(str) or TableLine
            New content for the line. 
        
        Warnings
        --------
        - Line of type 'MODE', 'FILES', 'DATE' and 'LINE' cannot be updated.
        - When updating a cell content, the cell is converted to a cell of type 'STRING' 
          if the new text contains double quotes, otherwise the cell becomes of type 'LEC'.

        Examples
        --------
        >>> from iode import SAMPLE_DATA_DIR
        >>> from iode import Table, comments, lists, variables
        >>> comments.load(f"{SAMPLE_DATA_DIR}/fun.cmt")
        >>> lists.load(f"{SAMPLE_DATA_DIR}/fun.lst")
        >>> variables.load(f"{SAMPLE_DATA_DIR}/fun.var")
        >>> table_title = "Table example"
        >>> lines_titles = ["GOSG:", "YSSG+COTRES:", "OCUG:"]
        >>> lines_lecs = ["GOSG", "YSSG+COTRES", "OCUG"]
        >>> table = Table(2, table_title, lines_lecs, lines_titles, True, True, True)
        >>> table           # doctest: +NORMALIZE_WHITESPACE
        DIVIS | 1              |
        TITLE |       "Table example"
        ----- | ----------------------------
        CELL  |                |     "#S"
        ----- | ----------------------------
        CELL  | "GOSG:"        |        GOSG
        CELL  | "YSSG+COTRES:" | YSSG+COTRES
        CELL  | "OCUG:"        |        OCUG
        ----- | ----------------------------
        MODE  |
        FILES |
        DATE  |
        <BLANKLINE>
        nb lines: 11
        nb columns: 2
        language: 'English'
        gridx: 'major'
        gridy: 'major'
        graph_axis: 'values'
        graph_alignment: 'left'
        <BLANKLINE>

        >>> # warning: line of type 'MODE', 'FILES', 'DATE' and 'LINE' cannot be updated.

        >>> # update a title line
        >>> table[0] = "New title"
        >>> table[0]
        New title

        >>> # update cell contents
        >>> # warning: the cell is converted to a cell of type 'STRING' if the new text contains 
        >>> # double quotes, otherwise the cell becomes of type 'LEC'.
        >>> table[5] = ('"YSSG:"', 'YSSG')
        >>> table[5]
        ('"YSSG:"', 'YSSG')

        >>> table           # doctest: +NORMALIZE_WHITESPACE
        DIVIS | 1       |
        TITLE |  "New title"
        ----- | --------------
        CELL  |         | "#S"
        ----- | --------------
        CELL  | "GOSG:" | GOSG
        CELL  | "YSSG:" | YSSG
        CELL  | "OCUG:" | OCUG
        ----- | --------------
        MODE  |
        FILES |
        DATE  |
        <BLANKLINE>
        nb lines: 11
        nb columns: 2
        language: 'English'
        gridx: 'major'
        gridy: 'major'
        graph_axis: 'values'
        graph_alignment: 'left'
        <BLANKLINE>
        """
        cdef CTableLine* c_line
        cdef CTableCell* c_cell

        row = self._get_row_from_index(index)

        if isinstance(value, TableLine):
            value = str(value)
        if isinstance(value, str) and '|' in value:
            value = value.split('|')
        
        c_line = self.c_table.get_line(row)
        line_type = <int>(c_line.get_line_type())
        nb_columns = self.nb_columns
        if line_type == LINE_TYPE_TITLE:
            if not isinstance(value, str):
                raise TypeError(f"Cannot upate line {row}. Expected new content of type str. "
                                f"Got new content of type {type(value).__name__} instead.")
            c_line.get_cell(0, nb_columns).set_text(value.encode())
        elif line_type == LINE_TYPE_CELL:
            if not (isinstance(value, Iterable) and all(isinstance(item, str) for item in value)):
                raise TypeError(f"Cannot upate cells at line {row}. Expected new content of type list or tuple of str.\n"
                                f"Got new content of type {type(value).__name__} instead.")
            if len(value) != nb_columns:
                raise ValueError(f"Cannot upate cells at line {row}.\nThe length of passed {type(value).__name__} "
                                 f"({len(value)}) must be the same of the number of cells ({nb_columns}) in the table")
            for j, cell_content in enumerate(value):
                c_cell = c_line.get_cell(j, nb_columns)
                c_cell.set_content(cell_content.encode())
        else:
            warnings.warn(f"Line of type '{LINE_TYPE_DICT[line_type]}' cannot be updated")
    
    def __delitem__(self, index: int):
        """
        Delete the line at the given index

        Parameters
        ----------
        index: int
            index of the line to be removed.

        Examples
        --------
        >>> from iode import SAMPLE_DATA_DIR
        >>> from iode import Table, comments, lists, variables
        >>> comments.load(f"{SAMPLE_DATA_DIR}/fun.cmt")
        >>> lists.load(f"{SAMPLE_DATA_DIR}/fun.lst")
        >>> variables.load(f"{SAMPLE_DATA_DIR}/fun.var")
        >>> table_title = "Table example"
        >>> lines_titles = ["GOSG:", "YSSG+COTRES:", "OCUG:"]
        >>> lines_lecs = ["GOSG", "YSSG+COTRES", "OCUG"]
        >>> table = Table(2, table_title, lines_lecs, lines_titles, True, True, True)
        >>> table           # doctest: +NORMALIZE_WHITESPACE
        DIVIS | 1              |
        TITLE |       "Table example"
        ----- | ----------------------------
        CELL  |                |     "#S"
        ----- | ----------------------------
        CELL  | "GOSG:"        |        GOSG
        CELL  | "YSSG+COTRES:" | YSSG+COTRES
        CELL  | "OCUG:"        |        OCUG
        ----- | ----------------------------
        MODE  |
        FILES |
        DATE  |
        <BLANKLINE>
        nb lines: 11
        nb columns: 2
        language: 'English'
        gridx: 'major'
        gridy: 'major'
        graph_axis: 'values'
        graph_alignment: 'left'
        <BLANKLINE> 

        >>> # remove the MODE line
        >>> del table[-3]
        >>> table           # doctest: +NORMALIZE_WHITESPACE
        DIVIS | 1              |
        TITLE |       "Table example"
        ----- | ----------------------------
        CELL  |                |     "#S"
        ----- | ----------------------------
        CELL  | "GOSG:"        |        GOSG
        CELL  | "YSSG+COTRES:" | YSSG+COTRES
        CELL  | "OCUG:"        |        OCUG
        ----- | ----------------------------
        FILES |
        DATE  |
        <BLANKLINE>
        nb lines: 10
        nb columns: 2
        language: 'English'
        gridx: 'major'
        gridy: 'major'
        graph_axis: 'values'
        graph_alignment: 'left'
        <BLANKLINE>        
        """
        row = self._get_row_from_index(index)
        self.c_table.delete_line(row)

    def __iadd__(self, value: Union[int, str, List[str], Tuple[str], TableLine]):
        """
        Append a new line to the table.

        Parameters
        ----------
        value: int or str or list(str) or tuple(str) or TableLine
            value of the new line.\n
            If int, 'value' represents the type of the new line: LINE_TYPE_FILES, LINE_TYPE_MODE, 
            LINE_TYPE_DATE, LINE_TYPE_LINE.\n
            If str, 'value' represents either a separator line if it only contains characters '-' 
            or a title line.
            If an iterable of str, 'value' represents the content of the cells of the new line.\n

        Examples
        --------
        >>> from iode import SAMPLE_DATA_DIR, LINE_TYPE_LINE
        >>> from iode import Table, comments, lists, variables
        >>> comments.load(f"{SAMPLE_DATA_DIR}/fun.cmt")
        >>> lists.load(f"{SAMPLE_DATA_DIR}/fun.lst")
        >>> variables.load(f"{SAMPLE_DATA_DIR}/fun.var")
        >>> table_title = "Table example"
        >>> lines_titles = ["GOSG:", "YSSG+COTRES:", "OCUG:"]
        >>> lines_lecs = ["GOSG", "YSSG+COTRES", "OCUG"]
        >>> table = Table(2, table_title, lines_lecs, lines_titles)
        >>> table           # doctest: +NORMALIZE_WHITESPACE
        DIVIS | 1              |
        TITLE |       "Table example"
        ----- | ----------------------------
        CELL  |                |     "#S"
        ----- | ----------------------------
        CELL  | "GOSG:"        |        GOSG
        CELL  | "YSSG+COTRES:" | YSSG+COTRES
        CELL  | "OCUG:"        |        OCUG
        <BLANKLINE>
        nb lines: 7
        nb columns: 2
        language: 'English'
        gridx: 'major'
        gridy: 'major'
        graph_axis: 'values'
        graph_alignment: 'left'
        <BLANKLINE>

        >>> # append a separator line
        >>> table += '-'

        >>> # append a title
        >>> table += "New Title"

        >>> # append a separator line 
        >>> table += LINE_TYPE_LINE

        >>> # append a line with cells
        >>> # "    -> STRING cell
        >>> # no " -> LEC cell
        >>> table += ['"RIDG:"', 'RIDG']

        >>> table           # doctest: +NORMALIZE_WHITESPACE
        DIVIS | 1              |
        TITLE |       "Table example"
        ----- | ----------------------------
        CELL  |                |     "#S"
        ----- | ----------------------------
        CELL  | "GOSG:"        |        GOSG
        CELL  | "YSSG+COTRES:" | YSSG+COTRES
        CELL  | "OCUG:"        |        OCUG
        ----- | ----------------------------
        TITLE |         "New Title"
        ----- | ----------------------------
        CELL  | "RIDG:"        |        RIDG
        <BLANKLINE>
        nb lines: 11
        nb columns: 2
        language: 'English'
        gridx: 'major'
        gridy: 'major'
        graph_axis: 'values'
        graph_alignment: 'left'
        <BLANKLINE>
        """
        cdef CTableLine* c_line

        if isinstance(value, int):
            if value == LINE_TYPE_FILES:
                self.c_table.add_line_files()
            elif value == LINE_TYPE_MODE: 
                self.c_table.add_line_mode()
            elif value == LINE_TYPE_DATE: 
                self.c_table.add_line_date()
            elif value == LINE_TYPE_LINE: 
                self.c_table.add_line_separator()
            else:
                raise ValueError(f"The value of 'value' must be either LINE_TYPE_FILES, LINE_TYPE_MODE, "
                                 f"LINE_TYPE_DATE or LINE_TYPE_LINE.\nGot value {value} instead.")
        elif isinstance(value, str):
            if all(character == '-' for character in value):
                self.c_table.add_line_separator()
            elif '|' in value:
                self.__iadd__(value.split('|'))
            else:
                self.c_table.add_title(value.encode())
        elif isinstance(value, Iterable) and all(isinstance(item, str) for item in value):
            c_line = self.c_table.add_line_with_cells()
            nb_columns = self.nb_columns
            if len(value) != nb_columns:
                raise ValueError(f"The length of 'value' {len(value)} must be equal to the number of columns {nb_columns}")
            for j in range(nb_columns):
                c_line.get_cell(j, nb_columns).set_content(value[j].encode())
        elif isinstance(value, TableLine):
            line_type = <int>(value.c_line.get_line_type())
            if line_type == LINE_TYPE_TITLE or line_type == LINE_TYPE_CELL:
                self.__iadd__(str(value))
            else:
                self.__iadd__(line_type)
        else:
            raise TypeError(f"The value of 'value' must be of type int, str, list(str) or tuple(str).\n"
                            f"Got value of type {type(value).__name__} instead.")
        return self

    def __str__(self) -> str:
        cdef CTableLine* c_line
        cdef CTableCell* c_cell

        nb_cols = self.nb_columns
        max_title_length = 0
        max_cells_length = [0] * nb_cols
        lines = []

        # lines (-1 for the divider line)
        for i in range(-1, self.nb_lines):
            c_line = self.c_table.get_line(i) if i >= 0 else self.c_table.get_divider_line()
            line_type = <int>(c_line.get_line_type())
            if line_type == LINE_TYPE_TITLE:
                content = c_line.get_cell(0, nb_cols).get_content(<bint>True).decode()
                if len(content) > max_title_length:
                    max_title_length = <int>len(content)
                lines += [[line_type, content]]
            elif line_type == LINE_TYPE_CELL:
                cells_content = []
                for j in range(nb_cols):
                    c_cell = c_line.get_cell(j, nb_cols)
                    quotes = <int>(c_cell.get_type()) == CELL_TYPE_STRING
                    cell_align = <int>(c_cell.get_align())
                    content = c_cell.get_content(<bint>quotes).decode()
                    if len(content) > max_cells_length[j]:
                        max_cells_length[j] = <int>len(content)
                    cells_content += [(cell_align, content)]
                lines += [[line_type, *cells_content]]
            else:
                lines += [line_type]

        # (nb_cols-1) * 3 -> for the ' | ' cell separators
        table_length = max(max_title_length, sum(max_cells_length) + ((nb_cols-1) * 3))

        s = ""
        for i, line in enumerate(lines):
            line_type = line if isinstance(line, int) else line[0]
            if line_type == LINE_TYPE_TITLE:
                # remove newline characters
                content = ''.join(line[1].splitlines())
                s += LINE_TYPE_DICT[line_type].ljust(5) + " | " + content.center(table_length)
            elif line_type == LINE_TYPE_CELL:
                # i == 0 -> divider line
                s += "DIVIS" if i == 0 else LINE_TYPE_DICT[line_type].ljust(5)
                line = line[1:]
                for j in range(nb_cols):
                    s += " | "
                    cell_align, cell_content = line[j]
                    cell_width = max_cells_length[j]
                    # remove newline characters
                    cell_content = ''.join(cell_content.splitlines())
                    if cell_align == CELL_ALIGN_LEFT:
                        s += cell_content.ljust(cell_width)
                    elif cell_align == CELL_ALIGN_RIGHT:
                        s += cell_content.rjust(cell_width)
                    elif cell_align == CELL_ALIGN_CENTER:
                        s += cell_content.center(cell_width)
                    # TODO: check what to do with DECIMAL cell
                    elif cell_align == CELL_ALIGN_DECIMAL:
                        s += cell_content.rjust(cell_width)
            elif line_type == LINE_TYPE_LINE:
                s += '-' * 5 + " | " + '-' * table_length
            else:
                s += LINE_TYPE_DICT[line_type].ljust(5) + " | "
            s += "\n"
        
        return s

    def __repr__(self) -> str:
        s = self.__str__() + '\n'
        s += f"nb lines: {self.nb_lines}\n"
        s += f"nb columns: {self.nb_columns}\n"
        s += f"language: '{self.language}'\n"
        s += f"gridx: '{self.gridx}'\n"
        s += f"gridy: '{self.gridy}'\n"
        s += f"graph_axis: '{self.graph_axis}'\n"
        s += f"graph_alignment: '{self.graph_alignment}'\n"
        return s

