#pragma once

#include <ctype.h>
#include "api/utils/utils.h"

#include <string>
#include <map>
#include <array>
#include <stdexcept>

/**
 *  Variables for period definitions: 
 *      periodicities : list of valid periodicities :
 *          Y = Yearly
 *          S = Half-Yearly
 *          Q = Quaterly
 *          M = Monthly
 *          W = Weekly
 *          D = Daily
 */

const static std::string periodicities = "YSQMWD";

const static std::map<char, int> map_periodicities =
{
	{'Y', 1},
	{'S', 2},
	{'Q', 4},
	{'M', 12},
	{'W', 52},
	{'D', 365}
};

inline std::array<int, 6> L_Period_NB = {1, 2, 4, 12, 52, 365};

/**
 *  Gives the nbr of periods in one year for the periodicity ch.
 *  
 *  @param ch periodicity. Can be upper or lowercase.
 *  @return number of periods for the periodicity ch.
 *  
 *  @note same function as PER_nb
 */
inline int get_nb_periods_per_year(const char ch)
{
	if(ch == 0)
		throw std::invalid_argument("Cannot get the number of periods per year:\nEmpty periodicity");

	char upper_ch = toupper(ch);
	if(!map_periodicities.contains(upper_ch))
		throw std::invalid_argument("Invalid periodicity '" + std::string(1, ch) + "'.\n" +
									"Possible values are '" + periodicities + "'");
	return map_periodicities.at(upper_ch);
}

const static std::map<char, float> map_steps =
{
	{'Y', 0.f},
	{'S', 1.f/2},
	{'Q', 1.f/4},
	{'M', 1.f/12},
	{'W', 1.f/52},
	{'D', 1.f/365}
};

inline float get_step(const char ch)
{
	if(ch == 0)
		throw std::invalid_argument("Cannot convert the periodicity to float:\nEmpty periodicity");

	char upper_ch = toupper(ch);
	if(!map_steps.contains(upper_ch))
		throw std::invalid_argument("Invalid periodicity '" + std::string(1, ch) + "'.\n" +
									"Possible values are '" + periodicities + "'");
	return map_steps.at(upper_ch);
}


struct Period
{
    long    year;        	// year
    long    step;        	// position in the year (according to the periodicity)
    char    periodicity;	// periodicity (Y S Q M W D)
    // char    p_pad[3]; 

public:
	Period()
	{
		year = 0;
		periodicity = 0;
		step = 0;
	}

	Period(const int year, const char periodicity, const int step)
	{
		std::string error_msg = "Cannot create a new period '" + std::to_string(year) + 
			std::string(1, periodicity) + std::to_string(step) + "'\n";

		// check periodicity
		int nb_periods;
		try
		{
			nb_periods = get_nb_periods_per_year(periodicity);
		}
		catch(const std::exception& e)
		{
			throw std::invalid_argument(error_msg + std::string(e.what()));
		}
		
		// check step
		if (step < 1 || step > nb_periods)
			throw std::invalid_argument(error_msg + "Period step must be in range [1, " + std::to_string(nb_periods) + "])");
		
		// initialize class members
		this->year = year;
		this->periodicity = periodicity;
		this->step = step;
	}

	Period(const Period& other)
	{
		this->year = other.year;
		this->periodicity = other.periodicity;
		this->step = other.step;
	}

