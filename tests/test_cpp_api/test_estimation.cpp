#include "pch.h"


class EstimationTest : public KDBTest, public ::testing::Test
{
protected:
    Estimation* est;
    std::string from;
    std::string to;
    std::string eq_name;
    KDBEquations kdb_eqs;
    KDBVariables kdb_vars;

    void SetUp() override
    {
        load_global_kdb(I_EQUATIONS, input_test_dir + "fun.eqs");
        load_global_kdb(I_SCALARS, input_test_dir + "fun.scl");
        load_global_kdb(I_VARIABLES, input_test_dir + "fun.var");

        from = "1980Y1";
        to = "1996Y1";
        eq_name = "ACAF";
        est = new Estimation(from, to, eq_name);
    }

    void TearDown() override 
    {
        delete est;
    }
};


TEST_F(EstimationTest, Estimate)
{
    est->equations_estimate();

    EXPECT_DOUBLE_EQ(round(1e6 * kdb_vars.get_var("_YRES", "1980Y1")) / 1e6, -0.00115);

    Equation eq = kdb_eqs.get("ACAF");
    EXPECT_DOUBLE_EQ(round(1e6 * eq.get_tests()[KE_R2 - 10]) / 1e6, 0.821815);
    EXPECT_DOUBLE_EQ(round(1e6 * eq.get_tests()[KE_R2ADJ - 10]) / 1e6, 0.79636);
    EXPECT_DOUBLE_EQ(round(1e6 * eq.get_tests()[KE_DW - 10]) / 1e6, 2.33007);
}