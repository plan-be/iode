#pragma once
#include "period.h"


struct Sample
{
	SAMPLE* c_sample;

public:
	Sample();

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
		return Period(&c_sample->s_p1);
	}

	Period end_period() const
	{
		return Period(&c_sample->s_p2);
	}

	int nb_periods()
	{
		return c_sample->s_nb;
	}

	int get_period_position(const Period& period)
	{
		return period.difference(start_period());
	}

	int get_period_position(const std::string& period)
	{
		return get_period_position(Period(period));
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
