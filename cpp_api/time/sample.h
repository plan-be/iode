#pragma once
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
		if (c_sample == NULL)
		{
			IodeExceptionInitialization error("Sample", "Unknown");
			error.add_argument("start period", str_start_period);
			error.add_argument("end   period", str_end_period);
			throw error;
		}
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
		IodeExceptionFunction error("Cannot calculate the intersection between the samples " + 
			to_string() + " and " + other.to_string());

		SAMPLE* c_other = other.c_sample;
		if (c_sample->s_p1.p_p != c_other->s_p1.p_p)
		{
			error.set_reason("The periodicity of the starting period of the two samples must be same");
			throw error;
		}
		SAMPLE c_res;
		int res = PER_common_smpl(c_sample, c_other, &c_res);
		if (res < 0)
		{
			error.set_reason("Unknown");
			throw error;
		}
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
