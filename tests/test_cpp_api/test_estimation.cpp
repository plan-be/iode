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

    // R2, R2 ajusted and DW
    Equation eq = kdb_eqs.get("ACAF");
    EXPECT_DOUBLE_EQ(round(1e6 * eq.get_tests()[KE_R2 - 10]) / 1e6, 0.821815);
    EXPECT_DOUBLE_EQ(round(1e6 * eq.get_tests()[KE_R2ADJ - 10]) / 1e6, 0.79636);
    EXPECT_DOUBLE_EQ(round(1e6 * eq.get_tests()[KE_DW - 10]) / 1e6, 2.33007);

    // correlation matrix
    MAT* cm = est->get_correlation_matrix();
    /*
    EXPECT_DOUBLE_EQ(MATE(cm, 0, 0), 1.);
    EXPECT_DOUBLE_EQ(round(1e6 * MATE(cm, 0, 1)) / 1e6, -0.936111);
    EXPECT_DOUBLE_EQ(round(1e6 * MATE(cm, 0, 2)) / 1e6, 0.200169);
    EXPECT_DOUBLE_EQ(round(1e6 * MATE(cm, 1, 0)) / 1e6, -0.936111);
    EXPECT_DOUBLE_EQ(MATE(cm, 1, 1), 1.);
    EXPECT_DOUBLE_EQ(round(1e6 * MATE(cm, 1, 2)) / 1e6, -0.3);
    EXPECT_DOUBLE_EQ(round(1e6 * MATE(cm, 2, 0)) / 1e6, 0.200169);
    EXPECT_DOUBLE_EQ(round(1e6 * MATE(cm, 2, 1)) / 1e6, -0.3);
    EXPECT_DOUBLE_EQ(MATE(cm, 2, 2), 1.);
    */
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

// TODO : fix E_UnitRoot() 
TEST_F(EstimationTest, DickeyFullerTest)
{
    std::string var_name = "ACAF";
    bool drift = false;
    bool trend = false;
    int order = 0;

    Scalar* scl_drift;
    Scalar* scl_trend;
    Scalar* scl_order_0;

    // Unknown variable
    EXPECT_THROW(dickey_fuller_test("XORGLUB", drift, trend, order), IodeExceptionFunction);

    // no drift, no trend
    drift = false;
    trend = false;
    KDBScalars* kdb_res = dickey_fuller_test(var_name, drift, trend, order);

    std::vector<std::string> names = kdb_res->get_names();

    scl_order_0 = new Scalar(kdb_res->get("df_"));
    EXPECT_DOUBLE_EQ(round(1e6 * scl_order_0->value()) / 1e6, 0.051603);
    EXPECT_DOUBLE_EQ(round(1e6 * scl_order_0->relax()) / 1e6, 0.053847);
    EXPECT_DOUBLE_EQ(round(1e6 * scl_order_0->std()) / 1e6, 0.053847);
    delete scl_order_0;

    delete kdb_res;

    // drift, no trend
    drift = true;
    trend = false;
    kdb_res = dickey_fuller_test(var_name, drift, trend, order);

    scl_order_0 = new Scalar(kdb_res->get("df_"));
    EXPECT_DOUBLE_EQ(round(1e6 * scl_order_0->value()) / 1e6, 0.05969);
    EXPECT_DOUBLE_EQ(round(1e6 * scl_order_0->relax()) / 1e6, 0.053414);
    EXPECT_DOUBLE_EQ(round(1e6 * scl_order_0->std()) / 1e6, 0.053414);
    delete scl_order_0;

    scl_drift = new Scalar(kdb_res->get("df_d"));
    EXPECT_DOUBLE_EQ(round(1e6 * scl_drift->value()) / 1e6, -2.351039);
    EXPECT_DOUBLE_EQ(round(1e6 * scl_drift->relax()) / 1e6, 1.588698);
    EXPECT_DOUBLE_EQ(round(1e6 * scl_drift->std()) / 1e6, 1.588698);
    delete scl_drift;

    delete kdb_res;

    // drift, trend
    drift = true;
    trend = true;
    kdb_res = dickey_fuller_test(var_name, drift, trend, order);

    scl_order_0 = new Scalar(kdb_res->get("df_"));
    EXPECT_DOUBLE_EQ(round(1e6 * scl_order_0->value()) / 1e6, -0.055952);
    EXPECT_DOUBLE_EQ(round(1e6 * scl_order_0->relax()) / 1e6, 0.070056);
    EXPECT_DOUBLE_EQ(round(1e6 * scl_order_0->std()) / 1e6, 0.070056);
    delete scl_order_0;

    scl_drift = new Scalar(kdb_res->get("df_d"));
    EXPECT_DOUBLE_EQ(round(1e6 * scl_drift->value()) / 1e6, 10.599557);
    EXPECT_DOUBLE_EQ(round(1e6 * scl_drift->relax()) / 1e6, 5.620008);
    EXPECT_DOUBLE_EQ(round(1e6 * scl_drift->std()) / 1e6, 5.620008);
    delete scl_drift;

    scl_trend = new Scalar(kdb_res->get("df_t"));
    EXPECT_DOUBLE_EQ(round(1e6 * scl_trend->value()) / 1e6, -0.381779);
    EXPECT_DOUBLE_EQ(round(1e6 * scl_trend->relax()) / 1e6, 0.1596);
    EXPECT_DOUBLE_EQ(round(1e6 * scl_trend->std()) / 1e6, 0.1596);
    delete scl_trend;

    delete kdb_res;
}
