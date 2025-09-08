#pragma once

#include "table.h"


// ================ CELL ================

static void copy_cell(TCELL* c_cell_dest, const TCELL* c_cell_src)
{
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
	this->content = NULL;
	copy_cell(this, &other);
}

TableCell::~TableCell() {}

void TableCell::free()
{
	T_free_cell(this);
}

bool TableCell::is_null() const
{
	return content == NULL;
}

// The table cell contains a "packed" IDT object (lec + clec) 
// -> see T_set_lec_cell from k_tbl.c
CLEC* TableCell::get_compiled_lec()
{
	if(type != TABLE_CELL_LEC)
		throw std::runtime_error("Cannot get the compiled LEC. The table cell does not contain a LEC expression");

	if(content == NULL)
		throw std::runtime_error("Cannot get the compiled LEC. The table cell is empty");

	// see VT_edit() from o_vt.c from the old GUI
	return (CLEC*) P_get_ptr(content, 1);
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
	if (type != other.type) return false;
	if (attribute != other.attribute) return false;
	// need to create a copy because T_cell_cont returns a pointer to the global 
	// allocated buffer BUF_DATA (see buf.c)
	char* content1 = copy_char_array(T_cell_cont((TCELL*) this, 0));
	char* content2 = copy_char_array(T_cell_cont((TCELL*)(&other), 0));
	bool is_same_content = strcmp(content1, content2) == 0;
	delete[] content1;
	delete[] content2;
	return is_same_content;
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
	c_line_dest->unused = c_line_src->unused;

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
	this->unused = 0;
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
			cells[col].content = NULL;
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
	return right_axis == 0;
}

void TableLine::set_line_axis(const bool is_left)
{
	right_axis = is_left ? 0 : 1;
}

unsigned char TableLine::get_line_pbyte() const
{
	return unused;
}

void TableLine::set_line_pbyte(const unsigned char pbyte)
{
	unused = pbyte;
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
	if (unused != other.unused) return false;

	TableCell* cells_this = (TableCell*) cells;
	TableCell* cells_other = (TableCell*) other.cells;
	if(type == TABLE_LINE_TITLE)
		return cells_this->get_content(false) == cells_other->get_content(false);
	else if(type == TABLE_LINE_CELL)
	{
		for (int col = 0; col < nb_cells; col++) 
			if(cells_this[col] != cells_other[col]) return false;
		return true;
	}
	else
		// cells == NULL for FILES, MODE, LINE and DATE type
		return true;
}


// ================ TABLE ================


bool table_equal(const TBL& table1, const TBL& table2)
{
	if(table1.t_lang != table2.t_lang) return false;
	if(table1.t_free != table2.t_free) return false;
	if(table1.t_nl != table2.t_nl) return false;
	if(table1.t_nc != table2.t_nc) return false;

	int nb_columns = table1.t_nc;
	TableLine* div1 = (TableLine*) const_cast<TLINE*>(&table1.t_div);
	TableLine* div2 = (TableLine*) const_cast<TLINE*>(&table2.t_div);
	if(!div1->equals(*div2, nb_columns))
		return false;

	TableLine* cells1 = (TableLine*) table1.t_line;
	TableLine* cells2 = (TableLine*) table2.t_line;
	for (int i = 0; i < table1.t_nl; i++)
		if (!cells1[i].equals(cells2[i], nb_columns)) 
			return false;

	if(table1.t_zmin != table2.t_zmin) return false;
	if(table1.t_zmax != table2.t_zmax) return false;
	if(table1.t_ymin != table2.t_ymin) return false;
	if(table1.t_ymax != table2.t_ymax) return false;
	if(table1.t_attr != table2.t_attr) return false;
	if(table1.t_box != table2.t_box) return false;
	if(table1.t_shadow != table2.t_shadow) return false;
	if(table1.t_gridx != table2.t_gridx) return false;
	if(table1.t_gridy != table2.t_gridy) return false;
	if(table1.t_axis != table2.t_axis) return false;
	if(table1.t_align != table2.t_align) return false;

	return true;
}

void Table::initialize(const int nb_columns)
{
	t_nc = nb_columns;

	// NOTE: T_create() - returns an allocated TBL* pointer
	//                  - initializes t_nc, t_lang, t_zmin, t_zmax, t_ymin, t_ymax and t_div
	TBL* c_table = T_create(nb_columns);

    t_div.type = TABLE_LINE_CELL;
    t_div.cells  = SW_nalloc(nb_columns * sizeof(TCELL));
	copy_line(nb_columns, &t_div, &c_table->t_div);

	t_lang = c_table->t_lang;
	t_zmin = c_table->t_zmin;
	t_zmax = c_table->t_zmax;
	t_ymin = c_table->t_ymin;
	t_ymax = c_table->t_ymax;

	T_free(c_table);

	t_nl = 0;
	t_line = NULL;
	t_attr = TABLE_CELL_NORMAL;
	t_box = 0;
	t_shadow = 0;
	t_gridx = TABLE_GRAPH_MAJOR;
	t_gridy = TABLE_GRAPH_MAJOR;
	t_axis = TABLE_GRAPH_VALUES;
	t_align = TABLE_GRAPH_LEFT;
}

