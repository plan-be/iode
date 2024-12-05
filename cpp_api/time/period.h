#pragma once

#include <ctype.h>
#include "cpp_api/common.h"
#include "cpp_api/utils/utils.h"


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

/**
 *  Gives the nbr of periods in one year for the periodicity ch.
 *  
 *  @param ch periodicity. Can be upper or lowercase.
 *  @return number of periods for the periodicity ch.
 *  
 *  @note same function as PER_nb
 */
inline int nb_periods_per_year(const char ch)
{
	if(ch == 0)
	{
		kwarning("Cannot get the number of periods per year:\nEmpty periodicity");
		return 0;
	}

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
	{
		kwarning("Cannot convert the periodicity to float:\nEmpty periodicity");
		return 0.f;
	}

	char upper_ch = toupper(ch);
	if(!map_steps.contains(upper_ch))
		throw std::invalid_argument("Invalid periodicity '" + std::string(1, ch) + "'.\n" +
									"Possible values are '" + periodicities + "'");
	return map_steps.at(upper_ch);
}


struct Period: public PERIOD
{
public:
	Period();

	Period(const Period& other);

	Period(const PERIOD& other);

	Period(const int year, const char periodicity, const int step);

	/**
	 * @note same as PER_atoper() function
	 */
	Period(const std::string str_period);

	int nb_periods_per_year() const
	{
		return ::nb_periods_per_year(p_p);
	}

	/**
	 * @note same as PER_diff_per() function 
	 */
	int difference(const Period& other) const;

	/**
	 * @note same as PER_addper() function
	 */
	Period shift(const int nb_periods);
	
	/**
	 * @note same as PER_pertoa() function
	 */
	std::string to_string() const
	{
		if(p_y == 0 || p_p == 0 || p_s == 0)
			return "";
		else
			// <year><periodicity><step>
			return std::to_string(p_y) + std::string(1, p_p) + std::to_string(p_s);
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
			float value = static_cast<float>(p_y);
			if(p_p != 'Y') 
				value += get_step(p_p) * (p_s - 1);
			return value;
	}

	bool operator==(const Period& other) const
	{ 
		return (p_y == other.p_y) && (p_p == other.p_p) && (p_s == other.p_s);
	}

	// TODO : implement operators > and < ? 
	 
};


// Custom specialization of std::hash can be injected in namespace std.
template<>
struct std::hash<PERIOD>
{
    std::size_t operator()(const PERIOD& period) const noexcept
    {
		std::size_t seed = 0;

		hash_combine<long>(seed, period.p_y);
		hash_combine<long>(seed, period.p_s);
		hash_combine<char>(seed, period.p_p);

		return seed;
    }
};