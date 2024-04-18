#pragma once

#include "table.h"


// ================ CELL ================

static void copy_cell(TCELL* c_cell_dest, const TCELL* c_cell_src)
{
	unsigned char* cell_src_content = (unsigned char*) T_cell_cont(const_cast<TCELL*>(c_cell_src), 0);
	if (c_cell_src->tc_type == IT_LEC) 
		T_set_lec_cell(c_cell_dest, cell_src_content);
	else 
		T_set_string_cell(c_cell_dest, cell_src_content);
	c_cell_dest->tc_attr = c_cell_src->tc_attr;
}

TableCell::TableCell(const EnumCellType cell_type, const std::string& content, const EnumCellAlign align, 
	const bool bold, const bool italic, const bool underline)
{
	this->tc_type = (char) cell_type;
	if(cell_type == EnumCellType::IT_STRING)
		set_text(content);
	else
		set_lec(content);
	this->tc_attr = (char) align;
	set_bold(bold);
	set_italic(italic);
	set_underline(underline);
}

TableCell::~TableCell() {}

void TableCell::free()
{
	T_free_cell(this);
}

// The table cell contains a "packed" IDT object (lec + clec) 
// -> see T_set_lec_cell from k_tbl.c
CLEC* TableCell::get_compiled_lec()
{
	if(tc_type != IT_LEC)
		throw std::runtime_error("Cannot get the compiled LEC. The table cell does not contain a LEC expression");

	if(tc_val == NULL)
		throw std::runtime_error("Cannot get the compiled LEC. The table cell is empty");

	// see VT_edit() from o_vt.c from the old GUI
	return (CLEC*) P_get_ptr(tc_val, 1);
}

std::vector<std::string> TableCell::get_variables_from_lec()
{
	CLEC* clec = get_compiled_lec();
	return get_variables_from_clec(clec);
}

