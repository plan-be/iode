#pragma once
#include "kdb_tables.h"


Table KDBTables::copy_obj(const Table& original) const
{
	return Table(original);
}

Table KDBTables::get_unchecked(const int pos) const
{
    return Table(pos, get_KDB());
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

int KDBTables::add(const std::string& name, const int nbColumns)
{
	// throw exception if object with passed name already exist
	char* c_name = const_cast<char*>(name.c_str());
	if (K_find(get_KDB(), c_name) >= 0) throw std::runtime_error(iode_type_name + " with name " + name + " already exists. Use update() method instead.");

	TBL* c_table = T_create(nbColumns);

	return KDBTemplate::add(name, c_table);
}

int KDBTables::add(const std::string& name, const int nbColumns, const std::string& def, std::vector<std::string>& vars, bool mode, bool files, bool date)
{
	std::string vars_list;
	for (std::string& var : vars) vars_list += var + ';';
	vars_list += '\n';

	return add(name, nbColumns, def, vars_list, mode, files, date);
}

int KDBTables::add(const std::string& name, const int nbColumns, const std::string& def, const std::string& lecs, bool mode, bool files, bool date)
{
	// throw exception if object with passed name already exist
	char* c_name = const_cast<char*>(name.c_str());
	if (K_find(get_KDB(), c_name) >= 0) throw std::runtime_error(iode_type_name + " with name " + name + " already exists. Use update() method instead.");

	char* c_def = const_cast<char*>(def.c_str());

	char* pattern = const_cast<char*>(lecs.c_str());

	// Retrieves all variable names matching one or more patterns in KV_WS (similar to grep)
	char* lst = K_expand(I_VARIABLES, NULL, pattern, '*');
	// Parses a string and replaces @filename and $listname by their contents
	char** c_lecs = B_ainit_chk(lst, NULL, 0);

	int c_mode = mode ? 1 : 0;
	int c_files = files ? 1 : 0;
	int c_date = date ? 1 : 0;

	TBL* c_table = T_create(nbColumns);
	T_auto(c_table, c_def, c_lecs, c_mode, c_files, c_date);

	SCR_free(lst);
	SCR_free_tbl((unsigned char**) c_lecs);

	return KDBTemplate::add(name, c_table);
}
