#pragma once

#include "api/constants.h"
#include "api/b_args.h"
#include "api/k_super.h"
#include "api/objs/kdb.h"           // KDB
#include "api/objs/pack.h"
#include "api/objs/identities.h"    // Identity
#include "api/objs/grep.h"

#include <string>
#include <vector>
#include <bitset>

/*----------------------- ENUMS ----------------------------*/

enum TableCellFont
{
    TABLE_CELL_NORMAL = 0,
    TABLE_CELL_BOLD = 1,
    TABLE_CELL_ITALIC = 2,
    TABLE_CELL_UNDERLINE = 4
};

enum TableCellAlign
{
	TABLE_CELL_CENTER = 8,
	TABLE_CELL_LEFT = 16,
	TABLE_CELL_RIGHT = 32,
	TABLE_CELL_DECIMAL = 64
};

enum TableAscii
{
    TABLE_ASCII_ENGLISH = 100,
    TABLE_ASCII_DUTCH,
    TABLE_ASCII_FRENCH,
    TABLE_ASCII_DIM,
    TABLE_ASCII_DIVIDER,
	TABLE_ASCII_LINE_FILES,
	TABLE_ASCII_LINE_MODE,
    TABLE_ASCII_OPEN,
    TABLE_ASCII_CLOSE,
    TABLE_ASCII_BREAK,
	TABLE_ASCII_LINE_CELL,
	TABLE_ASCII_LINE_SEP,
    TABLE_ASCII_BOLD_LINE,
	TABLE_ASCII_LINE_TITLE,
	TABLE_ASCII_CELL_LEC,
	TABLE_ASCII_CELL_STRING,
	TABLE_ASCII_LINE_DATE,
    TABLE_ASCII_YMIN,
    TABLE_ASCII_YMAX,            
    TABLE_ASCII_ZMIN,            
    TABLE_ASCII_ZMAX,            
    TABLE_ASCII_XGRID,           
    TABLE_ASCII_YGRID,           
    TABLE_ASCII_BOX,             
    TABLE_ASCII_AXIS,            
    TABLE_ASCII_ALIGN,               
    TABLE_ASCII_LEFT_AXIS,           
    TABLE_ASCII_RIGHT_AXIS,          
    TABLE_ASCII_GRAPH_LINE,          
    TABLE_ASCII_GRAPH_BAR,           
    TABLE_ASCII_GRAPH_SCATTER      
};

enum TableLang
{
    TABLE_ENGLISH = TABLE_ASCII_ENGLISH,
    TABLE_DUTCH = TABLE_ASCII_DUTCH,
    TABLE_FRENCH = TABLE_ASCII_FRENCH
};

const static int IODE_NB_LANGS = 3;

enum TableLineType
{
	TABLE_LINE_FILES = TABLE_ASCII_LINE_FILES,
	TABLE_LINE_MODE  = TABLE_ASCII_LINE_MODE,
	TABLE_LINE_CELL  = TABLE_ASCII_LINE_CELL,
	TABLE_LINE_SEP   = TABLE_ASCII_LINE_SEP,
	TABLE_LINE_TITLE = TABLE_ASCII_LINE_TITLE,
	TABLE_LINE_DATE  = TABLE_ASCII_LINE_DATE
};

enum TableCellType
{
	TABLE_CELL_LEC    = TABLE_ASCII_CELL_LEC,
	TABLE_CELL_STRING = TABLE_ASCII_CELL_STRING
};

const static std::map<std::string, TableLineType> m_line_type =
{
	{"Cells", TABLE_LINE_CELL},
	{"Separator", TABLE_LINE_SEP},
	{"Title", TABLE_LINE_TITLE},
	{"Files", TABLE_LINE_FILES},
	{"Mode",  TABLE_LINE_MODE},
	{"Date",  TABLE_LINE_DATE}
};

enum TableGraphType
{
	TABLE_GRAPH_LINE,
	TABLE_GRAPH_SCATTER,
	TABLE_GRAPH_BAR
};

const static int IODE_NB_CHART_TYPES = 3;

