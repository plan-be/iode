#pragma once
#include "kdb_scalars.h"


Scalar* KDBScalars::copy_obj(Scalar* const original) const
{
	return new Scalar(*original);
}

Scalar* KDBScalars::get_unchecked(const int pos) const
{
	KDB* kdb = get_database();
    // Note: KSVAL does NOT allocate a new pointer Scalar*
    return static_cast<Scalar*>(KSVAL(kdb, pos));
}

bool KDBScalars::add(const std::string& name, const Scalar& obj)
{
	Scalar scalar(obj);
	return KDBTemplate::add(name, &scalar);
}

bool KDBScalars::add(const std::string& name, const double value, const double relax, const double std)
{
	Scalar scalar(value, relax, std);
	return KDBTemplate::add(name, &scalar);
}

void KDBScalars::update(const std::string& name, const Scalar& obj)
{
	Scalar scalar(obj);
	KDBTemplate::update(name, &scalar);
}

void KDBScalars::update(const std::string& name, const double value, const double relax, const double std)
{
	Scalar scalar(value, relax, std);
	KDBTemplate::update(name, &scalar);
}

void KDBScalars::update(const int pos, const double value, const double relax, const double std)
{
	Scalar scalar(value, relax, std);
	KDBTemplate::update(pos, &scalar);
}
