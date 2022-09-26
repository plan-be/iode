#include "pch.h"


class EstimationTest : public KDBTest, public ::testing::Test
{
protected:
    Estimation* est;
    std::string from;
    std::string to;
    std::string eq_name;
    KDBEquations kdb_eqs;
    KDBScalars kdb_scl;
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
    kdb_scl.update("acaf1", 0., 1.);
    kdb_scl.update("acaf2", 0., 1.);
    kdb_scl.update("acaf4", 0., 1.);

    est->equations_estimate();
    est->save();

    // coeff values
    EXPECT_DOUBLE_EQ(round(1e6 * kdb_scl.get("acaf1").value()) / 1e6, 0.01577);
    EXPECT_DOUBLE_EQ(round(1e6 * kdb_scl.get("acaf2").value()) / 1e6, -8.e-06);
    EXPECT_DOUBLE_EQ(round(1e6 * kdb_scl.get("acaf4").value()) / 1e6, -0.008503);

    // result values
    EXPECT_DOUBLE_EQ(round(1e6 * kdb_vars.get_var("_YRES", "1980Y1")) / 1e6, -0.00115);

    // Tests values
    EXPECT_DOUBLE_EQ(round(1e6 * kdb_scl.get("e0_stdev").value()) / 1e6, 0.00427);
    EXPECT_DOUBLE_EQ(round(1e6 * kdb_scl.get("e0_meany").value()) / 1e6, 0.008185);
    EXPECT_DOUBLE_EQ(round(1e6 * kdb_scl.get("e0_ssres").value()) / 1e6, 5.2e-05);
    EXPECT_DOUBLE_EQ(round(1e6 * kdb_scl.get("e0_stderr").value()) / 1e6, 0.001927);
    EXPECT_DOUBLE_EQ(round(1e6 * kdb_scl.get("e0_sderrp").value()) / 1e6, 23.542242);
    EXPECT_DOUBLE_EQ(round(1e6 * kdb_scl.get("e0_fstat").value()) / 1e6, 32.285107);
    EXPECT_DOUBLE_EQ(round(1e6 * kdb_scl.get("e0_r2").value()) / 1e6, 0.821815);
    EXPECT_DOUBLE_EQ(round(1e6 * kdb_scl.get("e0_r2adj").value()) / 1e6, 0.79636);
    EXPECT_DOUBLE_EQ(round(1e6 * kdb_scl.get("e0_dw").value()) / 1e6, 2.33007);
    EXPECT_DOUBLE_EQ(round(1e6 * kdb_scl.get("e0_loglik").value()) / 1e6, 83.810104);

    Equation eq = kdb_eqs.get("ACAF");
    std::array<float, EQS_NBTESTS> tests = eq.get_tests();
    EXPECT_DOUBLE_EQ(round(1e6 * tests[0]) / 1e6, 1.);
    EXPECT_DOUBLE_EQ(round(1e6 * tests[IE_STDEV]) / 1e6, 0.00427);
    EXPECT_DOUBLE_EQ(round(1e6 * tests[IE_MEANY]) / 1e6, 0.008185);
    EXPECT_DOUBLE_EQ(round(1e6 * tests[IE_SSRES]) / 1e6, 5.2e-05);
    EXPECT_DOUBLE_EQ(round(1e6 * tests[IE_STDERR]) / 1e6, 0.001927);
    EXPECT_DOUBLE_EQ(round(1e6 * tests[IE_STDERRP]) / 1e6, 23.542242);
    EXPECT_DOUBLE_EQ(round(1e6 * tests[IE_FSTAT]) / 1e6, 32.285107);
    EXPECT_DOUBLE_EQ(round(1e6 * tests[IE_R2]) / 1e6, 0.821815);
    EXPECT_DOUBLE_EQ(round(1e6 * tests[IE_R2ADJ]) / 1e6, 0.79636);
    EXPECT_DOUBLE_EQ(round(1e6 * tests[IE_DW]) / 1e6, 2.33007);
    EXPECT_DOUBLE_EQ(round(1e6 * tests[IE_LOGLIK]) / 1e6, 83.810104);

    // correlation matrix
    MAT* cm = est->get_correlation_matrix();
    EXPECT_DOUBLE_EQ(MATE(cm, 0, 0), 1.);
    EXPECT_DOUBLE_EQ(round(1e6 * MATE(cm, 0, 1)) / 1e6, -0.936111);
    EXPECT_DOUBLE_EQ(round(1e6 * MATE(cm, 0, 2)) / 1e6, 0.20017);
    EXPECT_DOUBLE_EQ(round(1e6 * MATE(cm, 1, 0)) / 1e6, -0.936111);
    EXPECT_DOUBLE_EQ(MATE(cm, 1, 1), 1.);
    EXPECT_DOUBLE_EQ(round(1e6 * MATE(cm, 1, 2)) / 1e6, -0.300746);
    EXPECT_DOUBLE_EQ(round(1e6 * MATE(cm, 2, 0)) / 1e6, 0.20017);
    EXPECT_DOUBLE_EQ(round(1e6 * MATE(cm, 2, 1)) / 1e6, -0.300746);
    EXPECT_DOUBLE_EQ(MATE(cm, 2, 2), 1.);
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
