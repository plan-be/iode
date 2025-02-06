# distutils: language = c++

from pathlib import Path
from collections.abc import Iterable
from typing import Union, Tuple, List, Dict, Optional, Any
if sys.version_info.minor >= 11:
    from typing import Self
else:
    Self = Any

cimport cython
from cython.operator cimport dereference
from pyiode.objects.table cimport CTable
from pyiode.iode_database.cpp_api_database cimport hash_value
from pyiode.iode_database.cpp_api_database cimport KDBTables as CKDBTables
from pyiode.iode_database.cpp_api_database cimport Tables as cpp_global_tables
from pyiode.iode_database.cpp_api_database cimport KCPTR, KIPTR, KLPTR, KVPTR
from pyiode.iode_database.cpp_api_database cimport B_TBL_TITLE, B_PrintObjTblTitle

from iode.common import PrintTablesAs


@cython.final
cdef class Tables(_AbstractDatabase):
    r"""
    IODE Tables database. 

    Attributes
    ----------
    filename: str
    description: str

    Parameters
    ----------
    filepath: str, optional
        file containing the IODE tables to load.

    Returns
    -------
    Tables

    Examples
    --------
    >>> from iode import tables, SAMPLE_DATA_DIR
    >>> tables.load(f"{SAMPLE_DATA_DIR}/fun.tbl")      # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
    Loading .../fun.tbl
    46 objects loaded 
    >>> len(tables)
    46
    >>> tables                                          # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
    Workspace: Tables
    nb tables: 46
    filename: ...\tests\data\fun.tbl
    <BLANKLINE>
      name                             table titles
    ANAKNFF     Déterminants de la croissance de K
    ANAPRIX     Analyse des prix
    C8_1        Déterminants de l'output potentiel
    C8_10       Coin salarial parafiscal
    C8_11       Propension moyenne à épargner
    ...         ...
    UCLASS      Chômage classique
    UY          Taux de chômage
    XPC         Inflation: PC
    XQBBP       Croissance
    YDH         Tableau B-3. Revenu disponible des ménages à prix constant
    <BLANKLINE>
    """
    cdef bint ptr_owner
    cdef CKDBTables* database_ptr
    cdef int print_as

    def __cinit__(self, filepath: str = None) -> Tables:
        self.database_ptr = NULL
        self.abstract_db_ptr = NULL
        self.ptr_owner = False
        self.print_as = B_TBL_TITLE

    def __init__(self, filepath: str = None):
        # Prevent accidental instantiation from normal Python code
        # since we cannot pass a struct pointer into a Python constructor.
        raise TypeError("This class cannot be instantiated directly.")

    def __dealloc__(self):
        # if self.database_ptr points to the C++ global instance Tables 
        # which does not need to be manually deleted 
        if self.ptr_owner and self.database_ptr is not NULL:
            del self.database_ptr
            self.database_ptr = NULL

    # see https://cython.readthedocs.io/en/stable/src/userguide/extension_types.html#instantiation-from-existing-c-c-pointers 
    @staticmethod
    cdef Tables _from_ptr(CKDBTables* database_ptr = NULL, bint owner=False):
        # call to __new__() that bypasses the __init__() constructor.
        cdef Tables wrapper = Tables.__new__(Tables)
        if database_ptr is not NULL:
            wrapper.ptr_owner = owner
            wrapper.database_ptr = database_ptr
            wrapper.abstract_db_ptr = database_ptr
        else:
            wrapper.ptr_owner = False
            wrapper.database_ptr = &cpp_global_tables
            wrapper.abstract_db_ptr = &cpp_global_tables
        wrapper.print_as = B_TBL_TITLE
        return wrapper

    # TODO: implement KDBAbstract::load() method (for global KDB only)
    def _load(self, filepath: str):
        cdef CKDBTables* kdb = new CKDBTables(filepath.encode())
        del kdb

    def _subset(self, pattern: str, copy: bool) -> Tables:
        cdef Tables subset_db = Tables.__new__(Tables)
        subset_db.database_ptr = subset_db.abstract_db_ptr = self.database_ptr.subset(pattern.encode(), <bint>copy)
        return subset_db

    def get_title(self, key: Union[str, int]) -> str:
        """
        Get the title of a table from its name or index.

        Parameters
        ----------
        key: str or int
           Name or index of the table to get the title from.
        
        Returns
        -------
        str

        Examples
        --------
        >>> from iode import tables, SAMPLE_DATA_DIR
        >>> tables.load(f"{SAMPLE_DATA_DIR}/fun.tbl")      # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Loading .../fun.tbl
        46 objects loaded
        >>> tables.get_title("ANAKNFF")
        'Déterminants de la croissance de K'
        >>> tables.get_title(0)
        'Déterminants de la croissance de K'
        """
        if isinstance(key, int):
            return self.database_ptr.get_title(<int>key).decode()
        else:
            return self.database_ptr.get_title(<string>(key.encode())).decode()

    @property
    def i(self) -> PositionalIndexer:
        """
        Allow to select the ith table in the database.

        Examples
        --------
        >>> from iode import tables, SAMPLE_DATA_DIR, TableLineType
        >>> tables.load(f"{SAMPLE_DATA_DIR}/fun.tbl")      # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Loading .../fun.tbl
        46 objects loaded
        >>> # get the first table
        >>> tables.i[0]                 # doctest: +NORMALIZE_WHITESPACE
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
        >>> # get the last table
        >>> tables.i[-1]                # doctest: +NORMALIZE_WHITESPACE
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
        >>> # update first table
        >>> table = tables.i[0]
        >>> table += '-'
        >>> table += TableLineType.DATE
        >>> tables.i[0] = table
        >>> tables.i[0]                 # doctest: +NORMALIZE_WHITESPACE
        DIVIS |                                  1 |
        TITLE |                "Déterminants de la croissance de K"
        ----- | ------------------------------------------------------------------
        CELL  | ""                                 |              "#s"
        ----- | ------------------------------------------------------------------
        CELL  | "Croissance de K "                 |                      dln KNFF
        CELL  | "Output gap "                      |    knff1*ln (QAFF_/(Q_F+Q_I))
        CELL  | "Rentabilité "                     |          knf2*ln mavg(3,RENT)
        CELL  | "Croissance anticipée de l'output" | 0.416*mavg(4,dln QAFF_)+0.023
        ----- | ------------------------------------------------------------------
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
        >>> # update last table
        >>> table = tables.i[-1]
        >>> table += TableLineType.MODE
        >>> tables.i[-1] = table
        >>> tables.i[-1]                # doctest: +NORMALIZE_WHITESPACE
        DIVIS |                                                          1 |                                    PC_*40.34
        TITLE |                        "Tableau B-3. Revenu disponible des ménages à prix constant"                             
        ----- | ---------------------------------------------------------------------------------------------------------
        CELL  | ""                                                         |                     "#S"                    
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
        MODE  |
        <BLANKLINE>
        nb lines: 20
        nb columns: 2
        language: 'ENGLISH'
        gridx: 'MAJOR'
        gridy: 'MAJOR'
        graph_axis: 'VALUES'
        graph_alignment: 'LEFT'
        <BLANKLINE>
        """
        return PositionalIndexer(self)

    def _get_object(self, key: Union[str, int]):
        cdef CTable* c_table
        if isinstance(key, int):
            c_table = self.database_ptr.get(<int>key)
            name = self.get_name(<int>key)
        else:
            key = key.strip()
            c_table = self.database_ptr.get(<string>(key.encode()))
            name = key

        py_table = Table._from_ptr(c_table, <bint>True, name.encode(), self.database_ptr) 
        return py_table

    def _set_object(self, key: Union[str, int], value):
        cdef CTable* c_table

        if isinstance(key, str):
            key = key.strip()

        if isinstance(key, int) or self.database_ptr.contains(key.encode()):
            # update existing table
            if not isinstance(value, Table):
                raise TypeError(f"Update table '{key}': Expected input of type 'Table'. "
                                f"Got value of type {type(value).__name__} instead")
            c_table = (<Table>value).c_table
            if isinstance(key, int):
                self.database_ptr.update(<int>key, dereference(c_table))
            else:
                self.database_ptr.update(<string>(key.encode()), dereference(c_table))
        else:
            # add a new table
            if isinstance(value, int):
                table = Table(nb_columns=value)
            elif isinstance(value, Table):
                table = value
            elif isinstance(value, (tuple, list)):
                table = Table(*value)
            elif isinstance(value, dict):
                table = Table(**value)
            else:
                raise TypeError(f"New table '{key}': Expected input to be of type int or tuple or list or "
                                f"dict or Table. Got value of type {type(value).__name__} instead")
            c_table = (<Table>table).c_table
            self.database_ptr.add(<string>(key.encode()), dereference(c_table))

    def __getitem__(self, key: Union[str, List[str]]) -> Union[Table, Tables]:
        r"""
        Return the (subset of) table(s) referenced by `key`.

        The `key` can represent a single object name (e.g. "ACAF") or a list of object names ("ACAF;ACAG;AOUC") 
        or a pattern (e.g. "A*") or a list of sub-patterns (e.g. "A*;*_").
        
        If the `key` represents a list of object names or of sub-patterns, each name or sub-pattern is separated 
        by a `separator` character which is either a whitespace ` `, or a comma `,`, or a semi-colon `;`, or a 
        tabulation `\t`, or a newline `\n`.

        A (sub-)`pattern` is a list of characters representing a group of object names. 
        It includes some special characters which have a special meaning:
        
            - `*` : any character sequence, even empty
            - `?` : any character (one and only one)
            - `@` : any alphanumerical char [A-Za-z0-9]
            - `&` : any non alphanumerical char
            - `|` : any alphanumeric character or none at the beginning and end of a string 
            - `!` : any non-alphanumeric character or none at the beginning and end of a string 
            - `\` : escape the next character

        Note that the `key` can contain references to IODE lists which are prefixed with the symbol `$`.

        Parameters
        ----------
        key: str or list(str)
            (the list of) name(s) of the table(s) to get.
            The list of tables to get can be specified by a pattern or by a list of sub-patterns (e.g. "A*;*_").

        Returns
        -------
        Single table or a subset of the database.

        Examples
        --------
        >>> from iode import SAMPLE_DATA_DIR
        >>> from iode import tables
        >>> tables.load(f"{SAMPLE_DATA_DIR}/fun.tbl")      # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Loading .../fun.tbl
        46 objects loaded

        >>> # a) get one table
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

        >>> # b) get a subset of the Tables database using a pattern
        >>> tables_subset = tables["C8_*"]
        >>> tables_subset.names
        ['C8_1', 'C8_10', 'C8_11', 'C8_13', 'C8_14', 'C8_2', 'C8_3', 'C8_4', 'C8_5', 'C8_6', 'C8_7', 'C8_8', 'C8_9']

        >>> # c) get a subset of the Tables database using a list of names
        >>> tables_subset = tables[["C8_1", "C8_2", "C8_4", "C8_5", "C8_7"]]
        >>> tables_subset.names
        ['C8_1', 'C8_2', 'C8_4', 'C8_5', 'C8_7']
        """
        return super().__getitem__(key)

    def __setitem__(self, key: Union[str, List[str]], value: Union[int, Tuple[...], Dict[str, Any], Table, 
                                                        List[Union[int, Tuple[...], Dict[str, Any], Table]]]):
        r"""
        Update/add a (subset of) table(s) referenced by `key` from/to the Tables database.

        The `key` can represent a single object name (e.g. "ACAF") or a list of object names ("ACAF;ACAG;AOUC") 
        or a pattern (e.g. "A*") or a list of sub-patterns (e.g. "A*;*_").
        
        If the `key` represents a list of object names or of sub-patterns, each name or sub-pattern is separated 
        by a `separator` character which is either a whitespace ` `, or a comma `,`, or a semi-colon `;`, or a 
        tabulation `\t`, or a newline `\n`.

        A (sub-)`pattern` is a list of characters representing a group of object names. 
        It includes some special characters which have a special meaning:
        
            - `*` : any character sequence, even empty
            - `?` : any character (one and only one)
            - `@` : any alphanumerical char [A-Za-z0-9]
            - `&` : any non alphanumerical char
            - `|` : any alphanumeric character or none at the beginning and end of a string 
            - `!` : any non-alphanumeric character or none at the beginning and end of a string 
            - `\` : escape the next character

        Note that the `key` can contain references to IODE lists which are prefixed with the symbol `$`.

        Parameters
        ----------
        key: str or list(str)
            (the list of) name(s) of the table(s) to update/add.
            The list of tables to update/add can be specified by a pattern or by a list of sub-patterns 
            (e.g. "A*;*_").
        value: int, tuple(...), dict(str, ...), Table or list of any of those
            If int, then it is interpreted as the number of columns to create a new empty table.
            If tuple or dictionary, it is forwarded to the Table constructor to create a new table.
            If Table, then it is used to update an existing table or to create a new table if it does not exist yet.

        See Also
        --------
        iode.Table

        Examples
        --------
        >>> from iode import SAMPLE_DATA_DIR
        >>> from iode import tables, Table, comments, TableGraphAxis
        >>> comments.load(f"{SAMPLE_DATA_DIR}/fun.cmt")     # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Loading .../fun.cmt
        317 objects loaded 
        >>> tables.load(f"{SAMPLE_DATA_DIR}/fun.tbl")       # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Loading .../fun.tbl
        46 objects loaded
        
        >>> # a) -------- new table --------
        >>> # 1. specify list of line titles and list of LEC expressions
        >>> lines_titles = ["GOSG:", "YDTG:", "DTH:", "DTF:", "IT:", "YSSG+COTRES:", "RIDG:", "OCUG:"]
        >>> lines_lecs = ["GOSG", "YDTG", "DTH", "DTF", "IT", "YSSG+COTRES", "RIDG", "OCUG"]
        >>> tables["TABLE_CELL_LECS"] = {"nb_columns": 2, "table_title": "New Table", "lecs_or_vars": lines_lecs, 
        ...                              "lines_titles": lines_titles, "mode": True, "files": True, "date": True} 
        >>> tables["TABLE_CELL_LECS"]         # doctest: +NORMALIZE_WHITESPACE
        DIVIS | 1              |
        TITLE |         "New Table"
        ----- | ----------------------------
        CELL  | ""             |     "#S"
        ----- | ----------------------------
        CELL  | "GOSG:"        |        GOSG
        CELL  | "YDTG:"        |        YDTG
        CELL  | "DTH:"         |         DTH
        CELL  | "DTF:"         |         DTF
        CELL  | "IT:"          |          IT
        CELL  | "YSSG+COTRES:" | YSSG+COTRES
        CELL  | "RIDG:"        |        RIDG
        CELL  | "OCUG:"        |        OCUG
        ----- | ----------------------------
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

        >>> # 2. specify list of variables
        >>> vars_list = ["GOSG", "YDTG", "DTH", "DTF", "IT", "YSSG", "COTRES", "RIDG", "OCUG", "$ENVI"]
        >>> tables["TABLE_VARS"] = {"nb_columns": 2, "table_title": "New Table", "lecs_or_vars": vars_list, 
        ...                         "mode": True, "files": True, "date": True}
        >>> tables["TABLE_VARS"]             # doctest: +NORMALIZE_WHITESPACE
        DIVIS | 1                                                                    |
        TITLE |                                  "New Table"
        ----- | ------------------------------------------------------------------------------
        CELL  | ""                                                                   |   "#S"
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

        >>> # b) -------- update table --------
        >>> tables["TABLE_CELL_LECS"]               # doctest: +NORMALIZE_WHITESPACE
        DIVIS | 1              |
        TITLE |         "New Table"
        ----- | ----------------------------
        CELL  | ""             |     "#S"
        ----- | ----------------------------
        CELL  | "GOSG:"        |        GOSG
        CELL  | "YDTG:"        |        YDTG
        CELL  | "DTH:"         |         DTH
        CELL  | "DTF:"         |         DTF
        CELL  | "IT:"          |          IT
        CELL  | "YSSG+COTRES:" | YSSG+COTRES
        CELL  | "RIDG:"        |        RIDG
        CELL  | "OCUG:"        |        OCUG
        ----- | ----------------------------
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

        >>> # set graph axis type
        >>> tables["TABLE_CELL_LECS"].graph_axis = TableGraphAxis.SEMILOG
        >>> # print first line
        >>> tables["TABLE_CELL_LECS"][0]
        New Table
        >>> # print last line
        >>> tables["TABLE_CELL_LECS"][-1]
        <DATE>
        >>> # delete last line
        >>> del tables["TABLE_CELL_LECS"][-1]
        >>> # get index of line containing YSSG+COTRES
        >>> index = tables["TABLE_CELL_LECS"].index("YSSG+COTRES")
        >>> index
        9
        >>> tables["TABLE_CELL_LECS"][index]
        ('"YSSG+COTRES:"', 'YSSG+COTRES')
        >>> # get line type
        >>> tables["TABLE_CELL_LECS"][index].line_type
        'CELL'
        >>> # get line graph type
        >>> tables["TABLE_CELL_LECS"][index].graph_type
        'LINE'
        >>> # know if axis is left
        >>> tables["TABLE_CELL_LECS"][index].axis_left
        True
        >>> # update cells
        >>> # double quotes "    -> STRING cell
        >>> # no double quotes   -> LEC cell
        >>> tables["TABLE_CELL_LECS"][index] = ('"YSSG:"', 'YSSG')
        >>> tables["TABLE_CELL_LECS"][index]
        ('"YSSG:"', 'YSSG')
        >>> # insert a new title line surrounded by two separator lines
        >>> tables["TABLE_CELL_LECS"].insert(index + 1, '-')
        >>> tables["TABLE_CELL_LECS"].insert(index + 2, "New Title")
        >>> tables["TABLE_CELL_LECS"].insert(index + 3, '-')
        >>> # append a new sepatator line
        >>> tables["TABLE_CELL_LECS"] += '-'

        >>> tables["TABLE_CELL_LECS"]                # doctest: +NORMALIZE_WHITESPACE
        DIVIS | 1       |
        TITLE |  "New Table"
        ----- | --------------
        CELL  | ""      | "#S"
        ----- | --------------
        CELL  | "GOSG:" | GOSG
        CELL  | "YDTG:" | YDTG
        CELL  | "DTH:"  |  DTH
        CELL  | "DTF:"  |  DTF
        CELL  | "IT:"   |   IT
        CELL  | "YSSG:" | YSSG
        ----- | --------------
        TITLE |  "New Title"
        ----- | --------------
        CELL  | "RIDG:" | RIDG
        CELL  | "OCUG:" | OCUG
        ----- | --------------
        MODE  |
        FILES |
        ----- | --------------
        <BLANKLINE>
        nb lines: 19
        nb columns: 2
        language: 'ENGLISH'
        gridx: 'MAJOR'
        gridy: 'MAJOR'
        graph_axis: 'SEMILOG'
        graph_alignment: 'LEFT'
        <BLANKLINE>

        >>> # c) working on a subset
        >>> # 1) get subset
        >>> tables_subset = tables["C8_*"]
        >>> tables_subset.names
        ['C8_1', 'C8_10', 'C8_11', 'C8_13', 'C8_14', 'C8_2', 'C8_3', 'C8_4', 'C8_5', 'C8_6', 'C8_7', 'C8_8', 'C8_9']
        >>> # 2) add a table to the subset 
        >>> vars_list = ["XNATY", "XPOIL", "XPWMAB", "XPWXAB"]
        >>> tables_subset["X_GRT"] = {"nb_columns": 2, "table_title": "Croissance", "lecs_or_vars": vars_list, 
        ...                           "mode": True, "files": True, "date": True}  
        >>> tables_subset["X_GRT"]                      # doctest: +NORMALIZE_WHITESPACE
        DIVIS | 1                                                       |
        TITLE |                           "Croissance"
        ----- | ----------------------------------------------------------------
        CELL  | ""                                                      |  "#S"
        ----- | ----------------------------------------------------------------
        CELL  | "Croissance de la population active"                    |  XNATY
        CELL  | "Croissance du prix du pétrole"                         |  XPOIL
        CELL  | "Croissance des prix des biens importés"                | XPWMAB
        CELL  | "Croissance des prix des marchés pertinents à l'export" | XPWXAB
        ----- | ----------------------------------------------------------------
        MODE  |
        FILES |
        DATE  |
        <BLANKLINE>
        nb lines: 12
        nb columns: 2
        language: 'ENGLISH'
        gridx: 'MAJOR'
        gridy: 'MAJOR'
        graph_axis: 'VALUES'
        graph_alignment: 'LEFT'
        <BLANKLINE>
        >>> # --> new table also appears in the global workspace
        >>> "X_GRT" in tables
        True
        >>> tables["X_GRT"]                             # doctest: +NORMALIZE_WHITESPACE
        DIVIS | 1                                                       |
        TITLE |                           "Croissance"
        ----- | ----------------------------------------------------------------
        CELL  | ""                                                      |  "#S"
        ----- | ----------------------------------------------------------------
        CELL  | "Croissance de la population active"                    |  XNATY
        CELL  | "Croissance du prix du pétrole"                         |  XPOIL
        CELL  | "Croissance des prix des biens importés"                | XPWMAB
        CELL  | "Croissance des prix des marchés pertinents à l'export" | XPWXAB
        ----- | ----------------------------------------------------------------
        MODE  |
        FILES |
        DATE  |
        <BLANKLINE>
        nb lines: 12
        nb columns: 2
        language: 'ENGLISH'
        gridx: 'MAJOR'
        gridy: 'MAJOR'
        graph_axis: 'VALUES'
        graph_alignment: 'LEFT'
        <BLANKLINE>
        >>> # 3) update a table in the subset
        >>> table_x_grt = tables_subset["X_GRT"]
        >>> index = table_x_grt.index("XPWXAB")
        >>> table_x_grt.insert(index + 1, (f'"{comments["XQWXSS"]}"', "XQWXSS"))
        >>> tables_subset["X_GRT"]                      # doctest: +NORMALIZE_WHITESPACE
        DIVIS | 1                                                       |
        TITLE |                           "Croissance"
        ----- | ----------------------------------------------------------------
        CELL  | ""                                                      |  "#S"
        ----- | ----------------------------------------------------------------
        CELL  | "Croissance de la population active"                    |  XNATY
        CELL  | "Croissance du prix du pétrole"                         |  XPOIL
        CELL  | "Croissance des prix des biens importés"                | XPWMAB
        CELL  | "Croissance des prix des marchés pertinents à l'export" | XPWXAB
        CELL  | "Croissance des marchés pertinents"                     | XQWXSS
        ----- | ----------------------------------------------------------------
        MODE  |
        FILES |
        DATE  |
        <BLANKLINE>
        nb lines: 13
        nb columns: 2
        language: 'ENGLISH'
        gridx: 'MAJOR'
        gridy: 'MAJOR'
        graph_axis: 'VALUES'
        graph_alignment: 'LEFT'
        <BLANKLINE>
        >>> # --> table is also updated in the global workspace
        >>> tables["X_GRT"]                             # doctest: +NORMALIZE_WHITESPACE
        DIVIS | 1                                                       |
        TITLE |                           "Croissance"
        ----- | ----------------------------------------------------------------
        CELL  | ""                                                      |  "#S"
        ----- | ----------------------------------------------------------------
        CELL  | "Croissance de la population active"                    |  XNATY
        CELL  | "Croissance du prix du pétrole"                         |  XPOIL
        CELL  | "Croissance des prix des biens importés"                | XPWMAB
        CELL  | "Croissance des prix des marchés pertinents à l'export" | XPWXAB
        CELL  | "Croissance des marchés pertinents"                     | XQWXSS
        ----- | ----------------------------------------------------------------
        MODE  |
        FILES |
        DATE  |
        <BLANKLINE>
        nb lines: 13
        nb columns: 2
        language: 'ENGLISH'
        gridx: 'MAJOR'
        gridy: 'MAJOR'
        graph_axis: 'VALUES'
        graph_alignment: 'LEFT'
        <BLANKLINE>
        """
        super().__setitem__(key, value)

    def __delitem__(self, key):
        """
        Remove the (subset of) table(s) referenced by `key` from the Tables database.

        Parameters
        ----------
        key: str or list(str)
            (list of) name(s) of the table(s) to be removed.
            The list of names can be given as a string pattern (e.g. "A*;*_").

        Examples
        --------
        >>> from iode import SAMPLE_DATA_DIR
        >>> from iode import tables
        >>> tables.load(f"{SAMPLE_DATA_DIR}/fun.tbl")      # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Loading .../fun.tbl
        46 objects loaded

        >>> # a) delete one table
        >>> tables.get_names("G*")
        ['GAP', 'GDP', 'GFR', 'GFRLEVEL', 'GFRPC', 'GROWTH']
        >>> del tables["GFRLEVEL"]
        >>> tables.get_names("G*")
        ['GAP', 'GDP', 'GFR', 'GFRPC', 'GROWTH']

        >>> # b) delete several tables at once using a pattern
        >>> del tables["G*"]
        >>> tables.get_names("G*")
        []

        >>> # c) delete several tables at once using a list of names
        >>> tables.get_names("C8_?")
        ['C8_1', 'C8_2', 'C8_3', 'C8_4', 'C8_5', 'C8_6', 'C8_7', 'C8_8', 'C8_9']
        >>> del tables[["C8_1", "C8_3", "C8_5", "C8_7", "C8_9"]]
        >>> tables.get_names("C8_?")
        ['C8_2', 'C8_4', 'C8_6', 'C8_8']

        >>> # delete one table from a subset of the global database
        >>> tables_subset = tables["M*"]
        >>> tables_subset.names
        ['MULT1FR', 'MULT1RESU', 'MULT2FR', 'MULT2RESU']
        >>> del tables_subset["MULT2RESU"]
        >>> tables_subset.names
        ['MULT1FR', 'MULT1RESU', 'MULT2FR']
        >>> # NOTE: the table has also been deleted from the global database
        >>> "MULT2RESU" in tables
        False
        >>> tables.get_names("M*")
        ['MULT1FR', 'MULT1RESU', 'MULT2FR']
        """
        super().__delitem__(key)

    @property
    def coefficients(self) -> List[str]:
        """
        Return the list of coefficients (scalars) used in the tables of 
        (the subset of) the database.
        
        Examples
        --------
        >>> from iode import SAMPLE_DATA_DIR
        >>> from iode import tables
        >>> tables.load(f"{SAMPLE_DATA_DIR}/fun.tbl")      # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Loading .../fun.tbl
        46 objects loaded
        >>> tables["A*"].coefficients
        ['knf2', 'knff1']
        """
        return self._coefficients()
    
    @property
    def variables(self) -> List[str]:
        """
        Return the list of variables used in the tables of (the subset of) the database.

        Examples
        --------
        >>> from iode import SAMPLE_DATA_DIR
        >>> from iode import tables
        >>> tables.load(f"{SAMPLE_DATA_DIR}/fun.tbl")      # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Loading .../fun.tbl
        46 objects loaded
        >>> tables["A*"].variables
        ['AOUC', 'GAP_', 'ITCR', 'KNFF', 'PC', 'QAFF_', 'Q_F', 'Q_I', 'RENT']
        """
        return self._variables()

    def copy_from(self, input_files: Union[str, List[str]], names: Union[str, List[str]]='*'):
        """
        Copy (a subset of) tables from the input file(s) 'input_files' into the current database.

        Parameters
        ----------
        input_file: str or list(str)
            file(s) from which the copied tables are read.
        names: str or list(str)
            list of tables to copy from the input file(s).
            Defaults to load all tables from the input file(s). 
        """
        if not (self.is_global_workspace or self.is_detached):
            raise RuntimeError("Cannot call 'copy_from' method on a subset of a workspace")
        input_files, names = self._copy_from(input_files, names)
        self.database_ptr.copy_from(input_files.encode(), names.encode())

    def _str_table(self, names: List[str]) -> str:
        titles = [join_lines(self.database_ptr.get_title(<string>(name.encode())).decode()) for name in names]
        columns = {"name": names, "table titles": titles}
        return table2str(columns, max_lines=10, max_width=-1, justify_funcs={"name": JUSTIFY.LEFT, "table titles": JUSTIFY.LEFT})

    @property
    def print_nb_decimals(self) -> int:
        """
        Number of decimals to print.

        Parameters
        ----------
        value: int
            number of decimals to print (equations coeffs, scalars and computed tables values).

        Examples
        --------
        >>> from iode import tables
        >>> tables.print_nb_decimals = 4
        >>> tables.print_nb_decimals
        4
        """
        return self._get_print_nb_decimals()

    @print_nb_decimals.setter
    def print_nb_decimals(self, value: int):
        self._set_print_nb_decimals(value)

    @property
    def print_tables_as(self) -> PrintTablesAs:
        """
        Whether to print the full definitions, only the titles or the computed values 
        of the IODE tables in the current database.

        Parameters
        ----------
        value: PrintTablesAs
            Possible values are: PrintTablesAs.FULL, PrintTablesAs.TITLES, 
            PrintTablesAs.COMPUTED

        Examples
        --------
        >>> from iode import tables, PrintTablesAs
        >>> tables.print_tables_as
        <PrintTablesAs.FULL: 0>
        >>> tables.print_tables_as = PrintTablesAs.TITLES
        >>> tables.print_tables_as
        <PrintTablesAs.TITLES: 1>
        >>> tables.print_tables_as = "COMPUTED"
        >>> tables.print_tables_as
        <PrintTablesAs.COMPUTED: 2>
        """
        return PrintTablesAs(self.print_as)

    @print_tables_as.setter
    def print_tables_as(self, value: Union[PrintTablesAs, str]):
        if isinstance(value, str):
            upper_str = value.upper()
            if upper_str not in PrintTablesAs.__members__:
                raise ValueError(f"Invalid value '{value}'. "
                                 f"Expected one of {', '.join(PrintTablesAs.__members__.keys())}. ")
            value = PrintTablesAs[upper_str]
        value = int(value)
        self.print_as = value
        if value <= 1:
            B_PrintObjTblTitle(str(value).encode())

    def print_to_file(self, filepath: Union[str, Path], format: str=None, names: Union[str, List[str]]=None, 
                      generalized_sample: str=None, nb_decimals: int=4) -> None:
        """
        Print the list tables defined by `names` to the file `filepath` using the format `format`.

        Argument `format` must be in the list:
        - 'H' (HTML file)
        - 'M' (MIF file)
        - 'R' (RTF file)
        - 'C' (CSV file)

        If argument `format` is null (default), the *A2M* format will be used to print the output.

        If the filename does not contain an extension, it is automatically added based on 
        the value of `format`.

        If `names` is a string, it is considered as a *pattern* and the function will print 
        all tables matching the pattern. The following characters in *pattern* have a 
        special meaning:
        
            - `*` : any character sequence, even empty
            - `?` : any character (one and only one)
            - `@` : any alphanumerical char [A-Za-z0-9]
            - `&` : any non alphanumerical char
            - `|` : any alphanumeric character or none at the beginning and end of a string 
            - `!` : any non-alphanumeric character or none at the beginning and end of a string 
            - `\` : escape the next character

        If `names` is None, print all tables of the (subset of the) current database.

        Parameters
        ----------
        filepath: str or Path
            path to the file to print.
            If the filename does not contain an extension, it is automatically 
            added based on the value of the format argument.
        format: str, optional
            format of the output file. Possible values are: 'H' (HTML file), 
            'M' (MIF file), 'R' (RTF file) or 'C' (CSV file).
            Defaults to None meaning that the tables will be dumped in the *A2M* format.
        names: str or list of str, optional
            pattern or list of names of the tables to print.
            If None, print all tables of the (subset of the) current database.
            Defaults to None.
        generalized_sample: str
            generalized sample to use for computing the tables. 
            Mandatory if `print_tables_as` is `PrintTablesAs.COMPUTED`.
        nb_decimals: int, optional
            number of decimals to use for computing the tables. 
            Only used if `print_tables_as` is `PrintTablesAs.COMPUTED`.
            Defaults to 4.

        Examples
        --------
        >>> from iode import tables, SAMPLE_DATA_DIR, PrintTablesAs
        >>> tables.load(f"{SAMPLE_DATA_DIR}/fun.tbl")           # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Loading .../fun.tbl
        46 objects loaded

        >>> tables.print_tables_as = PrintTablesAs.TITLES
        >>> tables.print_to_file(output_dir / "tables_titles.csv", names=["ANAKNFF", "ANAPRIX"])        # doctest: +ELLIPSIS
        Printing IODE objects definition to file '...tables_titles.csv'...
        Printing ANAKNFF ...
        Printing ANAPRIX ...
        Print done
        >>> with open(output_dir / "tables_titles.csv") as f:                # doctest: +NORMALIZE_WHITESPACE
        ...     print(f.read())
        ...
        "ANAKNFF : Déterminants de la croissance de K"
        "ANAPRIX : Analyse des prix"

        >>> tables.print_tables_as = PrintTablesAs.FULL
        >>> tables.print_to_file(output_dir / "tables_full.csv", names=["ANAKNFF", "ANAPRIX"])          # doctest: +ELLIPSIS
        Printing IODE objects definition to file '...tables_full.csv'...
        Printing ANAKNFF ...
        Printing ANAPRIX ...
        Print done
        >>> with open(output_dir / "tables_full.csv") as f:                  # doctest: +NORMALIZE_WHITESPACE
        ...     print(f.read())
        ...
        "ANAKNFF : definition",
        <BLANKLINE>
        \"""Déterminants de la croissance de K""\",
        <BLANKLINE>
        " ",\"""#s""\",
        <BLANKLINE>
        \"""Croissance de K ""\","dln KNFF",
        \"""Output gap ""\","knff1*ln (QAFF_/(Q_F+Q_I))",
        \"""Rentabilité ""\","knf2*ln mavg(3,RENT)",
        \"""Croissance anticipée de l'output""\","0.416*mavg(4,dln QAFF_)+0.023",
        <BLANKLINE>
        "ANAPRIX : definition",
        <BLANKLINE>
        \"""Analyse des prix""\",
        <BLANKLINE>
        " ",\"""#s""\",
        <BLANKLINE>
        \"""GAP_""\","GAP_",
        \"""dln (PC/(1+ITCR))-dln AOUC""\","100*(dln (PC/(1+ITCR))-dln AOUC)",

        >>> tables.print_tables_as = PrintTablesAs.COMPUTED
        >>> tables.print_to_file(output_dir / "tables_computed.csv", names=["ANAKNFF", "ANAPRIX"], 
        ...                      generalized_sample="(2010;2010/2009):5", nb_decimals=4)                # doctest: +ELLIPSIS
        >>> with open(output_dir / "tables_computed.csv") as f:                     # doctest: +NORMALIZE_WHITESPACE
        ...     print(f.read())
        ...
        "Déterminants de la croissance de K"
        <BLANKLINE>
        " ","10","10/09","11","11/10","12","12/11","13","13/12","14","14/13",
        <BLANKLINE>
        "Croissance de K","0.0203","-27.1039","0.0182","-10.1346","0.0201","10.5786","0.0238","18.2162","0.0244","2.4760",
        "Output gap","0.0008","-36.5874","0.0019","150.7142","0.0031","59.9968","0.0035","14.1806","0.0030","-13.9993",
        "Rentabilité","-0.0078","34.5565","-0.0084","7.5121","-0.0084","0.3108","-0.0084","0.1740","-0.0086","2.4613",
        "Croissance anticipée de l'output","0.0293","-5.2615","0.0285","-2.9300","0.0293","2.9104","0.0318","8.6515","0.0322","1.2418",
        "Analyse des prix"
        <BLANKLINE>
        " ","10","10/09","11","11/10","12","12/11","13","13/12","14","14/13",
        <BLANKLINE>
        "GAP_","3.3765","-11.6805","3.5664","5.6238","3.7392","4.8438","3.6566","-2.2090","3.3138","-9.3736",
        "dln (PC/(1+ITCR))-dln AOUC","0.0354","-384.6132","-0.1389","-491.8586","-0.0381","-72.5373","0.0422","-210.7056","0.0224","-46.8953",
        <BLANKLINE>
        """
        if self.print_tables_as == PrintTablesAs.COMPUTED:
            self.compute_and_print_to_file(filepath, generalized_sample, names, nb_decimals, format)
        else:
            self._print_to_file(filepath, format, names)

    def compute_and_print_to_file(self, destination_file: Union[str, Path], generalized_sample: str, 
                                  names: Union[str, Path, List[Union[str, Path]]]=None, nb_decimals: int=4, 
                                  format: str = None):
        """
        Compute and print a list of IODE tables to a file.

        Argument `format` must be in the list:
        - 'H' (HTML file)
        - 'M' (MIF file)
        - 'R' (RTF file)
        - 'C' (CSV file)

        If argument `format` is not provided, the A2M format will be used to print the output.

        If the filename does not contain an extension, it is automatically added based on the format.

        Parameters
        ----------
        destination_file: str or Path 
            The destination file path.
        generalized_sample: str
            The generalized sample (see :py:meth:`` for details).
        names: str or list(str), optinal 
            The names of the IODE tables to be printed.
            If None, all tables of the (subset of the) database will be printed.
        nb_decimals: int, optional
            The number of decimals to use in the output.
            Defaults to 4.
        format: str, optional 
            The format of the output file. Deduced from the extension if not provided.
            If destination_file has no extension and format is None, the A2M format is used.

        Examples
        --------
        >>> from pathlib import Path
        >>> from iode import SAMPLE_DATA_DIR
        >>> from iode import Table, load_extra_files, tables, variables
        >>> tables.load(f"{SAMPLE_DATA_DIR}/fun.tbl")           # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Loading .../fun.tbl
        46 objects loaded
        >>> variables.load(f"{SAMPLE_DATA_DIR}/fun.var")        # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Loading .../fun.var
        394 objects loaded

        >>> tables["C8_1"]              # doctest: +NORMALIZE_WHITESPACE
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

        >>> tables["C8_2"]              # doctest: +NORMALIZE_WHITESPACE
        DIVIS | 1                      |
        TITLE |     "Déterminants de la productivité"
        ----- | ------------------------------------------
        CELL  |                        |        "#s"
        ----- | ------------------------------------------
        CELL  | "Productivité"         | (QAFF_/(NFY+NIY))
        CELL  | "Intensité de capital" |    KLFHP**(-0.31)
        CELL  | "TFPFHP_"              |           TFPFHP_
        <BLANKLINE>
        nb lines: 7
        nb columns: 2
        language: 'ENGLISH'
        gridx: 'MAJOR'
        gridy: 'MAJOR'
        graph_axis: 'VALUES'
        graph_alignment: 'LEFT'
        <BLANKLINE>

        >>> tables["C8_3"]              # doctest: +NORMALIZE_WHITESPACE
        DIVIS | 1                                |
        TITLE |               "Output gap"
        ----- | ---------------------------------------
        CELL  |                                  | "#s"
        ----- | ---------------------------------------
        CELL  | "Output gap (methodologie FPB)." | GAP_
        <BLANKLINE>
        nb lines: 5
        nb columns: 2
        language: 'ENGLISH'
        gridx: 'MAJOR'
        gridy: 'MAJOR'
        graph_axis: 'VALUES'
        graph_alignment: 'LEFT'
        <BLANKLINE>

        >>> tables["C8_4"]              # doctest: +NORMALIZE_WHITESPACE
        DIVIS | 1                              |
        TITLE |  "Déterminants de la croissance du stock de capital"
        ----- | -----------------------------------------------------
        CELL  |                                |         "#s"
        ----- | -----------------------------------------------------
        CELL  | "Croissnce du stoc de capital" |             dln KNFF
        CELL  | "Rentabilité"                  | 0.04*ln mavg(3,RENT)
        <BLANKLINE>
        nb lines: 6
        nb columns: 2
        language: 'ENGLISH'
        gridx: 'MAJOR'
        gridy: 'MAJOR'
        graph_axis: 'VALUES'
        graph_alignment: 'LEFT'
        <BLANKLINE>

        >>> names = ["C8_1", "C8_2", "C8_3", "C8_4"]
        >>> tables.compute_and_print_to_file(output_dir / "tables_2_periods.csv", "(2010;2010/2009):5", names, 2)
        >>> with open(output_dir / "tables_2_periods.csv", "r") as f:    # doctest: +NORMALIZE_WHITESPACE
        ...     print(f.read())
        "Déterminants de l'output potentiel"
        <BLANKLINE>
        " ","10","10/09","11","11/10","12","12/11","13","13/12","14","14/13",
        <BLANKLINE>
        "Output potentiel","6936.11","1.74","7045.34","1.57","7161.54","1.65","7302.29","1.97","7460.12","2.16",       
        "Stock de capital","11293.85","2.82","11525.01","2.05","11736.78","1.84","11975.49","2.03","12263.95","2.41",  
        "Intensité de capital","0.39","-2.17","0.38","-2.05","0.37","-1.91","0.36","-1.86","0.36","-1.90",
        "Productivité totale des facteurs","1.10","1.00","1.11","1.00","1.12","1.00","1.13","1.00","1.14","1.00",      
        "Déterminants de la productivité"
        <BLANKLINE>
        " ","10","10/09","11","11/10","12","12/11","13","13/12","14","14/13",
        <BLANKLINE>
        "Productivité","2.13","1.42","2.17","2.07","2.22","2.00","2.26","1.86","2.30","1.61",
        "Intensité de capital","1.34","0.68","1.35","0.64","1.36","0.60","1.37","0.59","1.38","0.60",
        "TFPFHP_","1.10","1.00","1.11","1.00","1.12","1.00","1.13","1.00","1.14","1.00",
        "Output gap"
        <BLANKLINE>
        " ","10","10/09","11","11/10","12","12/11","13","13/12","14","14/13",
        <BLANKLINE>
        "Output gap (methodologie FPB).","3.38","-11.68","3.57","5.62","3.74","4.84","3.66","-2.21","3.31","-9.37",    
        "Déterminants de la croissance du stock de capital"
        <BLANKLINE>
        " ","10","10/09","11","11/10","12","12/11","13","13/12","14","14/13",
        <BLANKLINE>
        "Croissnce du stoc de capital","0.02","-27.10","0.02","-10.13","0.02","10.58","0.02","18.22","0.02","2.48",    
        "Rentabilité","-0.01","34.56","-0.01","7.51","-0.01","0.31","-0.01","0.17","-0.01","2.46",
        <BLANKLINE>

        >>> extra_files = load_extra_files(Path(SAMPLE_DATA_DIR) / "ref.av")        # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Loading ...\\ref.av
        ...
        394 objects loaded
        >>> extra_files[0].name
        'ref.av'
        >>> tables.compute_and_print_to_file(output_dir / "tables_2_files.csv", "2010[1-2]:5", names, 2)    
        >>> with open(output_dir / "tables_2_files.csv", "r") as f:
        ...     print(f.read())
        "Déterminants de l'output potentiel"
        <BLANKLINE>
        " ","10[1-2]","11[1-2]","12[1-2]","13[1-2]","14[1-2]",
        <BLANKLINE>
        "Output potentiel","138.72","140.91","143.23","146.05","149.20",
        "Stock de capital","225.88","230.50","234.74","239.51","245.28",
        "Intensité de capital","0.01","0.01","0.01","0.01","0.01",
        "Productivité totale des facteurs","0.02","0.02","0.02","0.02","0.02",
        "Déterminants de la productivité"
        <BLANKLINE>
        " ","10[1-2]","11[1-2]","12[1-2]","13[1-2]","14[1-2]",
        <BLANKLINE>
        "Productivité","0.00","-0.00","-0.00","0.00","0.00",
        "Intensité de capital","-0.01","-0.01","-0.01","-0.01","-0.01",
        "TFPFHP_","0.02","0.02","0.02","0.02","0.02",
        "Output gap"
        <BLANKLINE>
        " ","10[1-2]","11[1-2]","12[1-2]","13[1-2]","14[1-2]",
        <BLANKLINE>
        "Output gap (methodologie FPB).","-0.00","0.00","0.00","0.00","0.00",
        "Déterminants de la croissance du stock de capital"
        <BLANKLINE>
        " ","10[1-2]","11[1-2]","12[1-2]","13[1-2]","14[1-2]",
        <BLANKLINE>
        "Croissnce du stoc de capital","0.00","-0.00","-0.00","0.00","-0.00",
        "Rentabilité","0.00","-0.00","-0.00","-0.00","0.00",
        <BLANKLINE>
        """
        cdef char c_format = b'\0'
        if format is not None:
            if not len(format):
                raise ValueError("format must be a non-empty char")
            c_format = format.encode('utf-8')[0]

        if isinstance(destination_file, str):
            if not len(destination_file):
                raise ValueError("'destination_file' must be a non-empty string or a Path object.")
            destination_file = Path(destination_file)
        if destination_file.suffix:
            c_format = destination_file.suffix.encode('utf-8')[1]
        destination_file = str(destination_file.resolve())

        if not isinstance(names, str) and not all(isinstance(name, str) for name in names):
            raise TypeError("'names' must be a string or a list of strings")        
        if isinstance(names, list):
            names = ';'.join(names)
        if not len(names):
            raise ValueError("'names' must be a non-empty string or a non-empty list of strings.")
        
        if generalized_sample is None or len(generalized_sample) == 0:
            raise ValueError("'generalized_sample' must be a non-empty string.")

        self.database_ptr.print_to_file(destination_file.encode(), generalized_sample.encode(), 
                                        names.encode(), nb_decimals, c_format)
    
    # TODO: fix the skiped tests below
    def __hash__(self) -> int:
        """
        Return a hash value for the current Tables database.

        Examples
        --------
        >>> from iode import SAMPLE_DATA_DIR, tables, Table
        >>> tables.load(f"{SAMPLE_DATA_DIR}/fun.tbl")      # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Loading .../fun.tbl
        46 objects loaded 
        >>> len(tables)
        46
        >>> original_hash = hash(tables)
        
        >>> # rename 1 table
        >>> tables.rename("ANAKNFF", "ANAKNFF_")
        >>> original_hash == hash(tables)
        False
        >>> tables.rename("ANAKNFF_", "ANAKNFF")  # revert the change
        >>> original_hash == hash(tables)
        True

        >>> # modify one table
        >>> tbl = tables["ANAKNFF"]
        >>> original_title = str(tbl[0])
        >>> tbl[0] = "New title"
        >>> tables["ANAKNFF"] = tbl
        >>> original_hash == hash(tables)
        False
        >>> tbl[0] = original_title  # revert the change
        >>> tables["ANAKNFF"] = tbl
        >>> original_hash == hash(tables)           # doctest: +SKIP
        True

        >>> # delete a table
        >>> original_table = tables["ANAKNFF"]
        >>> del tables["ANAKNFF"]
        >>> original_hash == hash(tables)
        False
        >>> tables["ANAKNFF"] = original_table
        >>> original_hash == hash(tables)           # doctest: +SKIP
        True

        >>> # add a table
        >>> tables["NEW"] = Table()
        >>> original_hash == hash(tables)
        False
        >>> del tables["NEW"]
        >>> original_hash == hash(tables)           # doctest: +SKIP
        True
        """
        return hash_value(dereference(self.database_ptr))


tables: Tables = Tables._from_ptr()
