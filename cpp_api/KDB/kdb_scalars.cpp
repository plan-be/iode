#pragma once
#include "kdb_scalars.h"


Scalar KDBScalars::copy_obj(const Scalar& original) const
{
	return Scalar(original);
}

Scalar KDBScalars::get_unchecked(const int pos) const
{
	return Scalar(pos, get_database());
}

int KDBScalars::add(const std::string& name, const Scalar& obj)
{
	Scalar scalar(obj);
	return KDBTemplate::add(name, static_cast<SCL*>(&scalar));
}

int KDBScalars::add(const std::string& name, const IODE_REAL value, const IODE_REAL relax, const IODE_REAL std)
{
	Scalar scalar(value, relax, std);
	return KDBTemplate::add(name, static_cast<SCL*>(&scalar));
}

void KDBScalars::update(const std::string& name, const IODE_REAL value, const IODE_REAL relax, const IODE_REAL std)
{
	Scalar scalar(value, relax, std);
	KDBTemplate::update(name, static_cast<SCL*>(&scalar));
}

void KDBScalars::update(const int pos, const IODE_REAL value, const IODE_REAL relax, const IODE_REAL std)
{
	Scalar scalar(value, relax, std);
	KDBTemplate::update(pos, static_cast<SCL*>(&scalar));
}
