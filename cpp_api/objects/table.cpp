#pragma once

#include "table.h"


static void copy_cell(TCELL* c_cell_dest, const TCELL* c_cell_src)
{
	unsigned char* cell_src_content = (unsigned char*) T_cell_cont(const_cast<TCELL*>(c_cell_src), 0);
	if (c_cell_src->tc_type == IT_LEC) 
		T_set_lec_cell(c_cell_dest, cell_src_content);
	else 
		T_set_string_cell(c_cell_dest, cell_src_content);
	c_cell_dest->tc_attr = c_cell_src->tc_attr;
}

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

static bool cell_equal(const TCELL* c_cell1, const TCELL* c_cell2)
{
	if (c_cell1->tc_type != c_cell2->tc_type) return false;
	if (c_cell1->tc_attr != c_cell2->tc_attr) return false;
	// need to create a copy because T_cell_cont returns a pointer to the global 
	// allocated buffer BUF_DATA (see buf.c)
	char* content1 = copy_char_array(T_cell_cont(const_cast<TCELL*>(c_cell1), 0));
	char* content2 = copy_char_array(T_cell_cont(const_cast<TCELL*>(c_cell2), 0));
	bool is_same_content = strcmp(content1, content2) == 0;
	delete[] content1;
	delete[] content2;
	return is_same_content;
}

static bool line_equal(const int nb_columns, const TLINE* c_line1, const TLINE* c_line2)
{
	if (c_line1->tl_type != c_line2->tl_type) return false;
	if (c_line1->tl_axis != c_line2->tl_axis) return false;
	if (c_line1->tl_graph != c_line2->tl_graph) return false;
	if (c_line1->tl_pbyte != c_line2->tl_pbyte) return false;

	TCELL* cells_line1 = (TCELL*)c_line1->tl_val;
	TCELL* cells_line2 = (TCELL*)c_line2->tl_val;
	switch (c_line1->tl_type)
	{
	case IT_TITLE:
		if (strcmp(T_cell_cont(cells_line1, 0), T_cell_cont(cells_line2, 0)) != 0) return false;
		break;
	case IT_CELL:
		for (int col = 0; col < nb_columns; col++) 
			if(!cell_equal(&cells_line1[col], &cells_line2[col])) return false;
		break;
	default:
		break;
	}

	return true;
}

