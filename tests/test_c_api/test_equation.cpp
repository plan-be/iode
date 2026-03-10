#include "pch.h"


class EquationTest : public TestAbstract, public ::testing::Test
{
protected:
    std::shared_ptr<Equation> eq_ptr;
    std::string name = "ACAF";

    void SetUp() override
    {
        global_ws_eqs->load(str_input_test_dir + "fun.ae");
        eq_ptr = global_ws_eqs->get_obj_ptr(name);
    }

    void TearDown() override 
    {
        global_ws_eqs->clear();
        eq_ptr.reset();
    }
};


TEST_F(EquationTest, Endo)
{
    EXPECT_EQ(eq_ptr->endo, "ACAF");
}

TEST_F(EquationTest, Lec)
{
    // get
    EXPECT_EQ(eq_ptr->lec, "(ACAF/VAF[-1]) :=acaf1+acaf2*GOSF[-1]+\nacaf4*(TIME=1995)");

    // set
    std::string new_lec = "(ACAF/VAF[-1]) :=acaf2*GOSF[-1]+\nacaf4*(TIME=1995)";
    eq_ptr->set_lec(new_lec);
    EXPECT_EQ(eq_ptr->lec, new_lec);
}

TEST_F(EquationTest, SplitEquation)
{
    // expected left and right hand side
    std::string expected_lhs = "(ACAF/VAF[-1])";
    std::string expected_rhs = "acaf1+acaf2*GOSF[-1]+\nacaf4*(TIME=1995)";

    std::pair<std::string, std::string> lrhs = eq_ptr->split_equation();
    EXPECT_EQ(lrhs.first, expected_lhs);
    EXPECT_EQ(lrhs.second, expected_rhs);
}

TEST_F(EquationTest, Method)
{
    std::string method;
    std::string expected_method = v_eq_methods[0];

    // get
    EXPECT_EQ(eq_ptr->get_method(), expected_method);

    // set
    for (const std::string& method : v_eq_methods)
    {
        eq_ptr->set_method(method);
        EXPECT_EQ(eq_ptr->get_method(), method);
    }
}

TEST_F(EquationTest, Sample)
{
    // get
    Sample expected_sample("1980Y1", "1996Y1");
    Sample sample = eq_ptr->sample;
    EXPECT_EQ(sample.to_string(), expected_sample.to_string());

    // set
    std::string from = "2000Y1";
    std::string to = "2020Y1";
    Sample new_sample(from, to);
    eq_ptr->set_sample(from, to);
    Sample sample_res = eq_ptr->sample;
    EXPECT_EQ(sample_res.to_string(), new_sample.to_string());
}

TEST_F(EquationTest, Comment)
{
    // get
    EXPECT_EQ(eq_ptr->get_comment(), "");

    // set
    std::string new_comment = "New Comment";
    eq_ptr->set_comment(new_comment);
    EXPECT_EQ(eq_ptr->get_comment(), new_comment);
}

TEST_F(EquationTest, Date)
{
    // get
    EXPECT_EQ(eq_ptr->date, 19980612);
    EXPECT_EQ(eq_ptr->get_date_as_string(), "12-06-1998");

    // set
    int current_date = SCR_current_date();
    eq_ptr->update_date();
    EXPECT_EQ(eq_ptr->date, current_date);

    std::string s_date = "20-05-2010";
    eq_ptr->set_date(s_date);
    EXPECT_EQ(eq_ptr->date, 20100520);

    s_date = "07/08/2016";
    eq_ptr->set_date(s_date, "dd/mm/yyyy");
    EXPECT_EQ(eq_ptr->date, 20160807);
}