	Period(const std::string& str_period)
	{
		this->year = 0;
		this->periodicity = 0;
		this->step = 0;

		if(str_period.empty())
			throw std::invalid_argument("String for creating a period is empty");

		std::string error_msg = "Cannot create a period from the string '" + str_period + "'\n";

		// Search the position of the periodicity character
		int i;
		for(i = 0 ; i < 4 ; i++)
			if(!isdigit(str_period[i])) 
				throw std::invalid_argument(error_msg + "The character at position " + 
					std::to_string(i) + "must be a digit (year).");

		// Check that the periodicity is valid
		char periodicity = toupper(str_period[i]);
		int max_step; 
		try
		{
			max_step = get_nb_periods_per_year(periodicity);
		}
		catch(const std::exception& e)
		{
			throw std::invalid_argument(error_msg + std::string(e.what()));
		}
		this->periodicity = periodicity;
		
		// Retrieve the year (first digits). 
		// If year < 50, adds 2000 (49Y1 => 2049Y1)
		// if year < 200, adds 1900 (60Y1 => 1960Y1)
		// NOTE: std::stoi() -> If no conversion could be performed, 
		//       an invalid_argument exception is thrown.
		int year = 0;
		try
		{    
			year = std::stoi(str_period.substr(0, i));
			if(year < 50) 
				year += 2000;
			else if(year < 200) 
				year += 1900;
		}
		catch(const std::exception& e)
		{
			throw std::invalid_argument(error_msg + std::string(e.what()));
		}
		this->year = year;

		// calculate sub period
		int step = 0;
		try
		{
			step = (str_period.size() == i) ? 1 : std::stoi(str_period.substr(i+1));
		}
		catch(const std::exception& e)
		{
			throw std::invalid_argument(error_msg + std::string(e.what()));
		}

		if(step > max_step)
			throw std::invalid_argument(error_msg + "The maximum position in the year for the periodicity '" + 
										std::to_string(periodicity) + "' is " + std::to_string(max_step));
		this->step = step;
	}

	int difference(const Period& other) const
	{
		if (periodicity != other.periodicity) 
		{
			std::string error_msg = "Cannot calculate the difference between the periods '" + 
									to_string() + "' and '" + other.to_string() + "'\n";
			error_msg += "The two periods must share the same periodicity:\n";
			error_msg += "left period periodicity :" + std::string(1, periodicity) + "\n";
			error_msg += "right period periodicity:" + std::string(1, other.periodicity);
			throw std::runtime_error(error_msg);
		}

		return (get_nb_periods_per_year(periodicity) * (year - other.year)) + (step - other.step);
	}

	Period shift(const int nb_periods)
	{
		int nb_periods_per_year_ = get_nb_periods_per_year(this->periodicity);
		int delta_years;

		Period shifted_period(*this);

		// NOTE: - first step of the year is 1, not 0 !
		//       - step is in range [1, nb periods per year]

		// go forward
		if(nb_periods >= 0) 
		{
			int sum_steps = shifted_period.step + nb_periods;
			delta_years = (sum_steps - 1) / nb_periods_per_year_;

			shifted_period.year += delta_years;
			shifted_period.step = 1 + (sum_steps - 1) % nb_periods_per_year_;
		}
		// go backward
		else 
		{
			int shift_steps = shifted_period.step + nb_periods;
			if(shift_steps <= 0) 
			{
				delta_years = 1 + (-shift_steps) / nb_periods_per_year_;

				shifted_period.year -= delta_years;
				shifted_period.step = delta_years * nb_periods_per_year_ + shift_steps;
			}
			else 
				shifted_period.step = shift_steps;
		}

		return shifted_period;
	}
	
	std::string to_string() const
	{
		if(year == 0 || periodicity == 0 || step == 0)
			return "";
		else
			// <year><periodicity><step>
			return std::to_string(year) + std::string(1, periodicity) + std::to_string(step);
	}

	/**
	 * @brief transforms a period into a float value.
	 *        For examples:
	 *        2000Y1    ->    2000.0
	 *        2000Q2    ->    2000.25
	 *        2000M4    ->    2000.25
	 * 
	 * @return float 
	 */
	float to_float() const
	{
			float value = static_cast<float>(year);
			if(periodicity != 'Y') 
				value += get_step(periodicity) * (step - 1);
			return value;
	}

	bool operator==(const Period& other) const
	{ 
		return (year == other.year) && (periodicity == other.periodicity) && (step == other.step);
	}

	// TODO : implement operators > and < ? 
	 
};


// Custom specialization of std::hash can be injected in namespace std.
template<>
struct std::hash<Period>
{
    std::size_t operator()(const Period& period) const noexcept
    {
		std::size_t seed = 0;

		hash_combine<long>(seed, period.year);
		hash_combine<long>(seed, period.step);
		hash_combine<char>(seed, period.periodicity);

		return seed;
    }
};
