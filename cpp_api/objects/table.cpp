#pragma once

#include "table.h"


// ================ TABLE ================


bool table_equal(const TBL& table1, const TBL& table2)
{
	if(table1.language != table2.language) return false;
	if(table1.repeat_columns != table2.repeat_columns) return false;
	if(table1.nb_columns != table2.nb_columns) return false;
	
	int nb_columns = table1.nb_columns;
	TableLine* div1 = (TableLine*) &table1.divider_line;
	TableLine* div2 = (TableLine*) &table2.divider_line;
	if(*div1 != *div2) return false;
	
	if(table1.lines.size() != table2.lines.size()) return false;
	TableLine* line1;
	TableLine* line2;
	for (int i = 0; i < table1.lines.size(); i++)
	{
		line1 = (TableLine*) &table1.lines[i];
		line2 = (TableLine*) &table2.lines[i];
		if (*line1 != *line2) 
			return false;
	}

	if(table1.z_min != table2.z_min) return false;
	if(table1.z_max != table2.z_max) return false;
	if(table1.y_min != table2.y_min) return false;
	if(table1.y_max != table2.y_max) return false;
	if(table1.attribute != table2.attribute) return false;
	if(table1.chart_box != table2.chart_box) return false;
	if(table1.chart_shadow != table2.chart_shadow) return false;
	if(table1.chart_gridx != table2.chart_gridx) return false;
	if(table1.chart_gridy != table2.chart_gridy) return false;
	if(table1.chart_axis_type != table2.chart_axis_type) return false;
	if(table1.text_alignment != table2.text_alignment) return false;

	return true;
}

Table::Table(const int nb_columns) : TBL(nb_columns) {}

Table::Table(const int nb_columns, const std::string& def, const std::vector<std::string>& vars, 
	bool mode, bool files, bool date): TBL(nb_columns)
{
	char* c_def = to_char_array(def);

	char** c_lecs = NULL;
	if(vars.size() > 0)
	{
		std::string vars_list;
		for(const std::string& var : vars) 
			vars_list += var + ';';
		c_lecs = B_ainit_chk(to_char_array(vars_list), NULL, 0);
	}

	int c_mode = mode ? 1 : 0;
	int c_files = files ? 1 : 0;
	int c_date = date ? 1 : 0;

	T_auto(this, c_def, c_lecs, c_mode, c_files, c_date);

	SCR_free_tbl((unsigned char**) c_lecs);
}

Table::Table(const int nb_columns, const std::string& def, const std::vector<std::string>& titles, 
		     const std::vector<std::string>& lecs, bool mode, bool files, bool date): TBL(nb_columns)
{
	char* c_def = to_char_array(def);

	if(titles.size() != lecs.size())
		throw std::invalid_argument(std::string("Table: the list of title lines and the list of LEC ") + 
			"expressions must have the same size.\n" + 
			"Size of the list of title lines: " + std::to_string(titles.size()) + "\n" +
			"Size of the list of LEC expressions: " + std::to_string(lecs.size()));

	char** c_titles = vector_to_double_char(titles);
	char** c_lecs = vector_to_double_char(lecs);

	int c_mode = mode ? 1 : 0;
	int c_files = files ? 1 : 0;
	int c_date = date ? 1 : 0;

	T_default(this, c_def, c_titles, c_lecs, c_mode, c_files, c_date);

	SCR_free_tbl((unsigned char**) c_titles);
	SCR_free_tbl((unsigned char**) c_lecs);
}

Table::Table(const int nb_columns, const std::string& def, const std::string& lecs, 
	bool mode, bool files, bool date): TBL(nb_columns)
{
	char* c_def = to_char_array(def);

	char** c_lecs = NULL;
	if(!lecs.empty())
	{
		char* pattern = to_char_array(lecs);
		// Retrieves all variable names matching one or more patterns in KV_WS (similar to grep)
		char* lst = K_expand(VARIABLES, NULL, pattern, '*');
		// Parses a string and replaces @filename and $listname by their contents
		c_lecs = B_ainit_chk(lst, NULL, 0);
		SCR_free(lst);
	}

	int c_mode = mode ? 1 : 0;
	int c_files = files ? 1 : 0;
	int c_date = date ? 1 : 0;

	T_auto(this, c_def, c_lecs, c_mode, c_files, c_date);

	SCR_free_tbl((unsigned char**) c_lecs);
}

Table::Table(const TBL* c_table): TBL(*c_table) {}

Table::Table(const Table& table): TBL((TBL&) table) {}

// ================ TABLE ================

std::string Table::get_language() const
{
	return v_table_langs.at(language - TABLE_ENGLISH);
}

void Table::set_language(const TableLang lang)
{
	language = (short) lang;
}

TableGraphGrid Table::get_gridx() const
{
	return static_cast<TableGraphGrid>(chart_gridx);
}

