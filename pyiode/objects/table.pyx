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
    >>> from iode import Comments, Lists, Variables, Table
    >>> cmt_db = Comments(f"{SAMPLE_DATA_DIR}/fun.cmt")
    >>> lst_db = Lists(f"{SAMPLE_DATA_DIR}/fun.lst")
    >>> var_db = Variables(f"{SAMPLE_DATA_DIR}/fun.var")

    >>> # empty table
    >>> table = Table()
    >>> table           # doctest: +NORMALIZE_WHITESPACE
    DIVIS | 1 |
    TITLE |
    ----- | --------
    CELL  |   | "#S"
    ----- | --------
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
        f"""
        Language.

        Parameters
        ----------
        lang : int
            Possible values are {LANG_ENGLISH} (LANG_ENGLISH), {LANG_DUTCH} (LANG_DUTCH) and 
            {LANG_FRENCH} (LANG_FRENCH).

        Examples
        --------
        >>> from iode import Table, LANG_ENGLISH, LANG_DUTCH, LANG_FRENCH
        >>> table = Table()
        >>> table.language
        ''
        >>> table.language = LANG_DUTCH
        >>> table.language
        'Dutch'
        """
        return self.c_table.get_language().encode()

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
        >>> from iode import Comments, Lists, Variables, Table
        >>> cmt_db = Comments(f"{SAMPLE_DATA_DIR}/fun.cmt")
        >>> lst_db = Lists(f"{SAMPLE_DATA_DIR}/fun.lst")
        >>> var_db = Variables(f"{SAMPLE_DATA_DIR}/fun.var")
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

    def insert(self, index: int, value: Union[int, str, List[str], Tuple[str]], after: bool = True):
        r"""
        Insert a new line in the table.

        Parameters
        ----------
        index: int
            index where to insert a line.
        value: int or str or list(str) or tuple(str)
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
        >>> from iode import Comments, Lists, Variables, Table
        >>> cmt_db = Comments(f"{SAMPLE_DATA_DIR}/fun.cmt")
        >>> lst_db = Lists(f"{SAMPLE_DATA_DIR}/fun.lst")
        >>> var_db = Variables(f"{SAMPLE_DATA_DIR}/fun.var")
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
            else:
                self.c_table.insert_title(row, value.encode(), <bint>after)
        elif isinstance(value, Iterable) and all(isinstance(item, str) for item in value):
            c_line = self.c_table.insert_line_with_cells(row, <bint>after)
            nb_columns = self.nb_columns
            if len(value) != nb_columns:
                raise ValueError(f"The length of 'value' {len(value)} must be equal to the number of columns {nb_columns}")
            for j in range(nb_columns):
                c_line.get_cell(j, nb_columns).set_content(value[j].encode())
        else:
            raise TypeError(f"The value of 'value' must be of type int, str, list(str) or tuple(str).\n"
                            f"Got value of type {type(value).__name__} instead.")

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

        >>> len(table)
        4
        >>> # equivalent to nb_lines
        >>> table.nb_lines
        4
        """
        return self.nb_lines

    def __getitem__(self, index: int):
        """
        Get a line of the table.

        Parameters
        ----------
        index: int
            index of the line.

        Examples
        --------
        >>> from iode import SAMPLE_DATA_DIR
        >>> from iode import Comments, Lists, Variables, Table
        >>> cmt_db = Comments(f"{SAMPLE_DATA_DIR}/fun.cmt")
        >>> lst_db = Lists(f"{SAMPLE_DATA_DIR}/fun.lst")
        >>> var_db = Variables(f"{SAMPLE_DATA_DIR}/fun.var")
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

        >>> table[0]
        'Table example'
        >>> table[1]
        '---'
        >>> table[4]
        ('"GOSG:"', 'GOSG')
        >>> table[-3]
        '<MODE>'
        >>> table[-2]
        '<FILES>'
        >>> table[-1]
        '<DATE>'
        """
        cdef CTableLine* c_line
        cdef CTableCell* c_cell

        row = self._get_row_from_index(index)
        
        c_line = self.c_table.get_line(row)
        line_type = <int>(c_line.get_line_type())
        nb_columns = self.nb_columns
        if line_type == LINE_TYPE_TITLE:
            content = c_line.get_cell(0, nb_columns).get_content(<bint>False).decode()
            # remove newline characters
            return ''.join(content.splitlines())
        elif line_type == LINE_TYPE_CELL:
            cells_content = []
            for j in range(nb_columns):
                c_cell = c_line.get_cell(j, nb_columns)
                quotes = <int>(c_cell.get_type()) == CELL_TYPE_STRING
                content = c_cell.get_content(<bint>quotes).decode()
                # remove newline characters
                cells_content += [''.join(content.splitlines())]
            return tuple(cells_content)
        elif line_type == LINE_TYPE_LINE:
            return '---'
        else:
            return f"<{LINE_TYPE_DICT[line_type]}>"
    
    def __setitem__(self, index: int, value: Union[str, List[str], Tuple[str]]):
        """
        Update a line of the table.

        Parameters
        ----------
        index: int
            index of the line to be updated.
        value: str or list(str) or tuple(str)
            New content for the line. 
        
        Warnings
        --------
        - Line of type 'MODE', 'FILES', 'DATE' and 'LINE' cannot be updated.
        - When updating a cell content, the cell is converted to a cell of type 'STRING' 
          if the new text contains double quotes, otherwise the cell becomes of type 'LEC'.

        Examples
        --------
        >>> from iode import SAMPLE_DATA_DIR
        >>> from iode import Comments, Lists, Variables, Table
        >>> cmt_db = Comments(f"{SAMPLE_DATA_DIR}/fun.cmt")
        >>> lst_db = Lists(f"{SAMPLE_DATA_DIR}/fun.lst")
        >>> var_db = Variables(f"{SAMPLE_DATA_DIR}/fun.var")
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

        >>> # warning: line of type 'MODE', 'FILES', 'DATE' and 'LINE' cannot be updated.

        >>> # update a title line
        >>> table[0] = "New title"
        >>> table[0]
        'New title'

        >>> # update cell contents
        >>> # warning: the cell is converted to a cell of type 'STRING' if the new text contains 
        >>> # double quotes, otherwise the cell becomes of type 'LEC'.
        >>> table[5] = ('"YSSG:"', 'YSSG')
        >>> table[5]
        ('"YSSG:"', 'YSSG')
        """
        cdef CTableLine* c_line
        cdef CTableCell* c_cell

        row = self._get_row_from_index(index)
        
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
        >>> from iode import Comments, Lists, Variables, Table
        >>> cmt_db = Comments(f"{SAMPLE_DATA_DIR}/fun.cmt")
        >>> lst_db = Lists(f"{SAMPLE_DATA_DIR}/fun.lst")
        >>> var_db = Variables(f"{SAMPLE_DATA_DIR}/fun.var")
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
        """
        row = self._get_row_from_index(index)
        self.c_table.delete_line(row)

    def __iadd__(self, value: Union[int, str, List[str], Tuple[str]]):
        """
        Append a new line to the table.

        Parameters
        ----------
        value: int or str or list(str) or tuple(str)
            value of the new line.\n
            If int, 'value' represents the type of the new line: LINE_TYPE_FILES, LINE_TYPE_MODE, 
            LINE_TYPE_DATE, LINE_TYPE_LINE.\n
            If str, 'value' represents either a separator line if it only contains characters '-' 
            or a title line.
            If an iterable of str, 'value' represents the content of the cells of the new line.\n

        Examples
        --------
        >>> from iode import SAMPLE_DATA_DIR, LINE_TYPE_LINE
        >>> from iode import Comments, Lists, Variables, Table
        >>> cmt_db = Comments(f"{SAMPLE_DATA_DIR}/fun.cmt")
        >>> lst_db = Lists(f"{SAMPLE_DATA_DIR}/fun.lst")
        >>> var_db = Variables(f"{SAMPLE_DATA_DIR}/fun.var")
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
            else:
                self.c_table.add_title(value.encode())
        elif isinstance(value, Iterable) and all(isinstance(item, str) for item in value):
            c_line = self.c_table.add_line_with_cells()
            nb_columns = self.nb_columns
            if len(value) != nb_columns:
                raise ValueError(f"The length of 'value' {len(value)} must be equal to the number of columns {nb_columns}")
            for j in range(nb_columns):
                c_line.get_cell(j, nb_columns).set_content(value[j].encode())
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
        return self.__str__()

