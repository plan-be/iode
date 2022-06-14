#pragma once
#include "kdb_lists.h"


// CRUD (Create - Read - Update - Delete) + Copy methods

int KDBLists::add_or_update(const std::string& name, const List& list)
{
	char* c_name = const_cast<char*>(name.c_str());
	char* c_list = const_cast<char*>(list.c_str());
	int pos = K_add(K_WS[I_LISTS], c_name, c_list);
	if (pos == -1) throw std::runtime_error("Iode has not been initialized");
	if (pos < -1) throw std::runtime_error("Cannot create or update " + vIodeTypes[type] + " with name " + name);
	return pos;
}

List KDBLists::copy_obj(const List& original) const
{
	return List(original);
}

List KDBLists::get_unchecked(const int pos) const
{
	return std::string(KLVAL(get_KDB(), pos));
}