void Table::copy_from_TBL_obj(const TBL* obj)
{
	initialize(obj->t_nc);

	t_lang = obj->t_lang;
	t_free = obj->t_free;
	t_nl = 0;
	t_nc = obj->t_nc;
	copy_line(t_nc, &t_div, &obj->t_div);

	TLINE* lines;
	for (int i = 0; i < obj->t_nl; i++)
	{
		lines = &obj->t_line[i];
		switch (lines->type)
		{
		case TABLE_LINE_TITLE:
			T_insert_line(this, t_nl - 1, TABLE_LINE_TITLE, 0);
			copy_line(t_nc, &t_line[i], &obj->t_line[i]);
			break;
		case TABLE_LINE_CELL:
			T_insert_line(this, t_nl - 1, TABLE_LINE_CELL, 0);
			copy_line(t_nc, &t_line[i], &obj->t_line[i]);
			break;
		case TABLE_LINE_SEP:
			T_insert_line(this, t_nl - 1, TABLE_LINE_SEP, 0);
			break;
		case TABLE_LINE_MODE:
			T_insert_line(this, t_nl - 1, TABLE_LINE_MODE, 0);
			break;
		case TABLE_LINE_FILES:
			T_insert_line(this, t_nl - 1, TABLE_LINE_FILES, 0);
			break;
		case TABLE_LINE_DATE:
			T_insert_line(this, t_nl - 1, TABLE_LINE_DATE, 0);
			break;
		default:
			break;
		}
	}

	t_zmin   = obj->t_zmin;
	t_zmax   = obj->t_zmax;
	t_ymin   = obj->t_ymin;
	t_ymax   = obj->t_ymax;
	t_attr   = obj->t_attr;
	t_box    = obj->t_box;
	t_shadow = obj->t_shadow;
	t_gridx  = obj->t_gridx;
	t_gridy  = obj->t_gridy;
	t_axis   = obj->t_axis;
	t_align  = obj->t_align;
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
	return v_table_langs.at(t_lang - TABLE_ENGLISH);
}

void Table::set_language(const TableLang lang)
{
	t_lang = (short) lang;
}

short Table::nb_columns() const
{
	return t_nc;
}

short Table::nb_lines() const
{
	return t_nl;
}

TableGraphGrid Table::get_gridx() const
{
	return static_cast<TableGraphGrid>(t_gridx);
}

void Table::set_gridx(const TableGraphGrid gridx)
{
	t_gridx = gridx - TABLE_GRAPH_MAJOR;
}

TableGraphGrid Table::get_gridy() const
{
	return static_cast<TableGraphGrid>(t_gridy);
}

void Table::set_gridy(const TableGraphGrid gridy)
{
	t_gridy = gridy;
}

TableGraphAxis Table::get_graph_axis() const
{
	return static_cast<TableGraphAxis>(t_axis);
}

void Table::set_graph_axis(const TableGraphAxis axis)
{
	t_axis = axis;
}

TableGraphAlign Table::get_graph_alignment() const
{
	return static_cast<TableGraphAlign>(t_align);
}

void Table::set_graph_alignment(const TableGraphAlign align)
{
	t_align = align;
}

// -------- LINES --------

TableLine* Table::get_line(const int row)
{
	if (row < 0 || row > nb_lines())
		throw std::invalid_argument("Cannot get table line at index " + std::to_string(row) + ".\n" + 
			"Line index must be in range [0, " + std::to_string(nb_lines()) + "]");

	return static_cast<TableLine*>(&this->t_line[row]);
}

TableLine* Table::insert_line(const int pos, const TableLineType line_type, const bool after)
{
	if (pos < 0 || pos > nb_lines())
		throw std::invalid_argument("Cannot insert table line at index " + std::to_string(pos) + ".\n" +  
			"New line index must be in range [0, " + std::to_string(nb_lines() - 1) + "]");

	int where_ = after ? 0 : 1;
	// WARNING: When inserting a new line of type TABLE_CELL, the attribute TCELL::type of cells is undefined!
	int new_pos = T_insert_line(this, pos, line_type, where_);
	if (new_pos < 0) 
		throw std::runtime_error("Cannot insert table line at position " + std::to_string(pos));

	return static_cast<TableLine*>(&this->t_line[new_pos]);
}

