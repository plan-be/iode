#pragma once

#include "table.h"


// ================ CELL ================

static void copy_cell(TCELL* c_cell_dest, const TCELL* c_cell_src)
{
	if(c_cell_src == NULL)
		throw std::runtime_error("Cannot copy the table cell. The source cell is NULL");
	if(c_cell_dest == NULL)
		throw std::runtime_error("Cannot copy the table cell. The destination cell is NULL");

	unsigned char* cell_src_content = (unsigned char*) T_cell_cont(const_cast<TCELL*>(c_cell_src), 0);
	if (c_cell_src->type == TABLE_CELL_LEC) 
		T_set_lec_cell(c_cell_dest, cell_src_content);
	else 
		T_set_string_cell(c_cell_dest, cell_src_content);
	if(c_cell_src->attribute != 0)
		c_cell_dest->attribute = c_cell_src->attribute;
}

TableCell::TableCell(const TableCellType cell_type, const std::string& content, const TableCellAlign align, 
	const bool bold, const bool italic, const bool underline): TCELL((char) cell_type)
{
	if(cell_type == TableCellType::TABLE_CELL_STRING)
		set_text(content);
	else
		set_lec(content);
	this->attribute = (char) align;
	set_bold(bold);
	set_italic(italic);
	set_underline(underline);
}

TableCell::TableCell(const TableCell& other): TCELL(other.type)
{
	copy_cell(this, &other);
}

TableCell::~TableCell() {}

// The table cell contains a "packed" IDT object (lec + clec) 
// -> see T_set_lec_cell from k_tbl.c
CLEC* TableCell::get_compiled_lec()
{
	if(type != TABLE_CELL_LEC)
		throw std::runtime_error("Cannot get the compiled LEC. The table cell does not contain a LEC expression");

	if(!idt)
		throw std::runtime_error("Cannot get the compiled LEC. The table cell is empty");

	// see VT_edit() from o_vt.c from the old GUI
	return idt->get_compiled_lec();
}

std::vector<std::string> TableCell::get_variables_from_lec()
{
	CLEC* clec = get_compiled_lec();
	return get_variables_from_clec(clec);
}

std::vector<std::string> TableCell::get_coefficients_from_lec()
{
	CLEC* clec = get_compiled_lec();
	return get_scalars_from_clec(clec);
}

std::string TableCell::get_content(const bool quotes) const
{
	int mode = quotes ? 1 : 0;
	std::string content_oem = std::string(T_cell_cont((TCELL*) this, mode));
	std::string content = (type == TABLE_CELL_STRING) ? oem_to_utf8(content_oem) : content_oem;
	return content;
}

/**
 * @brief 
 *
 * @param text text to be written in the cell. Note that leading and trailing double quotes are removed.
 * 
 * @note: The string argument `text` passed to this C++ method is assumed to be written with the UTF8 format
 */
void TableCell::set_text(const std::string& text)
{
	std::string text_oem = utf8_to_oem(text);
	unsigned char* c_text = reinterpret_cast<unsigned char*>(to_char_array(text_oem));
	T_set_string_cell((TCELL*) this, c_text);
}

void TableCell::set_lec(const std::string& lec)
{
	unsigned char* c_lec = reinterpret_cast<unsigned char*>(to_char_array(lec));
	T_set_lec_cell((TCELL*) this, c_lec);
}

/**
 * @brief Set the content of a cell.
 *        Rule: If the content starts with a double quotes, we assume it is a string cell. 
 *        Otherwise, it is a LEC cell.
 *        Note that the leading and trailing double quotes are removed when the text is written 
 *        in a string table cell.
 * 
 * @param content 
 */
void TableCell::set_content(const std::string& content)
{
	if(content.starts_with('\"'))
		set_text(content);
	else
		set_lec(content);
}

TableCellType TableCell::get_type() const
{
	return static_cast<TableCellType>(type);
}

