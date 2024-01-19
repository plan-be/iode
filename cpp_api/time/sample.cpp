#include "sample.h"


Sample::Sample()
{
    s_p1 = PERIOD();
    s_p2 = PERIOD();
    s_nb = 0;
}

Sample::Sample(const Sample& sample)
{
    memcpy(&s_p1, &(sample.s_p1), sizeof(PERIOD));
    memcpy(&s_p2, &(sample.s_p2), sizeof(PERIOD));
    s_nb = sample.s_nb; 
}

Sample::Sample(const SAMPLE& c_sample)
{
    memcpy(&s_p1, &(c_sample.s_p1), sizeof(PERIOD));
    memcpy(&s_p2, &(c_sample.s_p2), sizeof(PERIOD));
    s_nb = c_sample.s_nb; 
}

void Sample::sample_from_periods(const Period& start_period, const Period& end_period)
{
        int diff = end_period.difference(start_period);
        if(diff < 0)
        {
            std::string error_msg = "Cannot create a new sample from the start period '";
            error_msg += start_period.to_string() + "' and end period '";
            error_msg += end_period.to_string() + "'.\nEnd period must be after the start period";
            throw std::invalid_argument(error_msg);
        }

        memcpy(&s_p1, &start_period, sizeof(PERIOD));
        memcpy(&s_p2, &end_period, sizeof(PERIOD));
        s_nb = diff + 1;
}

Sample::Sample(const Period& start_period, const Period& end_period)
{
    sample_from_periods(start_period, end_period);
}

Sample::Sample(const std::string str_start_period, std::string str_end_period)
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
}

Sample Sample::intersection(const Sample& other)
{
    std::string error_msg = "Cannot calculate the intersection between the samples '" + 
                            to_string() + "' and '" + other.to_string() + "'.\n";

    if (s_p1.p_p != other.s_p1.p_p)
        throw std::invalid_argument(error_msg + "The periodicity of the two samples must be same");

    Period this_start_period = start_period();
    Period other_start_period = other.start_period();
    // lower bound: if this_period < other_period -> other_period else this_period
    Period res_start_period = (this_start_period.difference(other_start_period) < 0) ? other_start_period : this_start_period; 

    Period this_end_period = end_period();
    Period other_end_period = other.end_period();
    // upper bound: if this_period > other_period -> other_period else this_period
    Period res_end_period = (this_end_period.difference(other_end_period) > 0) ? other_end_period : this_end_period; 

    return Sample(res_start_period, res_end_period);
}

std::size_t hash_value(SAMPLE const& c_sample)
{
    std::size_t seed = 0;

    boost::hash_combine(seed, c_sample.s_p1);
    boost::hash_combine(seed, c_sample.s_p2);
    boost::hash_combine(seed, c_sample.s_nb);

    return seed;
}

std::size_t hash_value(Sample const& sample)
{
    std::size_t seed = 0;

    boost::hash_combine(seed, sample.s_p1);
    boost::hash_combine(seed, sample.s_p2);
    boost::hash_combine(seed, sample.s_nb);

    return seed;
}
