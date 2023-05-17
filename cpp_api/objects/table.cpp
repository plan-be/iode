#pragma once

#include "table.h"


static void copy_cell(TCELL* c_cell_out, TCELL* c_cell_in)
{
	unsigned char* cell_in_content = (unsigned char*)T_cell_cont(c_cell_in, 0);
	if (c_cell_in->tc_type == IT_LEC) T_set_lec_cell(c_cell_out, cell_in_content);
	else T_set_string_cell(c_cell_out, cell_in_content);
	c_cell_out->tc_attr = c_cell_in->tc_attr;
}

static void copy_line(TBL* c_table, TLINE* c_line_out, TLINE* c_line_in)
{
	TCELL* cells_out = (TCELL*)c_line_out->tl_val;
	TCELL* cells_in = (TCELL*)c_line_in->tl_val;
	switch (c_line_in->tl_type)
	{
	case IT_TITLE:
		T_set_string_cell(cells_out, (unsigned char*) cells_in->tc_val);
		break;
	case IT_CELL:
		for (int col = 0; col < c_table->t_nc; col++) copy_cell(&cells_out[col], &cells_in[col]);
		break;
	default:
		break;
	}
}

TBL* create_table_deep_copy(TBL* original_table)
{
	TBL* copy_table = T_create(original_table->t_nc);

	copy_table->t_lang = original_table->t_lang;
	copy_table->t_free = original_table->t_free;
	copy_table->t_nl = 0;
	copy_table->t_nc = original_table->t_nc;
	copy_line(copy_table, &copy_table->t_div, &original_table->t_div);

	TLINE* other_line;
	for (int i = 0; i < original_table->t_nl; i++)
	{
		other_line = &original_table->t_line[i];
		switch (other_line->tl_type)
		{
		case IT_TITLE:
			T_insert_line(copy_table, copy_table->t_nl - 1, IT_TITLE, 0);
			copy_line(copy_table, &copy_table->t_line[i], &original_table->t_line[i]);
			break;
		case IT_CELL:
			T_insert_line(copy_table, copy_table->t_nl - 1, IT_CELL, 0);
			copy_line(copy_table, &copy_table->t_line[i], &original_table->t_line[i]);
			break;
		case IT_LINE:
			T_insert_line(copy_table, copy_table->t_nl - 1, IT_LINE, 0);
			break;
		case IT_MODE:
			T_insert_line(copy_table, copy_table->t_nl - 1, IT_MODE, 0);
			break;
		case IT_FILES:
			T_insert_line(copy_table, copy_table->t_nl - 1, IT_FILES, 0);
			break;
		case IT_DATE:
			T_insert_line(copy_table, copy_table->t_nl - 1, IT_DATE, 0);
			break;
		default:
			break;
		}
	}

	copy_table->t_zmin = original_table->t_zmin;
	copy_table->t_zmax = original_table->t_zmax;
	copy_table->t_ymin = original_table->t_ymin;
	copy_table->t_ymax = original_table->t_ymax;
	copy_table->t_attr = original_table->t_attr;
	copy_table->t_box = original_table->t_box;
	copy_table->t_shadow = original_table->t_shadow;
	copy_table->t_gridx = original_table->t_gridx;
	copy_table->t_gridy = original_table->t_gridy;
	copy_table->t_axis = original_table->t_axis;
	copy_table->t_align = original_table->t_align;

	return copy_table;
}


static bool cell_equal(TCELL* c_cell1, TCELL* c_cell2)
{
	if (c_cell1->tc_type != c_cell2->tc_type) return false;
	if (c_cell1->tc_attr != c_cell2->tc_attr) return false;
	if (strcmp(T_cell_cont(c_cell1, 0), T_cell_cont(c_cell2, 0)) != 0) return false;

	return true;
}

static bool line_equal(int nb_columns, TLINE* c_line1, TLINE* c_line2)
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

