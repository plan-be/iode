#pragma once
#include "kdb_variables.h"


Variable KDBVariables::get_unchecked(const std::string& name) const
{
	Variable vars;
	if(!check_sample())
		return vars;

	int nb_obs = get_nb_periods();
	KDB* kdb = get_database();
	vars.reserve(nb_obs);
	for (int i=0; i < nb_obs; i++) 
		vars.push_back(KV_get(kdb, name, i, 0));
	return vars;
}

double KDBVariables::get_var(const std::string& name, const int t, const IodeVarMode mode) const
{
	// throw exception if object with passed name does not exist
    if(!this->contains(name))
		throw std::invalid_argument("Cannot get Variable '" + name + "'.\nThe Variable does not exist in the database.");
    return KV_get(get_database(), name, t, mode);
}

double KDBVariables::get_var(const std::string& name, const std::string& period, const IodeVarMode mode) const
{
	if(!check_sample())
		return IODE_NAN;
	
	Sample* sample = get_sample();
	int t = sample->get_period_position(period);
	return get_var(name, t, mode);
}

double KDBVariables::get_var(const std::string& name, const Period& period, const IodeVarMode mode) const
{
	if(!check_sample())
		return IODE_NAN;
	
	Sample* sample = get_sample();
	int t = sample->get_period_position(period);
	return get_var(name, t, mode);
}

double* KDBVariables::get_var_ptr(const std::string& name)
{
	// throw exception if object with passed name does not exist
    if(!this->contains(name))
		throw std::invalid_argument("Cannot get Variable '" + name + "'.\nThe Variable does not exist in the database.");
    return KVVAL(get_database(), name, 0);
}

void KDBVariables::set_var(const std::string& name, const int t, const double value, const IodeVarMode mode)
{
	// throw exception if object with passed name does not exist
    if(!this->contains(name))
		throw std::invalid_argument("Cannot get Variable '" + name + "'.\nThe Variable does not exist in the database.");
	KV_set(get_database(), name, t, mode, value);
}

void KDBVariables::set_var(const std::string& name, const std::string& period, const double value, const IodeVarMode mode)
{
	if(!check_sample())
		return;
	
	Sample* sample = get_sample();
	int t = sample->get_period_position(period);
	set_var(name, t, value, mode);
}

void KDBVariables::set_var(const std::string& name, const Period& period, const double value, const IodeVarMode mode)
{
	if(!check_sample())
		return;
	
	Sample* sample = get_sample();
	int t = sample->get_period_position(period);
	set_var(name, t, value, mode);
}

