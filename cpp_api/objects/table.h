#pragma once

#include "common.h"
#include "utils/utils.h"
#include "utils/iode_exceptions.h"
#include <stdexcept>
#include <boost/functional/hash.hpp>


enum EnumCellType
{
	IT_LEC = KT_LEC,				// 114
	IT_STRING = KT_STRING,			// 115
};

enum EnumCellAttribute
{
	IT_NORMAL = KT_NORMAL,			// 0
	IT_BOLD = KT_BOLD,				// 1
	IT_ITALIC = KT_ITALIC,			// 2
	IT_UNDERLINE = KT_UNDERLINE,	// 4
	// -----------------------------------
	IT_CENTER = KT_CENTER,			// 8
	IT_LEFT = KT_LEFT,				// 16
	IT_RIGHT = KT_RIGHT,			// 32
	IT_DECIMAL = KT_DECIMAL			// 64
};

enum EnumLineType
{
	IT_FILES = KT_FILES,			// 105
	IT_MODE = KT_MODE,				// 106
	IT_CELL = KT_CELL,				// 110
	IT_LINE = KT_LINE,				// 111
	IT_TITLE = KT_TITLE,			// 113
	IT_DATE = KT_DATE				// 116
};

const static std::map<std::string, EnumLineType> mLineType =
{
	{"Cells", IT_CELL},
	{"Line",  IT_LINE},
	{"Title", IT_TITLE},
	{"Files", IT_FILES},
	{"Mode",  IT_MODE},
	{"Date",  IT_DATE}
};

inline std::string get_line_type_as_string(EnumLineType line_type)
{
	for(const auto& [key, value]: mLineType)
		if(line_type == value) return key;
	return "";
}

enum EnumGraphType
{
	IG_LINE,
	IG_SCATTER,
	IG_BAR
};

enum EnumGraphGrid
{
	IG_MAJOR,
	IG_NONE,
	IG_MINOR
};

enum EnumGraphAlign
{
	IG_LEFT,
	IG_CENTER,
	IG_RIGHT
};

enum EnumGraphAxis
{
	IG_VALUES,
	IG_LOG,
	IG_SEMILOG,
	IG_PERCENT
};


void copy_cell(TCELL* c_cell_dest, const TCELL* c_cell_src);
void copy_line(const int nb_columns, TLINE* c_cell_dest, const TLINE* c_cell_src);

bool cell_equal(const TCELL* c_cell1, const TCELL* c_cell2);
bool line_equal(const int nb_columns, const TLINE* c_line1, const TLINE* c_line2);
bool table_equal(const TBL& table1, const TBL& table2);

/**
 * @brief compute a hash value for a table line.
 * 
 * @note see https://www.boost.org/doc/libs/1_55_0/doc/html/hash/custom.html
 *       and https://www.boost.org/doc/libs/1_55_0/doc/html/hash/combine.html
 * 
 * @return std::size_t 
 */
std::size_t hash_value(TLINE const& c_line);


struct Table: public TBL
{
private:
	/**
	 * @brief Call the C function T_create() and set a default value to the remaining attributes.
	 * 
	 * @param nb_columns 
	 */
	void initialize(const int nb_columns);

    void copy_from_TBL_obj(const TBL* obj);

public:
	Table(const int pos, KDB* kdb);

	Table(const std::string& name, KDB* kdb);

    Table(const int nb_columns);

    Table(const int nb_columns, const std::string& def, const std::vector<std::string>& vars, 
		bool mode = false, bool files = false, bool date = false);

    Table(const int nb_columns, const std::string& def, const std::string& lecs, 
		bool mode = false, bool files = false, bool date = false);

	Table(const Table& table);

	~Table();

	// required to be used in std::map
    Table& operator=(const Table& table);

	// ================ TABLE ================

	void extend();

	std::string get_language() const;

	void set_language(const EnumLang lang);

	short nb_columns() const;

	short nb_lines() const;

	EnumGraphGrid get_gridx() const;

	void set_gridx(const EnumGraphGrid gridx);

	EnumGraphGrid get_gridy() const;

	void set_gridy(const EnumGraphGrid gridy);

	EnumGraphAxis get_graph_axis() const;

	void set_graph_axis(const EnumGraphAxis axis);

	EnumGraphAlign get_graph_alignment() const;

	void set_graph_alignment(const EnumGraphAlign align);

	// ================ LINES ================

	EnumLineType get_line_type(const int row) const;

	void set_line_type(const int row, const EnumLineType line_type);

	EnumGraphType get_line_graph(const int row) const;

	void set_line_graph(const int row, const EnumGraphType graph_type);

	bool is_left_axis(const int row) const;

	void set_line_axis(const int row, const bool is_left);

	// TODO : returned value should be an enum
	unsigned char get_line_pbyte(const int row) const;

	// TODO : passed value should be an enum
	void set_line_pbyte(const int row, const unsigned char pbyte);

	// -------- DIVIDER --------

	EnumCellType get_divider_cell_type(const int column) const;

	EnumCellAttribute get_divider_cell_align(const int column) const;

	int get_divider_cell_font(const int column) const;

	std::string get_divider_cell_content(const int column, const bool quotes) const;

