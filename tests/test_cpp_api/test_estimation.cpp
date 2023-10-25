#include "pch.h"


class EstimationTest : public KDBTest, public ::testing::Test
{
protected:
    std::string from;
    std::string to;
    std::string eq_name;
    std::string eq_name2;
    std::string block;
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

        eq_name2 = "DPUH";
        block = eq_name + ";" + eq_name2;
    }

    // void TearDown() override {}
};


TEST_F(EstimationTest, Estimate)
{
    Equation eq(eq_name);
    Equation eq2(eq_name2);

    kdb_scl.update("acaf1", 0., 1.);
    kdb_scl.update("acaf2", 0., 1.);
    kdb_scl.update("acaf4", 0., 1.);
    kdb_scl.update("dpuh_1", 0., 1.);
    kdb_scl.update("dpuh_2", 0., 1.);

    Estimation* est = new Estimation(from, to, block);

    est->equations_estimate();
    const KDBScalars* kdb_scl_res = est->get_coefficients();
    est->save();

    // Sample
    Sample sample = est->get_sample();
    EXPECT_EQ(sample.to_string(), from + ":" + to);

    // List of equations
    std::vector<std::string> expected_list_eqs = {eq_name, eq_name2};
    std::vector<std::string> list_eqs = est->get_list_equations();
    EXPECT_EQ(list_eqs.size(), expected_list_eqs.size());
    EXPECT_EQ(list_eqs, expected_list_eqs);

    // Current equation
    NamedEquation named_eq = est->current_equation();
    EXPECT_EQ(named_eq.name, eq_name);
    EXPECT_EQ(named_eq.eq.get_lec(), eq.get_lec());

    // Next equation 
    NamedEquation next_named_eq = est->next_equation();
    EXPECT_EQ(next_named_eq.name, eq_name2);
    EXPECT_EQ(next_named_eq.eq.get_lec(), eq2.get_lec());

    // -- go back to first eq
    next_named_eq = est->next_equation();
    EXPECT_EQ(next_named_eq.name, eq_name);
    EXPECT_EQ(next_named_eq.eq.get_lec(), eq.get_lec());

    // Coeff values
    EXPECT_DOUBLE_EQ(round(1e6 * kdb_scl.get("acaf1").val) / 1e6, 0.01577);
    EXPECT_DOUBLE_EQ(round(1e6 * kdb_scl.get("acaf2").val) / 1e6, -8.e-06);
    EXPECT_DOUBLE_EQ(round(1e6 * kdb_scl.get("acaf4").val) / 1e6, -0.008503);
    EXPECT_DOUBLE_EQ(round(1e6 * kdb_scl.get("dpuh_1").val) / 1e6, 0.010986);
    EXPECT_DOUBLE_EQ(round(1e6 * kdb_scl.get("dpuh_2").val) / 1e6, 0.057489);

    EXPECT_DOUBLE_EQ(round(1e6 * kdb_scl_res->get("acaf1").val) / 1e6, 0.01577);
    EXPECT_DOUBLE_EQ(round(1e6 * kdb_scl_res->get("acaf2").val) / 1e6, -8.e-06);
    EXPECT_DOUBLE_EQ(round(1e6 * kdb_scl_res->get("acaf4").val) / 1e6, -0.008503);
    EXPECT_DOUBLE_EQ(round(1e6 * kdb_scl_res->get("dpuh_1").val) / 1e6, 0.010986);
    EXPECT_DOUBLE_EQ(round(1e6 * kdb_scl_res->get("dpuh_2").val) / 1e6, 0.057489);

    // Estimates ACAF only
    delete est;
    est = new Estimation(from, to, eq_name);
    est->equations_estimate();

    // Result values
    EXPECT_DOUBLE_EQ(round(1e6 * kdb_vars.get_var("_YRES0", "1980Y1")) / 1e6, -0.00115);

    // Tests values (ACAF)
    EXPECT_DOUBLE_EQ(round(1e6 * kdb_scl.get("e0_stdev").val) / 1e6, 0.00427);
    EXPECT_DOUBLE_EQ(round(1e6 * kdb_scl.get("e0_meany").val) / 1e6, 0.008185);
    EXPECT_DOUBLE_EQ(round(1e6 * kdb_scl.get("e0_ssres").val) / 1e6, 5.2e-05);
    EXPECT_DOUBLE_EQ(round(1e6 * kdb_scl.get("e0_stderr").val) / 1e6, 0.001927);
    EXPECT_DOUBLE_EQ(round(1e6 * kdb_scl.get("e0_sderrp").val) / 1e6, 23.542242);
    EXPECT_DOUBLE_EQ(round(1e6 * kdb_scl.get("e0_fstat").val) / 1e6, 32.285107);
    EXPECT_DOUBLE_EQ(round(1e6 * kdb_scl.get("e0_r2").val) / 1e6, 0.821815);
    EXPECT_DOUBLE_EQ(round(1e6 * kdb_scl.get("e0_r2adj").val) / 1e6, 0.79636);
    EXPECT_DOUBLE_EQ(round(1e6 * kdb_scl.get("e0_dw").val) / 1e6, 2.33007);
    EXPECT_DOUBLE_EQ(round(1e6 * kdb_scl.get("e0_loglik").val) / 1e6, 83.810104);

    Equation eq_est = kdb_eqs.get("ACAF");
    std::array<float, EQS_NBTESTS> tests = eq_est.get_tests();
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

    IODE_REAL max_value;
    IODE_REAL min_value;

    // Observed values
    Variable obs = est->get_observed_values("ACAF");
    EXPECT_DOUBLE_EQ(round(1e6 * obs[0]) / 1e6, 0.011412);
    max_value = *std::max_element(obs.begin(), obs.end());
    EXPECT_DOUBLE_EQ(round(1e6 * max_value) / 1e6, 0.016028);
    min_value = *std::min_element(obs.begin(), obs.end());
    EXPECT_DOUBLE_EQ(round(1e6 * min_value) / 1e6, -0.002985);

    // Fitted values
    Variable fitted = est->get_fitted_values("ACAF");
    EXPECT_DOUBLE_EQ(round(1e6 * fitted[0]) / 1e6, 0.012562);
    max_value = *std::max_element(fitted.begin(), fitted.end());
    EXPECT_DOUBLE_EQ(round(1e6 * max_value) / 1e6, 0.012562);
    min_value = *std::min_element(fitted.begin(), fitted.end());
    EXPECT_DOUBLE_EQ(round(1e6 * min_value) / 1e6, -0.002985);

    // Residual values
    Variable residuals = est->get_residual_values("ACAF");
    EXPECT_DOUBLE_EQ(round(1e6 * residuals[0]) / 1e6, -0.00115);
    max_value = *std::max_element(residuals.begin(), residuals.end());
    EXPECT_DOUBLE_EQ(round(1e6 * max_value) / 1e6, 0.003537);
    min_value = *std::min_element(residuals.begin(), residuals.end());
    EXPECT_DOUBLE_EQ(round(1e6 * min_value) / 1e6, -0.004334);

    // Correlation matrix
    // -- ACAF

    MAT* cm = est->get_correlation_matrix();
    EXPECT_EQ(M_NL(cm), 3);
    EXPECT_EQ(M_NC(cm), 3);
    // -- line 0
    EXPECT_DOUBLE_EQ(MATE(cm, 0, 0), 1.);
    EXPECT_DOUBLE_EQ(round(1e6 * MATE(cm, 0, 1)) / 1e6, -0.936111);
    EXPECT_DOUBLE_EQ(round(1e6 * MATE(cm, 0, 2)) / 1e6, 0.20017);
    // -- line 1
    EXPECT_DOUBLE_EQ(round(1e6 * MATE(cm, 1, 0)) / 1e6, -0.936111);
    EXPECT_DOUBLE_EQ(MATE(cm, 1, 1), 1.);
    EXPECT_DOUBLE_EQ(round(1e6 * MATE(cm, 1, 2)) / 1e6, -0.300746);
    // -- line 2
    EXPECT_DOUBLE_EQ(round(1e6 * MATE(cm, 2, 0)) / 1e6, 0.20017);
    EXPECT_DOUBLE_EQ(round(1e6 * MATE(cm, 2, 1)) / 1e6, -0.300746);
    EXPECT_DOUBLE_EQ(MATE(cm, 2, 2), 1.);

    // -- DPUH
    delete est;
    est = new Estimation(from, to, eq_name2);
    est->equations_estimate();

    cm = est->get_correlation_matrix();
    EXPECT_EQ(M_NL(cm), 2);
    EXPECT_EQ(M_NC(cm), 2);
    // -- line 0
    EXPECT_DOUBLE_EQ(MATE(cm, 0, 0), 1.);
    EXPECT_DOUBLE_EQ(round(1e6 * MATE(cm, 0, 1)) / 1e6, -0.042291);
    // -- line 1
    EXPECT_DOUBLE_EQ(round(1e6 * MATE(cm, 1, 0)) / 1e6, -0.042291);
    EXPECT_DOUBLE_EQ(MATE(cm, 1, 1), 1.);

    delete est;
}