void TableCell::set_type(const TableCellType cell_type)
{
	this->type = cell_type;
}

// TODO: check if it is correct
TableCellAlign TableCell::get_align() const
{
	return static_cast<TableCellAlign>((int) (this->attribute / 8) * 8);
}

void TableCell::set_align(const TableCellAlign align)
{
	char font = ((int) this->attribute) % 8;
	this->attribute = ((char) align) + font;
}

bool TableCell::is_bold() const
{
	return bitset_8(this->attribute).test(0);
}

void TableCell::set_bold(const bool value)
{
	bitset_8 attr(this->attribute);
	attr.set(0, value);
	this->attribute = (char) attr.to_ulong();
}

bool TableCell::is_italic() const
{
	return bitset_8(this->attribute).test(1);
}

void TableCell::set_italic(const bool value)
{
	bitset_8 attr(this->attribute);
	attr.set(1, value);
	this->attribute = (char) attr.to_ulong();
}

bool TableCell::is_underline() const
{
	return bitset_8(this->attribute).test(2);
}

void TableCell::set_underline(const bool value)
{
	bitset_8 attr(this->attribute);
	attr.set(2, value);
	this->attribute = (char) attr.to_ulong();
}

bool TableCell::operator==(const TableCell& other) const
{
	if (type != other.type) 
		return false;

	if (attribute != other.attribute) 
		return false;
	
	if(type == TABLE_CELL_STRING)
		return this->content == other.content;
	else if(type == TABLE_CELL_LEC)
	{
		if(idt == nullptr && other.idt == nullptr) 
			return true;
		if(idt == nullptr || other.idt == nullptr) 
			return false;
		return idt->lec == other.idt->lec;
	}
	else
		return true;
}

// ================ LINE ================

void copy_line(const int nb_columns, TLINE* c_line_dest, const TLINE* c_line_src)
{
	c_line_dest->type = c_line_src->type;
	c_line_dest->right_axis = c_line_src->right_axis;
	c_line_dest->graph_type = c_line_src->graph_type;

	unsigned char* cell_src_content;
	TCELL* cells_dest = c_line_dest->cells.data();
	TCELL* cells_src = (TCELL*) c_line_src->cells.data();
	switch (c_line_src->type)
	{
	case TABLE_LINE_TITLE:
		cell_src_content = (unsigned char*) T_cell_cont(cells_src, 0);
		T_set_string_cell(cells_dest, cell_src_content);
		if(cells_src->attribute != 0)
			cells_dest->attribute = cells_src->attribute;
		break;
	case TABLE_LINE_CELL:
		for (int col = 0; col < nb_columns; col++) 
			copy_cell(&cells_dest[col], &cells_src[col]);
		break;
	default:
		break;
	}
}

TableLine::TableLine(const TableLineType line_type, const TableGraphType graph_type, const bool axis_left): 
	TLINE((char) line_type)
{
	this->type = (char) line_type;
	set_line_graph(graph_type);
	set_line_axis(axis_left);
}

TableLineType TableLine::get_line_type() const
{
	return static_cast<TableLineType>(type);
}

void TableLine::set_line_type(const TableLineType line_type)
{
	type = line_type;
}

TableGraphType TableLine::get_line_graph() const
{
	return static_cast<TableGraphType>(graph_type);
}

void TableLine::set_line_graph(const TableGraphType graph_type)
{
	this->graph_type = graph_type;
}

bool TableLine::is_left_axis() const
{
	return !right_axis;
}

void TableLine::set_line_axis(const bool is_left)
{
	right_axis = !is_left;
}