void Table::set_gridx(const TableGraphGrid gridx)
{
	chart_gridx = gridx - TABLE_GRAPH_MAJOR;
}

TableGraphGrid Table::get_gridy() const
{
	return static_cast<TableGraphGrid>(chart_gridy);
}

void Table::set_gridy(const TableGraphGrid gridy)
{
	chart_gridy = gridy;
}

TableGraphAxis Table::get_graph_axis() const
{
	return static_cast<TableGraphAxis>(chart_axis_type);
}

void Table::set_graph_axis(const TableGraphAxis axis)
{
	chart_axis_type = axis;
}

TableGraphAlign Table::get_graph_alignment() const
{
	return static_cast<TableGraphAlign>(text_alignment);
}

void Table::set_graph_alignment(const TableGraphAlign align)
{
	text_alignment = align;
}

// -------- LINES --------

TableLine* Table::append_line(const TableLineType line_type)
{
	int new_pos = T_append_line(this, line_type);
	if (new_pos < 0) 
		throw std::runtime_error("Cannot append a new line to the table");

	return static_cast<TableLine*>(&this->lines.back());
}

TableLine* Table::insert_line(const int pos, const TableLineType line_type, const bool after)
{
	if (pos < 0 || pos >= this->lines.size())
		throw std::invalid_argument("Cannot insert table line at index " + std::to_string(pos) + ".\n" +  
			"New line index must be in range [0, " + std::to_string(this->lines.size() - 1) + "]");

	int where_ = after ? 1 : 0;
	// WARNING: When inserting a new line of type TABLE_CELL, the attribute TableCell::type of cells is undefined!
	int new_pos = T_insert_line(this, pos, line_type, where_);
	if (new_pos < 0) 
		throw std::runtime_error("Cannot insert table line at position " + std::to_string(pos));

	return static_cast<TableLine*>(&this->lines[new_pos]);
}

// -------- TITLE --------

// we assume that title string is written in UTF8 format
TableLine* Table::insert_title(const int pos, const std::string& title, const bool after)
{
	TableLine* title_line = insert_line(pos, TABLE_LINE_TITLE, after);
	title_line->cells[0].set_text(title);
	return title_line;
}

TableLine* Table::add_title(const std::string& title)
{
	TableLine* title_line = append_line(TABLE_LINE_TITLE);
	title_line->cells[0].set_text(title);
	return title_line;
}

std::string Table::get_title(const int row)
{
	TableLine& line = lines[row];
	if(line.get_type() != TableLineType::TABLE_LINE_TITLE) 
		throw std::invalid_argument("Cannot get title at line index " + std::to_string(row) + ".\n" +
			"Line at index " + std::to_string(row) + " is not a TITLE line but of type " + 
			get_line_type_as_string(line.get_type()) + ".");
	return line.cells[0].get_content(false);
}

// we assume that title string is written in UTF8 format
void Table::set_title(const int row, const std::string title)
{
	TableLine& line = lines[row];
	if(line.get_type() != TableLineType::TABLE_LINE_TITLE) 
		throw std::invalid_argument("Cannot set table title at index " + std::to_string(row) + ".\n" + 
			"Line at index " + std::to_string(row) + " is not a TITLE line but of type " + 
			get_line_type_as_string(line.get_type()) + ".");
	line.cells[0].set_text(title);
}

// -------- CELLS --------

TableLine* Table::insert_line_with_cells(const int pos, const bool after)
{
	return insert_line(pos, TABLE_LINE_CELL, after);
}

TableLine* Table::add_line_with_cells()
{
	return append_line(TABLE_LINE_CELL);
}

// -------- SEPARATOR --------

TableLine* Table::insert_line_separator(const int pos, const bool after)
{
	return insert_line(pos, TABLE_LINE_SEP, after);
}

TableLine* Table::add_line_separator()
{
	return append_line(TABLE_LINE_SEP);
}

// -------- MODE --------

TableLine* Table::insert_line_mode(const int pos, const bool after)
{
	return insert_line(pos, TABLE_LINE_MODE, after);
}

TableLine* Table::add_line_mode()
{
	return append_line(TABLE_LINE_MODE);
}

// -------- FILES --------

TableLine* Table::insert_line_files(const int pos, const bool after)
{
	return insert_line(pos, TABLE_LINE_FILES, after);
}

TableLine* Table::add_line_files()
{
	return append_line(TABLE_LINE_FILES);
}

// -------- DATE --------

TableLine* Table::insert_line_date(const int pos, const bool after)
{
	return insert_line(pos, TABLE_LINE_DATE, after);
}

TableLine* Table::add_line_date()
{
	return append_line(TABLE_LINE_DATE);
}

// -------- EQUAL --------

bool Table::operator==(const Table& other) const
{
	return table_equal(static_cast<TBL>(*this), static_cast<TBL>(other));
}