Variable KDBVariables::calculate_var_from_lec(const std::string& lec, const int t_first, const int t_last)
{
	std::string prefix = "Cannot calculate variable values: ";
	if(!check_sample())
		return Variable();

	int total_nb_periods = get_nb_periods();
	if (total_nb_periods == 0) 
		throw std::runtime_error(prefix + "The Variables sample has not been yet defined");

	std::string error_msg = "period position must be in the range [0, " + 
	                        std::to_string(total_nb_periods - 1) + "]. Got value ";
	if(t_first < 0 || t_first >= total_nb_periods)
	{
		error_msg = prefix + "the first " + error_msg + std::to_string(t_first);
		throw std::invalid_argument(error_msg);
	}

	if(t_last < 0 || t_last >= total_nb_periods)
	{
		error_msg = prefix + "the last " + error_msg + std::to_string(t_last);
		throw std::invalid_argument(error_msg);	
	}

	Variable var;
	var.reserve(t_last - t_first + 1);

	if (lec.empty())
	{
		for (int t = t_first; t <= t_last; t++) 
			var.push_back(IODE_NAN);
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
	if(!check_sample())
		return Variable();
	
	Sample* sample = get_sample();
	int t_first = first_period.empty() ? 0 : sample->get_period_position(first_period);
	int t_last = last_period.empty() ? sample->nb_periods - 1 : sample->get_period_position(last_period);

	return calculate_var_from_lec(lec, t_first, t_last);
}

bool KDBVariables::add(const std::string& name, const Variable& variable)
{
	if(!check_sample())
		throw std::runtime_error("Cannot add Variable '" + name + "'.\nSample is empty");

	check_var_size("add", name, variable);

	int var_size = (int) variable.size();
	return KDBTemplate::add(name, variable.data(), &var_size);
}

bool KDBVariables::add(const std::string& name, const std::string& lec)
{
	if(!check_sample())
		throw std::runtime_error("Cannot add Variable '" + name + "'.\nSample is empty");
	
	int total_nb_periods = get_nb_periods();
	Variable var = calculate_var_from_lec(lec, 0, total_nb_periods - 1);
	return add(name, var);
}

void KDBVariables::update(const std::string& name, const Variable& values, const int t_first, const int t_last)
{
	std::string prefix = "Cannot update variable '" + name + "' values: ";

	if(!check_sample())
			throw std::runtime_error(prefix + "Sample is empty");

	int total_nb_periods = get_nb_periods();
	std::string error_msg = "period position must be in the range [0, " + 
	                        std::to_string(total_nb_periods - 1) + "]. Got value ";
	if(t_first < 0 || t_first >= total_nb_periods)
	{
		error_msg = prefix + "the first " + error_msg + std::to_string(t_first);
		throw std::invalid_argument(error_msg);
	}

	if(t_last < 0 || t_last >= total_nb_periods)
	{
		error_msg = prefix + "the last " + error_msg + std::to_string(t_last);
		throw std::invalid_argument(error_msg);	
	}

	int nb_periods = t_last - t_first + 1;
	if (values.size() != nb_periods)
	{
		error_msg = prefix + "the size of the passed vector " + std::to_string(values.size()) +  
				    " does match the number of expected periods (" + std::to_string(nb_periods) + 
					" -> period " + std::to_string(t_first) + " to " + std::to_string(t_last) + ")";
		throw std::range_error(error_msg);
	}

	// prepare new Variable vector	
	Variable variable = get(name);
	for(int i=0; i < values.size(); i++)
		variable[t_first + i] = values[i];

	// update the variable
	KDBTemplate::update(name, variable.data(), &total_nb_periods);
}

void KDBVariables::update(const std::string& name, const Variable& values, const std::string& first_period, const std::string& last_period)
{
	if(!check_sample())
		return;
	
	Sample* sample = get_sample();
	int t_first = first_period.empty() ? 0 : sample->get_period_position(first_period);
	int t_last = last_period.empty() ? sample->nb_periods - 1 : sample->get_period_position(last_period);

	update(name, values, t_first, t_last);
}

void KDBVariables::update(const std::string& name, const std::string& lec, const int t_first, const int t_last)
{
	Variable var = calculate_var_from_lec(lec, t_first, t_last);
	update(name, var, t_first, t_last);
}

void KDBVariables::update(const std::string& name, const std::string& lec, const std::string& first_period, const std::string& last_period)
{
	if(!check_sample())
		return;
	
	Sample* sample = get_sample();
	int t_first = first_period.empty() ? 0 : sample->get_period_position(first_period);
	int t_last = last_period.empty() ? sample->nb_periods - 1 : sample->get_period_position(last_period);

	update(name, lec, t_first, t_last);
}

// WARNING: the returned Sample pointer must not be deleted
Sample* KDBVariables::get_sample() const
{
	return get_database()->sample;
}

void KDBVariables::set_sample(const std::string& from, const std::string& to)
{
	if(from.empty() && to.empty())
		return;
	
	Sample* sample = get_sample();
    if ((sample == nullptr || sample->nb_periods == 0) && (from.empty() || to.empty()))
	{
        throw std::invalid_argument(std::string("Current sample is empty.\n") + 
			"Please provide a value for both 'from' and 'to' arguments"); 
	}

	std::string from_ = from.empty() ? sample->start_period.to_string() : from;
	std::string to_ = to.empty() ? sample->end_period.to_string() : to;

	Period period_from(from_);
	Period period_to(to_);
	set_sample(period_from, period_to);
}

void KDBVariables::set_sample(const Period& from, const Period& to)
{
	Sample* sample = get_sample();
	Sample new_sample(from, to);
	if(sample != nullptr && new_sample == *sample)
		return;

	// NOTE: prevent changing the sample on a subset (shallow copy).
	//       A shallow copy is created by calling the C function K_quick_refer().
	//       Each 'key' in the shallow copy points to the same data as the original.
	//       The C function KV_sample(KDB*, Sample*) used below does the following things: 
	//       1. changes the 'sample' attribute of the passed KDB
	//       2. reallocates the data for each 'key' (IODE variable) [a bit more more complicated than that but that's not the point]
	//       Problem: if the 'sample' attribute is changed on the subset (passed KDB), the 'sample' attribute of 
	//                the global KDB (K_WS) is NOT changed. Now, let's say the sample of the global KDB is [1990, 2010] 
	//                and the sample of the subset (shallow copy) is [1990, 2000]. Then calling K_WS[var_name, 2001] is still 
	//                possible but will return garbage.
	if(this->is_shallow_copy_database())
		throw std::runtime_error("Changing the sample on a subset of the Variables workspace is not allowed");	

	int res = KV_sample(get_database(), &new_sample);
	if (res < 0) 
	{
		std::string error_msg = "Cannot set sample -> invalid \"from_period\" or \"to_period\" argument\n";
		error_msg += "from_period: " + from.to_string() + "\n";
		error_msg += "to_period: " + to.to_string();
		throw std::invalid_argument(error_msg);
	}
}

int KDBVariables::get_nb_periods() const
{
	Sample* sample = get_sample();
	if(!sample)
		return 0;
    return sample->nb_periods;
}

std::string KDBVariables::get_period(const int t) const
{
	if(!check_sample())
		return "";
	
	Sample* sample = get_sample();
    Period period = sample->start_period;
    return Period(period).shift(t).to_string();
}

float KDBVariables::get_period_as_float(const int t) const
{
	if(!check_sample())
		return 0;
	
	Sample* sample = get_sample();
    Period period = sample->start_period;
    return Period(period).shift(t).to_float();
}

std::vector<std::string> KDBVariables::get_list_periods(const std::string& from, const std::string& to) const
{
	if(!check_sample())
		return std::vector<std::string>();
	
	Sample* sample = get_sample();
	if(from.empty() && to.empty())
		return sample->get_list_periods();
	else
	{
		std::string from_ = from.empty() ? sample->start_period.to_string() : from;
		std::string to_ = to.empty() ? sample->end_period.to_string() : to;
		return Sample(from_, to_).get_list_periods();
	}
}

std::vector<float> KDBVariables::get_list_periods_as_float(const std::string& from, const std::string& to) const
{
	if(!check_sample())
		return std::vector<float>();
	
	Sample* sample = get_sample();
	if(from.empty() && to.empty())
		return sample->get_list_periods_as_float();
	else
	{
		std::string from_ = from.empty() ? sample->start_period.to_string() : from;
		std::string to_ = to.empty() ? sample->end_period.to_string() : to;
		return Sample(from_, to_).get_list_periods_as_float();
	}
}

void KDBVariables::copy_from(const std::string& input_file, const std::string& from, const std::string& to, 
	const std::string objects_names)
{
	    std::string buf = input_file + " ";

		if((!from.empty()) || (!to.empty()))
		{
			Sample* var_sample = get_sample();
			buf += from.empty() ? var_sample->start_period.to_string() + " " : from + " ";
			buf += to.empty() ? var_sample->end_period.to_string() + " " : to + " ";
			// throw error if wrong samples
			Sample copy_sample(from, to);
		}

        buf += objects_names;

        int res = B_WsCopy(const_cast<char*>(buf.c_str()), VARIABLES);

        if (res < 0)
		{
			std::string last_error = error_manager.get_last_error();
			if(!last_error.empty())
				throw std::runtime_error("Cannot copy the content of file '" + input_file + 
						"' into the Variables database.\n" + last_error);
		}
}

void KDBVariables::copy_from(const std::string& input_file, const Period* from, const Period* to, 
	const std::string& objects_names)
{
	std::string s_from = (from == nullptr) ? "" : from->to_string();
	std::string s_to = (to == nullptr) ? "" : to->to_string();
	copy_from(input_file, s_from, s_to, objects_names);
}

void KDBVariables::extrapolate(const VariablesInitialization method, const std::string& from, 
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
		std::string error_msg = "Cannot extrapolate variables\n";
		error_msg += "method: " + v_simulation_initialization[method] + "\n";
		error_msg += "from period: " + from + "\n";
		error_msg += "to period: " + to + "\n";
		error_msg += "variables list: " + variables_list;
		throw std::runtime_error(error_msg);
	}
}

