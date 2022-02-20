#pragma once
#include "kdb_tables.h"


Table KDBTables::get(const int pos) const
{
    TBL* c_table = KTVAL(getKDB(), pos);
    return Table(c_table);
}

Table KDBTables::get(const std::string name) const
{
    int pos = getPosition(name);
    return get(pos);
}

std::string KDBTables::getTitle(const int pos) const
{
    TBL* c_table = KTVAL(getKDB(), pos);
    std::string title_oem = std::string((char*)T_get_title(c_table));
    std::string title = convert_oem_to_utf8(title_oem);
    T_free(c_table);
    return title;
}

std::string KDBTables::getTitle(const std::string name) const
{
    int pos = getPosition(name);
    return getTitle(pos);
}

Table KDBTables::add_table(const std::string name, const int nbColumns)
{
	char* c_name = const_cast<char*>(name.c_str());
	TBL* c_table = T_create(nbColumns);

	int pos = K_add(getKDB(), c_name, c_table);
	if (pos == -2) throw std::runtime_error("Cannot create or update table with name " + name);

	return Table(c_table);
}

Table KDBTables::add_table(const std::string name, const int nbColumns, const std::string def, std::vector<std::string> vars, bool mode, bool files, bool date)
{
	char* c_name = const_cast<char*>(name.c_str());
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

	int pos = K_add(getKDB(), c_name, c_table);
	if (pos == -2) throw std::runtime_error("Cannot create or update table with name " + name);

	return Table(c_table);
}

Table KDBTables::copy(const std::string name, const std::string original_table_name)
{
	Table original_table = get(original_table_name);
	TBL* c_copy_table = create_table_deep_copy(original_table.c_table);

	char* c_name = const_cast<char*>(name.c_str());
	int pos = K_add(getKDB(), c_name, c_copy_table);
	if (pos == -2) throw std::runtime_error("Cannot create or update table with name " + name);

	return Table(c_copy_table);
}

void KDBTables::remove_table(const int pos)
{
	// test if table exists in K_WS
	getName(pos);
	K_del(getKDB(), pos);
}

void KDBTables::remove_table(const std::string name)
{
	// test if table exists in K_WS
	int pos = getPosition(name);
	remove_table(pos);
}