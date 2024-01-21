#include "period.h"


Period::Period()
{
    p_y = 0;
    p_p = 0;
    p_s = 0;
}

Period::Period(const Period& other)
{
    p_y = other.p_y;
    p_p = other.p_p;
    p_s = other.p_s;
}

Period::Period(const PERIOD& other)
{
    p_y = other.p_y;
    p_p = other.p_p;
    p_s = other.p_s;
}

Period::Period(const int year, const char periodicity, const int step)
{
    std::string error_msg = "Cannot create a new period '" + std::to_string(year) + 
        std::string(1, periodicity) + std::to_string(step) + "'\n";

    // check periodicity
    int nb_periods;
    try
    {
        nb_periods = ::nb_periods_per_year(periodicity);
    }
    catch(const std::exception& e)
    {
        throw std::invalid_argument(error_msg + std::string(e.what()));
    }
    
    // check step
    if (step < 1 || step > nb_periods)
        throw std::invalid_argument(error_msg + "Period step must be in range [1, " + std::to_string(nb_periods) + "])");
    
    // initialize class members
    p_y = year;
    p_p = periodicity;
    p_s = step;
}

Period::Period(const std::string str_period)
{
    p_y = 0;
    p_p = 0;
    p_s = 0;

    if(str_period.empty())
    {
        kwarning("String for creating a period is empty");
        return;
    }

    std::string error_msg = "Cannot create a period from the string '" + str_period + "'\n";

    // Search the position of the periodicity character
    int i;
    for(i = 0 ; i < 4 ; i++)
        if(!isdigit(str_period[i])) 
            break;

    if(i == 0)
        throw std::invalid_argument(error_msg);

    // Check that the periodicity is valid
    char periodicity = toupper(str_period[i]);
    int max_step; 
    try
    {
        max_step = ::nb_periods_per_year(periodicity);
    }
    catch(const std::exception& e)
    {
        throw std::invalid_argument(error_msg + std::string(e.what()));
    }
    p_p = periodicity;
    
    // Retrieve the year (first digits). 
    // If year < 50, adds 2000 (49Y1 => 2049Y1)
    // if year < 200, adds 1900 (60Y1 => 1960Y1)
    // NOTE: std::stoi() -> If no conversion could be performed, 
    //       an invalid_argument exception is thrown.
    try
    {    
        int year = std::stoi(str_period.substr(0, i));
        if(year < 50) 
            year += 2000;
        else if(year < 200) 
            year += 1900;
        p_y = year;
    }
    catch(const std::exception& e)
    {
        throw std::invalid_argument(error_msg + std::string(e.what()));
    }

    // calculate sub period
    int step;
    try
    {
        step = (str_period.size() == i) ? 1 : std::stoi(str_period.substr(i+1));
    }
    catch(const std::exception& e)
    {
        throw std::invalid_argument(error_msg + std::string(e.what()));
    }

    if(step > max_step)
        throw std::invalid_argument(error_msg + "The maximum position in the year for the periodicity '" + 
                                    std::to_string(periodicity) + "' is " + std::to_string(max_step));
    p_s = step;
}

int Period::difference(const Period& other) const
{   
    if (p_p != other.p_p) 
    {
        std::string error_msg = "Cannot calculate the difference between the periods '" + 
                                to_string() + "' and '" + other.to_string() + "'\n";
        error_msg += "The two periods must share the same periodicity:\n";
        error_msg += "left period periodicity :" + std::string(1, p_p) + "\n";
        error_msg += "right period periodicity:" + std::string(1, other.p_p);
        throw std::runtime_error(error_msg);
    }

    return (nb_periods_per_year() * (p_y - other.p_y)) + (p_s - other.p_s);
}

Period Period::shift(const int nb_periods)
{
    int nb_periods_per_year_ = nb_periods_per_year();
    int delta_years;

    Period shifted_period(*this);

    // NOTE: - first step of the year is 1, not 0 !
    //       - step is in range [1, nb periods per year]

    // go forward
    if(nb_periods >= 0) 
    {
        int sum_steps = shifted_period.p_s + nb_periods;
        delta_years = (sum_steps - 1) / nb_periods_per_year_;

        shifted_period.p_y += delta_years;
        shifted_period.p_s = 1 + (sum_steps - 1) % nb_periods_per_year_;
    }
    // go backward
    else 
    {
        int shift_steps = shifted_period.p_s + nb_periods;
        if(shift_steps <= 0) 
        {
            delta_years = 1 + (-shift_steps) / nb_periods_per_year_;

            shifted_period.p_y -= delta_years;
            shifted_period.p_s = delta_years * nb_periods_per_year_ + shift_steps;
        }
        else 
            shifted_period.p_s = shift_steps;
    }

    return shifted_period;
}

std::size_t hash_value(PERIOD const& c_period)
{
    std::size_t seed = 0;

    boost::hash_combine(seed, c_period.p_y);
    boost::hash_combine(seed, c_period.p_s);
    boost::hash_combine(seed, c_period.p_p);

    return seed;
}

std::size_t hash_value(Period const& period)
{
    std::size_t seed = 0;

    boost::hash_combine(seed, period.p_y);
    boost::hash_combine(seed, period.p_s);
    boost::hash_combine(seed, period.p_p);

    return seed;
}
