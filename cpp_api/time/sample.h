#pragma once
#include "period.h"
#include <stdexcept>


struct Sample : public SAMPLE
{
private:
	void sample_from_periods(const Period& start_period, const Period& end_period);

public:
	Sample();

	Sample(const Sample& sample);

	Sample(const SAMPLE& c_sample);

	/**
	 * @note see PER_pertosmpl() for more details
	 */
	Sample(const Period& start_period, const Period& end_period);

	/**
     * @note same as PER_atosmpl() function
     */
	Sample(const std::string str_start_period, const std::string str_end_period);

	Period start_period() const
	{
		return Period(s_p1);
	}

	Period end_period() const
	{
		return Period(s_p2);
	}

	int nb_periods() const
	{
		return s_nb;
	}

	int get_period_position(const Period& period) const
	{
		int t = period.difference(start_period());
		if(t < 0 || t >= nb_periods())
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
	 * @note same as PER_common_smpl() function
	 */
	Sample intersection(const Sample& other);

	/**
	 * @note same as PER_smpltoa() function
	 */
	std::string to_string() const
	{
		return Period(s_p1).to_string() + ":" + Period(s_p2).to_string();
	}

	bool operator==(const Sample& other) const
	{
		return (start_period() == other.start_period()) && 
		       (end_period() == other.end_period()) && 
			   (nb_periods() == other.nb_periods());
	}
};


// Custom specialization of std::hash can be injected in namespace std.
template<>
struct std::hash<SAMPLE>
{
    std::size_t operator()(const SAMPLE& sample) const noexcept
    {
		std::size_t seed = 0;

		hash_combine<PERIOD>(seed, sample.s_p1);
		hash_combine<PERIOD>(seed, sample.s_p2);
		hash_combine<short>(seed, sample.s_nb);

		return seed;
    }
};
