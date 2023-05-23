#pragma once

#include "common.h"
#include "utils/utils.h"
#include "utils/iode_exceptions.h"
#include <boost/functional/hash.hpp>


enum EnumCellType
{
	IT_LEC = KT_LEC,				// 114
	IT_STRING = KT_STRING,			// 115
};

enum EnumCellAttribute
{
	IT_NORMAL = KT_NORMAL,			// 0
	IT_BOLD = KT_BOLD,				// 1
	IT_ITALIC = KT_ITALIC,			// 2
	IT_UNDERLINE = KT_UNDERLINE,	// 4
	// -----------------------------------
	IT_CENTER = KT_CENTER,			// 8
	IT_LEFT = KT_LEFT,				// 16
	IT_RIGHT = KT_RIGHT,			// 32
	IT_DECIMAL = KT_DECIMAL			// 64
};

enum EnumLineType
{
	IT_FILES = KT_FILES,			// 105
	IT_MODE = KT_MODE,				// 106
	IT_CELL = KT_CELL,				// 110
	IT_LINE = KT_LINE,				// 111
	IT_TITLE = KT_TITLE,			// 113
	IT_DATE = KT_DATE				// 116
};

const static std::map<std::string, EnumLineType> mLineType =
{
	{"Cells", IT_CELL},
	{"Line",  IT_LINE},
	{"Title", IT_TITLE},
	{"Files", IT_FILES},
	{"Mode",  IT_MODE},
	{"Date",  IT_DATE}
};

inline std::string get_line_type(EnumLineType line_type)
{
	for(const auto& [key, value]: mLineType)
		if(line_type == value) return key;
	return "";
}

enum EnumGraphType
{
	IG_LINE,
	IG_SCATTER,
	IG_BAR
};

enum EnumGraphGrid
{
	IG_MAJOR,
	IG_NONE,
	IG_MINOR
};

enum EnumGraphAlign
{
	IG_LEFT,
	IG_CENTER,
	IG_RIGHT
};

enum EnumGraphAxis
{
	IG_VALUES,
	IG_LOG,
	IG_SEMILOG,
	IG_PERCENT
};


void copy_cell(TCELL* c_cell_out, TCELL* c_cell_in);
void copy_line(TBL* c_table, TLINE* c_line_out, TLINE* c_line_in);
TBL* create_table_deep_copy(TBL* original_table);

bool cell_equal(TCELL* c_cell1, TCELL* c_cell2);
bool line_equal(int nb_columns, TLINE* c_line1, TLINE* c_line2);
bool table_equal(TBL* c_table1, TBL* c_table2);

/**
 * @brief compute a hash value for a table line.
 * 
 * @note see https://www.boost.org/doc/libs/1_55_0/doc/html/hash/custom.html
 *       and https://www.boost.org/doc/libs/1_55_0/doc/html/hash/combine.html
 * 
 * @return std::size_t 
 */
std::size_t hash_value(TLINE const& c_line);

/**
 * @brief compute a hash value for a table.
 * 
 * @note see https://www.boost.org/doc/libs/1_55_0/doc/html/hash/custom.html
 *       and https://www.boost.org/doc/libs/1_55_0/doc/html/hash/combine.html
 * 
 * @return std::size_t 
 */
std::size_t hash_value(TBL const& c_table);


struct Table
{
	TBL* c_table;

public:
	Table();

	Table(const int pos, KDB* kdb = nullptr);

	Table(const std::string& name, KDB* kdb = nullptr);

	Table(const Table& table);

	~Table();

	// required to be used in std::map
    Table& operator=(const Table& table);

	// ================ TABLE ================

	void extend();

	std::string getLanguage() const;

	void setLanguage(const EnumLang lang);

	short nbColumns() const;

	short nbLines() const;

	float getZMin() const;

	void setZMin(float zmin);

	float getZMax() const;

	void setZMax(float zmax);

	float getYMin() const;

	void setYMin(float ymin);

	float getYMax() const;

	void setYMax(float ymax);

	// TODO : returned value should be an enum
	char getAttribute() const;

	void setAttribute(const char attribute);