enum TableGraphGrid
{
	TABLE_GRAPH_MAJOR,
	TABLE_GRAPH_NONE,
	TABLE_GRAPH_MINOR
};

const static int IODE_NB_AXIS_THICKS = 3;

enum TableGraphAxis
{
	TABLE_GRAPH_VALUES,
	TABLE_GRAPH_LOG,
	TABLE_GRAPH_SEMILOG,
	TABLE_GRAPH_PERCENT
};

enum TableTextAlign
{
	TABLE_TEXT_LEFT,
	TABLE_TEXT_CENTER,
	TABLE_TEXT_RIGHT
};

/*----------------------- GLOBALS ----------------------------*/

inline std::string get_line_type_as_string(TableLineType line_type)
{
	for(const auto& [key, value]: m_line_type)
		if(line_type == value) return key;
	return "";
}

const static std::vector<std::string> v_graph_chart_types = 
    { "Line chart", "Scatter chart", "Bar chart" };

const static std::vector<std::string> v_graph_axis_thicks = 
    { "Major thicks", "No grids", "Minor thicks" };

inline TableGraphType T_GRAPHDEFAULT = TableGraphType::TABLE_GRAPH_LINE;

/*----------------------- STRUCTS ----------------------------*/

using bitset_8 = std::bitset<8>;


// WARNING: C++ allows functions returning a reference to be left-values. 
//          This is currently not supported in Cython.


class TableCell 
{
private:
    char        type;       // TABLE_CELL_STRING or TABLE_CELL_LEC
    std::string content;    // if type == TABLE_CELL_STRING
    Identity*   idt;        // if type == TABLE_CELL_LEC
    char        attribute;  // TABLE_CELL_LEFT, TABLE_CELL_CENTER, TABLE_CELL_RIGHT, TABLE_CELL_BOLD,
                            // TABLE_CELL_ITALIC, TABLE_CELL_UNDERLINE, TABLE_CELL_NORMAL

public:
	TableCell(const TableCellType cell_type, const std::string& content = "", const int col=0)
        : type((char) cell_type), idt(nullptr)
    {
        if(this->type == TABLE_CELL_STRING)
        {
            this->content = content;
            this->attribute = TABLE_CELL_LEFT;
        }
        else
        {
            this->content = "";
            if(!content.empty())
            {
                try
                {
                    this->idt = new Identity(content);
                }
                catch(std::exception& e)
                {
                    kwarning(e.what());
                    this->idt = nullptr;
                }
            }
            this->attribute = (col > 0) ? TABLE_CELL_DECIMAL : TABLE_CELL_LEFT;
        }
    }

    TableCell(const TableCell& other)
    {
        type = other.type;
        attribute = other.attribute;
        content = other.content;
        if(other.idt)
            idt = new Identity(*(other.idt));
        else
            idt = nullptr;
    }

     ~TableCell()
    {
        if(idt)
            delete idt;
        idt = nullptr;
    }

    TableCell& operator=(const TableCell& other)
    {
        type = other.type;
        attribute = other.attribute;
        content = other.content;
        if(idt)
            delete idt;
        idt = nullptr;
        if(other.idt)
            idt = new Identity(*(other.idt));
        return *this;
    }

    TableCellType get_type() const
    {
        return (TableCellType) type;
    }

    bool is_null() const
    {
        if (type == TABLE_CELL_LEC && idt == NULL)
            return true;
        if (type == TABLE_CELL_STRING && content.empty())
            return true;
        return false;
    }

    TableCellAlign get_align() const
    {
        return static_cast<TableCellAlign>((int) (this->attribute / 8) * 8);
    }

    void set_align(const TableCellAlign align)
    {
        char font = ((int) this->attribute) % 8;
        this->attribute = ((char) align) + font;
    }

    bool is_bold() const
    {
        return bitset_8(this->attribute).test(0);
    }

    void set_bold(const bool value)
    {
        bitset_8 attr(this->attribute);
        attr.set(0, value);
        this->attribute = (char) attr.to_ulong();
    }

    bool is_italic() const
    {
        return bitset_8(this->attribute).test(1);
    }

