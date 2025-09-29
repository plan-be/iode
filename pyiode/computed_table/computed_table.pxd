# distutils: language = c++

from collections.abc import Iterable
from typing import Union, Tuple, List, Dict, Optional

# Import necessary C++ types/classes
from libcpp.string cimport string
from libcpp.vector cimport vector

from pyiode.common cimport TableGraphAxis, TableGraphGrid, TableTextAlign
from pyiode.time.period cimport CPeriod
from pyiode.time.sample cimport CSample
from pyiode.objects.table cimport CTable


cdef extern from "api/all.h":
    cdef int    COL_NOP             # No operation

    ctypedef struct COL:
        short    cl_opy              # operator on periods => cl_per[0] cl_opy cl_per[1])
        CPeriod  cl_per[2]           # period 1 , period 2
        short    cl_opf              # operator on files => cl_fnb[0] cl_opf cl_fnb[1]
        short    cl_fnb[2]           # position in K_RWS of file1 and file2 (starts at 1)
        double   cl_val[2][2]        # computed values of the LEC formulas on periods / files => max 4 values see table below
        double   cl_res              # computed value (v00 opp v10) opf (v01 opp v11)

    ctypedef struct COLS:
        int      cl_nb               # Number of columns
        COL      *cl_cols            # Pointer to the first COL struct

    char* COL_ctoa(COL*, int, int, int)     # COL to ASCII


cdef extern from "cpp_api/computed_table/computed_table.h":
    cdef cppclass CComputedTable "ComputedTable":
        CTable*     ref_table
        int         dim
        COLS*       columns
        vector[COL] files_ops

        CComputedTable(CTable* table, const string& gsample, const int nb_decimals) except +

        # Properties
        int get_nb_decimals()
        void set_nb_decimals(const int nb_decimals) except +
        int get_nb_lines()
        int get_nb_columns()
        int get_nb_files()
        int get_nb_periods()
        int get_nb_files_ops()
        TableGraphAxis get_graph_axis()
        TableGraphGrid get_grid_xaxis()
        TableGraphGrid get_grid_yaxis()
        TableTextAlign get_alignement()
        double get_ymin()
        double get_ymax()
        CSample* get_sample()
        vector[string] get_list_files()
        string get_title() except +

        # Methods
        int find_file_op(const COL& col) except +
        string get_line_name(const int line) except +
        string get_column_name(const int col) except +
        double get_value(const int line, const int col, const bint full_precision) except +
        bint is_editable(const int line, const int col) except +
        void set_value(const int line, const int col, const double value, bint check_if_editable) except +
        void print_to_file(string destination_file, char format_) except +
