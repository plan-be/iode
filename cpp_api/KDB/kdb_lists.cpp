#pragma once
#include "kdb_lists.h"


List KDBLists::get_unchecked(const std::string& name) const
{
	return std::string(KLVAL(get_database(), name));
}

bool KDBLists::add(const std::string& name, const List& list)
{;
	return KDBTemplate::add(name, list);
}

void KDBLists::update(const std::string& name, const List& list)
{
	KDBTemplate::update(name, list);
}