    void set_italic(const bool value)
    {
        bitset_8 attr(this->attribute);
        attr.set(1, value);
        this->attribute = (char) attr.to_ulong();
    }

    bool is_underline() const
    {
        return bitset_8(this->attribute).test(2);
    }

    void set_underline(const bool value)
    {
        bitset_8 attr(this->attribute);
        attr.set(2, value);
        this->attribute = (char) attr.to_ulong();
    }

    // NOTE: for (un)packing to old binary format
    char get_attribute() const
    {
        return (char) this->attribute;
    }

    // NOTE: for (un)packing to old binary format
    void set_attribute(const char attr)
    {
        this->attribute = attr;
    }

    // If quotes is true, the content is returned between double quotes.
    std::string get_content(const bool quotes=false, const bool to_utf8=true) const;

    // to_oem set to false means that the text is already in OEM encoding
    // i.e. when reading from old binary files
    void set_text(const std::string& text, const bool to_oem=true);

    void set_lec(const std::string& lec);

    // Rule: If the content starts with a double quotes, we assume it is a string cell. 
    //       Otherwise, it is a LEC cell.
    // to_oem set to false means that the text is already in OEM encoding
    // i.e. when reading from old binary files
    void set_content(const std::string& content, const bool to_oem=true);

	CLEC* get_compiled_lec()
    {
        if(type != TABLE_CELL_LEC)
            throw std::runtime_error("Cannot get the compiled LEC. The table cell does not contain a LEC expression");

        if(!idt)
            throw std::runtime_error("Cannot get the compiled LEC. The table cell is empty");

        // see VT_edit() from o_vt.c from the old GUI
        return idt->get_compiled_lec();
    }

	std::vector<std::string> get_variables_from_lec()
    {
        CLEC* clec = get_compiled_lec();
        return get_variables_from_clec(clec);
    }

	std::vector<std::string> get_coefficients_from_lec()
    {
        CLEC* clec = get_compiled_lec();
        return get_scalars_from_clec(clec);
    }

    // Checks that a TableCell is not empty (for TEXT cells) and not "1" (for LEC cells)
    bool check_print_def() const
    {
        if(is_null()) 
            return false;
    
        if(get_type() == TABLE_CELL_LEC && get_content() == "1")
            return false;
    
        return true;
    }

    bool print_definition(int nb_columns) const;

	bool operator==(const TableCell& other) const
    {	
        if (type != other.type) 
            return false;

        if (attribute != other.attribute) 
            return false;
        
        if(type == TABLE_CELL_STRING)
            return this->content == other.content;
        else
        {
            if(idt == nullptr && other.idt == nullptr) 
                return true;
            if(idt == nullptr || other.idt == nullptr) 
                return false;
            return idt->lec == other.idt->lec;
        }
    }

    std::size_t hash() const
    {
		std::size_t seed = 0;

        hash_combine<char>(seed, type);
        if(type == TABLE_CELL_STRING)
		    hash_combine<std::string>(seed, content);
        else
        {
            if(idt)
                hash_combine<std::string>(seed, idt->lec);
        }
        hash_combine<char>(seed, attribute);
        
		return seed;
    }
};

// Custom specialization of std::hash can be injected in namespace std.
template<>
struct std::hash<TableCell>
{
    std::size_t operator()(const TableCell& cell) const noexcept
    {
        return cell.hash();
    }
};

std::size_t hash_value(const TableCell& cell);


class TableLine 
{
    char    type;                   // TABLE_LINE_FILES, TABLE_LINE_MODE, TABLE_LINE_DATE, TABLE_LINE_SEP, 
                                    // TABLE_LINE_TITLE or TABLE_LINE_CELL
    char    graph_type;             // 0=Line, 1=scatter, 2=bar (non implemented in all IODE flavours)

public:
    std::vector<TableCell> cells;   // empty if type is FILES, MODE, DATE or SEP
    bool    right_axis;             // false if values are relative to the left axis, true to the right axis

public:
	TableLine(const TableLineType line_type, 
              const TableGraphType graph_type = TableGraphType::TABLE_GRAPH_LINE, 
		      const bool axis_left = true)
    {
        this->type = (char) line_type;
        this->graph_type = (char) graph_type;
        this->right_axis = !axis_left;
    }

