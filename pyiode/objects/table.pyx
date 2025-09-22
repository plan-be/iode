import warnings
from copy import copy
from collections.abc import Iterable
from typing import Union, Tuple, List, Dict, Optional

from libcpp.string cimport string
from cython.operator cimport dereference
from pyiode.common cimport TableLang as CTableLang
from pyiode.common cimport TableCellType as CTableCellType 
from pyiode.common cimport TableCellFont as CTableCellFont
from pyiode.common cimport TableCellAlign as CTableCellAlign 
from pyiode.common cimport TableLineType as CTableLineType
from pyiode.common cimport TableGraphAlign as CTableGraphAlign 
from pyiode.common cimport TableGraphAxis as CTableGraphAxis 
from pyiode.common cimport TableGraphGrid as CTableGraphGrid 
from pyiode.common cimport TableGraphType as CTableGraphType
from pyiode.objects.table cimport CTableCell, CTableLine, CTable
from pyiode.objects.table cimport hash_value as hash_value_tbl
from pyiode.iode_database.cpp_api_database cimport KDBTables as CKDBTables


cdef class TableCell:
    cdef CTableCell* c_cell
    cdef int nb_columns
    cdef object py_parent_table

    def __cinit__(self):
        self.c_cell = NULL
        self.nb_columns = 0
        self.py_parent_table = None

    def __dealloc__(self):
        pass

    @staticmethod
    cdef TableCell from_ptr(CTableCell* c_cell_ptr, int nb_columns, py_parent_table):
        """
        Factory function to create TableCell objects from given CTableCell pointer.
        """
        # Fast call to __new__() that bypasses the __init__() constructor.
        cdef TableCell wrapper = TableCell.__new__(TableCell)
        wrapper.c_cell = c_cell_ptr
        wrapper.nb_columns = nb_columns
        wrapper.py_parent_table = py_parent_table
        return wrapper

    def get_nb_columns(self) -> int:
        return self.nb_columns

    def get_cell_type(self) -> str:
        return TableCellType(<int>(self.c_cell.get_type())).name if self.c_cell is not NULL else None

    def get_align(self) -> str:
        return TableCellAlign(<int>(self.c_cell.get_align())).name if self.c_cell is not NULL else None

    def set_align(self, value: Union[TableCellAlign, str]):
        if isinstance(value, str):
            value = value.upper()
            value = TableCellAlign[value]
        value = int(value)
        self.c_cell.set_align(<CTableCellAlign>value)
        self.py_parent_table.update_owner_database()

    def get_bold(self) -> bool:
        return self.c_cell.is_bold() if self.c_cell is not NULL else None

    def set_bold(self, value: bool):
        if self.c_cell is not NULL:
            self.c_cell.set_bold(<bint>value)
            self.py_parent_table.update_owner_database()

    def get_italic(self) -> bool:
        return self.c_cell.is_italic() if self.c_cell is not NULL else None

    def set_italic(self, value: bool):
        if self.c_cell is not NULL:
            self.c_cell.set_italic(<bint>value)
            self.py_parent_table.update_owner_database()

    def get_underline(self) -> bool:
        return self.c_cell.is_underline() if self.c_cell is not NULL else None

    def set_underline(self, value: bool):
        if self.c_cell is not NULL:
            self.c_cell.set_underline(<bint>value)
            self.py_parent_table.update_owner_database()

    def get_coefficients(self) -> List[str]:
        if self.c_cell is NULL:
            return
        return [item.decode() for item in self.c_cell.get_coefficients_from_lec()]     

    def get_variables(self) -> List[str]:
        if self.c_cell is NULL:
            return
        return [item.decode() for item in self.c_cell.get_variables_from_lec()]

    def _str_(self) -> str:
        if self.c_cell is NULL:
            return ''
        quotes = <int>(self.c_cell.get_type()) == TableCellType.STRING
        content = self.c_cell.get_content(<bint>quotes).decode()
        # remove newline characters
        return ''.join(content.splitlines())


