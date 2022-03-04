#pragma once
#include "kdb_variables.h"


// CRUD (Create - Read - Update - Delete) + Copy methods

void KDBVariables::add_or_update(const std::string& name, const Variable& vars)
{
	char* c_name = const_cast<char*>(name.c_str());
	int nb_obs = get_nb_periods();
	if (vars.size() != nb_obs)
		throw std::runtime_error("Wrong size of the variable vector. Excepted " + std::to_string(nb_obs) + " values but got " + std::to_string(vars.size()) + " values.");
	int pos = K_add(get_KDB(), c_name, vars.data(), &nb_obs);
	if (pos == -1) throw std::runtime_error("Iode has not been initialized");
	if (pos == -2) throw std::runtime_error("Cannot create or update table with name " + name);
}

Variable KDBVariables::copy_obj(const Variable& original) const
{
	return Variable(original);
}

Variable KDBVariables::get_unchecked(const int pos) const
{
	Variable vars;
	int nb_obs = get_nb_periods();
	vars.reserve(nb_obs);
	for (int i=0; i < nb_obs; i++) vars.push_back(KV_get(get_KDB(), pos, i, 0));
	return vars;
}

IODE_REAL KDBVariables::get_var(const int pos, const int t, const int mode) const
{
	// throw exception if object with passed position is not valid
	get_name(pos);
    return KV_get(get_KDB(), pos, t, mode);
}

IODE_REAL KDBVariables::get_var(const std::string& name, const int t, const int mode) const
{
	// throw exception if object with passed name does not exist
    int pos = get_position(name);
    return get_var(pos, t, mode);
}

void KDBVariables::set_var(const int pos, const int t, const int mode, const IODE_REAL value)
{
	// throw exception if object with passed position is not valid
	get_name(pos);
	KV_set(get_KDB(), pos, t, mode, value);
}

void KDBVariables::set_var(const std::string& name, const int t, const int mode, const IODE_REAL value)
{
	// throw exception if object with passed name does not exist
	int pos = get_position(name);
	set_var(pos, t, mode, value);
}

int KDBVariables::get_nb_periods() const
{
    return KSMPL(get_KDB())->s_nb;
}

Period KDBVariables::get_period(const int t) const
{
    PERIOD period = KSMPL(get_KDB())->s_p1;
    return Period(&period).shift(t);
}
