import sys
import warnings
from copy import copy
from pathlib import Path
from typing import Union, Tuple, List, Dict, Any
if sys.version_info.minor >= 11:
    from typing import Self
else:
    Self = Any

from iode.common import TableLang
from iode.iode_database.extra_files import load_extra_files
from iode.computed_table.computed_table import ComputedTable

from iode.iode_cython import (TableGraphType, TableGraphGrid, TableGraphAlign, 
                              TableGraphAxis, TableLineType, TableCellAlign)
from iode.iode_cython import TableCell as CythonTableCell
from iode.iode_cython import TableLine as CythonTableLine
from iode.iode_cython import Table as CythonTable


class TableCell:
    r"""
    IODE Table cell.

    Cells in TITLE and CELL lines can contain text. Cells in CELL lines can also contain LEC expressions. 
    Cells of other line types (SEP, FILES, MODE and DATE) do not contain any information, as this would 
    not result in printing.

    Interpolation Of Text Cells
    ~~~~~~~~~~~~~~~~~~~~~~~~~~~

    Text cells in a table can contain expressions that adapt to the sample when printed. 
    These expressions are composed of the '#' character, followed by a letter and optionally a number:

    The letter indicates the value to be taken: 'y' for the year, for example.

    The number indicates the period to be taken into account. In fact, several periods may be involved in 
    calculating the column (80/79, for example). 80 will correspond to period 1, and 79 to period 2, so the 
    number can take the values 1 and 2.

    Some expressions do not need a number. For example, '#O' indicates the operation performed on the years 
    (like growth rate) and must not be followed by 1 or 2.

    If one expression is not followed by a number, then the value 1 is used for the number.

    The expressions can contain::

        Syntax          Meaning                     Example
        ---------------------------------------------------
        #y1 or #y2      year of column              80
        #Y1 or #Y2      year of column              1980
        #p              periodicity                 q
        #P              periodicity                 Q
        #M1 or #M2      sub-period  Monthly         February
                                    Quarterly       IV
                                    Yearly          -
                                    Other           52
        #m1 or #m2      sub-period  Monthly         Feb
                                    Quarterly       iv
                                    Annual          -
                                    Other           52
        #r1 or #r2      sub-period  Monthly         ix
                                    Quarterly       iv
                                    Yearly          -
                                    Other           52
        #R1 or #R2      sub-period  Monthly         IX
                                    Quarterly       IV
                                    Yearly          -
                                    Other           52
        #n1 or #n2      sub-period                  7
        #N1 or #N2      sub-period                  11
        #o              period transaction          /
        #O              period operation            Growth Rates
        #F or #F        files used                  [1-2]

    For the ease of writing, the following macros are defined::

        #t is equivalent to #y1#P1#n1#o1#y2#P2#n2    
        #T is equivalent to #Y1#P1#n1#o1#Y2#P2#n2    
        #S is equivalent to #T#F    
        #s is equivalent to #t#f

    Depending on the sample, some expressions may or may not be empty: if there is no period operation 
    (growth rate), all variables followed by 2 (#Y2) remain empty, as do the #O and #o operations.

    If only one file is used to calculate the table, #f and #F remain empty.

    If the periodicity is annual, the variables #P, #N, #M, #R remain empty, whether in upper or lower case, 
    followed by 1 or 2.

    Attributes
    ----------
    cell_type: TableCellType
        Type of cell content. 
        Possible value is STRING or LEC.
    align: TableCellAlign
        Alignment of the text in the cell. 
        Possible values are CENTER, LEFT, RIGHT or DECIMAL.
    bold: bool
    italic: bool
    underline: bool

    Warnings
    --------
    To distinguish a STRING cell from a LEC cell, **the text of a STRING cell must begin with double 
    quotation marks** (which will not be printed). **A cell that does not begin with a double quotation 
    mark will be considered a LEC form** and syntactically verified.

    See Also
    --------
    TableLine
    Table

    Examples
    --------
    >>> from iode import SAMPLE_DATA_DIR
    >>> from iode import tables, Table
    >>> tables.load(f"{SAMPLE_DATA_DIR}/fun.tbl")       # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
    Loading .../fun.tbl
    46 objects loaded 
    >>> table = tables["ANAPRIX"]
    >>> table           # doctest: +NORMALIZE_WHITESPACE
    DIVIS | 1                            |
    TITLE |                        "Analyse des prix"
    ----- | ---------------------------------------------------------------
    CELL  |                              |               "#s"
    ----- | ---------------------------------------------------------------
    CELL  | "GAP_"                       |                             GAP_
    CELL  | "dln (PC/(1+ITCR))-dln AOUC" | 100*(dln (PC/(1+ITCR))-dln AOUC)
    <BLANKLINE>
    nb lines: 6
    nb columns: 2
    language: 'ENGLISH'
    gridx: 'MAJOR'
    gridy: 'MAJOR'
    graph_axis: 'VALUES'
    graph_alignment: 'LEFT'
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
    'LEC'
    """
    def __init__(self):
        raise TypeError("This class cannot be instantiated directly.")

    @classmethod
    def get_instance(cls) -> Self:
        instance = cls.__new__(cls)
        instance._cython_instance = CythonTableCell()
        return instance

    @property
    def cell_type(self) -> str:
        r"""
        Type of the content of the cell. 
        Possible value are STRING or LEC.

        Examples
        --------
        >>> from iode import SAMPLE_DATA_DIR
        >>> from iode import tables, Table
        >>> tables.load(f"{SAMPLE_DATA_DIR}/fun.tbl")       # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Loading .../fun.tbl
        46 objects loaded
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
        return self._cython_instance.get_cell_type()

    @property
    def align(self) -> str:
        r"""
        Alignment of the text in the cell.

        Parameters
        ----------
        align : TableCellAlign or str
            The alignment to set for the cell.
            Possible values are CENTER, LEFT, RIGHT or DECIMAL.

        Examples
        --------
        >>> from iode import SAMPLE_DATA_DIR, tables, Table, TableCellAlign
        >>> tables.load(f"{SAMPLE_DATA_DIR}/fun.tbl")       # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Loading .../fun.tbl
        46 objects loaded
        >>> table = tables["ANAPRIX"]
        >>> table[4][0].align
        'LEFT'
        >>> table[4][0].align = TableCellAlign.CENTER
        >>> table[4][0].align
        'CENTER'
        >>> table[4][0].align = "right"
        >>> table[4][0].align
        'RIGHT'
        """
        return self._cython_instance.get_align()

    @align.setter
    def align(self, value: Union[TableCellAlign, str]):
        self._cython_instance.set_align(value)

    @property
    def bold(self) -> bool:
        r"""
        Whether or not the cell text is bold.

        Parameters
        ----------
        value : bool
            True to set the cell as bold, False otherwise.

        Examples
        --------
        >>> from iode import SAMPLE_DATA_DIR
        >>> from iode import tables, Table
        >>> tables.load(f"{SAMPLE_DATA_DIR}/fun.tbl")       # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Loading .../fun.tbl
        46 objects loaded
        >>> table = tables["ANAPRIX"]
        >>> table[4][0].bold
        False
        >>> table[4][0].bold = True
        >>> table[4][0].bold
        True
        """
        return self._cython_instance.get_bold()

    @bold.setter
    def bold(self, value: bool):
        self._cython_instance.set_bold(value)

    @property
    def italic(self) -> bool:
        r"""
        Whether or not the cell text is italic.

        Parameters
        ----------
        value : bool
            True to set the cell as italic, False otherwise.

        Examples
        --------
        >>> from iode import SAMPLE_DATA_DIR
        >>> from iode import tables, Table
        >>> tables.load(f"{SAMPLE_DATA_DIR}/fun.tbl")       # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Loading .../fun.tbl
        46 objects loaded
        >>> table = tables["ANAPRIX"]
        >>> table[4][0].italic
        False
        >>> table[4][0].italic = True
        >>> table[4][0].italic
        True
        """
        return self._cython_instance.get_italic()

    @italic.setter
    def italic(self, value: bool):
        self._cython_instance.set_italic(value)

    @property
    def underline(self) -> bool:
        r"""
        Whether or not the cell text is underline.

        Parameters
        ----------
        value : bool
            True to set the cell as underlined, False otherwise.

        Examples
        --------
        >>> from iode import SAMPLE_DATA_DIR
        >>> from iode import tables, Table
        >>> tables.load(f"{SAMPLE_DATA_DIR}/fun.tbl")       # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Loading .../fun.tbl
        46 objects loaded
        >>> table = tables["ANAPRIX"]
        >>> table[4][0].underline
        False
        >>> table[4][0].underline = True
        >>> table[4][0].underline
        True
        """
        return self._cython_instance.get_underline()

    @underline.setter
    def underline(self, value: bool):
        self._cython_instance.set_underline(value)

    @property
    def coefficients(self) -> List[str]:
        r"""
        Get the list of coefficients (scalars) associated with the LEC expression 
        of a cell of type 'LEC'.

        Returns
        -------
        list(str)

        Examples
        --------
        >>> from iode import SAMPLE_DATA_DIR
        >>> from iode import variables, tables
        >>> variables.load(f"{SAMPLE_DATA_DIR}/fun.var")    # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Loading .../fun.var
        394 objects loaded
        >>> tables.load(f"{SAMPLE_DATA_DIR}/fun.tbl")       # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Loading .../fun.tbl
        46 objects loaded
        >>> table = tables["ANAKNFF"]    
        >>> table[5]
        ('"Output gap "', 'knff1*ln (QAFF_/(Q_F+Q_I))')
        >>> table[5][1].coefficients
        ['knff1']
        """
        if self.cell_type != 'LEC':
            warnings.warn("Cannot get list of variables from a cell which is not of type 'LEC'")
            return []
        else:
            return self._cython_instance.get_coefficients()

    @property
    def variables(self) -> List[str]:
        r"""
        Get the list of variables associated with the LEC expression of a cell of type 'LEC'.

        Returns
        -------
        list(str)

        Examples
        --------
        >>> from iode import SAMPLE_DATA_DIR
        >>> from iode import variables, tables
        >>> variables.load(f"{SAMPLE_DATA_DIR}/fun.var")    # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Loading .../fun.var
        394 objects loaded
        >>> tables.load(f"{SAMPLE_DATA_DIR}/fun.tbl")       # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Loading .../fun.tbl
        46 objects loaded
        >>> table = tables["ANAKNFF"]    
        >>> table[5]
        ('"Output gap "', 'knff1*ln (QAFF_/(Q_F+Q_I))')
        >>> table[5][1].variables
        ['QAFF_', 'Q_F', 'Q_I']
        """
        if self.cell_type != 'LEC':
            warnings.warn("Cannot get list of variables from a cell which is not of type 'LEC'")
            return []
        else:
            return self._cython_instance.get_variables()

    def __str__(self) -> str:
        return self._cython_instance._str_()

    def __repr__(self) -> str:
        return self.__str__()


class TableLine:
    r"""
    IODE Table line.

    The following line types are defined:

        - TITLE: this is a title line. The text encoded in the first column is printed across the entire width of the table. 
                 A LEC form in this type of line makes no sense and is rejected by the editor. 
        - CELL: this is a *normal* table line: the first column contains a line title, the second a LEC form which, 
                once calculated for the requested periods, will provide the values for columns 2 and following in the table. 
                In fact, both columns can be either text or a LEC form, but the point of placing a LEC form in the first 
                column is not obvious. On the other hand, you can place two headings in both columns, or at least leave the 
                second column empty to mark a separation in the table. 
        - SEP: this type of line can contain neither text nor LEC shapes: it's simply a dividing line in the middle of the table.
        - FILES: when printed, this line will contain the names of files containing printed data extending across the entire 
                 width of the table. If several files are printed in comparison, this line is automatically multiplied. 
                 No data (text or LEC) can be encoded in the columns of this line type. 
        - MODE: this is a line which depends on the period operations performed when the table is printed. 
                For example, if a column is printed as a growth rate, a carry forward will be indicated in the line 
                corresponding to MODE. No data can be encoded in columns of this type. 
        - DATE: a line of this type will contain the date on which the table was printed. 

    Attributes
    ----------
    line_type: TableLineType
        Type of the table line. Possible values are TITLE, CELL, SEP, MODE, FILES or DATE.

    graph_type: TableGraphType
        Graph type associated with the table line. Possible values are LINE, SCATTER or BAR.

    axis_left: bool 
        Indicate whether the y-axis should be on the left (True) or on the right (False) of the chart.
        Defaults to True (left).

    See Also
    --------
    TableCell
    Table

    Examples
    --------
    >>> from iode import SAMPLE_DATA_DIR
    >>> from iode import tables, Table
    >>> tables.load(f"{SAMPLE_DATA_DIR}/fun.tbl")       # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
    Loading .../fun.tbl
    46 objects loaded
    >>> table = tables["ANAPRIX"]
    >>> table           # doctest: +NORMALIZE_WHITESPACE
    DIVIS | 1                            |
    TITLE |                        "Analyse des prix"
    ----- | ---------------------------------------------------------------
    CELL  |                              |               "#s"
    ----- | ---------------------------------------------------------------
    CELL  | "GAP_"                       |                             GAP_
    CELL  | "dln (PC/(1+ITCR))-dln AOUC" | 100*(dln (PC/(1+ITCR))-dln AOUC)
    <BLANKLINE>
    nb lines: 6
    nb columns: 2
    language: 'ENGLISH'
    gridx: 'MAJOR'
    gridy: 'MAJOR'
    graph_axis: 'VALUES'
    graph_alignment: 'LEFT'
    <BLANKLINE>

    >>> table[0]
    Analyse des prix
    >>> table[0].line_type
    'TITLE'
    >>> table[0].graph_type
    'LINE'
    >>> table[0].axis_left
    True
    """
    def __init__(self):
        raise TypeError("This class cannot be instantiated directly.")

    @classmethod
    def get_instance(cls) -> Self:
        instance = cls.__new__(cls)
        instance._cython_instance = CythonTableLine()
        return instance

    @property
    def line_type(self) -> str:
        r"""
        Type of the table line.
        Possible values are TITLE, CELL, SEP, MODE, FILES or DATE.

        Returns
        -------
        str

        Examples
        --------
        >>> from iode import SAMPLE_DATA_DIR
        >>> from iode import tables, Table
        >>> tables.load(f"{SAMPLE_DATA_DIR}/fun.tbl")       # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Loading .../fun.tbl
        46 objects loaded
        >>> table = tables["ANAPRIX"]
        >>> table           # doctest: +NORMALIZE_WHITESPACE
        DIVIS | 1                            |
        TITLE |                        "Analyse des prix"
        ----- | ---------------------------------------------------------------
        CELL  |                              |               "#s"
        ----- | ---------------------------------------------------------------
        CELL  | "GAP_"                       |                             GAP_
        CELL  | "dln (PC/(1+ITCR))-dln AOUC" | 100*(dln (PC/(1+ITCR))-dln AOUC)
        <BLANKLINE>
        nb lines: 6
        nb columns: 2
        language: 'ENGLISH'
        gridx: 'MAJOR'
        gridy: 'MAJOR'
        graph_axis: 'VALUES'
        graph_alignment: 'LEFT'
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
        return self._cython_instance.get_type()

    @property
    def graph_type(self) -> str:
        r"""
        Graph type associated with the table line.

        Parameters
        ----------
        value: TableGraphType
            Possible values are LINE, SCATTER or BAR.

        Examples
        --------
        >>> from iode import SAMPLE_DATA_DIR, TableGraphType
        >>> from iode import tables, Table
        >>> tables.load(f"{SAMPLE_DATA_DIR}/fun.tbl")       # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Loading .../fun.tbl
        46 objects loaded
        >>> table = tables["ANAPRIX"]
        >>> table           # doctest: +NORMALIZE_WHITESPACE
        DIVIS | 1                            |
        TITLE |                        "Analyse des prix"
        ----- | ---------------------------------------------------------------
        CELL  |                              |               "#s"
        ----- | ---------------------------------------------------------------
        CELL  | "GAP_"                       |                             GAP_
        CELL  | "dln (PC/(1+ITCR))-dln AOUC" | 100*(dln (PC/(1+ITCR))-dln AOUC)
        <BLANKLINE>
        nb lines: 6
        nb columns: 2
        language: 'ENGLISH'
        gridx: 'MAJOR'
        gridy: 'MAJOR'
        graph_axis: 'VALUES'
        graph_alignment: 'LEFT'
        <BLANKLINE>

        >>> table[4]
        ('"GAP_"', 'GAP_')
        >>> table[4].graph_type
        'LINE'
        >>> table[4].graph_type = "scatter"
        >>> table[4].graph_type
        'SCATTER'
        >>> table[4].graph_type = TableGraphType.BAR
        >>> table[4].graph_type
        'BAR'
        """
        return self._cython_instance.get_graph_type()

    @graph_type.setter
    def graph_type(self, value: Union[TableGraphType, str]):
        if isinstance(value, str):
            value = value.upper()
            value = TableGraphType[value]
        value = int(value)
        self._cython_instance.set_graph_type(value)

    @property
    def axis_left(self) -> bool:
        r"""
        Whether or not the values of the present line correspond to the Y axis displayed on the left of the graph.

        Parameters
        ----------
        value: bool

        Examples
        --------
        >>> from iode import SAMPLE_DATA_DIR
        >>> from iode import tables, Table
        >>> tables.load(f"{SAMPLE_DATA_DIR}/fun.tbl")       # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Loading .../fun.tbl
        46 objects loaded
        >>> table = tables["ANAPRIX"]
        >>> table           # doctest: +NORMALIZE_WHITESPACE
        DIVIS | 1                            |
        TITLE |                        "Analyse des prix"
        ----- | ---------------------------------------------------------------
        CELL  |                              |               "#s"
        ----- | ---------------------------------------------------------------
        CELL  | "GAP_"                       |                             GAP_
        CELL  | "dln (PC/(1+ITCR))-dln AOUC" | 100*(dln (PC/(1+ITCR))-dln AOUC)
        <BLANKLINE>
        nb lines: 6
        nb columns: 2
        language: 'ENGLISH'
        gridx: 'MAJOR'
        gridy: 'MAJOR'
        graph_axis: 'VALUES'
        graph_alignment: 'LEFT'
        <BLANKLINE>

        >>> table[4]
        ('"GAP_"', 'GAP_')
        >>> table[4].axis_left
        True
        >>> table[4].axis_left = False
        >>> table[4].axis_left
        False
        """
        return self._cython_instance.get_axis_left()

    @axis_left.setter
    def axis_left(self, value: bool):
        self._cython_instance.set_axis_left(value)

    def __len__(self) -> int:
        return self._cython_instance.size()

    def _get_col_from_index(self, index: int) -> int:
        self_size = len(self)
        if not (-self_size < index < self_size):
            raise ValueError(f"The index of the cell must be in range [{-self.nb_columns + 1, self.nb_columns - 1}].\n"
                             f"Got value {index} instead.")
        if index < 0:
            index = self_size + index
        return index

    def __getitem__(self, index) -> TableCell:
        r"""
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
        >>> tables.load(f"{SAMPLE_DATA_DIR}/fun.tbl")       # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Loading .../fun.tbl
        46 objects loaded
        >>> table = tables["ANAPRIX"]
        >>> table           # doctest: +NORMALIZE_WHITESPACE
        DIVIS | 1                            |
        TITLE |                        "Analyse des prix"
        ----- | ---------------------------------------------------------------
        CELL  |                              |               "#s"
        ----- | ---------------------------------------------------------------
        CELL  | "GAP_"                       |                             GAP_
        CELL  | "dln (PC/(1+ITCR))-dln AOUC" | 100*(dln (PC/(1+ITCR))-dln AOUC)
        <BLANKLINE>
        nb lines: 6
        nb columns: 2
        language: 'ENGLISH'
        gridx: 'MAJOR'
        gridy: 'MAJOR'
        graph_axis: 'VALUES'
        graph_alignment: 'LEFT'
        <BLANKLINE>

        >>> table[4][0]
        "GAP_"
        >>> table[4][1]
        GAP_
        """
        table_cell = TableCell.get_instance()
        col = self._get_col_from_index(index)
        table_cell._cython_instance = self._cython_instance._getitem_(col, table_cell._cython_instance)
        return table_cell

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
        >>> tables.load(f"{SAMPLE_DATA_DIR}/fun.tbl")       # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Loading .../fun.tbl
        46 objects loaded
        >>> table = tables["ANAPRIX"]
        >>> table           # doctest: +NORMALIZE_WHITESPACE
        DIVIS | 1                            |
        TITLE |                        "Analyse des prix"
        ----- | ---------------------------------------------------------------
        CELL  |                              |               "#s"
        ----- | ---------------------------------------------------------------
        CELL  | "GAP_"                       |                             GAP_
        CELL  | "dln (PC/(1+ITCR))-dln AOUC" | 100*(dln (PC/(1+ITCR))-dln AOUC)
        <BLANKLINE>
        nb lines: 6
        nb columns: 2
        language: 'ENGLISH'
        gridx: 'MAJOR'
        gridy: 'MAJOR'
        graph_axis: 'VALUES'
        graph_alignment: 'LEFT'
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
        col = self._get_col_from_index(index)
        if isinstance(value, TableCell):
            value = str(value)
        if not isinstance(value, str):
            raise TypeError(f"Expected value of type str or TableCell. "
                            f"Got value of type '{type(value).__name__}' instead.")
        self._cython_instance._setitem_(col, value)

    def __delitem__(self, index):
        raise RuntimeError("A Table cell cannot be deleted")

    def __str__(self) -> str:
        return self._cython_instance._str_()

    def __repr__(self) -> str:
        return self.__str__()


