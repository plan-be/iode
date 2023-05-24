#include "period.h"


Period::Period()
{
    c_period = nullptr;
}

Period::Period(const Period& period)
{
    this->c_period = (PERIOD*) SW_nalloc(sizeof(PERIOD));
    memcpy(this->c_period, period.c_period, sizeof(PERIOD));
}

Period::Period(const int year, const char periodicity, const int position)
{
    IodeExceptionInvalidArguments error("Cannot create new Period");
    int max_position;
    // check periodicity
    if (mPeriodicities.count(periodicity) == 0)
    {
        error.add_argument("periodicity", std::string(1, periodicity) + 
                            " (possible values for the periodicity are " + std::string(L_PERIOD_CH) + ")");
        throw error;
    }
    // check position
    max_position = mPeriodicities.at(periodicity);
    if (position < 1 || position > max_position)
    {
        error.add_argument("position", std::to_string(position) + 
                            " (position argument must be in range [1, " + std::to_string(max_position) + "])");
        throw error;
    }
    // initialize class members
    c_period = (PERIOD*) SW_nalloc(sizeof(PERIOD));
    c_period->p_y = year;
    c_period->p_p = periodicity;
    c_period->p_s = position;
}

Period::Period(const std::string str_period)
{ 
    c_period = PER_atoper(to_char_array(str_period));
    if (c_period == NULL)
    {
        IodeExceptionInitialization error("Period", "Unknown"); 
        error.add_argument("period", str_period);
        throw error;
    } 
}

Period::Period(PERIOD* c_period) : c_period(c_period) 
{
    this->c_period = (PERIOD*) SW_nalloc(sizeof(PERIOD));
    memcpy(this->c_period, c_period, sizeof(PERIOD));
}

Period::~Period()
{
    SW_nfree(c_period);
}

int Period::difference(const Period& other) const
{
    PERIOD* c_other = other.c_period;
    if (c_other->p_p != c_period->p_p) 
    {
        IodeExceptionFunction error("Cannot calculate the difference between the periods " + 
            to_string() + " and " + other.to_string());
        error.set_reason("The two periods must share the same periodicity");
        error.add_argument("left  period periodicity", std::string(1, c_period->p_p));
        error.add_argument("right period periodicity", std::string(1, c_other->p_p));
        throw error;
    }
    return PER_diff_per(c_period, c_other);
}

Period Period::shift(const int nb_periods)
{
    // WARNING: PER_addper() returns a pointer to a STATIC structure
    PERIOD* static_period = PER_addper(c_period, nb_periods);
    // needed because the destructor of Period calls SW_nfree(c_period)
    PERIOD shifted_period;
    memcpy(&shifted_period, static_period, sizeof(PERIOD));
    return Period(&shifted_period);
}

std::size_t hash_value(PERIOD const& c_period)
{
    std::size_t seed = 0;

    boost::hash_combine(seed, c_period.p_y);
    boost::hash_combine(seed, c_period.p_s);
    boost::hash_combine(seed, c_period.p_p);

    return seed;
}
