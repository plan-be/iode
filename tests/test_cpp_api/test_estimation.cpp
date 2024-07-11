#include "pch.h"


class EstimationTest : public KDBTest, public ::testing::Test
{
protected:
    void SetUp() override
    {
        KDBEquations kdb_eqs(input_test_dir + "fun.eqs");
        KDBScalars kdb_scl(input_test_dir + "fun.scl");
        KDBVariables kdb_var(input_test_dir + "fun.var");
    }

    // void TearDown() override {}
};


TEST_F(EstimationTest, SetBlock)
{
    std::vector<std::string> v_eqs;
    std::vector<std::string> v_expected_eqs;

    std::vector<std::string> v_coeffs;
    std::vector<std::string> v_expected_coeffs;

    std::string from = "1980Y1";
    std::string to = "1996Y1";
    EditAndEstimateEquations est(from, to);

    Equation* eq_ACAF = Equations.get("ACAF");
    Equation* eq_DPUH = Equations.get("DPUH");

    // ---- block = ACAF;DPUH ----
    // set_block("new_block", "currently_displayed_equation")
    est.set_block("ACAF;DPUH", "ACAF");
    
    v_eqs = est.get_list_equations();
    v_expected_eqs = {"ACAF", "DPUH"};
    EXPECT_EQ(v_eqs, v_expected_eqs);

    v_eqs = est.get_equations()->get_names();
    EXPECT_EQ(v_eqs, v_expected_eqs);

    est.update_scalars();
    v_coeffs = est.get_scalars()->get_names();
    v_expected_coeffs = {"acaf1", "acaf2", "acaf4", "dpuh_1", "dpuh_2"};
    EXPECT_EQ(v_coeffs, v_expected_coeffs);

    // current equation
    Equation* current_eq = est.current_equation();
    EXPECT_EQ(current_eq->get_endo(), "ACAF");
    EXPECT_EQ(current_eq->get_lec(), eq_ACAF->get_lec());

    // next equation 
    Equation* next_eq = est.next_equation();
    EXPECT_EQ(next_eq->get_endo(), "DPUH");
    EXPECT_EQ(next_eq->get_lec(), eq_DPUH->get_lec());

    // go back to first eq
    next_eq = est.next_equation();
    EXPECT_EQ(next_eq->get_endo(), "ACAF");
    EXPECT_EQ(next_eq->get_lec(), eq_ACAF->get_lec());

    // ---- add a non existing equation to the block ----
    // set_block("new_block", "currently_displayed_equation")
    est.set_block("ACAF;DPUH;TEST", "DPUH");

    v_eqs = est.get_list_equations();
    v_expected_eqs = {"ACAF", "DPUH", "TEST"};
    EXPECT_EQ(v_eqs, v_expected_eqs);

    v_eqs = est.get_equations()->get_names();
    EXPECT_EQ(v_eqs, v_expected_eqs);

    est.update_scalars();
    v_coeffs = est.get_scalars()->get_names();
    v_expected_coeffs = {"acaf1", "acaf2", "acaf4", "dpuh_1", "dpuh_2"};
    EXPECT_EQ(v_coeffs, v_expected_coeffs);

    // current equation
    current_eq = est.current_equation();
    EXPECT_EQ(current_eq->get_endo(), "DPUH");
    EXPECT_EQ(current_eq->get_lec(), eq_DPUH->get_lec());

    // next equation
    next_eq = est.next_equation();
    EXPECT_EQ(next_eq->get_endo(), "TEST");
    EXPECT_EQ(next_eq->get_lec(), "TEST := 0");

    // next equation
    next_eq = est.next_equation();
    EXPECT_EQ(next_eq->get_endo(), "ACAF");
    EXPECT_EQ(next_eq->get_lec(), eq_ACAF->get_lec());

    // ---- remove an equation from the block ----
    // set_block("new_block", "currently_displayed_equation")
    est.set_block("ACAF;TEST", "ACAF");

    v_eqs = est.get_list_equations();
    v_expected_eqs = {"ACAF", "TEST"};
    EXPECT_EQ(v_eqs, v_expected_eqs);

    v_eqs = est.get_equations()->get_names();
    v_expected_eqs = {"ACAF", "DPUH", "TEST"};
    EXPECT_EQ(v_eqs, v_expected_eqs);

    est.update_scalars();
    v_coeffs = est.get_scalars()->get_names();
    v_expected_coeffs = {"acaf1", "acaf2", "acaf4"};
    EXPECT_EQ(v_coeffs, v_expected_coeffs);

    // current equation
    current_eq = est.current_equation();
    EXPECT_EQ(current_eq->get_endo(), "ACAF");
    EXPECT_EQ(current_eq->get_lec(), eq_ACAF->get_lec());

    // next equation
    next_eq = est.next_equation();
    EXPECT_EQ(next_eq->get_endo(), "TEST");
    EXPECT_EQ(next_eq->get_lec(), "TEST := 0");

    // ---- currently displayed equation not in the block -> add it to the block ----
    // set_block("new_block", "currently_displayed_equation")
    est.set_block("ACAF;TEST", "DPUH");

    std::string block = est.get_block();
    EXPECT_EQ(block, "ACAF;TEST;DPUH");

    v_eqs = est.get_list_equations();
    v_expected_eqs = {"ACAF", "TEST", "DPUH"};
    EXPECT_EQ(v_eqs, v_expected_eqs);

    v_eqs = est.get_equations()->get_names();
    v_expected_eqs = {"ACAF", "DPUH", "TEST"};
    EXPECT_EQ(v_eqs, v_expected_eqs);

    est.update_scalars();
    v_coeffs = est.get_scalars()->get_names();
    v_expected_coeffs = {"acaf1", "acaf2", "acaf4", "dpuh_1", "dpuh_2"};
    EXPECT_EQ(v_coeffs, v_expected_coeffs);

    // current equation
    current_eq = est.current_equation();
    EXPECT_EQ(current_eq->get_endo(), "DPUH");
    EXPECT_EQ(current_eq->get_lec(), eq_DPUH->get_lec());

    // ---- some scalars does not exist yet ----
    Scalars.remove("dpuh_1");
    Scalars.remove("dpuh_2");

    EditAndEstimateEquations est_new_coeffs(from, to);
    // set_block("new_block", "currently_displayed_equation")
    est_new_coeffs.set_block("ACAF;DPUH", "ACAF");
    
    v_eqs = est_new_coeffs.get_list_equations();
    v_expected_eqs = {"ACAF", "DPUH"};
    EXPECT_EQ(v_eqs, v_expected_eqs);

    est_new_coeffs.update_scalars();
    v_coeffs = est_new_coeffs.get_scalars()->get_names();
    v_expected_coeffs = {"acaf1", "acaf2", "acaf4", "dpuh_1", "dpuh_2"};
    EXPECT_EQ(v_coeffs, v_expected_coeffs);

    KDBScalars* kdb_scl_res = est_new_coeffs.get_scalars();;
    EXPECT_DOUBLE_EQ(kdb_scl_res->get("dpuh_1")->val, 0.9);
    EXPECT_DOUBLE_EQ(kdb_scl_res->get("dpuh_1")->relax, 1.0);
    EXPECT_DOUBLE_EQ(kdb_scl_res->get("dpuh_2")->val, 0.9);
    EXPECT_DOUBLE_EQ(kdb_scl_res->get("dpuh_2")->relax, 1.0);
}