    TableLine(const TableLine& other)
    {
        type = other.type;
        graph_type = other.graph_type;
        right_axis = other.right_axis;
        cells.clear();
        for(const TableCell& cell : other.cells)
            cells.push_back(cell);
    }

     ~TableLine()
    {
        cells.clear();
    }

    TableLine& operator=(const TableLine& other)
    {
        type = other.type;
        graph_type = other.graph_type;
        right_axis = other.right_axis;
        cells.clear();
        for(const TableCell& cell : other.cells)
            cells.push_back(cell);
        return *this;
    }

    TableLineType get_type() const
    {
        return static_cast<TableLineType>(type);
    }

    TableGraphType get_graph_type() const
    {
        return static_cast<TableGraphType>(graph_type);
    }

    void set_graph_type(const TableGraphType graph_type)
    {
        this->graph_type = graph_type;
    }

    bool operator==(const TableLine& other) const
    {
        if (type != other.type) return false;
        if (right_axis != other.right_axis) return false;
        if (graph_type != other.graph_type) return false;

        if(type == TABLE_LINE_TITLE)
            return this->cells[0] == other.cells[0];
        else if(type == TABLE_LINE_CELL)
        {
            if(this->cells.size() != other.cells.size()) 
                return false;
            for(int col = 0; col < other.cells.size(); col++)
                if(this->cells[col] != other.cells[col])
                    return false;
            return true;
        }
        else
            // cells == NULL for FILES, MODE, LINE and DATE type
            return true;
    }

    std::size_t hash() const
    {
		std::size_t seed = 0;

		hash_combine<char>(seed, this->type);
		hash_combine<bool>(seed, this->right_axis);
		hash_combine<char>(seed, this->graph_type);

		switch(this->type)
		{
		case TABLE_LINE_TITLE:
			hash_combine<TableCell>(seed, this->cells[0]);
			break;
		case TABLE_LINE_CELL:
			for(const TableCell& cell : this->cells)
				hash_combine<TableCell>(seed, cell);
			break;
		default:
			break;
		}

		return seed;
    }
};

// Custom specialization of std::hash can be injected in namespace std.
template<>
struct std::hash<TableLine>
{
    std::size_t operator()(const TableLine& line) const noexcept
    {
        return line.hash();
    }
};

std::size_t hash_value(const TableLine& cell);


class Table 
{
    short   language = TABLE_ENGLISH;               // Output language : TABLE_ENGLISH, TABLE_FRENCH, TABLE_DUTCH
    char    chart_gridx = TABLE_GRAPH_MAJOR;        // 0 = major grids, 1 = no grids, 2 = minor + major grids
    char    chart_gridy = TABLE_GRAPH_MAJOR;        // idem
    char    chart_axis_type = TABLE_GRAPH_VALUES;   // 0=normal axis, 1=log, 2=semi-log, 3=percents (TODO: to be tested)
    char    text_alignment = TABLE_TEXT_LEFT;       // Text alignment: 0=left, 1=centered, 2=right

public:
    short   nb_columns;                         // Number of columns (of text and lec, not calculated values)
    short   repeat_columns = 0;                 // if 0, first column is frozen, otherwise, col 1 is repeated as other columns
    TableLine divider_line{TABLE_LINE_CELL};    // nb_columns TableCell's, each TableCell contains a divider
    std::vector<TableLine> lines;
    float   z_min = (float) IODE_NAN;           // Min on the right axis
    float   z_max = (float) IODE_NAN;           // Max on the right axis
    float   y_min = (float) IODE_NAN;           // Min on left axis
    float   y_max = (float) IODE_NAN;           // Max on left axis
    char    attribute = TABLE_CELL_NORMAL;      // Combination (logical &) of attributes: TABLE_CELL_BOLD, TABLE_CELL_ITALIC, TABLE_CELL_UNDERLINE, TABLE_CELL_CENTER,
                                                // TABLE_CELL_DECIMAL, TABLE_CELL_LEFT and TABLE_CELL_RIGHT
    char    chart_box = 0;                      // 1 to surround the chart by a box
    char    chart_shadow = 0;                   // 1 to place a shadow behind the chart

public:
    Table(const int nb_columns);

