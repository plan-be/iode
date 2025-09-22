#pragma once

#include "api/constants.h"
#include "api/k_super.h"
#include "api/objs/kdb.h"           // KDB
#include "api/objs/identities.h"    // Identity

#include <string>
#include <vector>

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

inline int T_GRAPHDEFAULT = 0;

/*----------------------- STRUCTS ----------------------------*/

struct TCELL 
{
    std::string content;    // if type == TABLE_CELL_STRING
    Identity*   idt;        // if type == TABLE_CELL_LEC
    char        type;       // TABLE_CELL_STRING or TABLE_CELL_LEC
    char        attribute;  // TABLE_CELL_LEFT, TABLE_CELL_CENTER, TABLE_CELL_RIGHT, TABLE_CELL_BOLD,
                            // TABLE_CELL_ITALIC, TABLE_CELL_UNDERLINE, TABLE_CELL_NORMAL

public:
    TCELL(char type, int col=0) : type(type)
    {
        if(type != TABLE_CELL_STRING && type != TABLE_CELL_LEC)
            this->type = TABLE_CELL_STRING;
        
        if(this->type == TABLE_CELL_STRING)
        {
            content = "";
            idt = nullptr;
            attribute = TABLE_CELL_NORMAL;
        }
        else
        {
            attribute = (col > 0) ? TABLE_CELL_DECIMAL : TABLE_CELL_LEFT;
            content = "";
            idt = nullptr;
        }
    }

    TCELL(const TCELL& other)
    {
        type = other.type;
        attribute = other.attribute;
        content = other.content;
        if(other.idt)
            idt = new Identity(*(other.idt));
        else
            idt = nullptr;
    }

     ~TCELL()
    {
        content.clear();
        if(idt)
            delete idt;
        idt = nullptr;
    }

    TCELL& operator=(const TCELL& other)
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

    bool is_null() const
    {
        if (type == TABLE_CELL_LEC && idt == NULL)
            return true;
        if (type == TABLE_CELL_STRING && content.empty())
            return true;
        return false;
    }
};

struct TLINE 
{
    std::vector<TCELL> cells;   // empty if type is FILES, MODE, DATE or SEP
    char    type;               // TABLE_LINE_FILES, TABLE_LINE_MODE, TABLE_LINE_DATE, TABLE_LINE_SEP, 
                                // TABLE_LINE_TITLE or TABLE_LINE_CELL
    char    graph_type;         // 0=Line, 1=scatter, 2=bar (non implemented in all IODE flavours)
    bool    right_axis;         // false if values are relative to the left axis, true to the right axis

public:
    TLINE(char type): type(type), graph_type(0), right_axis(false)
    {
        if(type != TABLE_LINE_TITLE && type != TABLE_LINE_CELL && type != TABLE_LINE_FILES && 
            type != TABLE_LINE_MODE && type != TABLE_LINE_DATE && type != TABLE_LINE_SEP)
        {
            std::string msg = "Creating a table line: invalid line type " + std::to_string(type) + ". ";
            msg += "Defaulting to line of type CELL.";
            kwarning((char*) msg.c_str());
            this->type = TABLE_LINE_CELL;
        }
    }

    TLINE(const TLINE& other)
    {
        type = other.type;
        graph_type = other.graph_type;
        right_axis = other.right_axis;
        cells.clear();
        for(const TCELL& cell : other.cells)
            cells.push_back(cell);
    }

     ~TLINE()
    {
        cells.clear();
    }

    TLINE& operator=(const TLINE& other)
    {
        type = other.type;
        graph_type = other.graph_type;
        right_axis = other.right_axis;
        cells.clear();
        for(const TCELL& cell : other.cells)
            cells.push_back(cell);
        return *this;
    }
};

// Custom specialization of std::hash can be injected in namespace std.
template<>
struct std::hash<TLINE>
{
    std::size_t operator()(const TLINE& line) const noexcept
    {
		std::size_t seed = 0;

		hash_combine<char>(seed, line.type);
		hash_combine<bool>(seed, line.right_axis);
		hash_combine<char>(seed, line.graph_type);

        const TCELL* title_cell;
		const Identity* idt;
		switch(line.type)
		{
		case TABLE_LINE_TITLE:
			title_cell = &(line.cells[0]);
			hash_combine<char>(seed, title_cell->type);
			hash_combine<char>(seed, title_cell->attribute);
			hash_combine<std::string>(seed, title_cell->content);
			break;
		case TABLE_LINE_CELL:
			for(const TCELL& cell : line.cells)
			{
				hash_combine<char>(seed, cell.type);
				hash_combine<char>(seed, cell.attribute);
				hash_combine<std::string>(seed, cell.content);
				idt = cell.idt;
				if(idt)
					hash_combine<std::string>(seed, idt->lec);
			}
			break;
		default:
			break;
		}

		return seed;
    }
};

struct TBL 
{
    short   language;           // Output language : TABLE_ENGLISH, TABLE_FRENCH, TABLE_DUTCH
    short   repeat_columns;     // if 0, first column is frozen, otherwise, col 1 is repeated as other columns
    short   nb_columns;         // Number of columns (of text and lec, not calculated values)
    TLINE   divider_line;       // nb_columns TCELL's, each TCELL contains a divider
    std::vector<TLINE> lines;   
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

        for(int i = 0; i < dim; i++) 
            this->divider_line.cells.push_back(TCELL(TABLE_CELL_LEC, i));
        this->divider_line.cells[0].idt = new Identity("1");
        /* rest is repetitive if val[i] == 0, val[i] = val[i-1] */
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

		hash_combine<TLINE>(seed, table.divider_line);
		for(const TLINE& line : table.lines)
			hash_combine<TLINE>(seed, line);

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
char *T_cell_cont(TCELL *,int );
char *T_cell_cont_tbl(TBL *,int, int, int );
char *T_div_cont_tbl(TBL *, int, int );
int T_append_line(TBL*, int);
int T_insert_line(TBL*, int, int, int);
int T_set_lec_cell(TCELL *,unsigned char *);
int T_set_lec_cell_tbl(TBL *, int, int, unsigned char *);
void T_set_string_cell(TCELL *,unsigned char *);
void T_set_string_cell_tbl(TBL *, int, int, unsigned char *);
void T_set_cell_attr(TBL *,int ,int ,int );
int T_default(TBL *,char *,char **,char **,int ,int ,int );
void T_auto(TBL *,char *,char **,int ,int ,int );

/*----------------------- MACROS ----------------------------*/

#define T_NC(tbl)           (tbl->nb_columns)
#define T_NL(tbl)           (tbl->lines.size())
#define T_LANG(tbl)         (tbl->language)
#define T_L(tbl)            (tbl->lines)
#define T_C(tbl, i, j)      ((tbl->lines[i]).cells[j])

#define TABLE_CELL_ALIGN(attr, align)   (((attr) & 7) | (align))
#define TABLE_CELL_FONT(attr, font)     ((attr) | (font))
#define TABLE_CELL_SETFONT(attr, font)  (((attr) & 120) | (font))

#define KTVAL(kdb, pos)     (K_tunpack(SW_getptr(kdb->k_objs[pos].o_val), KONAME(kdb, pos)) )
#define KTPTR(name)         K_tptr(KT_WS, name)      // returns an allocated TBL
