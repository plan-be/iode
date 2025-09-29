#pragma once

#include "cpp_api/common.h"
#include "api/utils/utils.h"
#include <stdexcept>


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

	// -------- LINES --------

	TableLine* append_line(const TableLineType line_type);

	TableLine* insert_line(const int pos, const TableLineType line_type, const bool after = true);

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

	bool operator==(const Table& other) const
	{
		return TBL::operator==((TBL&) other);
	}
};
