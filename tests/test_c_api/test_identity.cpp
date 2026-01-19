#include "pch.h"


class IdentityTest : public TestAbstract, public ::testing::Test
{
protected:
    Identity* idt_coeffs;
    Identity* idt_vars;
    std::string name_idt_coeffs = "NAWRU";
    std::string name_idt_vars = "AOUC";

    void SetUp() override
    {
        global_ws_eqs->load(str_input_test_dir + "fun.ae");
        global_ws_idt->load(str_input_test_dir + "fun.ai");
    }

    // void TearDown() override {}
};


TEST_F(IdentityTest, CreateAndCopy)
{
    std::string lec = "1 - exp((gamma2 + gamma3 * ln(W/ZJ)[-1] + gamma4 * ln(WMIN/ZJ))/gamma_)";
    
    Identity idt(lec);
    Identity idt_copy(idt);
    ASSERT_EQ(idt.get_lec(), idt_copy.get_lec());
}


TEST_F(IdentityTest, GetCoefficients)
{
    Identity* idt_coeffs = global_ws_idt->get(name_idt_coeffs);

    std::vector<std::string> expected_coefs_list = {"gamma2", "gamma3", "gamma4", "gamma_"};
    std::vector<std::string> coefs_list = idt_coeffs->get_coefficients_list();
    EXPECT_EQ(coefs_list, expected_coefs_list);

    // check that coeffs have been created
    EXPECT_EQ(global_ws_scl->size(), coefs_list.size());
    EXPECT_TRUE(global_ws_scl->contains("gamma2"));
    EXPECT_TRUE(global_ws_scl->contains("gamma3"));
    EXPECT_TRUE(global_ws_scl->contains("gamma4"));
    EXPECT_TRUE(global_ws_scl->contains("gamma_"));

    delete idt_coeffs; // clean up
}

TEST_F(IdentityTest, GetVariables)
{
    Identity* idt_vars = global_ws_idt->get(name_idt_vars);

    Equation* eq = global_ws_eqs->get("ACAF");
    Sample eq_sample = eq->sample;
    global_ws_var->set_sample(eq_sample.start_period, eq_sample.end_period);

    std::vector<std::string> expected_vars_list = {"WCRH", "QL", "VAFF", "VM", "PM"};
    std::vector<std::string> vars_list = idt_vars->get_variables_list();
    EXPECT_EQ(vars_list, expected_vars_list);

    // check that variables have been created
    EXPECT_EQ(global_ws_var->size(), vars_list.size());
    EXPECT_TRUE(global_ws_var->contains("PM"));
    EXPECT_TRUE(global_ws_var->contains("QL"));
    EXPECT_TRUE(global_ws_var->contains("VAFF"));
    EXPECT_TRUE(global_ws_var->contains("VM"));
    EXPECT_TRUE(global_ws_var->contains("WCRH"));

    delete idt_vars; // clean up
}
