#pragma once
#include "period.h"
#include <stdexcept>
#include <boost/functional/hash.hpp>


struct Sample
{
	SAMPLE* c_sample;

public:
	Sample();

	Sample(const Sample& sample);

	Sample(const Period& start_period, const Period& end_period);

	/**
     * same as PER_atosmpl() function
     */
	Sample(const std::string str_start_period, std::string str_end_period);

	/**
	 * see PER_pertosmpl() for more details
	 */
	Sample(PERIOD* c_start_period, PERIOD* c_end_period);

	// We assume that the C sample is valid (i.e. generated via the C API)
	// NOTE : making a copy of the passed C structure to avoid Heap errors when the
	//        destructor is called (the C structure may be already freed)
	Sample(SAMPLE* c_sample);

	~Sample();

	Period start_period() const
	{
		return Period(c_sample->s_p1);
	}

	Period end_period() const
	{
		return Period(c_sample->s_p2);
	}

	int nb_periods() const
	{
		return c_sample->s_nb;
	}

	int get_period_position(const Period& period) const
	{
		int t = period.difference(start_period());
		if(t < 0 || t >= nb_periods())
			throw std::out_of_range("The period " + period.to_string() + 
				" is not in the sample " + to_string());
		return t;
	}

	int get_period_position(const std::string& period) const
	{
		return get_period_position(Period(period));
	}

	std::vector<std::string> get_list_periods() const
	{
		std::vector<std::string> periods;
		periods.reserve(nb_periods());
		Period period = start_period();
		for(int t = 0; t < nb_periods(); t++)
			periods.push_back(period.shift(t).to_string());
		return periods;
	}

	std::vector<float> get_list_periods_as_float() const
	{
		std::vector<float> periods;
		periods.reserve(nb_periods());
		Period period = start_period();
		for(int t = 0; t < nb_periods(); t++)
			periods.push_back(period.shift(t).to_float());
		return periods;
	}

	/**
	 * same as PER_common_smpl() function
	 */
	Sample intersection(const Sample& other);

	std::string to_string() const
	{
		char ch_sample[20];
		PER_smpltoa(c_sample, ch_sample);
		return std::string(ch_sample);
	}

	bool operator==(const Sample& other) const
	{
		SAMPLE* c_other = other.c_sample;
		return (start_period() == other.start_period()) && (end_period() == other.end_period()) && (c_sample->s_nb == c_other->s_nb);
	}
};

/**
 * @brief compute a hash value for a sample.
 * 
 * @note see https://www.boost.org/doc/libs/1_55_0/doc/html/hash/custom.html
 *       and https://www.boost.org/doc/libs/1_55_0/doc/html/hash/combine.html
 * 
 * @return std::size_t 
 */
std::size_t hash_value(SAMPLE const& c_sample);
