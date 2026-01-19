#include "pch.h"


class LecTest : public KDBTest, public ::testing::Test
{
protected:
    void SetUp() override
    {
        global_ws_var->load(input_test_dir + "a.var");
    }

    // void TearDown() override {}
};


TEST_F(LecTest, Copy)
{
    std::string lec = "A + 1";
    CLEC* clec = NULL;
    
    clec = L_cc(to_char_array(lec));
    SW_nfree(clec);

    clec = L_cc(to_char_array(lec));
    CLEC* copy_clec = clec_deep_copy(clec);
    SW_nfree(clec);
    SW_nfree(copy_clec);
}

TEST_F(LecTest, LEC)
{
    Variable var_A = global_ws_var->get("A");

    Sample* sample = global_ws_var->get_sample();
    EXPECT_TRUE(sample != nullptr);

    int t = 10;
    double value = execute_lec("A + 1", t);
    EXPECT_DOUBLE_EQ(var_A[t] + 1, value);

    std::string period = "2002Y1";
    value = execute_lec("A - 1", period);
    t = global_ws_var->get_sample()->get_period_position(period);
    EXPECT_DOUBLE_EQ(var_A[t] - 1, value);

    std::vector<double> values = execute_lec("1 + A - 1");
    EXPECT_EQ(var_A, values);
}