bool table_equal(const TBL& table1, const TBL& table2)
{
	if(table1.t_lang != table2.t_lang) return false;
	if(table1.t_free != table2.t_free) return false;
	if(table1.t_nl != table2.t_nl) return false;
	if(table1.t_nc != table2.t_nc) return false;

	if (!line_equal(table1.t_nc, &table1.t_div, &table2.t_div)) return false;
	for (int i = 0; i < table1.t_nl; i++)
		if (!line_equal(table1.t_nc, &table1.t_line[i], &table2.t_line[i])) return false;

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

// ================ LINES ================

EnumLineType Table::get_line_type(const int row) const
{
	TLINE* line = get_line(row);
	return static_cast<EnumLineType>(line->tl_type);
}

void Table::set_line_type(const int row, const EnumLineType line_type)
{
	TLINE* line = get_line(row);
	line->tl_type = line_type;
}

EnumGraphType Table::get_line_graph(const int row) const
{
	TLINE* line = get_line(row);
	return static_cast<EnumGraphType>(line->tl_graph);
}

void Table::set_line_graph(const int row, const EnumGraphType graph_type)
{
	TLINE* line = get_line(row);
	line->tl_graph = graph_type;
}

bool Table::is_left_axis(const int row) const
{
	TLINE* line = get_line(row);
	return line->tl_axis == 0;
}

void Table::set_line_axis(const int row, const bool is_left)
{
	TLINE* line = get_line(row);
	line->tl_axis = is_left ? 0 : 1;
}

unsigned char Table::get_line_pbyte(const int row) const
{
	TLINE* line = get_line(row);
	return line->tl_pbyte;
}

void Table::set_line_pbyte(const int row, const unsigned char pbyte)
{
	TLINE* line = get_line(row);
	line->tl_pbyte = pbyte;
}

// -------- DIVIDER --------

EnumCellType Table::get_divider_cell_type(const int column) const
{
	return get_cell_type(0, column, true);
}

EnumCellAttribute Table::get_divider_cell_align(const int column) const
{
	return get_cell_align(0, column, true);
}

int Table::get_divider_cell_font(const int column) const
{
	return get_cell_font(0, column, true);
}

std::string Table::get_divider_cell_content(const int column, const bool quotes) const
{
	return get_cell_content(0, column, quotes, true);
}

void Table::set_cell_divider_text(const int column, const std::string& text)
{
	set_cell_text(0, column, text, true);
}

void Table::set_cell_divider_lec(const int column, const std::string& lec)
{
	set_cell_lec(0, column, lec, true);
}

// -------- TITLE --------

// we assume that title string is written in UTF8 format
int Table::insert_title(const int pos, const std::string& title, const bool after)
{
	int title_pos = insert_line(pos, IT_TITLE, after);
	set_title(title_pos, title);
	return title_pos;
}

int Table::add_title(const std::string& title)
{
	return insert_title(nb_lines() - 1, title);
}

std::string Table::get_title(const int row) const
{
	TLINE* title_line = get_line(row);
	EnumLineType line_type = (EnumLineType) title_line->tl_type;
	if (line_type != IT_TITLE) throw IodeExceptionFunction("Cannot get table title.", 
		"Line at position " + std::to_string(row) + " is not a Title line but of type " + 
		get_line_type_as_string(line_type) + ".");
	std::string title = get_cell_content(row, 0, false);
	return title;
}

// we assume that title string is written in UTF8 format
void Table::set_title(const int row, const std::string title)
{
	TLINE* title_line = get_line(row);
	if (title_line->tl_type != IT_TITLE) throw IodeExceptionFunction("Cannot set table title", 
		"Line at position " + std::to_string(row) + " is not a TITLE line.");
	set_cell_text(row, 0, title);
}

// -------- CELLS --------

int Table::insert_line_with_cells(const int pos, const bool after)
{
	int line_cells_pos = insert_line(pos, IT_CELL, after);
	return line_cells_pos;
}

int Table::add_line_with_cells()
{
	return insert_line_with_cells(nb_lines() - 1);
}

std::vector<std::string> Table::get_variables_from_lec_cell(const int row, const int column)
{
	CLEC* clec = get_cell_compiled_lec(row, column);
	return get_variables_from_clec(clec);
}

// -------- SEPARATOR --------

int Table::insert_line_separator(const int pos, const bool after)
{
	return insert_line(pos, IT_LINE, after);
}

int Table::add_line_separator()
{
	return insert_line_separator(nb_lines() - 1);
}

// -------- MODE --------

int Table::insert_line_mode(const int pos, const bool after)
{
	return insert_line(pos, IT_MODE, after);
}

int Table::add_line_mode()
{
	return insert_line_mode(nb_lines() - 1);
}

// -------- FILES --------

int Table::insert_line_files(const int pos, const bool after)
{
	return insert_line(pos, IT_FILES, after);
}

int Table::add_line_files()
{
	return insert_line_files(nb_lines() - 1);
}

// -------- DATE --------

int Table::insert_line_date(const int pos, const bool after)
{
	return insert_line(pos, IT_DATE, after);
}

int Table::add_line_date()
{
	return insert_line_date(nb_lines() - 1);
}

bool Table::operator==(const Table& other) const
{
	return table_equal(static_cast<TBL>(*this), static_cast<TBL>(other));
}

// ================ LINES (PRIVATE) ================

TLINE* Table::get_line(const int row) const
{
	if (row < 0 || row > nb_lines())
	{
		IodeExceptionInvalidArguments error("Cannot get table line at position " + 
			std::to_string(row) + ".\nLine position must be in range [0, " + 
			std::to_string(nb_lines()) + "]");
		error.add_argument("line position", std::to_string(row));
		throw error;
	}

	return &t_line[row];
}

int Table::insert_line(const int pos, const EnumLineType line_type, const bool after)
{
	if (pos < 0 || pos > nb_lines())
	{
		IodeExceptionInvalidArguments error("Cannot insert table line",  
			"New line position must be in range [0, " + std::to_string(nb_lines() - 1) + "]");
		error.add_argument("new line position", std::to_string(pos));
		throw error;
	}

	int where_ = after ? 0 : 1;
	// WARNING: When inserting a new line of type IT_CELL, the attribute TCELL::tc_type of cells is undefined!
	int new_pos = T_insert_line(this, pos, line_type, where_);
	if (new_pos < 0) throw IodeExceptionFunction("Cannot insert table line at position " + std::to_string(pos), "Unknown");
	return new_pos;
}

void Table::delete_line(const int row)
{
	if(row >= nb_lines())
	{
		IodeExceptionInvalidArguments error("Cannot delete row at position " + std::to_string(row));
		error.add_argument("position of the row to delete: ", std::to_string(row));
		error.add_argument("number of rows: ", std::to_string(nb_lines()));
		throw error;
	}

	// pointer to all lines of the table
    TLINE* lines = t_line;

    /* free line at position row */
    TLINE* line = get_line(row);
	EnumLineType line_type = get_line_type(row);
	std::string content;
	if(line_type == IT_CELL)
		content = get_cell_content(row, 0, false);
    T_free_line(line, nb_columns());

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

void Table::free_line(const int row)
{
	T_free_line(get_line(row), nb_columns());
}

void Table::free_cell(const int row, const int column)
{
	T_free_cell(get_cell(row, column, false));
}

// ================ TABLE (PRIVATE) ================

void Table::free_all_lines()
{
    T_free_line(&t_div, nb_columns());

    for(int i = 0; i < t_nl; i++)
		free_line(i);
    SW_nfree(t_line);
}

// ================ CELLS (PRIVATE) ================

TCELL* Table::get_cell(const int row, const int column, const bool divider) const
{
	const TLINE* line = divider ? &t_div : get_line(row);

	if (column < 0 || column > nb_columns())
	{
		IodeExceptionInvalidArguments error("Cannot get table cell at position (" +
			std::to_string(row), + ", " + std::to_string(column) + ").\n" +   
			"Cell column must be in range [0, " + std::to_string(nb_columns()) + "]");
		error.add_argument("cell position", std::to_string(row) + ", " + std::to_string(column));
		throw error;
	}

	EnumLineType line_type = (EnumLineType) line->tl_type;
	if(line_type != IT_CELL && line_type != IT_TITLE)
	{
		IodeExceptionInvalidArguments error("Cannot get table cell or title at position (" +
			std::to_string(row), + ", " + std::to_string(column) + ").\n" + 
			"Expected line of type Cell or Title but got line of type " + get_line_type_as_string(line_type) + ".");
		error.add_argument("cell position", std::to_string(row) + ", " + std::to_string(column));
		throw error;
	}

	if(line_type == IT_TITLE && column > 0)
	{
		IodeExceptionInvalidArguments error("Line at position " + std::to_string(row) + 
			" is of type Title.\nDid you expect line of type Cell?");
		error.add_argument("cell position", std::to_string(row) + ", " + std::to_string(column));
		throw error;
	}

	TCELL* cell = &reinterpret_cast<TCELL*>(line->tl_val)[column];
	return cell;
}

// The table cell contains a "packed" IDT object (lec + clec) 
// -> see T_set_lec_cell from k_tbl.c
CLEC* Table::get_cell_compiled_lec(const int row, const int column)
{
	TCELL* cell = get_cell(row, column, false);

	if(cell->tc_type != IT_LEC)
	{
		std::string msg = "The table cell at position ";
		msg += "(" + std::to_string(row) + ", " + std::to_string(column) + ") ";
		msg += "does not contain a LEC expression";
		throw IodeException(msg);
	}

	if(cell->tc_val == NULL)
	{
		std::string msg = "Cannot extract content of the table cell at position ";
		msg += "(" + std::to_string(row) + ", " + std::to_string(column) + ")";
		throw IodeException(msg);
	}

	// see VT_edit() from o_vt.c from the old GUI
	return (CLEC*) P_get_ptr(cell->tc_val, 1);
}

std::string Table::get_cell_content(const int row, const int column, const bool quotes, const bool divider) const
{
	TCELL* cell = get_cell(row, column, divider);
	int mode = quotes ? 1 : 0;
	std::string content_oem = std::string(T_cell_cont(cell, mode));
	std::string content = (cell->tc_type == IT_STRING) ? oem_to_utf8(content_oem) : content_oem;
	return content;
}

// 
/**
 * @brief 
 * 
 * @param row 
 * @param column 
 * @param text text to be written in the cell. Note that leading and trailing double quotes are removed.
 * @param divider 
 * 
 * @note: The string argument `text` passed to this C++ method is assumed to be written with the UTF8 format
 */
void Table::set_cell_text(const int row, const int column, const std::string& text, const bool divider)
{
	TCELL* cell = get_cell(row, column, divider);
	std::string text_oem = utf8_to_oem(text);
	unsigned char* c_text = reinterpret_cast<unsigned char*>(to_char_array(text_oem));
	T_set_string_cell(cell, c_text);
}

void Table::set_cell_lec(const int row, const int column, const std::string& lec, const bool divider)
{
	TCELL* cell = get_cell(row, column, divider);
	unsigned char* c_lec = reinterpret_cast<unsigned char*>(to_char_array(lec));
	T_set_lec_cell(cell, c_lec);
}

/**
 * @brief Set the content of a cell.
 *        Rule: If the content starts with a double quotes, we assume it is a string cell. 
 *        Otherwise, it is a LEC cell.
 *        Note that the leading and trailing double quotes are removed when the text is written 
 *        in a string table cell.
 * 
 * @param row 
 * @param column 
 * @param content 
 * @param divider
 * 
 * @note When inserting a new line of type IT_CELL, the attribute TCELL::tc_type of cells is undefined!
 *       See function `T_create_cell()` in file `k_tbl.c` from the C API 
 */
void Table::set_cell_content(const int row, const int column, const std::string& content, const bool divider)
{
	if(content.starts_with('\"'))
		set_cell_text(row, column, content, divider);
	else
		set_cell_lec(row, column, content, divider);
}

EnumCellType Table::get_cell_type(const int row, const int column, const bool divider) const
{
	TCELL* cell = get_cell(row, column, divider);
	return static_cast<EnumCellType>(cell->tc_type);
}

void Table::set_cell_type(const int row, const int column, const EnumCellType cell_type, const bool divider)
{
	TCELL* cell = get_cell(row, column, divider);
	cell->tc_type = cell_type;
}

// TODO: check if it is correct
EnumCellAttribute Table::get_cell_align(const int row, const int column, const bool divider) const
{
	TCELL* cell = get_cell(row, column, divider);
	return static_cast<EnumCellAttribute>(((int) cell->tc_attr / 8) * 8);
}

int Table::get_cell_font(const int row, const int column, const bool divider) const
{
	TCELL* cell = get_cell(row, column, divider);
	return ((int) cell->tc_attr % 8);
}

bool Table::is_cell_bold_font(const int row, const int column, const bool divider) const
{
	int font = get_cell_font(row, column, divider);
	return int_bitset(font).test(0);
}

bool Table::is_cell_italic_font(const int row, const int column, const bool divider) const
{
	int font = get_cell_font(row, column, divider);
	return int_bitset(font).test(1);
}

bool Table::is_cell_underline_font(const int row, const int column, const bool divider) const
{
	int font = get_cell_font(row, column, divider);
	return int_bitset(font).test(2);
}

void Table::set_cell_attribute(const int row, const int column, const char attr, const bool divider)
{
	// check cell exist at position (row, column)
	get_cell(row, column, divider);
	T_set_cell_attr(this, row, column, attr);
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
