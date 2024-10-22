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
    >>> tables.load(f"{SAMPLE_DATA_DIR}/fun.tbl")
    >>> len(tables)
    46
    >>> tables          # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
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

    def __cinit__(self, filepath: str = None) -> Tables:
        self.database_ptr = NULL
        self.abstract_db_ptr = NULL
        self.ptr_owner = False

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
        return wrapper

    # TODO: implement KDBAbstract::load() method (for global KDB only)
    def _load(self, filepath: str):
        cdef CKDBTables* kdb = new CKDBTables(filepath.encode())
        del kdb

    def _subset(self, pattern: str, copy: bool) -> Tables:
        cdef Tables subset_db = Tables.__new__(Tables)
        subset_db.database_ptr = subset_db.abstract_db_ptr = self.database_ptr.subset(pattern.encode(), <bint>copy)
        return subset_db

    def _get_object(self, key: str):
        key = key.strip()
        cdef CTable* c_table = self.database_ptr.get(key.encode())
        py_table = Table._from_ptr(c_table, <bint>True) 
        return py_table

    def _set_object(self, key, value):
        cdef CTable* c_table
        if not isinstance(key, str):
            raise TypeError(f"Cannot set object {key}.\nExpected a string value for {key} " + 
                            f"but got value of type {type(key).__name__}")
        key = key.strip()

        if self.database_ptr.contains(key.encode()):
            # update existing table
            if not isinstance(value, Table):
                raise TypeError(f"Update table '{key}': Expected input of type 'Table'. "
                                f"Got value of type {type(value).__name__} instead")
            c_table = (<Table>value).c_table
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

    def _str_table(self, names: List[str]) -> str:
        titles = [join_lines(self.database_ptr.get_title(name.encode()).decode()) for name in names]
        columns = {"name": names, "table titles": titles}
        return table2str(columns, max_lines=10, max_width=-1, justify_funcs={"name": JUSTIFY.LEFT, "table titles": JUSTIFY.LEFT})

    def print_to_file(self, destination_file: Union[str, Path], generalized_sample: str, 
                      names: Union[str, Path, List[Union[str, Path]]], nb_decimals: int, format: str = None):
        """
        Compute and print a list of IODE tables to a file.

        Argument `format` must be in the list:
        - 'H' (HTML file)
        - 'M' (MIF file)
        - 'R' (RTF file)
        - 'C' (CSV file)
        - 'D' (DUMMY file)

        If argument `format` is not provided, the A2M format will be used to print the output.

        If the filename does not contain an extension, it is automatically added based on the format.

        Parameters
        ----------
        destination_file: str or Path 
            The destination file path.
        generalized_sample: str
            The generalized sample (see :py:meth:`` for details).
        names: str or list(str) 
            The names of the IODE tables to be printed.
        nb_decimals: int 
            The number of decimals to use in the output.
        format: str, optional 
            The format of the output file. Deduced from the extension if not provided.
            If destination_file has no extension and format is None, the A2M format is used.

        Examples
        --------
        >>> from pathlib import Path
        >>> from iode import SAMPLE_DATA_DIR
        >>> from iode import Table, load_extra_files, tables, variables
        >>> tables.load(f"{SAMPLE_DATA_DIR}/fun.tbl")
        >>> variables.load(f"{SAMPLE_DATA_DIR}/fun.var")

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
        >>> tables.print_to_file(tmp_dir / "tables_2_periods.csv", "(2010;2010/2009):5", names, 2)
        >>> with open(tmp_dir / "tables_2_periods.csv", "r") as f:    # doctest: +NORMALIZE_WHITESPACE
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

        >>> extra_files = load_extra_files(Path(SAMPLE_DATA_DIR) / "ref.av")
        >>> extra_files[0].name
        'ref.av'
        >>> tables.print_to_file(tmp_dir / "tables_2_files.csv", "2010[1-2]:5", names, 2)    
        >>> with open(tmp_dir / "tables_2_files.csv", "r") as f:
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

        if not len(generalized_sample):
            raise ValueError("'generalized_sample' must be a non-empty string.")

        if not isinstance(names, str) and not all(isinstance(name, str) for name in names):
            raise TypeError("'names' must be a string or a list of strings")        
        if isinstance(names, list):
            names = ';'.join(names)
        if not len(names):
            raise ValueError("'names' must be a non-empty string or a non-empty list of strings.")
        
        self.database_ptr.print_to_file(destination_file.encode(), generalized_sample.encode(), 
                                        names.encode(), nb_decimals, c_format)
    # TODO: fix the skiped tests below
    def __hash__(self) -> int:
        """
        Return a hash value for the current Tables database.

        Examples
        --------
        >>> from iode import SAMPLE_DATA_DIR, tables, Table
        >>> tables.load(f"{SAMPLE_DATA_DIR}/fun.tbl")
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
