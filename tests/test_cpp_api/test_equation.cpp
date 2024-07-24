#include "pch.h"


class EquationTest : public KDBTest, public ::testing::Test
{
protected:
    Equation* equation;
    KDBEquations* kdb_eqs;
    std::string name = "ACAF";

    void SetUp() override
    {
        kdb_eqs = new KDBEquations(input_test_dir + "fun.ae");
        equation = kdb_eqs->get(name);
    }

    void TearDown() override
    {
        delete equation;
        delete kdb_eqs;
    }
};


TEST_F(EquationTest, Equivalence_C_CPP)
{
    char* c_name = const_cast<char*>(name.c_str());
    std::string lec = equation->get_lec();
    std::string method = equation->get_method();
    Sample sample = equation->get_sample();
    std::string from = sample.start_period().to_string();
    std::string to = sample.end_period().to_string();
    std::string comment = equation->get_comment();
    std::string instruments = equation->get_instruments();
    std::string block = equation->get_block();

    kdb_eqs->remove(name);

    // test if a Equation object can be added to the Equations KDB via K_add()
    Equation eq(name, lec, method, from, to, comment, instruments, block, true);
    // NOTE: for equations --> K_add(KDB* kdb, char* name, EQ* eq, char* endo) [where endo = name]
    K_add(KE_WS, c_name, static_cast<EQ*>(&eq), c_name);
    int pos = K_find(KE_WS, c_name);
    ASSERT_GT(pos, -1);

    EQ* c_eq = KEVAL(KE_WS, pos);
    ASSERT_EQ(std::string(c_eq->endo), eq.get_endo());
    ASSERT_EQ(std::string(c_eq->lec), eq.get_lec());
    ASSERT_EQ((int) c_eq->method, eq.get_method_as_int());
    ASSERT_EQ(Sample(c_eq->smpl), eq.get_sample());
    ASSERT_EQ(std::string(c_eq->cmt), eq.get_comment());
    ASSERT_EQ(std::string(c_eq->instr), eq.get_instruments());
    ASSERT_EQ(std::string(c_eq->blk), eq.get_block());
    ASSERT_EQ(c_eq->date, eq.get_date());

    // test memcpy between a Equation object and a EQ object
    eq.set_lec("(ACAF/VAF[-1]) :=acaf2*GOSF[-1]+\nacaf4*(TIME=1995)");
    eq.set_sample("2000Y1", "2020Y1");
    eq.set_method("MAX_LIKELIHOOD");
    memcpy(c_eq, &eq, sizeof(EQ));
    ASSERT_EQ(std::string(c_eq->endo), eq.get_endo());
    ASSERT_EQ(std::string(c_eq->lec), eq.get_lec());
    ASSERT_EQ((int) c_eq->method, eq.get_method_as_int());
    ASSERT_EQ(Sample(c_eq->smpl), eq.get_sample());
    ASSERT_EQ(std::string(c_eq->cmt), eq.get_comment());
    ASSERT_EQ(std::string(c_eq->instr), eq.get_instruments());
    ASSERT_EQ(std::string(c_eq->blk), eq.get_block());
    ASSERT_EQ(c_eq->date, eq.get_date());

    // test if a Equation object can be passed to the hash function for the objects of type EQ.
    std::hash<EQ> eq_hasher;
    std::size_t c_hash = eq_hasher(*c_eq);
    std::size_t cpp_hash = eq_hasher(static_cast<EQ>(eq));
    ASSERT_EQ(c_hash, cpp_hash);
}

TEST_F(EquationTest, Endo)
{
    EXPECT_EQ(equation->get_endo(), "ACAF");
}

TEST_F(EquationTest, Lec)
{
    // get
    EXPECT_EQ(equation->get_lec(), "(ACAF/VAF[-1]) :=acaf1+acaf2*GOSF[-1]+\nacaf4*(TIME=1995)");

    // set
    std::string new_lec = "(ACAF/VAF[-1]) :=acaf2*GOSF[-1]+\nacaf4*(TIME=1995)";
    equation->set_lec(new_lec);
    EXPECT_EQ(equation->get_lec(), new_lec);
}

TEST_F(EquationTest, SplitEquation)
{
    // expected left and right hand side
    std::string expected_lhs = "(ACAF/VAF[-1])";
    std::string expected_rhs = "acaf1+acaf2*GOSF[-1]+\nacaf4*(TIME=1995)";

    std::pair<std::string, std::string> lrhs = equation->split_equation();
    EXPECT_EQ(lrhs.first, expected_lhs);
    EXPECT_EQ(lrhs.second, expected_rhs);
}

TEST_F(EquationTest, Method)
{
    std::string method;
    std::string expected_method = v_eq_methods[0];

    // get
    EXPECT_EQ(equation->get_method(), expected_method);

    // set
    for (const std::string& method : v_eq_methods)
    {
        equation->set_method(method);
        EXPECT_EQ(equation->get_method(), method);
    }
}

TEST_F(EquationTest, Block)
{
    // get
    EXPECT_EQ(equation->get_block(), name);

    // set
    std::string new_block = "ACAF;AGAF";
    equation->set_block(new_block);
    EXPECT_EQ(equation->get_block(), new_block);
}

TEST_F(EquationTest, Sample)
{
    // get
    Sample expected_sample("1980Y1", "1996Y1");
    Sample sample = equation->get_sample();
    EXPECT_EQ(sample.to_string(), expected_sample.to_string());

    // set
    std::string from = "2000Y1";
    std::string to = "2020Y1";
    Sample new_sample(from, to);
    equation->set_sample(from, to);
    Sample sample_res = equation->get_sample();
    EXPECT_EQ(sample_res.to_string(), new_sample.to_string());
}

