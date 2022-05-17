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

Variable KDBVariables::new_var_from_lec(const std::string& lec)
{
	if (lec.empty()) throw std::runtime_error("Cannot create or update variable. Passed LEC expression is empty.");

	Sample sample = get_sample();
	if (sample.nb_periods() == 0) throw std::runtime_error("Variables sample has not been yet defined. Cannot create a new variable.");

	// code below is an adapted copy/paste from B_DataCalcVar() (in b_data.c)
	char* c_lec = const_cast<char*>(lec.c_str());
	// Compiles the LEC string 
	CLEC* clec = L_cc(c_lec);
	// L_link(): Links the CLEC expression to KDB's of variables and of scalars.
	// The CLEC object is modified (inplace) by L_link()
	if (clec != NULL && L_link(KV_WS, KS_WS, clec) == 0)
	{
		Variable var;
		for (int t = 0; t < sample.nb_periods(); t++) var.push_back(L_exec(KV_WS, KS_WS, clec, t));
		SW_nfree(clec);
		return var;
	}
	else 
	{
		SW_nfree(clec);
		throw std::runtime_error("Cannot create or update variable.");
	}
}

void KDBVariables::add(const std::string& name, const Variable& variable)
{
	KDBAbstract::add(name, variable);
}

void KDBVariables::add(const std::string& name, const std::string& lec)
{
	Variable var = new_var_from_lec(lec);
	KDBAbstract::add(name, var);
}

void KDBVariables::update(const int pos, const Variable& variable)
{
	KDBAbstract::update(pos, variable);
}

void KDBVariables::update(const int pos, const std::string& lec)
{
	Variable var = new_var_from_lec(lec);
	KDBAbstract::update(pos, var);
}

void KDBVariables::update(const std::string& name, const Variable& variable)
{
	KDBAbstract::update(name, variable);
}

void KDBVariables::update(const std::string& name, const std::string& lec)
{
	Variable var = new_var_from_lec(lec);
	KDBAbstract::update(name, var);
}

Sample KDBVariables::get_sample() const
{
	return Sample(KSMPL(get_KDB()));
}

void KDBVariables::set_sample(const std::string& from, const std::string& to)
{
	Period period_from(from);
	Period period_to(to);
	set_sample(period_from, period_to);
}

void KDBVariables::set_sample(const Period& from, const Period& to)
{
	Sample sample(from, to);
	int res = KV_sample(get_KDB(), sample.c_sample);
	if (res < 0) throw std::runtime_error("Could not associate the sample " + sample.to_string() + " to the Variables.");
}

int KDBVariables::get_nb_periods() const
{
    return get_sample().nb_periods();
}

Period KDBVariables::get_period(const int t) const
{
    PERIOD period = KSMPL(get_KDB())->s_p1;
    return Period(&period).shift(t);
}

