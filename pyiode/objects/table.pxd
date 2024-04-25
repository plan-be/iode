# distutils: language = c++

from libcpp.string cimport string
from libcpp.vector cimport vector

from pyiode.common cimport (EnumLang, EnumCellType, EnumCellAlign, EnumCellFont, EnumLineType, 
                            EnumGraphAlign, EnumGraphAxis, EnumGraphGrid, EnumGraphType)


cdef extern from "cpp_api/objects/table.h":

    # declare C++ TableCell class
    cdef cppclass CTableCell "TableCell":
        # Constructor
        CTableCell(EnumCellType cell_type, string content,
                   EnumCellAlign align=EnumCellAlign.IT_LEFT,
                   bint bold=False, bint italic=False, bint underline=False) except +

        # Methods
        void free() except +

        string get_content(bint quotes) except +

        void set_text(string& text) except +
        void set_lec(string& lec) except +
        void set_content(string& content) except +

        EnumCellType get_type()
        void set_type(EnumCellType cell_type) except +

        EnumCellAlign get_align()
        void set_align(EnumCellAlign align) except +

        bint is_bold()
        void set_bold(bint value) except +

        bint is_italic()
        void set_italic(bint value) except +

        bint is_underline()
        void set_underline(bint value) except +

        vector[string] get_variables_from_lec() except +

        bint operator==(const CTableCell& other) except +


    # declare C++ TableLine class
    cdef cppclass CTableLine "TableLine":
        # Constructor
        CTableLine(EnumLineType line_type, EnumGraphType graph_type, bint axis_left) except +

        # Getters and Setters
        EnumLineType get_line_type()
        void set_line_type(EnumLineType line_type) except +

        EnumGraphType get_line_graph()
        void set_line_graph(EnumGraphType graph_type) except +

        bint is_left_axis()
        void set_line_axis(bint is_left) except +

        unsigned char get_line_pbyte()
        void set_line_pbyte(unsigned char pbyte) except +

        # Methods
        void free(int nb_cells) except +

        CTableCell* get_cell(int column, int nb_cells) except +

        bint equals(const CTableLine& other, int nb_cells) except +


    # declare C++ Table class
    # see https://cython.readthedocs.io/en/latest/src/userguide/wrapping_CPlusPlus.html#declaring-a-c-class-interface 
    cdef cppclass CTable "Table":
        # Constructor
        CTable(int nb_columns, string& def_, vector[string]& variables, bint mode, bint files, bint date) except +
        CTable(int nb_columns, string& def_, vector[string]& titles, vector[string]& lecs, bint mode, bint files, bint date) except +
        CTable(int nb_columns, string& def_, string& lecs, bint mode, bint files, bint date) except +
        # Copy constructor
        CTable(const CTable& table) except +

        # Getters and Setters
        short nb_lines()
        short nb_columns()

        string get_language()
        void set_language(EnumLang lang) except +

        EnumGraphGrid get_gridx()
        void set_gridx(EnumGraphGrid gridx) except +

        EnumGraphGrid get_gridy()
        void set_gridy(EnumGraphGrid gridy) except +

        EnumGraphAxis get_graph_axis()
        void set_graph_axis(EnumGraphAxis axis) except +

        EnumGraphAlign get_graph_alignment()
        void set_graph_alignment(EnumGraphAlign align) except +

        # Methods
        void extend() except +

        # Lines
        CTableLine* get_line(int row) except +
        CTableLine* insert_line(int pos, EnumLineType line_type, bint after) except +
        CTableLine* get_divider_line() except +

        # Title
        CTableLine* insert_title(int pos, string& title, bint after) except +
        CTableLine* add_title(string& title) except +
        string get_title(int row) except +
        void set_title(int row, string& title) except +

        # Cells
        CTableLine* insert_line_with_cells(int pos, bint after) except +
        CTableLine* add_line_with_cells() except +

        # Separator
        CTableLine* insert_line_separator(int pos, bint after) except +
        CTableLine* add_line_separator() except +

        # Mode
        CTableLine* insert_line_mode(int pos, bint after) except +
        CTableLine* add_line_mode() except +

        # Files
        CTableLine* insert_line_files(int pos, bint after) except +
        CTableLine* add_line_files() except +

        # Date
        CTableLine* insert_line_date(int pos, bint after) except +
        CTableLine* add_line_date() except +

        # Free
        void delete_line(int row) except +
        void free_line(int row) except +
        void free_cell(int row, int column) except +

        # Equal
        bint operator==(const CTable& other) except +
    
    size_t hash_value(CTable&)
