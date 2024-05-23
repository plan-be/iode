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

	KDB* kdb = get_database();
	vars.reserve(nb_obs);
	for (int i=0; i < nb_obs; i++) 
		vars.push_back(KV_get(kdb, pos, i, 0));
	return vars;
}

IODE_REAL KDBVariables::get_var(const int pos, const int t, const EnumIodeVarMode mode) const
{
	// throw exception if object with passed position is not valid
	get_name(pos);
    return KV_get(get_database(), pos, t, mode);
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

IODE_REAL* KDBVariables::get_var_ptr(const int pos)
{
	if(get_nb_periods() == 0)
		return NULL;

	// throw exception if object with passed position is not valid
	get_name(pos);

	return KVVAL(get_database(), pos, 0);
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

IODE_REAL* KDBVariables::get_var_ptr(const std::string& name)
{
	// throw exception if object with passed name does not exist
	int pos = get_position(name);
	return get_var_ptr(pos);
}

void KDBVariables::set_var(const int pos, const int t, const IODE_REAL value, const EnumIodeVarMode mode)
{
	// throw exception if object with passed position is not valid
	get_name(pos);
	KV_set(get_database(), pos, t, mode, value);
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

Variable KDBVariables::calculate_var_from_lec(const std::string& lec, const int t_first, const int t_last)
{
	std::string prefix = "Cannot calculate variable values: ";

	int total_nb_periods = get_nb_periods();
	if (total_nb_periods == 0) 
		throw std::runtime_error(prefix + "The Variables sample has not been yet defined");

	std::string error_msg = prefix + "the {} period position must be in the range [0, " + 
							std::to_string(total_nb_periods - 1) + "]. Got value {:d}";
	if(t_first < 0 || t_first >= total_nb_periods)
		throw std::invalid_argument(std::vformat(error_msg, std::make_format_args("first", t_first)));

	if(t_last < 0 || t_last >= total_nb_periods)
		throw std::invalid_argument(std::vformat(error_msg, std::make_format_args("last", t_last)));

	Variable var;
	var.reserve(t_last - t_first + 1);

	if (lec.empty())
	{
		for (int t = t_first; t <= t_last; t++) 
			var.push_back(L_NAN);
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
		for (int t = t_first; t <= t_last; t++) 
			var.push_back(L_exec(KV_WS, KS_WS, clec, t));
		SW_nfree(clec);
		return var;
	}
	else 
	{
		SW_nfree(clec);
		throw std::runtime_error("Calculate variable values: Cannot compute LEC expressions: '" + lec + "'");
	}
}

Variable KDBVariables::calculate_var_from_lec(const std::string& lec, const std::string& first_period, const std::string& last_period)
{
	Sample sample = get_sample();
	if (sample.nb_periods() == 0) 
		throw std::runtime_error("The Variables sample has not been yet defined");

	int t_first = first_period.empty() ? 0 : sample.get_period_position(first_period);
	int t_last = last_period.empty() ? sample.nb_periods() - 1 : sample.get_period_position(last_period);

	return calculate_var_from_lec(lec, t_first, t_last);
}

int KDBVariables::add(const std::string& name, const Variable& variable)
{
	if(get_nb_periods() == 0)
		throw std::runtime_error("Cannot add Variable '" + name + "'.\nSample is empty");

	check_var_size("add", name, variable);

	int var_size = (int) variable.size();
	return KDBTemplate::add(name, variable.data(), &var_size);
}

int KDBVariables::add(const std::string& name, const std::string& lec)
{
	int total_nb_periods = get_nb_periods();
	if(total_nb_periods == 0)
		throw std::runtime_error("Cannot add Variable '" + name + "'.\nSample is empty");

	Variable var = calculate_var_from_lec(lec, 0, total_nb_periods - 1);
	return add(name, var);
}

void KDBVariables::update(const int pos, const Variable& values, const int t_first, const int t_last)
{
	std::string name = get_name(pos);
	update(name, values, t_first, t_last);
}

void KDBVariables::update(const int pos, const Variable& values, const std::string& first_period, const std::string& last_period)
{
	std::string name = get_name(pos);
	update(name, values, first_period, last_period);
}

void KDBVariables::update(const int pos, const std::string& lec, const int t_first, const int t_last)
{
	std::string name = get_name(pos);
	update(name, lec, t_first, t_last);
}

void KDBVariables::update(const int pos, const std::string& lec, const std::string& first_period, const std::string& last_period)
{
	std::string name = get_name(pos);
	update(name, lec, first_period, last_period);
}

void KDBVariables::update(const std::string& name, const Variable& values, const int t_first, const int t_last)
{
	std::string prefix = "Cannot update variable '" + name + "' values: ";

	int total_nb_periods = get_nb_periods();
	if (total_nb_periods == 0) 
		throw std::runtime_error(prefix + "The Variables sample has not been yet defined");

	std::string error_msg = prefix + "the {} period position must be in the range [0, " + 
							std::to_string(total_nb_periods - 1) + "]. Got value {:d}";
	if(t_first < 0 || t_first >= total_nb_periods)
		throw std::invalid_argument(std::vformat(error_msg, std::make_format_args("first", t_first)));

	if(t_last < 0 || t_last >= total_nb_periods)
		throw std::invalid_argument(std::vformat(error_msg, std::make_format_args("last", t_last)));

	int nb_periods = t_last - t_first + 1;
	if (values.size() != nb_periods)
		throw std::range_error(std::vformat(prefix + "the size of the passed vector ({:d}) " + 
				"does match the number of expected periods ({:d} -> period {:d} to {:d})", 
				std::make_format_args((int) values.size(), nb_periods, t_first, t_last)));

	// prepare new Variable vector	
	Variable variable = get(name);
	for(int i=0; i < values.size(); i++)
		variable[t_first + i] = values[i];

	// update the variable
	KDBTemplate::update(name, variable.data(), &total_nb_periods);
}

void KDBVariables::update(const std::string& name, const Variable& values, const std::string& first_period, const std::string& last_period)
{
	Sample sample = get_sample();
	if (sample.nb_periods() == 0) 
		throw std::runtime_error("The Variables sample has not been yet defined");

	int t_first = first_period.empty() ? 0 : sample.get_period_position(first_period);
	int t_last = last_period.empty() ? sample.nb_periods() - 1 : sample.get_period_position(last_period);

	update(name, values, t_first, t_last);
}

void KDBVariables::update(const std::string& name, const std::string& lec, const int t_first, const int t_last)
{
	Variable var = calculate_var_from_lec(lec, t_first, t_last);
	update(name, var, t_first, t_last);
}

void KDBVariables::update(const std::string& name, const std::string& lec, const std::string& first_period, const std::string& last_period)
{
	Sample sample = get_sample();
	if (sample.nb_periods() == 0) 
		throw std::runtime_error("The Variables sample has not been yet defined");
	
	int t_first = first_period.empty() ? 0 : sample.get_period_position(first_period);
	int t_last = last_period.empty() ? sample.nb_periods() - 1 : sample.get_period_position(last_period);

	update(name, lec, t_first, t_last);
}

Sample KDBVariables::get_sample() const
{
	return Sample(*KSMPL(get_database()));
}

void KDBVariables::set_sample(const std::string& from, const std::string& to)
{
	if(from.empty() && to.empty())
		return;
	
	Sample sample = get_sample();
    if (sample.nb_periods() == 0 && (from.empty() || to.empty()))
	{
        throw std::invalid_argument(std::string("Current sample is empty.\n") + 
			"Please provide a value for both 'from' and 'to' arguments"); 
	}

	std::string from_ = from.empty() ? sample.start_period().to_string() : from;
	std::string to_ = to.empty() ? sample.end_period().to_string() : to;

	Period period_from(from_);
	Period period_to(to_);
	set_sample(period_from, period_to);
}

void KDBVariables::set_sample(const Period& from, const Period& to)
{
	Sample sample(from, to);
	int res = KV_sample(get_database(), &sample);
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

std::string KDBVariables::get_period(const int t) const
{
    PERIOD period = KSMPL(get_database())->s_p1;
    return Period(period).shift(t).to_string();
}

float KDBVariables::get_period_as_float(const int t) const
{
    PERIOD period = KSMPL(get_database())->s_p1;
    return Period(period).shift(t).to_float();
}

std::vector<std::string> KDBVariables::get_list_periods(const std::string& from, const std::string& to) const
{
	Sample sample = get_sample();
	if(from.empty() && to.empty())
		return sample.get_list_periods();
	else
	{
		std::string from_ = from.empty() ? sample.start_period().to_string() : from;
		std::string to_ = to.empty() ? sample.end_period().to_string() : to;
		return Sample(from_, to_).get_list_periods();
	}
}

std::vector<float> KDBVariables::get_list_periods_as_float(const std::string& from, const std::string& to) const
{
	Sample sample = get_sample();
	if(from.empty() && to.empty())
		return sample.get_list_periods_as_float();
	else
	{
		std::string from_ = from.empty() ? sample.start_period().to_string() : from;
		std::string to_ = to.empty() ? sample.end_period().to_string() : to;
		return Sample(from_, to_).get_list_periods_as_float();
	}
}

void KDBVariables::copy_from(const std::string& input_file, const std::string& from, const std::string& to, 
	const std::string objects_names)
{
	    std::string buf = input_file + " ";

		if((!from.empty()) || (!to.empty()))
		{
			Sample var_sample = get_sample();
			buf += from.empty() ? var_sample.start_period().to_string() + " " : from + " ";
			buf += to.empty() ? var_sample.end_period().to_string() + " " : to + " ";
			// throw error if wrong samples
			Sample copy_sample(from, to);
		}

        buf += objects_names;

        int res = B_WsCopy(const_cast<char*>(buf.c_str()), I_VARIABLES);

        if (res < 0)
		{
			std::string msg = "Cannot copy the content of file '" + input_file + "' into the Variables database.\n";
			msg += get_last_error();
			throw std::runtime_error(msg);
		}
}

void KDBVariables::copy_from(const std::string& input_file, const Period* from, const Period* to, 
	const std::string& objects_names)
{
	std::string s_from = (from == nullptr) ? "" : from->to_string();
	std::string s_to = (to == nullptr) ? "" : to->to_string();
	copy_from(input_file, s_from, s_to, objects_names);
}

// TODO JMP : please provide input for testing KV_extrapolate
void KDBVariables::extrapolate(const EnumSimulationInitialization method, const std::string& from, 
	const std::string& to, const std::string& variables_list)
{
	KDB* kdb = get_database();
	if (kdb == NULL) return;

	Sample sample(from, to);

	char* c_variables_list = const_cast<char*>(variables_list.c_str());
	char** vars = (variables_list.empty()) ? NULL : B_ainit_chk(c_variables_list, NULL, 0);

	int res = KV_extrapolate(kdb, (int) method, &sample, vars);
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
	KDB* kdb = get_database();
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
	KDB* kdb = get_database();
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
