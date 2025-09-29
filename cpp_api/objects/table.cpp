#pragma once

#include "table.h"


// ================ TABLE ================

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
