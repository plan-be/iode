#include "pch.h"


class IdentityTest : public KDBTest, public ::testing::Test
{
protected:
    Identity* idt_coeffs;
    Identity* idt_vars;
    KDBIdentities kdb;
    std::string name_idt_coeffs = "NAWRU";
    std::string name_idt_vars = "AOUC";

    void SetUp() override
    {
        load_global_kdb(I_EQUATIONS, input_test_dir + "fun.eqs");
        load_global_kdb(I_IDENTITIES, input_test_dir + "fun.idt");
    
        idt_coeffs = new Identity(name_idt_coeffs, nullptr);
        idt_vars = new Identity(name_idt_vars, nullptr);
    }

    void TearDown() override
    {
        delete idt_coeffs;
        delete idt_vars;
    }
};


TEST_F(IdentityTest, GetCoefficients)
{
    std::vector<std::string> expected_coefs_list = {"gamma2", "gamma3", "gamma4", "gamma"};
    std::vector<std::string> coefs_list = idt_coeffs->get_coefficients_list();
    EXPECT_EQ(coefs_list, expected_coefs_list);

    // check that coeffs have been created
    KDBScalars kdb_scl;
    EXPECT_EQ(kdb_scl.count(), coefs_list.size());
    EXPECT_TRUE(kdb_scl.contains("gamma2"));
    EXPECT_TRUE(kdb_scl.contains("gamma3"));
    EXPECT_TRUE(kdb_scl.contains("gamma4"));
    EXPECT_TRUE(kdb_scl.contains("gamma"));
}

TEST_F(IdentityTest, GetVariables)
{
    Equation eq("ACAF");
    Sample eq_sample = eq.get_sample();
    KDBVariables kdb_var;
    kdb_var.set_sample(eq_sample.start_period(), eq_sample.end_period());

    std::vector<std::string> expected_vars_list = {"WCRH", "QL", "VAFF", "VM", "PM"};
    std::vector<std::string> vars_list = idt_vars->get_variables_list();
    EXPECT_EQ(vars_list, expected_vars_list);

    // check that variables have been created
    EXPECT_EQ(kdb_var.count(), vars_list.size());
    EXPECT_TRUE(kdb_var.contains("PM"));
    EXPECT_TRUE(kdb_var.contains("QL"));
    EXPECT_TRUE(kdb_var.contains("VAFF"));
    EXPECT_TRUE(kdb_var.contains("VM"));
    EXPECT_TRUE(kdb_var.contains("WCRH"));
}