bool table_equal(TBL* c_table1, TBL* c_table2)
{
	if(c_table1->t_lang != c_table2->t_lang) return false;
	if(c_table1->t_free != c_table2->t_free) return false;
	if(c_table1->t_nl != c_table2->t_nl) return false;
	if(c_table1->t_nc != c_table2->t_nc) return false;

	if (!line_equal(c_table1->t_nc, &c_table1->t_div, &c_table2->t_div)) return false;
	for (int i = 0; i < c_table1->t_nl; i++)
		if (!line_equal(c_table1->t_nc, &c_table1->t_line[i], &c_table2->t_line[i])) return false;

	if(c_table1->t_zmin != c_table2->t_zmin) return false;
	if(c_table1->t_zmax != c_table2->t_zmax) return false;
	if(c_table1->t_ymin != c_table2->t_ymin) return false;
	if(c_table1->t_ymax != c_table2->t_ymax) return false;
	if(c_table1->t_attr != c_table2->t_attr) return false;
	if(c_table1->t_box != c_table2->t_box) return false;
	if(c_table1->t_shadow != c_table2->t_shadow) return false;
	if(c_table1->t_gridx != c_table2->t_gridx) return false;
	if(c_table1->t_gridy != c_table2->t_gridy) return false;
	if(c_table1->t_axis != c_table2->t_axis) return false;
	if(c_table1->t_align != c_table2->t_align) return false;

	return true;
}


Table::Table()
{
	c_table = nullptr;
}

Table::Table(const int pos, KDB* kdb)
{
	if (!kdb) kdb = K_WS[I_TABLES];
	if (pos < 0 || pos > kdb->k_nb)
	{
		IodeExceptionInvalidArguments error("Cannot extract Table");
		error.add_argument("table position", std::to_string(pos) + 
			               " (table position must be in range [0, " + std::to_string(kdb->k_nb - 1) + "])");
		throw error;
	}
	// Note: KTVAL allocate a new pointer TBL*
	c_table = KTVAL(kdb, pos);
}

Table::Table(const std::string& name, KDB* kdb)
{
	if (!kdb) kdb = K_WS[I_TABLES];
	int pos = K_find(kdb, to_char_array(name));
	if (pos < 0) throw IodeExceptionFunction("Cannot extract Table", "Table with name " + name + " does not exist");
	// Note: KTVAL allocate a new pointer TBL*
	c_table = KTVAL(kdb, pos);
}

Table::Table(const Table& table)
{
	c_table = create_table_deep_copy(table.c_table);
}

Table::~Table()
{
	if(c_table) T_free(c_table);
}

// required to be used in std::map
Table& Table::operator=(const Table& table)
{
    this->c_table = create_table_deep_copy(table.c_table);
    return *this;
}

// ================ TABLE ================

void Table::extend()
{
	int res = T_add_line(c_table);
	if (res < 0) throw IodeExceptionFunction("Cannot extend table \"" + 
		std::string((char*)T_get_title(c_table)) + "\"", "Unknown");
}

std::string Table::getLanguage() const
{
	return vLangs.at(c_table->t_lang - KT_ENGLISH);
}

void Table::setLanguage(const EnumLang lang)
{
	c_table->t_lang = lang + KT_ENGLISH;
}

short Table::nbColumns() const
{
	return c_table->t_nc;
}

short Table::nbLines() const
{
	return c_table->t_nl;
}

float Table::getZMin() const
{
	return c_table->t_zmin;
}

void Table::setZMin(float zmin)
{
	c_table->t_zmin = zmin;
}

float Table::getZMax() const
{
	return c_table->t_zmax;
}

void Table::setZMax(float zmax)
{
	c_table->t_zmax = zmax;
}

float Table::getYMin() const
{
	return c_table->t_ymin;
}

void Table::setYMin(float ymin)
{
	c_table->t_ymin = ymin;
}

float Table::getYMax() const
{
	return c_table->t_ymax;
}

void Table::setYMax(float ymax)
{
	c_table->t_ymax = ymax;
}

char Table::getAttribute() const
{
	return c_table->t_attr;
}

void Table::setAttribute(const char attribute)
{
	c_table->t_attr = attribute;
}

char Table::getGraphBox() const
{
	return c_table->t_box;
}

void Table::setGraphBox(const char box) const
{
	c_table->t_box = box;
}

char Table::getGraphShadow() const
{
	return c_table->t_shadow;
}

void Table::setGraphShadow(const char shadow) const
{
	c_table->t_shadow = shadow;
}

EnumGraphGrid Table::getGridX() const
{
	return static_cast<EnumGraphGrid>(c_table->t_gridx);
}

void Table::setGridX(const EnumGraphGrid gridx)
{
	c_table->t_gridx = gridx - IG_MAJOR;
}

EnumGraphGrid Table::getGridY() const
{
	return static_cast<EnumGraphGrid>(c_table->t_gridy);
}

void Table::setGridY(const EnumGraphGrid gridy)
{
	c_table->t_gridy = gridy;
}

EnumGraphAxis Table::getGraphAxis() const
{
	return static_cast<EnumGraphAxis>(c_table->t_axis);
}

void Table::setGraphAxis(const EnumGraphAxis axis)
{
	c_table->t_axis = axis;
}

