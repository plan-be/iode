# distutils: language = c++

from collections.abc import Iterable
from typing import Union, Tuple, List, Dict, Optional

# Import necessary C++ types/classes
from libcpp.string cimport string
from libcpp.vector cimport vector

from pyiode.time.sample cimport CSample
from pyiode.objects.table cimport CTable


cdef extern from "cpp_api/computed_table/computed_table.h":
    cdef cppclass CComputedTable "ComputedTable":
        CComputedTable(CTable* table, const string& gsample, const int nb_decimals) except +

        # Properties
        int get_nb_decimals()
        void set_nb_decimals(const int nb_decimals) except +
        int get_nb_lines()
        int get_nb_columns()
        int get_nb_files()
        int get_nb_periods()
        int get_nb_files_ops()
        CSample* get_sample()
        vector[string] get_list_files()
        string get_title() except +

        # Methods
        string get_line_name(const int line) except +
        string get_column_name(const int col) except +
        double get_value(const int line, const int col, const bint full_precision) except +
        bint is_editable(const int line, const int col) except +
        void set_value(const int line, const int col, const double value, bint check_if_editable) except +
        void print_to_file(string destination_file, char format_) except +
