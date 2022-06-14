#pragma once
#include "kdb_scalars.h"


// CRUD (Create - Read - Update - Delete) + Copy methods

int KDBScalars::add_or_update(const std::string& name, const Scalar& scalar)
{
	char* c_name = const_cast<char*>(name.c_str());
	int pos = K_add(K_WS[I_SCALARS], c_name, scalar);
	if (pos == -1) throw std::runtime_error("Iode has not been initialized");
	if (pos < -1) throw std::runtime_error("Cannot create or update " + vIodeTypes[type] + " with name " + name);
	return pos;
}

void KDBScalars::add(const std::string& name, const IODE_REAL value, const IODE_REAL relax, const IODE_REAL std)
{
	Scalar scalar;
	scalar.val = value;
	scalar.relax = relax;
	scalar.std = std;
	KDBAbstract::add(name, scalar);
}

void KDBScalars::update(const std::string& name, const IODE_REAL value, const IODE_REAL relax, const IODE_REAL std)
{
	Scalar scalar;
	scalar.val = value;
	scalar.relax = relax;
	scalar.std = std;
	KDBAbstract::update(name, scalar);
}

Scalar KDBScalars::copy_obj(const Scalar& original) const
{
	Scalar scalar_copy;
	memcpy(&scalar_copy, &original, sizeof(Scalar));
	return scalar_copy;
}

Scalar KDBScalars::get_unchecked(const int pos) const
{
	return *KSVAL(get_KDB(), pos);
}
