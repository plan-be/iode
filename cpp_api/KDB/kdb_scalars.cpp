#pragma once
#include "kdb_scalars.h"


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

int KDBScalars::add(const std::string& name, const IODE_REAL value, const IODE_REAL relax, const IODE_REAL std)
{
	Scalar scalar;
	scalar.val = value;
	scalar.relax = relax;
	scalar.std = std;
	return KDBTemplate::add(name, scalar);
}

void KDBScalars::update(const std::string& name, const IODE_REAL value, const IODE_REAL relax, const IODE_REAL std)
{
	Scalar scalar;
	scalar.val = value;
	scalar.relax = relax;
	scalar.std = std;
	KDBTemplate::update(name, scalar);
}

void KDBScalars::update(const int pos, const IODE_REAL value, const IODE_REAL relax, const IODE_REAL std)
{
	Scalar scalar;
	scalar.val = value;
	scalar.relax = relax;
	scalar.std = std;
	KDBTemplate::update(pos, scalar);
}