TEST_F(EstimationTest, Estimate)
{
    std::string from = "1980Y1";
    std::string to = "1996Y1";

    Equation* eq = Equations.get("ACAF");
    Equation* eq2 = Equations.get("DPUH");

    Scalars.update("acaf1", 0., 1.);
    Scalars.update("acaf2", 0., 1.);
    Scalars.update("acaf4", 0., 1.);
    Scalars.update("dpuh_1", 0., 1.);
    Scalars.update("dpuh_2", 0., 1.);

    EditAndEstimateEquations est(from, to);
    est.set_instruments("");
    est.set_method(EQ_LSQ);

    // Estimates the block ACAF;DPUH
    est.set_block("ACAF;DPUH");
    est.update_scalars();
    est.estimate();
    ASSERT_TRUE(est.is_estimation_done());

    est.copy_eq_tests_values();
    est.save();

    // number of coefficients 
    EXPECT_EQ(E_NCE, 5);

    // number of equations
    EXPECT_EQ(E_NEQ, 2);

    // Sample
    Sample* sample = est.get_sample();
    EXPECT_EQ(sample->to_string(), from + ":" + to);

    // Correlation matrix
    // -- ACAF;DPUH

    CorrelationMatrix* m_corr = est.get_correlation_matrix();
    EXPECT_EQ(m_corr->nb_coeffs, E_NCE);
    // -- line 0
    EXPECT_DOUBLE_EQ(m_corr->get_value(0, 0), 1.);
    EXPECT_DOUBLE_EQ(round(1e6 * m_corr->get_value(0, 1)) / 1e6, -0.935266);
    EXPECT_DOUBLE_EQ(round(1e6 * m_corr->get_value(0, 2)) / 1e6, 0.200167);
    EXPECT_DOUBLE_EQ(round(1e6 * m_corr->get_value(0, 3)) / 1e6, 0.044832);
    EXPECT_DOUBLE_EQ(round(1e6 * m_corr->get_value(0, 4)) / 1e6, -0.03729);
    // -- line 1
    EXPECT_DOUBLE_EQ(round(1e6 * m_corr->get_value(1, 0)) / 1e6, -0.935266);
    EXPECT_DOUBLE_EQ(m_corr->get_value(1, 1), 1.);
    EXPECT_DOUBLE_EQ(round(1e6 * m_corr->get_value(1, 2)) / 1e6, -0.300833);
    EXPECT_DOUBLE_EQ(round(1e6 * m_corr->get_value(1, 3)) / 1e6, -0.001662);
    EXPECT_DOUBLE_EQ(round(1e6 * m_corr->get_value(1, 4)) / 1e6, 0.039581);
    // -- line 2
    EXPECT_DOUBLE_EQ(round(1e6 * m_corr->get_value(2, 0)) / 1e6, 0.200167);
    EXPECT_DOUBLE_EQ(round(1e6 * m_corr->get_value(2, 1)) / 1e6, -0.300833);
    EXPECT_DOUBLE_EQ(m_corr->get_value(2, 2), 1.);
    EXPECT_DOUBLE_EQ(round(1e6 * m_corr->get_value(2, 3)) / 1e6, 0.000375);
    EXPECT_DOUBLE_EQ(round(1e6 * m_corr->get_value(2, 4)) / 1e6, -0.008926);
    // -- line 3
    EXPECT_DOUBLE_EQ(round(1e6 * m_corr->get_value(3, 0)) / 1e6, 0.044832);
    EXPECT_DOUBLE_EQ(round(1e6 * m_corr->get_value(3, 1)) / 1e6, -0.001662);
    EXPECT_DOUBLE_EQ(round(1e6 * m_corr->get_value(3, 2)) / 1e6, 0.000375);
    EXPECT_DOUBLE_EQ(m_corr->get_value(3, 3), 1.);
    EXPECT_DOUBLE_EQ(round(1e6 * m_corr->get_value(3, 4)) / 1e6, -0.041987);
    // -- line 3
    EXPECT_DOUBLE_EQ(round(1e6 * m_corr->get_value(4, 0)) / 1e6, -0.03729);
    EXPECT_DOUBLE_EQ(round(1e6 * m_corr->get_value(4, 1)) / 1e6, 0.039581);
    EXPECT_DOUBLE_EQ(round(1e6 * m_corr->get_value(4, 2)) / 1e6, -0.008926);
    EXPECT_DOUBLE_EQ(round(1e6 * m_corr->get_value(4, 3)) / 1e6, -0.041987);
    EXPECT_DOUBLE_EQ(m_corr->get_value(4, 4), 1.);

    // Coeff values
    EXPECT_DOUBLE_EQ(round(1e6 * Scalars.get("acaf1")->val) / 1e6, 0.01577);
    EXPECT_DOUBLE_EQ(round(1e6 * Scalars.get("acaf2")->val) / 1e6, -8.e-06);
    EXPECT_DOUBLE_EQ(round(1e6 * Scalars.get("acaf4")->val) / 1e6, -0.008503);
    EXPECT_DOUBLE_EQ(round(1e6 * Scalars.get("dpuh_1")->val) / 1e6, 0.010986);
    EXPECT_DOUBLE_EQ(round(1e6 * Scalars.get("dpuh_2")->val) / 1e6, 0.057489);

    KDBScalars* kdb_scl_res = est.get_scalars();
    EXPECT_DOUBLE_EQ(round(1e6 * kdb_scl_res->get("acaf1")->val) / 1e6, 0.01577);
    EXPECT_DOUBLE_EQ(round(1e6 * kdb_scl_res->get("acaf2")->val) / 1e6, -8.e-06);
    EXPECT_DOUBLE_EQ(round(1e6 * kdb_scl_res->get("acaf4")->val) / 1e6, -0.008503);
    EXPECT_DOUBLE_EQ(round(1e6 * kdb_scl_res->get("dpuh_1")->val) / 1e6, 0.010986);
    EXPECT_DOUBLE_EQ(round(1e6 * kdb_scl_res->get("dpuh_2")->val) / 1e6, 0.057489);

    // Estimates ACAF only
    est.set_block("ACAF");
    est.update_scalars();
    est.estimate();
    ASSERT_TRUE(est.is_estimation_done());
    
    est.copy_eq_tests_values();
    est.save();

    // number of coefficients 
    EXPECT_EQ(E_NCE, 3);

    // number of equations
    EXPECT_EQ(E_NEQ, 1);

    // Tests values (ACAF)
    EXPECT_DOUBLE_EQ(round(1e6 * Scalars.get("e0_stdev")->val) / 1e6, 0.00427);
    EXPECT_DOUBLE_EQ(round(1e6 * Scalars.get("e0_meany")->val) / 1e6, 0.008185);
    EXPECT_DOUBLE_EQ(round(1e6 * Scalars.get("e0_ssres")->val) / 1e6, 5.2e-05);
    EXPECT_DOUBLE_EQ(round(1e6 * Scalars.get("e0_stderr")->val) / 1e6, 0.001927);
    EXPECT_DOUBLE_EQ(round(1e6 * Scalars.get("e0_sderrp")->val) / 1e6, 23.542242);
    EXPECT_DOUBLE_EQ(round(1e6 * Scalars.get("e0_fstat")->val) / 1e6, 32.285107);
    EXPECT_DOUBLE_EQ(round(1e6 * Scalars.get("e0_r2")->val) / 1e6, 0.821815);
    EXPECT_DOUBLE_EQ(round(1e6 * Scalars.get("e0_r2adj")->val) / 1e6, 0.79636);
    EXPECT_DOUBLE_EQ(round(1e6 * Scalars.get("e0_dw")->val) / 1e6, 2.33007);
    EXPECT_DOUBLE_EQ(round(1e6 * Scalars.get("e0_loglik")->val) / 1e6, 83.810104);

    Equation* eq_est = Equations.get("ACAF");
    std::array<float, EQS_NBTESTS> tests = eq_est->get_tests();
    EXPECT_DOUBLE_EQ(round(1e6 * tests[0]) / 1e6, 1.);
    EXPECT_DOUBLE_EQ(round(1e6 * tests[EQ_STDEV]) / 1e6, 0.00427);
    EXPECT_DOUBLE_EQ(round(1e6 * tests[EQ_MEANY]) / 1e6, 0.008185);
    EXPECT_DOUBLE_EQ(round(1e6 * tests[EQ_SSRES]) / 1e6, 5.2e-05);
    EXPECT_DOUBLE_EQ(round(1e6 * tests[EQ_STDERR]) / 1e6, 0.001927);
    EXPECT_DOUBLE_EQ(round(1e6 * tests[EQ_STDERRP]) / 1e6, 23.542242);
    EXPECT_DOUBLE_EQ(round(1e6 * tests[EQ_FSTAT]) / 1e6, 32.285107);
    EXPECT_DOUBLE_EQ(round(1e6 * tests[EQ_R2]) / 1e6, 0.821815);
    EXPECT_DOUBLE_EQ(round(1e6 * tests[EQ_R2ADJ]) / 1e6, 0.79636);
    EXPECT_DOUBLE_EQ(round(1e6 * tests[EQ_DW]) / 1e6, 2.33007);
    EXPECT_DOUBLE_EQ(round(1e6 * tests[EQ_LOGLIK]) / 1e6, 83.810104);

    double max_value;
    double min_value;

    // Observed values
    Variable obs = est.get_observed_values("ACAF");
    EXPECT_DOUBLE_EQ(round(1e6 * obs[0]) / 1e6, 0.011412);
    max_value = *std::max_element(obs.begin(), obs.end());
    EXPECT_DOUBLE_EQ(round(1e6 * max_value) / 1e6, 0.016028);
    min_value = *std::min_element(obs.begin(), obs.end());
    EXPECT_DOUBLE_EQ(round(1e6 * min_value) / 1e6, -0.002985);

    // Fitted values
    Variable fitted = est.get_fitted_values("ACAF");
    EXPECT_DOUBLE_EQ(round(1e6 * fitted[0]) / 1e6, 0.012562);
    max_value = *std::max_element(fitted.begin(), fitted.end());
    EXPECT_DOUBLE_EQ(round(1e6 * max_value) / 1e6, 0.012562);
    min_value = *std::min_element(fitted.begin(), fitted.end());
    EXPECT_DOUBLE_EQ(round(1e6 * min_value) / 1e6, -0.002985);

    // Residual values
    Variable residuals = est.get_residual_values("ACAF");
    EXPECT_DOUBLE_EQ(round(1e6 * residuals[0]) / 1e6, -0.00115);
    max_value = *std::max_element(residuals.begin(), residuals.end());
    EXPECT_DOUBLE_EQ(round(1e6 * max_value) / 1e6, 0.003537);
    min_value = *std::min_element(residuals.begin(), residuals.end());
    EXPECT_DOUBLE_EQ(round(1e6 * min_value) / 1e6, -0.004334);

    // Correlation matrix
    // -- ACAF

    CorrelationMatrix* m_corr2 = est.get_correlation_matrix();
    EXPECT_EQ(m_corr2->nb_coeffs, E_NCE);
    // -- line 0
    EXPECT_DOUBLE_EQ(m_corr2->get_value(0, 0), 1.);
    EXPECT_DOUBLE_EQ(round(1e6 * m_corr2->get_value(0, 1)) / 1e6, -0.936111);
    EXPECT_DOUBLE_EQ(round(1e6 * m_corr2->get_value(0, 2)) / 1e6, 0.20017);
    // -- line 1
    EXPECT_DOUBLE_EQ(round(1e6 * m_corr2->get_value(1, 0)) / 1e6, -0.936111);
    EXPECT_DOUBLE_EQ(m_corr2->get_value(1, 1), 1.);
    EXPECT_DOUBLE_EQ(round(1e6 * m_corr2->get_value(1, 2)) / 1e6, -0.300746);
    // -- line 2
    EXPECT_DOUBLE_EQ(round(1e6 * m_corr2->get_value(2, 0)) / 1e6, 0.20017);
    EXPECT_DOUBLE_EQ(round(1e6 * m_corr2->get_value(2, 1)) / 1e6, -0.300746);
    EXPECT_DOUBLE_EQ(m_corr2->get_value(2, 2), 1.);

    // -- DPUH
    est.set_block("DPUH");
    est.update_scalars();
    est.estimate();
    ASSERT_TRUE(est.is_estimation_done());

    est.copy_eq_tests_values();
    est.save();

    // number of coefficients 
    EXPECT_EQ(E_NCE, 2);

    // number of equations
    EXPECT_EQ(E_NEQ, 1);

    CorrelationMatrix* m_corr3 = est.get_correlation_matrix();
    EXPECT_EQ(m_corr3->nb_coeffs, E_NCE);
    // -- line 0
    EXPECT_DOUBLE_EQ(m_corr3->get_value(0, 0), 1.);
    EXPECT_DOUBLE_EQ(round(1e6 * m_corr3->get_value(0, 1)) / 1e6, -0.042291);
    // -- line 1
    EXPECT_DOUBLE_EQ(round(1e6 * m_corr3->get_value(1, 0)) / 1e6, -0.042291);
    EXPECT_DOUBLE_EQ(m_corr3->get_value(1, 1), 1.);

    // Error
    est.set_block("ACAF");
    est.update_scalars();
    Variables.clear();
    EXPECT_THROW(est.estimate(), std::runtime_error);
}

