#pragma once

#include "cpp_api/common.h"
#include "api/utils/utils.h"
#include <stdexcept>


using bitset_8 = std::bitset<8>;


// WARNING: C++ allows functions returning a reference to be left-values. 
//          This is currently not supported in Cython.

// ================ CELL ================

struct TableCell: public TCELL
{
	TableCell(const TableCellType cell_type, const std::string& content, const TableCellAlign align = TableCellAlign::TABLE_CELL_LEFT, 
		const bool bold = false, const bool italic = false, const bool underline = false);

	TableCell(const TableCell& other);

	// WARNING: a table cell must be deleted (freed) from a Table instance
	~TableCell();

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

	std::vector<std::string> get_coefficients_from_lec();

	bool operator==(const TableCell& other) const;
};


// ================ LINE ================

struct TableLine: public TLINE
{
	TableLine(const TableLineType line_type, const TableGraphType graph_type = TableGraphType::TABLE_GRAPH_LINE, 
		const bool axis_left = true);

	TableLineType get_line_type() const;

	void set_line_type(const TableLineType line_type);

	TableGraphType get_line_graph() const;

	void set_line_graph(const TableGraphType graph_type);

	bool is_left_axis() const;

	void set_line_axis(const bool is_left);

	TableCell* get_cell(const int column, const int nb_cells) const;

	bool operator==(const TableLine& other) const;
};

void copy_line(const int nb_columns, TLINE* c_cell_dest, const TLINE* c_cell_src);


// ================ TABLE ================

struct Table: public TBL
{
public:
    Table(const int nb_columns);

    Table(const int nb_columns, const std::string& def, const std::vector<std::string>& vars, 
		bool mode = false, bool files = false, bool date = false);

    Table(const int nb_columns, const std::string& def, const std::vector<std::string>& titles, 
		const std::vector<std::string>& lecs, bool mode = false, bool files = false, bool date = false);

    Table(const int nb_columns, const std::string& def, const std::string& lecs, 
		bool mode = false, bool files = false, bool date = false);

	Table(const TBL* c_table);
	Table(const Table& table);

	std::string get_language() const;

	void set_language(const TableLang lang);

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

	TableLine* append_line(const TableLineType line_type);

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

	// -------- REMOVE --------

	void remove_line(const int row)
	{
		if(row < 0 || row >= lines.size())
			throw std::out_of_range("Table line index " + std::to_string(row) + " is out of range [0, " + 
				std::to_string(lines.size()) + ").");
		lines.erase(lines.begin() + row);
	}

	// -------- EQUAL --------

	bool operator==(const Table& other) const;
};
