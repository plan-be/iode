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
	const bool bold, const bool italic, const bool underline)
{
	this->type = (char) cell_type;
	if(cell_type == TableCellType::TABLE_CELL_STRING)
		set_text(content);
	else
		set_lec(content);
	this->attribute = (char) align;
	set_bold(bold);
	set_italic(italic);
	set_underline(underline);
}

TableCell::TableCell(const TableCell& other)
{
	this->content = "";
	this->idt = nullptr;
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
	return idt->clec;
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
 * 
 * @note When inserting a new line of type TABLE_CELL, the attribute TCELL::type of cells is undefined!
 *       See function `T_create_cell()` in file `k_tbl.c` from the C API 
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
	unsigned char* cell_src_content;
	TCELL* cells_dest = (TCELL*)c_line_dest->cells;
	TCELL* cells_src = (TCELL*)c_line_src->cells;

	c_line_dest->type = c_line_src->type;
	c_line_dest->right_axis = c_line_src->right_axis;
	c_line_dest->graph_type = c_line_src->graph_type;

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

TableLine::TableLine(const TableLineType line_type, const TableGraphType graph_type, const bool axis_left)
{
	this->type = (char) line_type;
	this->cells = NULL;
	set_line_graph(graph_type);
	set_line_axis(axis_left);
}

TableLine::TableLine(const TableLine& other, const int nb_cells)
{
	TCELL* cells;

	this->cells = NULL;
	switch (other.type)
	{
	case TABLE_LINE_TITLE:
		this->cells = SW_nalloc(sizeof(TCELL));
		break;
	case TABLE_LINE_CELL:
		this->cells = SW_nalloc(nb_cells * sizeof(TCELL));
		cells = (TCELL*) this->cells;
		for (int col = 0; col < nb_cells; col++)
			cells[col].content = "";
		break;
	default:
		break;
	}
	
	copy_line(nb_cells, this, &other);
}

TableLine::~TableLine() {}

void TableLine::free(const int nb_cells)
{
	T_free_line(this, nb_cells);
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

	TableCell* cells = reinterpret_cast<TableCell*>(this->cells);
	return &cells[column];
}

bool TableLine::equals(const TableLine& other, const int nb_cells) const
{
	if (type != other.type) return false;
	if (right_axis != other.right_axis) return false;
	if (graph_type != other.graph_type) return false;

	TableCell* cells_this = (TableCell*) cells;
	TableCell* cells_other = (TableCell*) other.cells;
	TableCell* cell_this;
	TableCell* cell_other;
	if(type == TABLE_LINE_TITLE)
	{
		cell_this = cells_this;
		cell_other = cells_other;
		if(cell_this == nullptr && cell_other == nullptr) 
			return true;
		if(cell_this == nullptr || cell_other == nullptr) 
			return false;
		return cell_this->get_content(false) == cell_other->get_content(false);
	}
	else if(type == TABLE_LINE_CELL)
	{
		for (int col = 0; col < nb_cells; col++)
		{
			cell_this = cells_this + col;
			cell_other = cells_other + col;
			if(cell_this == nullptr && cell_other == nullptr) 
				continue;
			if(cell_this == nullptr || cell_other == nullptr) 
				return false;
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
	if(table1.nb_lines != table2.nb_lines) return false;
	if(table1.nb_columns != table2.nb_columns) return false;

	int nb_columns = table1.nb_columns;
	TableLine* div1 = (TableLine*) const_cast<TLINE*>(&table1.divider_line);
	TableLine* div2 = (TableLine*) const_cast<TLINE*>(&table2.divider_line);
	if(!div1->equals(*div2, nb_columns))
		return false;

	TableLine* cells1 = (TableLine*) table1.lines;
	TableLine* cells2 = (TableLine*) table2.lines;
	for (int i = 0; i < table1.nb_lines; i++)
		if (!cells1[i].equals(cells2[i], nb_columns)) 
			return false;

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

void Table::initialize(const int nb_columns)
{
	this->nb_columns = nb_columns;

	// NOTE: T_create() - returns an allocated TBL* pointer
	//                  - initializes nb_columns, language, t_zmin, t_zmax, t_ymin, t_ymax and divider_line
	TBL* c_table = T_create(nb_columns);

    divider_line.type = TABLE_LINE_CELL;
    divider_line.cells  = SW_nalloc(nb_columns * sizeof(TCELL));
	copy_line(nb_columns, &divider_line, &c_table->divider_line);

	this->language = c_table->language;
	this->z_min = c_table->z_min;
	this->z_max = c_table->z_max;
	this->y_min = c_table->y_min;
	this->y_max = c_table->y_max;

	T_free(c_table);

	this->nb_lines = 0;
	this->lines = NULL;
	this->attribute = TABLE_CELL_NORMAL;
	this->chart_box = 0;
	this->chart_shadow = 0;
	this->chart_gridx = TABLE_GRAPH_MAJOR;
	this->chart_gridy = TABLE_GRAPH_MAJOR;
	this->chart_axis_type = TABLE_GRAPH_VALUES;
	this->text_alignment = TABLE_GRAPH_LEFT;
}

void Table::copy_from_TBL_obj(const TBL* obj)
{
	initialize(obj->nb_columns);

	this->language = obj->language;
	this->repeat_columns = obj->repeat_columns;
	this->nb_lines = 0;
	this->nb_columns = obj->nb_columns;
	copy_line(nb_columns, &divider_line, &obj->divider_line);

	for (int i = 0; i < obj->nb_lines; i++)
	{
		switch (obj->lines[i].type)
		{
		case TABLE_LINE_TITLE:
			T_insert_line(this, nb_lines - 1, TABLE_LINE_TITLE, 0);
			copy_line(nb_columns, &this->lines[i], &obj->lines[i]);
			break;
		case TABLE_LINE_CELL:
			T_insert_line(this, nb_lines - 1, TABLE_LINE_CELL, 0);
			copy_line(nb_columns, &this->lines[i], &obj->lines[i]);
			break;
		case TABLE_LINE_SEP:
			T_insert_line(this, nb_lines - 1, TABLE_LINE_SEP, 0);
			break;
		case TABLE_LINE_MODE:
			T_insert_line(this, nb_lines - 1, TABLE_LINE_MODE, 0);
			break;
		case TABLE_LINE_FILES:
			T_insert_line(this, nb_lines - 1, TABLE_LINE_FILES, 0);
			break;
		case TABLE_LINE_DATE:
			T_insert_line(this, nb_lines - 1, TABLE_LINE_DATE, 0);
			break;
		default:
			break;
		}
	}

	this->z_min = obj->z_min;
	this->z_max = obj->z_max;
	this->y_min = obj->y_min;
	this->y_max = obj->y_max;
	this->attribute    = obj->attribute;
	this->chart_box    = obj->chart_box;
	this->chart_shadow = obj->chart_shadow;
	this->chart_gridx  = obj->chart_gridx;
	this->chart_gridy  = obj->chart_gridy;
	this->chart_axis_type = obj->chart_axis_type;
	this->text_alignment  = obj->text_alignment;
}

Table::Table(const int nb_columns)
{
	initialize(nb_columns);
}

Table::Table(const int nb_columns, const std::string& def, const std::vector<std::string>& vars, 
	bool mode, bool files, bool date)
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

	initialize(nb_columns);
	T_auto(this, c_def, c_lecs, c_mode, c_files, c_date);

	SCR_free_tbl((unsigned char**) c_lecs);
}

Table::Table(const int nb_columns, const std::string& def, const std::vector<std::string>& titles, 
		     const std::vector<std::string>& lecs, bool mode, bool files, bool date)
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

	initialize(nb_columns);
	T_default(this, c_def, c_titles, c_lecs, c_mode, c_files, c_date);

	SCR_free_tbl((unsigned char**) c_titles);
	SCR_free_tbl((unsigned char**) c_lecs);
}

Table::Table(const int nb_columns, const std::string& def, const std::string& lecs, 
	bool mode, bool files, bool date)
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

	initialize(nb_columns);
	T_auto(this, c_def, c_lecs, c_mode, c_files, c_date);

	SCR_free_tbl((unsigned char**) c_lecs);
}

Table::Table(const TBL* c_table)
{
	copy_from_TBL_obj(c_table);
}

Table::Table(const Table& table)
{
	copy_from_TBL_obj(static_cast<const TBL*>(&table));
}

Table::~Table()
{      
    free_all_lines();
}

// required to be used in std::map
Table& Table::operator=(const Table& table)
{
	free_all_lines();
    copy_from_TBL_obj(static_cast<const TBL*>(&table));
    return *this;
}

// ================ TABLE ================

void Table::extend()
{
	int res = T_add_line(this);
	if (res < 0) 
		throw std::runtime_error("Cannot extend table with title \"" + 
								 std::string((char*)T_get_title(this)) + "\"");
}

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
	if (row < 0 || row > nb_lines)
		throw std::invalid_argument("Cannot get table line at index " + std::to_string(row) + ".\n" + 
			"Line index must be in range [0, " + std::to_string(nb_lines) + "]");

	return static_cast<TableLine*>(&this->lines[row]);
}