TEST_F(EquationTest, Comment)
{
    // get
    EXPECT_EQ(equation->get_comment(), "");

    // set
    std::string new_comment = "New Comment";
    equation->set_comment(new_comment);
    EXPECT_EQ(equation->get_comment(), new_comment);
}

TEST_F(EquationTest, Instruments)
{
    // get
    EXPECT_EQ(equation->get_instruments(), "");

    // set
    // TODO : find a realistic value for new_instruments
    std::string new_instruments = "random_text";
    equation->set_instruments(new_instruments);
    EXPECT_EQ(equation->get_instruments(), new_instruments);
}

TEST_F(EquationTest, Date)
{
    // get
    EXPECT_EQ(equation->get_date(), 19980612);
    EXPECT_EQ(equation->get_date_as_string(), "12-06-1998");

    // set
    int current_date = SCR_current_date();
    equation->update_date();
    EXPECT_EQ(equation->get_date(), current_date);

    long date = 20240618;
    equation->set_date(date);
    EXPECT_EQ(equation->get_date(), date);

    std::string s_date = "20-05-2010";
    equation->set_date(s_date);
    EXPECT_EQ(equation->get_date(), 20100520);

    s_date = "07/08/2016";
    equation->set_date(s_date, "dd/mm/yyyy");
    EXPECT_EQ(equation->get_date(), 20160807);
}

TEST_F(EquationTest, Tests)
{
    std::array<float, EQS_NBTESTS> tests;
    std::array<float, EQS_NBTESTS> expected_tests = { 0.f, 0.0042699f, 0.00818467f, 5.19945e-05f, 
                                                      0.0019271461f, 0.f, 32.2732f, 0.82176137f, 
                                                      0.79629868f, 2.3293459f, 83.8075f };
    std::map<std::string, float> m_tests;

    // get
    tests = equation->get_tests();
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

    m_tests = equation->get_tests_as_map();
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
    equation->set_tests(new_tests);
    tests = equation->get_tests();
    for (int i = 0; i < EQS_NBTESTS; i++) EXPECT_FLOAT_EQ(tests[i], new_tests[i]);
}

TEST_F(EquationTest, GetCoefficients)
{
    std::vector<std::string> expected_coefs_list = {"acaf1", "acaf2", "acaf4"};
    std::vector<std::string> coefs_list = equation->get_coefficients_list();
    EXPECT_EQ(coefs_list, expected_coefs_list);

    // check that coeffs have been created
    KDBScalars kdb_scl;
    EXPECT_EQ(kdb_scl.count(), coefs_list.size());
    EXPECT_TRUE(kdb_scl.contains("acaf1"));
    EXPECT_TRUE(kdb_scl.contains("acaf2"));
    EXPECT_TRUE(kdb_scl.contains("acaf4"));
}

TEST_F(EquationTest, GetVariables)
{
    Sample eq_sample = equation->get_sample();
    KDBVariables kdb_var;
    kdb_var.set_sample(eq_sample.start_period(), eq_sample.end_period());

    std::vector<std::string> expected_vars_list = {name, "VAF", "GOSF", "TIME"};
    std::vector<std::string> vars_list = equation->get_variables_list();
    EXPECT_EQ(vars_list, expected_vars_list);

    // check that variables have been created
    EXPECT_EQ(kdb_var.count(), vars_list.size());
    EXPECT_TRUE(kdb_var.contains(name));
    EXPECT_TRUE(kdb_var.contains("GOSF"));
    EXPECT_TRUE(kdb_var.contains("TIME"));
    EXPECT_TRUE(kdb_var.contains("VAF"));
}

TEST_F(EquationTest, Hash)
{
    std::hash<EQ> equation_hasher;
    std::size_t hash_before;
    std::size_t hash_after;

    hash_before = equation_hasher(*equation);

    // same equation
    Equation* same_equation = Equations.get(name);
    EXPECT_EQ(*equation, *same_equation);
    hash_after = equation_hasher(*same_equation);
    EXPECT_EQ(hash_before, hash_after);
    delete same_equation;

    // different lec
    std::string new_lec = "(ACAF/VAF[-1]) :=acaf2*GOSF[-1]+\nacaf4*(TIME=1995)";
    equation->set_lec(new_lec); 
    hash_after = equation_hasher(*equation);
    EXPECT_NE(hash_before, hash_after);

    // different method
    hash_before = hash_after;
    std::string new_method = v_eq_methods[1];
    equation->set_method(new_method); 
    hash_after = equation_hasher(*equation);
    EXPECT_NE(hash_before, hash_after);

    // different sample
    hash_before = hash_after;
    std::string from = "2000Y1";
    std::string to = "2020Y1";
    equation->set_sample(from, to); 
    hash_after = equation_hasher(*equation);
    EXPECT_NE(hash_before, hash_after);

    // different comment
    hash_before = hash_after;
    std::string new_comment = "New Comment";
    equation->set_comment(new_comment); 
    hash_after = equation_hasher(*equation);
    EXPECT_NE(hash_before, hash_after);

    // different block
    hash_before = hash_after;
    std::string new_block = "ACAF;AGAF";
    equation->set_block(new_block); 
    hash_after = equation_hasher(*equation);
    EXPECT_NE(hash_before, hash_after);

    // different instrument
    hash_before = hash_after;
    std::string new_instruments = "random_text";
    equation->set_instruments(new_instruments); 
    hash_after = equation_hasher(*equation);
    EXPECT_NE(hash_before, hash_after);
}
