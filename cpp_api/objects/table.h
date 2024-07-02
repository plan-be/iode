#pragma once

#include "common.h"
#include "utils/utils.h"
#include "utils/iode_exceptions.h"
#include <stdexcept>
#include <boost/functional/hash.hpp>


using bitset_8 = std::bitset<8>;

const static std::map<std::string, TableLineType> m_line_type =
{
	{"Cells", TABLE_LINE_CELL},
	{"Line",  TABLE_LINE_SEP},
	{"Title", TABLE_LINE_TITLE},
	{"Files", TABLE_LINE_FILES},
	{"Mode",  TABLE_LINE_MODE},
	{"Date",  TABLE_LINE_DATE}
};

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


// WARNING: C++ allows functions returning a reference to be left-values. 
//          This is currently not supported in Cython.

// ================ CELL ================

struct TableCell: public TCELL
{
	TableCell(const TableCellType cell_type, const std::string& content, const TableCellAlign align = TableCellAlign::TABLE_CELL_LEFT, 
		const bool bold = false, const bool italic = false, const bool underline = false);

	// WARNING: a table cell must be deleted (freed) from a Table instance
	~TableCell();

	// WARNING: must NOT be exposed to Python API -> called from a Table instance
	void free();

	CLEC* get_compiled_lec();

	std::string get_content(const bool quotes) const;

	void set_text(const std::string& text);

	void set_lec(const std::string& lec);

	void set_content(const std::string& content);

	TableCellType get_type() const;

	void set_type(const TableCellType cell_type);

	TableCellAlign get_align() const;

	void set_align(const TableCellAlign align);

	bool is_bold() const;

	void set_bold(const bool value);

	bool is_italic() const;

	void set_italic(const bool value);

	bool is_underline() const;

	void set_underline(const bool value);

	std::vector<std::string> get_variables_from_lec();

	bool operator==(const TableCell& other) const;
};


// ================ LINE ================

struct TableLine: public TLINE
{
	TableLine(const TableLineType line_type, const TableGraphType graph_type = TableGraphType::TABLE_GRAPH_LINE, 
		const bool axis_left = true);

	// WARNING: a table line must be deleted (freed) from a Table instance
	~TableLine();

	// WARNING: must NOT be exposed to Python API -> called from a Table instance
	void free(const int nb_cells);

	TableLineType get_line_type() const;

	void set_line_type(const TableLineType line_type);

	TableGraphType get_line_graph() const;

	void set_line_graph(const TableGraphType graph_type);

	bool is_left_axis() const;

	void set_line_axis(const bool is_left);

	// TODO : returned value should be an enum
	unsigned char get_line_pbyte() const;

	// TODO : passed value should be an enum
	void set_line_pbyte(const unsigned char pbyte);

	TableCell* get_cell(const int column, const int nb_cells) const;

	// WARNING: We cannot override the operator == since the number of cells is not 
	//          stored in the base structure TLINE (on purpose?) and to be able to 
	//          cast TableLine as TLINE, we cannot add any new member to TableLine
	bool equals(const TableLine& other, const int nb_cells) const;
};

void copy_line(const int nb_columns, TLINE* c_cell_dest, const TLINE* c_cell_src);

/**
 * @brief compute a hash value for a table line.
 * 
 * @note see https://www.boost.org/doc/libs/1_55_0/doc/html/hash/custom.html
 *       and https://www.boost.org/doc/libs/1_55_0/doc/html/hash/combine.html
 * 
 * @return std::size_t 
 */
std::size_t hash_value(TLINE const& c_line);


// ================ TABLE ================

bool table_equal(const TBL& table1, const TBL& table2);

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

    Table(const int nb_columns, const std::string& def, const std::vector<std::string>& titles, 
		const std::vector<std::string>& lecs, bool mode = false, bool files = false, bool date = false);

    Table(const int nb_columns, const std::string& def, const std::string& lecs, 
		bool mode = false, bool files = false, bool date = false);

	Table(const Table& table);

	~Table();

	// required to be used in std::map
    Table& operator=(const Table& table);

	void extend();

	std::string get_language() const;

	void set_language(const TableLang lang);

	short nb_columns() const;

	short nb_lines() const;

	TableGraphGrid get_gridx() const;

	void set_gridx(const TableGraphGrid gridx);

	TableGraphGrid get_gridy() const;

	void set_gridy(const TableGraphGrid gridy);

	TableGraphAxis get_graph_axis() const;

	void set_graph_axis(const TableGraphAxis axis);

	TableGraphAlign get_graph_alignment() const;

	void set_graph_alignment(const TableGraphAlign align);

	// -------- LINES --------

	TableLine* get_line(const int row);

	TableLine* insert_line(const int pos, const TableLineType line_type, const bool after = true);

	// -------- DIVIDER --------

	TableLine* get_divider_line();

	// -------- TITLE --------

	TableLine* insert_title(const int pos, const std::string& title, const bool after = true);

	TableLine* add_title(const std::string& title);

	std::string get_title(const int row);

	void set_title(const int row, const std::string title);

	// -------- CELLS --------

	TableLine* insert_line_with_cells(const int pos, const bool after = true);

	TableLine* add_line_with_cells();

	// -------- SEPARATOR --------

	TableLine* insert_line_separator(const int pos, const bool after = true);

	TableLine* add_line_separator();

	// -------- MODE --------

	TableLine* insert_line_mode(const int pos, const bool after = true);

	TableLine* add_line_mode();

	// -------- FILES --------

	TableLine* insert_line_files(const int pos, const bool after = true);

	TableLine* add_line_files();

	// -------- DATE --------

	TableLine* insert_line_date(const int pos, const bool after = true);

	TableLine* add_line_date();

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