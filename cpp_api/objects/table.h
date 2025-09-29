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

	// -------- EQUAL --------

	bool operator==(const Table& other) const
	{
		return TBL::operator==((TBL&) other);
	}
};