    Table(const int nb_columns, const std::string& def, const std::vector<std::string>& vars, 
		  bool mode = false, bool files = false, bool date = false);

    Table(const int nb_columns, const std::string& def, const std::vector<std::string>& titles, 
		  const std::vector<std::string>& lecs, bool mode = false, bool files = false, bool date = false);

    Table(const int nb_columns, const std::string& def, const std::string& lecs, 
		  bool mode = false, bool files = false, bool date = false);

	Table(const Table& table);

    ~Table();

    // -------- GETTERS AND SETTERS --------

    TableLang get_language() const
    {
        return (TableLang) language;
    }

    std::string get_language_as_string() const
    {
        return v_table_langs.at(language - TABLE_ENGLISH);
    }

    void set_language(const TableLang lang)
    {
        language = (short) lang;
    }

    TableGraphGrid get_gridx() const
    {
        return static_cast<TableGraphGrid>(chart_gridx);
    }

    void set_gridx(const TableGraphGrid gridx)
    {
        chart_gridx = (char) (gridx - TABLE_GRAPH_MAJOR);
    }

    TableGraphGrid get_gridy() const
    {
        return static_cast<TableGraphGrid>(chart_gridy);
    }

    void set_gridy(const TableGraphGrid gridy)
    {
        chart_gridy = (char) gridy;
    }

    TableGraphAxis get_graph_axis() const
    {
        return static_cast<TableGraphAxis>(chart_axis_type);
    }

    void set_graph_axis(const TableGraphAxis axis)
    {
        chart_axis_type = (char) axis;
    }

    TableTextAlign get_text_alignment() const
    {
        return static_cast<TableTextAlign>(text_alignment);
    }

    void set_text_alignment(const TableTextAlign align)
    {
        text_alignment = (char) align;
    }

	// -------- LINES --------

	TableLine* append_line(const TableLineType line_type);

	TableLine* insert_line(const int pos, const TableLineType line_type, const bool after = true);

	void remove_line(const int row);

    // -------- TITLE --------

    // we assume that title string is written in UTF8 format
    TableLine* insert_title(const int pos, const std::string& title, const bool after)
    {
        TableLine* title_line = insert_line(pos, TABLE_LINE_TITLE, after);
        title_line->cells[0].set_text(title);
        return title_line;
    }

    TableLine* add_title(const std::string& title)
    {
        TableLine* title_line = append_line(TABLE_LINE_TITLE);
        title_line->cells[0].set_text(title);
        return title_line;
    }

    std::string get_title(const int row)
    {
        TableLine& line = lines[row];
        if(line.get_type() != TableLineType::TABLE_LINE_TITLE) 
            throw std::invalid_argument("Cannot get title at line index " + std::to_string(row) + ".\n" +
                "Line at index " + std::to_string(row) + " is not a TITLE line but of type " + 
                get_line_type_as_string(line.get_type()) + ".");
        return line.cells[0].get_content(false);
    }

    std::string get_title()
    {
        for(TableLine& line : lines)
            if(line.get_type() == TableLineType::TABLE_LINE_TITLE)
                return line.cells[0].get_content(false);
        return "";
    }

    // we assume that title string is written in UTF8 format
    void set_title(const int row, const std::string title)
    {
        TableLine& line = lines[row];
        if(line.get_type() != TableLineType::TABLE_LINE_TITLE) 
            throw std::invalid_argument("Cannot set table title at index " + std::to_string(row) + ".\n" + 
                "Line at index " + std::to_string(row) + " is not a TITLE line but of type " + 
                get_line_type_as_string(line.get_type()) + ".");
        line.cells[0].set_text(title);
    }

    // -------- CELLS --------

