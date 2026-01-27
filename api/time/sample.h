#pragma once
#include "api/utils/utils.h"
#include "api/time/period.h"
#include <stdexcept>


struct Sample
{
    Period  start_period;     	// starting period
    Period  end_period;       	// ending period
    short   nb_periods;       	// number of periods in the sample
    // char    s_pad[2];

private:
	void sample_from_periods(const Period& start_period, const Period& end_period)
	{
        int diff = end_period.difference(start_period);
        if(diff < 0)
        {
            std::string error_msg = "Cannot create a new sample from the start period '";
            error_msg += start_period.to_string() + "' and end period '";
            error_msg += end_period.to_string() + "'.\nEnd period must be after the start period";
            throw std::invalid_argument(error_msg);
        }

        this->start_period = start_period;
		this->end_period = end_period;
        nb_periods = diff + 1;
	}

public:
	Sample()
	{
		this->nb_periods = 0;
	}

	Sample(const Sample& other)
	{
		this->start_period = other.start_period;
		this->end_period = other.end_period;
		this->nb_periods = other.nb_periods; 
	}

	Sample(const Period& start_period, const Period& end_period)
	{
		sample_from_periods(start_period, end_period);
	}

	Sample(const std::string& str_start_period, const std::string& str_end_period)
	{
		std::string error_msg = "Cannot create a new sample from the start period '" + str_start_period + 
								"' and end period '" + str_end_period + "'.\n";

		Period* start_period;
		Period* end_period;
		try
		{
			start_period = new Period(str_start_period);
			end_period = new Period(str_end_period);
		}
		catch(const std::exception& e)
		{
			throw std::invalid_argument(error_msg + std::string(e.what()));
		}
		
		sample_from_periods(*start_period, *end_period);
		delete start_period;
		delete end_period;
		start_period = nullptr;
		end_period = nullptr;
	}

	int get_period_position(const Period& period) const
	{
		int t = period.difference(this->start_period);
		if(t < 0 || t >= this->nb_periods)
			throw std::out_of_range("The period '" + period.to_string() + 
				                    "' is not in the sample '" + to_string() + "'");
		return t;
	}

	int get_period_position(const std::string& period) const
	{
		return get_period_position(Period(period));
	}

	std::vector<std::string> get_list_periods() const
	{
		std::vector<std::string> periods;
		periods.reserve(this->nb_periods);
		Period period = this->start_period;
		for(int t = 0; t < this->nb_periods; t++)
			periods.push_back(period.shift(t).to_string());
		return periods;
	}

	std::vector<float> get_list_periods_as_float() const
	{
		std::vector<float> periods;
		periods.reserve(this->nb_periods);
		Period period = this->start_period;
		for(int t = 0; t < this->nb_periods; t++)
			periods.push_back(period.shift(t).to_float());
		return periods;
	}

	Sample intersection(const Sample& other) const
	{
		std::string error_msg = "Cannot calculate the intersection between the samples '" + 
								to_string() + "' and '" + other.to_string() + "'.\n";

		if (start_period.periodicity != other.start_period.periodicity)
			throw std::invalid_argument(error_msg + "The periodicity of the two samples must be same");

		Period this_start_period = this->start_period;
		Period other_start_period = other.start_period;
		// lower bound: if this_period < other_period -> other_period else this_period
		Period res_start_period = (this_start_period.difference(other_start_period) < 0) ? other_start_period : this_start_period; 

		Period this_end_period = this->end_period;
		Period other_end_period = other.end_period;
		// upper bound: if this_period > other_period -> other_period else this_period
		Period res_end_period = (this_end_period.difference(other_end_period) > 0) ? other_end_period : this_end_period; 

		return Sample(res_start_period, res_end_period);
	}

	std::string to_string() const
	{
		return Period(start_period).to_string() + ":" + Period(end_period).to_string();
	}

	bool operator==(const Sample& other) const
	{
		return (this->start_period == other.start_period) && 
		       (this->end_period == other.end_period) && 
			   (this->nb_periods == other.nb_periods);
	}
};


// Custom specialization of std::hash can be injected in namespace std.
template<>
struct std::hash<Sample>
{
    std::size_t operator()(const Sample& sample) const noexcept
    {
		std::size_t seed = 0;

		hash_combine<Period>(seed, sample.start_period);
		hash_combine<Period>(seed, sample.end_period);
		hash_combine<short>(seed, sample.nb_periods);

		return seed;
    }
};