cdef class TableLine:
    cdef CTableLine* c_line
    cdef int nb_columns
    cdef object py_parent_table

    def __cinit__(self):
        self.c_line = NULL
        self.nb_columns = 0
        self.py_parent_table = None

    def __dealloc__(self):
        pass

    @staticmethod
    cdef TableLine from_ptr(CTableLine* c_line_ptr, int nb_columns, py_parent_table):
        """
        Factory function to create TableLine objects from given CTableLine pointer.
        """
        # Fast call to __new__() that bypasses the __init__() constructor.
        cdef TableLine wrapper = TableLine.__new__(TableLine)
        wrapper.c_line = c_line_ptr
        wrapper.nb_columns = nb_columns
        wrapper.py_parent_table = py_parent_table
        return wrapper

    def get_line_type(self) -> str:
        return TableLineType(<int>(self.c_line.get_line_type())).name if self.c_line is not NULL else None

    def get_graph_type(self) -> str:
        return TableGraphType(<int>(self.c_line.get_line_graph())).name if self.c_line is not NULL else None

    def set_graph_type(self, value: int):
        if self.c_line is NULL:
            return
        self.c_line.set_line_graph(<CTableGraphType>value)
        self.py_parent_table.update_owner_database()

    def get_axis_left(self) -> bool:
        return self.c_line.is_left_axis() if self.c_line is not NULL else None

    def set_axis_left(self, value: bool):
        if self.c_line is NULL:
            return
        self.c_line.set_line_axis(<bint>value)
        self.py_parent_table.update_owner_database()

    def size(self) -> int:
        if self.c_line is NULL:
            return 0
        
        line_type: TableLineType = self.c_line.get_line_type()
        if line_type == TableLineType.TITLE:
            return 1
        elif line_type == TableLineType.CELL:
            return self.nb_columns
        else:
            return 0

    def _getitem_(self, col: int, cell: TableCell) -> TableCell:
        cdef CTableCell* c_cell

        if self.c_line is NULL:
            return
        
        c_cell = self.c_line.get_cell(col, self.nb_columns)

        cell.c_cell = c_cell
        cell.nb_columns = self.nb_columns
        cell.py_parent_table = self.py_parent_table
        return cell

    def _setitem_(self, col: int, value: str):
        if self.c_line is NULL:
            return
        
        c_cell = self.c_line.get_cell(col, self.nb_columns)
        c_cell.set_content(value.encode())
        self.py_parent_table.update_owner_database()

    def _str_(self) -> str:
        cdef CTableCell* c_cell

        if self.c_line is NULL:
            return ''

        line_type = <int>(self.c_line.get_line_type())
        nb_columns = self.nb_columns
        if line_type == TableLineType.TITLE:
            content = self.c_line.get_cell(0, nb_columns).get_content(<bint>False).decode()
            # remove newline characters
            return ''.join(content.splitlines())
        elif line_type == TableLineType.CELL:
            cells_content = []
            for j in range(nb_columns):
                c_cell = self.c_line.get_cell(j, nb_columns)
                quotes = <int>(c_cell.get_type()) == TableCellType.STRING
                content = c_cell.get_content(<bint>quotes).decode()
                # remove newline characters
                cells_content += [''.join(content.splitlines())]
            return str(tuple(cells_content))
        elif line_type == TableLineType.SEP:
            return '---'
        else:
            return f"<{self.get_line_type()}>"