	// TODO: does it make sense to set a Divider cell as STRING
	void set_cell_divider_text(const int column, const std::string& text);

	void set_cell_divider_lec(const int column, const std::string& lec);

	// -------- TITLE --------

	int insert_title(const int pos, const std::string& title, const bool after = true);

	int add_title(const std::string& title);

	std::string get_title(const int row) const;

	void set_title(const int row, const std::string title);

	// -------- CELLS --------

	int insert_line_with_cells(const int pos, const bool after = true);

	int add_line_with_cells();
	
	std::string get_cell_content(const int row, const int column, const bool quotes = false) const
	{
		return get_cell_content(row, column, quotes, false);
	}

	std::vector<std::string> get_variables_from_lec_cell(const int row, const int column);
	
	void set_cell_text(const int row, const int column, const std::string& text)
	{
		set_cell_text(row, column, text, false);
	}
	
	void set_cell_lec(const int row, const int column, const std::string& lec)
	{
		set_cell_lec(row, column, lec, false);
	}
	
	void set_cell_content(const int row, const int column, const std::string& content)
	{
		set_cell_content(row, column, content, false);
	}
	
	EnumCellType get_cell_type(const int row, const int column) const
	{
		return get_cell_type(row, column, false);
	}

	void set_cell_type(const int row, const int column, const EnumCellType cell_type)
	{
		set_cell_type(row, column, cell_type, false);
	}
	
	EnumCellAttribute get_cell_align(const int row, const int column) const
	{
		return get_cell_align(row, column, false);
	}
	
	int get_cell_font(const int row, const int column) const
	{
		return get_cell_font(row, column, false);
	}
	
	bool is_cell_bold_font(const int row, const int column) const
	{
		return is_cell_bold_font(row, column, false);
	}
	
	bool is_cell_italic_font(const int row, const int column) const
	{
		return is_cell_italic_font(row, column, false);
	}
	
	bool is_cell_underline_font(const int row, const int column) const
	{
		return is_cell_underline_font(row, column, false);
	}

	void set_cell_attribute(const int row, const int column, const char attr)
	{
		set_cell_attribute(row, column, attr, false);
	}

	// -------- SEPARATOR --------

	int insert_line_separator(const int pos, const bool after = true);

	int add_line_separator();

	// -------- MODE --------

	int insert_line_mode(const int pos, const bool after = true);

	int add_line_mode();

	// -------- FILES --------

	int insert_line_files(const int pos, const bool after = true);

	int add_line_files();

	// -------- DATE --------

	int insert_line_date(const int pos, const bool after = true);

	int add_line_date();

	// -------- FREE --------

	/**
	 * @brief 
	 * 
	 * @param row 
	 * 
	 * @note see T_del_line() function from the source file o_tab.c from the old GUI.
	 */
	void delete_line(const int row);

	void free_line(const int row);

	void free_cell(const int row, const int column);

	// -------- EQUAL --------

	bool operator==(const Table& other) const;

private:
	// ================ TABLE (PRIVATE) ================

	/**
	 * @brief Same as C function T_free() but without its last line 
	 *        calling SW_nfree(tbl)
	 */
	void free_all_lines();

	// ================ LINES (PRIVATE) ================

	TLINE* get_line(const int row) const;

	int insert_line(const int pos, const EnumLineType line_type, const bool after = true);

	// ================ CELLS (PRIVATE) ================

	TCELL* get_cell(const int row, const int column, const bool divider) const;

	CLEC* get_cell_compiled_lec(const int row, const int column);

	std::string get_cell_content(const int row, const int column, const bool quotes, const bool divider) const;

	void set_cell_text(const int row, const int column, const std::string& text, const bool divider);

	void set_cell_lec(const int row, const int column, const std::string& lec, const bool divider);

	void set_cell_content(const int row, const int column, const std::string& content, const bool divider);

	EnumCellType get_cell_type(const int row, const int column, const bool divider) const;

	void set_cell_type(const int row, const int column, const EnumCellType cell_type, const bool divider);

	EnumCellAttribute get_cell_align(const int row, const int column, const bool divider) const;

	int get_cell_font(const int row, const int column, const bool divider) const;

	bool is_cell_bold_font(const int row, const int column, const bool divider) const;

	bool is_cell_italic_font(const int row, const int column, const bool divider) const;

	bool is_cell_underline_font(const int row, const int column, const bool divider) const;

	void set_cell_attribute(const int row, const int column, const char attr, const bool divider);
};


/**
 * @brief compute a hash value for an object of type TBL (C API).
 * 
 * @note see https://www.boost.org/doc/libs/1_55_0/doc/html/hash/custom.html
 *       and https://www.boost.org/doc/libs/1_55_0/doc/html/hash/combine.html
 * 
 * @return std::size_t 
 */
std::size_t hash_value(TBL const& c_table);

/**
 * @brief compute a hash value for a table.
 * 
 * @note see https://www.boost.org/doc/libs/1_55_0/doc/html/hash/custom.html
 *       and https://www.boost.org/doc/libs/1_55_0/doc/html/hash/combine.html
 * 
 * @return std::size_t 
 */
std::size_t hash_value(Table const& table);