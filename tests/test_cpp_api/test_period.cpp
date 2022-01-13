#include "pch.h"
#include "../../cpp_api/iode_cpp_api.h"


TEST(TestPeriod, Create) 
{
	// passing year, position in the year and periodicity
	Period period(2020, 1, 'Y');

	// passing a string
	Period period2("2020Y1");

	// passing a PERIOD C_API struct
	char buf[10] = "2020Y1";
	PERIOD* c_period = PER_atoper(buf);
	Period period3(c_period);
	SW_nfree(c_period);
}

TEST(TestPeriod, To_c_period)
{
	Period period(2020, 1, 'Y');
	PERIOD c_period = period.to_c_period();
	EXPECT_EQ(c_period.p_y, 2020);
	EXPECT_EQ(c_period.p_s, 1);
	EXPECT_EQ(c_period.p_p, 'Y');
}

TEST(TestPeriod, NbPeriodsPerYear)
{
	for (auto const& [periodicity, expected_nb_periods] : mPeriodicities)
	{
		Period period(2020, 1, periodicity);
		int nb_periods = period.nb_periods_per_year();
		EXPECT_EQ(nb_periods, expected_nb_periods);
	}
}

TEST(TestPeriod, Difference)
{
	int nb_periods;
	int year1 = 2015;
	int year2 = 2020;

	// periodicity = Y
	Period periodY1(year1, 1, 'Y');
	Period periodY2(year2, 1, 'Y');
	nb_periods = periodY2.difference(periodY1);
	EXPECT_EQ(nb_periods, 5);

	// periodicity = Q
	Period periodQ1(year1, 2, 'Q');
	Period periodQ2(year2, 3, 'Q');
	nb_periods = periodQ2.difference(periodQ1);
	EXPECT_EQ(nb_periods, 21);

	// periodicity = W
	Period periodW1(year1, 10, 'W');
	Period periodW2(year2, 50, 'W');
	nb_periods = periodW2.difference(periodW1);
	EXPECT_EQ(nb_periods, 300);
}

TEST(TestPeriod, Shift)
{
	int year1 = 2015;
	int year2 = 2020;

	// periodicity = Y
	Period periodY1(year1, 1, 'Y');
	Period periodY2(year2, 1, 'Y');
	Period shifted_periodY = periodY1.shift(5);
	EXPECT_EQ(shifted_periodY, periodY2);

	// periodicity = Q
	Period periodQ1(year1, 2, 'Q');
	Period periodQ2(year2, 3, 'Q');
	Period shifted_periodQ = periodQ1.shift(21);
	EXPECT_EQ(shifted_periodQ, periodQ2);

	// periodicity = W
	Period periodW1(year1, 10, 'W');
	Period periodW2(year2, 50, 'W');
	Period shifted_periodW = periodW1.shift(300);
	EXPECT_EQ(shifted_periodW, periodW2);
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