TEST_F(EstimationTest, EstimateBlock)
{
    Estimation* est = new Estimation(from, to, block);

    est->equations_estimate();

    NamedEquation first_eq = est->current_equation();
    EXPECT_EQ(first_eq.name, eq_name);
    EXPECT_EQ(first_eq.eq, Equation(eq_name));

    NamedEquation second_eq = est->next_equation();
    EXPECT_EQ(second_eq.name, eq_name2);
    EXPECT_EQ(second_eq.eq, Equation(eq_name2));

    delete est;
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
    EXPECT_DOUBLE_EQ(round(1e6 * scl_order_0->val) / 1e6, 0.051603);
    EXPECT_DOUBLE_EQ(round(1e6 * scl_order_0->relax) / 1e6, 0.053847);
    EXPECT_DOUBLE_EQ(round(1e6 * scl_order_0->std) / 1e6, 0.053847);
    delete scl_order_0;

    delete kdb_res;

    // drift, no trend
    drift = true;
    trend = false;
    kdb_res = dickey_fuller_test(var_name, drift, trend, order);

    scl_order_0 = new Scalar(kdb_res->get("df_"));
    EXPECT_DOUBLE_EQ(round(1e6 * scl_order_0->val) / 1e6, 0.05969);
    EXPECT_DOUBLE_EQ(round(1e6 * scl_order_0->relax) / 1e6, 0.053414);
    EXPECT_DOUBLE_EQ(round(1e6 * scl_order_0->std) / 1e6, 0.053414);
    delete scl_order_0;

    scl_drift = new Scalar(kdb_res->get("df_d"));
    EXPECT_DOUBLE_EQ(round(1e6 * scl_drift->val) / 1e6, -2.351039);
    EXPECT_DOUBLE_EQ(round(1e6 * scl_drift->relax) / 1e6, 1.588698);
    EXPECT_DOUBLE_EQ(round(1e6 * scl_drift->std) / 1e6, 1.588698);
    delete scl_drift;

    delete kdb_res;

    // drift, trend
    drift = true;
    trend = true;
    kdb_res = dickey_fuller_test(var_name, drift, trend, order);

    scl_order_0 = new Scalar(kdb_res->get("df_"));
    EXPECT_DOUBLE_EQ(round(1e6 * scl_order_0->val) / 1e6, -0.055952);
    EXPECT_DOUBLE_EQ(round(1e6 * scl_order_0->relax) / 1e6, 0.070056);
    EXPECT_DOUBLE_EQ(round(1e6 * scl_order_0->std) / 1e6, 0.070056);
    delete scl_order_0;

    scl_drift = new Scalar(kdb_res->get("df_d"));
    EXPECT_DOUBLE_EQ(round(1e6 * scl_drift->val) / 1e6, 10.599557);
    EXPECT_DOUBLE_EQ(round(1e6 * scl_drift->relax) / 1e6, 5.620008);
    EXPECT_DOUBLE_EQ(round(1e6 * scl_drift->std) / 1e6, 5.620008);
    delete scl_drift;

    scl_trend = new Scalar(kdb_res->get("df_t"));
    EXPECT_DOUBLE_EQ(round(1e6 * scl_trend->val) / 1e6, -0.381779);
    EXPECT_DOUBLE_EQ(round(1e6 * scl_trend->relax) / 1e6, 0.1596);
    EXPECT_DOUBLE_EQ(round(1e6 * scl_trend->std) / 1e6, 0.1596);
    delete scl_trend;

    delete kdb_res;
}
