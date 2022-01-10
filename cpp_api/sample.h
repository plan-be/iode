#pragma once

#include "common.h"
#include "period.h"


struct Sample
{
	Period* start_period;
	Period* end_period;
	int nb_periods;

private:
	/**
     * same as PER_atosmpl() function
     */
	void set_from_string(const std::string str_start_period, std::string str_end_period)
	{
		SAMPLE* new_sample = PER_atosmpl(const_cast<char*>(str_start_period.c_str()), const_cast<char*>(str_end_period.c_str()));
		if (new_sample == NULL) throw std::runtime_error("Cannot create sample from periods " + str_start_period + " and " + str_end_period);
		start_period = new Period(&new_sample->s_p1);
		end_period = new Period(&new_sample->s_p2);
		nb_periods = new_sample->s_nb;
		SW_nfree(new_sample);
	}

public:
	Sample(const Period& start_period, const Period& end_period)
	{
		std::string str_start_period = start_period.to_string();
		std::string str_end_period = end_period.to_string();
		set_from_string(str_start_period, str_end_period);
	}

	Sample(const std::string str_start_period, std::string str_end_period)
	{
		set_from_string(str_start_period, str_end_period);
	}

	/**
	 * see PER_pertosmpl() for more details
	 */
	Sample(const PERIOD* c_start_period, const PERIOD* c_end_period)
	{
		SAMPLE* new_sample = PER_pertosmpl(const_cast<PERIOD*>(c_start_period), const_cast<PERIOD*>(c_end_period));
		if (new_sample == NULL) throw std::runtime_error("Cannot create sample from periods " + Period(c_start_period).to_string() +" and " + Period(c_end_period).to_string());
		start_period = new Period(&new_sample->s_p1);
		end_period = new Period(&new_sample->s_p2);
		nb_periods = new_sample->s_nb;
		SW_nfree(new_sample);
	}

	// We assume that the C sample is valid (i.e. generated via the C API)
	Sample(const SAMPLE* c_sample)
	{
		start_period = new Period(&c_sample->s_p1);
		end_period = new Period(&c_sample->s_p2);
		nb_periods = c_sample->s_nb;
	}

	~Sample()
	{
		delete start_period;
		delete end_period;
	}

	SAMPLE to_c_sample() const
	{
		SAMPLE sample;
		sample.s_p1 = start_period->to_c_period();
		sample.s_p2 = end_period->to_c_period();
		sample.s_nb = (short) nb_periods;
		return sample;
	}

	/**
	 * same as PER_common_smpl() function
	 */
	Sample intersection(const Sample& other)
	{
		if (start_period->periodicity != other.start_period->periodicity)
			throw std::runtime_error("The periodicity of the starting period of the two samples must be same. Got " + 
				std::to_string(start_period->periodicity) + " vs " + std::to_string(other.start_period->periodicity));
		SAMPLE c_res;
		SAMPLE c_sample = to_c_sample();
		SAMPLE c_other = other.to_c_sample();
		int res = PER_common_smpl(&c_sample, &c_other, &c_res);
		if (res < 0) throw std::runtime_error("Something went wrong when trying to compute the intersection between samples " + to_string() + " and " + other.to_string());
		return Sample(&c_res);
	}

	std::string to_string() const
	{
		SAMPLE c_sample = to_c_sample();
		char ch_sample[20];
		PER_smpltoa(&c_sample, ch_sample);
		return std::string(ch_sample);
	}

	bool operator==(const Sample& other) const
	{
		return (*start_period == *other.start_period) && (*end_period == *other.end_period) && (nb_periods == other.nb_periods);
	}
};
