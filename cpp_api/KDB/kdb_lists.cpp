#pragma once
#include "kdb_lists.h"


List KDBLists::copy_obj(const List& original) const
{
	return List(original);
}

List KDBLists::get_unchecked(const int pos) const
{
	return std::string(KLVAL(get_KDB(), pos));
}

int KDBLists::add(const std::string& name, const List& list)
{
	char* c_list = to_char_array(list);
	return KDBTemplate::add(name, c_list);
}

void KDBLists::update(const std::string& name, const List& list)
{
	char* c_list = to_char_array(list);
	KDBTemplate::update(name, c_list);
}

void KDBLists::update(const int pos, const List& list)
{
	char* c_list = to_char_array(list);
	KDBTemplate::update(pos, c_list);
}
