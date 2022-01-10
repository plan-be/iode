#pragma once

#include "common.h"
#include <stdexcept>


const static std::map<char, int> mPeriodicities =
{
	{'Y', 1},
	{'S', 2},
	{'Q', 4},
	{'M', 12},
	{'W', 52},
	{'D', 365}
};


/**
 * same as PER_nb() function
 */ 
inline int nb_periods_per_year(char periodicity)
{
	int nb_per;
	periodicity = (char) toupper(periodicity);
	try 
	{
		nb_per = mPeriodicities.at(periodicity);
	}
	catch (std::out_of_range)
	{
		nb_per = -1;
	}
	return nb_per;
}


struct Period
{
	long year;
	long position;	// position in the year
	char periodicity;

private:
	/**
     * see PER_atoper() function for details
     */
	void set_from_string(const std::string str_period)
	{
		PERIOD* new_period = PER_atoper(const_cast<char*>(str_period.c_str()));
		if (new_period == NULL) throw std::runtime_error("Cannot create a new Period from string " + str_period);
		year = new_period->p_y;
		position = new_period->p_s;
		periodicity = new_period->p_p;
		SW_nfree(new_period);
	}

public:
	Period(const int year, const int position, const char periodicity)
	{
		std::string str_period = std::to_string(year) + periodicity + std::to_string(position);
		set_from_string(str_period);
	}

	Period(const std::string str_period)
	{ 
		set_from_string(str_period);
	}

	// We assume that the C period is valid (i.e. generated via the C API)
	Period(const PERIOD* c_period) : year(c_period->p_y), position(c_period->p_s), periodicity(c_period->p_p) {};

	PERIOD to_c_period() const
	{
		PERIOD period;
		period.p_y = year;
		period.p_s = position;
		period.p_p = periodicity;
		return period;
	}

	int nb_periods_per_year() const
	{
		return ::nb_periods_per_year(periodicity);
	}

	/**
	 * same as PER_diff_per() function 
	 */
	int difference(const Period& other) const
	{
		if (other.periodicity != periodicity) 
			throw std::runtime_error("The two periods must share the same periodicity. Got " + std::to_string(periodicity) + " and " + std::to_string(other.periodicity));
		PERIOD c_period = to_c_period();
		PERIOD c_period2 = other.to_c_period();
		return PER_diff_per(&c_period, &c_period2);
	}

	/**
	 * same as PER_addper() function
	 */
	Period shift(const int nb_periods)
	{
		PERIOD c_period = to_c_period();
		// not calling SW_nfree() on new_period because PER_addper() returns a pointer to a STATIC structure
		PERIOD* new_period = PER_addper(&c_period, nb_periods);
		return Period(new_period);
	}

	/**
	 * same as PER_pertoa() function
	 */
	std::string to_string() const
	{
		PERIOD c_period = to_c_period();
		char ch_period[10];
		PER_pertoa(&c_period, ch_period);
		return std::string(ch_period);
	}

	bool operator==(const Period& other) const
	{ 
		return (year == other.year) && (position == other.position) && (periodicity == other.periodicity);
	}

	// TODO : implement operators > and < ?  
};
