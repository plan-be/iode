# distutils: language = c++

from libcpp.string cimport string
from libcpp.vector cimport vector

from pyiode.common cimport (TableLang, TableCellType, TableCellAlign, TableCellFont, TableLineType, 
                            TableGraphAlign, TableGraphAxis, TableGraphGrid, TableGraphType)


cdef extern from "cpp_api/objects/table.h":

    # declare C++ TableCell class
    cdef cppclass CTableCell "TableCell":
        # Constructor
        CTableCell(TableCellType cell_type, string content,
                   TableCellAlign align=TableCellAlign.TABLE_CELL_LEFT,
                   bint bold=False, bint italic=False, bint underline=False) except +
        # Copy constructor
        CTableCell(CTableCell& other) except +

        # Methods
        bint is_null()

        TableCellType get_type()

        string get_content(bint quotes) except +

        void set_text(string& text) except +
        void set_lec(string& lec) except +
        void set_content(string& content) except +

        TableCellAlign get_align()
        void set_align(TableCellAlign align) except +

        bint is_bold()
        void set_bold(bint value) except +

        bint is_italic()
        void set_italic(bint value) except +

        bint is_underline()
        void set_underline(bint value) except +

        vector[string] get_variables_from_lec() except +
        vector[string] get_coefficients_from_lec() except +

        bint operator==(const CTableCell& other) except +

    size_t hash_value(CTableCell&)


    # declare C++ TableLine class
    cdef cppclass CTableLine "TableLine":
        vector[CTableCell] cells
        bint right_axis

        # Constructor
        CTableLine(TableLineType line_type, TableGraphType graph_type, bint axis_left) except +

        # Getters and Setters
        TableLineType get_type() except +
        TableGraphType get_graph_type() except +
        void set_graph_type(TableGraphType graph_type) except +

        # Methods
        bint operator==(const CTableLine& other) except +

    size_t hash_value(CTableLine&)

    # declare C++ Table class
    # see https://cython.readthedocs.io/en/latest/src/userguide/wrapping_CPlusPlus.html#declaring-a-c-class-interface 
    cdef cppclass CTable "Table":
        short language
        short repeat_columns
        short nb_columns
        short nb_lines
        CTableLine divider_line
        vector[CTableLine] lines
        float z_min
        float z_max
        float y_min
        float y_max
        char attribute
        char chart_box
        char chart_shadow
        char chart_gridx
        char chart_gridy
        char chart_axis_type
        char text_alignment

        # Constructor
        CTable(int nb_columns, string& def_, vector[string]& variables, bint mode, bint files, bint date) except +
        CTable(int nb_columns, string& def_, vector[string]& titles, vector[string]& lecs, bint mode, bint files, bint date) except +
        CTable(int nb_columns, string& def_, string& lecs, bint mode, bint files, bint date) except +
        # Copy constructor
        CTable(const CTable& table) except +

        string get_language()
        void set_language(TableLang lang) except +

        TableGraphGrid get_gridx()
        void set_gridx(TableGraphGrid gridx) except +

        TableGraphGrid get_gridy()
        void set_gridy(TableGraphGrid gridy) except +

        TableGraphAxis get_graph_axis()
        void set_graph_axis(TableGraphAxis axis) except +

        TableGraphAlign get_graph_alignment()
        void set_graph_alignment(TableGraphAlign align) except +

        # Methods
        # Lines
        CTableLine* add_line(TableLineType line_type) except +
        CTableLine* insert_line(int pos, TableLineType line_type, bint after) except +
        void remove_line(int row) except +

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

        # Equal
        bint operator==(const CTable& other) except +
    
    size_t hash_value(CTable&)
