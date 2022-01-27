#include "pch.h"
#include "../../cpp_api/iode_cpp_api.h"

#include <filesystem>


// To get current path
// std::filesystem::path cwd = std::filesystem::current_path();
// std::string str_path = cwd.string();

std::string input_test_dir = "../../tests/data/";
std::string output_test_dir = "../../tests/output/";


class KDBVariablesTest : public ::testing::Test {
protected:
    KDBVariables kdb;
    int mode = 0;
    int pos = 0;
    int t = 10;
    int t_nan = 5;

    void SetUp() override
    {
        kdb.load(input_test_dir + "fun.var");
    }

    // void TearDown() override {}
};


TEST(KDBVariablesTest_, Load)
{
    KDBVariables kdb;
    kdb.load(input_test_dir + "fun.var");
    EXPECT_EQ(kdb.count(), 394);
}

TEST_F(KDBVariablesTest, Get)
{
    Variable variable;
    Variable expected_variable = 1.2130001;

    // by position
    variable = kdb.get(pos, t, mode);
    EXPECT_DOUBLE_EQ(expected_variable, variable);    
    variable = kdb.get(pos, t_nan, mode);
    EXPECT_FALSE(L_ISAN(variable));

    // by name
    std::string name = kdb.getName(pos);
    variable = kdb.get(name, t, mode);
    EXPECT_DOUBLE_EQ(expected_variable, variable);    
    variable = kdb.get(name, t_nan, mode);
    EXPECT_FALSE(L_ISAN(variable));
}

TEST_F(KDBVariablesTest, GetNbPeriods)
{
    int expected_nb_periods = 56;
    EXPECT_EQ(kdb.getNbPeriods(), expected_nb_periods);
}

TEST_F(KDBVariablesTest, GetPeriod)
{
    Period expected_period(1970, 'Y', 1);
    EXPECT_EQ(kdb.getPeriod(t), expected_period);
}