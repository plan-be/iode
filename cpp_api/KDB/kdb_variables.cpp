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

IODE_REAL KDBVariables::get_var(const int pos, const int t, const EnumIodeVarMode mode) const
{
	// throw exception if object with passed position is not valid
	get_name(pos);
    return KV_get(get_KDB(), pos, t, mode);
}

IODE_REAL KDBVariables::get_var(const int pos, const std::string& period, const EnumIodeVarMode mode) const
{
	int t = get_sample().get_period_position(period);
	return get_var(pos, t, mode);
}

IODE_REAL KDBVariables::get_var(const int pos, const Period& period, const EnumIodeVarMode mode) const
{
	int t = get_sample().get_period_position(period);
	return get_var(pos, t, mode);
}

IODE_REAL KDBVariables::get_var(const std::string& name, const int t, const EnumIodeVarMode mode) const
{
	// throw exception if object with passed name does not exist
    int pos = get_position(name);
    return get_var(pos, t, mode);
}

IODE_REAL KDBVariables::get_var(const std::string& name, const std::string& period, const EnumIodeVarMode mode) const
{
	int t = get_sample().get_period_position(period);
	return get_var(name, t, mode);
}

IODE_REAL KDBVariables::get_var(const std::string& name, const Period& period, const EnumIodeVarMode mode) const
{
	int t = get_sample().get_period_position(period);
	return get_var(name, t, mode);
}

void KDBVariables::set_var(const int pos, const int t, const IODE_REAL value, const EnumIodeVarMode mode)
{
	// throw exception if object with passed position is not valid
	get_name(pos);
	KV_set(get_KDB(), pos, t, mode, value);
}

void KDBVariables::set_var(const int pos, const std::string& period, const IODE_REAL value, const EnumIodeVarMode mode)
{
	int t = get_sample().get_period_position(period);
	set_var(pos, t, value, mode);
}

void KDBVariables::set_var(const int pos, const Period& period, const IODE_REAL value, const EnumIodeVarMode mode)
{
	int t = get_sample().get_period_position(period);
	set_var(pos, t, value, mode);
}

void KDBVariables::set_var(const std::string& name, const int t, const IODE_REAL value, const EnumIodeVarMode mode)
{
	// throw exception if object with passed name does not exist
	int pos = get_position(name);
	set_var(pos, t, value, mode);
}

void KDBVariables::set_var(const std::string& name, const std::string& period, const IODE_REAL value, const EnumIodeVarMode mode)
{
	int t = get_sample().get_period_position(period);
	set_var(name, t, value, mode);
}

void KDBVariables::set_var(const std::string& name, const Period& period, const IODE_REAL value, const EnumIodeVarMode mode)
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
	char* c_lec = to_char_array(lec);
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

void KDBVariables::copy_into(const std::string& input_file, const std::string& from, const std::string& to, 
	const std::string objects_names)
{
	    std::string buf = input_file + " ";

		Sample smpl = get_sample();
		if (!from.empty()) buf += smpl.start_period().to_string() + " ";
		if (!to.empty()) buf += smpl.end_period().to_string() + " ";
        // thtrow error if wrong samples
        Sample copy_sample(from, to);

        buf += objects_names;

        int res = B_WsCopy(const_cast<char*>(buf.c_str()), I_VARIABLES);

        if (res < 0) B_display_last_error();
}

void KDBVariables::copy_into(const std::string& input_file, const Period& from, const Period& to, 
	const std::string objects_names)
{
	std::string s_from = (from == nullptr) ? "" : from.to_string();
	std::string s_to = (to == nullptr) ? "" : to.to_string();
	copy_into(input_file, s_from, s_to, objects_names);
}

// TODO JMP : please provide input for testing KV_extrapolate
void KDBVariables::extrapolate(const EnumSimulationInitialization method, const std::string& from, 
	const std::string& to, const std::string& variables_list)
{
	KDB* kdb = get_KDB();
	if (kdb == NULL) return;

	Sample sample(from, to);

	char* c_variables_list = const_cast<char*>(variables_list.c_str());
	char** vars = (variables_list.empty()) ? NULL : B_ainit_chk(c_variables_list, NULL, 0);

	int res = KV_extrapolate(kdb, (int) method, sample.c_sample, vars);
	SCR_free_tbl((unsigned char**) vars);

	if (res < 0)
	{
		IodeExceptionFunction error("Cannot extrapolate variables");
		error.add_argument("method", v_simulation_initialization[method]);
		error.add_argument("from", from);
		error.add_argument("to", to);
		error.add_argument("variables_list", variables_list);
		throw error;
	}
}

void KDBVariables::extrapolate(const EnumSimulationInitialization method, const Period& from, 
	const Period& to, const std::string& variables_list)
{
	extrapolate(method, from.to_string(), to.to_string(), variables_list);
}

void KDBVariables::seasonal_adjustment(std::string& input_file, const std::string& series, const double eps_test)
{
	KDB* kdb = get_KDB();
	if (kdb == NULL) return;

	std::string args;

	input_file = check_filepath(input_file, (EnumIodeFile) I_VARIABLES, "seasonal_adjustment", true);
	args = input_file + " "; 

	if (series.empty()) 
		throw IodeExceptionFunction("Cannot run seasonal adjustment", "Passed value for the VarList argument is empty");
	char** c_series = B_ainit_chk(to_char_array(series), NULL, 0);
    if (SCR_tbl_size((unsigned char**) c_series) == 0) 
		throw IodeExceptionFunction("Cannot run seasonal adjustment", "Passed value \"" + series + "\" for the VarList argument is invalid");
	args += series + " ";

	args += std::to_string(eps_test);

	int res = B_WsSeasonAdj(to_char_array(args));
	if (res < 0)
	{
		IodeExceptionFunction error("Cannot run seasonal adjustment");
		error.add_argument("Filename", input_file);
		error.add_argument("VarList", series);
		error.add_argument("Eps", std::to_string(eps_test));
		throw error;
	}
}

void KDBVariables::trend_correction(std::string& input_file, const double lambda, const std::string& series, const bool log)
{
	KDB* kdb = get_KDB();
	if (kdb == NULL) return;

	std::string args;

	input_file = check_filepath(input_file, (EnumIodeFile) I_VARIABLES, "trend_correction", true);
	args = input_file + " "; 

	args += std::to_string(lambda) + " ";

	if (series.empty()) 
		throw IodeExceptionFunction("Cannot run trend correction", "Passed value for the VarList argument is empty");
	char** c_series = B_ainit_chk(to_char_array(series), NULL, 0);
    if (SCR_tbl_size((unsigned char**) c_series) == 0) 
		throw IodeExceptionFunction("Cannot run trend correction", "Passed value \"" + series + "\" for the VarList argument is invalid");
	args += series;

	int res = (log) ? B_WsTrend(to_char_array(args)) : B_WsTrendStd(to_char_array(args));
	if (res < 0)
	{
		std::string fct_name = (log) ? "B_WsTrend" : "B_WsTrendStd";
		IodeExceptionFunction error("Cannot run trend correction (" + fct_name + ")");
		error.add_argument("VarFilename", input_file);
		error.add_argument("Lambda", std::to_string(lambda));
		error.add_argument("Series", series);
		throw error;
	}
}
