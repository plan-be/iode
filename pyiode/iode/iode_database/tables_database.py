import sys
from pathlib import Path
from typing import Union, Tuple, List, Dict, Any

if sys.version_info.minor >= 11:
    from typing import Self
else:
    Self = Any

from iode.common import PrintTablesAs
from iode.util import JUSTIFY, table2str, join_lines
from iode.objects.table import Table
from iode.iode_database.abstract_database import IodeDatabase, PositionalIndexer
from iode.iode_cython import Tables as CythonTables


class Tables(IodeDatabase):
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
    def __init__(self, filepath: str=None):
        raise TypeError("This class cannot be instantiated directly.")

    @classmethod
    def get_instance(cls) -> Self:
        instance = cls.__new__(cls)
        instance._cython_instance = CythonTables()
        instance = cls._get_instance(instance)
        return instance

    def _load(self, filepath: str):
        self._cython_instance._load(filepath)

    def _subset(self, pattern: str, copy: bool) -> Self:
        instance = Tables.get_instance()
        instance._cython_instance = self._cython_instance.initialize_subset(instance._cython_instance, pattern, copy)
        instance = self._subset_(instance, copy)
        return instance

    def get_title(self, key: Union[str, int]) -> str:
        r"""
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
        name = self._single_object_key_to_name(key)
        if name not in self:
            raise KeyError(f"Name '{name}' not found in the {type(self).__name__} workspace")
        return self._cython_instance.get_title(name)

    @property
    def i(self) -> PositionalIndexer:
        r"""
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
        CELL  |                                    |              "#s"
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

    def _get_object(self, key: Union[str, int]) -> Table:
        name = self._single_object_key_to_name(key)
        if not name in self:
            raise KeyError(f"Name '{name}' not found in the {type(self).__name__} workspace")
        table = Table.get_instance()
        table._cython_instance = self._cython_instance._get_object(name, table._cython_instance)
        return table

    def _set_object(self, key: Union[str, int], value):
        name = self._single_object_key_to_name(key)

        # update existing table
        if name in self:
            if not isinstance(value, Table):
                raise TypeError(f"Cannot update the table '{name}': Expected input of type 'Table'. "
                                f"Got value of type '{type(value).__name__}' instead")
            table = value
        # add a new table
        else:
            if isinstance(value, int):
                table = Table(nb_columns=value)
            elif isinstance(value, Table):
                table = value
            elif isinstance(value, (tuple, list)):
                table = Table(*value)
            elif isinstance(value, dict):
                table = Table(**value)
            else:
                raise TypeError(f"New table '{name}': Expected input to be of type int or tuple or list or "
                                f"dict or Table. Got value of type {type(value).__name__} instead")
        
        self._cython_instance._set_object(name, table._cython_instance)

    def __getitem__(self, key: Union[str, List[str]]) -> Union[Table, Self]:
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

    def __setitem__(self, key: Union[str, List[str]], value: Union[int, Dict[str, Any], Table, 
        List[Union[int, Dict[str, Any], Table]]]):
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
        value: int, Table, dict(str, ...) or Tables
            If int, then it is interpreted as the number of columns to create a new empty table.
            If Table, then it is used to update an existing table or to create a new table if it does not exist yet.

        See Also
        --------
        iode.Table

        Examples
        --------
        >>> from iode import SAMPLE_DATA_DIR
        >>> from iode import tables, Table, comments, lists, TableGraphAxis
        >>> comments.load(f"{SAMPLE_DATA_DIR}/fun.cmt")     # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Loading .../fun.cmt
        317 objects loaded 
        >>> tables.load(f"{SAMPLE_DATA_DIR}/fun.tbl")       # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Loading .../fun.tbl
        46 objects loaded
        >>> lists.load(f"{SAMPLE_DATA_DIR}/fun.lst")        # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Loading .../fun.lst
        17 objects loaded
        
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
        CELL  |                |     "#S"
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

        >>> # b) -------- update table --------
        >>> tables["TABLE_CELL_LECS"]               # doctest: +NORMALIZE_WHITESPACE
        DIVIS | 1              |
        TITLE |         "New Table"
        ----- | ----------------------------
        CELL  |                |     "#S"
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
        CELL  |         | "#S"
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

        >>> # d) working on a subset
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
        CELL  |                                                         |  "#S"
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
        CELL  |                                                         |  "#S"
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
        CELL  |                                                         |  "#S"
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
        CELL  |                                                         |  "#S"
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

        >>> # d) add/update several equations at once        
        >>> # 1) using a dict of values
        >>> table_C8_1 = tables["C8_1"].copy()
        >>> table_C8_1.title = table_C8_1.title + " (copy)"
        >>> table_C8_2 = tables["C8_2"].copy()
        >>> table_C8_2.title = table_C8_2.title + " (copy)"
        >>> table_C8_3 = tables["C8_3"].copy()
        >>> table_C8_3.title = table_C8_3.title + " (copy)"
        >>> values = {"C8_1": table_C8_1, "C8_2": table_C8_2, "C8_3": table_C8_3}
        >>> tables["C8_1, C8_2, C8_3"] = values
        >>> tables["C8_1, C8_2, C8_3"]      # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Workspace: Tables
        nb tables: 3
        filename: ...fun.tbl
        <BLANKLINE>
        name                       table titles
        C8_1        Déterminants de l'output potentiel (copy)
        C8_2        Déterminants de la productivité (copy)
        C8_3        Output gap (copy)
        <BLANKLINE>

        >>> # 2) using another Tables database (subset)
        >>> tables_subset = tables["C8_1, C8_2, C8_3"].copy()
        >>> tables_subset["C8_1"].title = tables_subset["C8_1"].title.replace("(copy)", "(detached subset)")
        >>> tables_subset["C8_2"].title = tables_subset["C8_2"].title.replace("(copy)", "(detached subset)")
        >>> tables_subset["C8_3"].title = tables_subset["C8_3"].title.replace("(copy)", "(detached subset)")
        >>> tables["C8_1, C8_2, C8_3"] = tables_subset
        >>> tables["C8_1, C8_2, C8_3"]      # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Workspace: Tables
        nb tables: 3
        filename: ...fun.tbl
        <BLANKLINE>
        name                            table titles
        C8_1        Déterminants de l'output potentiel (detached subset)
        C8_2        Déterminants de la productivité (detached subset)
        C8_3        Output gap (detached subset)
        <BLANKLINE>
        """
        super().__setitem__(key, value)

    def __delitem__(self, key):
        r"""
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
        r"""
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
        return super()._coefficients()

    @property
    def variables(self) -> List[str]:
        r"""
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
        return super()._variables()

    def copy_from(self, input_files: Union[str, List[str]], names: Union[str, List[str]]='*'):
        r"""
        Copy (a subset of) tables from the input file(s) 'input_files' into the current database.

        Parameters
        ----------
        input_file: str or list(str)
            file(s) from which the copied tables are read.
        names: str or list(str)
            list of tables to copy from the input file(s).
            Defaults to load all tables from the input file(s). 
        """
        input_files, names = self._copy_from(input_files, names)
        self._cython_instance.copy_from(input_files, names)

    def _str_table(self, names: List[str]) -> str:
        titles = [join_lines(self.get_title(name)) for name in names]
        columns = {"name": names, "table titles": titles}
        justify_funcs={"name": JUSTIFY.LEFT, "table titles": JUSTIFY.LEFT}
        return table2str(columns, max_lines=10, max_width=-1, justify_funcs=justify_funcs)

    @property
    def print_nb_decimals(self) -> int:
        r"""
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
        r"""
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
        i_value = self._cython_instance.get_print_tables_as()
        return PrintTablesAs(i_value)

    @print_tables_as.setter
    def print_tables_as(self, value: Union[PrintTablesAs, str, int]):
        if not isinstance(value, (PrintTablesAs, str, int)):
            raise TypeError(f"Expected value of type 'PrintTablesAs', 'str' or 'int'. "
                            f"Got value of type '{type(value).__name__}' instead.")
        if isinstance(value, int) and not (0 <= value <= 2):
            raise ValueError(f"Invalid value {value}. Expected an integer between 0 and 2.")
        if isinstance(value, str):
            upper_str = value.upper()
            if upper_str not in PrintTablesAs.__members__:
                raise ValueError(f"Invalid value '{value}'. "
                                 f"Expected one of {', '.join(PrintTablesAs.__members__.keys())}. ")
            value = PrintTablesAs[upper_str]
        value = int(value)
        self._cython_instance.set_print_tables_as(value)

    def print_to_file(self, filepath: Union[str, Path], names: Union[str, List[str]]=None, format: str=None, generalized_sample: str=None, nb_decimals: int=4):
        r'''
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
        names: str or list of str, optional
            pattern or list of names of the tables to print.
            If None, print all tables of the (subset of the) current database.
            Defaults to None.
        format: str, optional
            format of the output file. Possible values are: 'H' (HTML file), 
            'M' (MIF file), 'R' (RTF file) or 'C' (CSV file).
            Defaults to None meaning that the tables will be dumped in the *A2M* format.
        generalized_sample: str
            generalized sample to use for computing the tables. 
            Mandatory if `print_tables_as` is `PrintTablesAs.COMPUTED`.
        nb_decimals: int, optional
            number of decimals to use for computing the tables. 
            Only used if `print_tables_as` is `PrintTablesAs.COMPUTED`.
            Defaults to 4.

        Examples
        --------
        >>> from iode import SAMPLE_DATA_DIR
        >>> from iode import tables, scalars, variables, PrintTablesAs, load_extra_files
        >>> output_dir = getfixture('tmp_path')
        >>> scalars.load(f"{SAMPLE_DATA_DIR}/fun.scl")          # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Loading .../fun.scl
        161 objects loaded
        >>> tables.load(f"{SAMPLE_DATA_DIR}/fun.tbl")           # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Loading .../fun.tbl
        46 objects loaded
        >>> variables.load(f"{SAMPLE_DATA_DIR}/fun.var")        # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Loading .../fun.var
        394 objects loaded

        >>> tables["ANAKNFF"]              # doctest: +NORMALIZE_WHITESPACE
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

        >>> tables["ANAPRIX"]              # doctest: +NORMALIZE_WHITESPACE
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

        >>> tables.print_tables_as = PrintTablesAs.TITLES
        >>> tables.print_to_file(output_dir / "tables_titles.csv", ["ANAKNFF", "ANAPRIX"])        # doctest: +ELLIPSIS
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
        >>> tables.print_to_file(output_dir / "tables_full.csv", ["ANAKNFF", "ANAPRIX"])          # doctest: +ELLIPSIS
        Printing IODE objects definition to file '...tables_full.csv'...
        Printing ANAKNFF ...
        Printing ANAPRIX ...
        Print done
        >>> with open(output_dir / "tables_full.csv") as f:                  # doctest: +NORMALIZE_WHITESPACE
        ...     print(f.read())
        ...
        "ANAKNFF : definition",
        <BLANKLINE>
        """Déterminants de la croissance de K""",
        <BLANKLINE>
        " ","""#s""",
        <BLANKLINE>
        """Croissance de K ""","dln KNFF",
        """Output gap ""","knff1*ln (QAFF_/(Q_F+Q_I))",
        """Rentabilité ""","knf2*ln mavg(3,RENT)",
        """Croissance anticipée de l'output""","0.416*mavg(4,dln QAFF_)+0.023",
        <BLANKLINE>
        "ANAPRIX : definition",
        <BLANKLINE>
        """Analyse des prix""",
        <BLANKLINE>
        " ","""#s""",
        <BLANKLINE>
        """GAP_""","GAP_",
        """dln (PC/(1+ITCR))-dln AOUC""","100*(dln (PC/(1+ITCR))-dln AOUC)",
        <BLANKLINE>
        <BLANKLINE>
        
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

        >>> tables.print_tables_as = PrintTablesAs.COMPUTED
        >>> names = ["C8_1", "C8_2", "C8_3", "C8_4"]
        >>> tables.print_to_file(output_dir / "tables_2_periods.csv", names, 
        ...                      generalized_sample="(2010;2010/2009):5", nb_decimals=4)
        >>> with open(output_dir / "tables_2_periods.csv", "r") as f:    # doctest: +NORMALIZE_WHITESPACE
        ...     print(f.read())
        ...
        "Déterminants de l'output potentiel"
        <BLANKLINE>
        " ","10","10/09","11","11/10","12","12/11","13","13/12","14","14/13",
        <BLANKLINE>
        "Output potentiel","6936.1120","1.7386","7045.3431","1.5748","7161.5414","1.6493","7302.2903","1.9653","7460.1153","2.1613",
        "Stock de capital","11293.8486","2.8185","11525.0126","2.0468","11736.7826","1.8375","11975.4885","2.0338","12263.9472","2.4087",
        "Intensité de capital","0.3855","-2.1730","0.3776","-2.0452","0.3704","-1.9140","0.3635","-1.8641","0.3566","-1.8968",
        "Productivité totale des facteurs","1.0977","1.0000","1.1087","1.0000","1.1198","1.0000","1.1310","1.0000","1.1423","1.0000",
        "Déterminants de la productivité"
        <BLANKLINE>
        " ","10","10/09","11","11/10","12","12/11","13","13/12","14","14/13",
        <BLANKLINE>
        "Productivité","2.1305","1.4222","2.1746","2.0711","2.2181","2.0014","2.2593","1.8562","2.2957","1.6105",
        "Intensité de capital","1.3438","0.6834","1.3524","0.6426","1.3606","0.6009","1.3685","0.5850","1.3767","0.5954",
        "TFPFHP_","1.0977","1.0000","1.1087","1.0000","1.1198","1.0000","1.1310","1.0000","1.1423","1.0000",
        "Output gap"
        <BLANKLINE>
        " ","10","10/09","11","11/10","12","12/11","13","13/12","14","14/13",
        <BLANKLINE>
        "Output gap (methodologie FPB).","3.3765","-11.6805","3.5664","5.6238","3.7392","4.8438","3.6566","-2.2090","3.3138","-9.3736",
        "Déterminants de la croissance du stock de capital"
        <BLANKLINE>
        " ","10","10/09","11","11/10","12","12/11","13","13/12","14","14/13",
        <BLANKLINE>
        "Croissnce du stoc de capital","0.0203","-27.1039","0.0182","-10.1346","0.0201","10.5786","0.0238","18.2162","0.0244","2.4760",
        "Rentabilité","-0.0093","34.5565","-0.0100","7.5121","-0.0100","0.3108","-0.0100","0.1740","-0.0103","2.4613",
        <BLANKLINE>

        >>> extra_files = load_extra_files(f"{SAMPLE_DATA_DIR}/ref.av")         # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Loading ...ref.av
        ...
        394 objects loaded
        >>> extra_files[0].name
        'ref.av'
        >>> tables.print_to_file(output_dir / "tables_2_files.csv", names, generalized_sample="2010[1-2]:5", nb_decimals=4)    
        >>> with open(output_dir / "tables_2_files.csv", "r") as f:
        ...     print(f.read())
        ...
        "Déterminants de l'output potentiel"
        <BLANKLINE>
        " ","10[1-2]","11[1-2]","12[1-2]","13[1-2]","14[1-2]",
        <BLANKLINE>
        "Output potentiel","138.7221","140.9067","143.2307","146.0457","149.2022",
        "Stock de capital","225.8768","230.5000","234.7354","239.5095","245.2787",
        "Intensité de capital","0.0077","0.0076","0.0074","0.0073","0.0071",
        "Productivité totale des facteurs","0.0220","0.0222","0.0224","0.0226","0.0228",
        "Déterminants de la productivité"
        <BLANKLINE>
        " ","10[1-2]","11[1-2]","12[1-2]","13[1-2]","14[1-2]",
        <BLANKLINE>
        "Productivité","0.0000","-0.0000","-0.0000","0.0000","0.0000",
        "Intensité de capital","-0.0084","-0.0085","-0.0085","-0.0086","-0.0086",
        "TFPFHP_","0.0220","0.0222","0.0224","0.0226","0.0228",
        "Output gap"
        <BLANKLINE>
        " ","10[1-2]","11[1-2]","12[1-2]","13[1-2]","14[1-2]",
        <BLANKLINE>
        "Output gap (methodologie FPB).","-0.0000","0.0000","0.0000","0.0000","0.0000",
        "Déterminants de la croissance du stock de capital"
        <BLANKLINE>
        " ","10[1-2]","11[1-2]","12[1-2]","13[1-2]","14[1-2]",
        <BLANKLINE>
        "Croissnce du stoc de capital","0.0000","-0.0000","-0.0000","0.0000","-0.0000",
        "Rentabilité","0.0000","-0.0000","-0.0000","-0.0000","0.0000",
        <BLANKLINE>
        '''
        if self.print_tables_as != PrintTablesAs.COMPUTED:
           super().print_to_file(filepath, names, format)
        else:
            if format is not None and not len(format):
                raise ValueError("format must be a non-empty char")

            if isinstance(filepath, str):
                if not len(filepath):
                    raise ValueError("'filepath' must be a non-empty string or a Path object.")
                filepath = Path(filepath)
            if filepath.suffix:
                # [:1] to skip the dot
                format = filepath.suffix[1:]
            filepath = str(filepath.resolve())

            names = ';'.join(self.get_names(names))

            if generalized_sample is None or not len(generalized_sample):
                raise ValueError("'generalized_sample' must be a non-empty string.")
            
            self._cython_instance.cpp_tables_print_to_file(filepath, names, format, generalized_sample, nb_decimals)

    def __hash__(self) -> int:
        r"""
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
        return super().__hash__()


tables: Tables = Tables.get_instance()