TEST_F(EquationTest, Tests)
{
    std::array<float, EQS_NBTESTS> expected_tests = { 0.f, 0.0042699f, 0.00818467f, 5.19945e-05f, 
                                                      0.0019271461f, 0.f, 32.2732f, 0.82176137f, 
                                                      0.79629868f, 2.3293459f, 83.8075f };
    std::map<std::string, float> m_tests;

    // get
    std::array<float, EQS_NBTESTS> tests = eq_ptr->tests;
    EXPECT_FLOAT_EQ(tests[EQ_CORR], expected_tests[EQ_CORR]);
    EXPECT_FLOAT_EQ(tests[EQ_STDEV], expected_tests[EQ_STDEV]);
    EXPECT_FLOAT_EQ(tests[EQ_MEANY], expected_tests[EQ_MEANY]);
    EXPECT_FLOAT_EQ(tests[EQ_SSRES], expected_tests[EQ_SSRES]);
    EXPECT_FLOAT_EQ(tests[EQ_STDERR], expected_tests[EQ_STDERR]);
    EXPECT_FLOAT_EQ(tests[EQ_STDERRP], expected_tests[EQ_STDERRP]);
    EXPECT_FLOAT_EQ(tests[EQ_FSTAT], expected_tests[EQ_FSTAT]);
    EXPECT_FLOAT_EQ(tests[EQ_R2], expected_tests[EQ_R2]);
    EXPECT_FLOAT_EQ(tests[EQ_R2ADJ], expected_tests[EQ_R2ADJ]);
    EXPECT_FLOAT_EQ(tests[EQ_DW], expected_tests[EQ_DW]);
    EXPECT_FLOAT_EQ(tests[EQ_LOGLIK], expected_tests[EQ_LOGLIK]);

    m_tests = eq_ptr->get_tests_as_map();
    EXPECT_FLOAT_EQ(m_tests["corr"], expected_tests[EQ_CORR]);
    EXPECT_FLOAT_EQ(m_tests["stdev"], expected_tests[EQ_STDEV]);
    EXPECT_FLOAT_EQ(m_tests["meany"], expected_tests[EQ_MEANY]);
    EXPECT_FLOAT_EQ(m_tests["ssres"], expected_tests[EQ_SSRES]);
    EXPECT_FLOAT_EQ(m_tests["stderr"], expected_tests[EQ_STDERR]);
    EXPECT_FLOAT_EQ(m_tests["stderrp"], expected_tests[EQ_STDERRP]);
    EXPECT_FLOAT_EQ(m_tests["fstat"], expected_tests[EQ_FSTAT]);
    EXPECT_FLOAT_EQ(m_tests["r2"], expected_tests[EQ_R2]);
    EXPECT_FLOAT_EQ(m_tests["r2adj"], expected_tests[EQ_R2ADJ]);
    EXPECT_FLOAT_EQ(m_tests["dw"], expected_tests[EQ_DW]);
    EXPECT_FLOAT_EQ(m_tests["loglik"], expected_tests[EQ_LOGLIK]);

    // set
    std::array<float, EQS_NBTESTS> new_tests = { 1., 2., 3., 4., 5., 6., 7., 8., 9., 10., 11. };
    for(int i = 0; i < EQS_NBTESTS; i++)
    {
        eq_ptr->set_test(static_cast<IodeEquationTest>(i), new_tests[i]);
        EXPECT_FLOAT_EQ(eq_ptr->tests[i], new_tests[i]);
    }
}

TEST_F(EquationTest, GetCoefficients)
{
    std::vector<std::string> expected_coefs_list = {"acaf1", "acaf2", "acaf4"};
    std::vector<std::string> coefs_list = eq_ptr->get_coefficients_list();
    EXPECT_EQ(coefs_list, expected_coefs_list);

    // check that coeffs have been created
    EXPECT_EQ(global_ws_scl->size(), coefs_list.size());
    EXPECT_TRUE(global_ws_scl->contains("acaf1"));
    EXPECT_TRUE(global_ws_scl->contains("acaf2"));
    EXPECT_TRUE(global_ws_scl->contains("acaf4"));
}

TEST_F(EquationTest, GetVariables)
{
    Sample eq_sample = eq_ptr->sample;
    global_ws_var->set_sample(eq_sample.start_period, eq_sample.end_period);

    std::vector<std::string> expected_vars_list = {name, "VAF", "GOSF", "TIME"};
    std::vector<std::string> vars_list = eq_ptr->get_variables_list();
    EXPECT_EQ(vars_list, expected_vars_list);

    // check that variables have been created
    EXPECT_EQ(global_ws_var->size(), vars_list.size());
    EXPECT_TRUE(global_ws_var->contains(name));
    EXPECT_TRUE(global_ws_var->contains("GOSF"));
    EXPECT_TRUE(global_ws_var->contains("TIME"));
    EXPECT_TRUE(global_ws_var->contains("VAF"));
}

TEST_F(EquationTest, Hash)
{
    std::hash<Equation> equation_hasher;
    std::size_t hash_before;
    std::size_t hash_after;

    hash_before = equation_hasher(*eq_ptr);

    // same equation
    Equation same_equation = global_ws_eqs->get(name);
    EXPECT_EQ(*eq_ptr, same_equation);
    hash_after = equation_hasher(same_equation);
    EXPECT_EQ(hash_before, hash_after);

    // different lec
    std::string new_lec = "(ACAF/VAF[-1]) :=acaf2*GOSF[-1]+\nacaf4*(TIME=1995)";
    eq_ptr->set_lec(new_lec); 
    hash_after = equation_hasher(*eq_ptr);
    EXPECT_NE(hash_before, hash_after);

    // different method
    hash_before = hash_after;
    std::string new_method = v_eq_methods[1];
    eq_ptr->set_method(new_method); 
    hash_after = equation_hasher(*eq_ptr);
    EXPECT_NE(hash_before, hash_after);

    // different sample
    hash_before = hash_after;
    std::string from = "2000Y1";
    std::string to = "2020Y1";
    eq_ptr->set_sample(from, to); 
    hash_after = equation_hasher(*eq_ptr);
    EXPECT_NE(hash_before, hash_after);

    // different comment
    hash_before = hash_after;
    std::string new_comment = "New Comment";
    eq_ptr->set_comment(new_comment); 
    hash_after = equation_hasher(*eq_ptr);
    EXPECT_NE(hash_before, hash_after);

    // different block
    hash_before = hash_after;
    std::string new_block = "ACAF;AGAF";
    eq_ptr->block = new_block; 
    hash_after = equation_hasher(*eq_ptr);
    EXPECT_NE(hash_before, hash_after);

    // different instrument
    hash_before = hash_after;
    std::string new_instruments = "random_text";
    eq_ptr->instruments = new_instruments; 
    hash_after = equation_hasher(*eq_ptr);
    EXPECT_NE(hash_before, hash_after);
}
