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

int KDBTables::add(const std::string& name, const Table& obj)
{
	Table table(obj);
	return KDBTemplate::add(name, static_cast<TBL*>(&table));
}

int KDBTables::add(const std::string& name, const int nbColumns)
{
	// throw exception if object with passed name already exist
	if (K_find(get_KDB(), to_char_array(name)) >= 0) 
		throw IodeExceptionInitialization(iode_type_name + " with name " + name,  
			iode_type_name + " with name " + name + " already exists. Use update() method instead.");

	Table table(nbColumns);

	return KDBTemplate::add(name, static_cast<TBL*>(&table));
}

int KDBTables::add(const std::string& name, const int nbColumns, const std::string& def, const std::vector<std::string>& vars, bool mode, bool files, bool date)
{
	std::string vars_list;
	for (const std::string& var : vars) vars_list += var + ';';
	vars_list += '\n';

	return add(name, nbColumns, def, vars_list, mode, files, date);
}

int KDBTables::add(const std::string& name, const int nbColumns, const std::string& def, const std::string& lecs, bool mode, bool files, bool date)
{
	// throw exception if object with passed name already exist
	if (K_find(get_KDB(), to_char_array(name)) >= 0) 
		throw IodeExceptionInitialization(iode_type_name + " with name " + name,  
			iode_type_name + " with name " + name + " already exists. Use update() method instead.");

	Table table(nbColumns, def, lecs, mode, files, date);

	return KDBTemplate::add(name, static_cast<TBL*>(&table));
}