TableLine* Table::insert_line(const int pos, const TableLineType line_type, const bool after)
{
	if (pos < 0 || pos > nb_lines)
		throw std::invalid_argument("Cannot insert table line at index " + std::to_string(pos) + ".\n" +  
			"New line index must be in range [0, " + std::to_string(nb_lines - 1) + "]");

	int where_ = after ? 0 : 1;
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
	return insert_title(nb_lines - 1, title);
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
	return insert_line_with_cells(nb_lines - 1);
}

// -------- SEPARATOR --------

TableLine* Table::insert_line_separator(const int pos, const bool after)
{
	return insert_line(pos, TABLE_LINE_SEP, after);
}

TableLine* Table::add_line_separator()
{
	return insert_line_separator(nb_lines - 1);
}

// -------- MODE --------

TableLine* Table::insert_line_mode(const int pos, const bool after)
{
	return insert_line(pos, TABLE_LINE_MODE, after);
}

TableLine* Table::add_line_mode()
{
	return insert_line_mode(nb_lines - 1);
}

// -------- FILES --------

TableLine* Table::insert_line_files(const int pos, const bool after)
{
	return insert_line(pos, TABLE_LINE_FILES, after);
}

TableLine* Table::add_line_files()
{
	return insert_line_files(nb_lines - 1);
}

