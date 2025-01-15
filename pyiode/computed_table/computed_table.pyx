# Import necessary types/classes
from libcpp.string cimport string
from libcpp.vector cimport vector

from pyiode.objects.table cimport CTable
from pyiode.common cimport TableGraphAlign as CTableGraphAlign 
from pyiode.common cimport TableGraphAxis as CTableGraphAxis 
from pyiode.common cimport TableGraphGrid as CTableGraphGrid 
from pyiode.common cimport TableGraphType as CTableGraphType
from pyiode.computed_table.computed_table cimport CComputedTable


# ComputedTable wrapper class
# see https://cython.readthedocs.io/en/latest/src/userguide/wrapping_CPlusPlus.html#create-cython-wrapper-class 
# see also https://cython.readthedocs.io/en/stable/src/userguide/extension_types.html#instantiation-from-existing-c-c-pointers 
cdef class ComputedTable:
    """
    Object returned by the method :py:meth:`~iode.Table.compute`. 
    It represents the computation of an IODE table given a generalized sample.

    Examples
    --------
    >>> from iode import SAMPLE_DATA_DIR
    >>> from iode import Table, tables, variables
    >>> tables.load(f"{SAMPLE_DATA_DIR}/fun.tbl")           # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
    Loading .../fun.tbl
    46 objects loaded 
    >>> variables.load(f"{SAMPLE_DATA_DIR}/fun.var")        # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
    Loading .../fun.var
    394 objects loaded
    >>> tables["C8_1"]                                      # doctest: +NORMALIZE_WHITESPACE
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
    >>> # two time series (current workspace) - 5 observations - 2 decimals
    >>> computed_table = tables["C8_1"].compute("(2010;2010/2009):5")
    >>> computed_table              # doctest: +NORMALIZE_WHITESPACE
       line title \ period[file]     |    10    | 10/09 |    11    | 11/10 |    12    | 12/11 |    13    | 13/12 |    14    | 14/13
    --------------------------------------------------------------------------------------------------------------------------------
    Output potentiel                 |  6936.11 |  1.74 |  7045.34 |  1.57 |  7161.54 |  1.65 |  7302.29 |  1.97 |  7460.12 |  2.16
    Stock de capital                 | 11293.85 |  2.82 | 11525.01 |  2.05 | 11736.78 |  1.84 | 11975.49 |  2.03 | 12263.95 |  2.41
    Intensité de capital             |     0.39 | -2.17 |     0.38 | -2.05 |     0.37 | -1.91 |     0.36 | -1.86 |     0.36 |  -1.9
    Productivité totale des facteurs |      1.1 |   1.0 |     1.11 |   1.0 |     1.12 |   1.0 |     1.13 |   1.0 |     1.14 |   1.0
    <BLANKLINE>
    >>> # simple time series (current workspace + one extra file) - 5 observations - 2 decimals
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
    cdef CComputedTable* c_computed_table

    def __cinit__(self):
        self.c_computed_table = NULL

    def __init__(self):
        # Prevent accidental instantiation from normal Python code
        # since we cannot pass a struct pointer into a Python constructor.
        raise TypeError("This class cannot be instantiated directly.")

    def __del__(self):
        if self.c_computed_table is not NULL:
            del self.c_computed_table

    @staticmethod
    cdef ComputedTable initialize(CTable* c_table, string generalized_sample, int nb_decimals):
        # Fast call to __new__() that bypasses the __init__() constructor.
        cdef ComputedTable wrapper = ComputedTable.__new__(ComputedTable)
        wrapper.c_computed_table = new CComputedTable(c_table, generalized_sample, nb_decimals)
        return wrapper

    @property
    def nb_decimals(self) -> int:
        """
        The number of decimals used for rounding the values when displayed.

        Parameters
        ----------
        value: int

        Examples
        --------
        >>> from iode import SAMPLE_DATA_DIR
        >>> from iode import Table, tables, variables
        >>> tables.load(f"{SAMPLE_DATA_DIR}/fun.tbl")           # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Loading .../fun.tbl
        46 objects loaded 
        >>> variables.load(f"{SAMPLE_DATA_DIR}/fun.var")        # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Loading .../fun.var
        394 objects loaded

        >>> # simple time series (current workspace) - 6 observations - 2 decimals (default)
        >>> computed_table = tables["C8_1"].compute("2000:6")
        >>> computed_table              # doctest: +NORMALIZE_WHITESPACE
           line title \ period[file]     |    00   |    01   |    02   |    03   |    04   |    05
        ---------------------------------------------------------------------------------------------
        Output potentiel                 | 5495.21 | 5627.86 | 5748.78 | 5857.95 | 5966.20 | 6103.63
        Stock de capital                 | 8083.55 | 8359.89 | 8647.94 | 8910.34 | 9175.81 | 9468.89
        Intensité de capital             |    0.50 |    0.49 |    0.48 |    0.46 |    0.45 |    0.43
        Productivité totale des facteurs |    0.99 |    1.00 |    1.01 |    1.02 |    1.03 |    1.04
        <BLANKLINE>

        >>> # specify to print 4 decimals  
        >>> computed_table.nb_decimals = 4
        >>> computed_table              # doctest: +NORMALIZE_WHITESPACE
           line title \ period[file]     |     00    |     01    |     02    |     03    |     04    |     05
        ---------------------------------------------------------------------------------------------------------
        Output potentiel                 | 5495.2128 | 5627.8589 | 5748.7804 | 5857.9529 | 5966.1999 | 6103.6318
        Stock de capital                 | 8083.5517 | 8359.8908 | 8647.9354 | 8910.3393 | 9175.8106 | 9468.8865
        Intensité de capital             |    0.5032 |    0.4896 |    0.4758 |    0.4623 |    0.4481 |    0.4349
        Productivité totale des facteurs |    0.9938 |    1.0037 |    1.0137 |    1.0239 |    1.0341 |    1.0445
        <BLANKLINE>
        """
        return self.c_computed_table.get_nb_decimals()

    @nb_decimals.setter
    def nb_decimals(self, value: int):
        self.c_computed_table.set_nb_decimals(value)

    @property
    def nb_lines(self) -> int:
        """
        The number of lines of the computed table.

        Examples
        --------
        >>> from iode import SAMPLE_DATA_DIR
        >>> from iode import Table, tables, variables
        >>> tables.load(f"{SAMPLE_DATA_DIR}/fun.tbl")           # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Loading .../fun.tbl
        46 objects loaded 
        >>> variables.load(f"{SAMPLE_DATA_DIR}/fun.var")        # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Loading .../fun.var
        394 objects loaded

        >>> # simple time series (current workspace) - 6 observations
        >>> computed_table = tables["C8_1"].compute("2000:6")
        >>> computed_table.nb_lines
        4
        >>> computed_table.lines
        ['Output potentiel', 'Stock de capital', 'Intensité de capital', 'Productivité totale des facteurs']
        >>> computed_table              # doctest: +NORMALIZE_WHITESPACE
           line title \ period[file]     |    00   |    01   |    02   |    03   |    04   |    05
        ---------------------------------------------------------------------------------------------
        Output potentiel                 | 5495.21 | 5627.86 | 5748.78 | 5857.95 | 5966.20 | 6103.63
        Stock de capital                 | 8083.55 | 8359.89 | 8647.94 | 8910.34 | 9175.81 | 9468.89
        Intensité de capital             |    0.50 |    0.49 |    0.48 |    0.46 |    0.45 |    0.43
        Productivité totale des facteurs |    0.99 |    1.00 |    1.01 |    1.02 |    1.03 |    1.04
        <BLANKLINE>
        """
        return self.c_computed_table.get_nb_lines()

    @property
    def lines(self) -> List[str]:
        """
        List of line names of the computed table.

        Examples
        --------
        >>> from iode import SAMPLE_DATA_DIR
        >>> from iode import Table, tables, variables
        >>> tables.load(f"{SAMPLE_DATA_DIR}/fun.tbl")           # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Loading .../fun.tbl
        46 objects loaded 
        >>> variables.load(f"{SAMPLE_DATA_DIR}/fun.var")        # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Loading .../fun.var
        394 objects loaded

        >>> # simple time series (current workspace) - 6 observations
        >>> computed_table = tables["C8_1"].compute("2000:6")
        >>> computed_table.nb_lines
        4
        >>> computed_table.lines
        ['Output potentiel', 'Stock de capital', 'Intensité de capital', 'Productivité totale des facteurs']
        >>> computed_table              # doctest: +NORMALIZE_WHITESPACE
           line title \ period[file]     |    00   |    01   |    02   |    03   |    04   |    05
        ---------------------------------------------------------------------------------------------
        Output potentiel                 | 5495.21 | 5627.86 | 5748.78 | 5857.95 | 5966.20 | 6103.63
        Stock de capital                 | 8083.55 | 8359.89 | 8647.94 | 8910.34 | 9175.81 | 9468.89
        Intensité de capital             |    0.50 |    0.49 |    0.48 |    0.46 |    0.45 |    0.43
        Productivité totale des facteurs |    0.99 |    1.00 |    1.01 |    1.02 |    1.03 |    1.04
        <BLANKLINE>
        """
        return [self.c_computed_table.get_line_name(i).decode() for i in range(self.nb_lines)]

    @property
    def nb_columns(self) -> int:
        """
        The number of columns of the computed table.

        Examples
        --------
        >>> from iode import SAMPLE_DATA_DIR
        >>> from iode import Table, tables, variables
        >>> tables.load(f"{SAMPLE_DATA_DIR}/fun.tbl")           # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Loading .../fun.tbl
        46 objects loaded 
        >>> variables.load(f"{SAMPLE_DATA_DIR}/fun.var")        # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Loading .../fun.var
        394 objects loaded

        >>> # two time series (current workspace) - 5 observations
        >>> computed_table = tables["C8_1"].compute("(2010;2010/2009):5")
        >>> computed_table.nb_columns
        10
        >>> computed_table.columns
        ['10', '10/09', '11', '11/10', '12', '12/11', '13', '13/12', '14', '14/13']
        >>> computed_table              # doctest: +NORMALIZE_WHITESPACE
           line title \ period[file]     |    10    | 10/09 |    11    | 11/10 |    12    | 12/11 |    13    | 13/12 |    14    | 14/13
        --------------------------------------------------------------------------------------------------------------------------------
        Output potentiel                 |  6936.11 |  1.74 |  7045.34 |  1.57 |  7161.54 |  1.65 |  7302.29 |  1.97 |  7460.12 |  2.16
        Stock de capital                 | 11293.85 |  2.82 | 11525.01 |  2.05 | 11736.78 |  1.84 | 11975.49 |  2.03 | 12263.95 |  2.41
        Intensité de capital             |     0.39 | -2.17 |     0.38 | -2.05 |     0.37 | -1.91 |     0.36 | -1.86 |     0.36 | -1.90
        Productivité totale des facteurs |     1.10 |  1.00 |     1.11 |  1.00 |     1.12 |  1.00 |     1.13 |  1.00 |     1.14 |  1.00
        <BLANKLINE>
        """
        return self.c_computed_table.get_nb_columns()

    @property
    def columns(self) -> List[str]:
        """
        List of column names of the computed table.

        Examples
        --------
        >>> from iode import SAMPLE_DATA_DIR
        >>> from iode import Table, tables, variables
        >>> tables.load(f"{SAMPLE_DATA_DIR}/fun.tbl")           # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Loading .../fun.tbl
        46 objects loaded 
        >>> variables.load(f"{SAMPLE_DATA_DIR}/fun.var")        # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Loading .../fun.var
        394 objects loaded

        >>> # two time series (current workspace) - 5 observations
        >>> computed_table = tables["C8_1"].compute("(2010;2010/2009):5")
        >>> computed_table.nb_columns
        10
        >>> computed_table.columns
        ['10', '10/09', '11', '11/10', '12', '12/11', '13', '13/12', '14', '14/13']
        >>> computed_table              # doctest: +NORMALIZE_WHITESPACE
           line title \ period[file]     |    10    | 10/09 |    11    | 11/10 |    12    | 12/11 |    13    | 13/12 |    14    | 14/13
        --------------------------------------------------------------------------------------------------------------------------------
        Output potentiel                 |  6936.11 |  1.74 |  7045.34 |  1.57 |  7161.54 |  1.65 |  7302.29 |  1.97 |  7460.12 |  2.16
        Stock de capital                 | 11293.85 |  2.82 | 11525.01 |  2.05 | 11736.78 |  1.84 | 11975.49 |  2.03 | 12263.95 |  2.41
        Intensité de capital             |     0.39 | -2.17 |     0.38 | -2.05 |     0.37 | -1.91 |     0.36 | -1.86 |     0.36 | -1.90
        Productivité totale des facteurs |     1.10 |  1.00 |     1.11 |  1.00 |     1.12 |  1.00 |     1.13 |  1.00 |     1.14 |  1.00
        <BLANKLINE>
        """
        return [self.c_computed_table.get_column_name(j).decode() for j in range(self.nb_columns)]

    @property
    def nb_files(self) -> int:
        """
        The number of files used to compute the table.

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

        >>> # simple time series (current workspace + one extra file) - 5 observations
        >>> computed_table = tables["C8_1"].compute("2010[1-2]:5", extra_files=f"{SAMPLE_DATA_DIR}/ref.av")     # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Loading ...\\ref.av
        ...
        394 objects loaded
        >>> computed_table.nb_files
        2
        >>> for filepath in computed_table.files:
        ...     print(Path(filepath).name)
        fun.var
        ref.av
        >>> computed_table.nb_operations_between_files
        1
        >>> computed_table              # doctest: +NORMALIZE_WHITESPACE
           line title \ period[file]     | 10[1-2] | 11[1-2] | 12[1-2] | 13[1-2] | 14[1-2]
        -----------------------------------------------------------------------------------
        Output potentiel                 |  138.72 |  140.91 |  143.23 |  146.05 |  149.20
        Stock de capital                 |  225.88 |  230.50 |  234.74 |  239.51 |  245.28
        Intensité de capital             |    0.01 |    0.01 |    0.01 |    0.01 |    0.01
        Productivité totale des facteurs |    0.02 |    0.02 |    0.02 |    0.02 |    0.02
        <BLANKLINE>
        """
        return self.c_computed_table.get_nb_files()

    @property
    def files(self) -> List[str]:
        """
        The list of files associated with the computed table.

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

        >>> # simple time series (current workspace + one extra file) - 5 observations
        >>> computed_table = tables["C8_1"].compute("2010[1-2]:5", extra_files=f"{SAMPLE_DATA_DIR}/ref.av")     # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Loading ...\\ref.av
        ...
        394 objects loaded
        >>> computed_table.nb_files
        2
        >>> for filepath in computed_table.files:
        ...     print(Path(filepath).name)
        fun.var
        ref.av
        >>> computed_table.nb_operations_between_files
        1
        >>> computed_table              # doctest: +NORMALIZE_WHITESPACE
           line title \ period[file]     | 10[1-2] | 11[1-2] | 12[1-2] | 13[1-2] | 14[1-2]
        -----------------------------------------------------------------------------------
        Output potentiel                 |  138.72 |  140.91 |  143.23 |  146.05 |  149.20
        Stock de capital                 |  225.88 |  230.50 |  234.74 |  239.51 |  245.28
        Intensité de capital             |    0.01 |    0.01 |    0.01 |    0.01 |    0.01
        Productivité totale des facteurs |    0.02 |    0.02 |    0.02 |    0.02 |    0.02
        <BLANKLINE>
        """
        return [item.decode() for item in self.c_computed_table.get_list_files()]

    @property
    def nb_operations_between_files(self) -> int:
        """
        The number of operations between files.

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

        >>> # simple time series (current workspace + one extra file) - 5 observations
        >>> computed_table = tables["C8_1"].compute("2010[1-2]:5", extra_files=f"{SAMPLE_DATA_DIR}/ref.av")     # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Loading ...\\ref.av
        ...
        394 objects loaded
        >>> computed_table.nb_files
        2
        >>> for filepath in computed_table.files:
        ...     print(Path(filepath).name)
        fun.var
        ref.av
        >>> computed_table.nb_operations_between_files
        1
        >>> computed_table              # doctest: +NORMALIZE_WHITESPACE
           line title \ period[file]     | 10[1-2] | 11[1-2] | 12[1-2] | 13[1-2] | 14[1-2]
        -----------------------------------------------------------------------------------
        Output potentiel                 |  138.72 |  140.91 |  143.23 |  146.05 |  149.20
        Stock de capital                 |  225.88 |  230.50 |  234.74 |  239.51 |  245.28
        Intensité de capital             |    0.01 |    0.01 |    0.01 |    0.01 |    0.01
        Productivité totale des facteurs |    0.02 |    0.02 |    0.02 |    0.02 |    0.02
        <BLANKLINE>
        """
        return self.c_computed_table.get_nb_files_ops()

    @property
    def nb_periods(self) -> int:
        """
        The number of periods associated with the computed table.

        Examples
        --------
        >>> from iode import SAMPLE_DATA_DIR
        >>> from iode import Table, tables, variables
        >>> tables.load(f"{SAMPLE_DATA_DIR}/fun.tbl")           # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Loading .../fun.tbl
        46 objects loaded 
        >>> variables.load(f"{SAMPLE_DATA_DIR}/fun.var")        # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Loading .../fun.var
        394 objects loaded

        >>> # two time series (current workspace) - 5 observations
        >>> computed_table = tables["C8_1"].compute("(2010;2010/2009):5")
        >>> computed_table.nb_periods
        5
        >>> computed_table.sample
        Sample("2010Y1:2014Y1")
        >>> computed_table              # doctest: +NORMALIZE_WHITESPACE
           line title \ period[file]     |    10    | 10/09 |    11    | 11/10 |    12    | 12/11 |    13    | 13/12 |    14    | 14/13
        --------------------------------------------------------------------------------------------------------------------------------
        Output potentiel                 |  6936.11 |  1.74 |  7045.34 |  1.57 |  7161.54 |  1.65 |  7302.29 |  1.97 |  7460.12 |  2.16
        Stock de capital                 | 11293.85 |  2.82 | 11525.01 |  2.05 | 11736.78 |  1.84 | 11975.49 |  2.03 | 12263.95 |  2.41
        Intensité de capital             |     0.39 | -2.17 |     0.38 | -2.05 |     0.37 | -1.91 |     0.36 | -1.86 |     0.36 | -1.90
        Productivité totale des facteurs |     1.10 |  1.00 |     1.11 |  1.00 |     1.12 |  1.00 |     1.13 |  1.00 |     1.14 |  1.00
        <BLANKLINE>
        """
        return self.c_computed_table.get_nb_periods()

    @property
    def sample(self) -> Sample:
        """
        The sample associated with the computed table.

        Examples
        --------
        >>> from iode import SAMPLE_DATA_DIR
        >>> from iode import Table, tables, variables
        >>> tables.load(f"{SAMPLE_DATA_DIR}/fun.tbl")           # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Loading .../fun.tbl
        46 objects loaded 
        >>> variables.load(f"{SAMPLE_DATA_DIR}/fun.var")        # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Loading .../fun.var
        394 objects loaded

        >>> # two time series (current workspace) - 5 observations
        >>> computed_table = tables["C8_1"].compute("(2010;2010/2009):5")
        >>> computed_table.nb_periods
        5
        >>> computed_table.sample
        Sample("2010Y1:2014Y1")
        >>> computed_table              # doctest: +NORMALIZE_WHITESPACE
           line title \ period[file]     |    10    | 10/09 |    11    | 11/10 |    12    | 12/11 |    13    | 13/12 |    14    | 14/13
        --------------------------------------------------------------------------------------------------------------------------------
        Output potentiel                 |  6936.11 |  1.74 |  7045.34 |  1.57 |  7161.54 |  1.65 |  7302.29 |  1.97 |  7460.12 |  2.16
        Stock de capital                 | 11293.85 |  2.82 | 11525.01 |  2.05 | 11736.78 |  1.84 | 11975.49 |  2.03 | 12263.95 |  2.41
        Intensité de capital             |     0.39 | -2.17 |     0.38 | -2.05 |     0.37 | -1.91 |     0.36 | -1.86 |     0.36 | -1.90
        Productivité totale des facteurs |     1.10 |  1.00 |     1.11 |  1.00 |     1.12 |  1.00 |     1.13 |  1.00 |     1.14 |  1.00
        <BLANKLINE>
        """
        cdef CSample* c_sample = self.c_computed_table.get_sample()
        return Sample._from_ptr(c_sample, <bint>False)

    @property
    def title(self) -> str:
        """
        The title of the computed table.

        Examples
        --------
        >>> from iode import SAMPLE_DATA_DIR
        >>> from iode import Table, tables, variables
        >>> tables.load(f"{SAMPLE_DATA_DIR}/fun.tbl")           # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Loading .../fun.tbl
        46 objects loaded 
        >>> variables.load(f"{SAMPLE_DATA_DIR}/fun.var")        # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Loading .../fun.var
        394 objects loaded

        >>> # simple time series (current workspace) - 6 observations
        >>> computed_table = tables["C8_1"].compute("2000:6")
        >>> computed_table.title
        "Déterminants de l'output potentiel"
        """
        return self.c_computed_table.get_title().decode()
    
    @property
    def ymin(self) -> float:
        """
        Minimum values on the Y axis. If data falls outside these values, the axis scale adapts to the data. 
        The value :math:`NA` can be set for ymin and/or ymax: in this case, the graphics program will calculate 
        an optimum scale value. 
        """
        return self.c_computed_table.get_ymin()

    @property
    def ymax(self) -> float:
        """
        Maximum values on the Y axis. If data falls outside these values, the axis scale adapts to the data. 
        The value :math:`NA` can be set for ymin and/or ymax: in this case, the graphics program will calculate 
        an optimum scale value. 
        """
        self.c_computed_table.get_ymax()

    @property
    def gridx(self) -> TableGraphGrid:
        """
        The gridx value of the table offers a choice of three X-grid options: 
        
            - MAJOR: draws a line across the entire graph at each main axis graduation
            - NONE: removes the grid from the graph.
            - MINOR: draws a line at all graduations 

        Returns
        -------
        TableGraphGrid

        Examples
        --------
        >>> from iode import SAMPLE_DATA_DIR
        >>> from iode import Table, tables
        >>> tables.load(f"{SAMPLE_DATA_DIR}/fun.tbl")           # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Loading .../fun.tbl
        46 objects loaded
        >>> # simple time series (current workspace) - 6 observations
        >>> computed_table = tables["C8_1"].compute("2000:6")
        >>> computed_table.gridx
        <TableGraphGrid.MAJOR: 0>
        """
        return TableGraphGrid(<int>(self.c_computed_table.get_grid_xaxis()))

    @property
    def gridy(self) -> TableGraphGrid:
        """
        The gridy value of the table offers a choice of three Y-grid options: 
        
            - MAJOR: draws a line across the entire graph at each main axis graduation
            - NONE: removes the grid from the graph.
            - MINOR: draws a line at all graduations 

        Returns
        -------
        TableGraphGrid

        Examples
        --------
        >>> from iode import SAMPLE_DATA_DIR
        >>> from iode import Table, tables
        >>> tables.load(f"{SAMPLE_DATA_DIR}/fun.tbl")           # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Loading .../fun.tbl
        46 objects loaded
        >>> # simple time series (current workspace) - 6 observations
        >>> computed_table = tables["C8_1"].compute("2000:6")
        >>> computed_table.gridy
        <TableGraphGrid.MAJOR: 0>
        """
        return TableGraphGrid(<int>(self.c_computed_table.get_grid_yaxis()))

    @property
    def graph_axis(self) -> TableGraphAxis:
        """
        Graph axis of the table allows you to select the type of axis: 
        
            - VALUES : level
            - LOG: logarithmic scale
            - SEMILOG: semi-logarithmic scale 
            - PERCENT: Y scale in percent from 0 to 100

        Returns
        -------
        TableGraphAxis

        Examples
        --------
        >>> from iode import SAMPLE_DATA_DIR
        >>> from iode import Table, tables
        >>> tables.load(f"{SAMPLE_DATA_DIR}/fun.tbl")           # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Loading .../fun.tbl
        46 objects loaded
        >>> # simple time series (current workspace) - 6 observations
        >>> computed_table = tables["C8_1"].compute("2000:6")
        >>> computed_table.graph_axis
        <TableGraphAxis.VALUES: 0>
        """
        return TableGraphAxis(self.c_computed_table.get_graph_axis())

    @property
    def graph_alignment(self) -> TableGraphAlign:
        """
        Graph alignment of the table.

        Returns
        -------
        TableGraphAlign

        Examples
        --------
        >>> from iode import SAMPLE_DATA_DIR
        >>> from iode import Table, tables
        >>> tables.load(f"{SAMPLE_DATA_DIR}/fun.tbl")           # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Loading .../fun.tbl
        46 objects loaded
        >>> # simple time series (current workspace) - 6 observations
        >>> computed_table = tables["C8_1"].compute("2000:6")
        >>> computed_table.graph_alignment
        <TableGraphAlign.LEFT: 0>
        """
        return TableGraphAlign(<int>(self.c_computed_table.get_alignement()))

    def is_editable(self, row: Union[int, str], column: Union[int, str]) -> bool:
        """
        Check if a cell in the computed table is editable.

        A cell is **not** editable if:

          1. the corresponding column contains on operation on periods or files
          2. the corresponding column does not refer to the current workspace
          3. the corresponding LEC expression from the original table starts with 0+
          4. the corresponding LEC expression from the original table does not refer to at least one variable

        Parameters
        ----------
        row : int or str
            The row passed as position (int) or as name (str).
        column : int or str
            The column passed as position (int) or as name (str).

        Returns
        -------
        bool: True if the cell is editable, False otherwise.

        Examples
        --------
        >>> from iode import SAMPLE_DATA_DIR
        >>> from iode import Table, comments, variables
        >>> comments.load(f"{SAMPLE_DATA_DIR}/fun.cmt")         # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Loading .../fun.cmt
        317 objects loaded 
        >>> variables.load(f"{SAMPLE_DATA_DIR}/fun.var")        # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Loading .../fun.var
        394 objects loaded

        >>> title = "Example Table"
        >>> lecs = ["Q_F", "Q_I", "Q_F/Q_I", "ln(Q_I+Q_F)", "KNFF", "KNFF[-1]", "3+ln(10)", "0+KNFF"]
        >>> table = Table(2, title, lecs)
        >>> table                       # doctest: +NORMALIZE_WHITESPACE
        DIVIS | 1             |
        TITLE |       "Example Table"
        ----- | ---------------------------
        CELL  |               |     "#S"
        ----- | ---------------------------
        CELL  | "Q_F"         |         Q_F
        CELL  | "Q_I"         |         Q_I
        CELL  | "Q_F/Q_I"     |     Q_F/Q_I
        CELL  | "ln(Q_I+Q_F)" | ln(Q_I+Q_F)
        CELL  | "KNFF"        |        KNFF
        CELL  | "KNFF[-1]"    |    KNFF[-1]
        CELL  | "3+ln(10)"    |    3+ln(10)
        CELL  | "0+KNFF"      |      0+KNFF
        <BLANKLINE>
        nb lines: 12
        nb columns: 2
        language: 'ENGLISH'
        gridx: 'MAJOR'
        gridy: 'MAJOR'
        graph_axis: 'VALUES'
        graph_alignment: 'LEFT'
        <BLANKLINE>

        >>> # compute table
        >>> computed_table = table.compute("(2010;2010/2009)[1;2]:2", extra_files=f"{SAMPLE_DATA_DIR}/ref.av")      # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Loading ...\\ref.av
        ...
        394 objects loaded
        >>> computed_table                      # doctest: +NORMALIZE_WHITESPACE
         line title \ period[file]  | 2010[1]  | 2010[2]  | 2010/2009[1] | 2010/2009[2] | 2011[1]  | 2011[2]  | 2011/2010[1] | 2011/2010[2]
        ------------------------------------------------------------------------------------------------------------------------------------
        Q_F                         |  5842.74 |  5725.89 |         1.90 |         1.90 |  5930.75 |  5812.13 |         1.51 |         1.51
        Q_I                         |  1093.37 |  1071.50 |         0.89 |         0.89 |  1114.60 |  1092.30 |         1.94 |         1.94
        Q_F/Q_I                     |     5.34 |     5.34 |         1.00 |         1.00 |     5.32 |     5.32 |        -0.43 |        -0.43
        ln(Q_I+Q_F)                 |     8.84 |     8.82 |         0.20 |         0.20 |     8.86 |     8.84 |         0.18 |         0.18
        KNFF                        | 11525.01 | 11294.51 |         2.05 |         2.05 | 11736.78 | 11502.05 |         1.84 |         1.84
        KNFF[-1]                    | 11293.85 | 11067.97 |         2.82 |         2.82 | 11525.01 | 11294.51 |         2.05 |         2.05
        3+ln(10)                    |     5.30 |     5.30 |         0.00 |         0.00 |     5.30 |     5.30 |         0.00 |         0.00
        0+KNFF                      | 11525.01 | 11294.51 |         2.05 |         2.05 | 11736.78 | 11502.05 |         1.84 |         1.84
        <BLANKLINE>

        >>> computed_table.is_editable("Q_F", "2011[1]")
        True
        >>> # 1. cell is not editable if the corresponding column contains on operation on periods or files
        >>> computed_table.is_editable("Q_F", "2010/2009[1]")
        False
        >>> # 2. cell is not editable if the corresponding column does not refer to the current workspace
        >>> computed_table.is_editable("Q_F", "2011[2]")
        False
        >>> # 3. cell is not editable if the corresponding LEC expression from the original table starts with 0+
        >>> computed_table.is_editable("0+KNFF", "2011[1]")
        False
        >>> # 4. cell is not editable if the corresponding LEC expression from the original table 
        >>> #    does not refer to at least one variable
        >>> computed_table.is_editable("3+ln(10)", "2011[1]")
        False
        """
        # if row and column is a string -> convert to position in the table 
        row, column = self._unfold_key((row, column))
        return self.c_computed_table.is_editable(row, column)

    def to_frame(self) -> DataFrame:
        """
        Convert the computed table to a pandas DataFrame.

        Examples
        --------
        >>> from iode import SAMPLE_DATA_DIR
        >>> from iode import Table, tables, variables
        >>> tables.load(f"{SAMPLE_DATA_DIR}/fun.tbl")           # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Loading .../fun.tbl
        46 objects loaded 
        >>> variables.load(f"{SAMPLE_DATA_DIR}/fun.var")        # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Loading .../fun.var
        394 objects loaded

        >>> # simple time series (current workspace) - 6 observations
        >>> computed_table = tables["C8_1"].compute("2000:6")
        >>> computed_table              # doctest: +NORMALIZE_WHITESPACE
           line title \ period[file]     |    00   |    01   |    02   |    03   |    04   |    05
        ---------------------------------------------------------------------------------------------
        Output potentiel                 | 5495.21 | 5627.86 | 5748.78 | 5857.95 | 5966.20 | 6103.63
        Stock de capital                 | 8083.55 | 8359.89 | 8647.94 | 8910.34 | 9175.81 | 9468.89
        Intensité de capital             |    0.50 |    0.49 |    0.48 |    0.46 |    0.45 |    0.43
        Productivité totale des facteurs |    0.99 |    1.00 |    1.01 |    1.02 |    1.03 |    1.04
        <BLANKLINE>
        >>> df = computed_table.to_frame()
        >>> df                          # doctest: +NORMALIZE_WHITESPACE
        period[file]                               00  ...           05
        name                                           ...
        Output potentiel                  5495.212782  ...  6103.631844
        Stock de capital                  8083.551748  ...  9468.886506
        Intensité de capital                 0.503166  ...     0.434914
        Productivité totale des facteurs     0.993773  ...     1.044466
        <BLANKLINE>
        [4 rows x 6 columns]
        """
        if pd is None:
            raise RuntimeError("pandas library not found")

        # get data from the C++ computed table
        data = [[self.c_computed_table.get_value(i, j, <bint>True) for j in range(self.nb_columns)] 
                for i in range(self.nb_lines)]
        df = DataFrame(index=self.lines, columns=self.columns, data=data)
        # replace IODE NaN values by numpy or pandas values
        df.where(df < IODE_NAN * (1.0 - 1e-30))
        df.index.name = "name"
        df.columns.name = "period[file]"
        return df

    def to_array(self) -> Array:
        """
        Convert the computed table to a larray Array.

        Examples
        --------
        >>> from iode import SAMPLE_DATA_DIR
        >>> from iode import Table, tables, variables
        >>> tables.load(f"{SAMPLE_DATA_DIR}/fun.tbl")           # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Loading .../fun.tbl
        46 objects loaded 
        >>> variables.load(f"{SAMPLE_DATA_DIR}/fun.var")        # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Loading .../fun.var
        394 objects loaded

        >>> # simple time series (current workspace) - 6 observations
        >>> computed_table = tables["C8_1"].compute("2000:6")
        >>> computed_table              # doctest: +NORMALIZE_WHITESPACE
           line title \ period[file]     |    00   |    01   |    02   |    03   |    04   |    05
        ---------------------------------------------------------------------------------------------
        Output potentiel                 | 5495.21 | 5627.86 | 5748.78 | 5857.95 | 5966.20 | 6103.63
        Stock de capital                 | 8083.55 | 8359.89 | 8647.94 | 8910.34 | 9175.81 | 9468.89
        Intensité de capital             |    0.50 |    0.49 |    0.48 |    0.46 |    0.45 |    0.43
        Productivité totale des facteurs |    0.99 |    1.00 |    1.01 |    1.02 |    1.03 |    1.04
        <BLANKLINE>
        >>> array = computed_table.to_array()
        >>> array                       # doctest: +NORMALIZE_WHITESPACE
        name\period[file]                  00  ...                  05
         Output potentiel   5495.212781863819  ...   6103.631843893574
         Stock de capital   8083.551747773469  ...   9468.886506037787
             Intensité de  0.5031658911658933  ...  0.4349138159428717
                  capital
             Productivité   0.993773299183911  ...  1.0444657249633247
               totale des
                 facteurs
        """
        if la is None:
            raise RuntimeError("larray library not found")

        df = self.to_frame()
        return la.from_frame(df)

    def print_to_file(self, destination_file: Union[str, Path], format: str=None):
        """
        Print the present computed table to a file.

        Argument `format` must be in the list:
        - 'H' (HTML file)
        - 'M' (MIF file)
        - 'R' (RTF file)
        - 'C' (CSV file)
        - 'D' (DUMMY file)

        If argument `format` is null (default), the A2M format will be used
        to print the output.

        If the filename does not contain an extension, it is automatically
        added based on the format.

        Parameters
        ----------
        destination_file: str or Path 
            The destination file for printing
        format: str, optional
            The format of the output file. Deduced from the extension if not provided.
            If destination_file has no extension and format is None, the A2M format is used.
            
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

        >>> computed_table = tables["C8_1"].compute("(2010;2010/2009):5")
        >>> computed_table              # doctest: +NORMALIZE_WHITESPACE, +ELLIPSIS
           line title \ period[file]     |    10    | 10/09 |    11    | 11/10 | ... |    14    | 14/13
        -------------------------------------------------------------------------...--------------------
        Output potentiel                 |  6936.11 |  1.74 |  7045.34 |  1.57 | ... |  7460.12 |  2.16
        Stock de capital                 | 11293.85 |  2.82 | 11525.01 |  2.05 | ... | 12263.95 |  2.41
        Intensité de capital             |     0.39 | -2.17 |     0.38 | -2.05 | ... |     0.36 | -1.90
        Productivité totale des facteurs |     1.10 |  1.00 |     1.11 |  1.00 | ... |     1.14 |  1.00
        <BLANKLINE>
        >>> computed_table.print_to_file(tmp_dir / "computed_table_2_periods.csv")
        >>> with open(tmp_dir / "computed_table_2_periods.csv", "r") as f:    # doctest: +NORMALIZE_WHITESPACE, +ELLIPSIS
        ...     print(f.read())
        "Déterminants de l'output potentiel"
        <BLANKLINE>
        " ","10","10/09","11","11/10","12","12/11","13","13/12","14","14/13",
        <BLANKLINE>
        "Output potentiel","6936.11","1.74",...,"7460.12","2.16",       
        "Stock de capital","11293.85","2.82",...,"12263.95","2.41",  
        "Intensité de capital","0.39","-2.17",...,"0.36","-1.90",
        "Productivité totale des facteurs","1.10","1.00",...,"1.14","1.00",      
        <BLANKLINE>

        >>> extra_files = Path(SAMPLE_DATA_DIR) / "ref.av"
        >>> computed_table = tables["C8_1"].compute("2010[1-2]:5", extra_files=extra_files)     # doctest: +NORMALIZE_WHITESPACE, +ELLIPSIS
        Loading ...\\ref.av
        ...
        394 objects loaded
        >>> computed_table              # doctest: +NORMALIZE_WHITESPACE
           line title \ period[file]     | 10[1-2] | 11[1-2] | 12[1-2] | 13[1-2] | 14[1-2]
        -----------------------------------------------------------------------------------
        Output potentiel                 |  138.72 |  140.91 |  143.23 |  146.05 |  149.20
        Stock de capital                 |  225.88 |  230.50 |  234.74 |  239.51 |  245.28
        Intensité de capital             |    0.01 |    0.01 |    0.01 |    0.01 |    0.01
        Productivité totale des facteurs |    0.02 |    0.02 |    0.02 |    0.02 |    0.02
        <BLANKLINE>
        >>> computed_table.print_to_file(tmp_dir / "computed_table_2_files.csv")    
        >>> with open(tmp_dir / "computed_table_2_files.csv", "r") as f:
        ...     print(f.read())
        "Déterminants de l'output potentiel"
        <BLANKLINE>
        " ","10[1-2]","11[1-2]","12[1-2]","13[1-2]","14[1-2]",
        <BLANKLINE>
        "Output potentiel","138.72","140.91","143.23","146.05","149.20",
        "Stock de capital","225.88","230.50","234.74","239.51","245.28",
        "Intensité de capital","0.01","0.01","0.01","0.01","0.01",
        "Productivité totale des facteurs","0.02","0.02","0.02","0.02","0.02",
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

        self.c_computed_table.print_to_file(destination_file.encode('utf-8'), c_format)

    def _unfold_key(self, key: Tuple[Union[int, str], Union[int, str]]) -> Tuple[int, int]:
        if not isinstance(key, tuple) and len(key) == 2:
            raise ValueError("Expected 'row, column' as key to access the cell of the computed table.\n"
                             f"Got key '{key}' instead")
        row, column = key
        if isinstance(row, str):
            row = self.lines.index(row)
        if not isinstance(row, int):
            raise TypeError("Expected value of type int or str as first part of the key to access "
                            f"the cell of the computed table.\nGot value of type {type(row).__name__} instead.")
        if isinstance(column, str):
            column = self.columns.index(column)
        if not isinstance(column, int):
            raise TypeError("Expected value of type int or str as second part of the key to access "
                            f"the cell of the computed table.\nGot value of type {type(column).__name__} instead.")
        return row, column

    def __getitem__(self, key: Tuple[Union[int, str], Union[int, str]]) -> float:
        """
        Get the value of a cell of the computed table.

        Parameters
        ----------
        key: tuple(int or str, int or str) or int or str
            Row and column of the cell to access. 
            Row and column can be passed either as position (int) or as name (str).

        Returns
        -------
        float: 
            The value of a given cell.

        Examples
        --------
        >>> from iode import SAMPLE_DATA_DIR
        >>> from iode import Table, comments, variables
        >>> comments.load(f"{SAMPLE_DATA_DIR}/fun.cmt")         # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Loading .../fun.cmt
        317 objects loaded 
        >>> variables.load(f"{SAMPLE_DATA_DIR}/fun.var")        # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Loading .../fun.var
        394 objects loaded

        >>> title = "Example Table"
        >>> lecs = ["Q_F", "Q_I", "Q_F/Q_I", "ln(Q_I+Q_F)", "KNFF", "KNFF[-1]", "3+ln(10)", "0+KNFF"]
        >>> table = Table(2, title, lecs)
        >>> table                       # doctest: +NORMALIZE_WHITESPACE
        DIVIS | 1             |
        TITLE |       "Example Table"
        ----- | ---------------------------
        CELL  |               |     "#S"
        ----- | ---------------------------
        CELL  | "Q_F"         |         Q_F
        CELL  | "Q_I"         |         Q_I
        CELL  | "Q_F/Q_I"     |     Q_F/Q_I
        CELL  | "ln(Q_I+Q_F)" | ln(Q_I+Q_F)
        CELL  | "KNFF"        |        KNFF
        CELL  | "KNFF[-1]"    |    KNFF[-1]
        CELL  | "3+ln(10)"    |    3+ln(10)
        CELL  | "0+KNFF"      |      0+KNFF
        <BLANKLINE>
        nb lines: 12
        nb columns: 2
        language: 'ENGLISH'
        gridx: 'MAJOR'
        gridy: 'MAJOR'
        graph_axis: 'VALUES'
        graph_alignment: 'LEFT'
        <BLANKLINE>

        >>> # compute table
        >>> computed_table = table.compute("(2010;2010/2009)[1;2]:2", extra_files=f"{SAMPLE_DATA_DIR}/ref.av")      # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Loading ...\\ref.av
        ...
        394 objects loaded
        >>> computed_table                      # doctest: +NORMALIZE_WHITESPACE
         line title \ period[file]  | 2010[1]  | 2010[2]  | 2010/2009[1] | 2010/2009[2] | 2011[1]  | 2011[2]  | 2011/2010[1] | 2011/2010[2]
        ------------------------------------------------------------------------------------------------------------------------------------
        Q_F                         |  5842.74 |  5725.89 |         1.90 |         1.90 |  5930.75 |  5812.13 |         1.51 |         1.51
        Q_I                         |  1093.37 |  1071.50 |         0.89 |         0.89 |  1114.60 |  1092.30 |         1.94 |         1.94
        Q_F/Q_I                     |     5.34 |     5.34 |         1.00 |         1.00 |     5.32 |     5.32 |        -0.43 |        -0.43
        ln(Q_I+Q_F)                 |     8.84 |     8.82 |         0.20 |         0.20 |     8.86 |     8.84 |         0.18 |         0.18
        KNFF                        | 11525.01 | 11294.51 |         2.05 |         2.05 | 11736.78 | 11502.05 |         1.84 |         1.84
        KNFF[-1]                    | 11293.85 | 11067.97 |         2.82 |         2.82 | 11525.01 | 11294.51 |         2.05 |         2.05
        3+ln(10)                    |     5.30 |     5.30 |         0.00 |         0.00 |     5.30 |     5.30 |         0.00 |         0.00
        0+KNFF                      | 11525.01 | 11294.51 |         2.05 |         2.05 | 11736.78 | 11502.05 |         1.84 |         1.84
        <BLANKLINE>

        >>> # get cell value by position
        >>> computed_table[2, 2]
        0.9975986300775119
        >>> # get cell value by labels
        >>> computed_table["Q_F/Q_I", "2010/2009[1]"]
        0.9975986300775119
        """
        row, column = self._unfold_key(key)
        return self.c_computed_table.get_value(row, column, <bint>True)

    def __setitem__(self, key: Tuple[Union[int, str], Union[int, str]], value: float):
        """
        Assign a new value to a cell. 
        May modify the values of other cells.

        A cell is **not** editable if:

          1. the corresponding column contains on operation on periods or files
          2. the corresponding column does not refer to the current workspace
          3. the corresponding LEC expression from the original table starts with 0+
          4. the corresponding LEC expression from the original table does not refer to at least one variable

        Parameters
        ----------
        key: tuple(int or str, int or str) 
            Row and column to access the cell. 
            Row and column can be passed either as position (int) or as name (str).
        value: float 
            New value of the cell.

        Notes
        -----
        When the corresponding LEC expression involves more than one variable, only 
        the value of the first variable is modified.

        Examples
        --------
        >>> from iode import SAMPLE_DATA_DIR
        >>> from iode import Table, comments, variables
        >>> comments.load(f"{SAMPLE_DATA_DIR}/fun.cmt")         # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Loading .../fun.cmt
        317 objects loaded 
        >>> variables.load(f"{SAMPLE_DATA_DIR}/fun.var")        # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Loading .../fun.var
        394 objects loaded

        >>> title = "Example Table"
        >>> lecs = ["Q_F", "Q_I", "Q_F/Q_I", "ln(Q_I+Q_F)", "KNFF", "KNFF[-1]", "3+ln(10)", "0+KNFF"]
        >>> table = Table(2, title, lecs)
        >>> table                       # doctest: +NORMALIZE_WHITESPACE
        DIVIS | 1             |
        TITLE |       "Example Table"
        ----- | ---------------------------
        CELL  |               |     "#S"
        ----- | ---------------------------
        CELL  | "Q_F"         |         Q_F
        CELL  | "Q_I"         |         Q_I
        CELL  | "Q_F/Q_I"     |     Q_F/Q_I
        CELL  | "ln(Q_I+Q_F)" | ln(Q_I+Q_F)
        CELL  | "KNFF"        |        KNFF
        CELL  | "KNFF[-1]"    |    KNFF[-1]
        CELL  | "3+ln(10)"    |    3+ln(10)
        CELL  | "0+KNFF"      |      0+KNFF
        <BLANKLINE>
        nb lines: 12
        nb columns: 2
        language: 'ENGLISH'
        gridx: 'MAJOR'
        gridy: 'MAJOR'
        graph_axis: 'VALUES'
        graph_alignment: 'LEFT'
        <BLANKLINE>

        >>> # compute table
        >>> computed_table = table.compute("(2010;2010/2009)[1;2]:2", extra_files=f"{SAMPLE_DATA_DIR}/ref.av")      # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
        Loading ...\\ref.av
        ...
        394 objects loaded
        >>> computed_table                      # doctest: +NORMALIZE_WHITESPACE
         line title \ period[file]  | 2010[1]  | 2010[2]  | 2010/2009[1] | 2010/2009[2] | 2011[1]  | 2011[2]  | 2011/2010[1] | 2011/2010[2]
        ------------------------------------------------------------------------------------------------------------------------------------
        Q_F                         |  5842.74 |  5725.89 |         1.90 |         1.90 |  5930.75 |  5812.13 |         1.51 |         1.51
        Q_I                         |  1093.37 |  1071.50 |         0.89 |         0.89 |  1114.60 |  1092.30 |         1.94 |         1.94
        Q_F/Q_I                     |     5.34 |     5.34 |         1.00 |         1.00 |     5.32 |     5.32 |        -0.43 |        -0.43
        ln(Q_I+Q_F)                 |     8.84 |     8.82 |         0.20 |         0.20 |     8.86 |     8.84 |         0.18 |         0.18
        KNFF                        | 11525.01 | 11294.51 |         2.05 |         2.05 | 11736.78 | 11502.05 |         1.84 |         1.84
        KNFF[-1]                    | 11293.85 | 11067.97 |         2.82 |         2.82 | 11525.01 | 11294.51 |         2.05 |         2.05
        3+ln(10)                    |     5.30 |     5.30 |         0.00 |         0.00 |     5.30 |     5.30 |         0.00 |         0.00
        0+KNFF                      | 11525.01 | 11294.51 |         2.05 |         2.05 | 11736.78 | 11502.05 |         1.84 |         1.84
        <BLANKLINE>

        >>> # set cell value by position
        >>> computed_table[1, 4] = 1114.0
        >>> computed_table[1, 4]
        1114.0
        >>> # set cell value by labels
        >>> computed_table["Q_I", "2011[1]"] = 1115.0
        >>> computed_table["Q_I", "2011[1]"]
        1115.0

        >>> # warning: when the corresponding LEC expression involves more than one variable, 
        >>> #          only the value of the first variable is modified.
        >>> # variables values before:
        >>> variables["Q_F", "2011Y1"]
        5930.747852857564
        >>> variables["Q_I", "2011Y1"]
        1115.0
        >>> # update cell of the computed table
        >>> computed_table["Q_F/Q_I", "2011[1]"] = 5.28
        >>> # variables values after:
        >>> #   Q_F/Q_I = new_value 
        >>> #   Q_F = new_value * Q_I 
        >>> variables["Q_F", "2011Y1"]
        5887.20023393631
        >>> variables["Q_I", "2011Y1"]
        1115.0
        """
        row, column = self._unfold_key(key)

        if not self.c_computed_table.is_editable(row, column):
            warnings.warn(f"The cell corresponding to the line '{self.lines[row]}' and " 
                          f"column '{self.columns[column]}' is not editable")
            return

        if isinstance(value, int):
            value = float(value)
        if not isinstance(value, float):
            raise TypeError(f"Expected new cell value of type float. Got value of type {type(value).__name__} instead.")

        self.c_computed_table.set_value(row, column, value, <bint>False)

    def __str__(self) -> str:
        """
        
        Examples
        --------

        """
        upper_left_corner = " line title \ period[file] "
        line_names = self.lines

        max_length = max([len(upper_left_corner)] + [len(line_name) for line_name in line_names])
        upper_left_corner = upper_left_corner.center(max_length)
        line_names = [line_name.ljust(max_length) for line_name in line_names]

        def cell_value_to_str(i: int, j: int) -> str:
            value = self.c_computed_table.get_value(i, j, <bint>True)
            return f"{value:.{self.nb_decimals}f}"  if value > IODE_NAN else NAN_REP

        data = []
        column_names = []
        for j, column_name in enumerate(self.columns):
            column_data = [cell_value_to_str(i, j) for i in range(self.nb_lines)]
            max_length = max([len(column_name)] + [len(value) for value in column_data])
            column_names += [column_name.center(max_length)]
            data += [[value.rjust(max_length) for value in column_data]]

        # transpose data
        data = list(zip(*data))

        # build string to return
        s = ' | '.join([upper_left_corner] + column_names) + '\n'
        s += '-' * len(s) + '\n'
        for i in range(self.nb_lines):
            s += ' | '.join([line_names[i]] + list(data[i])) + '\n'
        return s

    def __repr__(self) -> str:
        return str(self)
