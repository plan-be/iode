#include "pch.h"

TEST(TestPeriod, Create) 
{
	// passing year, position in the year and periodicity
	Period period(2020, 'Y', 1);

	// Invalid periodicity
	EXPECT_THROW(Period(2020, 'T', 1), std::invalid_argument);
	// Invalid position
	EXPECT_THROW(Period(2020, 'Q', 5), std::invalid_argument);

	// passing a string
	Period period2("2020Y1");

	// using a Period pointer
	std::string buf = "2020Y1";
	Period* ptr_period = new Period(buf);
	Period period3(*ptr_period);
	delete ptr_period;
}

TEST(TestPeriod, NbPeriodsPerYear)
{
	for (auto const& [periodicity, expected_nb_periods] : map_periodicities)
	{
		Period period(2020, periodicity, 1);
		int nb_periods = get_nb_periods_per_year(period.periodicity);
		EXPECT_EQ(nb_periods, expected_nb_periods);
	}
}

TEST(TestPeriod, Difference)
{
	int nb_periods;
	int year1 = 2015;
	int year2 = 2020;

	// periodicity = Y
	Period periodY1(year1, 'Y', 1);
	Period periodY2(year2, 'Y', 1);
	nb_periods = periodY2.difference(periodY1);
	EXPECT_EQ(nb_periods, 5);

	// periodicity = Q
	Period periodQ1(year1, 'Q', 2);
	Period periodQ2(year2, 'Q', 3);
	nb_periods = periodQ2.difference(periodQ1);
	EXPECT_EQ(nb_periods, 21);

	// periodicity = W
	Period periodW1(year1, 'W', 10);
	Period periodW2(year2, 'W', 50);
	nb_periods = periodW2.difference(periodW1);
	EXPECT_EQ(nb_periods, 300);
}

TEST(TestPeriod, Shift)
{
	int year1 = 2015;
	int year2 = 2020;

	// periodicity = Y
	Period periodY1(year1, 'Y', 1);
	Period periodY2(year2, 'Y', 1);
	Period shifted_periodY = periodY1.shift(5);
	EXPECT_EQ(shifted_periodY, periodY2);
	shifted_periodY = periodY2.shift(-5);
	EXPECT_EQ(shifted_periodY, periodY1);

	// periodicity = Q
	Period periodQ1(year1, 'Q', 2);
	Period periodQ2(year2, 'Q', 3);
	Period shifted_periodQ = periodQ1.shift(21);
	EXPECT_EQ(shifted_periodQ, periodQ2);
	shifted_periodQ = periodQ2.shift(-21);
	EXPECT_EQ(shifted_periodQ, periodQ1);

	// periodicity = W
	Period periodW1(year1, 'W', 10);
	Period periodW2(year2, 'W', 50);
	Period shifted_periodW = periodW1.shift(300);
	EXPECT_EQ(shifted_periodW, periodW2);
	shifted_periodW = periodW2.shift(-300);
	EXPECT_EQ(shifted_periodW, periodW1);
}

TEST(TestPeriod, ToString)
{
	std::string str_period;
	std::string str_res;

	// periodicity = Y
	str_period = "2020Y1";
	Period periodY(str_period);
	str_res = periodY.to_string();
	EXPECT_EQ(str_res, str_period);

	// periodicity = Q
	str_period = "2020Q2";
	Period periodQ(str_period);
	str_res = periodQ.to_string();
	EXPECT_EQ(str_res, str_period);

	// periodicity = W
	str_period = "2020W10";
	Period periodW(str_period);
	str_res = periodW.to_string();
	EXPECT_EQ(str_res, str_period);
}

TEST(TestPeriod, ToFloat)
{
	std::string str_period;
	float res;

	// periodicity = Y
	str_period = "2020Y1";
	Period periodY(str_period);
	res = periodY.to_float();
	EXPECT_FLOAT_EQ(res, 2020.0f);

	// periodicity = Q
	str_period = "2020Q2";
	Period periodQ(str_period);
	res = periodQ.to_float();
	EXPECT_FLOAT_EQ(res, 2020.0f + (1.f/4));

	// periodicity = W
	str_period = "2020W10";
	Period periodW(str_period);
	res = periodW.to_float();
	EXPECT_FLOAT_EQ(res, 2020.0f + (9 * (1.f/52)));
}

TEST(TestPeriod, Hash)
{
	std::size_t hash_before;
	std::size_t hash_after;

	Period period(2020, 'Y', 1);

	std::hash<Period> period_hasher;
    hash_before = period_hasher(period);

	// modify
	Period shifted_period = period.shift(1);
	hash_after = period_hasher(shifted_period);
	EXPECT_NE(hash_before, hash_after);

	// return to original value
	Period same_period = shifted_period.shift(-1);
	hash_after = period_hasher(same_period);
	EXPECT_EQ(hash_before, hash_after);
}
