#pragma once
#include "kdb_lists.h"


List KDBLists::get_unchecked(const std::string& name) const
{
	return std::string(KLVAL(get_database(), name));
}

bool KDBLists::add(const std::string& name, const List& list)
{
	char* c_list = to_char_array(list);
	return KDBTemplate::add(name, c_list);
}

void KDBLists::update(const std::string& name, const List& list)
{
	char* c_list = to_char_array(list);
	KDBTemplate::update(name, c_list);
}