// -------- DATE --------

TableLine* Table::insert_line_date(const int pos, const bool after)
{
	return insert_line(pos, TABLE_LINE_DATE, after);
}

TableLine* Table::add_line_date()
{
	return insert_line_date(nb_lines - 1);
}

// -------- FREE --------

void Table::delete_line(const int row)
{
	if(row < 0)
		throw std::invalid_argument("Cannot delete table line.\nTable line index cannot be negative.");

	if(row >= nb_lines)
		throw std::invalid_argument("Cannot delete line at index " + std::to_string(row) + ".\n" +
			"Line index must be in range [0, " + std::to_string(nb_lines) + "]." );

    /* free line at position row */
    T_free_line(&lines[row], nb_columns);

    /* shift all lines after position row + 1 */
	if(row < nb_lines - 1)
    	memcpy(lines + row, lines + row + 1, (nb_lines - row - 1) * sizeof(TLINE));

	// update the total number of lines
    nb_lines--;

    if(nb_lines > 0) 
	{
		// set the last line of the table to 0 
		memset(lines + nb_lines, 0, sizeof(TLINE));
		// reserve memory if needed
		if(nb_lines % KT_CHUNCK == 0)
			lines = (TLINE *) SW_nrealloc(lines, 
				sizeof(TLINE) * (nb_lines + KT_CHUNCK), sizeof(TLINE) * nb_lines);
	}
    else 
	{
		SW_nfree(lines);
		lines = NULL;
	}
}

// -------- EQUAL --------

bool Table::operator==(const Table& other) const
{
	return table_equal(static_cast<TBL>(*this), static_cast<TBL>(other));
}

// ================ TABLE (PRIVATE) ================

void Table::free_all_lines()
{

    T_free_line(&divider_line, nb_columns);

    for(int i = 0; i < nb_lines; i++)
		T_free_line(&lines[i], nb_columns);
    SW_nfree(lines);
}

std::size_t hash_value(const Table& table)
{
    std::hash<TBL> tbl_hash;
    return tbl_hash(table);
}