TableCell* TableLine::get_cell(const int column, const int nb_cells) const
{
	if(column < 0)
		throw std::invalid_argument("Table cell position cannot be negative");

	TableLineType line_type = (TableLineType) this->type;
	if(line_type != TABLE_LINE_TITLE && line_type != TABLE_LINE_CELL)
		throw std::runtime_error("Table line of type " + get_line_type_as_string(line_type) + " has no cells"); 

	if(line_type == TABLE_LINE_TITLE && column > 0)
		throw std::invalid_argument("Table cell position for a TITLE line must be 0");

	if(line_type == TABLE_LINE_CELL && column >= nb_cells)
		throw std::invalid_argument("Table cell position cannot exceed " + std::to_string(nb_cells));

	const TCELL* cell = &(this->cells[column]);
	return (TableCell*) cell;
}

bool TableLine::operator==(const TableLine& other) const
{
	if (type != other.type) return false;
	if (right_axis != other.right_axis) return false;
	if (graph_type != other.graph_type) return false;

	const TableCell* cell_this;
	const TableCell* cell_other;
	if(type == TABLE_LINE_TITLE)
	{
		cell_this = (TableCell*) &this->cells[0];
		cell_other = (TableCell*) &other.cells[0];
		return cell_this->content == cell_other->content;
	}
	else if(type == TABLE_LINE_CELL)
	{
		if(this->cells.size() != other.cells.size()) 
			return false;
		for(int col = 0; col < other.cells.size(); col++)
		{
			cell_this = (TableCell*) &this->cells[col];
			cell_other = (TableCell*) &other.cells[col];
			if(*cell_this != *cell_other) 
				return false;
		} 
		return true;
	}
	else
		// cells == NULL for FILES, MODE, LINE and DATE type
		return true;
}


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

TableLine* Table::get_line(const int row)
{
	if (row < 0 || row >= this->lines.size())
		throw std::invalid_argument("Cannot get table line at index " + std::to_string(row) + ".\n" + 
			"Line index must be in range [0, " + std::to_string(this->lines.size()) + "]");

	return static_cast<TableLine*>(&this->lines[row]);
}

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
	// WARNING: When inserting a new line of type TABLE_CELL, the attribute TCELL::type of cells is undefined!
	int new_pos = T_insert_line(this, pos, line_type, where_);
	if (new_pos < 0) 
		throw std::runtime_error("Cannot insert table line at position " + std::to_string(pos));

	return static_cast<TableLine*>(&this->lines[new_pos]);
}

// -------- DIVIDER --------

TableLine* Table::get_divider_line()
{
	return static_cast<TableLine*>(&this->divider_line);
}

// -------- TITLE --------

// we assume that title string is written in UTF8 format
TableLine* Table::insert_title(const int pos, const std::string& title, const bool after)
{
	TableLine* title_line = insert_line(pos, TABLE_LINE_TITLE, after);
	title_line->get_cell(0, nb_columns)->set_text(title);
	return title_line;
}

TableLine* Table::add_title(const std::string& title)
{
	TableLine* title_line = append_line(TABLE_LINE_TITLE);
	title_line->get_cell(0, nb_columns)->set_text(title);
	return title_line;
}

std::string Table::get_title(const int row)
{
	TableLine* line = get_line(row);
	if(line->get_line_type() != TableLineType::TABLE_LINE_TITLE) 
		throw std::invalid_argument("Cannot get title at line index " + std::to_string(row) + ".\n" +
			"Line at index " + std::to_string(row) + " is not a TITLE line but of type " + 
			get_line_type_as_string(line->get_line_type()) + ".");
	return line->get_cell(0, nb_columns)->get_content(false);
}

// we assume that title string is written in UTF8 format
void Table::set_title(const int row, const std::string title)
{
	TableLine* line = get_line(row);
	if(line->get_line_type() != TableLineType::TABLE_LINE_TITLE) 
		throw std::invalid_argument("Cannot set table title at index " + std::to_string(row) + ".\n" + 
			"Line at index " + std::to_string(row) + " is not a TITLE line but of type " + 
			get_line_type_as_string(line->get_line_type()) + ".");
	line->get_cell(0, nb_columns)->set_text(title);
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
