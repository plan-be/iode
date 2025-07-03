#pragma once
#include "kdb_tables.h"


Table* KDBTables::copy_obj(Table* const original) const
{
	return new Table(*original);
}

Table* KDBTables::get_unchecked(const int pos) const
{
	KDB* kdb = get_database();
	// Note: - KTVAL allocate a new pointer TBL*
	//       - static_cast<Table*>(TBL*) calls the copy constructor Table(const TBL* c_table)
	//         which calls copy_from_TBL_obj()
	return static_cast<Table*>(KTVAL(kdb, pos));
}

std::string KDBTables::get_title(const int pos) const
{
	// throw exception if table with passed position is not valid
	get_name(pos);
    TBL* c_table = KTVAL(get_database(), pos);
    std::string title_oem = std::string((char*) T_get_title(c_table));
	std::string title = oem_to_utf8(title_oem);
    T_free(c_table);
    return title;
}

std::string KDBTables::get_title(const std::string& name) const
{
	// throw exception if table with passed name does not exist
    int pos = get_position(name);
    return get_title(pos);
}

int KDBTables::add(const std::string& name, const Table& obj)
{
	Table table(obj);
	return KDBTemplate::add(name, static_cast<TBL*>(&table));
}

int KDBTables::add(const std::string& name, const int nbColumns)
{
	Table table(nbColumns);
	return KDBTemplate::add(name, static_cast<TBL*>(&table));
}

int KDBTables::add(const std::string& name, const int nbColumns, const std::string& def, 
	const std::vector<std::string>& vars, bool mode, bool files, bool date)
{
	Table table(nbColumns, def, vars, mode, files, date);
	return KDBTemplate::add(name, static_cast<TBL*>(&table));
}

int KDBTables::add(const std::string& name, const int nbColumns, const std::string& def, 
	const std::vector<std::string>& titles, const std::vector<std::string>& lecs, 
	bool mode, bool files, bool date)
{
	Table table(nbColumns, def, titles, lecs, mode, files, date);
	return KDBTemplate::add(name, static_cast<TBL*>(&table));
}

int KDBTables::add(const std::string& name, const int nbColumns, const std::string& def, 
	const std::string& lecs, bool mode, bool files, bool date)
{
	Table table(nbColumns, def, lecs, mode, files, date);
	return KDBTemplate::add(name, static_cast<TBL*>(&table));
}

void KDBTables::print_to_file(const std::string& destination_file, const std::string& gsample, 
	const std::string& names, const int nb_decimals, const char format)
{
	ComputedTable::initialize_printing(destination_file, format);

	Table* table;
	ComputedTable* computed_table;
	std::vector<std::string> v_names = get_names(names);
	for(const std::string& name : v_names)
	{
		table = get(name);
		computed_table = new ComputedTable(table, gsample, nb_decimals);
		computed_table->print_to_file();
		delete computed_table;
		delete table;
	}

	ComputedTable::finalize_printing();
}