std::string TableCell::get_content(const bool quotes) const
{
	int mode = quotes ? 1 : 0;
	std::string content_oem = std::string(T_cell_cont((TCELL*) this, mode));
	std::string content = (tc_type == IT_STRING) ? oem_to_utf8(content_oem) : content_oem;
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
 * @note When inserting a new line of type IT_CELL, the attribute TCELL::tc_type of cells is undefined!
 *       See function `T_create_cell()` in file `k_tbl.c` from the C API 
 */
void TableCell::set_content(const std::string& content)
{
	if(content.starts_with('\"'))
		set_text(content);
	else
		set_lec(content);
}

EnumCellType TableCell::get_type() const
{
	return static_cast<EnumCellType>(tc_type);
}

void TableCell::set_type(const EnumCellType cell_type)
{
	this->tc_type = cell_type;
}

// TODO: check if it is correct
EnumCellAlign TableCell::get_align() const
{
	return static_cast<EnumCellAlign>((int) (this->tc_attr / 8) * 8);
}

void TableCell::set_align(const EnumCellAlign align)
{
	char font = ((int) this->tc_attr) % 8;
	this->tc_attr = ((char) align) + font;
}

bool TableCell::is_bold() const
{
	return bitset_8(this->tc_attr).test(0);
}

void TableCell::set_bold(const bool value)
{
	bitset_8 attr(this->tc_attr);
	attr.set(0, value);
	this->tc_attr = (char) attr.to_ulong();
}

bool TableCell::is_italic() const
{
	return bitset_8(this->tc_attr).test(1);
}

void TableCell::set_italic(const bool value)
{
	bitset_8 attr(this->tc_attr);
	attr.set(1, value);
	this->tc_attr = (char) attr.to_ulong();
}

bool TableCell::is_underline() const
{
	return bitset_8(this->tc_attr).test(2);
}

void TableCell::set_underline(const bool value)
{
	bitset_8 attr(this->tc_attr);
	attr.set(2, value);
	this->tc_attr = (char) attr.to_ulong();
}

bool TableCell::operator==(const TableCell& other) const
{
	if (tc_type != other.tc_type) return false;
	if (tc_attr != other.tc_attr) return false;
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

static void copy_line(const int nb_columns, TLINE* c_cell_dest, const TLINE* c_cell_src)
{
	unsigned char* cell_src_content;
	TCELL* cells_dest = (TCELL*)c_cell_dest->tl_val;
	TCELL* cells_src = (TCELL*)c_cell_src->tl_val;

	c_cell_dest->tl_type = c_cell_src->tl_type;
	c_cell_dest->tl_axis = c_cell_src->tl_axis;
	c_cell_dest->tl_graph = c_cell_src->tl_graph;
	c_cell_dest->tl_pbyte = c_cell_src->tl_pbyte;

	switch (c_cell_src->tl_type)
	{
	case IT_TITLE:
		cell_src_content = (unsigned char*) T_cell_cont(cells_src, 0);
		T_set_string_cell(cells_dest, cell_src_content);
		break;
	case IT_CELL:
		for (int col = 0; col < nb_columns; col++) 
			copy_cell(&cells_dest[col], &cells_src[col]);
		break;
	default:
		break;
	}
}

TableLine::TableLine(const EnumLineType line_type, const EnumGraphType graph_type, const bool axis_left)
{
	this->tl_type = (char) line_type;
	this->tl_val = NULL;
	set_line_graph(graph_type);
	this->tl_pbyte = 0;
	set_line_axis(axis_left);
}

TableLine::~TableLine() {}

void TableLine::free(const int nb_cells)
{
	T_free_line(this, nb_cells);
}

EnumLineType TableLine::get_line_type() const
{
	return static_cast<EnumLineType>(tl_type);
}

void TableLine::set_line_type(const EnumLineType line_type)
{
	tl_type = line_type;
}

EnumGraphType TableLine::get_line_graph() const
{
	return static_cast<EnumGraphType>(tl_graph);
}

void TableLine::set_line_graph(const EnumGraphType graph_type)
{
	tl_graph = graph_type;
}

bool TableLine::is_left_axis() const
{
	return tl_axis == 0;
}

void TableLine::set_line_axis(const bool is_left)
{
	tl_axis = is_left ? 0 : 1;
}

unsigned char TableLine::get_line_pbyte() const
{
	return tl_pbyte;
}

void TableLine::set_line_pbyte(const unsigned char pbyte)
{
	tl_pbyte = pbyte;
}

TableCell* TableLine::get_cell(const int column, const int nb_cells) const
{
	if(column < 0)
		throw std::invalid_argument("Table cell position cannot be negative");

	EnumLineType line_type = (EnumLineType) this->tl_type;
	if(line_type != IT_TITLE && line_type != IT_CELL)
		throw std::runtime_error("Table line of type " + get_line_type_as_string(line_type) + " has no cells"); 

	if(line_type == IT_TITLE && column > 0)
		throw std::invalid_argument("Table cell position for a TITLE line must be 0");

	if(line_type == IT_CELL && column >= nb_cells)
		throw std::invalid_argument("Table cell position cannot exceed " + std::to_string(nb_cells));

	TableCell* cells = reinterpret_cast<TableCell*>(this->tl_val);
	return &cells[column];
}

bool TableLine::equals(const TableLine& other, const int nb_cells) const
{
	if (tl_type != other.tl_type) return false;
	if (tl_axis != other.tl_axis) return false;
	if (tl_graph != other.tl_graph) return false;
	if (tl_pbyte != other.tl_pbyte) return false;

	TableCell* cells = (TableCell*) tl_val;
	TableCell* cells_other = (TableCell*) other.tl_val;
	if(tl_type == IT_TITLE)
		return cells->get_content(false) == cells_other->get_content(false);
	else if(tl_type == IT_CELL)
	{
		for (int col = 0; col < nb_cells; col++) 
			if(cells[col] != cells_other[col]) return false;
		return true;
	}
	else
		// tl_val == NULL for FILES, MODE, LINE and DATE type
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

static int _nb_columns_;

std::size_t hash_value(TLINE const& c_line)
{
	std::size_t seed = 0;

	boost::hash_combine(seed, c_line.tl_type);
	boost::hash_combine(seed, c_line.tl_axis);
	boost::hash_combine(seed, c_line.tl_graph);

	TCELL* cells;
	TCELL* cell;
	switch(c_line.tl_type)
	{
	case IT_TITLE:
		cells = (TCELL*) c_line.tl_val;
		boost::hash_combine(seed, cells->tc_type);
		boost::hash_combine(seed, cells->tc_attr);
		boost::hash_combine(seed, std::string(cells->tc_val));
		break;
	case IT_CELL:
		cells = (TCELL*) c_line.tl_val;
		for(int col = 0; col < _nb_columns_; col++)
		{
			cell = &cells[col];
			boost::hash_combine(seed, cell->tc_type);
			boost::hash_combine(seed, cell->tc_attr);
			boost::hash_combine(seed, std::string(T_cell_cont(cell, 0)));
		}
		break;
	default:
		break;
	}

	return seed;
}


void Table::initialize(const int nb_columns)
{
	t_nc = nb_columns;

	// NOTE: T_create() - returns an allocated TBL* pointer
	//                  - initializes t_nc, t_lang, t_zmin, t_zmax, t_ymin, t_ymax and t_div
	TBL* c_table = T_create(nb_columns);

    t_div.tl_type = KT_CELL;
    t_div.tl_val  = SW_nalloc(nb_columns * sizeof(TCELL));
	copy_line(nb_columns, &t_div, &c_table->t_div);

	t_lang = c_table->t_lang;
	t_zmin = c_table->t_zmin;
	t_zmax = c_table->t_zmax;
	t_ymin = c_table->t_ymin;
	t_ymax = c_table->t_ymax;

	T_free(c_table);

	t_nl = 0;
	t_line = NULL;
	t_attr = IT_NORMAL;
	t_box = 0;
	t_shadow = 0;
	t_gridx = IG_MAJOR;
	t_gridy = IG_MAJOR;
	t_axis = IG_VALUES;
	t_align = IG_LEFT;
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
		switch (lines->tl_type)
		{
		case IT_TITLE:
			T_insert_line(this, t_nl - 1, IT_TITLE, 0);
			copy_line(t_nc, &t_line[i], &obj->t_line[i]);
			break;
		case IT_CELL:
			T_insert_line(this, t_nl - 1, IT_CELL, 0);
			copy_line(t_nc, &t_line[i], &obj->t_line[i]);
			break;
		case IT_LINE:
			T_insert_line(this, t_nl - 1, IT_LINE, 0);
			break;
		case IT_MODE:
			T_insert_line(this, t_nl - 1, IT_MODE, 0);
			break;
		case IT_FILES:
			T_insert_line(this, t_nl - 1, IT_FILES, 0);
			break;
		case IT_DATE:
			T_insert_line(this, t_nl - 1, IT_DATE, 0);
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

Table::Table(const int pos, KDB* kdb)
{
	if (!kdb) 
		kdb = K_WS[I_TABLES];

	if (pos < 0 || pos > kdb->k_nb)
	{
		IodeExceptionInvalidArguments error("Cannot extract Table");
		error.add_argument("table position", std::to_string(pos) + 
			               " (table position must be in range [0, " + std::to_string(kdb->k_nb - 1) + "])");
		throw error;
	}

	// Note: KTVAL allocate a new pointer TBL*
	TBL* tbl = KTVAL(kdb, pos);
	copy_from_TBL_obj(tbl);
	T_free(tbl);
}

Table::Table(const std::string& name, KDB* kdb)
{
	if (!kdb) 
		kdb = K_WS[I_TABLES];

	int pos = K_find(kdb, to_char_array(name));
	if (pos < 0) 
		throw IodeExceptionFunction("Cannot extract Table", "Table with name " + name + " does not exist");
	
	// Note: KTVAL allocate a new pointer TBL*
	TBL* tbl = KTVAL(kdb, pos);
	copy_from_TBL_obj(tbl);
	T_free(tbl);
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

Table::Table(const int nb_columns, const std::string& def, const std::string& lecs, 
	bool mode, bool files, bool date)
{
	char* c_def = to_char_array(def);

	char** c_lecs = NULL;
	if(!lecs.empty())
	{
		char* pattern = to_char_array(lecs);
		// Retrieves all variable names matching one or more patterns in KV_WS (similar to grep)
		char* lst = K_expand(I_VARIABLES, NULL, pattern, '*');
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
	if (res < 0) throw IodeExceptionFunction("Cannot extend table \"" + 
		std::string((char*)T_get_title(this)) + "\"", "Unknown");
}

std::string Table::get_language() const
{
	return vLangs.at(t_lang - KT_ENGLISH);
}

void Table::set_language(const EnumLang lang)
{
	t_lang = lang + KT_ENGLISH;
}

short Table::nb_columns() const
{
	return t_nc;
}

short Table::nb_lines() const
{
	return t_nl;
}

EnumGraphGrid Table::get_gridx() const
{
	return static_cast<EnumGraphGrid>(t_gridx);
}

void Table::set_gridx(const EnumGraphGrid gridx)
{
	t_gridx = gridx - IG_MAJOR;
}

EnumGraphGrid Table::get_gridy() const
{
	return static_cast<EnumGraphGrid>(t_gridy);
}

void Table::set_gridy(const EnumGraphGrid gridy)
{
	t_gridy = gridy;
}

EnumGraphAxis Table::get_graph_axis() const
{
	return static_cast<EnumGraphAxis>(t_axis);
}

void Table::set_graph_axis(const EnumGraphAxis axis)
{
	t_axis = axis;
}

EnumGraphAlign Table::get_graph_alignment() const
{
	return static_cast<EnumGraphAlign>(t_align);
}

void Table::set_graph_alignment(const EnumGraphAlign align)
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

TableLine* Table::insert_line(const int pos, const EnumLineType line_type, const bool after)
{
	if (pos < 0 || pos > nb_lines())
		throw std::invalid_argument("Cannot insert table line at index " + std::to_string(pos) + ".\n" +  
			"New line index must be in range [0, " + std::to_string(nb_lines() - 1) + "]");

	int where_ = after ? 0 : 1;
	// WARNING: When inserting a new line of type IT_CELL, the attribute TCELL::tc_type of cells is undefined!
	int new_pos = T_insert_line(this, pos, line_type, where_);
	if (new_pos < 0) 
		throw IodeExceptionFunction("Cannot insert table line at position " + std::to_string(pos), "Unknown");

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
	TableLine* title_line = insert_line(pos, IT_TITLE, after);
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
	if(line->get_line_type() != EnumLineType::IT_TITLE) 
		throw std::invalid_argument("Cannot get title at line index " + std::to_string(row) + ".\n" +
			"Line at index " + std::to_string(row) + " is not a TITLE line but of type " + 
			get_line_type_as_string(line->get_line_type()) + ".");
	return line->get_cell(0, t_nc)->get_content(false);
}

// we assume that title string is written in UTF8 format
void Table::set_title(const int row, const std::string title)
{
	TableLine* line = get_line(row);
	if(line->get_line_type() != EnumLineType::IT_TITLE) 
		throw std::invalid_argument("Cannot set table title at index " + std::to_string(row) + ".\n" + 
			"Line at index " + std::to_string(row) + " is not a TITLE line but of type " + 
			get_line_type_as_string(line->get_line_type()) + ".");
	line->get_cell(0, t_nc)->set_text(title);
}

// -------- CELLS --------

TableLine* Table::insert_line_with_cells(const int pos, const bool after)
{
	return insert_line(pos, IT_CELL, after);
}

TableLine* Table::add_line_with_cells()
{
	return insert_line_with_cells(nb_lines() - 1);
}

// -------- SEPARATOR --------

TableLine* Table::insert_line_separator(const int pos, const bool after)
{
	return insert_line(pos, IT_LINE, after);
}

TableLine* Table::add_line_separator()
{
	return insert_line_separator(nb_lines() - 1);
}

// -------- MODE --------

TableLine* Table::insert_line_mode(const int pos, const bool after)
{
	return insert_line(pos, IT_MODE, after);
}

TableLine* Table::add_line_mode()
{
	return insert_line_mode(nb_lines() - 1);
}

// -------- FILES --------

TableLine* Table::insert_line_files(const int pos, const bool after)
{
	return insert_line(pos, IT_FILES, after);
}

TableLine* Table::add_line_files()
{
	return insert_line_files(nb_lines() - 1);
}

// -------- DATE --------

TableLine* Table::insert_line_date(const int pos, const bool after)
{
	return insert_line(pos, IT_DATE, after);
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


std::size_t hash_value(TBL const& c_table)
{
	std::size_t seed = 0;

	boost::hash_combine(seed, c_table.t_lang);
	boost::hash_combine(seed, c_table.t_free);
	boost::hash_combine(seed, c_table.t_nl);
	boost::hash_combine(seed, c_table.t_nc);

	_nb_columns_ = c_table.t_nc;
	boost::hash_combine(seed, c_table.t_div);
	for (int i = 0; i < c_table.t_nl; i++)
		boost::hash_combine(seed, c_table.t_line[i]);

	boost::hash_combine(seed, c_table.t_zmin);
	boost::hash_combine(seed, c_table.t_zmax);
	boost::hash_combine(seed, c_table.t_ymin);
	boost::hash_combine(seed, c_table.t_ymax);
	boost::hash_combine(seed, c_table.t_attr);
	boost::hash_combine(seed, c_table.t_box);
	boost::hash_combine(seed, c_table.t_shadow);
	boost::hash_combine(seed, c_table.t_gridx);
	boost::hash_combine(seed, c_table.t_gridy);
	boost::hash_combine(seed, c_table.t_axis);
	boost::hash_combine(seed, c_table.t_align);

	return seed;
}

std::size_t hash_value(Table const& table)
{
	std::size_t seed = 0;

	boost::hash_combine(seed, table.t_lang);
	boost::hash_combine(seed, table.t_free);
	boost::hash_combine(seed, table.t_nl);
	boost::hash_combine(seed, table.t_nc);

	_nb_columns_ = table.t_nc;
	boost::hash_combine(seed, table.t_div);
	for (int i = 0; i < table.t_nl; i++)
		boost::hash_combine(seed, table.t_line[i]);

	boost::hash_combine(seed, table.t_zmin);
	boost::hash_combine(seed, table.t_zmax);
	boost::hash_combine(seed, table.t_ymin);
	boost::hash_combine(seed, table.t_ymax);
	boost::hash_combine(seed, table.t_attr);
	boost::hash_combine(seed, table.t_box);
	boost::hash_combine(seed, table.t_shadow);
	boost::hash_combine(seed, table.t_gridx);
	boost::hash_combine(seed, table.t_gridy);
	boost::hash_combine(seed, table.t_axis);
	boost::hash_combine(seed, table.t_align);

	return seed;
}
