#include "pch.h"


class EquationTest : public KDBTest, public ::testing::Test
{
protected:
    Equation* equation;
    KDBEquations kdb;

    void SetUp() override
    {
        load_global_kdb(I_EQUATIONS, input_test_dir + "fun.eqs");
        equation = new Equation("ACAF");
    }

    void TearDown() override
    {
        delete equation;
    }
};


TEST_F(EquationTest, Lec)
{
    // get
    EXPECT_EQ(equation->get_lec(), "(ACAF/VAF[-1]) :=acaf1+acaf2*GOSF[-1]+\nacaf4*(TIME=1995)");

    // set
    std::string new_lec = "(ACAF/VAF[-1]) :=acaf2*GOSF[-1]+\nacaf4*(TIME=1995)";
    equation->set_lec(new_lec, "ACAF");
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
    std::string expected_method = vEquationMethods[0];

    // get
    EXPECT_EQ(equation->get_method(), expected_method);

    // set
    for (const std::string& method : vEquationMethods)
    {
        equation->set_method(method);
        EXPECT_EQ(equation->get_method(), method);
    }
}

TEST_F(EquationTest, Block)
{
    // get
    EXPECT_EQ(equation->get_block(), "ACAF");

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
}

TEST_F(EquationTest, Tests)
{
    std::array<float, EQS_NBTESTS> tests;
    std::array<float, EQS_NBTESTS> expected_tests = { 1, 0.0042699, 0.00818467, 5.19945e-05, 0.0019271461, 23.545813, 32.2732, 0.82176137, 0.79629868, 2.3293459, 83.8075 };

    // get
    tests = equation->get_tests();
    for (int i = 0; i < EQS_NBTESTS; i++) EXPECT_FLOAT_EQ(tests[i], expected_tests[i]);

    // set
    std::array<float, EQS_NBTESTS> new_tests = { 1., 2., 3., 4., 5., 6., 7., 8., 9., 10., 11. };
    equation->set_tests(new_tests);
    tests = equation->get_tests();
    for (int i = 0; i < EQS_NBTESTS; i++) EXPECT_FLOAT_EQ(tests[i], new_tests[i]);
}

TEST_F(EquationTest, GetCoefficients)
{
    std::vector<std::string> expected_coefs_list = {"acaf1", "acaf2", "acaf4"};
    std::vector<std::string> coefs_list = equation->get_coefficients_list("ACAF");
    EXPECT_EQ(coefs_list, expected_coefs_list);
}
