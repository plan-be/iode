#include "pch.h"

TEST(TestSample, Create) 
{
	// passing 2 Period instances
	Period start(2015, 'Y', 1);
	Period end(2020, 'Y', 1);
	Sample sample(start, end);

	// passing 2 strings
	std::string str_start = start.to_string();
	std::string str_end = end.to_string();
	Sample sample2(str_start, str_end);

	// passing 2 Period pointers
	Period* ptr_start = new Period("2015Y1");
	Period* ptr_end = new Period("2020Y1");

	Sample sample3(*ptr_start, *ptr_end);
	
	delete ptr_start;
	delete ptr_end;

	// passing a Sample pointer
	Sample* ptr_sample = new Sample(start, end);

	Sample sample4(*ptr_sample);
	
	delete ptr_sample;
}

TEST(TestSample, GetPeriodPosition)
{
	Period start(2015, 'M', 1);
	Period end(2020, 'M', 6);
	Sample sample(start, end);
	int expected_pos = 18;

	// Period object
	Period period = sample.start_period.shift(expected_pos);
	EXPECT_EQ(sample.get_period_position(period), expected_pos);

	// string
	std::string s_period = period.to_string();
	EXPECT_EQ(sample.get_period_position(s_period), expected_pos);
}

TEST(TestSample, Intersection) 
{
	Period start1(2015, 'M', 1);
	Period end1(2020, 'M', 6);
	Sample sample1(start1, end1);

	Period start2(2017, 'M', 2);
	Period end2(2022, 'M', 1);
	Sample sample2(start2, end2);

	Sample sample_intersection = sample1.intersection(sample2);

	Period inter_start_period(2017, 'M', 2);
	Period inter_end_period(2020, 'M', 6);
	int nb_periods = inter_end_period.difference(inter_start_period) + 1;

	EXPECT_EQ(sample_intersection.start_period, inter_start_period);
	EXPECT_EQ(sample_intersection.end_period, inter_end_period);
	EXPECT_EQ(sample_intersection.nb_periods, nb_periods);
}

TEST(TestSample, ToString)
{
	Period start(2015, 'Y', 1);
	Period end(2020, 'Y', 1);
	Sample sample(start, end);

	std::string str_sample = sample.to_string();
	EXPECT_EQ(str_sample, start.to_string() + ':' + end.to_string());
}

TEST(TestSample, Hash)
{
	std::size_t hash_before;
	std::size_t hash_after;

	Period start(2015, 'Y', 1);
	Period end(2020, 'Y', 1);
	Sample sample(start, end);

	std::hash<Sample> sample_hasher;
    hash_before = sample_hasher(sample);

	// different starting year
	Period earlier_start(2014, 'Y', 1);
	Sample earlier_start_sample(earlier_start, end);
	hash_after = sample_hasher(earlier_start_sample);
	EXPECT_NE(hash_before, hash_after);

	// different periodicity
	Period month_start(2015, 'M', 1);
	Period month_end(2020, 'M', 1);
	Sample month_sample(month_start, month_end);
	hash_after = sample_hasher(month_sample);
	EXPECT_NE(hash_before, hash_after);

	// different position
	Period later_month_start(2015, 'M', 3);
	Sample later_start_sample(later_month_start, month_end);
	hash_after = sample_hasher(later_start_sample);
	EXPECT_NE(hash_before, hash_after);
}