class Table:
    r"""
    One of the most frequently performed operations during a simulation exercise 
    is the display of tables of results and charts. 

    Each IODE *table* is a set of lines. A line is composed of two parts (in general):

    - a *text* part, which will be the title of the line
    - a *formula* part, which will allow the calculation of the numerical values 
      to be placed in the *computed table*::

        TABLE TITLE    
        -----------    
        Gross National Product      GNP    
        Unemployment                UL    
        External Balance            X-I

        
    The lines are actually of several types: 

    - *TITLE* lines (centered on the page width), 
    - *CELL* lines (title + formula), 
    - *SEPARATOR* lines
    - *MODE* lines
    - *FILES* lines
    - *DATE* lines

    A table is designed to be "*computed*" over different periods, described by a 
    "*generalized sample*" such as::

        1980Y1:10               --> 10 observations from 1980Y1
        1980Y1, 1985Y1, 1990:5  --> 1980, 1985, then 5 observations from 1990Y1
        80/79:5                 --> 5 growth rates from 1980
        ...


    It can also contain values from different files::

        (1990:5)[1,2,1-2]   --> values from 1990 to 1994 for files
                                1, 2, and for the difference between the two files.


    The *computed table* can be:

    - displayed on screen
    - printed
    - exported as a chart
    - exported to a file (in CSV, HTML, ...)
    - (Python) converted to a Pandas DataFrame or an larray Array

    *Tables* can very well be used in a project that does not include an econometric model: 
    the only information used by tables are *variables* and possibly *scalars*.

    Each line in a table has a set of attributes, the most important of which is its type. 
    This type determines how the line will be printed, and the possible contents of the 
    line definition. The other attributes concern the type of characters to be used when 
    printing and the framing of the line. The table also contains graphic attributes that 
    concern the layout of the graphic that will eventually be built with this table. 

    Attributes
    ----------
    title: str
        The title of the table.
    ymin: float
        Minimum values on the Y axis. If data falls outside these values, the axis scale adapts to the data. 
        The value :math:`NA` can be set for ymin and/or ymax: in this case, the graphics program will calculate 
        an optimum scale value. 
    ymax: float 
        Maximum values on the Y axis. If data falls outside these values, the axis scale adapts to the data. 
        The value :math:`NA` can be set for ymin and/or ymax: in this case, the graphics program will calculate 
        an optimum scale value. 
    gridx: TableGraphGrid
         X-grid options: MAJOR, NONE or MINOR.
    gridy: TableGraphGrid
         Y-grid options: MAJOR, NONE or MINOR.
    graph_axis: TableGraphAxis
         Y-axis scale: VALUES, LOG, SEMILOG or PERCENT.
    graph_alignment: TableGraphAlign
        Graph alignment: LEFT, CENTER or RIGHT.
    box: bool 
        Whether the graph should be boxed. 
    shadow: bool
        Whether or not to place a shadow behind the chart.

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

    See Also
    --------
    TableCell
    TableLine

    Examples
    --------
    >>> from iode import SAMPLE_DATA_DIR
    >>> from iode import Table, comments, lists, variables
    >>> comments.load(f"{SAMPLE_DATA_DIR}/fun.cmt")         # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
    Loading .../fun.cmt
    317 objects loaded 
    >>> lists.load(f"{SAMPLE_DATA_DIR}/fun.lst")            # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
    Loading .../fun.lst
    17 objects loaded 
    >>> variables.load(f"{SAMPLE_DATA_DIR}/fun.var")        # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
    Loading .../fun.var
    394 objects loaded 

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
    language: 'ENGLISH'
    gridx: 'MAJOR'
    gridy: 'MAJOR'
    graph_axis: 'VALUES'
    graph_alignment: 'LEFT'
    <BLANKLINE>

    >>> # left column = line title
    >>> # right column = variable or LEC expression
    >>> table_title = "Table example with titles on the left and LEC expressions on the right"
    >>> # content of the left column
    >>> lines_titles = ["GOSG:", "YDTG:", "DTH:", "DTF:", "IT:", "YSSG+COTRES:", "RIDG:", "OCUG:"]
    >>> # content of the right column
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
    language: 'ENGLISH'
    gridx: 'MAJOR'
    gridy: 'MAJOR'
    graph_axis: 'VALUES'
    graph_alignment: 'LEFT'
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
    language: 'ENGLISH'
    gridx: 'MAJOR'
    gridy: 'MAJOR'
    graph_axis: 'VALUES'
    graph_alignment: 'LEFT'
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
    language: 'ENGLISH'
    gridx: 'MAJOR'
    gridy: 'MAJOR'
    graph_axis: 'VALUES'
    graph_alignment: 'LEFT'
    <BLANKLINE>
    """
    def __init__(self, nb_columns: int=2, table_title: str='', lecs_or_vars: Union[str, List[str]]=None, 
                 lines_titles: List[str]=None, mode: bool=False, files: bool=False, date: bool=False) -> Self:
        if nb_columns < 1:
            raise ValueError(f"'nb_columns': Expected value greater than 0. Got value '{nb_columns}'.")

        if lecs_or_vars is None:
            lecs_or_vars = ""

        if lines_titles is None:
            variables = lecs_or_vars          
            if not isinstance(variables, (str, list, tuple)):
                raise TypeError(f"'lecs_or_vars': Expected value of type str or list/tuple of str. "
                                f"Got value of type '{type(lecs_or_vars).__name__}' instead")
        else:
            if not isinstance(lines_titles, (str, list, tuple)):
                raise TypeError(f"'lines_titles': Expected a value of type str or list/tuple of str. "
                                f"Got value of type '{type(lines_titles).__name__}' instead")
            if isinstance(lines_titles, str):
                lines_titles = [lines_titles]

            lecs = lecs_or_vars
            if not isinstance(lecs, (str, list, tuple)):
                raise TypeError(f"'lecs_or_vars': Expected a value of type str or list of str. "
                                f"Got value of type {type(lines_titles).__name__} instead")
            if isinstance(lecs, str):
                lecs = [lecs]
            
            if len(lines_titles) != len(lecs):
                raise ValueError(f"'lecs_or_vars' and 'lines_titles': The list of LEC expressions (length={len(lecs)}) " 
                                 f"and the list of line titles (length={len(lines_titles)}) must have the same length")
            
        self._cython_instance = CythonTable(nb_columns, table_title, lecs_or_vars, lines_titles, mode, files, date)

    @classmethod
    def get_instance(cls) -> Self:
        instance = cls.__new__(cls)
        instance._cython_instance = CythonTable.__new__(CythonTable)
        return instance

    def update_owner_database(self):
        self._cython_instance.update_owner_database()

    @property
    def nb_lines(self) -> int:
        return self._cython_instance.get_nb_lines()

    @property
    def nb_columns(self) -> int:
        return self._cython_instance.get_nb_columns()

    @property
    def title(self) -> str:
        r"""
        Title of the table.

        Parameters
        ----------
        value: str
            Title of the table.

        Examples
        --------
        >>> from iode import Table, tables, SAMPLE_DATA_DIR
        >>> tables.load(f"{SAMPLE_DATA_DIR}/fun.tbl")       # doctest: +ELLIPSIS
        Loading ...fun.tbl
        46 objects loaded
        >>> table = tables["ANAKNFF"]
        >>> table.title
        'Déterminants de la croissance de K'
        >>> table.title = "New title"
        >>> table.title
        'New title'
        """
        return self._cython_instance.get_title()

    @title.setter
    def title(self, value: str):
        self._cython_instance.set_title(value)

    @property
    def language(self) -> str:
        r"""
        Language.

        Parameters
        ----------
        value : TableLang or str
            Possible values are ENGLISH, DUTCH, FRENCH.

        Examples
        --------
        >>> from iode import Table, TableLang
        >>> table = Table()
        >>> table.language
        'ENGLISH'
        >>> table.language = TableLang.DUTCH
        >>> table.language
        'DUTCH'
        >>> table.language = "French"
        >>> table.language
        'FRENCH'
        """
        return self._cython_instance.get_language()

    @language.setter
    def language(self, value: Union[TableLang, str]):
        self._cython_instance.set_language(value)

    @property
    def ymin(self) -> float:
        r"""
        Minimum values on the Y axis. If data falls outside these values, the axis scale adapts to the data. 
        The value :math:`NA` can be set for ymin and/or ymax: in this case, the graphics program will calculate 
        an optimum scale value. 

        """
        warnings.warn("'ymin' is not yet implemented")

    @ymin.setter
    def ymin(self, value: float):
        warnings.warn("'ymin' is not yet implemented")

    @property
    def ymax(self) -> float:
        r"""
        Maximum values on the Y axis. If data falls outside these values, the axis scale adapts to the data. 
        The value :math:`NA` can be set for ymin and/or ymax: in this case, the graphics program will calculate 
        an optimum scale value. 

        """
        warnings.warn("'ymax' is not yet implemented")

    @ymax.setter
    def ymax(self, value: float):
        warnings.warn("'ymax' is not yet implemented")

    @property
    def gridx(self) -> str:
        r"""
        The gridx value of the table offers a choice of three X-grid options: 
        
            - MAJOR: draws a line across the entire graph at each main axis graduation
            - NONE: removes the grid from the graph.
            - MINOR: draws a line at all graduations 

        Parameters
        ----------
        value: TableGraphGrid or str
            Possible values are MAJOR, NONE or MINOR.

        Examples
        --------
        >>> from iode import Table, TableGraphGrid
        >>> table = Table()
        >>> table.gridx
        'MAJOR'
        >>> table.gridx = TableGraphGrid.NONE
        >>> table.gridx
        'NONE'
        >>> table.gridx = "minor"
        >>> table.gridx
        'MINOR'
        """
        return self._cython_instance.get_gridx()

    @gridx.setter
    def gridx(self, value: Union[TableGraphGrid, str]):
        self._cython_instance.set_gridx(value)

    @property
    def gridy(self) -> str:
        r"""
        The gridy value of the table offers a choice of three Y-grid options: 
        
            - MAJOR: draws a line across the entire graph at each main axis graduation
            - NONE: removes the grid from the graph.
            - MINOR: draws a line at all graduations 

        Parameters
        ----------
        value: TableGraphGrid or str
            Possible values are MAJOR, NONE or MINOR.

        Examples
        --------
        >>> from iode import Table, TableGraphGrid
        >>> table = Table()
        >>> table.gridy
        'MAJOR'
        >>> table.gridy = TableGraphGrid.NONE
        >>> table.gridy
        'NONE'
        >>> table.gridy = "minor"
        >>> table.gridy
        'MINOR'
        """
        return self._cython_instance.get_gridy()

    @gridy.setter
    def gridy(self, value: Union[TableGraphGrid, str]):
        self._cython_instance.set_gridy(value)

    @property
    def graph_axis(self) -> str:
        r"""
        Graph axis of the table allows you to select the type of axis: 
        
            - VALUES : level
            - LOG: logarithmic scale
            - SEMILOG: semi-logarithmic scale 
            - PERCENT: Y scale in percent from 0 to 100

        Parameters
        ----------
        axis : TableGraphAxis or str
            Possible values are VALUES, LOG, SEMILOG or PERCENT.

        Examples
        --------
        >>> from iode import Table, TableGraphAxis
        >>> table = Table()
        >>> table.graph_axis
        'VALUES'
        >>> table.graph_axis = TableGraphAxis.LOG
        >>> table.graph_axis
        'LOG'
        >>> table.graph_axis = "percent"
        >>> table.graph_axis
        'PERCENT'
        """
        return self._cython_instance.get_graph_axis()

    @graph_axis.setter
    def graph_axis(self, value: Union[TableGraphAxis, str]):
        self._cython_instance.set_graph_axis(value)

    @property
    def graph_alignment(self) -> str:
        r"""
        Graph alignment of the table.

        Parameters
        ----------
        value : TableGraphAlign or str
            Possible values are LEFT, CENTER or RIGHT.

        Examples
        --------
        >>> from iode import Table, TableGraphAlign
        >>> table = Table()
        >>> table.graph_alignment
        'LEFT'
        >>> table.graph_alignment = TableGraphAlign.CENTER
        >>> table.graph_alignment
        'CENTER'
        >>> table.graph_alignment = "right"
        >>> table.graph_alignment
        'RIGHT'
        """
        return self._cython_instance.get_graph_alignment()

    @graph_alignment.setter
    def graph_alignment(self, value: Union[TableGraphAlign, str]):
        self._cython_instance.set_graph_alignment(value)

    @property
    def box(self) -> bool:
        r"""
        Whether the graph should be boxed. 
        """
        warnings.warn("'box' is not yet implemented")

    @box.setter
    def box(self, value: bool):
        warnings.warn("'box' is not yet implemented")

    @property
    def shadow(self) -> bool:
        r"""
        Whether or not to place a shadow behind the chart.
        """
        warnings.warn("'shadow' is not yet implemented")

    @shadow.setter
    def shadow(self, value: bool):
        warnings.warn("'shadow' is not yet implemented")

    @property
    def coefficients(self) -> List[str]:
        r"""
        Get the list of coefficients (scalars) associated with all cells 
        of type 'LEC' of the table.

        Returns
        -------
        list(str)

        Examples
        --------
        >>> from iode import SAMPLE_DATA_DIR
        >>> from iode import variables, tables
        >>> variables.load(f"{SAMPLE_DATA_DIR}/fun.var")        # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Loading .../fun.var
        394 objects loaded
        >>> tables.load(f"{SAMPLE_DATA_DIR}/fun.tbl")           # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Loading .../fun.tbl
        46 objects loaded
        >>> table = tables["ANAKNFF"]    
        >>> table                       # doctest: +NORMALIZE_WHITESPACE
        DIVIS |                                  1 |
        TITLE |                "Déterminants de la croissance de K"
        ----- | ------------------------------------------------------------------
        CELL  |                                    |              "#s"
        ----- | ------------------------------------------------------------------
        CELL  | "Croissance de K "                 |                      dln KNFF
        CELL  | "Output gap "                      |    knff1*ln (QAFF_/(Q_F+Q_I))
        CELL  | "Rentabilité "                     |          knf2*ln mavg(3,RENT)
        CELL  | "Croissance anticipée de l'output" | 0.416*mavg(4,dln QAFF_)+0.023
        <BLANKLINE>
        nb lines: 8
        nb columns: 2
        language: 'ENGLISH'
        gridx: 'MAJOR'
        gridy: 'MAJOR'
        graph_axis: 'VALUES'
        graph_alignment: 'LEFT'
        <BLANKLINE>
        >>> table.coefficients
        ['knf2', 'knff1']
        """
        return self._cython_instance.get_coefficients()

    @property
    def variables(self) -> List[str]:
        r"""
        Get the list of variables associated with all cells 
        of the type 'LEC' of the table.

        Returns
        -------
        list(str)

        Examples
        --------
        >>> from iode import SAMPLE_DATA_DIR
        >>> from iode import variables, tables
        >>> variables.load(f"{SAMPLE_DATA_DIR}/fun.var")    # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Loading .../fun.var
        394 objects loaded
        >>> tables.load(f"{SAMPLE_DATA_DIR}/fun.tbl")       # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Loading .../fun.tbl
        46 objects loaded
        >>> table = tables["ANAKNFF"]    
        >>> table                       # doctest: +NORMALIZE_WHITESPACE
        DIVIS |                                  1 |
        TITLE |                "Déterminants de la croissance de K"
        ----- | ------------------------------------------------------------------
        CELL  |                                    |              "#s"
        ----- | ------------------------------------------------------------------
        CELL  | "Croissance de K "                 |                      dln KNFF
        CELL  | "Output gap "                      |    knff1*ln (QAFF_/(Q_F+Q_I))
        CELL  | "Rentabilité "                     |          knf2*ln mavg(3,RENT)
        CELL  | "Croissance anticipée de l'output" | 0.416*mavg(4,dln QAFF_)+0.023
        <BLANKLINE>
        nb lines: 8
        nb columns: 2
        language: 'ENGLISH'
        gridx: 'MAJOR'
        gridy: 'MAJOR'
        graph_axis: 'VALUES'
        graph_alignment: 'LEFT'
        <BLANKLINE>
        >>> table.variables
        ['KNFF', 'QAFF_', 'Q_F', 'Q_I', 'RENT']
        """
        return self._cython_instance.get_variables()

    @property
    def divider(self) -> TableLine:
        r"""
        Returns the divider line of the table. 
        The value of each cell of the divider line is applied to the whole corresponding column as a global divider value.  

        Examples
        --------
        >>> from iode import SAMPLE_DATA_DIR
        >>> from iode import tables
        >>> tables.load(f"{SAMPLE_DATA_DIR}/fun.tbl")       # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Loading .../fun.tbl
        46 objects loaded
        >>> tables["YDH"]           # doctest: +NORMALIZE_WHITESPACE
        DIVIS |                                                          1 |                                    PC_*40.34
        TITLE |                        "Tableau B-3. Revenu disponible des ménages à prix constant"
        ----- | ---------------------------------------------------------------------------------------------------------
        CELL  |                                                            |                     "#S"
        CELL  | "Revenus primaires"                                        |                            WBU_+YN+GOSH_+IDH
        CELL  | "   Masse salariale totale"                                |                                         WBU_
        CELL  | "   Revenu net du travail en provenance du Reste du monde" |                                           YN
        CELL  | "   Surplus brut d'exploitation"                           |                                        GOSH_
        CELL  | "   Revenu net de la propriété"                            |                                          IDH
        CELL  | "Cotisations sociales et impôts"                           |                                  SSF+SSH+DTH
        CELL  | "   Cotisations patronales"                                |                                          SSF
        CELL  | "   Cotisations personnelles"                              |                                          SSH
        CELL  | "IPP"                                                      |                                          DTH
        CELL  | "Prestations sociales "                                    |                                     SBH+OCUH
        CELL  | "   Sécurité sociale"                                      |                                          SBH
        CELL  | "   Diverses prestations"                                  |                                         OCUH
        CELL  | "Total"                                                    | (WBU_+YN+GOSH_+IDH)-(SSF+SSH+DTH)+(SBH+OCUH)
        ----- | ---------------------------------------------------------------------------------------------------------
        FILES |
        DATE  |
        <BLANKLINE>
        nb lines: 19
        nb columns: 2
        language: 'ENGLISH'
        gridx: 'MAJOR'
        gridy: 'MAJOR'
        graph_axis: 'VALUES'
        graph_alignment: 'LEFT'
        <BLANKLINE>

        >>> tables["YDH"].divider
        ('1', 'PC_*40.34')

        >>> tables["YDH"].divider = ["1", "40.34"]
        >>> tables["YDH"].divider
        ('1', '40.34')
        """
        div_line = TableLine.get_instance()
        div_line._cython_instance = self._cython_instance.get_divider(div_line._cython_instance)
        return div_line

    @divider.setter
    def divider(self, value: Union[List[str], Tuple[str]]):
        if not isinstance(value, (list, tuple)):
            raise TypeError(f"Cannot update cells of the 'divider' line. Expected new content of type list "
                            f"or tuple of str.\nGot new content of type {type(value).__name__} instead.")
        if not all(isinstance(item, str) for item in value):
            raise TypeError(f"Cannot update cells of the 'divider' line. One or more items of the passed "
                            f"{type(value).__name__} are not of type str")
        if len(value) != self.nb_columns:
            raise ValueError(f"Cannot update cells of the 'divider' line.\nThe length of the passed "
                             f"{type(value).__name__} ({len(value)}) must be the same of the number of "
                             f"cells ({self.nb_columns}) in the table")
        self._cython_instance.set_divider(value)

    def _get_row_from_index(self, index: int) -> int:
        if not (-len(self) < index < len(self)):
            raise ValueError(f"The index of the line must be in range [{-len(self)+1, len(self)-1}].\n"
                             f"Got value {index} instead.")
        if index < 0:
            index = self.nb_lines + index
        return index

    def index(self, key: str) -> int:
        r"""
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
        >>> from iode import SAMPLE_DATA_DIR
        >>> from iode import Table, comments, lists, variables
        >>> comments.load(f"{SAMPLE_DATA_DIR}/fun.cmt")         # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Loading .../fun.cmt
        317 objects loaded 
        >>> lists.load(f"{SAMPLE_DATA_DIR}/fun.lst")            # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Loading .../fun.lst
        17 objects loaded
        >>> variables.load(f"{SAMPLE_DATA_DIR}/fun.var")        # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Loading .../fun.var
        394 objects loaded
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
        language: 'ENGLISH'
        gridx: 'MAJOR'
        gridy: 'MAJOR'
        graph_axis: 'VALUES'
        graph_alignment: 'LEFT'
        <BLANKLINE>        

        >>> table.index("Table example")
        0
        >>> table.index("GOSG:")
        4
        >>> table.index("OCUG")
        6
        """
        return self._cython_instance.index(key)

    def insert(self, index: int, value: Union[str, List[str], Tuple[str], TableLine, TableLineType]):
        r"""
        Insert a new line in the table.

        Parameters
        ----------
        index: int
            index where to insert a line.
        value: str or list(str) or tuple(str) or TableLine or TableLineType
            value of the new line.
            If TableLineType, 'value' represents the type of the new line: FILES, MODE, DATE or SEP.
            If str, 'value' represents either a separator line if it only contains characters '-' 
            or a title line.
            If an iterable of str, 'value' represents the content of the cells of the new line.

        Examples
        --------
        >>> from iode import SAMPLE_DATA_DIR, TableLineType
        >>> from iode import Table, comments, lists, variables
        >>> comments.load(f"{SAMPLE_DATA_DIR}/fun.cmt")         # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Loading .../fun.cmt
        317 objects loaded
        >>> lists.load(f"{SAMPLE_DATA_DIR}/fun.lst")            # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Loading .../fun.lst
        17 objects loaded
        >>> variables.load(f"{SAMPLE_DATA_DIR}/fun.var")        # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Loading .../fun.var
        394 objects loaded
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
        language: 'ENGLISH'
        gridx: 'MAJOR'
        gridy: 'MAJOR'
        graph_axis: 'VALUES'
        graph_alignment: 'LEFT'
        <BLANKLINE>

        >>> index = table.index("YSSG+COTRES:")
        >>> index
        5

        >>> # insert new separator line
        >>> index += 1
        >>> table.insert(index, '-')

        >>> # insert new title
        >>> index += 1
        >>> table.insert(index, "New Title")

        >>> # insert new separator line 
        >>> index += 1
        >>> table.insert(index, TableLineType.SEP)

        >>> # insert new line with cells
        >>> # "    -> STRING cell
        >>> # no " -> LEC cell
        >>> index += 1
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
        language: 'ENGLISH'
        gridx: 'MAJOR'
        gridy: 'MAJOR'
        graph_axis: 'VALUES'
        graph_alignment: 'LEFT'
        <BLANKLINE>
        """
        row = self._get_row_from_index(index)
        self._cython_instance.insert(row, value)

    def compute(self, generalized_sample: str, extra_files: Union[str, Path, List[str], List[Path]]=None, 
                nb_decimals: int=2, quiet: bool=True) -> ComputedTable:
        r"""
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
            P the periodicity and pp the sub-period (e.g. 1990Y1) 
          - a period can be shifted n periods to the left or right using the operators <n and >n 
          - when used with a zero argument, the shift oprerators have a special meaning: 
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
            It applies to all preceding period definitions.

        The following file operations are possible:

          - absolute value: [1] 
          - difference: [1-2] 
          - difference in percent: [1/2] 
          - sum: [1+2] 
          - average: [1~2] or [1^2]. 

        The file [1] always refers to the current workspace. 
        Extra files (if passed as argument) are numeroted from 2 to 5.

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
        extra_files: str or Path or list(str) or list(Path), optional
            (List of) extra file(s) referenced in the generalized sample.
            Maximum 4 files can be passed as argument.
            The file [1] always refers to the current workspace. 
            Extra files (if passed as argument) are numeroted from 2 to 5.
            Default to None (no extra files).
        nb_decimals: int, optional
            The number of decimals to display.
            Default to 2.
        quiet: bool, optional
            If True, suppresses the logging messages when loading the extra files.
            Default to True.

        Returns
        -------
        ComputedTable

        Examples
        --------
        >>> from pathlib import Path
        >>> from iode import SAMPLE_DATA_DIR
        >>> from iode import Table, tables, variables
        >>> tables.load(f"{SAMPLE_DATA_DIR}/fun.tbl")           # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Loading .../fun.tbl
        46 objects loaded
        >>> variables.load(f"{SAMPLE_DATA_DIR}/fun.var")        # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Loading .../fun.var
        394 objects loaded
        >>> tables["C8_1"]          # doctest: +NORMALIZE_WHITESPACE
        DIVIS | 1                                  |
        TITLE |      "Déterminants de l'output potentiel"
        ----- | ---------------------------------------------
        CELL  |                                    |   "#s"
        ----- | ---------------------------------------------
        CELL  | "Output potentiel"                 |  Q_F+Q_I
        CELL  | "Stock de capital"                 | KNFF[-1]
        CELL  | "Intensité de capital"             |    KLFHP
        CELL  | "Productivité totale des facteurs" |  TFPFHP_
        <BLANKLINE>
        nb lines: 8
        nb columns: 2
        language: 'ENGLISH'
        gridx: 'MAJOR'
        gridy: 'MAJOR'
        graph_axis: 'VALUES'
        graph_alignment: 'LEFT'
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
        Intensité de capital             |     0.39 | -2.17 |     0.38 | -2.05 |     0.37 | -1.91 |     0.36 | -1.86 |     0.36 | -1.90
        Productivité totale des facteurs |     1.10 |  1.00 |     1.11 |  1.00 |     1.12 |  1.00 |     1.13 |  1.00 |     1.14 |  1.00
        <BLANKLINE>

        >>> # simple time series (current workspace + one extra file) - 5 observations - 2 decimals (default)
        >>> sample_data_dir = Path(SAMPLE_DATA_DIR)
        >>> computed_table = tables["C8_1"].compute("2010[1;2]:5", extra_files=sample_data_dir/"ref.av")
        >>> computed_table              # doctest: +NORMALIZE_WHITESPACE
           line title \ period[file]     |  10[1]   |  10[2]   |  11[1]   |  11[2]   |  12[1]   |  12[2]   |  13[1]   |  13[2]   |  14[1]   |  14[2]    
        ----------------------------------------------------------------------------------------------------------------------------------------------- 
        Output potentiel                 |  6936.11 |  6797.39 |  7045.34 |  6904.44 |  7161.54 |  7018.31 |  7302.29 |  7156.24 |  7460.12 |  7310.91  
        Stock de capital                 | 11293.85 | 11067.97 | 11525.01 | 11294.51 | 11736.78 | 11502.05 | 11975.49 | 11735.98 | 12263.95 | 12018.67  
        Intensité de capital             |     0.39 |     0.38 |     0.38 |     0.37 |     0.37 |     0.36 |     0.36 |     0.36 |     0.36 |     0.35  
        Productivité totale des facteurs |     1.10 |     1.08 |     1.11 |     1.09 |     1.12 |     1.10 |     1.13 |     1.11 |     1.14 |     1.12  
        <BLANKLINE>
        >>> computed_table.files            # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        ['...ref.av']

        >>> # simple time series (current workspace + 4 extra files) - 5 observations - 2 decimals (default)
        >>> extra_files = [sample_data_dir / "ref.av", sample_data_dir / "fun.av", 
        ...                sample_data_dir / "fun2.av", sample_data_dir / "a.var"]
        >>> computed_table = tables["C8_1"].compute("2010[1;2;3;4;5]:1", extra_files=extra_files)
        >>> computed_table              # doctest: +NORMALIZE_WHITESPACE
           line title \ period[file]     |  10[1]   |  10[2]   |  10[3]   |  10[4]   | 10[5]         
        -------------------------------------------------------------------------------------
        Output potentiel                 |  6936.11 |  6797.39 |  6936.11 |  6936.11 |   --
        Stock de capital                 | 11293.85 | 11067.97 | 11293.85 | 11293.85 |   --
        Intensité de capital             |     0.39 |     0.38 |     0.39 |     0.39 |   --
        Productivité totale des facteurs |     1.10 |     1.08 |     1.10 |     1.10 |   --
        <BLANKLINE>
        >>> [Path(filepath).name for filepath in computed_table.files]
        ['fun.var', 'ref.av', 'fun.av', 'fun2.av', 'a.var']
        """
        if extra_files is not None:
            load_extra_files(extra_files, quiet)

        computed_table = ComputedTable.get_instance()
        computed_table._cython_instance = self._cython_instance.compute(generalized_sample, nb_decimals)
        if computed_table._cython_instance is None:
            warnings.warn(f"Could not compute the table '{self.title.strip()}' with the generalized sample '{generalized_sample}'.")
            return None
        return computed_table

    def plot(self, title: str=None, plot_type: Union[str, TableGraphType]=TableGraphType.LINE, 
             grid: Union[str, TableGraphGrid]=TableGraphGrid.MAJOR, y_log=False, y_min: float=None, 
             y_max: float=None, legend: bool=True, show: bool=True):
        r"""
        Plot the table using the sample of the variables workspace.
        
        Parameters
        ----------
        title: str, optional
            Title of the plot.
            If None, the title of the table is used.
            Default to None.
        plot_type: str or TableGraphType, optional
            Type of the plot.
            Can be one of 'line', 'scatter', or 'bar'.
            Default to 'line'.
        grid: str or TableGraphGrid, optional
            Type of the grid to be used in the plot.
            Can be one of 'major', 'minor', or 'none'.
            Default to 'major'.
        y_log: bool, optional
            If True, the y-axis will be in logarithmic scale.
            Default to False.
        y_min: float, optional
            Minimum value for the y-axis.
            If None, the minimum value is automatically determined.
            Default to None.
        y_max: float, optional
            Maximum value for the y-axis.
            If None, the maximum value is automatically determined.
            Default to None.
        legend: bool, optional
            whether to show legend. Defaults to True.
        show : bool, optional
            If True, the plot will be displayed immediately. 
            If False, the plot will not be shown until `plt.show()` is called.
            Default is True.
        
        Returns
        -------
        ax: matplotlib.axes.Axes
            The matplotlib Axes object containing the plot.

        Examples
        --------
        >>> from iode import SAMPLE_DATA_DIR, tables, variables
        >>> tables.load(f"{SAMPLE_DATA_DIR}/fun.tbl")       # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Loading .../fun.tbl
        46 objects loaded
        >>> variables.load(f"{SAMPLE_DATA_DIR}/fun.var")    # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Loading .../fun.var
        394 objects loaded
        >>> tables["C8_1"]                                  # doctest: +NORMALIZE_WHITESPACE
        DIVIS | 1                                  |
        TITLE |      "Déterminants de l'output potentiel"
        ----- | ---------------------------------------------
        CELL  |                                    |   "#s"
        ----- | ---------------------------------------------
        CELL  | "Output potentiel"                 |  Q_F+Q_I
        CELL  | "Stock de capital"                 | KNFF[-1]
        CELL  | "Intensité de capital"             |    KLFHP
        CELL  | "Productivité totale des facteurs" |  TFPFHP_
        <BLANKLINE>
        nb lines: 8
        nb columns: 2
        language: 'ENGLISH'
        gridx: 'MAJOR'
        gridy: 'MAJOR'
        graph_axis: 'VALUES'
        graph_alignment: 'LEFT'
        <BLANKLINE>

        >>> ax = tables["C8_1"].plot()                          # doctest: +SKIP
        """
        from iode import variables
        try:
            import matplotlib.pyplot as plt
        except ImportError:
            raise ImportError("Matplotlib is required for plotting. Please install it.")
        
        sample = variables.sample
        generalized_sample = f"{str(sample.start)}:{sample.nb_periods}"
        computed_table = self.compute(generalized_sample)
        if computed_table is None:
            warnings.warn(f"Could not compute the table with title '{self.title.strip()}' for plotting.")
        ax = computed_table.plot(title=title, plot_type=plot_type, grid=grid, y_log=y_log, 
                                 y_min=y_min, y_max=y_max, legend=legend, show=show)
        return ax

    def copy(self) -> Self:
        r"""
        Return a copy of the current Table.

        Examples
        --------
        >>> from iode import SAMPLE_DATA_DIR, tables
        >>> tables.load(f"{SAMPLE_DATA_DIR}/fun.tbl")       # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Loading .../fun.tbl
        46 objects loaded
        >>> tables["C8_1"]          # doctest: +NORMALIZE_WHITESPACE
        DIVIS | 1                                  |
        TITLE |      "Déterminants de l'output potentiel"
        ----- | ---------------------------------------------
        CELL  |                                    |   "#s"
        ----- | ---------------------------------------------
        CELL  | "Output potentiel"                 |  Q_F+Q_I
        CELL  | "Stock de capital"                 | KNFF[-1]
        CELL  | "Intensité de capital"             |    KLFHP
        CELL  | "Productivité totale des facteurs" |  TFPFHP_
        <BLANKLINE>
        nb lines: 8
        nb columns: 2
        language: 'ENGLISH'
        gridx: 'MAJOR'
        gridy: 'MAJOR'
        graph_axis: 'VALUES'
        graph_alignment: 'LEFT'
        <BLANKLINE>
        >>> copied_tbl = tables["C8_1"].copy()
        >>> copied_tbl              # doctest: +NORMALIZE_WHITESPACE
        DIVIS | 1                                  |
        TITLE |      "Déterminants de l'output potentiel"
        ----- | ---------------------------------------------
        CELL  |                                    |   "#s"
        ----- | ---------------------------------------------
        CELL  | "Output potentiel"                 |  Q_F+Q_I
        CELL  | "Stock de capital"                 | KNFF[-1]
        CELL  | "Intensité de capital"             |    KLFHP
        CELL  | "Productivité totale des facteurs" |  TFPFHP_
        <BLANKLINE>
        nb lines: 8
        nb columns: 2
        language: 'ENGLISH'
        gridx: 'MAJOR'
        gridy: 'MAJOR'
        graph_axis: 'VALUES'
        graph_alignment: 'LEFT'
        <BLANKLINE>
        """
        return copy(self)

    def __len__(self) -> int:
        r"""
        Number of lines of the table.

        Equivalent to :py:meth:`Table.nb_lines`.

        Notes
        -----
        The special divider line is not taken into account in the returned number of lines.

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
        language: 'ENGLISH'
        gridx: 'MAJOR'
        gridy: 'MAJOR'
        graph_axis: 'VALUES'
        graph_alignment: 'LEFT'
        <BLANKLINE>

        >>> len(table)
        4
        >>> # equivalent to nb_lines
        >>> table.nb_lines
        4
        """
        return self.nb_lines

    def __getitem__(self, index: int) -> TableLine:
        r"""
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
        >>> comments.load(f"{SAMPLE_DATA_DIR}/fun.cmt")         # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Loading .../fun.cmt
        317 objects loaded 
        >>> lists.load(f"{SAMPLE_DATA_DIR}/fun.lst")            # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Loading .../fun.lst
        17 objects loaded 
        >>> variables.load(f"{SAMPLE_DATA_DIR}/fun.var")        # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Loading .../fun.var
        394 objects loaded
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
        language: 'ENGLISH'
        gridx: 'MAJOR'
        gridy: 'MAJOR'
        graph_axis: 'VALUES'
        graph_alignment: 'LEFT'
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
        table_line = TableLine.get_instance()
        row = self._get_row_from_index(index)
        table_line._cython_instance = self._cython_instance._getitem_(row, table_line._cython_instance)
        return table_line

    def __setitem__(self, index: int, value: Union[str, List[str], Tuple[str], TableLine]):
        r"""
        Update a line of the table.

        Parameters
        ----------
        index: int
            index of the line to be updated.
        value: str or list(str) or tuple(str) or TableLine
            New content for the line. 
        
        Warnings
        --------
        - Line of type 'MODE', 'FILES', 'DATE' and 'SEP' cannot be updated.
        - When updating a cell content, the cell is converted to a cell of type 'STRING' 
          if the new text contains double quotes, otherwise the cell becomes of type 'LEC'.

        Examples
        --------
        >>> from iode import SAMPLE_DATA_DIR
        >>> from iode import Table, comments, lists, variables
        >>> comments.load(f"{SAMPLE_DATA_DIR}/fun.cmt")         # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Loading .../fun.cmt
        317 objects loaded 
        >>> lists.load(f"{SAMPLE_DATA_DIR}/fun.lst")            # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Loading .../fun.lst
        17 objects loaded 
        >>> variables.load(f"{SAMPLE_DATA_DIR}/fun.var")        # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Loading .../fun.var
        394 objects loaded
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
        language: 'ENGLISH'
        gridx: 'MAJOR'
        gridy: 'MAJOR'
        graph_axis: 'VALUES'
        graph_alignment: 'LEFT'
        <BLANKLINE>

        >>> # warning: line of type 'MODE', 'FILES', 'DATE' and 'SEP' cannot be updated.

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
        language: 'ENGLISH'
        gridx: 'MAJOR'
        gridy: 'MAJOR'
        graph_axis: 'VALUES'
        graph_alignment: 'LEFT'
        <BLANKLINE>
        """
        row = self._get_row_from_index(index)
        if isinstance(value, TableLine):
            value = str(value)
        if isinstance(value, str) and '|' in value:
            value = value.split('|')
        self._cython_instance._setitem_(row, value)

    def __delitem__(self, index: int):
        r"""
        Delete the line at the given index

        Parameters
        ----------
        index: int
            index of the line to be removed.

        Examples
        --------
        >>> from iode import SAMPLE_DATA_DIR
        >>> from iode import Table, comments, lists, variables
        >>> comments.load(f"{SAMPLE_DATA_DIR}/fun.cmt")         # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Loading .../fun.cmt
        317 objects loaded 
        >>> lists.load(f"{SAMPLE_DATA_DIR}/fun.lst")            # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Loading .../fun.lst
        17 objects loaded 
        >>> variables.load(f"{SAMPLE_DATA_DIR}/fun.var")        # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Loading .../fun.var
        394 objects loaded
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
        language: 'ENGLISH'
        gridx: 'MAJOR'
        gridy: 'MAJOR'
        graph_axis: 'VALUES'
        graph_alignment: 'LEFT'
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
        language: 'ENGLISH'
        gridx: 'MAJOR'
        gridy: 'MAJOR'
        graph_axis: 'VALUES'
        graph_alignment: 'LEFT'
        <BLANKLINE>        
        """
        row = self._get_row_from_index(index)
        self._cython_instance._delitem_(row)

    def __iadd__(self, value: Union[str, List[str], Tuple[str], TableLineType, TableLine]) -> Self:
        r"""
        Append a new line to the table.

        Parameters
        ----------
        value: str or list(str) or tuple(str) or TableLine or TableLineType
            value of the new line.
            If TableLineType, 'value' represents the type of the new line: FILES, MODE, DATE or SEP.
            If str, 'value' represents either a separator line if it only contains characters '-' 
            or a title line.
            If an iterable of str, 'value' represents the content of the cells of the new line.

        Examples
        --------
        >>> from iode import SAMPLE_DATA_DIR, TableLineType
        >>> from iode import Table, comments, lists, variables
        >>> comments.load(f"{SAMPLE_DATA_DIR}/fun.cmt")         # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Loading .../fun.cmt
        317 objects loaded 
        >>> lists.load(f"{SAMPLE_DATA_DIR}/fun.lst")            # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Loading .../fun.lst
        17 objects loaded 
        >>> variables.load(f"{SAMPLE_DATA_DIR}/fun.var")        # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Loading .../fun.var
        394 objects loaded
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
        language: 'ENGLISH'
        gridx: 'MAJOR'
        gridy: 'MAJOR'
        graph_axis: 'VALUES'
        graph_alignment: 'LEFT'
        <BLANKLINE>

        >>> # append a separator line
        >>> table += '-'

        >>> # append a title
        >>> table += "New Title"

        >>> # append a separator line 
        >>> table += TableLineType.SEP

        >>> # append a line with cells
        >>> # NOTE: line containing double quotes " -> assumed to be a STRING cell
        >>> #       line without double quotes      -> assumed to be a LEC cell
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
        language: 'ENGLISH'
        gridx: 'MAJOR'
        gridy: 'MAJOR'
        graph_axis: 'VALUES'
        graph_alignment: 'LEFT'
        <BLANKLINE>
        """
        if isinstance(value, TableLine):
            value = value._cython_instance
        self._cython_instance = self._cython_instance._iadd_(value)
        return self

    def __copy__(self) -> Self:
        r"""
        Return a copy of the current Table.

        Examples
        --------
        >>> from copy import copy
        >>> from iode import SAMPLE_DATA_DIR, tables
        >>> tables.load(f"{SAMPLE_DATA_DIR}/fun.tbl")       # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Loading .../fun.tbl
        46 objects loaded
        >>> tables["C8_1"]          # doctest: +NORMALIZE_WHITESPACE
        DIVIS | 1                                  |
        TITLE |      "Déterminants de l'output potentiel"
        ----- | ---------------------------------------------
        CELL  |                                    |   "#s"
        ----- | ---------------------------------------------
        CELL  | "Output potentiel"                 |  Q_F+Q_I
        CELL  | "Stock de capital"                 | KNFF[-1]
        CELL  | "Intensité de capital"             |    KLFHP
        CELL  | "Productivité totale des facteurs" |  TFPFHP_
        <BLANKLINE>
        nb lines: 8
        nb columns: 2
        language: 'ENGLISH'
        gridx: 'MAJOR'
        gridy: 'MAJOR'
        graph_axis: 'VALUES'
        graph_alignment: 'LEFT'
        <BLANKLINE>
        >>> copied_tbl = copy(tables["C8_1"])
        >>> copied_tbl              # doctest: +NORMALIZE_WHITESPACE
        DIVIS | 1                                  |
        TITLE |      "Déterminants de l'output potentiel"
        ----- | ---------------------------------------------
        CELL  |                                    |   "#s"
        ----- | ---------------------------------------------
        CELL  | "Output potentiel"                 |  Q_F+Q_I
        CELL  | "Stock de capital"                 | KNFF[-1]
        CELL  | "Intensité de capital"             |    KLFHP
        CELL  | "Productivité totale des facteurs" |  TFPFHP_
        <BLANKLINE>
        nb lines: 8
        nb columns: 2
        language: 'ENGLISH'
        gridx: 'MAJOR'
        gridy: 'MAJOR'
        graph_axis: 'VALUES'
        graph_alignment: 'LEFT'
        <BLANKLINE>
        """
        copied_tbl = Table()
        copied_tbl._cython_instance = self._cython_instance._copy_(copied_tbl._cython_instance)
        return copied_tbl

    def __str__(self) -> str:
        return self._cython_instance._str_()

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

    def __hash__(self) -> int:
        r"""
        Return the hash of the table. 

        Returns
        -------
        int
            The hash value of the table.
        
        Examples
        --------
        >>> from iode import SAMPLE_DATA_DIR, TableLineType
        >>> from iode import Table, comments, lists, variables
        >>> comments.load(f"{SAMPLE_DATA_DIR}/fun.cmt")         # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Loading .../fun.cmt
        317 objects loaded 
        >>> lists.load(f"{SAMPLE_DATA_DIR}/fun.lst")            # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Loading .../fun.lst
        17 objects loaded 
        >>> variables.load(f"{SAMPLE_DATA_DIR}/fun.var")        # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Loading .../fun.var
        394 objects loaded
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
        language: 'ENGLISH'
        gridx: 'MAJOR'
        gridy: 'MAJOR'
        graph_axis: 'VALUES'
        graph_alignment: 'LEFT'
        <BLANKLINE>
        >>> original_hash = hash(table)
        >>> title = str(table[0])
        >>> title
        'Table example'
        >>> # change the title of the table
        >>> table[0] = "New title"
        >>> table[0]
        New title
        >>> original_hash == hash(table)
        False
        >>> #revert changes
        >>> table[0] = title
        >>> original_hash == hash(table)
        True
        """
        return self._cython_instance.__hash__()
