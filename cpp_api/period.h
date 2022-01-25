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


struct Period
{
	PERIOD* c_period;

public:
	Period(const int year, const char periodicity, const int position)
	{
		int max_position;
		// check periodicity
		if (mPeriodicities.count(periodicity) == 0) throw std::runtime_error("Invalid periodicity " + std::string(1, periodicity) + 
			".\nPossible values for the periodicity are " + std::string(L_PERIOD_CH));
		// check position
		max_position = mPeriodicities.at(periodicity);
		if (position < 1 || position > max_position) throw std::runtime_error("Invalid position " + std::to_string(position) + 
			".\nValue of position argument must be in range [1, " + std::to_string(max_position) + "]");
		// initialize class members
		c_period = (PERIOD*) SW_nalloc(sizeof(PERIOD));
		c_period->p_y = year;
		c_period->p_p = periodicity;
		c_period->p_s = position;
	}

	/**
	 * same as PER_atoper() function
	 */
	Period(const std::string str_period)
	{ 
		c_period = PER_atoper(const_cast<char*>(str_period.c_str()));
		if (c_period == NULL) throw std::runtime_error("Cannot create a new Period from string " + str_period);
	}

	// We assume that the C period is valid (i.e. generated via the C API)
	// NOTE : making a copy of the passed C structure to avoid Heap errors when the
	//        destructor is called (the C structure may be already freed)
	Period(PERIOD* c_period) : c_period(c_period) 
	{
		this->c_period = (PERIOD*) SW_nalloc(sizeof(PERIOD));
		memcpy(this->c_period, c_period, sizeof(PERIOD));
	}

	~Period()
	{
		SW_nfree(c_period);
	}


	int nb_periods_per_year() const
	{
		return PER_nb(c_period->p_p);
	}

	/**
	 * same as PER_diff_per() function 
	 */
	int difference(const Period& other) const
	{
		PERIOD* c_other = other.c_period;
		if (c_other->p_p != c_period->p_p) 
			throw std::runtime_error("The two periods must share the same periodicity. Got " + std::string(1, c_period->p_p) + " and " + std::string(1, c_other->p_p));
		return PER_diff_per(c_period, c_other);
	}

	/**
	 * same as PER_addper() function
	 */
	Period shift(const int nb_periods)
	{
		// WARNING: PER_addper() returns a pointer to a STATIC structure
		PERIOD* static_period = PER_addper(c_period, nb_periods);
		// needed because the destructor of Period calls SW_nfree(c_period)
		PERIOD shifted_period;
		memcpy(&shifted_period, static_period, sizeof(PERIOD));
		return Period(&shifted_period);
	}

	/**
	 * same as PER_pertoa() function
	 */
	std::string to_string() const
	{
		char ch_period[10];
		PER_pertoa(c_period, ch_period);
		return std::string(ch_period);
	}

	bool operator==(const Period& other) const
	{ 
		PERIOD* c_other = other.c_period;
		return (c_period->p_y == c_other->p_y) && (c_period->p_p == c_other->p_p) && (c_period->p_s == c_other->p_s);
	}

	// TODO : implement operators > and < ?  
};