void KDBVariables::extrapolate(const VariablesInitialization method, const Period& from, 
	const Period& to, const std::string& variables_list)
{
	extrapolate(method, from.to_string(), to.to_string(), variables_list);
}

void KDBVariables::seasonal_adjustment(std::string& input_file, const std::string& series, const double eps_test)
{
	KDB* kdb = get_database();
	if (kdb == NULL) return;

	std::string args;

	input_file = check_filepath(input_file, FILE_VARIABLES, "seasonal_adjustment", true);
	args = input_file + " "; 

	if(series.empty()) 
		throw std::invalid_argument(std::string("Cannot run seasonal adjustment: Passed value for ") + 
		                            "the variables list argument is empty");
	char** c_series = B_ainit_chk(to_char_array(series), NULL, 0);
    if(SCR_tbl_size((unsigned char**) c_series) == 0) 
		throw std::invalid_argument("Cannot run seasonal adjustment: Passed value \"" + series + 
		                            "\" for the variables list argument is invalid");
	args += series + " ";

	args += std::to_string(eps_test);

	int res = B_WsSeasonAdj(to_char_array(args));
	if (res < 0)
	{
		std::string error_msg = "Cannot run seasonal adjustment\n";
		error_msg += "Filename: " + input_file + "\n";
		error_msg += "Variables list: " + series + "\n";
		error_msg += "Epsilon: " + std::to_string(eps_test);
		throw std::runtime_error(error_msg);
	}
}

