#pragma once

#include "api/constants.h"
#include "api/k_super.h"
#include "api/objs/kdb.h"           // KDB
#include "api/objs/identities.h"    // Identity

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

enum TableGraphAlign
{
	TABLE_GRAPH_LEFT,
	TABLE_GRAPH_CENTER,
	TABLE_GRAPH_RIGHT
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


struct TBL 
{
    short   language;           // Output language : TABLE_ENGLISH, TABLE_FRENCH, TABLE_DUTCH
    short   repeat_columns;     // if 0, first column is frozen, otherwise, col 1 is repeated as other columns
    short   nb_columns;         // Number of columns (of text and lec, not calculated values)
    TableLine divider_line;     // nb_columns TableCell's, each TableCell contains a divider
    std::vector<TableLine> lines;   
    float   z_min;              // Min on the right axis
    float   z_max;              // Max on the right axis
    float   y_min;              // Min on left axis
    float   y_max;              // Max on left axis
    char    attribute;          // Combination (logical &) of attributes: TABLE_CELL_BOLD, TABLE_CELL_ITALIC, TABLE_CELL_UNDERLINE, TABLE_CELL_CENTER,
                                // TABLE_CELL_DECIMAL, TABLE_CELL_LEFT and TABLE_CELL_RIGHT
    char    chart_box;          // 1 to surround the chart by a box
    char    chart_shadow;       // 1 to place a shadow behind the chart
    char    chart_gridx;        // 0 = major grids, 1 = no grids, 2 = minor + major grids
    char    chart_gridy;        // idem
    char    chart_axis_type;    // 0=normal axis, 1=log, 2=semi-log, 3=percents (TODO: to be tested)
    char    text_alignment;     // Text alignment: 0=left, 1=centered, 2 = right

public:
    TBL(const int dim): nb_columns(dim), divider_line(TABLE_LINE_CELL)
    {        
        this->language = TABLE_ENGLISH;
        this->repeat_columns = 0;

        this->z_min = (float) IODE_NAN;
        this->z_max = (float) IODE_NAN;
        this->y_min = (float) IODE_NAN;
        this->y_max = (float) IODE_NAN;
        this->attribute = TABLE_CELL_NORMAL;
        this->text_alignment = TABLE_GRAPH_LEFT;
        this->chart_box = 0;
        this->chart_shadow = 0;
        this->chart_gridx = TABLE_GRAPH_MAJOR;
        this->chart_gridy = TABLE_GRAPH_MAJOR;
        this->chart_axis_type = TABLE_GRAPH_VALUES;

        this->divider_line.cells.push_back(TableCell(TABLE_CELL_LEC, "1", 0));
        for(int j = 1; j < dim; j++) 
            this->divider_line.cells.push_back(TableCell(TABLE_CELL_LEC, "", j));
    }
};

// Custom specialization of std::hash can be injected in namespace std.
template<>
struct std::hash<TBL>
{
    std::size_t operator()(const TBL& table) const noexcept
    {
		std::size_t seed = 0;

		hash_combine<short>(seed, table.language);
		hash_combine<short>(seed, table.repeat_columns);
		hash_combine<short>(seed, table.nb_columns);

		hash_combine<TableLine>(seed, table.divider_line);
		for(const TableLine& line : table.lines)
			hash_combine<TableLine>(seed, line);

		hash_combine<float>(seed, table.z_min);
		hash_combine<float>(seed, table.z_max);
		hash_combine<float>(seed, table.y_min);
		hash_combine<float>(seed, table.y_max);
		hash_combine<char>(seed, table.attribute);
		hash_combine<char>(seed, table.chart_box);
		hash_combine<char>(seed, table.chart_shadow);
		hash_combine<char>(seed, table.chart_gridx);
		hash_combine<char>(seed, table.chart_gridy);
		hash_combine<char>(seed, table.chart_axis_type);
		hash_combine<char>(seed, table.text_alignment);

		return seed;
    }
};

std::size_t hash_value(const TBL& table);

/*----------------------- FUNCS ----------------------------*/

TBL* K_tptr(KDB* kdb, char* name);

/* k_tbl.c */
TBL *T_create(int );
void T_free(TBL *);
char *T_div_cont_tbl(TBL *, int, int );
int T_append_line(TBL*, int);
int T_insert_line(TBL*, int, int, int);
int T_set_lec_cell_tbl(TBL *, int, int, unsigned char *);
void T_set_string_cell_tbl(TBL *, int, int, unsigned char *);
int T_default(TBL *,char *,char **,char **,int ,int ,int );
void T_auto(TBL *,char *,char **,int ,int ,int );

/*----------------------- MACROS ----------------------------*/

#define T_NC(tbl)           (tbl->nb_columns)
#define T_NL(tbl)           (tbl->lines.size())
#define T_LANG(tbl)         (tbl->language)
#define T_L(tbl)            (tbl->lines)
#define T_C(tbl, i, j)      ((tbl->lines[i]).cells[j])

#define KTVAL(kdb, pos)     (K_tunpack(SW_getptr(kdb->k_objs[pos].o_val), KONAME(kdb, pos)) )
#define KTPTR(name)         K_tptr(KT_WS, name)      // returns an allocated TBL
