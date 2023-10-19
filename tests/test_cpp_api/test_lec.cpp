#include "pch.h"


class LecTest : public KDBTest, public ::testing::Test
{
protected:
    void SetUp() override
    {
        load_global_kdb(I_VARIABLES, input_test_dir + "a.var");
    }

    // void TearDown() override {}
};


TEST_F(LecTest, LEC)
{
    Variable var_A = Variables.get("A");

    int t = 10;
    double value = execute_lec("A + 1", t);
    EXPECT_DOUBLE_EQ(var_A[t] + 1, value);

    std::string period = "2002Y1";
    value = execute_lec("A - 1", period);
    t = Variables.get_sample().get_period_position(period);
    EXPECT_DOUBLE_EQ(var_A[t] - 1, value);

    std::vector<double> values = execute_lec("1 + A - 1");
    EXPECT_EQ(var_A, values);
}