// -------- DIVIDER --------

TableLine* Table::get_divider_line()
{
	return static_cast<TableLine*>(&this->t_div);
}

// -------- TITLE --------

// we assume that title string is written in UTF8 format
TableLine* Table::insert_title(const int pos, const std::string& title, const bool after)
{
	TableLine* title_line = insert_line(pos, TABLE_LINE_TITLE, after);
	title_line->get_cell(0, t_nc)->set_text(title);
	return title_line;
}

TableLine* Table::add_title(const std::string& title)
{
	return insert_title(nb_lines() - 1, title);
}

std::string Table::get_title(const int row)
{
	TableLine* line = get_line(row);
	if(line->get_line_type() != TableLineType::TABLE_LINE_TITLE) 
		throw std::invalid_argument("Cannot get title at line index " + std::to_string(row) + ".\n" +
			"Line at index " + std::to_string(row) + " is not a TITLE line but of type " + 
			get_line_type_as_string(line->get_line_type()) + ".");
	return line->get_cell(0, t_nc)->get_content(false);
}

// we assume that title string is written in UTF8 format
void Table::set_title(const int row, const std::string title)
{
	TableLine* line = get_line(row);
	if(line->get_line_type() != TableLineType::TABLE_LINE_TITLE) 
		throw std::invalid_argument("Cannot set table title at index " + std::to_string(row) + ".\n" + 
			"Line at index " + std::to_string(row) + " is not a TITLE line but of type " + 
			get_line_type_as_string(line->get_line_type()) + ".");
	line->get_cell(0, t_nc)->set_text(title);
}

// -------- CELLS --------

TableLine* Table::insert_line_with_cells(const int pos, const bool after)
{
	return insert_line(pos, TABLE_LINE_CELL, after);
}

TableLine* Table::add_line_with_cells()
{
	return insert_line_with_cells(nb_lines() - 1);
}

// -------- SEPARATOR --------

TableLine* Table::insert_line_separator(const int pos, const bool after)
{
	return insert_line(pos, TABLE_LINE_SEP, after);
}

TableLine* Table::add_line_separator()
{
	return insert_line_separator(nb_lines() - 1);
}

// -------- MODE --------

TableLine* Table::insert_line_mode(const int pos, const bool after)
{
	return insert_line(pos, TABLE_LINE_MODE, after);
}

TableLine* Table::add_line_mode()
{
	return insert_line_mode(nb_lines() - 1);
}

// -------- FILES --------

TableLine* Table::insert_line_files(const int pos, const bool after)
{
	return insert_line(pos, TABLE_LINE_FILES, after);
}

TableLine* Table::add_line_files()
{
	return insert_line_files(nb_lines() - 1);
}

// -------- DATE --------

TableLine* Table::insert_line_date(const int pos, const bool after)
{
	return insert_line(pos, TABLE_LINE_DATE, after);
}

TableLine* Table::add_line_date()
{
	return insert_line_date(nb_lines() - 1);
}

// -------- FREE --------

void Table::delete_line(const int row)
{
	if(row < 0)
		throw std::invalid_argument("Cannot delete table line.\nTable line index cannot be negative.");

	if(row >= nb_lines())
		throw std::invalid_argument("Cannot delete line at index " + std::to_string(row) + ".\n" +
			"Line index must be in range [0, " + std::to_string(nb_lines()) + "]." );

	// pointer to all lines of the table
    TLINE* lines = t_line;

    /* free line at position row */
    T_free_line(&lines[row], nb_columns());

    /* shift all lines after position row + 1 */
	if(row < nb_lines() - 1)
    	memcpy(lines + row, lines + row + 1, (t_nl - row - 1) * sizeof(TLINE));

	// update the total number of lines
    t_nl--;

    if(t_nl > 0) 
	{
		// set the last line of the table to 0 
		memset(lines + t_nl, 0, sizeof(TLINE));
		// reserve memory if needed
		if(t_nl % KT_CHUNCK == 0)
			t_line = (TLINE *) SW_nrealloc(lines, 
				sizeof(TLINE) * (t_nl + KT_CHUNCK), sizeof(TLINE) * t_nl);
	}
    else 
	{
		SW_nfree(t_line);
		t_line = NULL;
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

    T_free_line(&t_div, t_nc);

    for(int i = 0; i < t_nl; i++)
		T_free_line(&t_line[i], t_nc);
    SW_nfree(t_line);
}

std::size_t hash_value(const Table& table)
{
    std::hash<TBL> tbl_hash;
    return tbl_hash(table);
}