	// TODO : returned value should be an enum
	char getGraphBox() const;

	// TODO : passed value should be an enum
	void setGraphBox(const char box) const;

	// TODO : returned value should be an enum
	char getGraphShadow() const;

	// TODO : passed value should be an enum
	void setGraphShadow(const char shadow) const;

	EnumGraphGrid getGridX() const;

	void setGridX(const EnumGraphGrid gridx);

	EnumGraphGrid getGridY() const;

	void setGridY(const EnumGraphGrid gridy);

	EnumGraphAxis getGraphAxis() const;

	void setGraphAxis(const EnumGraphAxis axis);

	EnumGraphAlign getGraphAlignement() const;

	void setGraphAlignement(const EnumGraphAlign align);

	// ================ LINES ================

	EnumLineType getLineType(const int row) const;

	void setLineType(const int row, const EnumLineType line_type);

	EnumGraphType getLineGraph(const int row) const;

	void setLineGraph(const int row, const EnumGraphType graph_type);

	bool isLeftAxis(const int row) const;

	void setLineAxis(const int row, const bool is_left);

	// TODO : returned value should be an enum
	unsigned char getLinePByte(const int row) const;

	// TODO : passed value should be an enum
	void setLinePByte(const int row, const unsigned char pbyte);

	// -------- DIVIDER --------

	EnumCellType getDividerCellType(const int column) const;

	EnumCellAttribute getDividerCellAlign(const int column) const;

	int getDividerCellFont(const int column) const;

	std::string getDividerCellContent(const int column, const bool quotes) const;

	// TODO: does it make sense to set a Divider cell as STRING
	void setCellDividerText(const int column, const std::string& text);

	void setCellDividerLec(const int column, const std::string& lec);

	// -------- TITLE --------

	int insertTitle(const int pos, const std::string& title, const bool after = true);

	int addTitle(const std::string& title);

	std::string getTitle(const int row) const;

	void setTitle(const int row, const std::string title);

	// -------- CELLS --------

	int insertLineWithCells(const int pos, const bool after = true);

	int addLineWithCells();

	std::string getCellContent(const int row, const int column, const bool quotes, const bool divider = false) const;

	void setCellText(const int row, const int column, const std::string& text, const bool divider = false);

	void setCellLec(const int row, const int column, const std::string& lec, const bool divider = false);

	EnumCellType getCellType(const int row, const int column, const bool divider = false) const;

	void setCellType(const int row, const int column, const EnumCellType cell_type, const bool divider = false);

	EnumCellAttribute getCellAlign(const int row, const int column, const bool divider = false) const;

	int getCellFont(const int row, const int column, const bool divider = false) const;

	bool isCellBoldFont(const int row, const int column, const bool divider = false) const;

	bool isCellItalicFont(const int row, const int column, const bool divider = false) const;

	bool isCellUnderlineFont(const int row, const int column, const bool divider = false) const;

	void setCellAttribute(const int row, const int column, const char attr, const bool divider = false);

	// -------- SEPARATOR --------

	int insertLineSeparator(const int pos, const bool after = true);

	int addLineSeparator();

	// -------- MODE --------

	int insertLineMode(const int pos, const bool after = true);

	int addLineMode();

	// -------- FILES --------

	int insertLineFiles(const int pos, const bool after = true);

	int addLineFiles();

	// -------- DATE --------

	int insertLineDate(const int pos, const bool after = true);

	int addLineDate();

	// -------- FREE --------

	/**
	 * @brief 
	 * 
	 * @param row 
	 * 
	 * @note see T_del_line() function from the source file o_tab.c from the old GUI.
	 */
	void deleteLine(const int row);

	void freeLine(const int row);

	void freeCell(const int row, const int column);

	// -------- EQUAL --------

	bool operator==(const Table& other) const;

private:
	// ================ LINES (PRIVATE) ================

	TLINE* getLine(const int row) const;

	int insertLine(const int pos, const EnumLineType line_type, const bool after = true);

	// ================ CELLS (PRIVATE) ================

	TCELL* getCell(const int row, const int column, const bool divider = false) const;

};
