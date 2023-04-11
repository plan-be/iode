#include "sample.h"


Sample::Sample()
{
    c_sample = nullptr;
}

Sample::Sample(const Period& start_period, const Period& end_period)
{
    c_sample = PER_pertosmpl(start_period.c_period, end_period.c_period);
}

Sample::Sample(const std::string str_start_period, std::string str_end_period)
{
    c_sample = PER_atosmpl(to_char_array(str_start_period), to_char_array(str_end_period));
    if (c_sample == NULL)
    {
        IodeExceptionInitialization error("Sample", "Unknown");
        error.add_argument("start period", str_start_period);
        error.add_argument("end   period", str_end_period);
        throw error;
    }
}

Sample::Sample(PERIOD* c_start_period, PERIOD* c_end_period)
{
    c_sample = PER_pertosmpl(c_start_period, c_end_period);
}

Sample::Sample(SAMPLE* c_sample)
{
    if(c_sample == NULL) 
        throw IodeExceptionInitialization("Sample", "Sample not yet initialized");
    this->c_sample = (SAMPLE*) SW_nalloc(sizeof(SAMPLE));
    memcpy(this->c_sample, c_sample, sizeof(SAMPLE));
}

Sample::~Sample()
{
    SW_nfree(c_sample);
}

Sample Sample::intersection(const Sample& other)
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
