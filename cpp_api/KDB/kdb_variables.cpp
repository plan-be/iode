#pragma once
#include "kdb_variables.h"


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

IODE_REAL KDBVariables::get_var(const int pos, const std::string& period, const int mode) const
{
	int t = get_sample().get_period_position(period);
	return get_var(pos, t, mode);
}

IODE_REAL KDBVariables::get_var(const int pos, const Period& period, const int mode) const
{
	int t = get_sample().get_period_position(period);
	return get_var(pos, t, mode);
}

IODE_REAL KDBVariables::get_var(const std::string& name, const int t, const int mode) const
{
	// throw exception if object with passed name does not exist
    int pos = get_position(name);
    return get_var(pos, t, mode);
}

IODE_REAL KDBVariables::get_var(const std::string& name, const std::string& period, const int mode) const
{
	int t = get_sample().get_period_position(period);
	return get_var(name, t, mode);
}

IODE_REAL KDBVariables::get_var(const std::string& name, const Period& period, const int mode) const
{
	int t = get_sample().get_period_position(period);
	return get_var(name, t, mode);
}

void KDBVariables::set_var(const int pos, const int t, const IODE_REAL value, const int mode)
{
	// throw exception if object with passed position is not valid
	get_name(pos);
	KV_set(get_KDB(), pos, t, mode, value);
}

void KDBVariables::set_var(const int pos, const std::string& period, const IODE_REAL value, const int mode)
{
	int t = get_sample().get_period_position(period);
	set_var(pos, t, value, mode);
}

void KDBVariables::set_var(const int pos, const Period& period, const IODE_REAL value, const int mode)
{
	int t = get_sample().get_period_position(period);
	set_var(pos, t, value, mode);
}

void KDBVariables::set_var(const std::string& name, const int t, const IODE_REAL value, const int mode)
{
	// throw exception if object with passed name does not exist
	int pos = get_position(name);
	set_var(pos, t, value, mode);
}

void KDBVariables::set_var(const std::string& name, const std::string& period, const IODE_REAL value, const int mode)
{
	int t = get_sample().get_period_position(period);
	set_var(name, t, value, mode);
}

void KDBVariables::set_var(const std::string& name, const Period& period, const IODE_REAL value, const int mode)
{
	int t = get_sample().get_period_position(period);
	set_var(name, t, value, mode);
}

Variable KDBVariables::new_var_from_lec(const std::string& lec)
{
	Variable var;

	Sample sample = get_sample();
	if (sample.nb_periods() == 0) throw IodeExceptionInitialization("variable", 
		"Variables sample has not been yet defined");

	if (lec.empty())
	{
		for (int t = 0; t < sample.nb_periods(); t++) var.push_back(L_NAN);
		return var;
	}

	// code below is an adapted copy/paste from B_DataCalcVar() (in b_data.c)
	char* c_lec = const_cast<char*>(lec.c_str());
	// Compiles the LEC string 
	CLEC* clec = L_cc(c_lec);
	// L_link(): Links the CLEC expression to KDB's of variables and of scalars.
	// The CLEC object is modified (inplace) by L_link()
	if (clec != NULL && L_link(KV_WS, KS_WS, clec) == 0)
	{
		for (int t = 0; t < sample.nb_periods(); t++) var.push_back(L_exec(KV_WS, KS_WS, clec, t));
		SW_nfree(clec);
		return var;
	}
	else 
	{
		SW_nfree(clec);
		IodeExceptionFunction error("Cannot create or update variable", "Unknown");
		error.add_argument("lec", lec);
		throw error;
	}
}

int KDBVariables::add(const std::string& name, const Variable& variable)
{
	int var_size = (int) variable.size();
	return KDBTemplate::add(name, variable.data(), &var_size);
}

int KDBVariables::add(const std::string& name, const std::string& lec)
{
	Variable var = new_var_from_lec(lec);
	return add(name, var);
}

void KDBVariables::update(const int pos, const Variable& variable)
{
	int var_size = (int) variable.size();
	KDBTemplate::update(pos, variable.data(), &var_size);
}

void KDBVariables::update(const int pos, const std::string& lec)
{
	Variable var = new_var_from_lec(lec);
	update(pos, var);
}

void KDBVariables::update(const std::string& name, const Variable& variable)
{
	int var_size = (int) variable.size();
	KDBTemplate::update(name, variable.data(), &var_size);
}

void KDBVariables::update(const std::string& name, const std::string& lec)
{
	Variable var = new_var_from_lec(lec);
	update(name, var);
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
	if (res < 0) 
	{
		IodeExceptionFunction error("Cannot set sample", "Unknown");
		error.add_argument("from", from.to_string());
		error.add_argument("to  ", to.to_string());
		throw error;
	}
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