    TableLine* insert_line_with_cells(const int pos, const bool after)
    {
        return insert_line(pos, TABLE_LINE_CELL, after);
    }

    TableLine* add_line_with_cells()
    {
        return append_line(TABLE_LINE_CELL);
    }

    // -------- SEPARATOR --------

    TableLine* insert_line_separator(const int pos, const bool after)
    {
        return insert_line(pos, TABLE_LINE_SEP, after);
    }

    TableLine* add_line_separator()
    {
        return append_line(TABLE_LINE_SEP);
    }

    // -------- MODE --------

    TableLine* insert_line_mode(const int pos, const bool after)
    {
        return insert_line(pos, TABLE_LINE_MODE, after);
    }

    TableLine* add_line_mode()
    {
        return append_line(TABLE_LINE_MODE);
    }

    // -------- FILES --------

    TableLine* insert_line_files(const int pos, const bool after)
    {
        return insert_line(pos, TABLE_LINE_FILES, after);
    }

    TableLine* add_line_files()
    {
        return append_line(TABLE_LINE_FILES);
    }

    // -------- DATE --------

    TableLine* insert_line_date(const int pos, const bool after)
    {
        return insert_line(pos, TABLE_LINE_DATE, after);
    }

    TableLine* add_line_date()
    {
        return append_line(TABLE_LINE_DATE);
    }

    // -------- MISC --------

    bool print_definition() const
    {
        W_printf((char*) "\n.tl\n");

        /* lines */
        for(const TableLine& line : lines) 
        {
            switch(line.get_type()) 
            {
                case TABLE_LINE_CELL :
                    for(const TableCell& cell: line.cells)
                        cell.print_definition(1);
                    W_printf((char*) "\n");
                    break;
    
                case TABLE_LINE_TITLE :
                    line.cells[0].print_definition(nb_columns);
                    W_printf((char*) "\n");
                    break;
    
                case TABLE_LINE_SEP   :
                case TABLE_ASCII_BOLD_LINE  :
                    W_printf((char*) ".tl\n");
                    break;
                case TABLE_LINE_MODE  :
                    W_printfRepl((char*) "&%dL[MODE]\n", nb_columns);
                    break;
                case TABLE_LINE_DATE  :
                    W_printfRepl((char*) "&%dL[DATE]\n", nb_columns);
                    break;
                case TABLE_LINE_FILES :
                    W_printfRepl((char*) "&%dL[FILES]\n", nb_columns);
                    break;
    
                default       :
                    break;
            }
        }
    
        /* div */
        bool print_divider = false;
        for(const TableCell& cell: divider_line.cells)
            if(cell.check_print_def())
            {
                print_divider = true;
                break;
            }
        
        if(print_divider) 
        {
            W_printfRepl((char*) ".tl\n&%dC%cbColumn divisors%cB\n.tl\n", nb_columns, A2M_ESCCH, A2M_ESCCH); /* JMP 14-06-96 */
            for(const TableCell& cell: divider_line.cells)
                cell.print_definition(1);
        }
    
        W_printf((char*) "\n.tl\n");
        return true;
    }

    // -------- EQUAL --------

    bool operator==(const Table& other) const
    {
        if(this->get_language() != other.get_language()) return false;
        if(this->repeat_columns != other.repeat_columns) return false;
        if(this->nb_columns != other.nb_columns) return false;
        
        int nb_columns = this->nb_columns;
        TableLine* div1 = (TableLine*) &this->divider_line;
        TableLine* div2 = (TableLine*) &other.divider_line;
        if(*div1 != *div2) return false;
        
        if(this->lines.size() != other.lines.size()) return false;
        TableLine* line1;
        TableLine* line2;
        for (int i = 0; i < this->lines.size(); i++)
        {
            line1 = (TableLine*) &this->lines[i];
            line2 = (TableLine*) &other.lines[i];
            if (*line1 != *line2) 
                return false;
        }

        if(this->z_min != other.z_min) return false;
        if(this->z_max != other.z_max) return false;
        if(this->y_min != other.y_min) return false;
        if(this->y_max != other.y_max) return false;
        if(this->attribute != other.attribute) return false;
        if(this->chart_box != other.chart_box) return false;
        if(this->chart_shadow != other.chart_shadow) return false;
        if(this->get_gridx() != other.get_gridy()) return false;
        if(this->get_gridy() != other.get_gridy()) return false;
        if(this->get_graph_axis() != other.get_graph_axis()) return false;
        if(this->get_text_alignment() != other.get_text_alignment()) return false;

        return true;
    }