EnumGraphAlign Table::getGraphAlignement() const
{
	return static_cast<EnumGraphAlign>(c_table->t_align);
}

void Table::setGraphAlignement(const EnumGraphAlign align)
{
	c_table->t_align = align;
}

// ================ LINES ================

EnumLineType Table::getLineType(const int row) const
{
	TLINE* line = getLine(row);
	return static_cast<EnumLineType>(line->tl_type);
}

void Table::setLineType(const int row, const EnumLineType line_type)
{
	TLINE* line = getLine(row);
	line->tl_type = line_type;
}

EnumGraphType Table::getLineGraph(const int row) const
{
	TLINE* line = getLine(row);
	return static_cast<EnumGraphType>(line->tl_graph);
}

void Table::setLineGraph(const int row, const EnumGraphType graph_type)
{
	TLINE* line = getLine(row);
	line->tl_graph = graph_type;
}

bool Table::isLeftAxis(const int row) const
{
	TLINE* line = getLine(row);
	return line->tl_axis == 0;
}

void Table::setLineAxis(const int row, const bool is_left)
{
	TLINE* line = getLine(row);
	line->tl_axis = is_left ? 0 : 1;
}

unsigned char Table::getLinePByte(const int row) const
{
	TLINE* line = getLine(row);
	return line->tl_pbyte;
}

void Table::setLinePByte(const int row, const unsigned char pbyte)
{
	TLINE* line = getLine(row);
	line->tl_pbyte = pbyte;
}

// -------- DIVIDER --------

EnumCellType Table::getDividerCellType(const int column) const
{
	return getCellType(0, column, true);
}

EnumCellAttribute Table::getDividerCellAlign(const int column) const
{
	return getCellAlign(0, column, true);
}

int Table::getDividerCellFont(const int column) const
{
	return getCellFont(0, column, true);
}

std::string Table::getDividerCellContent(const int column, const bool quotes) const
{
	return getCellContent(0, column, quotes, true);
}

void Table::setCellDividerText(const int column, const std::string& text)
{
	setCellText(0, column, text, true);
}

void Table::setCellDividerLec(const int column, const std::string& lec)
{
	setCellLec(0, column, lec, true);
}

// -------- TITLE --------

// we assume that title string is written in UTF8 format
int Table::insertTitle(const int pos, const std::string& title, const bool after)
{
	int title_pos = insertLine(pos, IT_TITLE, after);
	std::string title_oem = utf8_to_oem(title);
	c_table->t_line[title_pos].tl_val = to_char_array(title_oem);
	return title_pos;
}

int Table::addTitle(const std::string& title)
{
	return insertTitle(nbLines() - 1, title);
}

std::string Table::getTitle(const int row) const
{
	TLINE* title_line = getLine(row);
	if (title_line->tl_type != IT_TITLE) throw IodeExceptionFunction("Cannot get table title", 
		"Line at position " + std::to_string(row) + " is not a TITLE line.");
	std::string title = getCellContent(row, 0, false);
	return title;
}

// we assume that title string is written in UTF8 format
void Table::setTitle(const int row, const std::string title)
{
	TLINE* title_line = getLine(row);
	if (title_line->tl_type != IT_TITLE) throw IodeExceptionFunction("Cannot set table title", 
		"Line at position " + std::to_string(row) + " is not a TITLE line.");
	setCellText(row, 0, title);
}

// -------- CELLS --------

int Table::insertLineWithCells(const int pos, const bool after)
{
	int line_cells_pos = insertLine(pos, IT_CELL, after);
	return line_cells_pos;
}

int Table::addLineWithCells()
{
	return insertLineWithCells(nbLines() - 1);
}

std::string Table::getCellContent(const int row, const int column, const bool quotes, const bool divider) const
{
	TCELL* cell = getCell(row, column, divider);
	int mode = quotes ? 1 : 0;
	std::string content_oem = std::string(T_cell_cont(cell, mode));
	std::string content = (cell->tc_type == IT_STRING) ? oem_to_utf8(content_oem) : content_oem;
	return content;
}

// we assume that text string is written in UTF8 format
void Table::setCellText(const int row, const int column, const std::string& text, const bool divider)
{
	TCELL* cell = getCell(row, column, divider);
	std::string text_oem = utf8_to_oem(text);
	unsigned char* c_text = reinterpret_cast<unsigned char*>(to_char_array(text_oem));
	T_set_string_cell(cell, c_text);
}

void Table::setCellLec(const int row, const int column, const std::string& lec, const bool divider)
{
	TCELL* cell = getCell(row, column, divider);
	unsigned char* c_lec = reinterpret_cast<unsigned char*>(to_char_array(lec));
	T_set_lec_cell(cell, c_lec);
}

