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

TEST_F(EstimationTest, DynamicAdjustment)
{
    std::string eqs = kdb_eqs.get_lec("ACAF");
    std::string c1 = "c1";
    std::string c2 = "";
    std::string adjusted_eqs;
    std::string expected_eqs;

    // Partial Adjustment
    expected_eqs = "d((ACAF/VAF[-1])) := c1 * (acaf1+acaf2*GOSF[-1]+\nacaf4*(TIME=1995) -((ACAF/VAF[-1]))[-1])";
    adjusted_eqs = dynamic_adjustment(AM_PARTIAL_ADJUSTMENT, eqs, c1, c2);
    EXPECT_EQ(adjusted_eqs, expected_eqs);

    // Error Correction Method
    expected_eqs = "d((ACAF/VAF[-1])) := c1 * d(acaf1+acaf2*GOSF[-1]+\nacaf4*(TIME=1995)) + ";
    expected_eqs += "c2 * (acaf1+acaf2*GOSF[-1]+\nacaf4*(TIME=1995) -(ACAF/VAF[-1]))[-1]";
    c2 = "c2";
    adjusted_eqs = dynamic_adjustment(AM_ERROR_CORRECTION_METHOD, eqs, c1, c2);
    EXPECT_EQ(adjusted_eqs, expected_eqs);

    // Invalid arguments
    // not valid LEC expression (missing :=)
    std::string not_eqs = eqs;
    not_eqs.replace(not_eqs.find(":="), 2, "==");
    EXPECT_THROW(dynamic_adjustment(AM_PARTIAL_ADJUSTMENT, not_eqs, c1, c2), IodeExceptionInvalidArguments);
    // c1 empty
    EXPECT_THROW(dynamic_adjustment(AM_PARTIAL_ADJUSTMENT, eqs, "", c2), IodeExceptionInvalidArguments);
    // c2 empty in case of Error Correction Method
    EXPECT_THROW(dynamic_adjustment(AM_ERROR_CORRECTION_METHOD, eqs, c1, ""), IodeExceptionInvalidArguments);
}