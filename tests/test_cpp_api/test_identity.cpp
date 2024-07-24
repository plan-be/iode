#include "pch.h"


class IdentityTest : public KDBTest, public ::testing::Test
{
protected:
    Identity* idt_coeffs;
    Identity* idt_vars;
    std::string name_idt_coeffs = "NAWRU";
    std::string name_idt_vars = "AOUC";

    void SetUp() override
    {
        KDBEquations kdb_eqs(input_test_dir + "fun.ae");
        KDBIdentities kdb_idt(input_test_dir + "fun.ai");
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
    Identity idt_coeffs = Identities.get(name_idt_coeffs);

    std::vector<std::string> expected_coefs_list = {"gamma2", "gamma3", "gamma4", "gamma_"};
    std::vector<std::string> coefs_list = idt_coeffs.get_coefficients_list();
    EXPECT_EQ(coefs_list, expected_coefs_list);

    // check that coeffs have been created
    EXPECT_EQ(Scalars.count(), coefs_list.size());
    EXPECT_TRUE(Scalars.contains("gamma2"));
    EXPECT_TRUE(Scalars.contains("gamma3"));
    EXPECT_TRUE(Scalars.contains("gamma4"));
    EXPECT_TRUE(Scalars.contains("gamma_"));
}

TEST_F(IdentityTest, GetVariables)
{
    Identity idt_vars = Identities.get(name_idt_vars);

    Equation eq = Equations.get("ACAF");
    Sample eq_sample = eq.get_sample();
    Variables.set_sample(eq_sample.start_period(), eq_sample.end_period());

    std::vector<std::string> expected_vars_list = {"WCRH", "QL", "VAFF", "VM", "PM"};
    std::vector<std::string> vars_list = idt_vars.get_variables_list();
    EXPECT_EQ(vars_list, expected_vars_list);

    // check that variables have been created
    EXPECT_EQ(Variables.count(), vars_list.size());
    EXPECT_TRUE(Variables.contains("PM"));
    EXPECT_TRUE(Variables.contains("QL"));
    EXPECT_TRUE(Variables.contains("VAFF"));
    EXPECT_TRUE(Variables.contains("VM"));
    EXPECT_TRUE(Variables.contains("WCRH"));
}