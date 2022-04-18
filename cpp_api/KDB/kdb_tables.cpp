#pragma once
#include "kdb_tables.h"


void KDBTables::add_or_update(const std::string& name, const Table& table)
{
	char* c_name = const_cast<char*>(name.c_str());
	int pos = K_add(get_KDB(), c_name, table.c_table);
	if (pos == -1) throw std::runtime_error("Iode has not been initialized");
	if (pos == -2) throw std::runtime_error("Cannot create or update table with name " + name);
}

Table KDBTables::copy_obj(const Table& original) const
{
	return Table(original);
}

Table KDBTables::get_unchecked(const int pos) const
{
    return Table(pos);
}

std::string KDBTables::get_title(const int pos) const
{
	// throw exception if table with passed position is not valid
	get_name(pos);
    TBL* c_table = KTVAL(get_KDB(), pos);
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

Table KDBTables::add(const std::string& name, const int nbColumns)
{
	// throw exception if object with passed name already exist
	char* c_name = const_cast<char*>(name.c_str());
	if (K_find(get_KDB(), c_name) >= 0) throw std::runtime_error(type_name + " with name " + name + " already exists. Use update() method instead.");

	TBL* c_table = T_create(nbColumns);

	int pos = K_add(get_KDB(), c_name, c_table);
	if (pos == -1) throw std::runtime_error("Iode has not been initialized");
	if (pos == -2) throw std::runtime_error("Cannot create or update table with name " + name);

	return Table(name);
}

Table KDBTables::add(const std::string& name, const int nbColumns, const std::string& def, std::vector<std::string>& vars, bool mode, bool files, bool date)
{
	// throw exception if object with passed name already exist
	char* c_name = const_cast<char*>(name.c_str());
	if (K_find(get_KDB(), c_name) >= 0) throw std::runtime_error(type_name + " with name " + name + " already exists. Use update() method instead.");

	char* c_def = const_cast<char*>(def.c_str());
	char** c_vars = new char* [vars.size() + 1];
	for (int i = 0; i < vars.size(); ++i) c_vars[i] = const_cast<char*>(vars[i].c_str());
	c_vars[vars.size()] = '\0';
	int c_mode = mode ? 1 : 0;
	int c_files = files ? 1 : 0;
	int c_date = date ? 1 : 0;

	TBL* c_table = T_create(nbColumns);
	T_auto(c_table, c_def, c_vars, c_mode, c_files, c_date);

	delete[] c_vars;

	int pos = K_add(get_KDB(), c_name, c_table);
	if (pos == -1) throw std::runtime_error("Iode has not been initialized");
	if (pos == -2) throw std::runtime_error("Cannot create or update table with name " + name);

	return Table(name);
}