cdef class Table:
    cdef bint ptr_owner
    cdef CKDBTables* c_database
    cdef string c_table_name
    cdef CTable* c_table

    def __cinit__(self):
        self.ptr_owner = False
        self.c_database = NULL
        self.c_table_name = b''
        self.c_table = NULL

    def __init__(self, nb_columns: int, table_title: str, lecs_or_vars: Union[None, List[str]], 
                 lines_titles: Union[None, List[str]], mode: bool, files: bool, date: bool) -> Table:
        cdef vector[string] cpp_variables = []
        cdef vector[string] cpp_lines_titles = []
        cdef vector[string] cpp_lecs = []

        if lines_titles is None:
            variables = lecs_or_vars
            if isinstance(variables, str):
                self.c_table = new CTable(<int>nb_columns, <string>table_title.encode(), <string>variables.encode(), <bint>mode, <bint>files, <bint>date)
            else:
                for variable in variables:
                    cpp_variables.push_back(variable.encode())
                self.c_table = new CTable(<int>nb_columns, <string>table_title.encode(), cpp_variables, <bint>mode, <bint>files, <bint>date)
        else:            
            for line_title in lines_titles:
                cpp_lines_titles.push_back(line_title.encode())

            lecs = lecs_or_vars
            for lec in lecs:
                cpp_lecs.push_back(lec.encode())

            self.c_table = new CTable(nb_columns, <string>table_title.encode(), cpp_lines_titles, cpp_lecs, <bint>mode, <bint>files, <bint>date)
        
        self.c_table_name = b''
        self.c_database = NULL
        self.ptr_owner = <bint>True

    def __dealloc__(self):
        if self.ptr_owner and self.c_table is not NULL:
            del self.c_table
            self.c_database = NULL
            self.c_table_name = b''
            self.c_table = NULL

    cdef void extract_tbl_from_database(self):
        # if c_table belongs to a database, we need to (re)extract it from the database every 
        # time we need to access one of its attributes since working for example on the global 
        # database may modify the table (which is not automatically reflected in c_table)
        if self.c_database is not NULL:
            if self.ptr_owner and self.c_table is not NULL:
                del self.c_table
            self.c_table = self.c_database.get(self.c_table_name)
            self.ptr_owner = True

    def update_owner_database(self):
        if self.c_database is not NULL and self.c_table is not NULL:
            self.c_database.update(self.c_table_name, dereference(self.c_table))

    @staticmethod
    cdef Table _from_ptr(CTable* ptr, bint owner=False, bytes b_table_name=b'', CKDBTables* c_database_ptr=NULL):
        """
        Factory function to create Table objects from a given CTable pointer.
        """
        # Fast call to __new__() that bypasses the __init__() constructor.
        cdef Table wrapper = Table.__new__(Table)
        wrapper.c_table = ptr
        wrapper.c_database = c_database_ptr
        wrapper.c_table_name = b_table_name
        wrapper.ptr_owner = owner
        return wrapper

    def get_nb_lines(self) -> int:
        self.extract_tbl_from_database()
        return <int>(self.c_table.lines.size())

    def get_nb_columns(self) -> int:
        self.extract_tbl_from_database()
        return <int>(self.c_table.nb_columns)

    def get_title(self) -> str:
        cdef string c_title
        cdef CTableLine* c_line
        self.extract_tbl_from_database()

        for i in range(self.c_table.lines.size()):
            c_line = self.c_table.get_line(i)
            line_type = <int>(c_line.get_line_type())
            if line_type == TableLineType.TITLE:
                c_title = self.c_table.get_title(i)
                return c_title.decode()
        return ""

    def set_title(self, value: str):
        cdef CTableLine* c_line
        cdef string c_title

        for i in range(self.c_table.lines.size()):
            c_line = self.c_table.get_line(i)
            line_type = <int>(c_line.get_line_type())
            if line_type == TableLineType.TITLE:
                c_title = value.encode()
                self.c_table.set_title(i, c_title)
        self.update_owner_database()

    def get_language(self) -> str:
        self.extract_tbl_from_database()
        return self.c_table.get_language().decode().upper()

    def set_language(self, value: Union[TableLang, str]):
        if isinstance(value, str):
            upper_str = value.upper()
            if upper_str not in TableLang.__members__:
                raise ValueError(f"'language': Invalid value '{value}'. "
                                 f"Expected one of {', '.join(TableLang.__members__.keys())}. ")
            value = TableLang[upper_str]
        value = int(value)
        self.c_table.set_language(<CTableLang>value)
        self.update_owner_database()

    def get_gridx(self) -> str:
        self.extract_tbl_from_database()
        return TableGraphGrid(<int>(self.c_table.get_gridx())).name

    def set_gridx(self, value: Union[TableGraphGrid, str]):
        if isinstance(value, str):
            value = value.upper()
            value = TableGraphGrid[value]
        value = int(value)
        self.c_table.set_gridx(<CTableGraphGrid>value)
        self.update_owner_database()

    def get_gridy(self) -> str:
        self.extract_tbl_from_database()
        return TableGraphGrid(<int>(self.c_table.get_gridy())).name

    def set_gridy(self, value: Union[TableGraphGrid, str]):
        if isinstance(value, str):
            value = value.upper()
            value = TableGraphGrid[value]
        value = int(value)
        self.c_table.set_gridy(<CTableGraphGrid>value)
        self.update_owner_database()

    def get_graph_axis(self) -> str:
        self.extract_tbl_from_database()
        return TableGraphAxis(self.c_table.get_graph_axis()).name

    def set_graph_axis(self, value: Union[TableGraphAxis, str]):
        if isinstance(value, str):
            value = value.upper()
            value = TableGraphAxis[value]
        value = int(value)
        self.c_table.set_graph_axis(<CTableGraphAxis>value)
        self.update_owner_database()

    def get_graph_alignment(self) -> str:
        self.extract_tbl_from_database()
        return TableGraphAlign(<int>(self.c_table.get_graph_alignment())).name

    def set_graph_alignment(self, value: Union[TableGraphAlign, str]):
        if isinstance(value, str):
            value = value.upper()
            value = TableGraphAlign[value]
        value = int(value)
        self.c_table.set_graph_alignment(<CTableGraphAlign>value)
        self.update_owner_database()

    def get_coefficients(self) -> List[str]:
        cdef CTableLine* c_line
        cdef CTableCell* c_cell
        self.extract_tbl_from_database()

        if self.c_table is NULL:
            return []

        py_coeffs = []
        nb_columns = self.c_table.nb_columns

        c_line = self.c_table.get_divider_line()
        for j in range(nb_columns):
            c_cell = c_line.get_cell(j, nb_columns)
            cell_type = <int>(c_cell.get_type())
            if cell_type == TableCellType.LEC and not c_cell.is_null():
                py_coeffs += [c_coeff.decode() for c_coeff in c_cell.get_coefficients_from_lec()]

        for i in range(self.c_table.lines.size()):
            c_line = self.c_table.get_line(i)
            line_type = <int>(c_line.get_line_type())
            if line_type == TableLineType.CELL:
                for j in range(nb_columns):
                    c_cell = c_line.get_cell(j, nb_columns)
                    cell_type = <int>(c_cell.get_type())
                    if cell_type == TableCellType.LEC and not c_cell.is_null():
                        py_coeffs += [c_coeff.decode() for c_coeff in c_cell.get_coefficients_from_lec()]
        
        return sorted(list(set(py_coeffs)))   

    def get_variables(self) -> List[str]:
        cdef CTableLine* c_line
        cdef CTableCell* c_cell
        self.extract_tbl_from_database()

        if self.c_table is NULL:
            return []

        py_vars = []
        nb_columns = self.c_table.nb_columns

        c_line = self.c_table.get_divider_line()
        for j in range(nb_columns):
            c_cell = c_line.get_cell(j, nb_columns)
            cell_type = <int>(c_cell.get_type())
            if cell_type == TableCellType.LEC and not c_cell.is_null():
                py_vars += [c_var.decode() for c_var in c_cell.get_variables_from_lec()]

        for i in range(self.c_table.lines.size()):
            c_line = self.c_table.get_line(i)
            line_type = <int>(c_line.get_line_type())
            if line_type == TableLineType.CELL:
                for j in range(nb_columns):
                    c_cell = c_line.get_cell(j, nb_columns)
                    cell_type = <int>(c_cell.get_type())
                    if cell_type == TableCellType.LEC and not c_cell.is_null():
                        py_vars += [c_var.decode() for c_var in c_cell.get_variables_from_lec()]
        return sorted(list(set(py_vars)))

    def get_divider(self, divider: TableLine) -> TableLine:
        cdef CTableLine* c_line

        self.extract_tbl_from_database()
        c_line = self.c_table.get_divider_line()

        divider.c_line = c_line
        divider.nb_columns = <int>(self.c_table.nb_columns)
        divider.py_parent_table = self
        return divider

    def set_divider(self, value: Union[List[str], Tuple[str]]):
        cdef CTableLine* c_line
        cdef CTableCell* c_cell
        
        c_line = self.c_table.get_divider_line()
        if c_line is NULL:
            raise RuntimeError("Cannot set divider line. No divider line found in the table.")

        if not isinstance(value, (list, tuple)):
            raise TypeError(f"Cannot update cells of the 'divider' line. Expected new content of type list "
                            f"or tuple of str.\nGot new content of type {type(value).__name__} instead.")
        if not all(isinstance(item, str) for item in value):
            raise TypeError(f"Cannot update cells of the 'divider' line. One or more items of the passed "
                            f"{type(value).__name__} are not of type str")
        
        nb_columns = self.c_table.nb_columns
        if len(value) != nb_columns:
            raise ValueError(f"Cannot update cells of the 'divider' line.\nThe length of the passed {type(value).__name__} "
                             f"({len(value)}) must be the same of the number of cells ({nb_columns}) in the table")
        
        for j, cell_content in enumerate(value):
            c_cell = c_line.get_cell(j, nb_columns)
            c_cell.set_content(cell_content.encode())

        self.update_owner_database()

    def index(self, key: str) -> int:
        cdef CTableLine* c_line
        cdef CTableCell* c_cell

        if not isinstance(key, str):
            raise TypeError(f"Expected value of type str. Got value of type {type(key).__name__} instead.")
        key = key.replace('"', '').strip()

        self.extract_tbl_from_database()
        nb_columns = self.c_table.nb_columns
        for i in range(self.c_table.lines.size()):
            c_line = self.c_table.get_line(i)
            line_type = <int>(c_line.get_line_type())
            if line_type == TableLineType.TITLE:
                content = c_line.get_cell(0, nb_columns).get_content(<bint>False).decode().strip()
                if content == key:
                    return i
            elif line_type == TableLineType.CELL:
                for j in range(nb_columns):
                    c_cell = c_line.get_cell(j, nb_columns)
                    content = c_cell.get_content(<bint>False).decode().strip()
                    if content == key:
                        return i
            else:
                pass
        
        raise ValueError(f"Content '{key}' not found in table")

    def insert(self, row: int, value: Union[str, List[str], Tuple[str], TableLine, TableLineType]):
        cdef CTableLine* c_line

        if isinstance(value, TableLineType):
            if value == TableLineType.FILES:
                self.c_table.insert_line_files(row, <bint>False)
            elif value == TableLineType.MODE: 
                self.c_table.insert_line_mode(row, <bint>False)
            elif value == TableLineType.DATE: 
                self.c_table.insert_line_date(row, <bint>False)
            elif value == TableLineType.SEP: 
                self.c_table.insert_line_separator(row, <bint>False)
            else:
                raise ValueError(f"The value of 'value' must be either TableLineType.FILES, TableLineType.MODE, "
                                 f"TableLineType.DATE or TableLineType.SEP.\nGot value {value} instead.")
        elif isinstance(value, str):
            if all(character == '-' for character in value):
                self.c_table.insert_line_separator(row, <bint>False)
            elif '|' in value:
                self.insert(row, value.split('|'))
            else:
                self.c_table.insert_title(row, value.encode(), <bint>False)
        elif isinstance(value, (list, tuple)):
            if not all(isinstance(item, str) for item in value):
                raise TypeError(f"Cannot insert line with cells at position {row}.\nOne or more items "
                                f"of the passed {type(value).__name__} are not of type str")
            c_line = self.c_table.insert_line_with_cells(row, <bint>False)
            nb_columns = self.c_table.nb_columns
            if len(value) != nb_columns:
                raise ValueError(f"The length of 'value' {len(value)} must be equal to the number of columns {nb_columns}")
            for j in range(nb_columns):
                c_line.get_cell(j, nb_columns).set_content(value[j].encode())
        elif isinstance(value, TableLine):
            line_type = <int>(value.c_line.get_line_type())
            if line_type == TableLineType.TITLE or line_type == TableLineType.CELL:
                self.insert(row, str(value))
            else:
                self.insert(row, line_type)
        
        self.update_owner_database()

    def compute(self, generalized_sample: str, nb_decimals: int=2) -> ComputedTable:
        self.extract_tbl_from_database()
        if not generalized_sample:
            raise ValueError("'generalized_sample' must not be empty")
        return ComputedTable.initialize(self.c_table, generalized_sample.encode(), nb_decimals)

    def _getitem_(self, row: int, line: TableLine) -> TableLine:
        cdef CTableLine* c_line
        c_line = self.c_table.get_line(row)

        line.c_line = c_line
        line.nb_columns = <int>(self.c_table.nb_columns)
        line.py_parent_table = self
        return line

    def _setitem_(self, row: int, value: Union[str, List[str], Tuple[str]]):
        cdef CTableLine* c_line
        cdef CTableCell* c_cell
        
        c_line = self.c_table.get_line(row)
        line_type = <int>(c_line.get_line_type())
        nb_columns = self.c_table.nb_columns
        if line_type == TableLineType.TITLE:
            if not isinstance(value, str):
                raise TypeError(f"Cannot update line {row}. Expected new content of type str. "
                                f"Got new content of type {type(value).__name__} instead.")
            c_line.get_cell(0, nb_columns).set_text(value.encode())
        elif line_type == TableLineType.CELL:
            if not isinstance(value, (list, tuple)):
                raise TypeError(f"Cannot update cells at line {row}. Expected new content of type list or tuple of str.\n"
                                f"Got new content of type {type(value).__name__} instead.")
            if not all(isinstance(item, str) for item in value):
                raise TypeError(f"Cannot update cells at line {row}. One or more items of the passed "
                                f"{type(value).__name__} are not of type str")
            if len(value) != nb_columns:
                raise ValueError(f"Cannot update cells at line {row}.\nThe length of the passed {type(value).__name__} "
                                 f"({len(value)}) must be the same of the number of cells ({nb_columns}) in the table")
            for j, cell_content in enumerate(value):
                c_cell = c_line.get_cell(j, nb_columns)
                c_cell.set_content(cell_content.encode())
        else:
            warnings.warn(f"Line of type '{TableLineType(line_type).name}' cannot be updated")
    
        self.update_owner_database()

    def _delitem_(self, row: int):
        self.c_table.remove_line(row)
        self.update_owner_database()

    def _iadd_(self, value: Union[str, List[str], Tuple[str], TableLineType, TableLine]) -> Table:
        cdef CTableLine* c_line

        if isinstance(value, TableLineType):
            if value == TableLineType.FILES:
                self.c_table.add_line_files()
            elif value == TableLineType.MODE: 
                self.c_table.add_line_mode()
            elif value == TableLineType.DATE: 
                self.c_table.add_line_date()
            elif value == TableLineType.SEP: 
                self.c_table.add_line_separator()
            else:
                raise ValueError(f"The value of 'value' must be either TableLineType.FILES, TableLineType.MODE, "
                                 f"TableLineType.DATE or TableLineType.SEP.\nGot value {value} instead.")
        elif isinstance(value, str):
            if all(character == '-' for character in value):
                self.c_table.add_line_separator()
            elif '|' in value:
                self._iadd_(value.split('|'))
            else:
                self.c_table.add_title(value.encode())
        elif isinstance(value, Iterable) and all(isinstance(item, str) for item in value):
            c_line = self.c_table.add_line_with_cells()
            nb_columns = self.c_table.nb_columns
            if len(value) != nb_columns:
                raise ValueError(f"The length of 'value' {len(value)} must be equal to the number of columns {nb_columns}")
            for j in range(nb_columns):
                c_line.get_cell(j, nb_columns).set_content(value[j].encode())
        elif isinstance(value, TableLine):
            line_type = <int>(value.c_line.get_line_type())
            if line_type == TableLineType.TITLE or line_type == TableLineType.CELL:
                self._iadd_(str(value))
            else:
                self._iadd_(line_type)

        self.update_owner_database()
        return self

    def _copy_(self, table: Table) -> Table:
        self.extract_tbl_from_database()
        del table.c_table
        table.c_table = new CTable(dereference(self.c_table))
        table.ptr_owner = <bint>True
        table.c_database = self.c_database
        table.c_table_name = self.c_table_name
        return table

    def _str_(self) -> str:
        cdef CTableLine* c_line
        cdef CTableCell* c_cell
        self.extract_tbl_from_database()

        nb_cols = self.c_table.nb_columns
        max_title_length = 0
        max_cells_length = [0] * nb_cols
        lines = []

        # lines (-1 for the divider line)
        for i in range(-1, self.c_table.lines.size()):
            c_line = self.c_table.get_line(i) if i >= 0 else self.c_table.get_divider_line()
            line_type = <int>(c_line.get_line_type())
            if line_type == TableLineType.TITLE:
                content = c_line.get_cell(0, nb_cols).get_content(<bint>True).decode()
                if len(content) > max_title_length:
                    max_title_length = <int>len(content)
                lines += [[line_type, content]]
            elif line_type == TableLineType.CELL:
                cells_content = []
                for j in range(nb_cols):
                    c_cell = c_line.get_cell(j, nb_cols)
                    quotes = <int>(c_cell.get_type()) == TableCellType.STRING
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
            line_type_name = TableLineType(line_type).name
            if line_type == TableLineType.TITLE:
                # remove newline characters
                content = ''.join(line[1].splitlines())
                s += line_type_name.ljust(5) + " | " + content.center(table_length)
            elif line_type == TableLineType.CELL:
                # i == 0 -> divider line
                s += "DIVIS" if i == 0 else line_type_name.ljust(5)
                line = line[1:]
                for j in range(nb_cols):
                    s += " | "
                    cell_align, cell_content = line[j]
                    cell_width = max_cells_length[j]
                    # remove newline characters
                    cell_content = ''.join(cell_content.splitlines())
                    if cell_align == TableCellAlign.LEFT:
                        s += cell_content.ljust(cell_width)
                    elif cell_align == TableCellAlign.RIGHT:
                        s += cell_content.rjust(cell_width)
                    elif cell_align == TableCellAlign.CENTER:
                        s += cell_content.center(cell_width)
                    # TODO: check what to do with DECIMAL cell
                    elif cell_align == TableCellAlign.DECIMAL:
                        s += cell_content.rjust(cell_width)
            elif line_type == TableLineType.SEP:
                s += '-' * 5 + " | " + '-' * table_length
            else:
                s += line_type_name.ljust(5) + " | "
            s += "\n"
        
        return s

    def __hash__(self) -> int:
        return <int>hash_value_tbl(dereference(self.c_table))
