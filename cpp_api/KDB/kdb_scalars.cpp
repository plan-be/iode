#pragma once
#include "kdb_scalars.h"


// CRUD (Create - Read - Update - Delete) + Copy methods

void KDBScalars::add_or_update(const std::string& name, const Scalar& scalar)
{
	char* c_name = const_cast<char*>(name.c_str());
	int pos = K_add(get_KDB(), c_name, scalar);
	if (pos == -1) throw std::runtime_error("Iode has not been initialized");
	if (pos == -2) throw std::runtime_error("Cannot create or update table with name " + name);
}

void KDBScalars::add(const std::string& name, const IODE_REAL value, const IODE_REAL relax)
{
	Scalar scalar;
	scalar.val = value;
	if (relax < 0.0 || relax > 1.0) throw std::runtime_error("Value for the parameter relax must be in range [0, 1].");
	scalar.relax = relax;
	KDBAbstract::add(name, scalar);
}

void KDBScalars::update(const std::string& name, const IODE_REAL value, const IODE_REAL relax)
{
	Scalar scalar;
	scalar.val = value;
	if (relax < 0.0 || relax > 1.0) throw std::runtime_error("Value for the parameter relax must be in range [0, 1].");
	scalar.relax = relax;
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