TEST_F(EstimationTest, DynamicAdjustment)
{
    std::string eqs = Equations.get_lec("ACAF");
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

    scl_order_0 = kdb_res->get("df_");
    EXPECT_DOUBLE_EQ(round(1e6 * scl_order_0->val) / 1e6, 0.051603);
    EXPECT_DOUBLE_EQ(round(1e6 * scl_order_0->relax) / 1e6, 0.053847);
    EXPECT_DOUBLE_EQ(round(1e6 * scl_order_0->std) / 1e6, 0.053847);

    delete kdb_res;

    // drift, no trend
    drift = true;
    trend = false;
    kdb_res = dickey_fuller_test(var_name, drift, trend, order);

    scl_order_0 = kdb_res->get("df_");
    EXPECT_DOUBLE_EQ(round(1e6 * scl_order_0->val) / 1e6, 0.05969);
    EXPECT_DOUBLE_EQ(round(1e6 * scl_order_0->relax) / 1e6, 0.053414);
    EXPECT_DOUBLE_EQ(round(1e6 * scl_order_0->std) / 1e6, 0.053414);

    scl_drift = kdb_res->get("df_d");
    EXPECT_DOUBLE_EQ(round(1e6 * scl_drift->val) / 1e6, -2.351039);
    EXPECT_DOUBLE_EQ(round(1e6 * scl_drift->relax) / 1e6, 1.);
    EXPECT_DOUBLE_EQ(round(1e6 * scl_drift->std) / 1e6, 1.588698);

    delete kdb_res;

    // drift, trend
    drift = true;
    trend = true;
    kdb_res = dickey_fuller_test(var_name, drift, trend, order);

    scl_order_0 = kdb_res->get("df_");
    EXPECT_DOUBLE_EQ(round(1e6 * scl_order_0->val) / 1e6, -0.055952);
    EXPECT_DOUBLE_EQ(round(1e6 * scl_order_0->relax) / 1e6, 0.070056);
    EXPECT_DOUBLE_EQ(round(1e6 * scl_order_0->std) / 1e6, 0.070056);

    scl_drift = kdb_res->get("df_d");
    EXPECT_DOUBLE_EQ(round(1e6 * scl_drift->val) / 1e6, 10.599557);
    EXPECT_DOUBLE_EQ(round(1e6 * scl_drift->relax) / 1e6, 1.);
    EXPECT_DOUBLE_EQ(round(1e6 * scl_drift->std) / 1e6, 5.620008);

    scl_trend = kdb_res->get("df_t");
    EXPECT_DOUBLE_EQ(round(1e6 * scl_trend->val) / 1e6, -0.381779);
    EXPECT_DOUBLE_EQ(round(1e6 * scl_trend->relax) / 1e6, 0.1596);
    EXPECT_DOUBLE_EQ(round(1e6 * scl_trend->std) / 1e6, 0.1596);

    delete kdb_res;
}
