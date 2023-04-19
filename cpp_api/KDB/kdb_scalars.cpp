#pragma once
#include "kdb_scalars.h"


Scalar KDBScalars::copy_obj(const Scalar& original) const
{
	return Scalar(original);
}

Scalar KDBScalars::get_unchecked(const int pos) const
{
	return Scalar(pos, get_KDB());
}

int KDBScalars::add(const std::string& name, const Scalar& obj)
{
	return KDBTemplate::add(name, obj.c_scalar);
}

int KDBScalars::add(const std::string& name, const IODE_REAL value, const IODE_REAL relax, const IODE_REAL std)
{
	Scalar scalar(value, relax, std);
	return KDBTemplate::add(name, scalar.c_scalar);
}

void KDBScalars::update(const std::string& name, const IODE_REAL value, const IODE_REAL relax, const IODE_REAL std)
{
	Scalar scalar(value, relax, std);
	KDBTemplate::update(name, scalar.c_scalar);
}

void KDBScalars::update(const int pos, const IODE_REAL value, const IODE_REAL relax, const IODE_REAL std)
{
	Scalar scalar(value, relax, std);
	KDBTemplate::update(pos, scalar.c_scalar);
}
