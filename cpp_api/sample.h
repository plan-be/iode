#pragma once

#include "common.h"
#include "period.h"


struct Sample
{
	SAMPLE* c_sample;

public:
	Sample(const Period& start_period, const Period& end_period)
	{
		c_sample = PER_pertosmpl(start_period.c_period, end_period.c_period);
	}

	/**
     * same as PER_atosmpl() function
     */
	Sample(const std::string str_start_period, std::string str_end_period)
	{
		c_sample = PER_atosmpl(const_cast<char*>(str_start_period.c_str()), const_cast<char*>(str_end_period.c_str()));
		if (c_sample == NULL) throw std::runtime_error("Cannot create sample from periods " + str_start_period + " and " + str_end_period);
	}

	/**
	 * see PER_pertosmpl() for more details
	 */
	Sample(PERIOD* c_start_period, PERIOD* c_end_period)
	{
		c_sample = PER_pertosmpl(c_start_period, c_end_period);
	}

	// We assume that the C sample is valid (i.e. generated via the C API)
	// NOTE : making a copy of the passed C structure to avoid Heap errors when the
	//        destructor is called (the C structure may be already freed)
	Sample(SAMPLE* c_sample)
	{
		this->c_sample = (SAMPLE*) SW_nalloc(sizeof(SAMPLE));
		memcpy(this->c_sample, c_sample, sizeof(SAMPLE));
	}

	~Sample()
	{
		SW_nfree(c_sample);
	}

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
	Sample intersection(const Sample& other)
	{
		SAMPLE* c_other = other.c_sample;
		if (c_sample->s_p1.p_p != c_other->s_p1.p_p)
			throw std::runtime_error("The periodicity of the starting period of the two samples must be same. Got " + 
				std::to_string(c_sample->s_p1.p_p) + " vs " + std::to_string(c_other->s_p1.p_p));
		SAMPLE c_res;
		int res = PER_common_smpl(c_sample, c_other, &c_res);
		if (res < 0) throw std::runtime_error("Something went wrong when trying to compute the intersection between samples " + to_string() + " and " + other.to_string());
		return Sample(&c_res);
	}

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