void KDBVariables::trend_correction(std::string& input_file, const double lambda, const std::string& series, const bool log)
{
	KDB* kdb = get_database();
	if (kdb == NULL) return;

	std::string args;

	input_file = check_filepath(input_file, FILE_VARIABLES, "trend_correction", true);
	args = input_file + " "; 

	args += std::to_string(lambda) + " ";

	if(series.empty()) 
		throw std::invalid_argument(std::string("Cannot run trend correction: Passed value for ") + 
		                            "the variables list argument is empty");
	char** c_series = B_ainit_chk(to_char_array(series), NULL, 0);
    if(SCR_tbl_size((unsigned char**) c_series) == 0) 
		throw std::invalid_argument("Cannot run trend correction: Passed value \"" + series + 
		                            "\" for the variables list argument is invalid");
	args += series;

	int res = (log) ? B_WsTrend(to_char_array(args)) : B_WsTrendStd(to_char_array(args));
	if (res < 0)
	{
		std::string fct_name = (log) ? "B_WsTrend" : "B_WsTrendStd";
		std::string error_msg = "Cannot run trend correction (" + fct_name + ")\n";
		error_msg += "Variables filename: " + input_file + "\n";
		error_msg += "Lambda: " + std::to_string(lambda) + "\n";
		error_msg += "Series: " + series;
		throw std::runtime_error(error_msg);
	}
}