EnumCellType Table::getCellType(const int row, const int column, const bool divider) const
{
	TCELL* cell = getCell(row, column, divider);
	return static_cast<EnumCellType>(cell->tc_type);
}

void Table::setCellType(const int row, const int column, const EnumCellType cell_type, const bool divider)
{
	TCELL* cell = getCell(row, column, divider);
	cell->tc_type = cell_type;
}

// TODO: check if it is correct
EnumCellAttribute Table::getCellAlign(const int row, const int column, const bool divider) const
{
	TCELL* cell = getCell(row, column, divider);
	return static_cast<EnumCellAttribute>(((int) cell->tc_attr / 8) * 8);
}

int Table::getCellFont(const int row, const int column, const bool divider) const
{
	TCELL* cell = getCell(row, column, divider);
	return ((int) cell->tc_attr % 8);
}

bool Table::isCellBoldFont(const int row, const int column, const bool divider) const
{
	int font = getCellFont(row, column, divider);
	return int_bitset(font).test(0);
}

bool Table::isCellItalicFont(const int row, const int column, const bool divider) const
{
	int font = getCellFont(row, column, divider);
	return int_bitset(font).test(1);
}

bool Table::isCellUnderlineFont(const int row, const int column, const bool divider) const
{
	int font = getCellFont(row, column, divider);
	return int_bitset(font).test(2);
}

void Table::setCellAttribute(const int row, const int column, const char attr, const bool divider)
{
	// check cell exist at position (row, column)
	getCell(row, column, divider);
	T_set_cell_attr(c_table, row, column, attr);
}

// -------- SEPARATOR --------

int Table::insertLineSeparator(const int pos, const bool after)
{
	return insertLine(pos, IT_LINE, after);
}

int Table::addLineSeparator()
{
	return insertLineSeparator(nbLines() - 1);
}

// -------- MODE --------

int Table::insertLineMode(const int pos, const bool after)
{
	return insertLine(pos, IT_MODE, after);
}

int Table::addLineMode()
{
	return insertLineMode(nbLines() - 1);
}

// -------- FILES --------

int Table::insertLineFiles(const int pos, const bool after)
{
	return insertLine(pos, IT_FILES, after);
}

int Table::addLineFiles()
{
	return insertLineFiles(nbLines() - 1);
}

// -------- DATE --------

int Table::insertLineDate(const int pos, const bool after)
{
	return insertLine(pos, IT_DATE, after);
}

int Table::addLineDate()
{
	return insertLineDate(nbLines() - 1);
}

bool Table::operator==(const Table& other) const
{
	return table_equal(c_table, other.c_table);
}

// ================ LINES (PRIVATE) ================

TLINE* Table::getLine(const int row) const
{
	if (row < 0 || row > nbLines())
	{
		IodeExceptionInvalidArguments error("Cannot get table line", 
			"Line position must be in range [0, " + std::to_string(nbLines()) + "]");
		error.add_argument("line position", std::to_string(row));
		throw error;
	}	
	return &c_table->t_line[row];
}

int Table::insertLine(const int pos, const EnumLineType line_type, const bool after)
{
	if (pos < 0 || pos > nbLines())
	{
		IodeExceptionInvalidArguments error("Cannot insert table line",  
			"New line position must be in range [0, " + std::to_string(nbLines() - 1) + "]");
		error.add_argument("new line position", std::to_string(pos));
		throw error;
	}
	int where_ = after ? 1 : 0;
	int new_pos = T_insert_line(c_table, pos, line_type, where_);
	if (new_pos < 0) throw IodeExceptionFunction("Cannot insert table line at position " + std::to_string(pos), "Unknown");
	return new_pos;
}

void Table::freeLine(const int row)
{
	T_free_line(getLine(row), nbColumns());
}

// ================ CELLS (PRIVATE) ================

TCELL* Table::getCell(const int row, const int column, const bool divider) const
{
	TLINE* line = divider ? &c_table->t_div : getLine(row);
	if (column < 0 || column > nbColumns())
	{
		IodeExceptionInvalidArguments error("Cannot get table cell content",  
			"Cell position must be in range [0, " + std::to_string(nbColumns()) + "]");
		error.add_argument("cell position", std::to_string(column));
		throw error;
	}
	TCELL* cell = &reinterpret_cast<TCELL*>(line->tl_val)[column];
	return cell;
}

void Table::freeCell(const int row, const int column)
{
	T_free_cell(getCell(row, column));
}
