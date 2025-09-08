# distutils: language = c++

from libcpp.string cimport string
from libcpp.vector cimport vector

from pyiode.common cimport (TableLang, TableCellType, TableCellAlign, TableCellFont, TableLineType, 
                            TableGraphAlign, TableGraphAxis, TableGraphGrid, TableGraphType)

cdef extern from "api/all.h":
    # Define the TCELL structure
    cdef struct TCELL:
        char* content
        char  type
        char  attribute
        char  pad[2]

    # Define the TLINE structure
    cdef struct TLINE:
        char*         cells
        char          type
        char          graph_type
        unsigned char right_axis
        unsigned char unused
        char          pad[1]

    # Define the TBL structure
    cdef struct TBL:
        short  language
        short  repeat_columns
        short  nb_columns
        short  nb_lines
        TLINE  divider_line
        TLINE* lines
        float  z_min
        float  z_max
        float  y_min
        float  y_max
        char   attribute
        char   chart_box
        char   chart_shadow
        char   chart_gridx
        char   chart_gridy
        char   chart_axis_type
        char   text_alignment
        char   pad[13]


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
        void free() except +

        bint is_null()

        string get_content(bint quotes) except +

        void set_text(string& text) except +
        void set_lec(string& lec) except +
        void set_content(string& content) except +

        TableCellType get_type()
        void set_type(TableCellType cell_type) except +

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


    # declare C++ TableLine class
    cdef cppclass CTableLine "TableLine":
        # Constructor
        CTableLine(TableLineType line_type, TableGraphType graph_type, bint axis_left) except +
        # Copy constructor
        CTableLine(CTableLine& other, int nb_cells) except +

        # Getters and Setters
        TableLineType get_line_type()

        TableGraphType get_line_graph()
        void set_line_graph(TableGraphType graph_type) except +

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
        short  nb_columns
        short  nb_lines

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
        void extend() except +

        # Lines
        CTableLine* get_line(int row) except +
        CTableLine* insert_line(int pos, TableLineType line_type, bint after) except +
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