    // ------- HASH --------

    std::size_t hash() const
    {
		std::size_t seed = 0;

		hash_combine<short>(seed, this->language);
		hash_combine<short>(seed, this->repeat_columns);
		hash_combine<short>(seed, this->nb_columns);

		hash_combine<TableLine>(seed, this->divider_line);
		for(const TableLine& line : this->lines)
			hash_combine<TableLine>(seed, line);

		hash_combine<float>(seed, this->z_min);
		hash_combine<float>(seed, this->z_max);
		hash_combine<float>(seed, this->y_min);
		hash_combine<float>(seed, this->y_max);
		hash_combine<char>(seed, this->attribute);
		hash_combine<char>(seed, this->chart_box);
		hash_combine<char>(seed, this->chart_shadow);
		hash_combine<char>(seed, this->chart_gridx);
		hash_combine<char>(seed, this->chart_gridy);
		hash_combine<char>(seed, this->chart_axis_type);
		hash_combine<char>(seed, this->text_alignment);

		return seed;
    }
};

// Custom specialization of std::hash can be injected in namespace std.
template<>
struct std::hash<Table>
{
    std::size_t operator()(const Table& table) const noexcept
    {
		return table.hash();
    }
};

std::size_t hash_value(const Table& table);

/*----------------------- STRUCTS ----------------------------*/

struct CKDBTables : public CKDBTemplate<Table>
{
    // global or standalone database
    CKDBTables(const bool is_global) : CKDBTemplate(TABLES, is_global) {}

    // shallow copy database
    CKDBTables(CKDBTables* db_parent, const std::string& pattern = "*") 
        : CKDBTemplate(db_parent, pattern) {}

    // copy constructor
    CKDBTables(const CKDBTables& other): CKDBTemplate(other) {}

    // NOTE: get_obj() and set_obj() methods to be replaced by operator[] when 
    //       k_objs will be changed to std::map<std::string, T>
    //       T& operator[](const std::string& name)

    Table* get_obj(const SWHDL handle) const override;
    Table* get_obj(const std::string& name) const override;

    bool set_obj(const std::string& name, const Table* value) override;

    bool load_asc(const std::string& filename) override;
    bool save_asc(const std::string& filename) override;

    char* dde_create_obj_by_name(const std::string& name, int* nc, int* nl) override;
    char* dde_create_table(const std::string& name, char *ismpl, int *nc, int *nl, int nbdec);

    bool print_obj_def(const std::string& name) override;

private:
    bool grep_obj(const std::string& name, const SWHDL handle, 
        const std::string& pattern, const bool ecase, const bool forms, 
        const bool texts, const char all) const override;
    
    void update_reference_db() override;
};

/*----------------------- GLOBALS ----------------------------*/
// unique_ptr -> automatic memory management
//            -> no need to delete KDB workspaces manually
inline std::unique_ptr<CKDBTables> global_ws_tbl = std::make_unique<CKDBTables>(true);

/*----------------------- FUNCS ----------------------------*/

Table* K_tunpack(char*, char* name = NULL);
Table* K_tptr(KDB* kdb, char* name);


// returns an allocated Table
inline Table* KTVAL(CKDBTables* kdb, const std::string& name) 
{
    return kdb->get_obj(name);
}

/*----------------------- MACROS ----------------------------*/

#define T_NC(tbl)           (tbl->nb_columns)
#define T_NL(tbl)           (tbl->lines.size())
#define T_LANG(tbl)         (tbl->get_language())
#define T_L(tbl)            (tbl->lines)
#define T_C(tbl, i, j)      ((tbl->lines[i]).cells[j])
