#include "pch.h"


class LecTest : public TestAbstract, public ::testing::Test 
{
protected:
    void SetUp() override
    {
        global_ws_var->load(str_input_test_dir + "a.var");
    }

    // void TearDown() override {}
};


TEST_F(LecTest, Tests_CLEC_Compile)
{
    print_test_title("Tests CLEC compile");

    std::string lec = "A + 1";
    std::shared_ptr<CLEC> clec = nullptr;
    
    create_dummy_lists_and_vars();

    Variable A = global_ws_var->get("A");
    Variable B = global_ws_var->get("B");

    clec = std::make_shared<CLEC>(lec);
    EXPECT_EQ(clec->duplicated_endo, false);
    EXPECT_EQ(clec->v_expression.size(), 3);
    EXPECT_EQ(clec->v_obj_names.size(), 1);
    EXPECT_TRUE(lec_contains(clec, "A"));
    
    // Using macros in LEC
    clec = std::make_shared<CLEC>("1 + vmax($LST1)");
    EXPECT_EQ(clec->v_obj_names.size(), 2);
    EXPECT_TRUE(lec_contains(clec, "A"));
    EXPECT_TRUE(lec_contains(clec, "B"));

    clec = std::make_shared<CLEC>("1 + vmax($LST2)");
    EXPECT_EQ(clec->v_obj_names.size(), 2);
    EXPECT_TRUE(lec_contains(clec, "A"));
    EXPECT_TRUE(lec_contains(clec, "B"));
}

TEST_F(LecTest, Tests_CLEC_Copy)
{
    print_test_title("Tests CLEC copy");

    std::string lec = "A + 1";
    std::shared_ptr<CLEC> clec = std::make_shared<CLEC>(lec);
    std::shared_ptr<CLEC> copy_clec = std::make_shared<CLEC>(*clec);

    EXPECT_TRUE(copy_clec != nullptr);
    EXPECT_EQ(copy_clec->duplicated_endo, clec->duplicated_endo);
    EXPECT_EQ(copy_clec->v_expression.size(), clec->v_expression.size());
    EXPECT_EQ(copy_clec->v_obj_names.size(), clec->v_obj_names.size());
    EXPECT_EQ(copy_clec->v_obj_names, clec->v_obj_names);
}

TEST_F(LecTest, Tests_LEC)
{
    int t = 2;
    double expected_value = 0.0;

    print_test_title("Tests LEC");

    // Create objects
    create_dummy_lists_and_vars();

    // A = 0 1 2 3 4  5  6  7  8  9 10
    // B = 0 2 4 6 8 10 12 14 16 18 20
    Variable A = global_ws_var->get("A");
    Variable B = global_ws_var->get("B");

    // Tests LEC
    
    // ---- test lag ----
    check_lec("A[2002Y1]",     t, A[2]);
    check_lec("A[2002Y1][-1]", t, A[2]);
    check_lec("A[-1]",         t, A[1]);
    check_lec("A[-1][2002Y1]", t, A[1]);
    
    // ---- test operators (LEC_OP) ----
    check_lec("A + B",  t, A[t] + B[t]);

    // ---- test no-time functions (LEC_FN) ----
    check_lec("ln A", t, log(A[t]));
    check_lec("ln B", t, log(B[t]));
    check_lec("ln A + ln B", t, log(A[t]) + log(B[t]));
    check_lec("ln (A + B)", t, log(A[t] + B[t]));
    check_lec("log(A)", t, log(A[t]));
    check_lec("log(A, 2)", t, log(A[t]) / log(2));
    check_lec("exp(A)", t, exp(A[t]));
    check_lec("exp(A, B)", t, pow(A[t], B[t]));
    check_lec("round(B / 3)", 5, round(B[5] / 3.0));
    check_lec("round(B / 3, 2)", 5, round(B[5] * 100.0 / 3.0) / 100.0);
    check_lec("if(t=2002Y1, A + B, A - B)", t, A[t] + B[t]);
    check_lec("if(t=2002Y1, A + B, A - B)", t-1, A[t-1] - B[t-1]);
    check_lec("div0(A, B)", t, A[t] / B[t]);
    check_lec("div0(A, B)", 0, 0.0);            // B[...] = 0.0
    
    // ---- test time functions (LEC_TFN) ----
    expected_value = 0.0;
    for(int k = 1; k < 11; k++)
        expected_value += A[k];
    check_lec("sum(2001Y1, 2010Y1, A)", t, expected_value);

    expected_value = 0.0;
    for(int k = 1; k <= t; k++)
        expected_value += A[k];
    check_lec("sum(2001Y1, A)", t, expected_value);

    expected_value = 0.0;
    for(int k = 0; k <= t; k++)
        expected_value += A[k];
    check_lec("sum(A)", t, expected_value);

    expected_value = 0.0;
    for(int k = 1; k < 11; k++)
        expected_value += A[k];
    for(int k = 2; k < 9; k++)
        expected_value += B[k];
    check_lec("sum(2001Y1, 2010Y1, A) + sum(2002Y1, 2008Y1, B)", t, expected_value);

    expected_value = (A[t] + A[t+1] + A[t+2]) / 3.0;
    check_lec("mean(t, t+2, A)", t, expected_value);

    expected_value = B[t] * (A[t] + A[t+1] + A[t+2]) / 3.0;
    check_lec("B * mean(t, t+2, A)", t, expected_value);

    expected_value = A[t] / A[t-1];
    check_lec("r(A)", t, expected_value);
    
    expected_value = B[t] / B[t-1];
    check_lec("r(B)", t, expected_value);
    
    // ---- test variadic time functions (LEC_MTFN) ----
    // covar0([from [,to],] X, Y) = sum(Xi * Yi) / n 
    expected_value = 0.0;
    for(int k = 0; k < 11; k++)
        expected_value += A[k] * B[k];
    expected_value /= 11.0;
    check_lec("covar0(2000Y1, 2010Y1, A, B)", t, expected_value);

    double covar0_1 = expected_value;
    double covar0_2 = 0.0;
    for(int k = 2; k < 9; k++)
        covar0_2 += A[k] * B[k];
    covar0_2 /= 7.0;
    expected_value = covar0_1 + covar0_2;
    check_lec("covar0(2000Y1, 2010Y1, A, B) + covar0(2002Y1, 2008Y1, A, B)", t, expected_value);

    // ---- test macros (LEC_MACRO) ----
    std::string lst = global_ws_lst->get("LST1");
    EXPECT_EQ(lst, "A,B");
    lst = global_ws_lst->get("LST2");
    EXPECT_EQ(lst, "A,B,A");

    double max_value = (A[t] > B[t]) ? A[t] : B[t];
    check_lec("1 + max($LST1)", t, 1.0 + max_value);
    check_lec("1 + max(60, $LST1)", t, 61.0);
    check_lec("1 + max($LST2)", t, 1.0 + max_value);
}

TEST_F(LecTest, LEC)
{
    Variable var_A = global_ws_var->get("A");

    std::shared_ptr<Sample> sample = global_ws_var->get_sample();
    EXPECT_TRUE(sample.get() != nullptr);

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
