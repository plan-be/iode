#include "pch.h"
#include "../../cpp_api/iode_cpp_api.h"


TEST(TestSample, Create) 
{
	// passing 2 Period instances
	Period start(2015, 1, 'Y');
	Period end(2020, 1, 'Y');
	Sample sample(start, end);

	// passing 2 strings
	std::string str_start = start.to_string();
	std::string str_end = end.to_string();
	Sample sample2(str_start, str_end);

	// passing 2 C PERIOD structures
	PERIOD c_start = start.to_c_period();
	PERIOD c_end = end.to_c_period();
	Sample sample3(&c_start, &c_end);

	// passing a C SAMPLE structure
	SAMPLE* c_sample = PER_pertosmpl(&c_start, &c_end);
	Sample sample4(c_sample);
}

TEST(TestSample, To_c_sample) 
{
	Period start(2015, 1, 'Y');
	Period end(2020, 1, 'Y');
	Sample sample(start, end);

	SAMPLE c_sample = sample.to_c_sample();

	PERIOD c_start = start.to_c_period();
	PERIOD c_end = end.to_c_period();
	EXPECT_EQ(Period(&c_sample.s_p1), start);
	EXPECT_EQ(Period(&c_sample.s_p2), end);
	EXPECT_EQ(c_sample.s_nb, sample.nb_periods);
}

TEST(TestSample, Intersection) 
{
	Period start1(2015, 1, 'M');
	Period end1(2020, 6, 'M');
	Sample sample1(start1, end1);

	Period start2(2017, 2, 'M');
	Period end2(2022, 1, 'M');
	Sample sample2(start2, end2);

	Sample sample_intersection = sample1.intersection(sample2);

	Period inter_start_period(2017, 2, 'M');
	Period inter_end_period(2020, 6, 'M');
	int nb_periods = inter_end_period.difference(inter_start_period) + 1;

	EXPECT_EQ(*sample_intersection.start_period, inter_start_period);
	EXPECT_EQ(*sample_intersection.end_period, inter_end_period);
	EXPECT_EQ(sample_intersection.nb_periods, nb_periods);
}

TEST(TestSample, ToString)
{
	Period start(2015, 1, 'Y');
	Period end(2020, 1, 'Y');
	Sample sample(start, end);

	std::string str_sample = sample.to_string();
	EXPECT_EQ(str_sample, start.to_string() + ':' + end.to_string());
}
