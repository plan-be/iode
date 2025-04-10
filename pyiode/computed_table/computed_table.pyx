from libcpp.string cimport string
from libcpp.vector cimport vector

from pyiode.util cimport IODE_IS_A_NUMBER
from pyiode.time.period cimport PERIOD
from pyiode.objects.table cimport CTable
from pyiode.common cimport IODE_NAN
from pyiode.common cimport TableGraphAlign as CTableGraphAlign 
from pyiode.common cimport TableGraphAxis as CTableGraphAxis 
from pyiode.common cimport TableGraphGrid as CTableGraphGrid 
from pyiode.common cimport TableGraphType as CTableGraphType
from pyiode.computed_table.computed_table cimport CComputedTable, COL, COL_ctoa, COL_NOP

import numpy as np


cdef class ComputedTable:
    cdef CComputedTable* c_computed_table

    def __cinit__(self):
        self.c_computed_table = NULL

    def __del__(self):
        if self.c_computed_table is not NULL:
            del self.c_computed_table

    @staticmethod
    cdef ComputedTable initialize(CTable* c_table, string generalized_sample, int nb_decimals):
        # Fast call to __new__() that bypasses the __init__() constructor.
        cdef ComputedTable wrapper = ComputedTable.__new__(ComputedTable)
        if c_table is NULL:
            warnings.warn("Could not initialize the computed table. The CTable pointer is NULL.")
            return None
        wrapper.c_computed_table = new CComputedTable(c_table, generalized_sample, nb_decimals)
        return wrapper

    def get_nb_decimals(self) -> int:
        return self.c_computed_table.get_nb_decimals()

    def set_nb_decimals(self, value: int):
        self.c_computed_table.set_nb_decimals(value)

    def get_nb_lines(self) -> int:
        return self.c_computed_table.get_nb_lines()

    def get_lines(self) -> List[str]:
        return [self.c_computed_table.get_line_name(i).decode() 
                for i in range(self.get_nb_lines())]

    def get_nb_columns(self) -> int:
        return self.c_computed_table.get_nb_columns()

    def get_columns(self) -> List[str]:
        return [self.c_computed_table.get_column_name(j).decode() 
                for j in range(self.get_nb_columns())]

    def get_nb_files(self) -> int:
        return self.c_computed_table.get_nb_files()

    def get_files(self) -> List[str]:
        return [item.decode() for item in self.c_computed_table.get_list_files()]

    def get_nb_operations_between_files(self) -> int:
        return self.c_computed_table.get_nb_files_ops()

    def get_nb_periods(self) -> int:
        return self.c_computed_table.get_nb_periods()

    def get_sample(self) -> Sample:
        cdef CSample* c_sample = self.c_computed_table.get_sample()
        return Sample._from_ptr(c_sample, <bint>False)

    def get_title(self) -> str:
        return self.c_computed_table.get_title().decode()

    def get_ymin(self) -> float:
        return self.c_computed_table.get_ymin()

    def get_ymax(self) -> float:
        self.c_computed_table.get_ymax()

    def get_gridx(self) -> TableGraphGrid:
        return TableGraphGrid(<int>(self.c_computed_table.get_grid_xaxis()))

    def get_gridy(self) -> TableGraphGrid:
        return TableGraphGrid(<int>(self.c_computed_table.get_grid_yaxis()))

    def get_graph_axis(self) -> TableGraphAxis:
        return TableGraphAxis(self.c_computed_table.get_graph_axis())

    def get_graph_alignment(self) -> TableGraphAlign:
        return TableGraphAlign(<int>(self.c_computed_table.get_alignement()))

    def plotting_series_name(self, row: int, op_files: int) -> str:
        cdef int ch = ord('f')
        cdef COL col
        cdef bytes b_file_op_name
        
        # see function APIGraphLineTitle() (from api/k_graph.c)
        name: str = self.c_computed_table.get_line_name(row).decode()
        if self.get_nb_operations_between_files():
            col = self.c_computed_table.files_ops[op_files]
            if self.get_nb_files() > 1 or col.cl_opf != COL_NOP:
                b_file_op_name = COL_ctoa(&col, ch, 0, self.get_nb_files())
                name += " " + b_file_op_name.decode()
        return name

    def plotting_series_values(self, row: int, op_files: int) -> Tuple[np.ndarray, np.ndarray]:
        cdef int pos
        cdef COL column
        cdef PERIOD c_period
        cdef double c_value

        sample: Sample = self.get_sample()
        if sample.is_undefined():
            warnings.warn("The sample of the computed table is undefined. "
                          "Cannot get the plotting series values.")
            return None, None
        first_period: Period = sample.get_start()

        # See function T_GraphLine() (k_graph.c)
        x = np.asarray(sample.get_period_list(astype='float'))
        y = np.full(self.get_nb_periods(), np.nan)

        col_val = 0
        for col in range(1, self.c_computed_table.columns.cl_nb, 2):
            column = self.c_computed_table.columns.cl_cols[col]
            pos = self.c_computed_table.find_file_op(column)
            if pos == op_files:
                c_period = column.cl_per[0]
                period = Period(c_period.p_y, chr(c_period.p_p), c_period.p_s)
                period_pos = period.difference(first_period)
                c_value = self.c_computed_table.get_value(row, col_val, <bint>True)
                y[period_pos] = c_value if IODE_IS_A_NUMBER(c_value) else np.nan
            col_val += 1

        return x, y

    def is_editable(self, row: int, column: int) -> bool:
        return self.c_computed_table.is_editable(row, column)

    def to_numpy(self) -> np.ndarray:
        # get data from the C++ computed table
        data = [[self.c_computed_table.get_value(i, j, <bint>True) for j in range(self.get_nb_columns())] 
                for i in range(self.get_nb_lines())]
        data = np.ascontiguousarray(data, dtype=float)
        # replace IODE NaN values by numpy or pandas values
        data = np.where(data < IODE_NAN * (1.0 - 1e-30), np.nan, data)
        return data

    def print_to_file(self, destination_file: str, format: str=None):
        cdef char c_format = b'\0'
        if format is not None:
            c_format = format.encode('utf-8')[0]
        self.c_computed_table.print_to_file(destination_file.encode('utf-8'), c_format)

    def _getitem_(self, row: int, column: int) -> float:
        cdef double c_value
        c_value = self.c_computed_table.get_value(row, column, <bint>True)
        return c_value if IODE_IS_A_NUMBER(c_value) else np.nan

    def _setitem_(self, row: int, column: int, value: float):
        cdef double c_value
        c_value = value if not np.isnan(value) else IODE_NAN
        self.c_computed_table.set_value(row, column, value, <bint>False)

    def cell_value_to_str(self, row: int, column: int) -> str:
        nb_decimals: int = self.get_nb_decimals()
        value: float = self.c_computed_table.get_value(row, column, <bint>True)
        return f"{value:.{nb_decimals}f}" if IODE_IS_A_NUMBER(value) else NAN_REP
