#pragma once

#include "common.h"
#include "utils/utils.h"
#include "utils/iode_exceptions.h"
#include <boost/functional/hash.hpp>


const static std::map<char, int> mPeriodicities =
{
	{'Y', 1},
	{'S', 2},
	{'Q', 4},
	{'M', 12},
	{'W', 52},
	{'D', 365}
};

const static std::map<char, double> mSteps =
{
	{'Y', 0.},
	{'S', 1./2.},
	{'Q', 1./4.},
	{'M', 1./12.},
	{'W', 1./52.},
	{'D', 1./365.}
};


struct Period
{
	PERIOD* c_period;

public:
	Period();

	Period(const Period& period);

	Period(const int year, const char periodicity, const int position);

	/**
	 * same as PER_atoper() function
	 */
	Period(const std::string str_period);

	// We assume that the C period is valid (i.e. generated via the C API)
	// NOTE : making a copy of the passed C structure to avoid Heap errors when the
	//        destructor is called (the C structure may be already freed)
	Period(PERIOD* c_period);

	~Period();

	int nb_periods_per_year() const
	{
		return PER_nb(c_period->p_p);
	}

	/**
	 * same as PER_diff_per() function 
	 */
	int difference(const Period& other) const;

	/**
	 * same as PER_addper() function
	 */
	Period shift(const int nb_periods);
	
	/**
	 * same as PER_pertoa() function
	 */
	std::string to_string() const
	{
		char ch_period[10];
		PER_pertoa(c_period, ch_period);
		return std::string(ch_period);
	}

	/**
	 * @brief transforms a period into a double value.
	 *        For examples:
	 *        2000Y1    ->    2000.0
	 *        2000Q2    ->    2000.25
	 *        2000M4    ->    2000.25
	 * 
	 * @return double 
	 */
	double to_double() const
	{
		try
		{
			double value = static_cast<double>(c_period->p_y);
			if(c_period->p_p != 'Y') value += mSteps.at(c_period->p_p) * (c_period->p_s - 1);
			return value;
		}
		catch(const std::exception)
		{
			throw IodeExceptionFunction("Invalid periodicity " + std::to_string(c_period->p_p) + ".\n" + 
				"Possible values for the periodicity are " + std::string(L_PERIOD_CH) + ")");
		}
	}

	bool operator==(const Period& other) const
	{ 
		PERIOD* c_other = other.c_period;
		return (c_period->p_y == c_other->p_y) && (c_period->p_p == c_other->p_p) && (c_period->p_s == c_other->p_s);
	}

	// TODO : implement operators > and < ?  
};

/**
 * @brief compute a hash value for a period.
 * 
 * @note see https://www.boost.org/doc/libs/1_55_0/doc/html/hash/custom.html
 *       and https://www.boost.org/doc/libs/1_55_0/doc/html/hash/combine.html
 * 
 * @return std::size_t 
 */
std::size_t hash_value(PERIOD const& c_period);
